#include "ResourceBundle.h"
#include "UserDataPaths.h"
#include "DataPack.h"
#include "AtomicJsonFile.h"
#include "../simulation-v2/ProfilePackRepository.h"
#include "../seasons/SeasonCalendarPreset.h"
#include "../seasons/SimulationSave.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QSaveFile>
#include <QTemporaryDir>
#include <QUuid>
#include <QSet>
#include <QRegularExpression>
#include <cmath>
#include <memory>

namespace {
const QByteArray magic("SJSIM-PACK\n1\n");
constexpr qint64 limit=256*1024*1024;
const QStringList kinds{"Athletes","Hills","Rules","Points","Calendars","Saves"};
QString directory(const QString &kind){return kind=="Calendars"||kind=="Saves"?UserDataPaths::path(kind):UserDataPaths::path("Packs/"+kind);}
QString category(const QString &kind){return kind=="Athletes"?QObject::tr("Skoczkowie"):kind=="Hills"?QObject::tr("Skocznie"):kind=="Rules"?QObject::tr("Zasady"):kind=="Points"?QObject::tr("Punktacje"):kind=="Calendars"?QObject::tr("Kalendarze"):QObject::tr("Symulacje");}
bool readJson(const QString &path,QJsonObject &out,QString &error){QFile f(path);QJsonParseError parse{};if(!f.open(QIODevice::ReadOnly)||f.size()>limit){error=QObject::tr("Nie można odczytać %1.").arg(path);return false;}const auto doc=QJsonDocument::fromJson(f.readAll(),&parse);if(parse.error!=QJsonParseError::NoError||!doc.isObject()){error=QObject::tr("Nieprawidłowy plik JSON: %1.").arg(path);return false;}out=doc.object();return true;}
QString title(const QString &kind,const QJsonObject &doc){return kind=="Calendars"?doc["calendar-preset"].toObject()["name"].toString():kind=="Points"?doc["points-for-places-preset"].toObject()["name"].toString():doc["name"].toString();}
bool relative(const QString &value){if(value.isEmpty()||QDir::isAbsolutePath(value)||value.contains('\\')||value.contains(':')||value.contains(QChar(0)))return false;const auto clean=QDir::cleanPath(value);if(clean!=value||clean==".."||clean.startsWith("../")||clean.startsWith('/'))return false;
    for(const auto &part:value.split('/'))if(part.isEmpty()||part.endsWith('.')||part.endsWith(' ')||AtomicJsonFile::namedPath(QDir::tempPath(),part).isEmpty()||QRegularExpression("^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\\.|$)",QRegularExpression::CaseInsensitiveOption).match(part).hasMatch())return false;
    return true;}
bool validate(const QString &kind,const QJsonObject &doc,QString &error){
    if(kind=="Athletes"||kind=="Hills")return SimulationV2::ProfilePackRepository(kind=="Athletes"?SimulationV2::ProfilePackKind::Athletes:SimulationV2::ProfilePackKind::Hills,directory(kind)).validate(doc,error);
    if(kind=="Rules"){DataPack p;return DataPack::fromJson(doc,p,error)&&p.kind==DataPack::Rules;}
    if(kind=="Calendars")return SeasonCalendarPreset::validateJson(doc["calendar-preset"].toObject(),error);
    if(kind=="Saves"){std::unique_ptr<SimulationSave> save(SimulationSave::getFromJson(doc["simulation-save"].toObject(),nullptr));if(save)return true;error=QObject::tr("Nieprawidłowy zapis symulacji.");return false;}
    if(kind=="Points"){
        const auto p=doc["points-for-places-preset"].toObject();const auto values=p["pointsForPlaces"].toArray();
        if(p["name"].toString().trimmed().isEmpty()||p["name"].toString().size()>100||!p["pointsForPlaces"].isArray()||values.size()>10000){error=QObject::tr("Nieprawidłowa punktacja.");return false;}
        for(const auto &value:values)if(!value.isDouble()||!std::isfinite(value.toDouble())||std::abs(value.toDouble())>1e9){error=QObject::tr("Nieprawidłowa liczba punktów.");return false;}
        return true;
    }
    error=QObject::tr("Nieobsługiwany rodzaj zasobu.");return false;
}
QJsonObject resource(const QString &kind,const QJsonObject &doc,const QString &photoDirectory,QString &error){
    if(!validate(kind,doc,error))return {};
    QJsonArray files;QSet<QString> seen;
    if(kind=="Athletes")for(const auto &value:doc["profiles"].toArray()){
        const auto path=value.toObject()["photoPath"].toString();if(path.isEmpty()||seen.contains(path))continue;seen.insert(path);
        if(!relative(path)){error=QObject::tr("Nieprawidłowa ścieżka zdjęcia.");return {};}
        const auto location=QDir(photoDirectory).filePath(path);const QFileInfo info(location);if(!info.exists())continue;
        if(!info.canonicalFilePath().startsWith(QFileInfo(photoDirectory).canonicalFilePath()+"/")||info.isSymLink()){error=QObject::tr("Zdjęcie musi należeć do folderu paczki.");return {};}
        QFile input(location);if(!input.open(QIODevice::ReadOnly)||input.size()>32*1024*1024){error=QObject::tr("Nie można odczytać zdjęcia: %1.").arg(path);return {};}
        const auto bytes=input.readAll();files.append(QJsonObject{{"path",path},{"data",QString::fromLatin1(bytes.toBase64())},{"sha256",QString::fromLatin1(QCryptographicHash::hash(bytes,QCryptographicHash::Sha256).toHex())}});
    }
    return {{"kind",kind},{"document",doc},{"files",files}};
}
bool writeBundle(const QString &path,const QJsonArray &items,QString &error){
    if(items.isEmpty()){error=QObject::tr("Wybierz zasoby do eksportu.");return false;}
    const auto json=QJsonDocument(QJsonObject{{"format","sj-sim-pack"},{"version",1},{"resources",items}}).toJson(QJsonDocument::Compact);
    if(json.size()>limit){error=QObject::tr("Paczka jest zbyt duża. Podziel ją na mniejsze paczki.");return false;}
    QSaveFile output(path);const auto bytes=magic+json;
    if(!output.open(QIODevice::WriteOnly)||output.write(bytes)!=bytes.size()||!output.commit()){error=QObject::tr("Nie udało się zapisać paczki.");return false;}return true;
}
QString relativeDestination(const QString &kind,const QJsonObject &doc,const QString &name){
    if(kind=="Athletes")return "Packs/Athletes/"+doc["id"].toString();
    if(kind=="Hills"||kind=="Rules")return "Packs/"+kind+"/"+doc["id"].toString()+".json";
    return (kind=="Points"?"Packs/Points/":kind+"/")+name+".json";
}
QJsonValue remap(const QJsonValue &value,const QHash<QString,QString> &ids){
    if(value.isString())return ids.value(value.toString(),value.toString());
    if(value.isArray()){QJsonArray out;for(const auto &v:value.toArray())out.append(remap(v,ids));return out;}
    if(value.isObject()){auto out=value.toObject();for(auto i=out.begin();i!=out.end();++i)i.value()=remap(i.value(),ids);return out;}
    return value;
}
}
QVariantList ResourceBundle::entries(){
    QVariantList out;
    for(const auto &kind:kinds){QDir dir(directory(kind));
        const auto items=kind=="Athletes"?dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name):dir.entryInfoList({"*.json"},QDir::Files,QDir::Name);
        for(const auto &item:items){if(item.isSymLink())continue;const auto path=kind=="Athletes"?item.filePath()+"/pack.json":item.filePath();QJsonObject doc;QString error;if(kind!="Saves"&&!readJson(path,doc,error))continue;
            QString name=title(kind,doc);if(name.isEmpty())name=item.completeBaseName();
            const int count=kind=="Athletes"||kind=="Hills"?doc["profiles"].toArray().size():kind=="Rules"?doc["competitionsRules"].toArray().size():kind=="Points"?doc["points-for-places-preset"].toObject()["pointsForPlaces"].toArray().size():kind=="Calendars"?doc["calendar-preset"].toObject()["calendar"].toObject()["competitions"].toArray().size():0;
            out.append(QVariantMap{{"key",kind+"/"+item.fileName()},{"kind",kind},{"category",category(kind)},{"name",name},{"count",count}});
        }
    }return out;
}
bool ResourceBundle::exportSelection(const QString &file,const QStringList &keys,QString &error){
    const auto all=entries();QSet<QString> selected(keys.begin(),keys.end());QSet<QString> venues,rules;bool calendars=false,saves=false;
    for(const auto &value:all){const auto entry=value.toMap();if(!selected.contains(entry["key"].toString()))continue;const auto kind=entry["kind"].toString();if(kind!="Calendars"&&kind!="Saves")continue;
        QJsonObject doc;if(!readJson(directory(kind)+"/"+entry["key"].toString().section('/',1),doc,error))return false;
        if(kind=="Saves"){saves=true;continue;}calendars=true;
        const auto preset=doc["calendar-preset"].toObject();for(const auto &hill:preset["hills"].toArray())venues.insert(hill.toObject()["name"].toString()+"/"+QString::number(hill.toObject()["hs-point"].toDouble(),'g',15));
        for(const auto &event:preset["calendar"].toObject()["competitions"].toArray())rules.insert(event.toObject()["rules"].toObject()["name"].toString());
    }
    if(calendars||saves)for(const auto &value:all){const auto entry=value.toMap();const auto kind=entry["kind"].toString();const auto key=entry["key"].toString();
        if(saves && kind!="Saves"&&kind!="Calendars"){selected.insert(key);continue;}
        if(kind=="Points"){selected.insert(key);continue;}
        if(kind!="Hills"&&kind!="Rules")continue;QJsonObject doc;if(!readJson(directory(kind)+"/"+key.section('/',1),doc,error))return false;
        if(kind=="Hills")for(const auto &hill:doc["profiles"].toArray()){const auto h=hill.toObject();const double hs=h["hs"].toDouble(h["hsPoint"].toDouble());if(venues.contains(h["name"].toString()+"/"+QString::number(hs,'g',15)))selected.insert(key);}
        else for(const auto &rule:doc["competitionsRules"].toArray())if(rules.contains(rule.toObject()["name"].toString()))selected.insert(key);
    }
    QJsonArray items;
    for(const auto &value:all){const auto entry=value.toMap();const auto key=entry["key"].toString();if(!selected.contains(key))continue;const auto kind=entry["kind"].toString();const auto path=directory(kind)+"/"+key.section('/',1);QJsonObject doc;
        if(!readJson(kind=="Athletes"?path+"/pack.json":path,doc,error))return false;
        auto item=resource(kind,doc,path,error);if(item.isEmpty())return false;item["name"]=entry["name"].toString();items.append(item);
    }
    return writeBundle(file,items,error);
}
bool ResourceBundle::exportPack(const QString &file,const QJsonObject &pack,const QString &photos,QString &error){
    const auto kind=pack["format"]=="sj-sim-athlete-pack"?QString("Athletes"):pack["format"]=="sj-sim-hill-pack"?QString("Hills"):QString("Rules");
    auto doc=pack;QJsonArray items;
    if(kind=="Rules") {for(const auto &points:doc["pointsForPlacesPresets"].toArray()){const auto item=resource("Points",{{"points-for-places-preset",points.toObject()}},{},error);if(item.isEmpty())return false;items.append(item);}doc["pointsForPlacesPresets"]=QJsonArray();}
    const auto item=resource(kind,doc,photos,error);if(item.isEmpty())return false;items.prepend(item);return writeBundle(file,items,error);
}
bool ResourceBundle::importFile(const QString &file,QStringList &installed,QString &error){
    installed.clear();QFile input(file);if(!input.open(QIODevice::ReadOnly)||input.size()>limit){error=QObject::tr("Nie można odczytać paczki.");return false;}
    const auto bytes=input.readAll();if(!bytes.startsWith(magic)||bytes.size()<=magic.size()){error=QObject::tr("Nieprawidłowy format paczki Sj.Sim.");return false;}
    QJsonParseError parse{};const auto json=bytes.mid(magic.size());const auto bundle=QJsonDocument::fromJson(json,&parse).object();const auto resources=bundle["resources"].toArray();
    if(parse.error!=QJsonParseError::NoError||bundle["format"]!="sj-sim-pack"||bundle["version"]!=1||resources.isEmpty()||resources.size()>1000){error=QObject::tr("Uszkodzona lub nieobsługiwana paczka.");return false;}
    QTemporaryDir staging(UserDataPaths::path(".import-XXXXXX"));if(!staging.isValid()){error=QObject::tr("Nie można przygotować importu.");return false;}
    struct Planned {QString kind,path;QJsonObject doc;QJsonArray files;};QVector<Planned> plans;QSet<QString> planned,identities;QHash<QString,QString> ids;
    qint64 total=0;
    for(const auto &value:resources){const auto item=value.toObject();const auto kind=item["kind"].toString();auto doc=item["document"].toObject();
        if(!kinds.contains(kind)||!validate(kind,doc,error))return false;
        QString name=title(kind,doc);if(kind=="Saves")name=item["name"].toString(QFileInfo(file).completeBaseName());
        const bool namedFile=kind=="Points"||kind=="Calendars"||kind=="Saves";
        if(name.trimmed().isEmpty()||name.size()>200||(namedFile && (AtomicJsonFile::namedPath(staging.path(),name).isEmpty()||name.endsWith('.')||name.endsWith(' ')||QRegularExpression("^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\\.|$)",QRegularExpression::CaseInsensitiveOption).match(name).hasMatch()))){error=QObject::tr("Nieprawidłowa nazwa zasobu.");return false;}
        const auto originalId=doc["id"].toString();
        const auto identity=kind+"/"+(originalId.isEmpty()?name:originalId);
        if(identities.contains(identity)){error=QObject::tr("Paczka zawiera powtórzone zasoby.");return false;}identities.insert(identity);
        QString destination=relativeDestination(kind,doc,name);
        if(kind=="Athletes"||kind=="Hills"||kind=="Rules"){
            if(QFileInfo::exists(UserDataPaths::path(destination))||planned.contains(destination)){
                const auto id=QUuid::createUuid().toString(QUuid::WithoutBraces);doc["id"]=id;ids.insert(originalId,id);destination=relativeDestination(kind,doc,name);
            }
        } else {
            const auto base=name;for(int n=2;QFileInfo::exists(UserDataPaths::path(destination))||planned.contains(destination);++n){name=base+QString(" (%1)").arg(n);destination=relativeDestination(kind,doc,name);}
            if(kind=="Calendars"||kind=="Points"){const auto key=kind=="Calendars"?QString("calendar-preset"):QString("points-for-places-preset");auto body=doc[key].toObject();body["name"]=name;doc[key]=body;}
        }
        if(!relative(destination)){error=QObject::tr("Nieprawidłowa ścieżka zasobu.");return false;}
        const auto files=item["files"].toArray();if(files.size()>10000 || (kind!="Athletes"&&!files.isEmpty())){error=QObject::tr("Nieprawidłowe załączniki paczki.");return false;}
        QSet<QString> paths;
        for(const auto &v:files){const auto attachment=v.toObject();const auto path=attachment["path"].toString();const auto base64=attachment["data"].toString().toLatin1();const auto data=QByteArray::fromBase64(base64);total+=data.size();
            if(!relative(path)||path=="pack.json"||paths.contains(path)||data.size()>32*1024*1024||total>limit||data.toBase64()!=base64||QCryptographicHash::hash(data,QCryptographicHash::Sha256).toHex()!=attachment["sha256"].toString().toLatin1()){error=QObject::tr("Uszkodzony załącznik lub niedozwolona ścieżka.");return false;}paths.insert(path);
            const auto target=staging.path()+"/"+destination+"/"+path;QDir().mkpath(QFileInfo(target).absolutePath());QFile output(target);if(!output.open(QIODevice::WriteOnly)||output.write(data)!=data.size()){error=QObject::tr("Nie można zapisać zdjęcia.");return false;}
        }
        planned.insert(destination);plans.append({kind,destination,doc,files});
    }
    for(const auto &plan:plans){auto doc=remap(plan.doc,ids).toObject();const auto target=staging.path()+"/"+plan.path+(plan.kind=="Athletes"?"/pack.json":QString());QDir().mkpath(QFileInfo(target).absolutePath());if(!AtomicJsonFile::write(target,QJsonDocument(doc))){error=QObject::tr("Nie można zapisać importowanego zasobu.");return false;}}
    QStringList committed;
    for(const auto &plan:plans){const auto target=UserDataPaths::path(plan.path);QDir().mkpath(QFileInfo(target).absolutePath());if(!QDir().rename(staging.path()+"/"+plan.path,target)){
            for(const auto &path:committed){if(QFileInfo(path).isDir())QDir(path).removeRecursively();else QFile::remove(path);}installed.clear();error=QObject::tr("Nie udało się zakończyć importu. Przywrócono poprzedni stan biblioteki.");return false;
        }committed.append(target);installed.append(plan.kind+"/"+QFileInfo(plan.path).fileName());}
    error.clear();return true;
}
