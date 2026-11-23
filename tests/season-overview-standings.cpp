#include "../source/UI/Quick/QuickUi.h"
#include "../source/UI/Seasons/Classifications/ClassificationResultsTableView.h"
#include "../source/UI/Seasons/SimulationSaveManagerWindow.h"
#include "../source/UI/mainwindow.h"
#include "../source/global/UserDataPaths.h"
#include "../source/seasons/SimulationSave.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QSettings>
#include <QTemporaryDir>
#include <cstdlib>

static void check(bool ok,const char *message)
{
    if(!ok){qCritical()<<message;std::exit(1);}
}

int main(int argc,char **argv)
{
    QApplication app(argc,argv);
    app.setOrganizationName("SJ-Sim-Season-Overview-Test");
    app.setApplicationName("Isolated");
    QTemporaryDir directory;
    check(directory.isValid(),"temporary library");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,directory.path()+"/settings");
    app.setProperty("sjSimDataRoot",directory.path());
    app.setProperty("sjSimDefaultsRoot",QString::fromLocal8Bit(argv[1])+"/data/defaults");
    QString error;
    check(UserDataPaths::initialize(error),qPrintable(error));

    MainWindow main;
    main.setParentApplication(&app);
    new QuickUi(&main);
    main.show();

    SimulationSave save;
    save.setAutosaveEnabled(false);
    auto *athlete=new Jumper("A","Jumper","POL");
    auto *hill=new Hill;
    save.setJumpers({athlete});
    save.setHills({hill});
    auto *winter=new SeasonCalendar("Zima");
    auto *juniors=new SeasonCalendar("Mś Juniorów");
    auto *classification=new Classification("Puchar Globu");
    auto *rows=classification->ownResult(std::make_unique<ClassificationSingleResult>(classification,athlete));
    rows->setPointsSum(100);
    rows->setPosition(1);
    classification->getResultsReference().append(rows);
    winter->setClassifications({classification});
    auto *final=new CompetitionInfo(hill);
    juniors->setCompetitions({final});
    Season season;
    season.setSeasonNumber(2026);
    season.setCalendars({juniors,winter});
    season.setActualCalendar(juniors);
    save.setSeasons({season});
    save.setActualSeason(&save.getSeasonsReference().first());
    save.updateNextCompetitionIndex();

    SimulationSaveManagerWindow manager(&save,&main);
    manager.show();
    app.processEvents();
    auto *bridge=manager.findChild<QuickBridge *>();
    auto *choice=manager.findChild<QComboBox *>("comboBox_classifications");
    auto *table=manager.findChild<ClassificationResultsTableView *>();
    check(bridge && choice && table && choice->count()==0,"unfinished junior calendar still uses its own standings");

    final->setPlayed(true);
    manager.refreshCalendarState();
    bridge->refresh();
    auto *panel=qobject_cast<QuickNode *>(bridge->find("season-standings"));
    check(choice->count()==1 && choice->currentText()==classification->getName(),"finished season shows winter classification");
    check(table->getClassification()==classification && table->getTableView()->model()->rowCount()==1,
          "finished season shows scored results");
    check(panel && panel->values().value("text").toString().contains(winter->getName()),"overview identifies standings calendar");
    check(save.getActualSeason()->getActualCalendar()==juniors,"viewing standings preserves active calendar");
    qInfo()<<"PASS: completed multi-calendar season shows final standings";
}
