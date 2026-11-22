// Benchmark: simulating N season events one by one (the quick-simulate path) versus the batch
// path used by "Przesymuluj N wydarzeń".
// Usage: season-batch-performance <copy-of-save.json> <events> [single|batch]
// Run it beside a portable.txt so that autosave writes only into an isolated SjSimData folder,
// and always on a copy of a save: the run plays and saves events.
#include <QApplication>
#include <QElapsedTimer>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QAbstractButton>
#include <iostream>
#include <memory>
#include "../source/seasons/SimulationSave.h"
#include "../source/UI/Seasons/SimulationSaveManagerWindow.h"
#include "../source/global/GlobalAppSettings.h"

int main(int argc,char **argv)
{
    QApplication app(argc,argv);
    if(argc<3){std::cerr<<"usage: save.json events [single|batch]\n";return 2;}
    GlobalAppSettings::get()->setLiveCompetition(false);
    QElapsedTimer load;load.start();
    std::unique_ptr<SimulationSave> save(SimulationSave::loadFromFile(argv[1]));
    if(!save){std::cerr<<"cannot load save\n";return 2;}
    std::cout<<"load_ms="<<load.elapsed()<<std::endl;
    const int events=QString(argv[2]).toInt();
    const QString mode=argc>3?QString(argv[3]):QString("batch");
    // Accept any message box a code path might still show, so the run never blocks.
    QTimer dialogs;QObject::connect(&dialogs,&QTimer::timeout,[]{for(auto *w:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(w)){auto *b=box->button(QMessageBox::Yes);if(!b)b=box->button(QMessageBox::Ok);if(b)b->click();else box->accept();}});dialogs.start(5);
    SimulationSaveManagerWindow window(save.get());
    const int pendingBefore=window.pendingSeasonEventsCount();
    QElapsedTimer t;t.start();int played=0;
    if(mode=="single") {
        auto *quick=window.findChild<QPushButton *>("quickSimulateNextCompetition");
        for(int i=0;i<events && window.pendingSeasonEventsCount()>0;++i){quick->click();QApplication::processEvents();++played;}
    } else {
        played=window.simulateSeasonEvents(events);
    }
    const double ms=t.nsecsElapsed()/1e6;
    std::cout<<"mode="<<mode.toStdString()<<" events="<<played<<" pending_before="<<pendingBefore<<" pending_after="<<window.pendingSeasonEventsCount()
             <<" total_ms="<<ms<<" per_event_ms="<<(played?ms/played:0)<<std::endl;
    return played==std::min(events,pendingBefore)?0:1;
}
