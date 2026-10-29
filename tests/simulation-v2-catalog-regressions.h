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
        const auto loaded=unconverted.load();
        check(loaded.packs.size()==1 && !loaded.needsPerkConversion && loaded.issues.isEmpty(),"old-scale authoring packs convert automatically instead of disappearing");
        const auto converted=loaded.packs.first()["profiles"].toArray().first().toObject();
        check(converted["perks"].toArray()==QJsonArray({1,2,4}) && converted["perkLevels"]==4 && converted["overall"]==oldProfile["overall"] && converted["photoPath"]==oldProfile["photoPath"],"automatic conversion maps 1/2/3 to 1/2/4 and preserves ratings and photo paths");
        QFile backup(original.fileName()+".three-levels.bak");check(backup.open(QIODevice::ReadOnly) && backup.readAll()==bytes,"automatic pack conversion preserves an exact original backup");backup.close();
        QJsonObject created;QStringList issues;check(unconverted.migrate(oldAthletes,100,{},created,issues,error) && created["id"]==oldPack["id"],"migration reuses the converted edited pack instead of generating replacement defaults");
        check(original.open(QIODevice::ReadOnly),"converted pack can be read");const auto savedBytes=original.readAll();original.close();
        check(unconverted.load().packs.size()==1 && original.open(QIODevice::ReadOnly) && original.readAll()==savedBytes,"second load leaves four-level data byte-for-byte unchanged");original.close();
        check(QDir(storage.path()+"/unconverted").entryList(QDir::Dirs|QDir::NoDotAndDotDot).size()==1,"conversion preserves canonical pack identity and folder");
        auto negative=oldProfile;negative["perks"]=QJsonArray{-1,-2,-3};auto current=athlete.toJson();current["id"]=QUuid::createUuid().toString(QUuid::WithoutBraces);current["perks"]=QJsonArray{3,-3,4};negative["id"]=QUuid::createUuid().toString(QUuid::WithoutBraces);
        auto mixed=oldPack;mixed["profiles"]=QJsonArray{negative,current};const auto mixedBytes=QJsonDocument(mixed).toJson();
        check(original.open(QIODevice::WriteOnly|QIODevice::Truncate) && original.write(mixedBytes)==mixedBytes.size(),"mixed-scale fixture");original.close();
        const auto mixedLoaded=unconverted.load();const auto mixedProfiles=mixedLoaded.packs.first()["profiles"].toArray();
        check(mixedProfiles[0].toObject()["perks"].toArray()==QJsonArray({-1,-2,-4}) && mixedProfiles[1].toObject()==current,"negative perks convert while current level three stays level three");
        check(backup.open(QIODevice::ReadOnly) && backup.readAll()==bytes,"different later source never overwrites the first backup");backup.close();
        check(QDir(unconverted.packDirectory(oldPack)).entryList({"*.bak"},QDir::Files).size()==2,"a different old source receives a separate backup");
        QFile exported(storage.path()+"/export.json");check(exported.open(QIODevice::WriteOnly) && exported.write(bytes)==bytes.size(),"old exported pack fixture");exported.close();QJsonObject imported;
        check(unconverted.read(exported.fileName(),imported,error) && imported["profiles"].toArray()[0].toObject()["perks"].toArray()==QJsonArray({1,2,4}),"import accepts an old exported pack without manual scripts");
        check(exported.open(QIODevice::ReadOnly) && exported.readAll()==bytes && !QFileInfo::exists(exported.fileName()+".three-levels.bak"),"external export is never rewritten by importing it");exported.close();
        auto broken=oldPack;auto bad=oldProfile;bad["perks"]=QJsonArray{1,2,4};broken["profiles"]=QJsonArray{bad};const auto brokenBytes=QJsonDocument(broken).toJson();
        check(original.open(QIODevice::WriteOnly|QIODevice::Truncate) && original.write(brokenBytes)==brokenBytes.size(),"invalid old pack fixture");original.close();
        check(!unconverted.read(original.fileName(),imported,error) && original.open(QIODevice::ReadOnly) && original.readAll()==brokenBytes,"invalid old data is rejected without overwriting the source");original.close();
        bad=oldProfile;bad["perkLevels"]=5;broken["profiles"]=QJsonArray{bad};const auto futureBytes=QJsonDocument(broken).toJson();check(original.open(QIODevice::WriteOnly|QIODevice::Truncate) && original.write(futureBytes)==futureBytes.size(),"future-scale fixture");original.close();
        check(!unconverted.read(original.fileName(),imported,error),"future perk scales are never guessed or downgraded");
        SimulationV2::ProfilePackRepository hills(SimulationV2::ProfilePackKind::Hills,storage.path()+"/old-hills");auto h=HillProfile::create().toJson();h.remove("perkLevels");h["perks"]=QJsonArray{-3,2,1};
        auto hp=oldPack;hp["format"]="sj-sim-hill-pack";hp["profiles"]=QJsonArray{h};QDir().mkpath(storage.path()+"/old-hills");QFile hf(hills.filePath(hp));const auto hb=QJsonDocument(hp).toJson();check(hf.open(QIODevice::WriteOnly) && hf.write(hb)==hb.size(),"old hill pack fixture");hf.close();
        const auto hl=hills.load();check(hl.packs.size()==1 && hl.packs[0]["profiles"].toArray()[0].toObject()["perks"].toArray()==QJsonArray({-4,2,1,0}),"hill packs automatically convert signs, strengths and the optional fourth category");

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
