#pragma once

#include "../source/simulation-v2/SimulationCatalog.h"

static void simulationV2CatalogRegressions()
{
    QTemporaryDir storage;check(storage.isValid(),"isolated simulation catalog");
    SimulationV2::CatalogPaths paths{storage.path()+"/athletes",storage.path()+"/hills",storage.path()+"/state/catalog.json",storage.path()+"/old-photos"};
    SimulationV2::ProfilePackRepository athleteStore(SimulationV2::ProfilePackKind::Athletes,paths.athletes);
    auto athlete=AthleteProfile::create();athlete.name="Ready";athlete.overall=88;athlete.photoPath="portrait.png";
    QJsonObject ready{{"format","sj-sim-athlete-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},
        {"name","Ready pack"},{"profiles",QJsonArray{athlete.toJson()}}};
    QString error;check(athleteStore.save(ready,error),"ready authoring data fixture");
    QImage portrait(4,4,QImage::Format_ARGB32);portrait.fill(Qt::blue);
    check(portrait.save(athleteStore.packDirectory(ready)+"/portrait.png"),"ready pack-owned photo fixture");
    DataPack oldAthletes;oldAthletes.name="Legacy athletes";oldAthletes.jumpers.append(GlobalDatabase::get()->getGlobalJumpers().first());oldAthletes.jumpers[0].setImageName({});
    DataPack oldHills;oldHills.kind=DataPack::Hills;oldHills.name="Legacy hills";oldHills.hills.append(GlobalDatabase::get()->getGlobalHills().first());
    DataPack rules;rules.kind=DataPack::Rules;rules.name="Rules";rules.rules.append(GlobalDatabase::get()->getGlobalCompetitionsRules().first());
    const QVector<DataPack> source{oldAthletes,oldHills,rules};const auto originalAthletes=oldAthletes.toJson(),originalHills=oldHills.toJson();
    {
        SimulationV2::ProfilePackRepository unconverted(SimulationV2::ProfilePackKind::Athletes,storage.path()+"/unconverted");
        auto oldPack=ready;auto oldProfile=athlete.toJson();oldProfile.remove("perkLevels");oldProfile["perks"]=QJsonArray{1,2,3};oldPack["profiles"]=QJsonArray{oldProfile};oldPack["approximate-migration-source"]=oldAthletes.id;
        check(QDir().mkpath(unconverted.packDirectory(oldPack)),"old-scale pack directory fixture");QFile original(unconverted.filePath(oldPack));const auto bytes=QJsonDocument(oldPack).toJson();check(original.open(QIODevice::WriteOnly) && original.write(bytes)==bytes.size(),"old-scale source fixture saved");original.close();
        check(unconverted.load().needsPerkConversion,"repository distinguishes unconverted perk data from missing packs");QJsonObject created;QStringList issues;check(!unconverted.migrate(oldAthletes,100,{},created,issues,error),"old-scale authoring packs block replacement migration instead of silently regenerating defaults");
        check(original.open(QIODevice::ReadOnly) && original.readAll()==bytes && QDir(storage.path()+"/unconverted").entryList(QDir::Dirs|QDir::NoDotAndDotDot).size()==1,"conversion requirement leaves edited source bytes and catalog directories intact");
    }
    const auto originalGlobal=GlobalDatabase::get()->packsSnapshot();
    SimulationV2::SimulationCatalog catalog(paths);
    auto snapshot=catalog.load(source,100,{oldAthletes.id,oldHills.id,rules.id});
    check(snapshot.packs.size()==4 && snapshot.selected(DataPack::Jumpers) && snapshot.selected(DataPack::Hills) && snapshot.selected(DataPack::Rules),"catalog combines canonical profiles, separate approximations and compatible rules");
    check(snapshot.selected(DataPack::Jumpers)->data.id==ready["id"].toString(),"ready new athletes take priority over automatically approximated data");
    auto readyAthlete=snapshot.selected(DataPack::Jumpers)->data.jumpers.first();
    check(!readyAthlete.getImagePixmap().isNull() && readyAthlete.getSimulationPackId()==ready["id"].toString(),"game athlete resolves its photo relative to the canonical pack folder");
    const auto portableAthlete=Jumper::getJsonObject(readyAthlete);
    check(portableAthlete["simulation-pack-id"]==ready["id"] && !QJsonDocument(portableAthlete).toJson().contains(storage.path().toUtf8()),"save records pack identity and relative photo path without machine-local directories");
    check(snapshot.selected(DataPack::Hills)->data.engineVersion==2 && snapshot.selected(DataPack::Hills)->data.approximateMigrationSource==oldHills.id,"legacy hill selection maps to its separate new pack");
    check(snapshot.selected(DataPack::Rules)->data.id==rules.id,"compatible rules retain their identity");
    check(oldAthletes.toJson()==originalAthletes && oldHills.toJson()==originalHills && GlobalDatabase::get()->packsSnapshot()==originalGlobal,"catalog preparation never mutates source packs or global game state");
    QString migratedId;QStringList originalOrder;
    for(const auto &pack:snapshot.packs) {originalOrder.append(pack.data.id);if(pack.data.approximateMigrationSource==oldAthletes.id)migratedId=pack.data.id;}
    check(!migratedId.isEmpty() && catalog.select(snapshot,DataPack::Jumpers,migratedId,error),"explicitly selecting a migrated pack is supported");
    auto reloaded=catalog.load(source,100);QStringList reloadedOrder;for(const auto &pack:reloaded.packs)reloadedOrder.append(pack.data.id);
    check(reloaded.packs.size()==4 && reloaded.active[0]==migratedId && originalOrder==reloadedOrder,"restart preserves selection/order without repeated migrations");
    check(!catalog.select(reloaded,DataPack::Hills,ready["id"].toString(),error),"cannot choose an athlete pack as hills");
    auto changed=ready;auto profiles=changed["profiles"].toArray();auto profile=profiles[0].toObject();profile["overall"]=95;profiles[0]=profile;changed["profiles"]=profiles;
    check(athleteStore.save(changed,error),"canonical authoring edit while catalog snapshot exists");
    const auto refreshed=catalog.load(source,100);
    check(refreshed.find(DataPack::Jumpers,ready["id"].toString())->data.jumpers.first().getSimulationProfile()->overall==95
        && snapshot.find(DataPack::Jumpers,ready["id"].toString())->data.jumpers.first().getSimulationProfile()->overall==88,
        "new setup reads current canonical edits while an existing session snapshot stays isolated");
    QFile state(paths.selection);check(state.open(QIODevice::ReadOnly),"selection metadata exists");const auto metadata=QJsonDocument::fromJson(state.readAll()).object();state.close();
    check(!metadata.contains("packs") && !metadata.contains("profiles") && metadata["active"].toArray().size()==3,"selection file contains no second editable copy of profile data");
    check(state.open(QIODevice::WriteOnly|QIODevice::Truncate),"corrupt selection fixture");state.write("{broken");state.close();
    auto recoverable=catalog.load(source,100);
    check(!recoverable.selectionWritable && recoverable.packs.size()==4 && !recoverable.issues.isEmpty(),"corrupt preferences do not hide playable canonical packs");
    check(!catalog.select(recoverable,DataPack::Jumpers,ready["id"].toString(),error),"corrupt preference source is protected from overwrite");
    check(state.open(QIODevice::ReadOnly) && state.readAll()=="{broken","corrupt index is preserved unchanged");state.close();
    auto importedProfile=AthleteProfile::create();importedProfile.name="Imported ready";importedProfile.overall=93;
    auto importedObject=ready;importedObject["id"]=QUuid::createUuid().toString(QUuid::WithoutBraces);importedObject["name"]="Imported ready pack";importedObject["profiles"]=QJsonArray{importedProfile.toJson()};
    DataPack imported;check(DataPack::fromJson(importedObject,imported,error),"already-new imported catalog fixture");
    auto extended=source;extended.append(imported);auto adopted=catalog.load(extended,100);
    check(adopted.packs.size()==5 && adopted.find(DataPack::Jumpers,imported.id)->data.jumpers.first().getSimulationProfile()->overall==93,
        "already-new source pack is preserved without approximate conversion");
    check(catalog.load(extended,100).packs.size()==5,"already-new imported pack is adopted only once");
    DataPack brokenReady;brokenReady.engineVersion=2;brokenReady.name="Incomplete new pack";brokenReady.jumpers.append(Jumper("Old","entry","POL"));extended.append(brokenReady);
    auto incomplete=catalog.load(extended,100);check(!incomplete.find(DataPack::Jumpers,brokenReady.id) && !incomplete.issues.isEmpty(),"incomplete new source cannot silently fall back to old skills");
    qInfo()<<"PASS: canonical simulation catalog, ready-pack preference, migration, stable selection, fresh snapshots and protected recovery";
}
