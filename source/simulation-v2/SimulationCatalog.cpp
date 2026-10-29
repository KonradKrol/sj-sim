#include "SimulationCatalog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>
#include <QSet>
#include <QStandardPaths>
#include <algorithm>
#include <utility>

namespace SimulationV2 {
namespace {
QString key(DataPack::Kind kind,const QString &id) {return QString::number(int(kind))+"/"+id;}
QString key(const CatalogPack &pack) {return key(pack.data.kind,pack.data.id);}
int count(const DataPack &pack) {return pack.kind==DataPack::Jumpers?pack.jumpers.size():pack.kind==DataPack::Hills?pack.hills.size():pack.rules.size();}
QJsonObject authoring(const DataPack &pack)
{
    QJsonArray profiles;
    for(const auto &athlete:pack.jumpers) {const auto profile=athlete.getSimulationProfile();if(!profile)return {};profiles.append(profile->toJson());}
    for(const auto &hill:pack.hills) {const auto profile=hill.getSimulationProfile();if(!profile)return {};profiles.append(profile->toJson());}
    QJsonObject result{{"format",pack.kind==DataPack::Jumpers?"sj-sim-athlete-pack":"sj-sim-hill-pack"},
        {"version",1},{"id",pack.id},{"name",pack.name},{"profiles",profiles}};
    if(!pack.approximateMigrationSource.isEmpty())result["approximate-migration-source"]=pack.approximateMigrationSource;
    return result;
}
}

const CatalogPack *CatalogSnapshot::find(DataPack::Kind kind,const QString &id) const
{
    for(const auto &pack:packs)if(pack.data.kind==kind && pack.data.id==id)return &pack;
    return nullptr;
}

CatalogPaths CatalogPaths::defaults()
{
    const QString root=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return {root+"/athlete-studio",root+"/hill-studio",root+"/simulation-v2/catalog.json",QDir("jumpersImages").absolutePath()};
}

SimulationCatalog::SimulationCatalog(CatalogPaths locations):paths(std::move(locations)) {}

bool SimulationCatalog::saveSelection(const CatalogSnapshot &snapshot,QString &error) const
{
    if(!snapshot.selectionWritable) {error=QObject::tr("Uszkodzony plik wyboru paczek nie zostanie nadpisany.");return false;}
    QJsonArray order,active;for(const auto &pack:snapshot.packs)order.append(key(pack));for(const auto &id:snapshot.active)active.append(id);
    const QJsonObject state{{"format","sj-sim-simulation-v2-catalog"},{"version",1},{"order",order},{"active",active}};
    const QByteArray bytes=QJsonDocument(state).toJson(QJsonDocument::Indented);
    if(!QDir().mkpath(QFileInfo(paths.selection).absolutePath())) {error=QObject::tr("Nie można utworzyć folderu katalogu.");return false;}
    QSaveFile file(paths.selection);
    if(!file.open(QIODevice::WriteOnly) || file.write(bytes)!=bytes.size() || !file.commit()) {
        error=QObject::tr("Nie udało się zapisać wyboru paczek.");return false;
    }
    error.clear();return true;
}

CatalogSnapshot SimulationCatalog::load(const QVector<DataPack> &legacy,double maximumSkills,const QStringList &legacyActive) const
{
    CatalogSnapshot snapshot;
    const ProfilePackRepository athleteStore(ProfilePackKind::Athletes,paths.athletes),hillStore(ProfilePackKind::Hills,paths.hills);
    auto athletes=athleteStore.load(),hills=hillStore.load();snapshot.issues=athletes.issues+hills.issues;
    QSet<QString> migratedSources;
    for(const auto &pack:athletes.packs)if(!pack["approximate-migration-source"].toString().isEmpty())migratedSources.insert(key(DataPack::Jumpers,pack["approximate-migration-source"].toString()));
    for(const auto &pack:hills.packs)if(!pack["approximate-migration-source"].toString().isEmpty())migratedSources.insert(key(DataPack::Hills,pack["approximate-migration-source"].toString()));
    for(const auto &source:legacy) {
        if(source.kind==DataPack::Rules)continue;
        const auto &store=source.kind==DataPack::Jumpers?athleteStore:hillStore;
        auto &listing=source.kind==DataPack::Jumpers?athletes:hills;
        QString error;
        if(source.engineVersion==1 && !migratedSources.contains(key(source.kind,source.id))) {
            QJsonObject migrated;QStringList warnings;
            if(store.migrate(source,maximumSkills,paths.legacyPhotos,migrated,warnings,error)) {
                listing.packs.append(migrated);snapshot.issues+=warnings;migratedSources.insert(key(source.kind,source.id));
            } else snapshot.issues.append(source.name+": "+error);
        } else if(source.engineVersion==2) {
            // Preserve ready profiles from an older global-catalog import, too.
            bool found=false;for(const auto &pack:listing.packs)if(pack["id"].toString()==source.id) {found=true;break;}
            if(found)continue; // canonical authoring edits always take precedence
            const auto ready=authoring(source);
            if(ready.isEmpty())error=QObject::tr("Paczka nie zawiera kompletnych nowych profili.");
            bool valid=!ready.isEmpty() && store.validate(ready,error);
            if(valid && QFileInfo::exists(store.filePath(ready))) {
                error=QObject::tr("Plik docelowy już istnieje i nie zostanie nadpisany.");valid=false;
            }
            if(valid)valid=store.save(ready,error,false);
            if(!valid) {
                snapshot.issues.append(QObject::tr("Nie przeniesiono paczki %1 do nowego katalogu: %2. Źródło pozostaje bez zmian.").arg(source.name,error));continue;
            }
            listing.packs.append(ready);
            if(source.kind==DataPack::Jumpers)for(const auto &athlete:source.jumpers)
                if(!athlete.getImageName().isEmpty()) {snapshot.issues.append(QObject::tr("Sprawdź pliki zdjęć w przeniesionej paczce %1.").arg(source.name));break;}
        }
    }
    const auto append=[&](const ProfilePackListing &listing,const ProfilePackRepository &store) {
        for(const auto &object:listing.packs) {
            DataPack pack;QString error;
            if(DataPack::fromJson(object,pack,error)) {
                if(!object["approximate-migration-source"].toString().isEmpty()) {
                    for(const auto &suffix:QStringList{QString::fromUtf8(" — przybliżenie"),QStringLiteral(" — approximation"),QString::fromUtf8(" — přibližně")})if(pack.name.endsWith(suffix)) {
                        pack.name.chop(suffix.size());pack.name+=QObject::tr(" — ze starej wersji");break;
                    }
                }
                const auto directory=store.packDirectory(object);
                for(auto &athlete:pack.jumpers)athlete.setSimulationPackContext(pack.id,directory);
                snapshot.packs.append({pack,directory,object["migration-notice"].toString()});
            }
            else snapshot.issues.append(error);
        }
    };
    append(athletes,athleteStore);append(hills,hillStore);
    for(const auto &source:legacy)if(source.kind==DataPack::Rules)snapshot.packs.append({source,{}, {}});
    QJsonObject selection;QFile file(paths.selection);
    if(file.exists()) {
        QJsonParseError parse{};
        const auto document=file.open(QIODevice::ReadOnly) && file.size()<=2*1024*1024
            ?QJsonDocument::fromJson(file.readAll(),&parse):QJsonDocument();
        selection=document.object();
        bool valid=parse.error==QJsonParseError::NoError && selection["format"]=="sj-sim-simulation-v2-catalog" && selection["version"]==1
            && selection["order"].isArray() && selection["active"].isArray() && selection["active"].toArray().size()==3;
        QSet<QString> seen;
        for(const auto &value:selection["order"].toArray()) {const auto id=value.toString();if(!value.isString() || id.isEmpty() || seen.contains(id))valid=false;seen.insert(id);}
        for(const auto &value:selection["active"].toArray())if(!value.isString())valid=false;
        snapshot.selectionWritable=valid;
        if(!valid) {selection={};snapshot.issues.append(QObject::tr("Nie wczytano wyboru paczek. Uszkodzony katalog nie zostanie nadpisany."));}
    }
    QStringList order;for(const auto &value:selection["order"].toArray())order.append(value.toString());
    std::stable_sort(snapshot.packs.begin(),snapshot.packs.end(),[&](const CatalogPack &a,const CatalogPack &b) {
        const int ai=order.indexOf(key(a)),bi=order.indexOf(key(b));
        if(ai<0)return false;
        if(bi<0)return true;
        return ai<bi;
    });
    const auto savedActive=selection["active"].toArray();
    for(int value=0;value<3;++value) {
        const auto kind=static_cast<DataPack::Kind>(value);const CatalogPack *chosen=nullptr;
        const QString saved=value<savedActive.size()?savedActive[value].toString():QString();
        if(!saved.isEmpty())chosen=snapshot.find(kind,saved);
        if(!chosen && kind!=DataPack::Rules)
            for(const auto &pack:snapshot.packs)if(pack.data.kind==kind && count(pack.data)>0 && pack.data.approximateMigrationSource.isEmpty()) {chosen=&pack;break;}
        if(!chosen && !legacyActive.value(value).isEmpty())
            for(const auto &pack:snapshot.packs)if(pack.data.kind==kind && (pack.data.id==legacyActive[value] || pack.data.approximateMigrationSource==legacyActive[value])) {chosen=&pack;break;}
        if(!chosen)for(const auto &pack:snapshot.packs)if(pack.data.kind==kind && count(pack.data)>0) {chosen=&pack;break;}
        if(!chosen)for(const auto &pack:snapshot.packs)if(pack.data.kind==kind) {chosen=&pack;break;}
        if(chosen)snapshot.active[value]=chosen->data.id;
    }
    if(snapshot.selectionWritable) {QString error;if(!saveSelection(snapshot,error))snapshot.issues.append(error);}
    return snapshot;
}

bool SimulationCatalog::select(CatalogSnapshot &snapshot,DataPack::Kind kind,const QString &id,QString &error) const
{
    if(snapshot.active.size()!=3 || !snapshot.find(kind,id)) {error=QObject::tr("Nie znaleziono paczki tego rodzaju.");return false;}
    auto next=snapshot;next.active[int(kind)]=id;
    if(!saveSelection(next,error))return false;
    snapshot.active=next.active;return true;
}

}
