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
    hill.setHSPoint(140);hill.setRealHSByCharacteristic();check(std::abs(hill.getRealHS()-144.2)<1e-8,"new hill real HS uses new perks and current HS");hill.setHSPoint(venue.hs);
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
    // Preview shares the new model and ignores all obsolete legacy skills.
    const double preview=simulator.previewDistance();jumper.getJumperSkillsPointer()->setTakeoffTechnique(0);jumper.getJumperSkillsPointer()->setFlightTechnique(10000);
    check(std::abs(preview-simulator.previewDistance())<1e-10,"new engine reads new OVR/form, not old techniques");
    const auto weather=std::make_shared<SimulationV2::ContinuousWeather>(7);
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
    auto zeroMetre=venue;zeroMetre.pointsPerMetre=0;check(hill.setSimulationProfile(zeroMetre,error),"new hill allows explicit zero metre value");simulator.setManipulator(nullptr);simulator.simulateJump();
    check(std::isfinite(simulator.getJumpData().getPoints()),"zero metre value does not block new-model flight");
    jumper.setPersonalBest(550);AthleteProfile largeRecord;
    check(AthleteProfile::fromJson(jumper.getSimulationProfile()->toJson(),largeRecord,error),"record from a valid large custom hill remains serializable");
    venue.record=750;HillProfile largeHillRecord;check(HillProfile::fromJson(venue.toJson(),largeHillRecord,error),"hill record range covers legal engine distances");
    qInfo()<<"PASS: simulation-v2 game bridge, profiles, pack import, identity, weather persistence, preview and manipulation";
}
