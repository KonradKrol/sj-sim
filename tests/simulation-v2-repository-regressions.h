#pragma once

#include "../source/simulation-v2/ProfilePackRepository.h"
#include "../source/athletes/AthleteStudioController.h"
#include "../source/hills/HillStudioController.h"

static void simulationV2RepositoryRegressions()
{
    using SimulationV2::ProfilePackKind;
    using SimulationV2::ProfilePackRepository;
    QTemporaryDir storage;check(storage.isValid(),"isolated profile repository");
    const QString photos=storage.path()+"/legacy-photos";check(QDir().mkpath(photos),"legacy photo fixture");
    QImage picture(4,4,QImage::Format_ARGB32);picture.fill(Qt::blue);check(picture.save(photos+"/shared.png"),"source photo fixture");
    check(picture.save(storage.path()+"/private.png"),"photo outside permitted source root");
    DataPack source;source.kind=DataPack::Jumpers;source.name="Legacy pack";
    for(int i=0;i<4;++i) {Jumper athlete("Athlete",QString::number(i),"POL");athlete.setImageName(i<2?"shared.png":i==2?"missing.png":"../private.png");source.jumpers.append(athlete);}
    const auto original=source.toJson();
    ProfilePackRepository athletes(ProfilePackKind::Athletes,storage.path()+"/athletes");
    QJsonObject migrated;QString error;QStringList issues;
    check(athletes.migrate(source,100,photos,migrated,issues,error),QString("legacy repository migration: %1").arg(error));
    check(source.toJson()==original && QFileInfo::exists(photos+"/shared.png"),"migration leaves source data and photos untouched");
    check(issues.size()==2,"missing and outside-root photos are reported without blocking profiles");
    const auto profiles=migrated["profiles"].toArray();check(profiles.size()==4,"all athletes remain available with optional photos");
    const QString first=profiles[0].toObject()["photoPath"].toString(),second=profiles[1].toObject()["photoPath"].toString();
    check(first!=second && QFileInfo::exists(athletes.packDirectory(migrated)+"/"+first)
        && QFileInfo::exists(athletes.packDirectory(migrated)+"/"+second),"same source filenames get separate pack-owned photo paths");
    check(!QFileInfo::exists(athletes.packDirectory(migrated)+"/"+profiles[2].toObject()["photoPath"].toString())
        && !QFileInfo::exists(athletes.packDirectory(migrated)+"/"+profiles[3].toObject()["photoPath"].toString()),"no missing or outside-root file is copied");
    QJsonObject read;check(athletes.read(athletes.filePath(migrated),read,error) && read==migrated,"published migrated folder is fully readable");
    check(athletes.load().packs.size()==1,"repository lists the published pack once");
    auto edited=migrated;edited["name"]="Edited new pack";
    check(athletes.save(edited,error),"canonical repository saves authoring edits");
    QJsonObject again;check(athletes.migrate(source,100,photos,again,issues,error) && again==edited
        && athletes.load().packs.size()==1,"repeated migration keeps existing edited new pack without duplicates or overwrites");
    DataPack gamePack;check(DataPack::fromJson(again,gamePack,error) && gamePack.approximateMigrationSource==source.id,"catalog reads provenance from canonical authoring data");
    auto invalid=edited;invalid["id"]="../escape";
    check(!athletes.save(invalid,error,false) && !QFileInfo::exists(storage.path()+"/escape/pack.json"),"even trusted fast writes reject a non-UUID path");
    invalid=edited;invalid["name"]="";check(!athletes.save(invalid,error),"empty pack names cannot make an unreadable saved pack");
    check(athletes.read(athletes.filePath(edited),read,error) && read==edited,"rejected edit preserves saved bytes");
    check(QDir().mkpath(storage.path()+"/athletes/broken"),"corrupt pack fixture directory");
    QFile broken(storage.path()+"/athletes/broken/pack.json");check(broken.open(QIODevice::WriteOnly),"corrupt pack fixture");broken.write("{broken");broken.close();
    const auto available=athletes.load();check(available.packs.size()==1 && available.issues.size()==1,"one corrupt file does not hide valid packs");
    check(broken.open(QIODevice::ReadOnly) && broken.readAll()=="{broken","corrupt source is reported and never overwritten");broken.close();
    DataPack hillSource;hillSource.kind=DataPack::Hills;hillSource.name="Legacy hills";hillSource.hills.append(GlobalDatabase::get()->getGlobalHills().first());
    const auto originalHills=hillSource.toJson();
    ProfilePackRepository hills(ProfilePackKind::Hills,storage.path()+"/hills");QJsonObject migratedHills;
    check(hills.migrate(hillSource,100,{},migratedHills,issues,error) && hillSource.toJson()==originalHills,QString("hill migration publishes a new file without touching source: %1").arg(error));
    check(hills.load().packs.size()==1 && hills.read(hills.filePath(migratedHills),read,error),"migrated hill file is readable by canonical repository");
    auto preservedHill=migratedHills;preservedHill["name"]="Edited hill pack";check(hills.save(preservedHill,error) && hills.migrate(hillSource,100,{},migratedHills,issues,error) && migratedHills==preservedHill && hills.load().packs.size()==1,"repeated hill migration preserves published edits without duplicate files");
    check(!hills.migrate(source,100,photos,again,issues,error),"repository rejects wrong source kind");
    AthleteStudioController athleteEditor(nullptr,storage.path()+"/athletes");
    check(athleteEditor.profiles().size()==4 && athleteEditor.profile()["photoAvailable"].toBool(),"studio opens canonical migrated athletes with pack-owned photos");
    athleteEditor.setValue("name","Edited in studio");
    check(athletes.read(athletes.filePath(edited),read,error) && read["profiles"].toArray()[0].toObject()["name"]=="Edited in studio"
        && read["approximate-migration-source"]==source.id,"studio and future game catalog read the same edited profile file");
    athleteEditor.renamePack("");
    check(athletes.read(athletes.filePath(edited),read,error) && read["name"]=="Edited new pack","invalid inline pack rename preserves the readable saved pack");
    HillStudioController hillEditor(nullptr,storage.path()+"/hills");hillEditor.setPerk(3,1,3,false);
    check(hills.read(hills.filePath(migratedHills),read,error) && read["profiles"].toArray()[0].toObject()["perks"].toArray()[3]==3,
        "hill studio writes new wind perk through canonical repository");
    qInfo()<<"PASS: canonical profile repository, atomic migration, idempotency, photos, validation and corrupt-file isolation";
}
