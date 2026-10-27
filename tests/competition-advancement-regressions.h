#pragma once
#include "../source/competitions/CompetitionAdvancement.h"
#include "../source/UI/Competition/Results/ResultsTableModel.h"
#include "../source/UI/Competition/Results/KOGroupResultsTableModel.h"

static void competitionAdvancementRegressions() {
    using namespace CompetitionAdvancement;
    constexpr int yes=StartListCompetitorStatus::SureAdvanced,no=StartListCompetitorStatus::SureDroppedOut,wait=StartListCompetitorStatus::Waiting;
    int keys[5];
    for(int pattern=0;pattern<81;++pattern)for(int cutoff=1;cutoff<=4;++cutoff)for(bool ties:{false,true}) {
        QVector<Entry> field;int digits=pattern;
        for(int i=0;i<4;++i){Entry entry;entry.competitor=&keys[i];entry.points=digits%3;digits/=3;entry.order=i;entry.pending=i>=2;field.append(entry);}
        for(int candidate=0;candidate<2;++candidate) {
            bool anyIn=false,anyOut=false;
            for(int a=-1;a<=3;++a)for(int b=-1;b<=3;++b) {
                auto future=field;future[2].pending=future[3].pending=false;future[2].points=a;future[3].points=b;future[2].excluded=a<0;future[3].excluded=b<0;
                int position=1;for(int i=0;i<4;++i)if(i!=candidate && !future[i].excluded && better(future[i],future[candidate],ties))++position;
                if(position<=cutoff)anyIn=true;else anyOut=true;
            }
            const int expected=anyIn?(anyOut?wait:yes):no;
            check(status(field[candidate],field,cutoff,ties)==expected,"certainty matches every possible future result, tie and DNS outcome");
        }
    }
    Jumper a("A","Leader","POL"),b("B","Tie","GER"),c("C","Tie","SUI"),d("D","Out","AUT"),e("E","Pending","NOR");
    QVector<Jumper *> athletes{&a,&b,&c,&d,&e};
    Hill hill;hill.setKPoint(90);hill.setHSPoint(100);
    CompetitionInfo source(&hill),nearTarget(&hill),farTarget(&hill);CompetitionRules rules;rules.setRounds({RoundInfo(5,true),RoundInfo(3,true)});source.setRules(rules);
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&source);manager.setCompetitionRules(source.getRulesPointer());manager.setResults(&source.getResultsReference());manager.setActualRound(1);manager.setFirstRoundJumpers(athletes);manager.setupStartListStatusesForActualRound();
    const auto finish=[&](Jumper *jumper,double points,int jumpStatus=StartListCompetitorStatus::Finished){JumpData jump(jumper,&hill);jump.setPoints(points);jump.setDSQ(jumpStatus==StartListCompetitorStatus::Dsq);jump.setDNS(jumpStatus==StartListCompetitorStatus::Dns);source.getResultsReference().addJump(jumper,jump);StartListCompetitorStatus::getStatusOfJumper(jumper,manager.getStartListStatusesReference())->setJumpStatus(jumpStatus);manager.updateCompetitorsAdvanceStatuses();};
    finish(&a,100);finish(&b,90);finish(&c,90);finish(&d,80);
    check(StartListCompetitorStatus::getStatusOfJumper(&d,manager.getStartListStatusesReference())->getAdvanceStatus()==no,"fourth place is already eliminated before the final rival jumps even though the cutoff rank is skipped by a tie");
    check(StartListCompetitorStatus::getStatusOfJumper(&b,manager.getStartListStatusesReference())->getAdvanceStatus()==yes && StartListCompetitorStatus::getStatusOfJumper(&c,manager.getStartListStatusesReference())->getAdvanceStatus()==yes,"both tied competitors are safe when only one rival remains");
    finish(&e,90);
    const auto qualified=manager.getFilteredJumpersForNextRound();check(qualified.size()==4 && !qualified.contains(&d),"three-way boundary tie expands the next round without admitting the next distinct rank");
    // Qualification quotas are independent of the next-round quota and use calendar order.
    CompetitionRules target;target.setRounds({RoundInfo(2,true)});nearTarget.setRules(target);nearTarget.setAdvancementCompetition(&source);target.getRoundsReference()[0].setCount(4);farTarget.setRules(target);farTarget.setAdvancementCompetition(&source);
    SeasonCalendar calendar;calendar.setCompetitions({&source,&nearTarget,&farTarget});source.updateQualifyingCompetitions(&calendar);
    source.setExceptionalRoundsCount(1);manager.updateCompetitorsAdvanceStatuses();
    check(manager.nearestQualificationTarget()==&nearTarget && manager.qualificationLimit()==2,"nearest dependent contest determines qualification, independent of a farther quota");
    check(manager.qualificationStatus(&b)==yes && manager.qualificationStatus(&c)==yes && manager.qualificationStatus(&d)==no,"external qualifying ranks include ties without promoting the next distinct place");
    ResultsTableModel model(CompetitionRules::Individual,manager.getResults(),&manager);model.setStartListStatuses(&manager.getStartListStatusesReference());
    const int row=manager.getResults()->getIndexOfJumperResult(&b);check(model.index(row,0).data(RoundStatusRole).toInt()==-1 && model.index(row,0).data(QualificationStatusRole).toInt()==yes,"one-round qualification exposes a neutral Q state without a fictitious next-series advance");
    const auto before=manager.getResults()->getResultsReference();for(int i=0;i<model.rowCount();++i){model.index(i,0).data(Qt::BackgroundRole);model.index(i,0).data(Qt::ForegroundRole);}for(int i=0;i<before.size();++i)check(before[i].getJumper()==manager.getResults()->getResultsReference()[i].getJumper(),"reading result presentation cannot reorder standings");
    nearTarget.setCancelled(true);manager.updateCompetitorsAdvanceStatuses();check(manager.nearestQualificationTarget()==&farTarget && manager.qualificationLimit()==4,"canceled nearest target is skipped");
    farTarget.setCancelled(true);manager.updateCompetitorsAdvanceStatuses();check(manager.qualificationStatus(&a)==-1,"no stale Q remains when all dependent competitions are canceled");
    source.setQualifyingCompetitions({});source.getRulesPointer()->setRounds({RoundInfo(0,false),RoundInfo(2,true)});source.setExceptionalRoundsCount(1);manager.setQualificationFixedLimit(true);manager.updateCompetitorsAdvanceStatuses();
    int exact=0;for(const auto &entry:manager.getStartListStatusesReference())if(entry.getAdvanceStatus()==yes)++exact;
    check(exact==2,"standalone qualifications feeding KO keep exactly the configured field even at a tied cutoff");
    manager.setQualificationFixedLimit(false);manager.updateCompetitorsAdvanceStatuses();int expanded=0;for(const auto &entry:manager.getStartListStatusesReference())if(entry.getAdvanceStatus()==yes)++expanded;
    check(expanded==4,"ordinary standalone qualifications retain every boundary tie");
    source.setQualifyingCompetitions({});source.setRules(CompetitionRules());source.getRulesPointer()->setRounds({RoundInfo(5,true)});source.setExceptionalRoundsCount(0);manager.updateCompetitorsAdvanceStatuses();check(manager.advancementLimit()==-1 && manager.qualificationStatus(&a)==-1,"ordinary one-round contest has no nonexistent advancement target");
    // KO winners and lucky losers need separate certainty bounds.
    Jumper f("F","Duel","FIN");athletes.append(&f);
    RoundInfo knockout(6,true);knockout.setKO(true);knockout.setAdvancingFromKOGroup(1);
    source.getRulesPointer()->setRounds({knockout,RoundInfo(4,true)});source.getResultsReference().getResultsReference().clear();
    manager.setFirstRoundJumpers(athletes);manager.setupStartListStatusesForActualRound();
    KOGroup g1,g2,g3;g1.setJumpers({&a,&b});g2.setJumpers({&c,&d});g3.setJumpers({&e,&f});QVector<KOGroup> groups{g1,g2,g3};
    KORoundManager ko(&groups,manager.getResults(),&source.getRulesPointer()->getRoundsReference()[0],&manager);ko.setLuckyLosersCount(1);manager.setKOManager(&ko);
    finish(&a,100);finish(&b,90);
    check(ko.advancementStatus(&a)==yes && ko.advancementStatus(&b)==wait,"a completed duel winner is safe while its loser still has a lucky-loser chance");
    check(manager.isLuckyLoser(&b),"current LL is marked before its advancement becomes certain");
    source.setExceptionalRoundsCount(1);ko.updateStatuses();
    check(ko.getLuckyLosersCount()==0 && ko.getLuckyLosers().isEmpty() && !manager.isLuckyLoser(&b),"single-round KO with a longer rules preset has no phantom lucky losers");
    for(int row=0;row<model.rowCount();++row)check(!model.index(row,0).data(LuckyLoserRole).toBool(),"effective final-round results never show LL from an unused preset round");
    source.setExceptionalRoundsCount(0);ko.setLuckyLosersCount(1);ko.updateStatuses();
    check(manager.isLuckyLoser(&b),"restoring a real next round restores the provisional LL");
    ko.setLuckyLosersCount(0);
    check(ko.getLuckyLosers().isEmpty() && !manager.isLuckyLoser(&b),"zero LL quota clears cached badge immediately");
    ko.setLuckyLosersCount(-2);ko.updateStatuses();check(ko.getLuckyLosersCount()==0 && !manager.isLuckyLoser(&b),"negative leftover capacity cannot create LL");
    ko.setLuckyLosersCount(1);ko.updateStatuses();
    KOGroupResultsTableModel groupModel(&ko,&groups[0]);
    for(int row=0;row<groupModel.rowCount();++row)if(groupModel.index(row,1).data().toString()==b.getNameAndSurname())check(groupModel.index(row,0).data(RoundStatusRole).toInt()==no,"KO group coloring indicates duel loss even while LL remains open");
    check(ko.guaranteedAdvancePoints(&e)<0,"no safe LL line is shown while an unresolved outside duel can produce a stronger loser");
    finish(&c,80);finish(&d,70);
    check(ko.guaranteedAdvancePoints(&e)==90 && ko.wouldSurelyAdvance(&e,900) && !ko.wouldSurelyAdvance(&e,899),"safe LL threshold can be lower than the group-winning score and includes boundary ties");
    check(ko.advancementStatus(&d)==no && ko.advancementStatus(&b)==wait,"a confirmed weaker loser is already eliminated before the remaining duel finishes");
    finish(&e,110);finish(&f,120);
    check(ko.advancementStatus(&b)==no && ko.advancementStatus(&e)==yes && ko.advancementStatus(&f)==yes,"final duel correctly resolves both the lucky-loser cutoff and its winner");
    check(manager.getFilteredJumpersForNextRound().size()==4,"KO selects the three winners and one lucky loser");
    check(manager.isLuckyLoser(&e) && !manager.isLuckyLoser(&f) && !manager.isLuckyLoser(&b),"LL marks only confirmed lucky losers, not winners or eliminated losers");
    manager.setupNextRound();manager.setKOManager(nullptr);
    check(manager.isLuckyLoser(&e) && !manager.isLuckyLoser(&f),"LL survives the transition into the next series after the KO manager is retired");
    source.getRulesPointer()->getRoundsReference()[1].setKO(true);
    check(!manager.isLuckyLoser(&e),"a fresh KO round must not reuse lucky-loser badges earned in the previous round");
    source.getRulesPointer()->getRoundsReference()[1].setKO(false);
    check(manager.isLuckyLoser(&e),"ordinary next round retains the genuine entry-via-LL badge");
    // Team certainty starts only after the team's last member has attempted the round.
    QVector<Jumper> members;members.reserve(8);QVector<Team> squads;squads.reserve(4);
    const QStringList countries{"POL","GER","SUI","AUT"};
    for(int i=0;i<4;++i){members.append(Jumper("Team",QString::number(i)+"A",countries[i]));members.append(Jumper("Team",QString::number(i)+"B",countries[i]));Team team(countries[i]);team.setJumpers({&members[2*i],&members[2*i+1]});squads.append(team);}
    QVector<Team *> teamPointers;for(auto &team:squads)teamPointers.append(&team);
    CompetitionInfo teamSource(&hill),teamTarget(&hill);CompetitionRules teamRules;teamRules.setCompetitionType(CompetitionRules::Team);teamRules.setJumpersInTeamCount(2);teamRules.setRounds({RoundInfo(4,true),RoundInfo(2,true)});teamSource.setRules(teamRules);
    TeamCompetitionManager teams;teams.setCompetitionInfo(&teamSource);teams.setCompetitionRules(teamSource.getRulesPointer());teams.setResults(&teamSource.getResultsReference());teams.setActualRound(1);teams.setActualGroup(1);teams.setRoundsTeams({teamPointers});teams.setupStartListStatusesForActualRound(true);
    const auto teamFinish=[&](int index,double points){auto *jumper=squads[index].getJumpersReference()[teams.getActualGroup()-1];JumpData jump(jumper,&hill);jump.setPoints(points);teamSource.getResultsReference().addJump(&squads[index],jump);StartListCompetitorStatus::getStatusOfJumper(jumper,teams.getStartListStatusesReference())->setJumpStatus(StartListCompetitorStatus::Finished);teams.updateCompetitorsAdvanceStatuses();};
    teamFinish(0,100);teamFinish(1,90);teamFinish(2,90);teamFinish(3,80);
    check(teams.getAdvanceStatusOfTeam(&squads[0])==wait,"an early team group does not falsely guarantee advancement before later members jump");
    teams.setActualGroup(2);teams.setupStartListStatusesForActualRound(true);teamFinish(0,0);teamFinish(1,0);teamFinish(2,0);teamFinish(3,0);
    check(teams.getAdvanceStatusOfTeam(&squads[1])==yes && teams.getAdvanceStatusOfTeam(&squads[2])==yes && teams.getAdvanceStatusOfTeam(&squads[3])==no,"team final-group ties retain all boundary teams and eliminate the next distinct rank");
    teamSource.setExceptionalRoundsCount(1);teamRules.setRounds({RoundInfo(3,true)});teamTarget.setRules(teamRules);teamTarget.setAdvancementCompetition(&teamSource);teamSource.setQualifyingCompetitions({&teamTarget});
    teamSource.getResultsReference().getResultOfTeam(&squads[0])->getJumpsReference().last().setDSQ(true);teams.updateCompetitorsAdvanceStatuses();
    check(teams.qualificationStatus(&squads[0])==no && TeamCompetitionManager::getFilteredTeamsAfterQualifications(&teamTarget).size()==3,"a member DSQ excludes the entire team and promotes eligible qualifying teams");
    check(teamSource.getResultsReference().getResultOfTeam(&squads[0])->getPosition()==0,"disqualified team has no classified place");
    teams.setActualGroup(1);teams.setupStartListStatusesForActualRound(true);
    check(teams.getStartListStatusesReference()[0].getJumpStatus()==StartListCompetitorStatus::Dsq,"later team groups skip disqualified squads");
    auto *disqualified=teamSource.getResultsReference().getResultOfTeam(&squads[0]);const int jumpCount=disqualified->getJumps().size();
    JumpData blocked(&members[0],&hill);blocked.setPoints(999);teamSource.getResultsReference().addJump(&squads[0],blocked);
    check(teamSource.getResultsReference().getResultOfTeam(&squads[0])->getJumps().size()==jumpCount,"engine rejects subsequent jumps for a disqualified team");
    for(auto *team:teamPointers){auto *result=teamSource.getResultsReference().getResultOfTeam(team);result->getJumpsReference().last().setDSQ(true);}
    teams.updateCompetitorsAdvanceStatuses();check(teams.checkCompetitionEnd(),"disqualifying every team ends an early group without waiting for impossible jumps");
    qInfo()<<"PASS: exhaustive advancement bounds, tied cutoffs, independent qualifying quotas and pure result presentation";
}
