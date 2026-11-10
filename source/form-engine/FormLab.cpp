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

namespace {
bool writeJson(const QString &path,const QJsonObject &object){QSaveFile file(path);const auto bytes=QJsonDocument(object).toJson();return file.open(QIODevice::WriteOnly) && file.write(bytes)==bytes.size() && file.commit();}
}
FormLabController::FormLabController(QObject *parent):QObject(parent){
    m_config=AutoForm::defaultConfig(&m_message);m_reference=m_config;
    m_options={{"days",150},{"count",100},{"repeats",1},{"threads",qBound(1,QThread::idealThreadCount(),8)},{"seed",QString("1")},{"form",50},{"perk",0},{"pack",0},{"usePack",false},{"compare",false}};
    SimulationV2::ProfilePackRepository repository(SimulationV2::ProfilePackKind::Athletes,SimulationV2::CatalogPaths::defaults().athletes);m_packs=repository.load().packs;
    m_progressTimer.setInterval(100);connect(&m_progressTimer,&QTimer::timeout,this,&FormLabController::progressChanged);
    connect(&m_watcher,&QFutureWatcher<QJsonObject>::finished,this,[this]{
        m_progressTimer.stop();m_busy=false;const auto result=m_watcher.result();
        if(result.contains("error"))m_message=result["error"].toString();
        else if(result["cancelled"].toBool())m_message=tr("Przerwano próbę. Poprzednie wyniki zachowano.");
        else {m_result=result;m_stale=false;m_message=tr("Próba zakończona. Wykresy pokazują pierwsze powtórzenie; statystyki obejmują cały batch.");emit resultChanged();}
        emit progressChanged();emit changed();
    });
}
FormLabController::~FormLabController(){disconnect(&m_watcher,nullptr,this,nullptr);cancel();m_watcher.waitForFinished();}
QStringList FormLabController::packs()const{QStringList result;for(const auto &p:m_packs)result.append(p["name"].toString());return result;}
double FormLabController::progress()const{return m_total>0 && m_control?double(m_control->completed.load())/m_total:0;}
QString FormLabController::progressText()const{return tr("%1 / %2 osobodni").arg(m_control?m_control->completed.load():0).arg(m_total);}
void FormLabController::edited(){m_stale=!m_result.isEmpty();emit changed();}
void FormLabController::setParameter(const QString &key,double value){
    if(m_busy)return;auto json=m_config.toJson();auto values=json["values"].toObject();if(!values.contains(key))return;values[key]=value;json["values"]=values;
    AutoForm::Config candidate;if(AutoForm::Config::fromJson(json,candidate,m_message)){m_config=candidate;edited();}else emit changed();
}
void FormLabController::setOption(const QString &key,const QVariant &value){if(m_busy || !m_options.contains(key))return;m_options[key]=value;edited();}
void FormLabController::freezeReference(){if(m_busy)return;m_reference=m_config;m_message=tr("Zachowano konfigurację A. Kolejne zmiany dotyczą B.");edited();}
void FormLabController::reset(){if(m_busy)return;m_config=AutoForm::Config();m_message=tr("Przywrócono robocze parametry początkowe.");edited();}
void FormLabController::start(){
    if(m_busy)return;
    AutoForm::Experiment e;e.config=m_config;e.reference=m_reference;e.compare=m_options["compare"].toBool();e.days=m_options["days"].toInt();e.repeats=m_options["repeats"].toInt();e.threads=m_options["threads"].toInt();bool seedOk=false;e.seed=m_options["seed"].toString().toULongLong(&seedOk);
    if(!seedOk){m_message=tr("Seed musi być nieujemną liczbą całkowitą do 18446744073709551615.");emit changed();return;}
    if(m_options["usePack"].toBool()){
        const int pack=m_options["pack"].toInt();if(pack<0 || pack>=m_packs.size()){m_message=tr("Wybierz paczkę skoczków.");emit changed();return;}
        for(const auto &entry:m_packs[pack]["profiles"].toArray()){AthleteProfile p;QString error;if(!AthleteProfile::fromJson(entry.toObject(),p,error)){m_message=error;emit changed();return;}e.athletes.append({p.name+" "+p.surname,double(p.form),p.perks.size()>3?p.perks[3]:0});}
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
