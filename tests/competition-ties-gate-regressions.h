#ifndef COMPETITION_TIES_GATE_REGRESSIONS_H
#define COMPETITION_TIES_GATE_REGRESSIONS_H

#include "../source/competitions/CompetitionInfo.h"
#include "../source/competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../source/competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../source/UI/Competition/CompetitionManagerWindow.h"
#include "../source/seasons/Classification.h"
#include <QApplication>
#include <QInputDialog>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTimer>
#include <functional>

static void competitionTieRegressions(const std::function<void(bool,const QString &)> &verify)
{
    Jumper a("Tie","A","POL"), b("Tie","B","GER"), c("Tie","C","SUI"), d("Tie","D","AUT");
    QVector<Jumper *> athletes{&a,&b,&c,&d};
    CompetitionRules rules;rules.setRounds({RoundInfo(4,true),RoundInfo(3,true),RoundInfo(2,true)});
    CompetitionInfo source;source.setRules(rules);
    auto &standings = source.getResultsReference();
    const auto add = [&](Jumper *jumper,double points) { JumpData jump(jumper);jump.setPoints(points);standings.addJump(jumper,jump); };
    add(&a,100.1);add(&a,.2);add(&b,100.3);add(&b,0);add(&c,100.2);add(&c,0);
    standings.sortInDescendingOrder();
    verify(standings.getResultOfIndividualJumper(&a)->getPosition()==1 && standings.getResultOfIndividualJumper(&b)->getPosition()==1 && standings.getResultOfIndividualJumper(&c)->getPosition()==3,"fractional cumulative scores use displayed tenths for a 1,1,3 tie");
    verify(standings.getJumpersPositions(&athletes)==QVector<int>({1,1,3}),"KO subset positions share the same tie definition as the main table");
    const auto snapshot = standings.getHashWithJumpersPositions(&athletes,2);
    verify(snapshot.value(&a)==1 && snapshot.value(&b)==1 && snapshot.value(&c)==3,"previous-round standings preserve cumulative fractional ties");
    add(&a,50);add(&b,50);add(&c,50);standings.sortInDescendingOrder();
    verify(standings.getResultOfIndividualJumper(&a)->getPosition()==1 && standings.getResultOfIndividualJumper(&b)->getPosition()==1 && standings.getResultOfIndividualJumper(&c)->getPosition()==3,"equal third-round totals retain 1,1,3 ranking");
    standings.sortInAscendingOrder();standings.sortInDescendingOrder();
    verify(standings.getResultOfIndividualJumper(&b)->getPosition()==1,"start-list ordering cannot split a tie when results are restored");

    rules.getRoundsReference()[1].setResetPoints(true);source.setRules(rules);standings.getResultsReference().clear();
    add(&a,100);add(&b,90);add(&c,.3);add(&a,.1+.2);add(&b,.3);
    standings.sortInDescendingOrder();
    verify(standings.getResultOfIndividualJumper(&a)->getPosition()==1 && standings.getResultOfIndividualJumper(&b)->getPosition()==1 && standings.getResultOfIndividualJumper(&c)->getPosition()==3,"reset-round ties exclude eliminated athletes with an equal numeric score");
    const auto tiered = standings.getHashWithJumpersPositions(&athletes);
    verify(tiered.value(&a)==1 && tiered.value(&b)==1 && tiered.value(&c)==3,"subset standings preserve ranking-round separation after a reset");

    rules.getRoundsReference()[1].setResetPoints(false);source.setRules(rules);standings.getResultsReference().clear();
    add(&a,100);add(&b,90);add(&c,90);add(&d,80);standings.sortInDescendingOrder();
    CompetitionInfo target;CompetitionRules targetRules;targetRules.setRounds({RoundInfo(3,true)});target.setRules(targetRules);target.setAdvancementCompetition(&source);
    const auto qualified = IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,athletes);
    verify(qualified==QVector<Jumper *>({&a,&b,&c}),"missing cutoff rank 3 in 1,2,2,4 does not admit the fourth-placed athlete");
    target.getRulesPointer()->getRoundsReference()[0].setCount(2);
    verify(IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,athletes).size()==3,"a genuine tie at the last qualifying rank retains every tied athlete");
    target.getRulesPointer()->getRoundsReference()[0].setKO(true);target.getRulesPointer()->getRoundsReference()[0].setCount(50);
    QVector<Jumper *> limited{&b,&c,&d,&b};
    const auto ko = IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,limited);
    verify(ko.size()==3 && !ko.contains(&a) && ko.count(&b)==1,"KO qualification is bounded by available eligible athletes without duplicates or out-of-range reads");
    target.getRulesPointer()->getRoundsReference()[0].setCount(2);
    verify(IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,athletes).size()==2,"KO target keeps an exact seeded field size across a qualifying tie");
    {
        Jumper ownedA=a,ownedB=b,ownedC=c;QVector<Jumper *> owned{&ownedA,&ownedB,&ownedC};
        const auto remapped=IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,owned);
        verify(remapped.size()==2 && remapped.contains(&ownedA) && !remapped.contains(&a),"qualification resolves logical athlete IDs to the current season instances before KO");
        target.getRulesPointer()->getRoundsReference()[0].setKO(false);
        verify(IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,owned).size()==3,"standard qualification also preserves ties when result instances differ");
        target.getRulesPointer()->getRoundsReference()[0].setKO(true);
    }
    target.getRulesPointer()->getRoundsReference()[0].setKO(false);target.getRulesPointer()->getRoundsReference()[0].setCount(3);
    auto *top = standings.getResultOfIndividualJumper(&a);top->getJumpsReference().last().setDSQ(true);
    verify(!IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,athletes).contains(&a),"a disqualified final qualification attempt cannot advance");
    top->getJumpsReference().last().setDSQ(false);top->getJumpsReference().last().setDNS(true);
    verify(!IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,athletes).contains(&a),"a DNS final qualification attempt cannot advance");
    top->getJumpsReference().last().setDNS(false);
    Hill hill("Protected","POL",100,110);source.setHill(&hill);source.getRulesPointer()->setHas95HSRule(true);
    auto &fall = standings.getResultOfIndividualJumper(&d)->getJumpsReference().last();fall.setLanding(Landing(Landing::Fall));fall.setDistance(104.5);
    verify(IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,athletes).contains(&d),"95-percent-HS protection survives qualification into a later standard competition");

    Classification classification;
    ClassificationSingleResult ca(&classification,&a),cb(&classification,&b),cc(&classification,&c),cd(&classification,&d);
    ca.setPointsSum(100);cb.setPointsSum(90);cc.setPointsSum(90);cd.setPointsSum(80);classification.setResults({&ca,&cb,&cc,&cd});classification.sortInDescendingOrder();
    verify(IndividualCompetitionManager::getFilteredJumpersByClassification(&target,&classification,athletes)==QVector<Jumper *>({&a,&b,&c}),"classification qualification also respects the missing cutoff rank");
    target.getRulesPointer()->getRoundsReference()[0].setKO(true);target.getRulesPointer()->getRoundsReference()[0].setCount(2);
    verify(IndividualCompetitionManager::getFilteredJumpersByClassification(&target,&classification,athletes).size()==2,"classification-seeded KO has a bounded exact field size");
    verify(IndividualCompetitionManager::getFilteredJumpersAfterQualifications(nullptr,athletes).isEmpty() && IndividualCompetitionManager::getFilteredJumpersByClassification(&target,nullptr,athletes).isEmpty(),"missing qualification sources return an empty selection safely");

    Team ta("POL"),tb("GER"),tc("SUI"),td("AUT");
    QVector<Team> teams{ta,tb,tc,td};CompetitionInfo teamSource;
    rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(1);teamSource.setRules(rules);
    for(int index=0;index<teams.size();++index){JumpData jump(athletes[index]);jump.setPoints(index==0?100:index==3?80:90);teamSource.getResultsReference().addJump(&teams[index],jump);}
    targetRules.setCompetitionType(CompetitionRules::Team);targetRules.setJumpersInTeamCount(1);targetRules.setRounds({RoundInfo(3,true)});target.setRules(targetRules);target.setAdvancementCompetition(&teamSource);
    const auto qualifiedTeams=TeamCompetitionManager::getFilteredTeamsAfterQualifications(&target);
    verify(qualifiedTeams.size()==3 && qualifiedTeams.last().getCountryCode()!="AUT","team qualification cannot widen a missing cutoff to the next distinct rank");
    Classification teamClassification;teamClassification.setClassificationType(Classification::Team);
    ClassificationSingleResult cta(&teamClassification,QString("POL")),ctb(&teamClassification,QString("GER")),ctc(&teamClassification,QString("SUI")),ctd(&teamClassification,QString("AUT"));
    cta.setPointsSum(100);ctb.setPointsSum(90);ctc.setPointsSum(90);ctd.setPointsSum(80);teamClassification.setResults({&cta,&ctb,&ctc,&ctd});teamClassification.sortInDescendingOrder();
    verify(TeamCompetitionManager::getFilteredTeamsByClassification(&target,&teamClassification,teams).size()==3,"team classification qualification preserves the cutoff across tied ranks");
    verify(TeamCompetitionManager::getFilteredTeamsAfterQualifications(nullptr).isEmpty(),"missing team qualification context is safe");
    qInfo()<<"PASS: fractional multiround ties, reset tiers, qualification cutoffs and bounded KO fields";
}

static void competitionCoachGateRegressions(CompetitionManagerWindow *window,const std::function<void(bool,const QString &)> &verify)
{
    auto *manager = window->getManager();
    auto *gate = window->findChild<QSpinBox *>("spinBox_actualGate");
    auto *coach = window->findChild<QPushButton *>("pushButton_coachGate");
    verify(gate && coach && gate->isEnabled() && coach->isEnabled(),"jury and coach gate controls remain enabled for the current including last athlete");
    const int originalGate = manager->getActualGate(), originalCoach = manager->getActualCoachGate(), originalDisplay = gate->value();
    const bool hadCoach = manager->getCoachGateForNextJumper();
    const auto originalBaselines = manager->getRoundsStartingGates();
    gate->setValue(qBound(gate->minimum()+4,20,gate->maximum()));
    const int juryGate=manager->getActualGate();
    const auto baselines=manager->getRoundsStartingGates();
    const auto choose=[&](int reduction,bool accept) {
        bool opened=false;QTimer answer;
        QObject::connect(&answer,&QTimer::timeout,[&]{for(auto *widget:QApplication::allWidgets())if(auto *input=qobject_cast<QInputDialog *>(widget);input && input->isVisible()){
            answer.stop();opened=true;input->setIntValue(reduction);if(accept)input->accept();else input->reject();break;
        }});
        answer.start(10);coach->click();answer.stop();verify(opened,"coach gate opens its real numeric dialog for the current including last athlete");
    };
    choose(2,true);
    verify(manager->getActualGate()==juryGate && manager->getActualCoachGate()==juryGate-2 && manager->getCoachGateForNextJumper() && gate->value()==juryGate-2 && manager->getRoundsStartingGates()==baselines,"coach reduction changes only takeoff gate and never the jury or round baseline");
    choose(3,true);
    verify(manager->getActualGate()==juryGate && manager->getActualCoachGate()==juryGate-3 && gate->value()==juryGate-3,"repeated coach adjustment remains relative to the original jury gate");
    choose(1,false);
    verify(manager->getActualCoachGate()==juryGate-3 && gate->value()==juryGate-3 && manager->getRoundsStartingGates()==baselines,"canceling coach gate preserves the existing reduction and baseline");
    choose(juryGate-gate->minimum(),true);
    verify(manager->getActualCoachGate()==gate->minimum(),"coach reduction can reach the lowest legal gate");
    choose(0,true);
    verify(!manager->getCoachGateForNextJumper() && manager->getActualCoachGate()==0 && gate->value()==juryGate,"zero coach reduction restores the jury gate without residual override");
    choose(2,true);gate->setValue(juryGate-1);
    verify(manager->getActualGate()==juryGate-1 && !manager->getCoachGateForNextJumper() && manager->getActualCoachGate()==0,"manual jury gate change clears a previous coach-only reduction");
    manager->setActualGate(originalGate);manager->setActualCoachGate(originalCoach);manager->setCoachGateForNextJumper(hadCoach);manager->setRoundsStartingGates(originalBaselines);
    {const QSignalBlocker blocked(gate);gate->setValue(originalDisplay);}
    window->updateToBeatDistanceLabel();window->updateToAdvanceDistanceLabel();
    qInfo()<<"PASS: coach and jury gate isolation at round"<<manager->getActualRound()<<"start index"<<manager->getActualStartListIndex();
}

#endif
