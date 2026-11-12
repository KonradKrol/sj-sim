#include <QApplication>
#include <QTemporaryDir>
#include <QSettings>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlError>
#include <QJSValue>
#include <QAbstractItemView>
#include <cstdlib>
#include <cmath>
#include "../source/global/UserDataPaths.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/UI/mainwindow.h"
#include "../source/UI/Quick/QuickUi.h"
#include "../source/UI/Seasons/NewSimulationSaveConfigurationWindow.h"
#include "../source/UI/Seasons/SimulationSaveManagerWindow.h"
#include "../source/simulation-v2/ProfileDraft.h"
static void check(bool condition,const char *message){if(!condition){qCritical()<<"FAIL:"<<message;std::exit(1);}}
static QObject *visibleItem(QQuickWidget *view,const QString &name){for(auto *item:view->rootObject()->findChildren<QQuickItem*>(name))if(item->isVisible()&&item->window()==view->quickWindow())return item;return nullptr;}
static void tick(int ms=100){QEventLoop wait;QTimer::singleShot(ms,&wait,&QEventLoop::quit);wait.exec();}
int main(int argc,char **argv){QApplication app(argc,argv);check(argc==2,"repository argument");QTemporaryDir temp;QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temp.path()+"/settings");QSettings().setValue("onboarding/welcomeDismissed",true);
    app.setProperty("sjSimDataRoot",temp.path()+"/library");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");QString error;check(UserDataPaths::initialize(error),"isolated defaults");
    MainWindow main;new QuickUi(&main);main.resize(1300,800);main.show();NewSimulationSaveConfigurationWindow setup({},&main);for(int i=0;i<200&&!setup.catalogReady();++i)tick(20);check(setup.catalogReady(),"example catalog ready");auto save=setup.createExampleSave();check(bool(save),"example created");save->setAutosaveEnabled(false);
    SimulationSaveManagerWindow manager(save.get(),&main);manager.show();tick(200);auto *bridge=manager.findChild<QuickBridge*>();check(bridge,"season bridge");auto *hillDraft=manager.findChild<SimulationProfileDraft*>("seasonHillDraft");auto *athleteDraft=manager.findChild<SimulationProfileDraft*>("seasonAthleteDraft");check(hillDraft&&athleteDraft,"profile drafts");
    auto navigate=[&](const QString &label){const auto fields=bridge->root()->values();const auto options=fields.value("options").toStringList();const int tab=options.indexOf(label=="Skoczkowie"?QString("Zawodnicy"):label);if(tab<0)qWarning()<<options;check(tab>=0,"season tab exists");bridge->edit(fields.value("id").toInt(),tab);tick(200);check(bridge->view()->status()==QQuickWidget::Ready,"QML ready");};
    navigate("Skocznie");auto *library=visibleItem(bridge->view(),"hillLibrary");check(library,"modern hill list exists before first selection");
    // A filtered legacy table must not reinterpret source-profile indices.
    auto *catalog=bridge->root()->children().first().value<QuickNode*>();auto *listNode=catalog->values().value("list").value<QuickNode*>();auto *table=listNode->values().value("table").value<QuickTableModel*>();check(table,"underlying table adapter");table->setFilter("NO MATCHING HILL IN LEGACY TABLE");
    QMetaObject::invokeMethod(library,"choose",Q_ARG(QVariant,QVariant(60)));tick(150);
    check(manager.selectedSeasonProfileId(true)==save->getHillsReference()[60]->getIDStr(),"actual hill UI selects by identity independently of table filter");check(hillDraft->profile().value("id").toString()==save->getHillsReference()[60]->getSimulationProfile()->id,"editor loads selected hill");
    auto *nameField=bridge->view()->rootObject()->findChild<QObject*>("hillName");check(nameField,"hill name field");nameField->setProperty("text","Hill synchronization test");QMetaObject::invokeMethod(nameField,"editingFinished");tick();check(save->getHillsReference()[60]->getName()=="Hill synchronization test","editing changes the selected hill");
    QMetaObject::invokeMethod(library,"choose",Q_ARG(QVariant,QVariant(61)));tick();check(manager.selectedSeasonProfileId(true)==save->getHillsReference()[61]->getIDStr(),"next hill is selectable");check(nameField->property("text").toString()==save->getHillsReference()[61]->getName(),"visible field updates to next hill");
    auto *hillRows=library->findChild<QQuickItem*>("hillList");check(hillRows,"hill viewport");hillRows->setProperty("contentY",650.);const double hillY=hillRows->property("contentY").toDouble();hillDraft->setValue("name","Another hill name");tick();check(std::abs(hillRows->property("contentY").toDouble()-hillY)<2,"editing preserves hill scroll");
    navigate("Skoczkowie");library=visibleItem(bridge->view(),"athleteLibrary");check(library,"modern athlete list exists");QMetaObject::invokeMethod(library,"choose",Q_ARG(QVariant,QVariant(35)),Q_ARG(QVariant,QVariant(false)));tick();check(manager.selectedSeasonProfileId(false)==save->getJumpersReference()[35]->getIDStr(),"athlete selection uses identity");
    auto *athleteRows=library->findChild<QQuickItem*>("athleteList");check(athleteRows,"athlete viewport");athleteRows->setProperty("contentY",550.);const double athleteY=athleteRows->property("contentY").toDouble();athleteDraft->setValue("overall",82);tick();if(std::abs(athleteRows->property("contentY").toDouble()-athleteY)>=2)qWarning()<<"Athlete scroll before/after"<<athleteY<<athleteRows->property("contentY")<<manager.seasonLibraryState(false);check(std::abs(athleteRows->property("contentY").toDouble()-athleteY)<2,"editing preserves athlete scroll");
    navigate("Skocznie");library=visibleItem(bridge->view(),"hillLibrary");hillRows=library->findChild<QQuickItem*>("hillList");check(manager.selectedSeasonProfileId(true)==save->getHillsReference()[61]->getIDStr(),"hill selection survives navigation");if(std::abs(hillRows->property("contentY").toDouble()-hillY)>=2)qWarning()<<"Hill restored scroll"<<hillRows->property("contentY")<<manager.seasonLibraryState(true)<<"ready"<<library->property("stateReady")<<"load"<<library->property("loadViewState").value<QJSValue>().call().toVariant()<<"height"<<hillRows->height()<<"content"<<hillRows->property("contentHeight");check(std::abs(hillRows->property("contentY").toDouble()-hillY)<2,"hill scroll restored after tab change");
    navigate("Przegląd");navigate("Skocznie");library=visibleItem(bridge->view(),"hillLibrary");hillRows=library->findChild<QQuickItem*>("hillList");check(std::abs(hillRows->property("contentY").toDouble()-hillY)<2,"hill scroll survives overview navigation");
    navigate("Skoczkowie");library=visibleItem(bridge->view(),"athleteLibrary");athleteRows=library->findChild<QQuickItem*>("athleteList");if(std::abs(athleteRows->property("contentY").toDouble()-athleteY)>=2)qWarning()<<"Athlete restored"<<athleteY<<athleteRows->property("contentY")<<manager.seasonLibraryState(false)<<"pending"<<library->property("viewportRestorePending")<<"load"<<library->property("loadViewState").value<QJSValue>().call().toVariant()<<"geometry"<<athleteRows->width()<<athleteRows->height()<<athleteRows->property("contentHeight");check(std::abs(athleteRows->property("contentY").toDouble()-athleteY)<2,"athlete scroll restored after tab change");
    main.resize(760,570);tick();check(library->property("visible").toBool(),"list stays available on small laptop layout");manager.hide();
    qInfo()<<"PASS actual QML selection, editing, filtered-table independence and persistent selection/scroll for both season libraries";
}
