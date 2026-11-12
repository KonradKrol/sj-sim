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

static void check(bool value,const QString &message){if(!value){qCritical().noquote()<<"FAIL:"<<message;std::exit(1);}}
static void tick(int milliseconds=80){QEventLoop loop;QTimer::singleShot(milliseconds,&loop,&QEventLoop::quit);loop.exec();}
static const Team *country(const QVector<Team> &teams,const QString &code){for(const auto &t:teams)if(t.getCountryCode()==code)return &t;return nullptr;}
int main(int argc,char **argv){
    QApplication app(argc,argv);check(argc==2,"repository argument");QTemporaryDir temp;
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temp.path()+"/settings");
    app.setProperty("sjSimDataRoot",temp.path()+"/library");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");
    QString error;check(UserDataPaths::initialize(error),error);check(GlobalDatabase::get()->loadFromJson(),"library loaded");
    GlobalAppSettings::get()->setShowSeasonJumpersAndHillsHelp(false);
    QTimer modalGuard;QObject::connect(&modalGuard,&QTimer::timeout,[]{for(auto *w:QApplication::allWidgets())if(auto *m=qobject_cast<QMessageBox*>(w);m && (m->isVisible()||m->property("sjQuickPage").toBool())){qCritical()<<"Unexpected test dialog:"<<m->windowTitle()<<m->text();std::exit(1);}});modalGuard.start(50);
    MainWindow main;new QuickUi(&main);main.resize(1180,800);main.show();QSettings().setValue("onboarding/welcomeDismissed",true);
    CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition,&main);
    for(int i=0;i<200&&!config.catalogReady();++i)tick(20);check(config.catalogReady(),"catalog ready");
    auto *rules=config.findChild<QComboBox*>("comboBox_existingCompetitionRules");check(rules,"rules selector");
    const int teamRule=rules->findText("Team Competition");check(teamRule>=0,"team rules available");rules->setCurrentIndex(teamRule);tick();
    const int count=config.getJumpersCountInTeam();check(count==4,"four-person teams");auto original=config.getCompetitionTeams();
    check(!original.isEmpty(),"initial squads");const Team *chosen=nullptr;for(const auto &t:original)if(t.getJumpersCount()>count){chosen=&t;break;}
    check(chosen,"country with reserves");const auto code=chosen->getCountryCode();auto *leader=chosen->getJumpers().first();auto *second=chosen->getJumpers()[1];
    QuickRoster roster(&config,&app);check(roster.teams(),"team eligibility backend");const int total=roster.count();
    roster.toggle(QuickRoster::key(leader));check(roster.count()==total-1,"eligibility deselects one athlete");
    auto teams=config.getCompetitionTeams();auto *changed=country(teams,code);check(changed&&!changed->getJumpers()[0]&&changed->getJumpers()[1]==second,"exclusion leaves a vacant starter slot without shifting teammates");
    check(!changed->getJumpers().contains(leader),"excluded athlete removed from reserves too");
    {
        Team editable=*changed;TeamEditorWidget editor(&editable,count);QuickRoster squad(&editor,&app);
        auto *reserve=editable.getJumpers()[count];check(reserve,"eligible reserve");
        check(squad.ordered()->data(squad.ordered()->index(0),Qt::UserRole+2).toString()=="Wolne miejsce","vacant slot is visible");
        squad.moveBefore(QuickRoster::key(reserve),"vacant:0");check(editor.getJumpers().first()==reserve,"reserve can be dropped into a vacant slot");
        check(!editor.getJumpers().contains(nullptr),"empty reserve slot removed after swap");
        squad.undo();check(editor.getJumpers().first()==nullptr,"undo restores the vacancy");
    }
    roster.undo();teams=config.getCompetitionTeams();check(roster.count()==total&&country(teams,code)->getJumpers().first()==leader,"undo restores eligibility and exact squad");
    roster.toggle(QuickRoster::key(leader));config.autoCompleteTeams(false);teams=config.getCompetitionTeams();
    for(const auto &t:teams){check(!t.getJumpers().contains(leader),"automatic selection never reintroduces excluded athlete");check(t.hasValidLineup(count),"automatic selection creates complete squads");}
    config.setTeamEligibility({});config.autoCompleteTeams(false);check(config.getCompetitionTeams().isEmpty(),"no eligible athletes means no automatic teams");
    roster.sync();roster.applyPreset(-1,3);config.autoCompleteTeams(false);check(!config.getCompetitionTeams().isEmpty(),"all athletes can be selected again");
    config.show();tick(250);auto *bridge=config.findChild<QuickBridge*>();check(bridge,"configuration bridge");
    const auto fields=bridge->root()->values();const auto options=fields.value("options").toStringList();const int page=options.indexOf("Składy");check(page>=0,"configuration exposes squad tab");
    bridge->edit(fields.value("id").toInt(),page);tick(250);auto *view=bridge->view();check(view&&view->status()==QQuickWidget::Ready,"configuration QML ready");
    check(view->rootObject()->findChild<QObject*>("teamRosterWorkspace"),"squad workspace is a full-height tab");
    view->grabFramebuffer().save("/tmp/sjsim-team-eligibility.png");main.resize(760,570);tick(150);view->grabFramebuffer().save("/tmp/sjsim-team-eligibility-small.png");
    config.hide();tick();
    NewSimulationSaveConfigurationWindow setup({},&main);for(int i=0;i<200&&!setup.catalogReady();++i)tick(20);check(setup.catalogReady(),"season source ready");auto save=setup.createExampleSave();check(bool(save),"example save created");
    const auto beforeFirst=*save->getJumpersReference()[0]->getSimulationProfile();const auto beforeSecond=*save->getJumpersReference()[1]->getSimulationProfile();
    {
        SimulationSaveManagerWindow manager(save.get(),&main);auto *draft=manager.findChild<SimulationProfileDraft*>("seasonAthleteDraft");check(draft,"season athlete draft");
        DatabaseItemsListView *list=nullptr;for(auto *candidate:manager.findChildren<DatabaseItemsListView*>())if(candidate->getType()==DatabaseItemsListView::SeasonJumpersItems){list=candidate;break;}
        check(list,"season athlete list");list->getListView()->setCurrentIndex(list->getListModel()->index(0,0));tick();
        check(draft->profile().value("id").toString()==beforeFirst.id,"first athlete selected");
        list->setupListModel();list->getListView()->setCurrentIndex(list->getListModel()->index(1,0));tick();
        check(draft->profile().value("id").toString()==beforeSecond.id,"second athlete selected after model rebuild");
        check(save->getJumpersReference()[1]->getSimulationProfile()->toJson()==beforeSecond.toJson(),"loading editor never overwrites selected athlete");
    }
    qInfo()<<"PASS team eligibility, exact undo, vacant-slot swaps, automatic selection, squad tab and season editor selection";
}
