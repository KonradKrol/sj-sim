#include "FormLab.h"
#include "../simulation-v2/SimulationCatalog.h"
#include <QtConcurrent>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QSaveFile>
#include <QThread>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QSignalBlocker>
#include <QDir>
#include <QUrl>
#include <cmath>
#include "../global/CountryCodes.h"

namespace {
enum AthleteRole {Id=Qt::UserRole+1,Name,Country,Chosen,InitialForm,FormPerk,Search,Flag};
bool writeJson(const QString &path,const QJsonObject &object){QSaveFile file(path);const auto bytes=QJsonDocument(object).toJson();return file.open(QIODevice::WriteOnly) && file.write(bytes)==bytes.size() && file.commit();}
}
FormLabController::FormLabController(QObject *parent):QObject(parent){
    m_config=AutoForm::defaultConfig(&m_message);m_reference=m_config;
    m_options={{"days",150},{"count",100},{"repeats",1},{"threads",qBound(1,QThread::idealThreadCount(),8)},{"seed",QString("1")},{"form",50},{"perk",0},{"pack",0},{"usePack",false},{"compare",false}};
    SimulationV2::ProfilePackRepository repository(SimulationV2::ProfilePackKind::Athletes,SimulationV2::CatalogPaths::defaults().athletes);m_packs=repository.load().packs;
    m_athletes=new QStandardItemModel(this);m_filteredAthletes=new QSortFilterProxyModel(this);
    m_filteredAthletes->setFilterRole(Search);m_filteredAthletes->setFilterCaseSensitivity(Qt::CaseInsensitive);rebuildAthletes();
    m_progressTimer.setInterval(100);connect(&m_progressTimer,&QTimer::timeout,this,&FormLabController::progressChanged);
    connect(&m_watcher,&QFutureWatcher<QJsonObject>::finished,this,[this]{
        m_progressTimer.stop();m_busy=false;auto result=m_watcher.result();
        if(result.contains("error"))m_message=result["error"].toString();
        else if(result["cancelled"].toBool())m_message=tr("Przerwano próbę. Poprzednie wyniki zachowano.");
        else {
            m_traces=result.take("traces").toArray();m_plotNames.clear();m_plottedIndexes.clear();
            for(const auto &trace:(m_traces.isEmpty()?QJsonArray():m_traces.at(0).toArray()))m_plotNames.append(trace.toObject()["name"].toString());
            for(int i=0;i<qMin(10,m_plotNames.size());++i)m_plottedIndexes.append(i);
            m_result=result;emit plotNamesChanged();emit plotsChanged();m_stale=false;m_message=tr("Próba zakończona. Wykresy pokazują pierwsze powtórzenie; statystyki obejmują cały batch.");emit resultChanged();}
        emit progressChanged();emit changed();
    });
}
void FormLabController::setPlottedAthletes(const QVariantList &values){
    if(m_busy)return;
    QVector<int> selected;QVariantList valid;
    for(const auto &value:values){bool ok=false;const int index=value.toInt(&ok);
        if(ok && index>=0 && index<m_plotNames.size() && !selected.contains(index) && selected.size()<10){selected.append(index);valid.append(index);}}
    if(valid==m_plottedIndexes)return;
    auto views=m_result["views"].toArray();
    for(int v=0;v<views.size() && v<m_traces.size();++v){
        auto view=views[v].toObject();const auto plots=AutoForm::plotTraces(m_traces[v].toArray(),selected);
        for(auto it=plots.begin();it!=plots.end();++it)view[it.key()]=it.value();
        views[v]=view;
    }
    m_plottedIndexes=valid;m_result["views"]=views;m_result["plottedAthletes"]=QJsonArray::fromVariantList(valid);
    emit plotsChanged();emit resultChanged();
}
FormLabController::~FormLabController(){disconnect(&m_watcher,nullptr,this,nullptr);cancel();m_watcher.waitForFinished();}
QStringList FormLabController::packs()const{QStringList result;for(const auto &p:m_packs)result.append(p["name"].toString());return result;}
QObject *FormLabController::athleteRows()const{return m_filteredAthletes;}
int FormLabController::athleteCount()const{return m_athletes->rowCount();}
QString FormLabController::athleteKey(const QString &id)const{return m_packs.value(m_options["pack"].toInt())["id"].toString()+"/"+id;}
void FormLabController::rebuildAthletes(){
    m_filteredAthletes->setSourceModel(nullptr);
    m_athletes->clear();m_athleteRows.clear();m_selectedAthletes=0;
    m_athletes->setItemRoleNames({{Id,"athleteId"},{Name,"athleteName"},{Country,"country"},{Chosen,"chosen"},{InitialForm,"initialForm"},{FormPerk,"formPerk"},{Search,"searchText"},{Flag,"flag"}});
    const auto entries=m_packs.value(m_options["pack"].toInt())["profiles"].toArray();
    for(const auto &entry:entries){
        const auto p=entry.toObject();const auto id=p["id"].toString(),name=p["name"].toString()+" "+p["surname"].toString(),country=p["country"].toString();
        const auto draft=m_athleteOverrides.value(athleteKey(id));auto *item=new QStandardItem;
        item->setData(id,Id);item->setData(name,Name);item->setData(country,Country);item->setData(name+" "+country,Search);
        item->setData(draft.value("selected",true),Chosen);item->setData(draft.value("form",p["form"].toInt()),InitialForm);
        const auto perks=p["perks"].toArray();item->setData(draft.value("perk",perks.size()>3?perks[3].toInt():0),FormPerk);
        item->setData(QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(country)+".svg")).toString(),Flag);
        m_selectedAthletes+=item->data(Chosen).toBool();m_athleteRows.insert(id,m_athletes->rowCount());m_athletes->appendRow(item);
    }
    m_filteredAthletes->setSourceModel(m_athletes);emit selectionChanged();
}
void FormLabController::setAthlete(const QString &id,const QString &key,const QVariant &value){
    if(m_busy || !m_athleteRows.contains(id))return;
    const int role=key=="selected"?Chosen:key=="form"?InitialForm:key=="perk"?FormPerk:0;if(!role)return;
    QVariant checked;
    if(role==Chosen){if(value.type()!=QVariant::Bool)return;checked=value.toBool();}
    else {bool ok=false;const double n=value.toDouble(&ok);if(!ok || !std::isfinite(n) || n!=std::floor(n) || (role==InitialForm?(n<1 || n>100):(n< -4 || n>4)))return;checked=int(n);}
    const auto index=m_athletes->index(m_athleteRows.value(id),0);const auto old=m_athletes->data(index,role);if(old==checked)return;
    m_options["usePack"]=true;m_athleteOverrides[athleteKey(id)][key]=checked;m_athletes->setData(index,checked,role);
    if(role==Chosen){m_selectedAthletes+=checked.toBool()?1:-1;emit selectionChanged();}m_message.clear();edited();
}
void FormLabController::selectAthletes(bool selected){
    if(m_busy)return;
    m_options["usePack"]=true;
    {QSignalBlocker block(m_athletes);for(int row=0;row<m_athletes->rowCount();++row){const auto index=m_athletes->index(row,0);m_athleteOverrides[athleteKey(index.data(Id).toString())]["selected"]=selected;m_athletes->setData(index,selected,Chosen);}}
    m_selectedAthletes=selected?m_athletes->rowCount():0;
    if(m_athletes->rowCount())emit m_athletes->dataChanged(m_athletes->index(0,0),m_athletes->index(m_athletes->rowCount()-1,0),{Chosen});
    m_message.clear();emit selectionChanged();edited();
}
void FormLabController::resetAthlete(const QString &id){
    if(m_busy || !m_athleteRows.contains(id))return;
    const int row=m_athleteRows.value(id);const auto p=m_packs.value(m_options["pack"].toInt())["profiles"].toArray()[row].toObject();const auto perks=p["perks"].toArray();
    setAthlete(id,"form",p["form"].toInt());setAthlete(id,"perk",perks.size()>3?perks[3].toInt():0);
}
void FormLabController::filterAthletes(const QString &text){m_filteredAthletes->setFilterFixedString(text.trimmed());}
double FormLabController::progress()const{return m_total>0 && m_control?double(m_control->completed.load())/m_total:0;}
QString FormLabController::progressText()const{return tr("%1 / %2 osobodni").arg(m_control?m_control->completed.load():0).arg(m_total);}
void FormLabController::edited(){m_stale=!m_result.isEmpty();emit changed();}
void FormLabController::setParameter(const QString &key,double value){
    if(m_busy)return;auto json=m_config.toJson();auto values=json["values"].toObject();if(!values.contains(key))return;values[key]=value;json["values"]=values;
    AutoForm::Config candidate;if(AutoForm::Config::fromJson(json,candidate,m_message)){m_config=candidate;edited();}else emit changed();
}
void FormLabController::setOption(const QString &key,const QVariant &value){if(m_busy || !m_options.contains(key))return;if(key=="pack"){bool ok=false;const int index=value.toInt(&ok);if(!ok || index<0 || index>=m_packs.size())return;if(m_options[key].toInt()==index)return;m_options[key]=index;rebuildAthletes();}else m_options[key]=value;edited();}
void FormLabController::freezeReference(){if(m_busy)return;m_reference=m_config;m_message=tr("Zachowano konfigurację A. Kolejne zmiany dotyczą B.");edited();}
void FormLabController::reset(){if(m_busy)return;m_config=AutoForm::Config();m_message=tr("Przywrócono robocze parametry początkowe.");edited();}
void FormLabController::start(){
    if(m_busy)return;
    AutoForm::Experiment e;e.config=m_config;e.reference=m_reference;e.compare=m_options["compare"].toBool();e.days=m_options["days"].toInt();e.repeats=m_options["repeats"].toInt();e.threads=m_options["threads"].toInt();bool seedOk=false;e.seed=m_options["seed"].toString().toULongLong(&seedOk);
    if(!seedOk){m_message=tr("Seed musi być nieujemną liczbą całkowitą do 18446744073709551615.");emit changed();return;}
    if(m_options["usePack"].toBool()){
        const int pack=m_options["pack"].toInt();if(pack<0 || pack>=m_packs.size()){m_message=tr("Wybierz paczkę skoczków.");emit changed();return;}
        for(int row=0;row<m_athletes->rowCount();++row){const auto index=m_athletes->index(row,0);if(index.data(Chosen).toBool())e.athletes.append({index.data(Name).toString(),index.data(InitialForm).toDouble(),index.data(FormPerk).toInt()});}
        if(e.athletes.isEmpty()){m_message=tr("Wybierz co najmniej jednego zawodnika.");emit changed();return;}
    }else{
        const int count=m_options["count"].toInt();if(count>0 && count<=10000)for(int i=0;i<count;++i)e.athletes.append({tr("Zawodnik %1").arg(i+1),m_options["form"].toDouble(),m_options["perk"].toInt()});
    }
    if(!e.validate(m_message)){emit changed();return;}
    m_total=qint64(e.days)*e.repeats*e.athletes.size()*(e.compare?2:1);m_control=std::make_shared<AutoForm::Control>();m_busy=true;m_message.clear();m_progressTimer.start();
    const auto control=m_control;m_watcher.setFuture(QtConcurrent::run([e,control]{try{return AutoForm::simulate(e,control);}catch(const std::exception &error){return QJsonObject{{"error",QString::fromUtf8(error.what())}};}}));emit changed();emit progressChanged();
}
void FormLabController::cancel(){if(m_control)m_control->cancelled.store(true);}
void FormLabController::saveConfig(){if(m_busy)return;const auto path=QFileDialog::getSaveFileName(qobject_cast<QWidget *>(parent()),tr("Zapis konfiguracji formy"),{},"JSON (*.json)");if(path.isEmpty())return;m_message=writeJson(path,m_config.toJson())?tr("Zapisano konfigurację."):tr("Nie udało się zapisać konfiguracji.");emit changed();}
void FormLabController::loadConfig(){
    if(m_busy)return;const auto path=QFileDialog::getOpenFileName(qobject_cast<QWidget *>(parent()),tr("Wczytaj konfigurację formy"),{},"JSON (*.json)");if(path.isEmpty())return;
    QFile file(path);AutoForm::Config candidate;
    if(!file.open(QIODevice::ReadOnly) || file.size()>1024*1024){m_message=tr("Nie udało się odczytać konfiguracji.");emit changed();return;}
    if(AutoForm::Config::fromJson(QJsonDocument::fromJson(file.readAll()).object(),candidate,m_message)){m_config=candidate;m_message=tr("Wczytano konfigurację.");edited();}else emit changed();
}
void FormLabController::useForNewGames(){if(m_busy)return;if(AutoForm::saveDefaultConfig(m_config,m_message))m_message=tr("Nowe rozgrywki będą korzystać z tej konfiguracji. Rozpoczęte zapisy zachowują własną.");emit changed();}
void FormLabController::exportResult(){if(m_result.isEmpty())return;const auto path=QFileDialog::getSaveFileName(qobject_cast<QWidget *>(parent()),tr("Eksport wyników formy"),{},"JSON (*.json)");if(path.isEmpty())return;m_message=writeJson(path,m_result)?tr("Zapisano wyniki i parametry próby."):tr("Nie udało się zapisać wyników.");emit changed();}
FormLabWindow::FormLabWindow(QWidget *parent):QDialog(parent){setObjectName("formEngineLab");setWindowTitle(tr("Laboratorium formy"));resize(1300,850);auto *controller=new FormLabController(this);setProperty("formLabController",QVariant::fromValue<QObject *>(controller));auto *layout=new QVBoxLayout(this);layout->addWidget(new QLabel(tr("Laboratorium formy — wykresy są dostępne w Qt Quick."),this));}
void FormLabWindow::reject(){if(auto *controller=findChild<FormLabController *>())controller->cancel();QDialog::reject();}
