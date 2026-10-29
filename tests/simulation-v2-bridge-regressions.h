#pragma once

#include "../source/simulation-v2/ProfileBridge.h"
#include "../source/simulation-v2/ProfileMigration.h"

static void simulationV2BridgeRegressions()
{
    QString error;
    auto athlete=AthleteProfile::create();athlete.name="Test";athlete.surname="V2";athlete.overall=90;athlete.form=70;athlete.perks={{0,1,2}};
    auto venue=HillProfile::create();venue.name="Test Hill";venue.perks={{1,1,-2,-3}};
    Jumper jumper;Hill hill;
    check(jumper.setSimulationProfile(athlete,error) && hill.setSimulationProfile(venue,error),"new profiles attach to competition identities");
    check(jumper.getIDStr()==athlete.id && hill.getIDStr()==venue.id,"profile UUIDs are retained");
    const auto athleteJson=Jumper::getJsonObject(jumper),hillJson=Hill::getJsonObject(hill);
    check(Jumper::validateSimulationProfileJson(athleteJson,error) && Hill::validateSimulationProfileJson(hillJson,error),"embedded new profiles validate");
    check(Jumper::getFromJson(athleteJson).getSimulationProfile()->toJson()==athlete.toJson(),"athlete profile round trips through game data");
    check(Hill::getFromJson(hillJson).getSimulationProfile()->toJson()==venue.toJson(),"hill profile round trips through game data");
    HillEditorWidget hillEditor;hillEditor.setHill(&hill);hillEditor.fillHillInputs();
    check(hillEditor.getHillFromWidgetInput().getSimulationProfile().has_value(),"competition configuration cannot silently drop the new hill profile");
    jumper.setName("Renamed");check(jumper.getSimulationProfile()->name=="Renamed","identity edits remain synchronized with profile serialization");
    hill.setHSPoint(140);hill.setRealHSByCharacteristic();check(std::abs(hill.getRealHS()-143.15)<1e-8,"new hill real HS uses new perks and current HS");hill.setHSPoint(venue.hs);
    auto broken=athleteJson;auto wrong=athlete.toJson();wrong["overall"]=0;broken["simulation-v2"]=wrong;
    check(!Jumper::validateSimulationProfileJson(broken,error),"invalid new athlete cannot silently load as legacy");
    broken=athleteJson;wrong=athlete.toJson();wrong["id"]=QUuid::createUuid().toString(QUuid::WithoutBraces);broken["simulation-v2"]=wrong;
    check(!Jumper::validateSimulationProfileJson(broken,error),"mismatched identity is rejected");
    DataPack athletePack,hillPack,decoded;
    const QJsonObject authors{{"format","sj-sim-athlete-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},
        {"name","New athletes"},{"profiles",QJsonArray{athlete.toJson()}}};
    check(DataPack::fromJson(authors,athletePack,error) && athletePack.engineVersion==2 && athletePack.jumpers.size()==1,"ready athlete authoring pack loads into game catalog");
    const QJsonObject hills{{"format","sj-sim-hill-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},
        {"name","New hills"},{"profiles",QJsonArray{venue.toJson()}}};
    check(DataPack::fromJson(hills,hillPack,error) && hillPack.engineVersion==2 && hillPack.hills.size()==1,"ready hill authoring pack loads into game catalog");
    check(athletePack.toJson()["version"]==2 && DataPack::fromJson(athletePack.toJson(),decoded,error)
        && decoded.jumpers.first().getSimulationProfile()->toJson()==athlete.toJson(),"new pack version round trips without reverting to old skills");
    auto damaged=athletePack.toJson();auto entries=damaged["jumpers"].toArray();auto entry=entries[0].toObject();entry.remove("simulation-v2");entries[0]=entry;damaged["jumpers"]=entries;
    check(!DataPack::fromJson(damaged,decoded,error),"new catalog pack requires complete new profiles");
    auto duplicate=authors;duplicate["profiles"]=QJsonArray{athlete.toJson(),athlete.toJson()};check(!DataPack::fromJson(duplicate,decoded,error),"duplicate profile identities rejected");
    DataPack legacy;legacy.name="Old athletes";legacy.jumpers.append(GlobalDatabase::get()->getGlobalJumpers().first());
    const auto sourceJson=legacy.toJson();SimulationV2::PackMigration migrated;
    check(SimulationV2::approximateLegacyPack(legacy,100,migrated,error) && legacy.toJson()==sourceJson,"migration creates a separate pack and leaves the source untouched");
    const auto converted=migrated.pack.jumpers.first().getSimulationProfile();
    check(converted && converted->id!=legacy.jumpers.first().getIDStr() && converted->sourceId==legacy.jumpers.first().getIDStr()
        && converted->perks==std::array<int,3>{{0,0,0}} && migrated.pack.id!=legacy.id,"approximation creates fresh identities, keeps provenance and resets perks");
    check(migrated.warning.contains(QString::fromUtf8("niedokładna, przybliżona")) && !migrated.pack.approximateMigrationSource.isEmpty(),"approximate migration remains clearly identified");
    check(DataPack::fromJson(migrated.pack.toJson(),decoded,error) && decoded.approximateMigrationSource==legacy.id,"migration provenance persists");
    check(!SimulationV2::approximateLegacyPack(athletePack,100,migrated,error),"ready new profiles are never approximated again");
    CompetitionRules rules;rules.setHasDsq(false);rules.setHasJudgesPoints(true);rules.setHasGateCompensations(true);rules.setHasWindCompensations(true);
    JumpSimulator simulator(&jumper,{},&hill);simulator.setCompetitionRules(&rules);simulator.setGate(10);simulator.setCompetitionStartGate(10);
    check(simulator.getSimulationData()!=nullptr,"simulation diagnostics have valid storage before first jump/preview");
    simulator.setSimulationEvent(SimulationV2::EventKind::Qualification);
    simulator.simulateJump();const auto jump=simulator.getJumpData();
    check(jump.getJumper()==&jumper && jump.getHill()==&hill && jump.getSimulator()==&simulator,"new engine result keeps competition object identity");
    check(jump.getSimulationV2Data()["engineVersion"]==2 && jump.getSimulationV2Data()["event"]==int(SimulationV2::EventKind::Qualification),"new profiles route through v2 engine with event category");
    check(jump.getSimulationV2Data()["exposures"].toArray().size()==6 && jump.getDistance()>0,"game result carries compact actual weather history");
    IdentifiableObjectsStorage objects;objects.add(&jumper);objects.add(&hill);
    const auto restored=JumpData::getFromJson(JumpData::getJsonObject(jump),&objects);
    check(restored.getSimulationV2Data()==jump.getSimulationV2Data() && restored.getJumper()==&jumper,"weather history persists with result, independently of seed replay");
    auto reset=restored;reset.setupForNextJumper();check(reset.getSimulationV2Data().isEmpty(),"next jumper cannot inherit historical weather");
    check(!QuickWindProfile::fromMetadata(restored.getSimulationV2Data()).isEmpty() && QuickWindProfile::fromMetadata(restored.getSimulationV2Data())==QuickWindProfile::fromMetadata(jump.getSimulationV2Data()),"recorded wind phases survive JumpData serialization");
    {
        QJsonArray parts;double position=0;
        for(int i=0;i<12;++i) {
            const double length=i%2?3:1;
            parts.append(QJsonObject{{"begin",position},{"end",position+length},{"traversed",i<6?length:0},
                {"before",QJsonArray{i%2?3:1,0}},{"experienced",QJsonArray{2,0}},{"after",QJsonArray{-1,0}}});position+=length;
        }
        QJsonObject metadata{{"engineVersion",2},{"exposures",parts}};const auto profile=QuickWindProfile::fromMetadata(metadata);
        const auto before=profile["before"].toList(),during=profile["during"].toList(),after=profile["after"].toList();
        check(before.size()==6 && before[0].toMap()["head"].toDouble()==2.5 && after[0].toMap()["direction"].toDouble()==180,"wind profile compacts many sections with length weights and preserves direction");
        check(during[0].toMap()["available"].toBool() && !during[5].toMap()["available"].toBool(),"unreached sections cannot masquerade as calm experienced wind");
        auto bad=parts[0].toObject();bad["before"]=QJsonArray{1};parts[0]=bad;metadata["exposures"]=parts;
        check(QuickWindProfile::fromMetadata(metadata).isEmpty() && QuickWindProfile::fromMetadata({}).isEmpty(),"malformed or absent historical wind has no misleading profile view");
    }
    // Preview shares the new model and ignores all obsolete legacy skills.
    const double preview=simulator.previewDistance();jumper.getJumperSkillsPointer()->setTakeoffTechnique(0);jumper.getJumperSkillsPointer()->setFlightTechnique(10000);
    check(std::abs(preview-simulator.previewDistance())<1e-10,"new engine reads new OVR/form, not old techniques");
    SimulationV2::PrecipitationParameters rain;rain.scenario=SimulationV2::PrecipitationScenario::Stable;rain.intensity=8;rain.kind=SimulationV2::PrecipitationKind::Rain;
    const auto weather=std::make_shared<SimulationV2::ContinuousWeather>(7,SimulationV2::WindParameters{},rain);
    const auto measurement=std::make_shared<SimulationV2::SensorWindMeasurement>(std::vector<SimulationV2::WindSensor>{{0,1},{60,1},{120,1}});
    simulator.setSimulationWeather(weather,measurement);simulator.setSimulationTime(100);simulator.simulateJump();
    check(simulator.getJumpData().getSimulationV2Data()["startSeconds"]==100,"game bridge accepts continuous weather with explicit contest time");
    {
        MyRandom::ScopedSeed fixed(713);
        const SimulationV2::Seed high=MyRandom::randomInt(0,0x3fffff),middle=MyRandom::randomInt(0,0x1fffff),low=MyRandom::randomInt(0,0x1fffff);
        const auto expected=(high<<42)|(middle<<21)|low;
        fixed.reseed(713);simulator.simulateJump();
        check(simulator.getJumpData().getSimulationV2Data()["seed"].toString()==QString::number(expected),"seed assembly has explicit cross-compiler draw order");
    }
    JumpManipulator manipulator;manipulator.setDistanceRange({120,120});manipulator.setExactJudges({18,18.5,19,19.5,20});simulator.setManipulator(&manipulator);simulator.simulateJump();
    check(simulator.getJumpData().getDistance()==120 && simulator.getJumpData().getJudges()==manipulator.getExactJudges(),"compatible legacy manipulation maps to new engine overrides");
    JumpManipulator exactWind;simulator.setManipulator(&exactWind);simulator.setSimulationSeed(123456);
    QVector<Wind> forced(6);for(auto &w:forced){w.setDirection(0);w.setStrength(3);}exactWind.setExactWinds(forced);
    const double forcedHead=simulator.previewDistance();
    for(auto &w:forced)w.setDirection(180);exactWind.setExactWinds(forced);const double forcedTail=simulator.previewDistance();
    check(forcedHead>forcedTail,"exact wind overrides an injected field in preview, rather than changing only the recorded winds");
    simulator.simulateJump();const auto forcedJump=simulator.getJumpData();
    check(forcedJump.getAveragedWind()<0 && forcedJump.getSimulationV2Data()["experiencedWind"].toArray()[0].toDouble()<0 && forcedJump.getSimulationV2Data()["exactWindOverride"].toBool(),"exact wind affects flight, measurement and explicit manipulation metadata");
    check(forcedJump.getSimulationV2Data()["precipitationKind"].toInt()==1 && forcedJump.getSimulationV2Data()["precipitation"].toDouble()==8,"exact wind override preserves rain kind and intensity");
    auto zeroMetre=venue;zeroMetre.pointsPerMetre=0;check(hill.setSimulationProfile(zeroMetre,error),"new hill allows explicit zero metre value");simulator.setManipulator(nullptr);simulator.simulateJump();
    check(std::isfinite(simulator.getJumpData().getPoints()),"zero metre value does not block new-model flight");
    jumper.setPersonalBest(550);AthleteProfile largeRecord;
    check(AthleteProfile::fromJson(jumper.getSimulationProfile()->toJson(),largeRecord,error),"record from a valid large custom hill remains serializable");
    venue.record=750;HillProfile largeHillRecord;check(HillProfile::fromJson(venue.toJson(),largeHillRecord,error),"hill record range covers legal engine distances");
    {
        SimulationSave save;check(save.setEngineVersion(2) && !save.setEngineVersion(3),"season engine marker accepts only supported versions");
        auto *savedAthlete=new Jumper(jumper);const auto packId=QUuid::createUuid().toString(QUuid::WithoutBraces);savedAthlete->setSimulationPackContext(packId,"/old-machine/athletes");
        save.setJumpers({savedAthlete});save.setHills({new Hill(hill)});Season season;season.setSeasonNumber(2026);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());
        auto json=SimulationSave::getJsonObject(save);check(json["engine-version"]==2 && !QJsonDocument(json).toJson().contains("/old-machine"),"new season persists engine marker and portable photo identity");
        std::unique_ptr<SimulationSave> restored(SimulationSave::getFromJson(json,nullptr));
        check(restored && restored->getEngineVersion()==2 && restored->getJumpers().first()->getSimulationProfile()->perks==jumper.getSimulationProfile()->perks,"new season round trip preserves complete profiles");
        check(restored->getJumpers().first()->getSimulationAssetDirectory()==QDir(SimulationV2::CatalogPaths::defaults().athletes).filePath(packId),"season reload resolves photo folder for the current machine");
        auto invalid=json;invalid["engine-version"]=3;check(!SimulationSave::getFromJson(invalid,nullptr),"future engine versions cannot silently run on the old engine");
        invalid=json;auto values=invalid["jumpers"].toArray();auto actor=values[0].toObject();actor.remove("simulation-v2");values[0]=actor;invalid["jumpers"]=values;
        check(!SimulationSave::getFromJson(invalid,nullptr),"new-engine season rejects mixed legacy profiles");
    }
    qInfo()<<"PASS: simulation-v2 game bridge, profiles, pack import, identity, weather persistence, preview and manipulation";
}
