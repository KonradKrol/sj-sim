#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <iostream>
#include <cmath>
#include "../source/seasons/SimulationSave.h"
#include "../source/balance/BalanceLabModel.h"
#include "../source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.h"
int main(int argc,char **argv){QApplication app(argc,argv);std::unique_ptr<SimulationSave> save(SimulationSave::loadFromFile(argv[1]));if(!save)return 2;int rows=0;
 for(auto &season:save->getSeasonsReference())for(auto *calendar:season.getCalendarsReference())for(auto *classification:calendar->getClassificationsReference()){
 classification->updatePointsSums();for(auto *row:classification->getResultsReference()){const auto score=row->getPointsSum();const bool eligible=row->hasEligibleParticipation();row->updatePointsSum();if(score!=row->getPointsSum() || eligible!=row->hasEligibleParticipation()){std::cerr<<"score mismatch";return 3;}++rows;}}
 auto values=QJsonObject{{"quality.eventDeviation.3",4.}};QString error;if(!BalanceLab::validateProfile(values,error)||!BalanceLab::validateProfile(values,error))return 4;
 values["quality.eventDeviation.3"]=-1.;if(BalanceLab::validateProfile(values,error))return 5;
 auto *calendar=save->getActualSeason()->getActualCalendar();CompetitionsArchiveListModel model(&calendar->getCompetitionsReference());int count=0;for(auto *event:calendar->getCompetitionsReference())if(event->getPlayed()){if(model.competitionAt(count++)!=event)return 6;}if(count!=model.rowCount())return 7;
 for(auto *event:calendar->getCompetitionsReference())if(!event->getPlayed()){event->setPlayed(true);model.setSeasonCompetitions(&calendar->getCompetitionsReference());if(model.rowCount()!=count+1)return 8;event->setPlayed(false);model.setSeasonCompetitions(&calendar->getCompetitionsReference());if(model.rowCount()!=count)return 9;break;}
 std::cout<<"PASS: "<<rows<<" classification scores and eligibility equal original per-row calculation; profile edits revalidated; archive index order and refresh correct\n";
}
