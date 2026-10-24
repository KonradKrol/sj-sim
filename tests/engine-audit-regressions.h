#pragma once
#include <cfenv>
#include "../source/utilities/functions.h"
#include <stdexcept>

static void engineAuditRegressions(int selected=0) {
    auto jumper=GlobalDatabase::get()->getEditableGlobalJumpers().first();auto hill=GlobalDatabase::get()->getEditableGlobalHills().first();hill.setRecord(0);hill.setPointsForGate(5);
    CompetitionRules rules;rules.setHasDsq(false);rules.setHasJudgesPoints(true);rules.setHasGateCompensations(true);rules.setHasWindCompensations(false);
    JumpManipulator manipulation;manipulation.setExactJudges({18,18,18,18,18});manipulation.setDistanceRange({100,100});
    JumpSimulator engine(&jumper,{},&hill);engine.setCompetitionRules(&rules);engine.setManipulator(&manipulation);engine.setGate(10);engine.setCompetitionStartGate(10);engine.setDSQBaseProbability(100);engine.setJumpsImportance(5);
    for(int test=1;test<=10;++test) {
        if(selected && selected!=test)continue;
        if(test==1) {
            JumpData jump(&jumper,&hill);jump.setDNS(true);jump.setHasCoachGate(true);jump.setCoachGate(12);jump.setBeats95HSPercents(true);jump.setJumperForm(88);jump.reset();
            check(!jump.getDNS() && !jump.getHasCoachGate() && jump.getCoachGate()==0 && !jump.getBeats95HSPercents() && jump.getJumperForm()==0,"reset cannot retain DNS, coach gate, protected distance or former athlete's form");
            JumpData fresh(&jumper,&hill);check(fresh.getPoints()==0 && fresh.getDistance()==0 && fresh.getSimulator()==nullptr && fresh.getSingleResult()==nullptr && fresh.getJumper()==&jumper,"new jump data starts with initialized scores and pointers while retaining constructor identity");
        } else if(test==2) {
            JumpSimulator empty(&jumper,{},&hill);empty.setJumper(nullptr);check(empty.getJumperSkills()==nullptr,"clearing the athlete clears the cached skills pointer");
            bool rejected=false;try{empty.simulateJump();}catch(const std::invalid_argument &){rejected=true;}check(rejected,"incomplete simulation context fails explicitly instead of dereferencing null pointers");
            engine.setManipulator(nullptr);engine.simulateJump();check(std::isfinite(engine.getJumpData().getPoints()) && engine.getManipulator()==nullptr,"missing optional manipulation uses a temporary default without a dangling pointer");engine.setManipulator(&manipulation);
        } else if(test==3) {
            rules.setHasDsq(true);jumper.getJumperSkillsPointer()->insertCharacteristic("dsq-probability",-7);std::feclearexcept(FE_ALL_EXCEPT);engine.simulateJump();
            check(!(std::fetestexcept(FE_DIVBYZERO|FE_INVALID)) && !engine.getJumpData().getDSQ(),"DSQ protection reaching zero cannot divide by zero or produce a forced disqualification");rules.setHasDsq(false);
        } else if(test==4) {
            auto *settings=GlobalSimulationSettings::get();const auto scale=settings->getMaxSkills();settings->setMaxSkills(100);hill.setCharacteristics({});jumper.getJumperSkillsPointer()->setCharacteristics({});jumper.getJumperSkillsPointer()->setFlightStyle(JumperSkills::ModernVStyle);jumper.getJumperSkillsPointer()->setForm(50);jumper.getJumperSkillsPointer()->setFlightTechnique(50);
            jumper.getJumperSkillsPointer()->insertCharacteristic("flight-height",Hill::calculateBestFlightHeightLevel(&hill));engine.previewDistance();const double rating=engine.getSimulationData()->getFlightRating();settings->setMaxSkills(scale);
            check(std::abs(rating-50)<1e-8,"the hill's ideal flight height has zero mismatch penalty");
        } else if(test==5) {
            manipulation.setExactJudges({19.5});check(manipulation.getExactJudges().size()==5 && manipulation.getExactJudges()[1]==-1,"short judge override is padded with unspecified marks");engine.simulateJump();check(engine.getJumpData().getJudges()[0]==19.5,"a single exact judge override remains usable");manipulation.setExactJudges({25,-2,18,18,18});check(manipulation.getExactJudges()[0]==20 && manipulation.getExactJudges()[1]==-1,"invalid exact marks cannot exceed scoring bounds");
        } else if(test==6) {
            manipulation.setDistanceRange({10,10});engine.setHasCoachGate(true);engine.setCoachGate(12);engine.simulateJump();
            check(engine.getJumpData().getGateCompensation()==roundDoubleToOnePlace(-2*hill.getPointsForGate()),"a higher coach gate always incurs its negative compensation even below 95 percent HS");engine.setHasCoachGate(false);
        } else if(test==7) {
            hill.setAutoPointsForKPoint(true);hill.setAutoPointsForMeter(false);hill.setPointsForMeter(2.3);auto json=Hill::getJsonObject(hill);check(json["points-for-meter"].isDouble() && Hill::getFromJson(json).getPointsForMeter()==2.3,"custom points per metre survive automatic K-point serialization");hill.setAutoPointsForKPoint(false);hill.setAutoPointsForMeter(true);json=Hill::getJsonObject(hill);check(json["points-for-meter"]=="auto","automatic metre scoring remains automatic when K-point scoring is custom");
        } else if(test==8) {
            check(Hill::calculatePointsForMeter(169.5)==1.8 && Hill::calculatePointsForMeter(170)==1.2,"fractional K points between 169 and 170 have a nonzero large-hill scoring factor");
        } else if(test==9) {
            Wind wind;wind.setDirection(-450);check(wind.getDirection()==270 && wind.getDirectionType()==Wind::Side,"negative multi-turn wind angles normalize before physical classification");wind.setDirection(810);check(wind.getDirection()==90,"positive multi-turn wind angles wrap consistently");
        } else if(test==10) {
            check(MyRandom::normalDistributionRandomHalf(2,0,MyRandom::Negative)==0,"impossible negative half-normal with zero deviation returns a boundary instead of hanging");check(MyRandom::normalDistributionRandomHalf(-2,0,MyRandom::Positive)==0,"impossible positive half-normal with zero deviation returns a boundary instead of hanging");
            const double rare=MyRandom::normalDistributionRandomHalf(-1000,.1,MyRandom::Positive);check(std::isfinite(rare) && rare>=0,"extreme opposite tails complete with a finite sample");
            double total=0;for(int sample=0;sample<20000;++sample)total+=MyRandom::normalDistributionRandomHalf(-4,1,MyRandom::Positive);
            const double expected=std::exp(-8)/std::sqrt(2*std::acos(-1.))/(.5*std::erfc(4/std::sqrt(2.)))-4;
            check(std::abs(total/20000-expected)<.02,"rare-tail sampler preserves the conditional normal mean");
        }
        qInfo()<<"PASS: engine audit case"<<test;
    }
}

static void engineAsyncRegression() {
    auto *db=GlobalDatabase::get();SingleJumpsManager manager;manager.setJumper(db->getEditableGlobalJumpers().first());manager.setHill(db->getEditableGlobalHills().first());manager.setGate(15);manager.setImportance(5);manager.setChangeableWind(true);manager.getRules().setHasDsq(false);
    manager.setWindsGeneratorSettings(QVector<WindGenerationSettings>(6,WindGenerationSettings(0,2,1,2)));
    auto control=std::make_shared<SingleJumpsManager::BatchControl>();
    const int scale=GlobalSimulationSettings::get()->getMaxSkills();auto isolated=manager.simulateAsync(2000,4,control);GlobalSimulationSettings::get()->setMaxSkills(0);isolated.waitForFinished();GlobalSimulationSettings::get()->setMaxSkills(scale);const auto values=isolated.result();
    check(values.size()==2000 && control->completed==2000,"parallel batch returns the requested number of completed jumps");
    QSet<QString> identities;for(const auto &jump:values){check(std::isfinite(jump.getPoints()) && std::isfinite(jump.getDistance()) && jump.getJumper()==nullptr && jump.getHill()==nullptr && jump.getSimulator()==nullptr && jump.getWinds().size()==6,"worker settings and wind histories are isolated and results contain no dangling worker pointers");identities.insert(jump.getIDStr());}
    check(!JumpData::getJsonObject(values.first()).isEmpty(),"detached worker snapshots can be serialized without context pointers");
    check(identities.size()==values.size(),"parallel jump identifiers remain unique");
    control=std::make_shared<SingleJumpsManager::BatchControl>();control->canceled=true;auto canceled=manager.simulateAsync(2000,4,control);canceled.waitForFinished();check(canceled.result().isEmpty(),"pre-canceled async batches do not perform simulation work");
    QFuture<QVector<JumpData>> outliving;
    {auto owner=std::make_unique<SingleJumpsManager>();owner->setJumper(manager.getJumper());owner->setHill(manager.getHill());owner->setGate(15);owner->getRules().setHasDsq(false);outliving=owner->simulateAsync(128,2);}
    outliving.waitForFinished();check(outliving.result().size()==128,"asynchronous snapshots outlive the manager that requested them");
    QVector<qint64> serial,parallel;constexpr int count=20000;
    for(int iteration=0;iteration<3;++iteration)for(int workers:{1,4}) {
        QElapsedTimer timer;timer.start();auto job=manager.simulateAsync(count,workers);job.waitForFinished();auto results=job.result();const auto elapsed=timer.elapsed();check(results.size()==count,"benchmark preserves complete results");(workers==1?serial:parallel).append(elapsed);
    }
    std::sort(serial.begin(),serial.end());std::sort(parallel.begin(),parallel.end());
    qInfo()<<"BENCH isolated engine"<<count<<"jumps serial median ms"<<serial[1]<<"parallel median ms"<<parallel[1]<<"speedup"<<double(serial[1])/qMax(qint64(1),parallel[1]);
    qInfo()<<"PASS: asynchronous engine snapshot isolation, cancellation, stable output and unique identities";
}
