#include <QApplication>
#include <QElapsedTimer>
#include <QTimer>
#include <QMessageBox>
#include <QAbstractButton>
#include <iostream>
#include "../source/seasons/SimulationSave.h"
#include "../source/UI/Competition/CompetitionManagerWindow.h"
#include "../source/competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.h"
#include "../source/UI/ResultsShowing/ResultsHistoryWidget.h"
#include "../source/global/GlobalAppSettings.h"
int main(int argc,char**argv){QApplication app(argc,argv);GlobalAppSettings::get()->setLiveCompetition(false);std::unique_ptr<SimulationSave> save(SimulationSave::loadFromFile(argv[1]));if(!save)return 2;
 for(int run=0;run<(argc>2 ? 0 : 3);++run){auto *hill=save->getHillsReference().first();for(auto *h:save->getHillsReference())if(h->getKPoint()==120){hill=h;break;}hill->setRecord(500);
 CompetitionInfo info(hill);CompetitionRules rules;rules.setHasDsq(false);rules.setRounds({RoundInfo(83,true),RoundInfo(30,true)});info.setRules(rules);
 IndividualCompetitionManager manager;manager.setCompetitionInfo(&info);manager.setCompetitionRules(info.getRulesPointer());manager.setResults(&info.getResultsReference());manager.setActualRound(1);manager.setActualGate(10);manager.getRoundsStartingGatesReference().append(10);info.getRoundsKOGroupsReference().append(QVector<KOGroup>());manager.getRoundsKOGroupsReference().append(&info.getRoundsKOGroupsReference().first());manager.setFirstRoundJumpers(save->getJumpersReference());manager.setupStartListStatusesForActualRound();manager.setActualStartListIndex(0);manager.setActualJumperToNextUnfinished();
 CompetitionManagerWindow window(&manager,nullptr,true);window.setProperty("quickSeasonSimulation",true);window.setProperty("forceAutomaticGate",true);
 QTimer dialogs;QObject::connect(&dialogs,&QTimer::timeout,[]{for(auto *w:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(w)){auto *b=box->button(QMessageBox::Yes);if(!b)b=box->button(QMessageBox::Ok);if(b)b->click();else box->accept();}});dialogs.start(1);
 QElapsedTimer t;t.start();window.autoSimulateCompetition();int jumps=0;for(const auto &r:info.getResultsReference().getResultsReference())jumps+=r.getJumps().size();std::cout<<"auto_ms="<<t.nsecsElapsed()/1e6<<" jumps="<<jumps<<std::endl;
 }
 auto *calendar=save->getActualSeason()->getActualCalendar();CompetitionsArchiveListModel model(&calendar->getCompetitionsReference());QElapsedTimer t;t.start();long sum=0;for(int pass=0;pass<500;++pass)for(int r=0;r<model.rowCount();++r){sum+=model.data(model.index(r),Qt::DisplayRole).toString().size();sum+=model.competitionAt(r)!=nullptr;}
 std::cout<<"archive_scan_ms="<<t.nsecsElapsed()/1e6<<" rows="<<model.rowCount()<<" checksum="<<sum<<std::endl;
 for(auto *event:calendar->getCompetitionsReference())if(event->getPlayed()&&!event->getResultsReference().getResultsReference().isEmpty()){t.restart();{ResultsHistoryWidget history(event,false,nullptr);}std::cout<<"archive_open_ms="<<t.nsecsElapsed()/1e6<<std::endl;break;}
}
