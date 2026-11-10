#pragma once

#include "../source/simulation-v2/SimulationCatalog.h"

static void simulationV2CatalogRegressions()
{
    // Load the exact files used by both release packagers, away from user data.
    const QString betaRoot=QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../data/beta");
    QFile betaFile(betaRoot+"/packs.json");
    check(betaFile.open(QIODevice::ReadOnly),"beta pack catalog fixture exists");
    const auto beta=QJsonDocument::fromJson(betaFile.readAll()).object();
    QVector<DataPack> betaSources;QString betaError;
    for(const auto &entry:beta["packs"].toArray()){
        DataPack pack;check(DataPack::fromJson(entry.toObject(),pack,betaError),qPrintable(betaError));betaSources.append(pack);
    }
    check(betaSources.size()==3 && betaSources[0].jumpers.size()==83 && betaSources[1].hills.size()==128
          && betaSources[2].rules.size()==13 && betaSources[2].points.size()==4,"beta contains exactly the selected three packs and current data");
    QTemporaryDir betaStorage;check(betaStorage.isValid(),"isolated packaged beta");
    const auto defaults=SimulationV2::CatalogPaths::defaults();
    check(defaults.athletes.startsWith(qApp->property("portablePacksRoot").toString()),"catalog uses portable installation root");
    const QString seededRoot=betaRoot+"/Packs";
    for(int kind=0;kind<2;++kind){
        const auto expected=beta["packs"].toArray()[kind].toObject();
        const QString file=kind==0?seededRoot+"/Athletes/"+expected["id"].toString()+"/pack.json":seededRoot+"/Hills/"+expected["id"].toString()+".json";
        QFile seed(file);check(seed.open(QIODevice::ReadOnly) && QJsonDocument::fromJson(seed.readAll()).object()==expected,"portable packaged profiles exactly match beta defaults");
    }
    QDirIterator files(seededRoot,QDir::Files,QDirIterator::Subdirectories);
    while(files.hasNext()){
        const QString file=files.next(),relative=QDir(seededRoot).relativeFilePath(file),destination=betaStorage.path()+"/"+relative;
        check(QDir().mkpath(QFileInfo(destination).absolutePath()) && QFile::copy(file,destination),"copy packaged files to isolated fixture");
    }
    SimulationV2::CatalogPaths betaPaths{betaStorage.path()+"/Athletes",betaStorage.path()+"/Hills",betaStorage.path()+"/catalog.json",{}};
    SimulationV2::SimulationCatalog betaCatalog(betaPaths);
    const auto loadedBeta=betaCatalog.load(betaSources,100);
    check(loadedBeta.packs.size()==3 && loadedBeta.selected(DataPack::Jumpers) && loadedBeta.selected(DataPack::Hills) && loadedBeta.selected(DataPack::Rules),"portable beta starts with exactly three selected packs");
    check(loadedBeta.selected(DataPack::Jumpers)->data.jumpers.size()==83 && loadedBeta.selected(DataPack::Hills)->data.hills.size()==128,"portable beta selects current athlete and hill packs");
    check(loadedBeta.active==QStringList({betaSources[0].id,betaSources[1].id,betaSources[2].id}),"portable selection wins without consulting AppData");
    const QString photo=loadedBeta.selected(DataPack::Jumpers)->directory+"/POL_DawidKubacki.png";
    check(QFileInfo::exists(photo),"pack-owned beta photo is bundled beside its pack");
    SimulationV2::ProfilePackRepository portableAthletes(SimulationV2::ProfilePackKind::Athletes,betaPaths.athletes);
    auto edited=portableAthletes.load().packs.first();
    auto betaProfiles=edited["profiles"].toArray();auto first=betaProfiles[0].toObject();
    const int modified=first["overall"].toInt()==99?98:99;first["overall"]=modified;betaProfiles[0]=first;edited["profiles"]=betaProfiles;
    check(portableAthletes.save(edited,betaError),qPrintable(betaError));
    const auto betaReloaded=betaCatalog.load(betaSources,100);
    check(betaReloaded.selected(DataPack::Jumpers)->data.jumpers.first().getSimulationProfile()->overall==modified,"reloading does not overwrite player edits with bundled defaults");

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
