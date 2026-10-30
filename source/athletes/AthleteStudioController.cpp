#include "AthleteStudioController.h"
#include "../simulation-v2/ProfilePackRepository.h"
#include "../global/GlobalDatabase.h"
#include "../global/GlobalSimulationSettings.h"
#include "../global/CountryCodes.h"
#include <QStandardPaths>
#include <QSaveFile>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSet>
#include <QUrl>
#include <QUuid>
#include <QTimer>
#include <QDesktopServices>
#include <QFileDialog>
#include <QWidget>
#include <cmath>

namespace {
QString uuid(){return QUuid::createUuid().toString(QUuid::WithoutBraces);}
QJsonObject emptyPack(const QString &name){return {{"format","sj-sim-athlete-pack"},{"version",1},{"id",uuid()},{"name",name},{"profiles",QJsonArray()}};}
bool writeJson(const QString &file,const QJsonObject &object){QSaveFile output(file);const auto data=QJsonDocument(object).toJson(QJsonDocument::Indented);return output.open(QIODevice::WriteOnly) && output.write(data)==data.size() && output.commit();}
QString title(const QJsonObject &p){const QString name=(p["name"].toString()+" "+p["surname"].toString()).trimmed();return name.isEmpty()?QObject::tr("Nowy skoczek"):name;}
bool insidePack(const QString &file,const QString &folder){
    const auto base=QFileInfo(folder).canonicalFilePath();if(base.isEmpty())return false;
#ifdef Q_OS_WIN
    return QFileInfo(file).canonicalFilePath().startsWith(base+"/",Qt::CaseInsensitive);
#else
    return QFileInfo(file).canonicalFilePath().startsWith(base+"/",Qt::CaseSensitive);
#endif
}
QString photoReference(const QJsonObject &profile){const auto path=profile["photoPath"].toString();return path.isEmpty()?profile["portrait"].toString():path;}
bool copyReferencedPhotos(const QJsonObject &pack,const QString &source,const QString &target,QString &error){
    QSet<QString> copied;
    for(const auto &entry:pack["profiles"].toArray()){
        const auto relative=photoReference(entry.toObject());if(relative.isEmpty() || copied.contains(relative))continue;copied.insert(relative);
        const auto origin=source+"/"+relative;const QFileInfo file(origin);if(!file.exists())continue;
        if(!file.isFile() || !insidePack(origin,source)){error=QObject::tr("Zdjęcie musi znajdować się w folderze paczki.");return false;}
        const auto destination=target+"/"+relative;
        if(!QDir().mkpath(QFileInfo(destination).absolutePath()) || !QFile::copy(origin,destination)){error=QObject::tr("Nie udało się przenieść zdjęć razem z paczką.");return false;}
    }
    return true;
}

}
AthleteStudioController::AthleteStudioController(QObject *parent,const QString &root):QObject(parent),m_root(root.isEmpty()?QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/athlete-studio":root){
    connect(this,&AthleteStudioController::changed,this,&AthleteStudioController::perksChanged);
    connect(this,&AthleteStudioController::changed,this,&AthleteStudioController::historyChanged);
    connect(this,&AthleteStudioController::changed,this,&AthleteStudioController::statusChanged);
    const auto loaded=SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Athletes,m_root).load();
    m_packs=loaded.packs;m_message=loaded.issues.join('\n');
    if(m_packs.isEmpty())m_packs.append(emptyPack(tr("Moi skoczkowie")));
    if(!currentPack()["profiles"].toArray().isEmpty())m_selected=0;
    m_lastPhoto=profile().value("photo").toString();
    auto *photoWatch=new QTimer(this);photoWatch->setInterval(1000);
    connect(photoWatch,&QTimer::timeout,this,[this]{const auto photo=profile().value("photo").toString();if(photo!=m_lastPhoto){m_lastPhoto=photo;notifyChanged();}});photoWatch->start();
}
QString AthleteStudioController::directory(const QJsonObject &pack)const{return m_root+"/"+pack["id"].toString();}
QJsonObject AthleteStudioController::currentPack()const{return m_packs.value(m_pack);}
QString AthleteStudioController::packName()const{return currentPack()["name"].toString();}
QVariantList AthleteStudioController::selectedPerks()const {const auto entries=currentPack()["profiles"].toArray();return m_selected>=0 && m_selected<entries.size()?entries[m_selected].toObject()["perks"].toArray().toVariantList():QVariantList();}
void AthleteStudioController::status(const QString &message){m_message=message;notifyChanged();}
bool AthleteStudioController::persist(const QJsonObject &pack,QString &error)const{
    return SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Athletes,m_root).save(pack,error,false);
}
bool AthleteStudioController::commit(const QJsonObject &next,int selection,const QString &message,bool perksOnly){if(next==currentPack()){m_selected=selection;return true;}QString error;QSet<QString> ids;const auto profiles=next["profiles"].toArray();if(profiles.size()>10000){status(tr("Paczka może mieć najwyżej 10 000 profili."));return false;}if(!perksOnly)for(const auto &entry:profiles){AthleteProfile checked;if(!AthleteProfile::fromJson(entry.toObject(),checked,error)){status(error);return false;}if(ids.contains(checked.id)){status(tr("Powtórzony identyfikator skoczka."));return false;}ids.insert(checked.id);}if(!persist(next,error)){status(error);return false;}m_undo.append(QJsonObject{{"pack",currentPack()},{"selection",m_selected}});if(m_undo.size()>100)m_undo.removeFirst();m_redo.clear();m_packs[m_pack]=next;m_selected=selection;m_message=message.isEmpty()?tr("Zapisano"):message;if(perksOnly){emit perksChanged();emit historyChanged();emit statusChanged();}else notifyChanged();return true;}
QVariantList AthleteStudioController::packs()const{QVariantList list;for(const auto &p:m_packs)list.append(QVariantMap{{"name",p["name"].toString()},{"count",p["profiles"].toArray().size()}});return list;}
QVariantList AthleteStudioController::profiles()const{QVariantList rows;const auto source=currentPack()["profiles"].toArray();for(int i=0;i<source.size();++i){auto p=source[i].toObject().toVariantMap();p["label"]=title(source[i].toObject());p["index"]=i;p["flag"]=QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(p["country"].toString())+".svg")).toString();rows.append(p);}return rows;}
QString AthleteStudioController::resolvedPhotoPath() const {
    const auto entries=currentPack()["profiles"].toArray();if(m_selected<0 || m_selected>=entries.size())return {};
    const auto relative=photoReference(entries[m_selected].toObject());return relative.isEmpty()?QString():QDir(packDirectory()).absoluteFilePath(relative);
}
QVariantMap AthleteStudioController::profile()const {
    const auto entries=currentPack()["profiles"].toArray();if(m_selected<0 || m_selected>=entries.size())return {};
    const auto object=entries[m_selected].toObject();auto p=object.toVariantMap();p["label"]=title(object);
    p["flag"]=QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(p["country"].toString())+".svg")).toString();
    const auto path=resolvedPhotoPath();const QFileInfo file(path);const bool available=!path.isEmpty() && file.isFile() && insidePack(path,packDirectory());
    if(p["photoPath"].toString().isEmpty() && !object["portrait"].toString().isEmpty())p["photoPath"]=object["portrait"].toString();
    p["photo"]=available?QUrl::fromLocalFile(path).toString():QString();p["photoAvailable"]=available;return p;
}
QVariantList AthleteStudioController::countries()const{if(!m_countryOptions.isEmpty())return m_countryOptions;QVariantList values;QSet<QString> codes;for(const auto &country:GlobalDatabase::get()->getCountries()){const auto code=CountryCodes::toFis(country.getAlpha3());if(code.size()!=3 || codes.contains(code))continue;codes.insert(code);values.append(QVariantMap{{"code",code},{"name",country.getName()+" · "+code},{"flag",QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(code)+".svg")).toString()}});}m_countryOptions=values;return m_countryOptions;}
QVariantList AthleteStudioController::legacyPacks()const{QVariantList values;for(const auto &p:GlobalDatabase::get()->getPacks())if(p.kind==DataPack::Jumpers)values.append(QVariantMap{{"name",p.name},{"id",p.id},{"count",p.jumpers.size()}});return values;}
QVariantList AthleteStudioController::legacyAthletes(int pack)const{int index=0;for(const auto &p:GlobalDatabase::get()->getPacks())if(p.kind==DataPack::Jumpers){if(index++!=pack)continue;QVariantList values;for(int i=0;i<p.jumpers.size();++i)values.append(QVariantMap{{"name",p.jumpers[i].getNameAndSurname()},{"index",i}});return values;}return {};}
void AthleteStudioController::selectPack(int index){if(index<0 || index>=m_packs.size() || index==m_pack)return;m_pack=index;m_selected=currentPack()["profiles"].toArray().isEmpty()?-1:0;m_undo.clear();m_redo.clear();m_message.clear();notifyChanged();}
void AthleteStudioController::renamePack(const QString &name){if(name.trimmed().isEmpty() || name.size()>100){status(tr("Nazwa paczki musi mieć od 1 do 100 znaków."));return;}auto next=currentPack();next["name"]=name.trimmed();commit(next,m_selected);}
void AthleteStudioController::createPack(){auto next=emptyPack(tr("Nowa paczka %1").arg(m_packs.size()+1));QString error;if(!persist(next,error)){status(error);return;}m_packs.append(next);m_pack=m_packs.size()-1;m_selected=-1;m_undo.clear();m_redo.clear();m_message=tr("Utworzono paczkę");notifyChanged();}
void AthleteStudioController::select(int index){if(index<0 || index>=currentPack()["profiles"].toArray().size() || index==m_selected)return;m_selected=index;m_message.clear();notifyChanged();}
void AthleteStudioController::move(int from,int to){
    auto next=currentPack();auto entries=next["profiles"].toArray();
    if(from<0 || to<0 || from>=entries.size() || to>=entries.size() || from==to)return;
    const auto moved=entries.takeAt(from);entries.insert(to,moved);next["profiles"]=entries;
    int selected=m_selected;
    if(selected==from)selected=to;
    else if(from<selected && selected<=to)--selected;
    else if(to<=selected && selected<from)++selected;
    commit(next,selected);
}
void AthleteStudioController::add(){auto next=currentPack();auto entries=next["profiles"].toArray();entries.append(AthleteProfile::create().toJson());next["profiles"]=entries;commit(next,entries.size()-1,tr("Dodano skoczka"));}
void AthleteStudioController::duplicate(){if(m_selected<0)return;auto next=currentPack();auto entries=next["profiles"].toArray();auto copy=entries[m_selected].toObject();copy["id"]=uuid();copy["surname"]=(copy["surname"].toString().left(90)+tr(" — kopia")).left(100);entries.append(copy);next["profiles"]=entries;commit(next,entries.size()-1,tr("Utworzono kopię"));}
void AthleteStudioController::remove(){if(m_selected<0)return;auto next=currentPack();auto entries=next["profiles"].toArray();entries.removeAt(m_selected);next["profiles"]=entries;commit(next,entries.isEmpty()?-1:qMin(m_selected,entries.size()-1),tr("Usunięto skoczka · możesz cofnąć"));}
void AthleteStudioController::setValue(const QString &key,const QVariant &value){if(m_selected<0)return;if(key=="photoPath"){setPhoto(value.toString());return;}const QSet<QString> keys{"name","surname","country","overall","form","landing","birthYear","personalBest","photoPath"};if(!keys.contains(key))return;auto next=currentPack();auto entries=next["profiles"].toArray();auto p=entries[m_selected].toObject();p[key]=QJsonValue::fromVariant(value);if(key=="photoPath"){p[key]=value.toString().trimmed();p["portrait"]="";}AthleteProfile checked;QString error;if(!AthleteProfile::fromJson(p,checked,error)){status(error);return;}entries[m_selected]=checked.toJson();next["profiles"]=entries;commit(next,m_selected);}
void AthleteStudioController::setPerk(int pair,int side,int level,bool toggle){if(m_selected<0 || pair<0 || pair>=3 || (side!=1 && side!=-1) || level<1 || level>4)return;auto next=currentPack();auto entries=next["profiles"].toArray();auto p=entries[m_selected].toObject();auto values=p["perks"].toArray();const int old=values[pair].toInt();values[pair]=(toggle && old*side>0)?0:side*level;p["perks"]=values;entries[m_selected]=p;next["profiles"]=entries;commit(next,m_selected,{},true);}
void AthleteStudioController::undo(){if(m_undo.isEmpty())return;const auto snapshot=m_undo.last();QString error;if(!persist(snapshot["pack"].toObject(),error)){status(error);return;}m_redo.append(QJsonObject{{"pack",currentPack()},{"selection",m_selected}});m_undo.removeLast();m_packs[m_pack]=snapshot["pack"].toObject();m_selected=snapshot["selection"].toInt();status(tr("Cofnięto"));}
void AthleteStudioController::redo(){if(m_redo.isEmpty())return;const auto snapshot=m_redo.last();QString error;if(!persist(snapshot["pack"].toObject(),error)){status(error);return;}m_undo.append(QJsonObject{{"pack",currentPack()},{"selection",m_selected}});m_redo.removeLast();m_packs[m_pack]=snapshot["pack"].toObject();m_selected=snapshot["selection"].toInt();status(tr("Ponowiono"));}
void AthleteStudioController::copyLegacy(int pack,int athlete){int index=0;for(const auto &p:GlobalDatabase::get()->getPacks())if(p.kind==DataPack::Jumpers){if(index++!=pack)continue;if(athlete<0 || athlete>=p.jumpers.size())return;const auto &old=p.jumpers[athlete];const auto skills=old.getJumperSkills();const double range=qMax(1,GlobalSimulationSettings::get()->getMaxSkills());auto profile=AthleteProfile::create();profile.name=old.getName();profile.surname=old.getSurname();profile.country=CountryCodes::toFis(old.getCountryCode());profile.birthYear=old.getBirthYear();profile.personalBest=qBound(0.,old.getPersonalBest(),1000.);profile.overall=qBound(1,qRound((skills.getTakeoffTechnique()+skills.getFlightTechnique())*.5/range*100),100);profile.form=qBound(1,qRound(skills.getForm()/range*100),100);profile.landing=qBound(0,qRound(skills.getLandingStyle()/5.),4);profile.sourceId=old.getIDStr();QString imageName=old.getImageName();imageName.replace('\\','/');profile.photoPath=QFileInfo(imageName).fileName();auto next=currentPack();auto entries=next["profiles"].toArray();entries.append(profile.toJson());next["profiles"]=entries;commit(next,entries.size()-1,tr("To niedokładna, przybliżona migracja. OVR i forma są przeliczone, lądowanie przybliżone, perki od zera. Plik zdjęcia nie jest kopiowany."));return;}}
bool AthleteStudioController::setPhoto(const QString &file) {
    if(m_selected<0)return false;
    QString relative=file.trimmed();const auto url=QUrl(relative);if(url.isLocalFile())relative=url.toLocalFile();
    if(QFileInfo(relative).isAbsolute())relative=QDir(packDirectory()).relativeFilePath(relative);
    auto next=currentPack();auto entries=next["profiles"].toArray();auto p=entries[m_selected].toObject();
    p["photoPath"]=relative;p["portrait"]="";AthleteProfile checked;QString error;
    if(!AthleteProfile::fromJson(p,checked,error)){status(error);return false;}
    const QFileInfo target(QDir(packDirectory()).absoluteFilePath(checked.photoPath));
    if(!checked.photoPath.isEmpty() && target.exists() && !insidePack(target.absoluteFilePath(),packDirectory())){status(tr("Zdjęcie musi znajdować się w folderze paczki."));return false;}
    entries[m_selected]=checked.toJson();next["profiles"]=entries;return commit(next,m_selected);
}
void AthleteStudioController::choosePhoto(){const auto file=QFileDialog::getOpenFileName(qobject_cast<QWidget *>(parent()),tr("Zdjęcie z folderu paczki"),packDirectory(),tr("Zdjęcia (*.png *.jpg *.jpeg *.webp)"));if(!file.isEmpty())setPhoto(file);}
void AthleteStudioController::openPackFolder(){QString error;if(!persist(currentPack(),error)){status(error);return;}if(!QDesktopServices::openUrl(QUrl::fromLocalFile(packDirectory())))status(tr("Nie udało się otworzyć folderu paczki: %1").arg(packDirectory()));}

void AthleteStudioController::clearPhoto(){setPhoto({});}
bool AthleteStudioController::readPack(const QString &file,QJsonObject &pack,QString &error)const{
    return SimulationV2::ProfilePackRepository(SimulationV2::ProfilePackKind::Athletes,m_root).read(file,pack,error);
}
bool AthleteStudioController::exportTo(const QString &destination){
    if(QFileInfo::exists(destination)){status(tr("Wybierz nowy folder. Istniejące pliki nie zostaną nadpisane."));return false;}
    if(!QDir().mkpath(destination)){status(tr("Nie udało się utworzyć folderu eksportu."));return false;}
    QString error;const bool images=copyReferencedPhotos(currentPack(),packDirectory(),destination,error);
    if(!images || !writeJson(destination+"/pack.json",currentPack())){QDir(destination).removeRecursively();status(error.isEmpty()?tr("Nie udało się wyeksportować paczki."):error);return false;}
    status(tr("Wyeksportowano paczkę"));return true;
}
bool AthleteStudioController::importFrom(const QString &file){
    QJsonObject pack;QString error;if(!readPack(file,pack,error)){status(error);return false;}pack["id"]=uuid();const auto target=directory(pack);
    if(!QDir().mkpath(target)){status(tr("Nie udało się utworzyć folderu paczki."));return false;}
    if(!copyReferencedPhotos(pack,QFileInfo(file).absolutePath(),target,error) || !persist(pack,error)){QDir(target).removeRecursively();status(error);return false;}
    m_packs.append(pack);m_pack=m_packs.size()-1;m_selected=pack["profiles"].toArray().isEmpty()?-1:0;m_undo.clear();m_redo.clear();status(tr("Wczytano kopię paczki"));return true;
}

void AthleteStudioController::exportPack(){const auto parent=QFileDialog::getExistingDirectory(qobject_cast<QWidget *>(this->parent()),tr("Folder docelowy paczki"));if(!parent.isEmpty())exportTo(parent+"/sj-athletes-"+uuid().left(8));}
void AthleteStudioController::importPack(){const auto file=QFileDialog::getOpenFileName(qobject_cast<QWidget *>(parent()),tr("Wczytaj nową paczkę skoczków"),{},tr("Paczka skoczków (pack.json)"));if(!file.isEmpty())importFrom(file);}

void AthleteStudioController::notifyChanged()
{
    // Synchronize domain observers now; refresh QML after its input callback.
    if(!m_changePending) {
        m_changePending=true;
        QTimer::singleShot(0,this,[this]{m_changePending=false;emit uiChanged();});
    }
    emit changed();
}
