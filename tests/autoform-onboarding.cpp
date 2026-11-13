#include <QApplication>
#include <QTemporaryDir>
#include <QSettings>
#include <QComboBox>
#include <QEventLoop>
#include <QTimer>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlError>
#include <QQmlContext>
#include <QDebug>
#include <QMessageBox>
#include "../source/global/GlobalAppSettings.h"
#include <QListView>
#include <cstdlib>
#include "../source/global/UserDataPaths.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/UI/mainwindow.h"
#include "../source/UI/Quick/QuickUi.h"
#include "../source/UI/Quick/QuickRoster.h"
#include "../source/UI/Competition/CompetitionConfigWindow.h"
#include "../source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.h"
#include "../source/UI/Seasons/NewSimulationSaveConfigurationWindow.h"
#include "../source/UI/Seasons/SimulationSaveManagerWindow.h"
#include "../source/simulation-v2/ProfileDraft.h"

#include <QCheckBox>

static void check(bool value,const QString &message){if(!value){qCritical().noquote()<<"FAIL:"<<message;std::exit(1);}}
static void tick(int milliseconds=80){QEventLoop loop;QTimer::singleShot(milliseconds,&loop,&QEventLoop::quit);loop.exec();}
int main(int argc,char **argv){
    QApplication app(argc,argv);check(argc==2,"repository argument");QTemporaryDir temp;
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temp.path()+"/settings");
    app.setProperty("sjSimDataRoot",temp.path()+"/library");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");
    QString error;check(UserDataPaths::initialize(error),error);check(GlobalDatabase::get()->loadFromJson(),"library loaded");
    MainWindow main;new QuickUi(&main);
    QTimer modalGuard;QObject::connect(&modalGuard,&QTimer::timeout,[]{for(auto *w:QApplication::allWidgets())if(auto *m=qobject_cast<QMessageBox*>(w);m && (m->isVisible()||m->property("sjQuickPage").toBool())){qCritical()<<"Unexpected test dialog:"<<m->text();std::exit(1);}});modalGuard.start(50);
    NewSimulationSaveConfigurationWindow setup({},&main);for(int i=0;i<200&&!setup.catalogReady();++i)tick(20);
    check(setup.catalogReady(),"season catalog ready");auto save=setup.createExampleSave();check(bool(save),"example created");
    check(save->formEngine().enabled && save->getAutosaveEnabled() && save->getActualSeason()->getDatesEnabled(),"example enables all three automations");
    check(save->getActualSeason()->getCalendarsReference().size()==2,"example has summer and winter");
    for(auto *calendar:save->getActualSeason()->getCalendarsReference())for(auto *event:calendar->getCompetitionsReference())check(event->getScheduledDate().isValid(),"preset dates retained");
    {
        SimulationSaveManagerWindow manager(save.get(),&main);
        check(manager.autoFormNoticeVisible(),"automatic form notice visible");
        manager.dismissAutoFormNotice(false);check(!manager.autoFormNoticeVisible() && save->formEngine().enabled,"dismissal keeps autoform enabled");
        QSettings().remove("onboarding/autoFormNoticeDismissed");
        manager.dismissAutoFormNotice(true);check(!manager.autoFormNoticeVisible() && !save->formEngine().enabled,"notice can disable autoform without another dialog");
    }
    {
        SimulationSaveManagerWindow reopened(save.get(),&main);check(!reopened.autoFormNoticeVisible(),"notice dismissal survives reopening");
    }
    setup.findChild<QCheckBox *>("newSeasonDates")->setChecked(false);
    auto undated=setup.createSave();check(undated && !undated->formEngine().enabled,"undated seasons do not enable autoform");
    qInfo()<<"PASS example dates, autoform, autosave, one-time notice, direct opt-out and undated seasons";
}
