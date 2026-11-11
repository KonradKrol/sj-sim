#pragma once
#include "../source/competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../source/competitions/CompetitionManagers/KORoundManager.h"
#include <memory>
#include <iostream>

static void koRoundLimitsRegressions() {
 for(int limit:{30,50})for(bool sorted:{false,true}){
  Hill hill;CompetitionInfo event(&hill);CompetitionRules rules;
  RoundInfo first(50,true);first.setKO(true);first.setCountInKOGroup(2);first.setAdvancingFromKOGroup(1);first.setKoGroupsSelectionType(CompetitionRules::Classic);
  rules.setRounds({first,RoundInfo(limit,sorted)});event.setRules(rules);
  QVector<Jumper*> athletes;std::vector<std::unique_ptr<Jumper>> owned;
  for(int i=0;i<50;++i){owned.emplace_back(new Jumper(QString::number(i),"Athlete","POL"));athletes.append(owned.back().get());}
  IndividualCompetitionManager manager;manager.setCompetitionInfo(&event);manager.setCompetitionRules(event.getRulesPointer());manager.setResults(&event.getResultsReference());manager.setActualRound(1);manager.setFirstRoundJumpers(athletes);manager.setupStartListStatusesForActualRound();
  auto groups=KOGroup::constructKOGroups(&event.getRulesPointer()->getRoundsReference()[0],&athletes,KOGroup::Classic,&event);
  RoundInfo detached=first;detached.setAdvancingFromKOGroup(2);
  KORoundManager ko(&groups,manager.getResults(),&detached,&manager);ko.setLuckyLosersCount(limit-groups.size());manager.setKOManager(&ko);
  auto *pending=StartListCompetitorStatus::getStatusOfJumper(athletes.first(),manager.getStartListStatusesReference());
  pending->setJumpStatus(StartListCompetitorStatus::Dns);
  check(ko.advancementStatus(athletes.first())==StartListCompetitorStatus::Waiting,"stale DNS before current jump cannot mark a new round eliminated");
  pending->setJumpStatus(StartListCompetitorStatus::Unfinished);
  manager.updateCompetitorsAdvanceStatuses();
  for(auto *athlete:athletes)check(ko.advancementStatus(athlete)==StartListCompetitorStatus::Waiting,"unperformed jumps remain undecided");
  for(int i=0;i<athletes.size();++i){
   auto *athlete=athletes[i];JumpData jump(athlete,&hill);jump.setPoints(100+i*.3);event.getResultsReference().addJump(athlete,jump);
   StartListCompetitorStatus::getStatusOfJumper(athlete,manager.getStartListStatusesReference())->setJumpStatus(StartListCompetitorStatus::Finished);manager.updateCompetitorsAdvanceStatuses();
   for(int j=i+1;j<athletes.size();++j)check(ko.advancementStatus(athletes[j])==StartListCompetitorStatus::Waiting,"pending competitors never receive final advancement colors");
   for(const auto &group:groups){int winners=0;for(auto *member:group.getJumpers())winners+=ko.groupAdvancementStatus(member)==StartListCompetitorStatus::SureAdvanced;check(winners<=1,"only one guaranteed group winner");}
  }
  int advanced=0;for(const auto &status:manager.getStartListStatusesReference())advanced+=status.getAdvanceStatus()==StartListCompetitorStatus::SureAdvanced;
  check(advanced==limit,"KO quota matches next round limit");
  check(manager.getFilteredJumpersForNextRound().size()==limit,"next round selection preserves quota");
  manager.setupNextRound();check(manager.getActualRoundJumpersReference().size()==limit,"next round actually contains requested count");
  std::cerr<<"PASS KO 50 -> "<<limit<<", sort="<<sorted<<"\n";
 }
}
