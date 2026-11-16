#include <QCoreApplication>
#include <QDebug>
#include "../source/competitions/CompetitionInfo.h"
#include "../source/competitions/CompetitionResults.h"
#include "../source/UI/ResultsShowing/ResultsHistoryWidget.h"
#include <cstdlib>
static void check(bool value,const char *why){if(!value){qCritical()<<why;std::exit(1);}}
int main(int argc,char **argv){
 QCoreApplication app(argc,argv);CompetitionInfo event;CompetitionRules rules;rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(2);rules.setRounds({RoundInfo(4,true),RoundInfo(4,true)});event.setRules(rules);
 Jumper a,b,c,d;Team first,second;
 CompetitionSingleResult team1(&event,&first,CompetitionSingleResult::TeamResult),team2(&event,&second,CompetitionSingleResult::TeamResult);team1.setPosition(1);team2.setPosition(2);
 auto jump=[&](Jumper *jumper,double points){JumpData j;j.setCompetition(&event);j.setJumper(jumper);j.setPoints(points);return j;};
 team1.getJumpsReference()={jump(&a,110),jump(&b,190),jump(&a,100),jump(&b,150)};team2.getJumpsReference()={jump(&c,200),jump(&d,80),jump(&c,120),jump(&d,70)};
 event.getResultsReference().setResults({team1,team2});
 check(event.getResultsReference().individualPosition(&a)==3,"individual place differs from the winning team's place");
 check(team1.getJumpsReference().first().getPositionInCompetitionForTeamCompetition()==3,"live jump card uses the individual ranking");
 check(event.getResultsReference().individualPosition(&b)==1,"strongest individual wins despite team display mode");
 auto snapshot=ResultsHistoryWidget::afterRound(event.getResultsReference(),1,false,&event);
 check(snapshot.individualPosition(&a)==3 && snapshot.individualPosition(&b)==2 && snapshot.individualPosition(&c)==1,"archive ranks only jumps included in the viewed snapshot");
 auto individual=ResultsHistoryWidget::afterRound(event.getResultsReference(),1,true,&event);
 check(individual.individualPosition(&a)==snapshot.individualPosition(&a),"team and individual archive modes agree");
 team2.getJumpsReference()[0].setPoints(210);team2.getJumpsReference()[2].setPoints(0);event.getResultsReference().setResults({team1,team2});
 check(event.getResultsReference().individualPosition(&a)==2 && event.getResultsReference().individualPosition(&c)==2,"individual tied totals share the same rank");
 Jumper absent;check(event.getResultsReference().individualPosition(&absent)==0,"missing jumper does not receive first place");
 qInfo()<<"PASS: live and archived individual team rankings, snapshots, ties and missing athletes";
}
