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
        }
        qInfo()<<"PASS: engine audit case"<<test;
    }
}
