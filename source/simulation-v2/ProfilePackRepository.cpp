#include "ProfilePackRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLockFile>
#include <QSaveFile>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QUuid>
#include <utility>

namespace SimulationV2 {
namespace {
constexpr qint64 maximumBytes=20*1024*1024;
bool inside(const QString &path,const QString &directory)
{
    const QString base=QFileInfo(directory).canonicalFilePath();
    const QString resolved=QFileInfo(path).canonicalFilePath();
#ifdef Q_OS_WIN
    return !base.isEmpty() && !resolved.isEmpty() && resolved.startsWith(base+"/",Qt::CaseInsensitive);
#else
    return !base.isEmpty() && !resolved.isEmpty() && resolved.startsWith(base+"/",Qt::CaseSensitive);
#endif
}
}

ProfilePackRepository::ProfilePackRepository(ProfilePackKind type,QString directory)
    : kind(type),root(QDir(std::move(directory)).absolutePath())
{}

bool ProfilePackRepository::validateEnvelope(const QJsonObject &pack,QString &error) const
{
    const QString format=kind==ProfilePackKind::Athletes?"sj-sim-athlete-pack":"sj-sim-hill-pack";
    const QString id=pack["id"].toString();
    if(pack["format"]!=format || pack["version"]!=1 || QUuid(id).isNull()
        || QUuid(id).toString(QUuid::WithoutBraces)!=id || !pack["name"].isString()
        || pack["name"].toString().trimmed().isEmpty() || pack["name"].toString().size()>100
        || !pack["profiles"].isArray() || pack["profiles"].toArray().size()>10000) {
        error=QObject::tr("Nieprawidłowy format paczki profili.");return false;
    }
    if(pack.contains("approximate-migration-source") && !pack["approximate-migration-source"].isString()) {
        error=QObject::tr("Nieprawidłowe pochodzenie migrowanej paczki.");return false;
    }
    return true;
}

bool ProfilePackRepository::validate(const QJsonObject &pack,QString &error) const
{
    if(!validateEnvelope(pack,error))return false;
    DataPack parsed;return DataPack::fromJson(pack,parsed,error);
}

QString ProfilePackRepository::packDirectory(const QJsonObject &pack) const
{
    return kind==ProfilePackKind::Athletes?QDir(root).filePath(pack["id"].toString()):root;
}

QString ProfilePackRepository::filePath(const QJsonObject &pack) const
{
    return kind==ProfilePackKind::Athletes?QDir(packDirectory(pack)).filePath("pack.json")
        :QDir(root).filePath(pack["id"].toString()+".json");
}

bool ProfilePackRepository::read(const QString &path,QJsonObject &result,QString &error) const
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly) || file.size()>maximumBytes) {
        error=QObject::tr("Nie można odczytać paczki (limit 20 MB).");return false;
    }
    QJsonParseError parse;const auto document=QJsonDocument::fromJson(file.readAll(),&parse);
    if(parse.error!=QJsonParseError::NoError || !document.isObject()) {
        error=parse.error!=QJsonParseError::NoError?QObject::tr("Nieprawidłowy JSON paczki: %1").arg(parse.errorString())
            :QObject::tr("Plik paczki musi zawierać obiekt JSON.");return false;
    }
    const auto pack=document.object();if(!validate(pack,error))return false;
    result=pack;error.clear();return true;
}

ProfilePackListing ProfilePackRepository::load() const
{
    ProfilePackListing result;QDir directory(root);
    const auto files=kind==ProfilePackKind::Athletes
        ?directory.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name)
        :directory.entryInfoList({"*.json"},QDir::Files,QDir::Name);
    for(const auto &file:files) {
        const QString path=kind==ProfilePackKind::Athletes?QDir(file.absoluteFilePath()).filePath("pack.json"):file.absoluteFilePath();
        QJsonObject pack;QString error;
        if(!read(path,pack,error)) {
            result.issues.append(file.fileName()+": "+error);
            QFile original(path);if(original.open(QIODevice::ReadOnly) && original.size()<=maximumBytes) {
                const auto profiles=QJsonDocument::fromJson(original.readAll()).object()["profiles"].toArray();
                for(auto profile:profiles)if(profile.toObject().contains("perks") && profile.toObject()["perkLevels"].toDouble()!=4)result.needsPerkConversion=true;
            }
            continue;
        }
        const QString expected=kind==ProfilePackKind::Athletes?file.fileName():file.completeBaseName();
        if(expected!=pack["id"].toString()) {result.issues.append(QObject::tr("Niezgodny identyfikator paczki: %1").arg(file.fileName()));continue;}
        result.packs.append(pack);
    }
    return result;
}

bool ProfilePackRepository::save(const QJsonObject &pack,QString &error,bool validateProfiles) const
{
    if(!validateEnvelope(pack,error) || (validateProfiles && !validate(pack,error)))return false;
    const QByteArray bytes=QJsonDocument(pack).toJson(QJsonDocument::Indented);
    if(bytes.size()>maximumBytes) {error=QObject::tr("Paczka przekracza limit 20 MB.");return false;}
    if(!QDir().mkpath(packDirectory(pack))) {error=QObject::tr("Nie można utworzyć folderu paczki.");return false;}
    QSaveFile file(filePath(pack));
    if(!file.open(QIODevice::WriteOnly) || file.write(bytes)!=bytes.size() || !file.commit()) {
        error=QObject::tr("Nie udało się zapisać paczki. Zmiana nie została zastosowana.");return false;
    }
    error.clear();return true;
}

bool ProfilePackRepository::migrate(const DataPack &source,double maximumSkills,const QString &photoRoot,
    QJsonObject &result,QStringList &issues,QString &error) const
{
    if(source.id.isEmpty() || source.engineVersion!=1) {
        error=QObject::tr("Wybierz starą paczkę z identyfikatorem. Nowe profile nie wymagają migracji.");return false;
    }
    if((kind==ProfilePackKind::Athletes && source.kind!=DataPack::Jumpers)
        || (kind==ProfilePackKind::Hills && source.kind!=DataPack::Hills)) {
        error=QObject::tr("Nieprawidłowy rodzaj paczki do migracji.");return false;
    }
    if(!QDir().mkpath(root)) {error=QObject::tr("Nie można utworzyć folderu paczek.");return false;}
    QLockFile lock(QDir(root).filePath(".migration.lock"));
    if(!lock.tryLock(100)) {error=QObject::tr("Trwa migracja paczek w innej instancji aplikacji.");return false;}
    const auto existing=load();
    if(existing.needsPerkConversion){error=QObject::tr("Najpierw skonwertuj istniejące paczki do czterech poziomów perków. Nie utworzono zastępczych kopii.");return false;}
    for(const auto &pack:existing.packs)if(pack["approximate-migration-source"].toString()==source.id) {
        result=pack;issues=existing.issues;error.clear();return true;
    }
    PackMigration migrated;if(!approximateLegacyPack(source,maximumSkills,migrated,error))return false;
    QJsonArray profiles;
    for(const auto &athlete:migrated.pack.jumpers)profiles.append(athlete.getSimulationProfile()->toJson());
    for(const auto &hill:migrated.pack.hills)profiles.append(hill.getSimulationProfile()->toJson());
    QJsonObject pack{{"format",kind==ProfilePackKind::Athletes?"sj-sim-athlete-pack":"sj-sim-hill-pack"},
        {"version",1},{"id",migrated.pack.id},{"name",migrated.pack.name},{"profiles",profiles},
        {"approximate-migration-source",source.id},{"migration-notice",migrated.warning}};
    if(!validate(pack,error))return false;
    QStringList warnings=existing.issues;
    if(kind==ProfilePackKind::Athletes) {
        QTemporaryDir staged(QDir(root).filePath(".migration-XXXXXX"));
        if(!staged.isValid()) {error=QObject::tr("Nie można przygotować folderu migracji.");return false;}
        for(const auto &photo:migrated.photos) {
            const QString original=QDir(photoRoot).filePath(photo.first);
            if(!QFileInfo(original).isFile() || !inside(original,photoRoot)) {
                warnings.append(QObject::tr("Nie przeniesiono zdjęcia %1; profil pozostaje dostępny bez zdjęcia.").arg(QFileInfo(photo.first).fileName()));continue;
            }
            const QString destination=QDir(staged.path()).filePath(photo.second);
            if(!QDir().mkpath(QFileInfo(destination).absolutePath()) || !QFile::copy(original,destination)) {
                error=QObject::tr("Nie udało się skopiować zdjęć. Oryginalna paczka pozostaje bez zmian.");return false;
            }
        }
        const QByteArray bytes=QJsonDocument(pack).toJson(QJsonDocument::Indented);
        QSaveFile file(QDir(staged.path()).filePath("pack.json"));
        if(bytes.size()>maximumBytes || !file.open(QIODevice::WriteOnly) || file.write(bytes)!=bytes.size() || !file.commit()
            || QFileInfo::exists(packDirectory(pack)) || !QDir().rename(staged.path(),packDirectory(pack))) {
            error=QObject::tr("Nie udało się opublikować migrowanej paczki. Oryginał pozostaje bez zmian.");return false;
        }
    } else {
        QTemporaryFile file(QDir(root).filePath(".migration-XXXXXX"));
        const QByteArray bytes=QJsonDocument(pack).toJson(QJsonDocument::Indented);
        if(bytes.size()>maximumBytes || !file.open() || file.write(bytes)!=bytes.size() || !file.flush()) {
            error=QObject::tr("Nie udało się przygotować migrowanej paczki.");return false;
        }
        if(!file.rename(filePath(pack))) {
            error=QObject::tr("Nie udało się opublikować migrowanej paczki. Istniejące pliki pozostają bez zmian.");return false;
        }
        file.setAutoRemove(false);
    }
    result=pack;issues=warnings;error.clear();return true;
}

}
