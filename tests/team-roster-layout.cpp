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

static void check(bool value,const QString &message){if(!value){qCritical().noquote()<<"FAIL:"<<message;std::exit(1);}}
static void tick(int milliseconds=80){QEventLoop loop;QTimer::singleShot(milliseconds,&loop,&QEventLoop::quit);loop.exec();}
int main(int argc,char **argv){
    QApplication app(argc,argv);check(argc==2,"repository argument");QTemporaryDir temp;
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temp.path()+"/settings");
    app.setProperty("sjSimDataRoot",temp.path()+"/library");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");
    QString error;check(UserDataPaths::initialize(error),error);check(GlobalDatabase::get()->loadFromJson(),"library loaded");
    GlobalAppSettings::get()->setShowSeasonJumpersAndHillsHelp(false);
    MainWindow main;new QuickUi(&main);main.resize(1180,800);main.show();QSettings().setValue("onboarding/welcomeDismissed",true);
    CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition,&main);
    for(int i=0;i<200&&!config.catalogReady();++i)tick(20);check(config.catalogReady(),"catalog ready");
    auto *rules=config.findChild<QComboBox*>("comboBox_existingCompetitionRules");check(rules,"rules selector");
    const int teamRule=rules->findText("Team Competition");check(teamRule>=0,"team rules available");rules->setCurrentIndex(teamRule);tick();
    config.resize(760,570);config.show();tick(250);auto *bridge=config.findChild<QuickBridge*>();check(bridge,"configuration bridge");
    const auto fields=bridge->root()->values();const auto options=fields.value("options").toStringList();const int page=options.indexOf("Składy");check(page>=0,"configuration exposes squad tab");
    bridge->edit(fields.value("id").toInt(),page);tick(250);
    config.resize(1180,800);tick(250);
    auto *view=bridge->view();check(view&&view->status()==QQuickWidget::Ready,"configuration QML ready");
    for(const QSize size:{QSize(760,570),QSize(1180,800),QSize(900,650)}) {
        config.resize(size);tick(150);
        for(int pass=0;pass<2;++pass) {
            if(pass){bridge->edit(fields.value("id").toInt(),0);tick(100);bridge->edit(fields.value("id").toInt(),page);tick(150);}
            auto *workspace=view->rootObject()->findChild<QQuickItem *>("teamRosterWorkspace");check(workspace,"squad workspace visible after every tab switch");
            QQuickItem *card=nullptr;for(auto *child:workspace->childItems())if(child->objectName().startsWith("quick-")){card=child;break;}
            check(card && card->width()>=workspace->width()*.5,"team card consistently uses the larger half of the available width");
            check(card->x()+card->width()<=workspace->width()+1,"team card stays inside workspace");
        }
    }
    qInfo()<<"PASS squad card width on initial entry, resize and repeated tab changes";
}
