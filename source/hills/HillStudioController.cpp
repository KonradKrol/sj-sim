#include "HillStudioController.h"
#include "../simulation-v2/ProfilePackRepository.h"
#include "../global/GlobalDatabase.h"
#include "../global/CountryCodes.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>
#include <QWidget>
#include <QUuid>
#include <QSet>
#include <QTimer>
#include <QUrl>

namespace {
QString uuid(){return QUuid::createUuid().toString(QUuid::WithoutBraces);}
QJsonObject emptyPack(const QString &name){return {{"format","sj-sim-hill-pack"},{"version",1},{"id",uuid()},{"name",name},{"profiles",QJsonArray()}};}
bool writeJson(const QString &file,const QJsonObject &pack){QSaveFile out(file);const auto bytes=QJsonDocument(pack).toJson(QJsonDocument::Indented);return bytes.size()<=20*1024*1024 && out.open(QIODevice::WriteOnly) && out.write(bytes)==bytes.size() && out.commit();}
QString title(const QJsonObject &p){return p["name"].toString().trimmed().isEmpty()?QObject::tr("Nowa skocznia"):p["name"].toString();}
}
HillStudioController::HillStudioController(QObject *parent,const QString &root):QObject(parent),m_root(root.isEmpty()?QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/hill-studio":root){
    connect(this,&HillStudioController::changed,this,&HillStudioController::perksChanged);
    connect(this,&HillStudioController::changed,this,&HillStudioController::historyChanged);
    connect(this,&HillStudioController::changed,this,&HillStudioController::statusChanged);
    const auto loaded=SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Hills,m_root).load();
    m_packs=loaded.packs;m_message=loaded.issues.join('\n');
    if(m_packs.isEmpty())m_packs.append(emptyPack(tr("Moje skocznie")));
    if(!currentPack()["profiles"].toArray().isEmpty())m_selected=0;
}
QJsonObject HillStudioController::currentPack()const{return m_packs.value(m_pack);}
QString HillStudioController::packName()const{return currentPack()["name"].toString();}
QVariantList HillStudioController::selectedPerks()const {const auto entries=currentPack()["profiles"].toArray();return m_selected>=0 && m_selected<entries.size()?entries[m_selected].toObject()["perks"].toArray().toVariantList():QVariantList();}
double HillStudioController::realHs()const {const auto entries=currentPack()["profiles"].toArray();HillProfile hill;QString error;return m_selected>=0 && m_selected<entries.size() && HillProfile::fromJson(entries[m_selected].toObject(),hill,error)?hill.realHs():0;}
void HillStudioController::status(const QString &value){m_message=value;notifyChanged();}
bool HillStudioController::validatePack(const QJsonObject &pack,QString &error)const{
    return SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Hills,m_root).validate(pack,error);
}
bool HillStudioController::persist(const QJsonObject &pack,QString &error,bool validate)const{
    return SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Hills,m_root).save(pack,error,validate);
}
bool HillStudioController::commit(const QJsonObject &pack,int selection,const QString &message,bool perksOnly){
    if(pack==currentPack()){m_selected=selection;return true;}
    QString error;if(!persist(pack,error,!perksOnly)){status(error);return false;}
    m_undo.append(QJsonObject{{"pack",currentPack()},{"selection",m_selected}});if(m_undo.size()>100)m_undo.removeFirst();m_redo.clear();m_packs[m_pack]=pack;m_selected=selection;m_message=message.isEmpty()?tr("Zapisano"):message;if(perksOnly){emit perksChanged();emit historyChanged();emit statusChanged();}else notifyChanged();return true;
}
QVariantList HillStudioController::packs()const{QVariantList rows;for(const auto &p:m_packs)rows.append(QVariantMap{{"name",p["name"].toString()},{"count",p["profiles"].toArray().size()}});return rows;}
QVariantList HillStudioController::profiles()const{QVariantList rows;const auto values=currentPack()["profiles"].toArray();for(int i=0;i<values.size();++i){auto p=values[i].toObject().toVariantMap();p["label"]=title(values[i].toObject());p["index"]=i;p["flag"]=QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(p["country"].toString())+".svg")).toString();rows.append(p);}return rows;}
QVariantMap HillStudioController::profile()const{const auto values=currentPack()["profiles"].toArray();if(m_selected<0 || m_selected>=values.size())return {};auto object=values[m_selected].toObject();auto p=object.toVariantMap();HillProfile hill;QString error;if(!HillProfile::fromJson(object,hill,error))return {};p["label"]=title(object);p["realHs"]=hill.realHs();p["takeoffBias"]=hill.takeoffBias();p["windVariability"]=hill.windVariability();p["flag"]=QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(hill.country)+".svg")).toString();return p;}
QVariantList HillStudioController::countries()const{if(!m_countryOptions.isEmpty())return m_countryOptions;QVariantList rows;QSet<QString> seen;for(const auto &c:GlobalDatabase::get()->getCountries()){const auto code=CountryCodes::toFis(c.getAlpha3());if(code.size()!=3 || seen.contains(code))continue;seen.insert(code);rows.append(QVariantMap{{"code",code},{"name",c.getName()+" · "+code},{"flag",QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(code)+".svg")).toString()}});}m_countryOptions=rows;return m_countryOptions;}
QVariantList HillStudioController::legacyHills()const{QVariantList rows;const auto &values=GlobalDatabase::get()->getEditableGlobalHills();for(int i=0;i<values.size();++i)rows.append(QVariantMap{{"name",values[i].getName()+" HS"+QString::number(values[i].getHSPoint())},{"index",i}});return rows;}
void HillStudioController::selectPack(int i){if(i<0 || i>=m_packs.size() || i==m_pack)return;m_pack=i;m_selected=currentPack()["profiles"].toArray().isEmpty()?-1:0;m_undo.clear();m_redo.clear();m_message.clear();notifyChanged();}
void HillStudioController::createPack(){auto pack=emptyPack(tr("Nowa paczka %1").arg(m_packs.size()+1));QString error;if(!persist(pack,error)){status(error);return;}m_packs.append(pack);m_pack=m_packs.size()-1;m_selected=-1;m_undo.clear();m_redo.clear();m_message.clear();notifyChanged();}
void HillStudioController::renamePack(const QString &name){auto p=currentPack();p["name"]=name.trimmed();commit(p,m_selected);}
void HillStudioController::select(int i){if(i<0 || i>=currentPack()["profiles"].toArray().size())return;m_selected=i;m_message.clear();notifyChanged();}
void HillStudioController::add(){auto p=currentPack();auto entries=p["profiles"].toArray();entries.append(HillProfile::create().toJson());p["profiles"]=entries;commit(p,entries.size()-1,tr("Dodano skocznię"));}
void HillStudioController::duplicate(){if(m_selected<0)return;auto p=currentPack();auto entries=p["profiles"].toArray();auto copy=entries[m_selected].toObject();copy["id"]=uuid();copy["name"]=(title(copy).left(85)+tr(" — kopia")).left(100);entries.append(copy);p["profiles"]=entries;commit(p,entries.size()-1);}
void HillStudioController::remove(){if(m_selected<0)return;auto p=currentPack();auto entries=p["profiles"].toArray();entries.removeAt(m_selected);p["profiles"]=entries;commit(p,qMin(m_selected,entries.size()-1),tr("Usunięto skocznię · możesz cofnąć"));}
void HillStudioController::setValue(const QString &key,const QVariant &value){if(m_selected<0)return;const QSet<QString> keys{"name","country","k","hs","record","pointsAtK","pointsPerMetre","gatePoints","headwindPoints","tailwindPoints"};if(!keys.contains(key))return;auto p=currentPack();auto entries=p["profiles"].toArray();auto next=entries[m_selected].toObject();next[key]=QJsonValue::fromVariant(value);entries[m_selected]=next;p["profiles"]=entries;commit(p,m_selected);}
void HillStudioController::setPerk(int pair,int side,int level,bool toggle){if(m_selected<0 || pair<0 || pair>=4 || (side!=1 && side!=-1) || level<1 || level>4)return;auto p=currentPack();auto entries=p["profiles"].toArray();auto next=entries[m_selected].toObject();auto values=next["perks"].toArray();while(values.size()<4)values.append(0);values[pair]=toggle && values[pair].toInt()*side>0?0:side*level;next["perks"]=values;entries[m_selected]=next;p["profiles"]=entries;commit(p,m_selected,{},true);}
void HillStudioController::copyLegacy(int index){const auto &values=GlobalDatabase::get()->getEditableGlobalHills();if(index<0 || index>=values.size())return;const auto &old=values[index];auto hill=HillProfile::create();hill.name=old.getName();hill.country=CountryCodes::toFis(old.getCountryCode());hill.sourceId=old.getIDStr();hill.k=old.getKPoint();hill.hs=old.getHSPoint();hill.record=old.getRecord();hill.pointsAtK=old.getPointsForKPoint();hill.pointsPerMetre=old.getPointsForMeter();hill.gatePoints=old.getPointsForGate();hill.headwindPoints=old.getPointsForFrontWind();hill.tailwindPoints=old.getPointsForBackWind();auto p=currentPack();auto entries=p["profiles"].toArray();entries.append(hill.toJson());p["profiles"]=entries;commit(p,entries.size()-1,tr("Skopiowano skocznię. Perki dobierz od zera."));}
void HillStudioController::undo(){if(m_undo.isEmpty())return;const auto state=m_undo.last();QString error;if(!persist(state["pack"].toObject(),error)){status(error);return;}m_redo.append(QJsonObject{{"pack",currentPack()},{"selection",m_selected}});m_packs[m_pack]=state["pack"].toObject();m_selected=state["selection"].toInt();m_undo.removeLast();m_message=tr("Cofnięto");notifyChanged();}
void HillStudioController::redo(){if(m_redo.isEmpty())return;const auto state=m_redo.last();QString error;if(!persist(state["pack"].toObject(),error)){status(error);return;}m_undo.append(QJsonObject{{"pack",currentPack()},{"selection",m_selected}});m_packs[m_pack]=state["pack"].toObject();m_selected=state["selection"].toInt();m_redo.removeLast();m_message=tr("Ponowiono");notifyChanged();}
bool HillStudioController::exportTo(const QString &file){QString error;if(!validatePack(currentPack(),error) || !writeJson(file,currentPack())){status(tr("Nie udało się wyeksportować paczki."));return false;}status(tr("Wyeksportowano paczkę"));return true;}
bool HillStudioController::importFrom(const QString &file){QString error;QJsonObject pack;if(!SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Hills,m_root).read(file,pack,error)){status(error);return false;}pack["id"]=uuid();auto entries=pack["profiles"].toArray();for(int i=0;i<entries.size();++i){auto p=entries[i].toObject();p["id"]=uuid();entries[i]=p;}pack["profiles"]=entries;if(!persist(pack,error)){status(error);return false;}m_packs.append(pack);m_pack=m_packs.size()-1;m_selected=entries.isEmpty()?-1:0;m_undo.clear();m_redo.clear();m_message=tr("Wczytano kopię paczki");notifyChanged();return true;}
void HillStudioController::exportPack(){const auto file=QFileDialog::getSaveFileName(qobject_cast<QWidget *>(parent()),tr("Eksportuj paczkę"),{},tr("Paczka skoczni (*.json)"));if(!file.isEmpty())exportTo(file);}
void HillStudioController::importPack(){const auto file=QFileDialog::getOpenFileName(qobject_cast<QWidget *>(parent()),tr("Importuj paczkę"),{},tr("Paczka skoczni (*.json)"));if(!file.isEmpty())importFrom(file);}

void HillStudioController::notifyChanged()
{
    // Synchronize domain observers now; refresh QML after its input callback.
    if(!m_changePending) {
        m_changePending=true;
        QTimer::singleShot(0,this,[this]{m_changePending=false;emit uiChanged();});
    }
    emit changed();
}
