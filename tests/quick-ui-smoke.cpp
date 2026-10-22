#include "../source/UI/Quick/QuickUi.h"
#include "../source/UI/Quick/QuickCalendar.h"
#include "../source/UI/Quick/QuickRoster.h"
#include "../source/UI/Quick/QuickRaceStats.h"
#include "../source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.h"
#include "../source/UI/DatabaseEditor/DatabaseItemsListView.h"
#include "../source/UI/Seasons/CalendarEditor/CalendarEditorWidget.h"
#include "../source/UI/mainwindow.h"
#include "../source/UI/DatabaseEditor/DatabaseEditorWindow.h"
#include "../source/UI/Seasons/SimulationSavesWindow.h"
#include "../source/UI/Seasons/NewSimulationSaveConfigurationWindow.h"
#include "../source/UI/Seasons/NewSeasonConfiguratorWindow.h"
#include "../source/UI/SingleJumps/SingleJumpsConfigWindow.h"
#include "../source/UI/SingleJumps/SingleJumpsResultsWindow.h"
#include "../source/UI/AppSettings/AppSettingsWindow.h"
#include "../source/UI/AppSettings/CountriesEditorWindow.h"
#include "../source/UI/AppSettings/JumpDataInfoChoiceDialog.h"
#include "../source/UI/SingleJumps/SingleJumpsResultsTableSortDialog.h"
#include "../source/UI/EditorWidgets/Dialogs/CharacteristicInputDialog.h"
#include "../source/UI/EditorWidgets/EditStartListWithJumpersListsWindow.h"
#include "../source/UI/Competition/StartListByOtherCompetitionConfigWindow.h"
#include "../source/UI/AppSettings/NewJumpsImportancePresetDialog.h"
#include "../source/UI/Competition/CompetitionConfigWindow.h"
#include "../source/UI/Competition/CompetitionManagerWindow.h"
#include "../source/UI/Competition/StartList/StartListModel.h"
#include "../source/UI/EditorWidgets/HillEditorWidget.h"
#include "../source/UI/EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../source/competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../source/UI/Seasons/SimulationSaveManagerWindow.h"
#include "../source/UI/Seasons/Stats/SimulationRatingsWindow.h"
#include "../source/UI/Seasons/Stats/JumperStatsWindow.h"
#include "../source/UI/Seasons/Stats/ApperanceInClassificationWindow.h"
#include "../source/UI/Seasons/CalendarEditor/MultipleTrainingsEditDialog.h"
#include "../source/UI/Seasons/CalendarEditor/MultipleTrialRoundsEditDialog.h"
#include "../source/UI/Competition/RandomWindConfigWindow.h"
#include "../source/UI/EditorWidgets/JumpersListEditorWindow.h"
#include "../source/UI/JumpManipulation/JumpManipulatorConfigWindow.h"
#include "../source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.h"
#include "../source/UI/HelpWindow.h"
#include "../source/UI/ThanksWindow.h"
#include "../source/UI/ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../source/single-jumps/SingleJumpsManager.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/global/GlobalAppSettings.h"
#include "../source/global/GlobalSimulationSettings.h"
#include "../source/global/MyRandom.h"
#include "../source/seasons/SeasonCalendarPreset.h"
#include <QApplication>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QJSValue>
#include <QRegularExpression>
#include <limits>
#include "../source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.h"
#include "../source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.h"
#include <QTemporaryDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QTabWidget>
#include <QToolBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QMenu>
#include <QCheckBox>
#include <QAbstractItemView>
#include <QSettings>
#include <QInputDialog>
#include <QTimer>
#include <QtTest/QTest>
#include <cstdlib>
#include <cmath>
#include <QSignalSpy>
#include <QAbstractItemModelTester>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QElapsedTimer>
#include <QProgressDialog>
#include <QProgressBar>
#include <QTableWidget>
#include <QLabel>
#include "../source/UI/EditorWidgets/KOSystem/KOGroupEditorWidget.h"
#include "../source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h"
#include "../source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../source/competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include <memory>
#include <functional>

static void check(bool ok,const QString &message) { if(!ok) { qCritical().noquote()<<"FAIL:"<<message; std::exit(1); } }
static QQuickItem *item(QQuickItem *root,const QString &name) {
    if(root->objectName()==name) return root;
    for(auto *child:root->childItems()) if(auto *result=item(child,name)) return result;
    return nullptr;
}
static MainWindow *applicationWindow=nullptr;
static QQuickWidget *scene(QWidget *window) {
    auto *bridge=window->findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly);
    return bridge?bridge->view():nullptr;
}
static QWidget *presentation(QWidget *window) { return window->property("sjQuickPage").toBool()?applicationWindow:window; }
static void clickControl(QQuickWidget *view,const QString &name);
static QString output;
static int renders=0;
static std::unique_ptr<CompetitionInfo> finishedCompetition;
static void render(QWidget *window,const QString &name,bool sweep=false) {
    window->show(); QTest::qWait(200);
    auto *view=scene(window);
    check(view && view->status()==QQuickWidget::Ready,"load "+name);
    auto *bridge=window->findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly);
    check(bridge!=nullptr,"adapter "+name);
    auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();
    auto sizes=sweep?QList<QSize>{{1440,900},{1024,640},{520,600},{360,480}}:QList<QSize>{{1024,640}};
    if(sweep && (name=="last-jump-full" || name=="live-results-2"))sizes.prepend(QSize(2560,1440));
    for(const auto &size:sizes) {
        presentation(window)->resize(size);
        for(bool dark:sweep?QList<bool>{false,true}:QList<bool>{false}) {
            appearance->setDark(dark); QTest::qWait(200);
            // A grab flushes pending Quick layout polish in the offscreen widget.
            auto image=presentation(window)->grab().toImage(); if(image.isNull()) qCritical()<<"Geometry"<<window->size()<<view->size(); check(!image.isNull(),"frame "+name);
            check(item(view->rootObject(),"releaseBadge")==nullptr,"pages and short dialogs do not repeat application chrome");
            auto *footer=applicationWindow->findChild<QQuickWidget *>("quickApplicationFooter");
            check(footer && item(footer->rootObject(),"releaseBadge"),"one shared release badge exists");
            if(window->property("sjQuickPage").toBool())check(window->testAttribute(Qt::WA_DontShowOnScreen) && view->window()==applicationWindow,"task screen is presented inside the main window");
            if(name=="home") {
                const auto modes=view->rootObject()->findChildren<QQuickItem *>("simulationMode");
                check(modes.size()==3,"all simulation modes exist");
                for(auto *mode:modes) {
                    const auto position=mode->mapToItem(view->rootObject(),QPointF());
                    check(mode->width()>=200 && position.x()>=0 && position.x()+mode->width()<=view->width()+1,"primary mode remains readable and inside the home viewport");
                }
            }
            if((name.startsWith("live-") || name=="last-jump-full") && sweep && size.width()>=1000) {
                auto *summary=item(view->rootObject(),"raceLastJump");
                check(summary!=nullptr,"live jump summary exists");
                const auto position=summary->mapToItem(view->rootObject(),QPointF());
                check(position.y()>=0 && position.y()+summary->height()<view->height(),"live jump summary stays in the viewport");
                auto *athlete=item(view->rootObject(),"raceAthlete");auto *results=item(view->rootObject(),"raceResults");auto *start=item(view->rootObject(),"raceStartList");
                check(athlete&&results&&start,"competition keeps athlete, results and start list in one workspace");
                check(summary->width()<=721 && athlete->width()<=321,"competition cards keep compact readable widths on large monitors");
                auto *content=item(view->rootObject(),"lastJumpContent");
                check(content && summary->height()<=content->implicitHeight()+17,"last jump card has no stretched empty footer");
                const auto athleteBounds=athlete->mapRectToItem(view->rootObject(),athlete->boundingRect());const auto summaryBounds=summary->mapRectToItem(view->rootObject(),summary->boundingRect());
                check(!athleteBounds.intersects(summaryBounds),"athlete and last jump never overlap");
                for(auto *panel:{results,start}){const auto bounds=panel->mapRectToItem(view->rootObject(),panel->boundingRect());if(bounds.bottom()>view->height()+1){presentation(window)->grab().save("/tmp/sj-race-overflow.png");auto *grid=item(view->rootObject(),"raceTables");auto *workspace=item(view->rootObject(),"raceWorkspace");qInfo()<<"RACEGEOMETRY"<<grid->height()<<grid->y()<<workspace->height()<<workspace->property("availableHeight")<<panel->parentItem()->height();}check(panel->isVisible()&&bounds.top()>=0&&bounds.bottom()<=view->height()+1&&panel->height()>=120,QString("critical tables remain visible together without page scrolling on laptops: %1 %2 %3x%4 y=%5 height=%6 viewport=%7").arg(name,panel->objectName()).arg(size.width()).arg(size.height()).arg(bounds.y()).arg(bounds.height()).arg(view->height()));}

            }
            if(name=="last-jump-full"){
                auto *summary=item(view->rootObject(),"raceLastJump");const auto bounds=summary->mapRectToItem(view->rootObject(),summary->boundingRect());
                if(bounds.bottom()>view->height()){presentation(window)->grab().save("/tmp/sj-last-jump-status-overflow.png");qInfo()<<"JUMPSTATUS"<<bounds<<view->height();}
                check(bounds.top()>=0&&bounds.bottom()<=view->height(),"complete last jump remains directly visible even in the smallest viewport");
            }
            if(name=="calendar-classifications" && size.width()==360 && !dark) {
                auto *classes=item(view->rootObject(),"calendarClassesWorkspace");classes->setProperty("editing",true);QTest::qWait(80);view->grabFramebuffer();auto *editor=item(view->rootObject(),"calendarClassEditorScroll");check(editor&&editor->isVisible()&&editor->height()>=60,"classification editor remains accessible on the smallest screen");view->grabFramebuffer().save(output+"/calendar-classifications-edit-360.png");classes->setProperty("editing",false);
            }
            if(name=="saves"){
                auto *entry=qobject_cast<QuickNode *>(bridge->find("listView_simulationSaves"));auto *table=item(view->rootObject(),"dataTable-"+QString::number(entry->values().value("id").toInt()));const auto bounds=table->mapRectToItem(view->rootObject(),table->boundingRect());check(bounds.bottom()>=view->height()-24&&bounds.bottom()<=view->height(),"save selection gives the remaining viewport to the list");
            }
            image.save(output+QString("/%1-%2-%3.png").arg(name).arg(size.width()).arg(dark)); ++renders;
            if(name=="ux-explicit-form" && size.width()==360 && !dark){
                auto *workspace=item(view->rootObject(),"formGeneratorWorkspace");workspace->setProperty("section",1);QTest::qWait(70);auto *apply=item(view->rootObject(),"formApplyParameters");const auto bounds=apply->mapRectToItem(view->rootObject(),apply->boundingRect());check(apply->isVisible()&&bounds.top()>=0&&bounds.bottom()<=view->height(),"UX 1: Apply stays pinned and reachable on the smallest parameters screen");view->grabFramebuffer().save(output+"/ux-form-parameters-360.png");workspace->setProperty("section",0);
            }
            if(name=="ux-saved-rosters" && size.width()==360 && !dark){auto *rows=item(view->rootObject(),"rosterOrder");check(rows&&rows->isVisible()&&rows->height()>=44,"UX 3: narrow saved-list controls leave a usable roster viewport");auto *workspace=item(view->rootObject(),"rosterWorkspace");workspace->setProperty("choosing",true);QTest::qWait(60);auto *library=item(view->rootObject(),"rosterLibrary");check(library&&library->isVisible()&&library->height()>=44,"UX 3: available-athlete selection also retains visible rows on the smallest screen");view->grabFramebuffer().save(output+"/ux-roster-library-360.png");workspace->setProperty("choosing",false);}
            if(name=="calendar-custom-rules" && size.width()==360 && !dark) {
                clickControl(view,"calendarReturnToList");auto *calendar=item(view->rootObject(),"calendarWorkspace");auto *rows=item(view->rootObject(),"calendarEvents");
                if(!rows || rows->height()<80){presentation(window)->grab().save("/tmp/sj-calendar-narrow.png");qInfo()<<"CALENDAR"<<calendar->height()<<calendar->property("tight")<<(rows?rows->height():-1);}
                check(calendar && !calendar->property("editing").toBool() && rows && rows->isVisible() && rows->height()>=80,"smallest calendar viewport retains a usable list and direct return navigation");
                calendar->setProperty("editing",true);QTest::qWait(50);
            }
        }
    }
    if(sweep) {
        appearance->setTextScale(1.3); QTest::qWait(100);
        view->grabFramebuffer().save(output+"/"+name+"-large-text.png");
        appearance->setTextScale(1);
    }
    appearance->setDark(false);
    if(name=="competition-config" || name=="jump-config" || name=="settings") {
        check(bridge->root()->values().value("kind")=="tabs","configuration has separate sections");
        const int count=bridge->root()->values().value("options").toStringList().size();
        check(count>=3,"configuration section count");
        presentation(window)->resize(1024,640);
        for(int page=0;page<count;++page) {
            bridge->edit(bridge->root()->values().value("id").toInt(),page);QTest::qWait(200);
            check(bridge->root()->values().value("value").toInt()==page,"section changes through Quick adapter");
            check(!bridge->root()->children().isEmpty(),"section has content");
            view->grabFramebuffer().save(output+QString("/%1-tab-%2.png").arg(name).arg(page));++renders;
        }
        bridge->edit(bridge->root()->values().value("id").toInt(),0);QTest::qWait(150);
    }
}
static int id(QuickBridge *bridge,const QString &name) {
    auto *node=qobject_cast<QuickNode *>(bridge->find(name));
    check(node!=nullptr,"field "+name);
    return node->values().value("id").toInt();
}
static int buttonId(QuickNode *node,const QString &text) {
    if(node->values().value("kind")=="button" && node->values().value("text")==text) return node->values().value("id").toInt();
    for(const auto &child:node->children()+node->values().value("buttons").toList()) if(auto *entry=child.value<QuickNode *>()) { const int found=buttonId(entry,text);if(found>=0)return found; }
    return -1;
}
static void runCompetition(MainWindow &main,int scenario) {
    Q_UNUSED(main)
    auto *db=GlobalDatabase::get();
    CompetitionRules rules=db->getEditableCompetitionRules().first();
    rules.setHasDsq(false);rules.setCompetitionType(scenario==2?CompetitionRules::Team:CompetitionRules::Individual);
    rules.setJumpersInTeamCount(4);
    RoundInfo first(16,true),second(scenario==2?3:8,true);
    if(scenario==1) { first.setKO(true);first.setCountInKOGroup(2);first.setAdvancingFromKOGroup(1);first.setKoGroupsSelectionType(KOGroup::Classic); }
    rules.setRounds({first,second});
    CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition);
    config.getHillEditor()->setHill(&db->getEditableGlobalHills().first());config.getHillEditor()->fillHillInputs();
    config.getCompetitionRulesEditor()->setCompetitionRules(&rules);config.getCompetitionRulesEditor()->fillCompetitionRulesInputs();
    auto jumpers=config.getCompetitionJumpers();check(jumpers.size()>=16,"competition fixture");jumpers.resize(16);
    if(scenario==2) {
        QVector<Jumper *> all;for(auto &jumper:db->getEditableGlobalJumpers())all.append(&jumper);
        auto teams=Team::constructTeamsVectorByJumpersList(all,4);check(teams.size()>=4,"team fixture");teams.resize(4);jumpers.clear();
        for(auto &team:teams) { Team::cutTeamJumpers(&team,4);jumpers+=team.getJumpers(); } config.setCompetitionTeams(teams);
    }
    config.setCompetitionJumpers(jumpers);
    if(scenario==1)config.setCompetitionGroups(KOGroup::constructKOGroups(&rules.getRoundsReference()[0],&jumpers,KOGroup::Classic,nullptr));
    config.show();QTest::qWait(150);
    bool ran=false;
    QPointer<CompetitionManagerWindow> presented;
    QTimer messages;
    QObject::connect(&messages,&QTimer::timeout,[&] {
        for(auto *widget:QApplication::allWidgets()) if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()) {
            auto *bridge=box->findChild<QuickBridge *>();check(bridge!=nullptr,"Quick confirmation in real competition");bridge->refresh();
            auto *button=box->button(QMessageBox::Yes);if(!button)button=box->defaultButton();if(!button&&!box->buttons().isEmpty())button=box->buttons().first();
            check(button!=nullptr,"message action");const int action=buttonId(bridge->root(),button->text().remove('&'));if(action<0) { qCritical()<<"Missing message"<<button->text()<<box->buttons().size(); for(const auto &child:bridge->root()->children()) if(auto *n=child.value<QuickNode *>()) qCritical()<<n->values()<<n->children(); } check(action>=0,"projected message action");bridge->activate(action);
        }
    });messages.start(20);
    QTimer runner;
    QObject::connect(&runner,&QTimer::timeout,[&] {
        for(auto *widget:QApplication::allWidgets()) if(auto *window=qobject_cast<CompetitionManagerWindow *>(widget);window && window->isVisible()) {
            runner.stop();ran=true;presented=window;
            auto *bridge=window->findChild<QuickBridge *>();check(bridge!=nullptr,"fresh competition adapter");
            check(window->getManager()->getToBeatLineDistance()<0 && window->findChild<QLabel *>("label_toBeatDistance")->text()==QString::fromUtf8("—"),"the first athlete has no fictitious zero-metre leader target");
            if(scenario==3) {
                const double speed=GlobalAppSettings::get()->getLiveCompetitionSpeedMulti();GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(1);
                window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(true);
                QObject watcher;bool closed=false;QObject::connect(window,&QDialog::finished,&watcher,[&]{closed=true;});
                QElapsedTimer elapsed;elapsed.start();QTimer::singleShot(20,window,&QDialog::reject);
                bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(180);
                check(closed && elapsed.elapsed()<1000,"closing a live competition cancels the presentation delay safely");
                GlobalAppSettings::get()->setLiveCompetition(false);GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(speed);return;
            }
            window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(false);
            render(window,QString("live-%1").arg(scenario),true);
            if(scenario==0) {
                main.resize(1024,640);main.activateWindow();scene(window)->setFocus();QTest::qWait(70);
                clickControl(scene(window),"raceLiveToggle");check(window->findChild<QCheckBox *>("checkBox_liveCompetition")->isChecked(),"UX 4: live presentation is directly available in the competition toolbar");clickControl(scene(window),"raceLiveToggle");
                clickControl(scene(window),"raceWindConditions");auto *optionsWorkspace=item(scene(window)->quickWindow()->contentItem(),"raceOptionsWorkspace");check(optionsWorkspace&&optionsWorkspace->property("section").toInt()==1,"UX 4: wind opens the conditions section directly");
                auto *optionsScroll=item(scene(window)->quickWindow()->contentItem(),"raceOptionsScroll");
                check(optionsScroll && optionsScroll->property("contentHeight").toReal()<=optionsScroll->height(),"conditions fit the competition options viewport");
                auto *optionsView=scene(window);
                const QPoint wheelPoint=optionsScroll->mapToItem(optionsView->rootObject(),QPointF(optionsScroll->width()-20,optionsScroll->height()/2)).toPoint();
                QTest::mouseMove(optionsView,wheelPoint);QTest::qWait(30);
                QWheelEvent wheel(wheelPoint,optionsView->mapToGlobal(wheelPoint),QPoint(),QPoint(0,-120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
                QApplication::sendEvent(optionsView,&wheel);QTest::qWait(60);
                check(!optionsScroll->property("interactive").toBool() && optionsScroll->property("contentY").toReal()==0,"mouse and wheel input do not enable scrolling when competition options fit");
                const QPoint dragEnd=wheelPoint-QPoint(0,60);
                QTest::mousePress(optionsView,Qt::LeftButton,Qt::NoModifier,wheelPoint);
                for(int step=1;step<=6;++step) {const QPoint at=wheelPoint+(dragEnd-wheelPoint)*step/6;QMouseEvent move(QEvent::MouseMove,at,optionsView->mapToGlobal(at),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(optionsView,&move);QTest::qWait(15);}
                QTest::mouseRelease(optionsView,Qt::LeftButton,Qt::NoModifier,dragEnd);QTest::qWait(60);
                check(optionsScroll->property("contentY").toReal()==0,"dragging fitted competition options does not move their content");
                const qreal panelHeight=optionsScroll->height();optionsScroll->setHeight(80);QTest::qWait(40);
                check(optionsScroll->property("interactive").toBool(),"competition options remain scrollable when their content overflows");
                const QPoint overflowPoint=optionsScroll->mapToItem(optionsView->rootObject(),QPointF(optionsScroll->width()-20,40)).toPoint();
                QWheelEvent overflowWheel(overflowPoint,optionsView->mapToGlobal(overflowPoint),QPoint(),QPoint(0,-120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
                QApplication::sendEvent(optionsView,&overflowWheel);QTest::qWait(80);
                check(optionsScroll->property("contentY").toReal()>0,"overflowing competition options can actually be scrolled by wheel");
                optionsScroll->setHeight(panelHeight);QTest::qWait(60);
                check(optionsScroll->property("contentY").toReal()==0 && !optionsScroll->property("interactive").toBool(),"restoring sufficient space resets and disables competition options scrolling");
                QSignalSpy windGenerated(window->findChild<QPushButton *>("pushButton_generateNewWinds"),&QPushButton::clicked);clickControl(scene(window),"action-pushButton_generateNewWinds");check(windGenerated.count()==1,"UX 4: generating wind uses the original competition action");QTest::keyClick(scene(window),Qt::Key_Escape);QTest::qWait(220);
                bridge->edit(id(bridge,"spinBox_actualGate"),17);
                for(const QString &name:{QString("pushButton_windsGeneratorSettings"),QString("pushButton_inrunSnowGeneratorSettings"),QString("pushButton_manipulateJump"),QString("pushButton_coachGate")}) {
                    bool seen=false;QTimer closeEditor;
                    QObject::connect(&closeEditor,&QTimer::timeout,[&] {
                        for(auto *widget:QApplication::allWidgets()) if(auto *editor=qobject_cast<QDialog *>(widget);editor && editor!=window && (editor->parentWidget()==window || editor->property("sjQuickOwner").value<QObject *>()==window) && editor->isVisible() && !qobject_cast<QMessageBox *>(editor)) {
                            closeEditor.stop();seen=true;render(editor,"live-editor-"+name);editor->reject();
                        }
                    });closeEditor.start(20);bridge->activate(id(bridge,name));QTest::qWait(100);closeEditor.stop();check(seen,"live editor opens through Quick: "+name);
                }
                {
                    auto *resultNode=qobject_cast<QuickNode *>(bridge->find("tableView_results"));
                    auto *adapter=qobject_cast<QuickTableModel *>(resultNode->values().value("table").value<QObject *>());
                    check(adapter!=nullptr,"live results adapter");const int countBefore=adapter->count();
                    const double speed=GlobalAppSettings::get()->getLiveCompetitionSpeedMulti();GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(.005);
                    QObject watcher;bool frozen=false;
                    QObject::connect(window,&CompetitionManagerWindow::liveStageChanged,&watcher,[&](int stage){if(stage==2){check(adapter->count()==countBefore,"live presentation does not reveal results early");frozen=true;}});
                    window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(true);
                    bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(300);
                    check(frozen && window->getLiveStage()==0,"live presentation completes all stages");
                    const double displayedLine=window->getManager()->getToBeatLineDistance();window->getManager()->updateToBeatLineDistance();
                    check(displayedLine==window->getManager()->getToBeatLineDistance(),"live leader line is refreshed for the next athlete and current wind");
                    check(adapter->count()==countBefore+1,"live results are released after presentation");
                    auto *table=item(scene(window)->rootObject(),"raceResults");const auto columns=table->property("displayColumns").value<QJSValue>().toVariant().toList();
                    check(columns.contains(3)&&columns.contains(2),"default competition results show distance and total points together");
                    auto *distance=item(scene(window)->rootObject(),"lastJumpDistance");check(distance&&distance->property("text").toString()==bridge->jumpSummary().value("distance").toString(),"last jump distance uses revealed real jump data");
                    const auto summary=bridge->jumpSummary();check(summary.contains("gate")&&summary.contains("windPoints")&&summary.contains("gatePoints")&&summary.contains("judgesPoints"),"last jump exposes gate, compensation components and judges total directly");
                    const auto marks=summary.value("judges").toList();int discarded=0;for(const auto &mark:marks)if(mark.toMap().value("discarded").toBool())++discarded;check(marks.size()==5&&discarded==2,"last jump preserves all five judges and marks exactly two discarded scores");
                    auto *resultWidget=window->findChild<JumperCompetitionResultsWidget *>();auto *jumpTabs=resultWidget->findChild<QTabWidget *>("tabWidget_jumps");auto *nativeInfo=qobject_cast<JumpDataDetailedInfoWindow *>(jumpTabs->currentWidget());check(nativeInfo&&nativeInfo->getJumpData(),"last jump has native result data");
                    auto *original=nativeInfo->getJumpData();JumpData display=*original;nativeInfo->setJumpData(&display);auto *windLabel=nativeInfo->findChild<QLabel *>("label_averageWind");auto *compensationLabel=nativeInfo->findChild<QLabel *>("label_totalCompensation");const auto windText=windLabel->text(),compensationText=compensationLabel->text();
                    for(int sign:{-1,0,1}){display.setAveragedWind(sign);display.setWindCompensation(-sign*3);display.setGateCompensation(0);display.setTotalCompensation(-sign*3);windLabel->setText(QString::number(sign)+"m/s");compensationLabel->setText(QString::number(-sign*3)+"pkt");bridge->refresh();QTest::qWait(50);for(const auto &name:{"lastJumpWind","lastJumpCompensation"}){const int value=QString(name)=="lastJumpWind"?sign:-sign;const QColor expected(value<0?"#a32f25":value>0?"#22633a":"#112b49");check(item(scene(window)->rootObject(),name)->property("valueColor").value<QColor>()==expected,"wind and total compensation independently distinguish negative, zero and positive values");}}
                    nativeInfo->setJumpData(original);windLabel->setText(windText);compensationLabel->setText(compensationText);bridge->refresh();render(window,"last-jump-full",true);main.resize(1024,640);QTest::qWait(100);
                    check(!bridge->jumpSummary().value("flag").toString().isEmpty(),"last jump exposes its own athlete flag");
                    check(bridge->jumpSummary().value("positionNumber").toInt()>0 && bridge->jumpSummary().value("leaderGap").toDouble()>=0,"last jump exposes rank and nonnegative leader gap");
                    main.activateWindow();scene(window)->setFocus();QTest::qWait(100);
                    for(const auto &category:QList<QPair<QString,int>>{{"raceConditionsOptions",1},{"raceJumpOptions",2},{"raceOptions",0}}){
                        clickControl(scene(window),category.first);auto *workspace=item(scene(window)->quickWindow()->contentItem(),"raceOptionsWorkspace");check(workspace&&workspace->property("section").toInt()==category.second,"toolbar opens the requested competition category directly");QTest::keyClick(scene(window),Qt::Key_Escape);QTest::qWait(220);
                    }
                    main.activateWindow();scene(window)->setFocus();QTest::qWait(50);clickControl(scene(window),"raceOptions");auto *options=scene(window)->rootObject()->findChild<QObject *>("raceOptionsPanel");check(options&&options->property("visible").toBool(),"secondary competition tools open through one menu");QTest::keyClick(scene(window),Qt::Key_Escape);QTest::qWait(220);check(!options->property("visible").toBool(),"competition tools close with Escape");

                    window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(false);GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(speed);
                }
                const int before=window->getManager()->getActualStartListIndex();bridge->activate(id(bridge,"toolButton_autoSimulationActions"),1);QTest::qWait(100);
                check(window->getManager()->getActualStartListIndex()!=before || window->getManager()->checkRoundEnd(),"automatic simulation menu advances real round");
            }
            auto *startView=window->findChild<QListView *>("listView_startList");check(startView,"source start list exists");
            auto *startNode=qobject_cast<QuickNode *>(bridge->find("listView_startList"));auto *startAdapter=qobject_cast<QuickTableModel *>(startNode->values().value("table").value<QObject *>());check(startAdapter,"Quick start list exists");
            QAbstractItemModelTester sourceStartContract(startView->model(),QAbstractItemModelTester::FailureReportingMode::Fatal);
            QAbstractItemModelTester quickStartContract(startAdapter,QAbstractItemModelTester::FailureReportingMode::Fatal);
            const auto checkStartList=[&]{
                const auto statuses=window->getManager()->getStartListStatusesReference();
                check(startAdapter->count()==statuses.size(),"start list row count follows round/group changes");
                for(int row=0;row<statuses.size();++row){const auto cells=startAdapter->data(startAdapter->index(row),Qt::UserRole+1).toList();check(!cells.isEmpty()&&cells[0].toMap().value("text").toString()==startView->model()->index(row,0).data().toString(),"Quick rows match the current source start list");check(cells[0].toMap().value("text").toString().contains(statuses[row].getJumper()->getNameAndSurname()),"every visible start-list row belongs to the current round/group");}
                check(startAdapter->currentRow()==startView->currentIndex().row(),"start list cursor follows the active competitor");
            };
            checkStartList();
            int jumps=0;bool previousRankTested=false;
            while(!window->getManager()->checkCompetitionEnd() && jumps++<250) {
                auto *next=window->findChild<QPushButton *>("pushButton_goToNext");
                auto *jump=window->findChild<QPushButton *>("pushButton_jump");
                if(next && !next->isHidden() && next->isEnabled()) bridge->activate(id(bridge,"pushButton_goToNext"));
                else if(scenario==0 && window->getManager()->getActualRound()>1 && !previousRankTested) {
                    bridge->refresh();const int previous=bridge->context().value("previousPosition").toInt();check(previous>0,"athlete on gate shows place after previous round");
                    const double speed=GlobalAppSettings::get()->getLiveCompetitionSpeedMulti();GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(.005);window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(true);
                    const auto advancementBefore=bridge->context().value("biggestAdvancement").toMap();
                    QObject watcher;bool seen=false;
                    QObject::connect(window,&CompetitionManagerWindow::liveStageChanged,&watcher,[&](int stage){if(stage==2){bridge->refresh();check(!bridge->jumpSummary().contains("positionNumber"),"last jump leaves position blank during flight");check(!bridge->jumpSummary().contains("leaderGap"),"current leader gap is not revealed during flight");check(bridge->context().value("biggestAdvancement").toMap()==advancementBefore,"biggest advancement stays frozen until live results are revealed");seen=true;}});
                    bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(300);bridge->refresh();check(seen&&!bridge->jumpSummary().contains("previousRound")&&bridge->jumpSummary().value("positionNumber").toInt()>0,"final result replaces previous place with current rank");
                    window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(false);GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(speed);previousRankTested=true;
                }
                else { check(jump&&jump->isEnabled(),"jump remains operable");if(scenario==2 && window->getManager()->getActualRound()>1){bridge->refresh();check(bridge->context().value("previousPosition").toInt()>0,"team on gate shows the team place after previous round");}bridge->activate(id(bridge,"pushButton_jump")); }
                QTest::qWait(10);bridge->refresh();checkStartList();
            }
            check(scenario!=0||previousRankTested,"previous-round rank transition was exercised in a real second round");
            check(window->getManager()->checkCompetitionEnd(),"competition finishes");
            check(!window->getManager()->getResults()->getResultsReference().isEmpty(),"competition has actual results");
            render(window,QString("live-results-%1").arg(scenario),scenario==2);
            bridge->refresh();
            check(bridge->context().value("biggestAdvancement").toMap()==QuickRaceStats::biggestAdvancement(*window->getManager()->getResults(),window->getManager()->getActualRound(),scenario==2?4:1),"competition header exposes the largest revealed advancement");
            check(!bridge->context().contains("leaderName"),"competition header no longer duplicates the leader");
            const auto finalSummary=bridge->jumpSummary();CompetitionSingleResult *shown=nullptr;
            for(auto &result:window->getManager()->getResults()->getResultsReference()) {
                if(scenario==2){for(const auto &athlete:result.getTeamJumpersResultsReference())if(athlete.getJumper()&&athlete.getJumper()->getNameAndSurname()==finalSummary.value("name").toString())shown=&result;}
                else if(result.getJumper()&&result.getJumper()->getNameAndSurname()==finalSummary.value("name").toString())shown=&result;
            }
            check(shown && finalSummary.value("positionNumber").toInt()==shown->getPosition(),"last jump rank uses the shown athlete or aggregate team");
            double leading=shown->getPointsSum();for(const auto &result:window->getManager()->getResults()->getResultsReference())leading=qMax(leading,result.getPointsSum());
            check(std::abs(finalSummary.value("leaderGap").toDouble()-qRound64((leading-shown->getPointsSum())*10)/10.)<.01,"leader gap belongs to the last jump rather than the next athlete on gate");
            check(!bridge->jumpSummary().value("name").toString().isEmpty() && !bridge->jumpSummary().value("distance").toString().isEmpty(),"live jump summary follows real result data");
            if(scenario==0) {
                finishedCompetition=std::make_unique<CompetitionInfo>(*window->getManager()->getCompetitionInfo());
                finishedCompetition->setHill(&db->getEditableGlobalHills().first());
                finishedCompetition->getResultsReference().setCompetition(finishedCompetition.get());
                for(auto &result:finishedCompetition->getResultsReference().getResultsReference()) {
                    result.setCompetition(finishedCompetition.get());
                    for(auto &jump:result.getJumpsReference()) { jump.setCompetition(finishedCompetition.get());jump.setSingleResult(&result);jump.setHill(finishedCompetition->getHill());jump.setSimulator(nullptr); }
                }
            }
            main.resize(1200,800);QTest::qWait(180);
            clickControl(scene(window),"action-pushButton_goToNext");
        }
    });runner.start(25);
    auto *bridge=config.findChild<QuickBridge *>();bridge->activate(id(bridge,"pushButton_submit"));
    QTest::qWait(300);runner.stop();messages.stop();check(ran,"real competition started");
    check(presented.isNull(),"finished competition window and its pending UI updates are destroyed before simulation data");
    config.hide();
}
static void seasonToolRegressions(SimulationSaveManagerWindow &manager);
static void seasons() {
    QTimer messages;
    QObject::connect(&messages,&QTimer::timeout,[] {
        for(auto *widget:QApplication::allWidgets()) if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()) {
            auto *bridge=box->findChild<QuickBridge *>();check(bridge!=nullptr,"season message uses Quick");bridge->refresh();
            QAbstractButton *button=box->defaultButton();if(!button&&!box->buttons().isEmpty())button=box->buttons().first();
            if(button) { const int action=buttonId(bridge->root(),button->text().remove('&'));if(action>=0)bridge->activate(action); }
        }
    });messages.start(20);
    check(finishedCompetition!=nullptr,"completed competition fixture");
    SimulationSave save;save.setName("Quick UI test");
    {
        NewSeasonConfiguratorWindow setup;render(&setup,"new-season",true);
        auto *bridge=setup.findChild<QuickBridge *>();auto *box=setup.getToolBox();
        for(int i=0;i<box->count();++i) { bridge->edit(id(bridge,box->objectName()),i);QTest::qWait(100);render(&setup,QString("new-season-page-%1").arg(i)); }
        save.setJumpers(setup.getJumpersReference());save.setHills(setup.getHillsReference());save.setCompetitionRules(setup.getCompetitionsRulesReference());
        setup.hide();
    }
    QHash<Jumper *,Jumper *> clones;
    const auto &original=GlobalDatabase::get()->getEditableGlobalJumpers();
    for(int i=0;i<original.size();++i)clones.insert(const_cast<Jumper *>(&original[i]),save.getJumpersReference()[i]);
    auto *competition=finishedCompetition.release();competition->setHill(save.getHillsReference().first());competition->setPlayed(true);
    for(auto &result:competition->getResultsReference().getResultsReference()) {
        result.setJumper(clones.value(result.getJumper()));check(result.getJumper()!=nullptr,"independent season athlete");
        for(auto &jump:result.getJumpsReference()) { jump.setJumper(result.getJumper());jump.setHill(competition->getHill()); }
    }
    for(auto *&jumper:competition->getStartListReference())jumper=clones.value(jumper);
    auto *calendar=new SeasonCalendar("Test World Cup");calendar->setCompetitions({competition});
    auto *classification=new Classification("Test World Cup");classification->setPunctationType(Classification::CompetitionPoints);calendar->setClassifications({classification});competition->setClassifications({classification});
    for(auto &result:competition->getResultsReference().getResultsReference()) {
        auto *standing=new ClassificationSingleResult(classification,result.getJumper());standing->addCompetitionResults(competition->getResultsReference());standing->updateSingleResults();standing->updatePointsSum();classification->getResultsReference().append(standing);
    }classification->sortInDescendingOrder();
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);
    save.getSeasonsReference().append(season);save.setActualSeason(&save.getSeasonsReference().last());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    {
        SimulationSaveManagerWindow manager(&save);manager.updateCompetitionConfigButton();manager.fillNextCompetitionInformations();render(&manager,"season-manager",true);
        messages.stop();seasonToolRegressions(manager);messages.start(20);
        auto *bridge=manager.findChild<QuickBridge *>();auto *tabs=manager.findChild<QTabWidget *>("toolBox");
        check(bridge->root()->values().value("kind")=="tabs" && bridge->root()->values().value("options").toStringList().size()==tabs->count(),"season overview and every workspace remain available with classifications inside calendars");
        auto *tools=qobject_cast<QuickNode *>(bridge->find("quickSeasonTools"));
        check(tools && tools->values().value("actions").toList().size()==4,"season tools preserve form, statistics, save-copy and repair actions");
        for(int i=0;i<tabs->count();++i) {
            if(i==4)continue;
            bridge->edit(bridge->root()->values().value("id").toInt(),i<4?i+1:i);QTest::qWait(100);
            check(tabs->currentIndex()==i,"season task navigation updates original controller state");
            render(&manager,QString("season-page-%1").arg(i));
            auto *action=item(scene(&manager)->rootObject(),"seasonNextAction");
            if(i==3 && scene(&manager)->height()<600) {
                check(!action || !action->isVisible(),"short calendar workspace reserves space for editing");continue;
            }
            check(action && action->isVisible(),"next competition remains available outside the overview");
            const auto pos=action->mapToItem(scene(&manager)->rootObject(),QPointF());
            check(pos.y()>=0 && pos.y()+action->height()<=scene(&manager)->height(),"next competition action remains in the viewport");
        }
        auto *archive=manager.findChild<QAbstractItemView *>("listView_competitionsArchive");
        check(archive && archive->model()->rowCount()>0,"real season archive");
        QMetaObject::invokeMethod(archive,"doubleClicked",Q_ARG(QModelIndex,archive->model()->index(0,0)));QTest::qWait(150);
        for(auto *dialog:applicationWindow->findChildren<QDialog *>()) if(dialog->property("sjQuickOwner").value<QObject *>()==&manager && dialog->isVisible()) { render(dialog,"archive-results");dialog->hide(); }
        manager.hide();
    }
    {
        JumperStatsWindow stats;stats.setSave(&save);stats.setupComboBox();stats.setJumper(competition->getResultsReference().getResultsReference().first().getJumper());
        stats.getRangeComboBoxes()->setSeasonsList(&save.getSeasonsReference());stats.getRangeComboBoxes()->setMergeCalendars(stats.getMergeCalendarsCheckBox()->isChecked());
        stats.getRangeComboBoxes()->setupComboBoxes();stats.getRangeComboBoxes()->setupConnections();stats.getClassificationsCheckBoxes()->setClassificationsList(&calendar->getClassificationsReference());stats.getClassificationsCheckBoxes()->setupCheckBoxes();stats.setupConnections();stats.fillWindow();render(&stats,"athlete-stats",true);stats.hide();
    }
    {
        SimulationRatingsWindow stats(&save);stats.getClassificationsCheckBoxes()->setClassificationsList(&calendar->getClassificationsReference());stats.getClassificationsCheckBoxes()->setupCheckBoxes();stats.setFilteredJumpers(save.getJumpersReference());stats.setupComboBox();stats.setupConnections();
        stats.getRangeComboBoxes()->setSeasonsList(&save.getSeasonsReference());stats.getRangeComboBoxes()->setMergeCalendars(stats.getMergeCalendarsCheckBox()->isChecked());stats.getRangeComboBoxes()->setupComboBoxes();stats.getRangeComboBoxes()->setupConnections();stats.fillWindow();render(&stats,"ratings",true);
        auto *tabs=stats.findChild<QTabWidget *>();auto *bridge=stats.findChild<QuickBridge *>();for(int i=0;i<tabs->count();++i){bridge->edit(id(bridge,tabs->objectName()),i);QTest::qWait(80);render(&stats,QString("ratings-tab-%1").arg(i));}stats.hide();
    }
    {
        ApperanceInClassificationWindow stats;stats.setSeason(save.getActualSeason());stats.setClassification(classification);stats.setJumper(competition->getResultsReference().getResultsReference().first().getJumper());stats.setArchiveResults({{competition,1}});stats.fillWindow();render(&stats,"classification-history",true);stats.hide();
    }
    { StartListByOtherCompetitionConfigWindow dialog;dialog.setSave(&save);dialog.setCompetitionType(CompetitionRules::Individual);dialog.setupComboBox();render(&dialog,"start-list-from-competition");dialog.hide(); }
    { EditStartListWithJumpersListsWindow dialog;dialog.setLists(&save.getJumpersListsReference());dialog.setupWidgets();render(&dialog,"start-list-presets");dialog.hide(); }
    { MultipleTrainingsEditDialog dialog;dialog.setRulesList(&save.getCompetitionRulesReference());render(&dialog,"multiple-trainings");dialog.hide(); }
    { MultipleTrialRoundsEditDialog dialog;dialog.setRulesList(&save.getCompetitionRulesReference());render(&dialog,"multiple-trial-rounds");dialog.hide(); }
}
static void navigationRegressions(MainWindow &main) {
    main.show();QTest::qWait(100);
    auto *navigation=main.findChild<QuickUi *>();check(navigation && !navigation->canGoBack(),"navigation starts at home");
    QDialog first(&main);first.setWindowTitle("First page");auto *layout=new QVBoxLayout(&first);auto *name=new QLineEdit("Retained",&first);layout->addWidget(name);
    QTimer::singleShot(50,&first,[&]{
        check(navigation->current()==first.findChild<QuickBridge *>(),"exec controller becomes the current page");
        check(QApplication::activeModalWidget()==nullptr,"task pages do not block the application as modal dialogs");
        QDialog second(&first);second.setWindowTitle("Second page");auto *form=new QVBoxLayout(&second);form->addWidget(new QLineEdit("Child",&second));
        QTimer::singleShot(50,&second,[&]{
            check(navigation->path().size()==3 && scene(&second)->window()==&main,"nested task uses the same application window");
            check(!item(scene(&second)->rootObject(),"releaseBadge"),"child task does not repeat the version badge");
            auto *header=main.findChild<QQuickWidget *>("quickApplicationHeader");auto *back=item(header->rootObject(),"navigateBack");check(back && back->isVisible(),"shared back control is visible");
            QTest::mouseClick(header,Qt::LeftButton,Qt::NoModifier,back->mapToItem(header->rootObject(),back->boundingRect().center()).toPoint());
        });
        check(second.exec()==QDialog::Rejected,"shared back control returns from a nested exec page");
        check(navigation->current()==first.findChild<QuickBridge *>() && name->text()=="Retained","return preserves the parent editor state");
        navigation->showAppearance();QTest::qWait(50);check(navigation->current()==first.findChild<QuickBridge *>(),"global appearance does not add another navigation page");navigation->closeAppearance();
        navigation->back();
    });
    check(first.exec()==QDialog::Rejected,"back finishes the parent controller safely");
    check(!navigation->canGoBack() && navigation->current()==main.findChild<QuickBridge *>(),"navigation returns home after nested pages finish");
    qInfo()<<"PASS: one application window, nested navigation, state retention and shared appearance";
}
static void menuRegressions(MainWindow &main) {
    main.show();QDialog dialog(&main);dialog.setWindowTitle("Menu test");auto *layout=new QVBoxLayout(&dialog);auto *button=new QPushButton("Tools",&dialog);button->setObjectName("testTools");layout->addWidget(button);
    auto *menu=new QMenu(button);auto *action=menu->addAction("Open editor");button->setMenu(menu);QSignalSpy triggered(action,&QAction::triggered);
    dialog.show();QTest::qWait(100);auto *view=scene(&dialog);auto *control=item(view->rootObject(),"action-testTools");check(control!=nullptr,"menu button is visible");
    QSignalSpy clicked(control,SIGNAL(clicked()));const auto clickPoint=control->mapToItem(view->rootObject(),control->boundingRect().center()).toPoint();QTest::mouseMove(view,clickPoint);QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,clickPoint);QTest::qWait(20);QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,clickPoint);QTest::qWait(80);
    auto *popup=control->findChild<QObject *>("actionMenu");check(popup && popup->property("visible").toBool() && popup->property("count").toInt()==1,QString("clicking a menu button opens populated menu items (clicked %1, visible %2, count %3, geometry %4x%5, enabled %6)").arg(clicked.size()).arg(popup?popup->property("visible").toBool():false).arg(popup?popup->property("count").toInt():-1).arg(control->width()).arg(control->height()).arg(control->isEnabled()));
    auto *entry=item(view->quickWindow()->contentItem(),"menu-entry-0");check(entry && entry->isVisible(),"menu entry is visible on screen");
    QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,entry->mapToItem(view->rootObject(),entry->boundingRect().center()).toPoint());QTest::qWait(50);
    check(triggered.size()==1,"clicking a menu entry invokes its real source action");dialog.reject();
    QDialog tableDialog(&main);auto *tableLayout=new QVBoxLayout(&tableDialog);auto *source=new QTableWidget(3,1,&tableDialog);source->setObjectName("menuTable");source->setSelectionMode(QAbstractItemView::ExtendedSelection);tableLayout->addWidget(source);
    for(int i=0;i<3;++i)source->setItem(i,0,new QTableWidgetItem(QString::number(i)));
    auto *tableAction=new QAction("Table action",&tableDialog);tableAction->setShortcut(QKeySequence("Ctrl+Shift+T"));tableDialog.addAction(tableAction);QSignalSpy tableTriggered(tableAction,&QAction::triggered);
    tableDialog.show();QTest::qWait(100);auto *tableBridge=tableDialog.findChild<QuickBridge *>();
    check(tableBridge->actions().size()==1,"table source action is registered");
    source->parentWidget()->setProperty("quickTableActions",tableBridge->actions());tableBridge->refresh();QTest::qWait(80);
    auto *tableNode=qobject_cast<QuickNode *>(tableBridge->find("menuTable"));auto *tableView=scene(&tableDialog);
    const auto optionsName="table-options-"+QString::number(tableNode->values().value("id").toInt());clickControl(tableView,optionsName);
    auto *options=item(tableView->rootObject(),optionsName)->findChild<QObject *>("actionMenu");
    check(options && options->property("count").toInt()==2,"table menu contains select-all and the source action");
    clickControl(tableView,"menu-entry-1");check(tableTriggered.size()==1,"table menu invokes its source action");
    clickControl(tableView,optionsName);clickControl(tableView,"menu-entry-0");check(source->selectionModel()->selectedRows().size()==3,"table menu selects all source rows");tableDialog.reject();
    qInfo()<<"PASS: real pointer clicks open menus and activate source actions";
}
static void clickControl(QQuickWidget *view,const QString &name) {
    auto *control=item(view->quickWindow()->contentItem(),name);
    if(!control || !control->isVisible() || !control->isEnabled()){applicationWindow->grab().save("/tmp/sj-click-failure.png");qInfo()<<"Control failure"<<name<<control<<(control?control->isVisible():false)<<(control?control->isEnabled():false);}
    check(control && control->isVisible() && control->isEnabled(),"clickable control: "+name);
    const auto point=control->mapToItem(view->rootObject(),control->boundingRect().center()).toPoint();
    check(view->rect().contains(point),"control inside viewport: "+name);
    QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,point);
    QTest::qWait(80);
}
static void seasonToolRegressions(SimulationSaveManagerWindow &manager) {
    auto *view=scene(&manager);auto *navigation=applicationWindow->findChild<QuickUi *>();
    presentation(&manager)->resize(1200,800);QTest::qWait(80);
    for(int action=0;action<4;++action){
        clickControl(view,"action-quickSeasonTools");QTest::qWait(250);
        bool opened=false;int ticks=0;QTimer watcher;
        QObject::connect(&watcher,&QTimer::timeout,[&]{
            check(++ticks<150,"season tool opens");
            for(auto *widget:QApplication::allWidgets()){
                auto *dialog=qobject_cast<QDialog *>(widget);if(!dialog || !dialog->isVisible() || dialog==&manager)continue;
                const bool matches=action==0?qobject_cast<JumpersFormGeneratorConfigWindow *>(dialog)!=nullptr:action==1?qobject_cast<SimulationRatingsWindow *>(dialog)!=nullptr:action==2?qobject_cast<QFileDialog *>(dialog)!=nullptr:qobject_cast<QMessageBox *>(dialog)!=nullptr;
                if(!matches)continue;
                opened=true;watcher.stop();
                if(action<2){check(navigation->current()==dialog->findChild<QuickBridge *>(),"season tool is current navigation page");clickControl(applicationWindow->findChild<QQuickWidget *>("quickApplicationHeader"),"navigateBack");}
                else if(action==2)dialog->reject();
                else {auto *message=qobject_cast<QMessageBox *>(dialog);auto *button=message->button(QMessageBox::No);check(button,"repair confirmation allows cancellation");auto *bridge=message->findChild<QuickBridge *>();bridge->refresh();QTest::qWait(50);clickControl(scene(message),"action-"+(button->objectName().isEmpty()?button->text().remove('&'):button->objectName()));}
                break;
            }
        });watcher.start(20);clickControl(view,QString("menu-entry-%1").arg(action));watcher.stop();
        check(opened && navigation->current()==manager.findChild<QuickBridge *>(),QString("season tool %1 returns to manager (opened %2, path %3)").arg(action).arg(opened).arg(navigation->path().join(" / ")));
    }
    qInfo()<<"PASS: all four season tools open through pointer-driven menus";
}
static void hillSearchRegression(QQuickWidget *view,const QString &name,const QStringList &labels) {
    auto *control=item(view->rootObject(),name);check(control && control->property("searchable").toBool(),"hill search is enabled at the intended selector");
    const int previous=control->property("currentIndex").toInt();int target=2;
    while(target<labels.size() && labels.count(labels[target])!=1)++target;
    check(target<labels.size(),"hill search fixture has a unique name");
    clickControl(view,name);auto *popup=control->property("popup").value<QObject *>();
    auto *search=popup->findChild<QQuickItem *>("hillChoiceSearch");auto *choices=popup->findChild<QQuickItem *>("choiceResults");
    check(search && search->isVisible() && search->hasActiveFocus() && choices,"opening a searchable hill selector focuses its discreet search field");
    search->setProperty("text","no-such-hill-xyz");QTest::qWait(60);
    check(choices->property("count").toInt()==0,"unknown hill search has no results");
    QTest::keyClick(view,Qt::Key_Return);QTest::qWait(30);check(control->property("currentIndex").toInt()==previous,"empty search does not change the selected hill");
    QTest::keyClick(view,Qt::Key_Escape);QTest::qWait(80);check(!popup->property("visible").toBool() && control->property("currentIndex").toInt()==previous,"cancelling hill search keeps the original selection");
    clickControl(view,name);search->setProperty("text",labels[target].toUpper());QTest::qWait(60);
    check(choices->property("count").toInt()==1,"hill search is case insensitive and filters the actual options");
    QTest::keyClick(view,Qt::Key_Down);QTest::keyClick(view,Qt::Key_Up);QTest::keyClick(view,Qt::Key_Return);QTest::qWait(100);
    check(!popup->property("visible").toBool() && control->property("currentIndex").toInt()==target,"filtered hill selection maps back to the original source index");
    clickControl(view,name);check(search->property("text").toString().isEmpty() && choices->property("count").toInt()==labels.size(),"reopening hill search restores the full list");QTest::keyClick(view,Qt::Key_Escape);QTest::qWait(60);
}
static void seasonWorkflowRegressions(MainWindow &main) {
    main.show();main.resize(1200,800);
    {
        CompetitionConfigWindow preparation(CompetitionConfigWindow::SingleCompetition);
        auto *hill=preparation.findChild<QComboBox *>("comboBox_existingHill");
        preparation.show();QTest::qWait(150);
        auto *preparationBridge=preparation.findChild<QuickBridge *>();
        auto *hillNode=qobject_cast<QuickNode *>(preparationBridge->find("comboBox_existingHill"));
        hillSearchRegression(scene(&preparation),"choice-comboBox_existingHill",hillNode->values().value("options").toStringList());
        check(hill->currentIndex()==item(scene(&preparation)->rootObject(),"choice-comboBox_existingHill")->property("currentIndex").toInt(),"searched hill updates the real competition configuration");
        hill->setCurrentIndex(1);
        auto *gate=preparation.findChild<QSpinBox *>("spinBox_startGate");
        check(preparation.suggestedGate()>=0 && gate->value()==preparation.suggestedGate() && gate->specialValueText().isEmpty(),"choosing a hill calculates a numeric gate for a single competition");
    }
    SimulationSave save;save.setName("Workflow test");
    {
        NewSeasonConfiguratorWindow setup;
        save.setJumpers(setup.getJumpersReference());save.setHills(setup.getHillsReference());save.setCompetitionRules(setup.getCompetitionsRulesReference());
    }
    auto *calendar=new SeasonCalendar("Workflow calendar");
    auto *history=new CompetitionInfo(save.getHillsReference().first());history->setRules(save.getCompetitionRulesReference().first());history->setPlayed(true);
    auto *first=new CompetitionInfo(save.getHillsReference().first());first->setRules(history->getRules());
    auto *second=new CompetitionInfo(save.getHillsReference().first());second->setRules(history->getRules());
    calendar->setCompetitions({history,first,second});
    auto *classification=new Classification("Workflow classification");calendar->setClassifications({classification});first->setClassifications({classification});
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);
    save.getSeasonsReference().append(season);save.setActualSeason(&save.getSeasonsReference().last());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    {
        CompetitionConfigWindow preparation(CompetitionConfigWindow::SeasonCompetition,&main,&save);preparation.show();QTest::qWait(120);
        auto *preparationBridge=preparation.findChild<QuickBridge *>();check(preparationBridge,"season preparation uses Quick");
        const auto pages=preparationBridge->root()->values().value("options").toStringList();
        auto *gate=preparation.findChild<QSpinBox *>("spinBox_startGate");check(gate->value()==preparation.suggestedGate() && gate->value()>=0 && gate->specialValueText().isEmpty(),"competition preparation starts with a calculated numeric gate");
        const int recommendation=preparation.suggestedGate();check(recommendation>=0 && preparation.suggestedGate()==recommendation,"automatic gate preview is deterministic");
        auto *windEditor=preparation.getWindGeneratorSettingsEditor();const auto originalWind=*windEditor->getWindGenerationSettings();
        for(auto &setting:*windEditor->getWindGenerationSettings()){setting.setBaseWindStrength(2);setting.setBaseDirection(0);}windEditor->fillSettingsInputs();const int front=preparation.suggestedGate();
        for(auto &setting:*windEditor->getWindGenerationSettings())setting.setBaseDirection(180);windEditor->fillSettingsInputs();const int back=preparation.suggestedGate();check(gate->value()==recommendation,"changing wind preserves the numeric gate until the suggestion is applied");check(front<=back,"headwind recommends no higher gate than tailwind");
        *windEditor->getWindGenerationSettings()=originalWind;windEditor->fillSettingsInputs();preparationBridge->refresh();
        preparationBridge->edit(id(preparationBridge,"spinBox_startGate"),recommendation+2);preparationBridge->edit(preparationBridge->root()->values().value("id").toInt(),pages.indexOf(QString::fromUtf8("Wiatr")));QTest::qWait(120);
        clickControl(scene(&preparation),"applySuggestedGate");check(gate->value()==preparation.suggestedGate(),"wind section applies its updated suggested gate directly");
        QMetaObject::invokeMethod(&preparation,"on_pushButton_autoGate_clicked");check(gate->value()==preparation.suggestedGate() && gate->text()==QString::number(gate->value()),"automatic selection writes the calculated number into the gate field");preparationBridge->edit(preparationBridge->root()->values().value("id").toInt(),0);QTest::qWait(80);

        check(!pages.contains(QString::fromUtf8("Skocznia"))&&!pages.contains(QString::fromUtf8("Zasady konkursu")),"calendar-owned hill and rules have no duplicate editors in preparation");
        auto *roster=preparationBridge->findChild<QuickRoster *>();check(roster&&roster->count()==preparation.getSeasonCompetitionJumpers().size(),"season roster starts with calendar-qualified participants");
        check(preparation.getSeasonCompetition()==first&&first->getHill()==save.getHillsReference().first()&&first->getRules().getName()==history->getRules().getName(),"preparation retains the next event and its calendar settings");
        SaveJumpersList preset("Roster preset");preset.setJumpers(save.getJumpersReference().mid(0,3));save.setJumpersLists({preset});
        roster->markAll();roster->removeMarked();roster->applyPreset(0,0);check(preparation.getSeasonCompetitionJumpers()==preset.getJumpers(),"saved list adds eligible season athletes directly");
        roster->applyPreset(0,1);check(roster->count()==0,"saved list removes members directly");roster->undo();check(preparation.getSeasonCompetitionJumpers()==preset.getJumpers(),"saved-list action supports undo");

        preparation.reject();
    }
    SimulationSaveManagerWindow manager(&save);manager.fillNextCompetitionInformations();manager.show();QTest::qWait(200);
    auto *view=scene(&manager);auto *navigation=main.findChild<QuickUi *>();auto *bridge=manager.findChild<QuickBridge *>();
    const auto openForm=[&](const QString &control){
        bool opened=false;int ticks=0;
        QTimer watcher;QObject::connect(&watcher,&QTimer::timeout,[&]{
            check(++ticks<100,"form generator opens after pointer click");
            for(auto *widget:QApplication::allWidgets())if(auto *form=qobject_cast<JumpersFormGeneratorConfigWindow *>(widget);form && form->isVisible()){
                opened=true;watcher.stop();check(scene(form) && scene(form)->status()==QQuickWidget::Ready,"form generator QML loads");
                check(navigation->current()==form->findChild<QuickBridge *>() && !QApplication::activeModalWidget(),"form generator opens in normal application navigation");
                auto *header=main.findChild<QQuickWidget *>("quickApplicationHeader");clickControl(header,"navigateBack");break;
            }
        });watcher.start(20);clickControl(view,control);watcher.stop();
        check(opened && navigation->current()==bridge,QString("closing form generator returns to season (%1, opened %2, path %3)").arg(control).arg(opened).arg(navigation->path().join(" / ")));
    };
    openForm("action-pushButton_formGenerator");
    // The synthetic tools menu must use the same pointer-driven path as native menus.
    auto *tools=qobject_cast<QuickNode *>(bridge->find("quickSeasonTools"));check(tools,"season tools exist");
    const QString toolsName="action-quickSeasonTools";
    clickControl(view,toolsName);auto *toolsButton=item(view->rootObject(),toolsName);
    auto *popup=toolsButton->findChild<QObject *>("actionMenu");check(popup && popup->property("visible").toBool() && popup->property("count").toInt()==4,"season tools menu opens all actions");
    QTest::qWait(500);openForm("menu-entry-0");
    auto *classificationEditor=manager.findChild<ClassificationEditorWidget *>();
    check(classificationEditor && !classificationEditor->getClassification(),"classification editor starts without an invalid source pointer");
    QMetaObject::invokeMethod(classificationEditor,"submitted");
    DatabaseItemsListView *classificationList=nullptr;
    for(auto *list:manager.findChildren<DatabaseItemsListView *>())if(list->getClassifications()==&calendar->getClassificationsReference())classificationList=list;
    check(classificationList,"classification list exists");auto *classView=classificationList->getListView();auto classIndex=classificationList->getListModel()->index(0,0);classView->setCurrentIndex(classIndex);
    QMetaObject::invokeMethod(classView,"doubleClicked",Q_ARG(QModelIndex,classIndex));
    check(classificationEditor->getClassification()==classification,"opening classification selects the correct object");
    const auto classificationId=classification->getIDStr();classificationEditor->findChild<QLineEdit *>("lineEdit_name")->setText("Renamed classification");QMetaObject::invokeMethod(classificationEditor,"submitted");
    check(classification->getName()=="Renamed classification" && classification->getIDStr()==classificationId && first->getClassifications().contains(classification),"classification editing retains identity and calendar links");
    bridge->edit(bridge->root()->values().value("id").toInt(),4);QTest::qWait(200);
    auto *editor=manager.findChild<CalendarEditorWidget *>();auto *backend=bridge->findChild<QuickCalendar *>();
    check(editor && backend,"direct calendar workspace backend exists");
    check(backend->events().size()==3,"calendar lists main competitions");
    main.resize(1440,1000);QTest::qWait(100);
    clickControl(view,"calendar-field-rules");QTest::keyClick(view,Qt::Key_Home);QTest::keyClick(view,Qt::Key_Return);QTest::qWait(150);
    auto *inspector=item(view->rootObject(),"calendarInspector");check(inspector && inspector->property("section").toInt()==1,"choosing custom rules opens their editor directly");
    render(&manager,"calendar-custom-rules",true);inspector->setProperty("section",2);render(&manager,"calendar-links");inspector->setProperty("section",0);main.resize(1200,800);QTest::qWait(100);

    const int oldHill=backend->selection().value("hill").toInt();hillSearchRegression(view,"calendar-field-hill",backend->hills());backend->undo();check(backend->selection().value("hill").toInt()==oldHill,"calendar hill search participates in undo");
    check(!item(view->rootObject(),"calendar-field-rules")->property("searchable").toBool(),"other calendar selectors do not gain hill search");
    backend->select(first->getIDStr());backend->update({{"trainings",2},{"trial",true},{"hill",1}});
    check(first->getTrainings().size()==2 && first->getTrialRound() && first->getHill()==save.getHillsReference()[1],"calendar edits create complete competition block directly");
    check(calendar->getCompetitionsReference().size()==6 && calendar->getCompetitionsReference()[1]==first->getTrainings().first(),"calendar keeps training and trial events before their competition");
    for(auto *training:first->getTrainings())check(training->getHill()==first->getHill(),"training hill follows competition");
    backend->update({{"trainingImportance",3.5},{"trialImportance",4.0}});backend->duplicate();
    const auto duplicateId=backend->events()[2].toMap().value("id").toString();CompetitionInfo *blockCopy=nullptr;
    for(auto *event:calendar->getCompetitionsReference())if(event->getIDStr()==duplicateId)blockCopy=event;
    check(blockCopy && blockCopy->getTrainings().size()==2 && blockCopy->getTrialRound(),"duplication retains the full auxiliary event block");
    check(blockCopy->getTrainings()[0]->getJumpsImportance()==3.5 && blockCopy->getTrainings()[1]->getJumpsImportance()==3.5 && blockCopy->getTrialRound()->getJumpsImportance()==4.0,"duplicating a calendar block preserves custom training and trial importance");
    check(blockCopy->getTrainings()[0]!=first->getTrainings()[0] && blockCopy->getTrialRound()!=first->getTrialRound(),"duplicated auxiliary events have independent object identities");
    blockCopy->getTrainings()[0]->setJumpsImportance(7.5);check(first->getTrainings()[0]->getJumpsImportance()==3.5,"editing a duplicated training cannot alter the source event");
    backend->undo();backend->undo();
    backend->undo();check(first->getTrainings().isEmpty() && !first->getTrialRound() && calendar->getCompetitionsReference().size()==3,"undo restores event graph and pointer identities");
    backend->select(history->getIDStr());backend->update({{"trainings",3}});check(history->getTrainings().isEmpty() && history->getPlayed(),"played competition is protected");
    backend->select(first->getIDStr());backend->select(second->getIDStr(),Qt::ControlModifier);backend->update({{"importance",6.5}});
    check(first->getJumpsImportance()==6.5 && second->getJumpsImportance()==6.5,"multi-selection edits all selected future competitions");
    backend->remove();check(calendar->getCompetitionsReference()==QVector<CompetitionInfo *>{history},"multi-delete preserves history");
    backend->undo();check(calendar->getCompetitionsReference()==QVector<CompetitionInfo *>({history,first,second}),"undo deletion restores original objects and order");
    backend->select(first->getIDStr());backend->duplicate();check(calendar->getCompetitionsReference().size()==4,"duplicate inserts a competition");
    auto *copy=calendar->getCompetitionsReference()[2];check(copy!=first && copy->getIDStr()!=first->getIDStr() && !copy->getPlayed() && copy->getResultsReference().getResultsReference().isEmpty(),"duplicate has fresh identity and no historical results");backend->undo();
    clickControl(view,"calendarAdd");check(backend->events().size()==4,"pointer click adds competition without a dialog");backend->undo();
    backend->select(history->getIDStr());check(backend->advancementCompetitions().size()==1,"first competition has no qualification sources");
    backend->select(first->getIDStr());check(backend->advancementCompetitions().size()==2,"second competition offers only the earlier source and None");
    backend->update({{"advancement",2}});check(!first->getAdvancementCompetition(),"out-of-range future qualification source is rejected");
    backend->select(second->getIDStr());check(backend->advancementCompetitions().size()==3,"third competition offers both earlier sources");backend->update({{"advancement",2}});
    check(second->getAdvancementCompetition()==first && first->getQualifyingCompetitions().contains(second),"competition qualifies from earlier event and reverse dependency is rebuilt");
    const auto eventById=[&](const QString &key)->CompetitionInfo *{for(auto *event:calendar->getCompetitionsReference())if(event->getIDStr()==key)return event;return nullptr;};
    backend->select(first->getIDStr());backend->update({{"advancement",1}});backend->select(second->getIDStr(),Qt::ControlModifier);backend->duplicate();
    auto *copiedSource=eventById(backend->events()[3].toMap().value("id").toString());auto *copiedTarget=eventById(backend->events()[4].toMap().value("id").toString());
    check(copiedSource && copiedTarget && copiedTarget->getAdvancementCompetition()==copiedSource,"duplicating related competitions remaps qualification to the copied source");
    check(copiedSource->getAdvancementCompetition()==history && second->getAdvancementCompetition()==first,"duplication retains external qualification sources and leaves original relations intact");
    check(copiedSource->getQualifyingCompetitions().contains(copiedTarget) && !first->getQualifyingCompetitions().contains(copiedTarget),"copied qualification reverse links do not leak into the original block");backend->undo();backend->undo();
    backend->select(first->getIDStr());backend->move(1);check(calendar->getCompetitionsReference()[2]==first && !second->getAdvancementCompetition() && second->getPendingAdvancementCompetition()==first,"reordering suspends and preserves a qualification source now in the future");
    backend->select(second->getIDStr());backend->select(first->getIDStr(),Qt::ControlModifier);backend->duplicate();
    auto *pendingTarget=eventById(backend->events()[3].toMap().value("id").toString());auto *pendingSource=eventById(backend->events()[4].toMap().value("id").toString());
    check(pendingTarget && pendingSource && !pendingTarget->getAdvancementCompetition() && pendingTarget->getPendingAdvancementCompetition()==pendingSource,"duplicating a pending qualification retains the link inside the copied block");
    backend->select(pendingSource->getIDStr());backend->move(-1);check(pendingTarget->getAdvancementCompetition()==pendingSource,"reordering a copied pending source restores only the copied qualification");backend->undo();backend->undo();backend->select(first->getIDStr());
    check(!manager.findChild<QPushButton *>("pushButton_competitionConfig")->isEnabled(),"unresolved qualification prevents starting the dependent competition");
    QMetaObject::invokeMethod(&manager,"on_pushButton_competitionConfig_clicked");check(manager.findChildren<CompetitionConfigWindow *>().isEmpty(),"direct launch cannot bypass pending qualification protection");
    IdentifiableObjectsStorage roundTripStorage;roundTripStorage.add(save.getHillsReference());roundTripStorage.add(save.getJumpersReference());auto roundTrip=SeasonCalendar::getFromJson(SeasonCalendar::getJsonObject(*calendar),&roundTripStorage);check(roundTrip.getCompetitionsReference()[1]->getPendingAdvancementCompetition()==roundTrip.getCompetitionsReference()[2],"suspended qualification survives save and reload by event identity");

    backend->move(-1);check(second->getAdvancementCompetition()==first && !second->getPendingAdvancementCompetition(),"moving source back automatically restores qualification");backend->undo();check(second->getPendingAdvancementCompetition()==first,"undo restores suspended link");backend->undo();check(second->getAdvancementCompetition()==first,"multiple undo restores original order and active link");backend->undo();
    check(!second->getAdvancementCompetition(),"undo restores qualification links");
    first->setAdvancementCompetition(second);calendar->fixAdvancementCompetitions();check(!first->getAdvancementCompetition(),"calendar repair removes qualification sources from the future");
    second->setAdvancementCompetition(second);calendar->fixAdvancementCompetitions();check(!second->getAdvancementCompetition(),"calendar repair removes self qualification links");
    backend->select(first->getIDStr());
    backend->update({{"trainings",2},{"trial",true}});backend->setRulesTarget(1);
    auto *custom=backend->rulesEditor();auto *judges=custom->findChild<QCheckBox *>("checkBox_judgesPoints");
    check(judges,"custom auxiliary rules expose full editor");judges->setChecked(!first->getRules().getHasJudgesPoints());backend->applyCustomRules();
    check(first->getTrainings().first()->getRules().getHasJudgesPoints()!=first->getRules().getHasJudgesPoints(),"training rules can be customized without changing main competition");backend->undo();backend->setRulesTarget(0);
    backend->catalogAction("add");check(backend->calendars().size()==2 && backend->calendarName()!="Workflow calendar" && backend->events().isEmpty(),"new calendar is selected immediately");
    first->setPlayed(true);check(!backend->canUndo(),"undo across calendars cannot overwrite competitions played after the snapshot");first->setPlayed(false);
    backend->renameCalendar("Renamed calendar");check(backend->calendarName()=="Renamed calendar","calendar name edits directly");
    backend->add();check(backend->events().size()==1,"new calendar accepts competitions");
    check(manager.getEditingCalendar()!=save.getActualSeason()->getActualCalendar() && save.getActualSeason()->getActualCalendar()==calendar,"editing a new calendar never activates it implicitly");
    auto *draft=manager.getEditingCalendar();backend->addMany(3);check(backend->events().size()==4 && backend->selection().value("count").toInt()==3,"batch creation selects exactly the newly created competitions");
    backend->update({{"trainings",2},{"trial",true},{"hill",1}});
    const auto draftEvents=backend->events();const QString stableId=draftEvents.last().toMap().value("id").toString();
    backend->undo();backend->undo();check(backend->events().size()==1,"multiple undo reverses bulk settings and bulk insertion");backend->redo();backend->redo();
    check(backend->events().size()==4 && backend->events().last().toMap().value("id").toString()==stableId,"redo restores event identities and complete blocks");
    QVector<CompetitionInfo *> draftMains;for(auto *event:draft->getCompetitionsReference())if(event->getSerieType()<=CompetitionInfo::Qualifications)draftMains.append(event);
    for(int i=1;i<draftMains.size();++i)check(draftMains[i]->getTrainings().size()==2 && draftMains[i]->getTrialRound() && draftMains[i]->getHill()==save.getHillsReference()[1],"bulk training, trial and hill changes apply to all selected blocks");
    main.resize(1600,1100);QTest::qWait(180);backend->select(draftMains[0]->getIDStr());backend->select(draftMains[1]->getIDStr(),Qt::ControlModifier);QTest::qWait(100);
    auto *grip=item(view->rootObject(),"calendar-grip-"+draftMains[0]->getIDStr());auto *target=item(view->rootObject(),"calendar-row-"+draftMains[3]->getIDStr());check(grip&&target,"calendar drag handle and target exist");
    const auto begin=grip->mapToItem(view->rootObject(),grip->boundingRect().center()).toPoint();const auto end=target->mapToItem(view->rootObject(),QPointF(target->width()/2,target->height()*.85)).toPoint();
    QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,begin);for(int step=1;step<=12;++step){const QPoint at=begin+(end-begin)*step/12;QMouseEvent move(QEvent::MouseMove,at,view->mapToGlobal(at),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(20);}QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,end);QTest::qWait(150);
    check(backend->events()[2].toMap().value("id").toString()==draftMains[0]->getIDStr() && backend->events()[3].toMap().value("id").toString()==draftMains[1]->getIDStr(),"real calendar pointer drag moves multiple selected blocks in their original order");backend->undo();
    backend->classAction("add");auto *draftClass=draft->getClassificationsReference().last();auto *classEditor=backend->classificationEditor();
    auto *points=classEditor->findChild<QTableView *>("tableView_pointsForPlaces")->model();QAbstractItemModelTester pointContract(points,QAbstractItemModelTester::FailureReportingMode::Fatal);
    points->insertRows(0,2);check(points->setData(points->index(0,1),12.5),"classification points accept fractions");QTest::qWait(100);
    check(draftClass->getPointsForPlaces().value(1)==12.5,"classification points synchronize without a submit button");
    classEditor->findChild<QLineEdit *>("lineEdit_name")->setText("Draft standings");QMetaObject::invokeMethod(classEditor->findChild<QLineEdit *>("lineEdit_name"),"editingFinished");QTest::qWait(100);check(draftClass->getName()=="Draft standings","classification name saves on field completion");
    auto *calendarWorkspace=item(view->rootObject(),"calendarWorkspace");calendarWorkspace->setProperty("section",1);render(&manager,"calendar-classifications",true);main.resize(1600,1100);calendarWorkspace->setProperty("section",0);QTest::qWait(100);

    draftMains[0]->setClassifications({draftClass});draftMains[0]->setPlayed(true);manager.refreshCalendarState();backend->sync();check(backend->classLocked(),"used classification scoring is locked");
    classEditor->findChild<QLineEdit *>("lineEdit_name")->setText("Renamed used standings");backend->applyClassification();check(draftClass->getName()=="Renamed used standings" && draftClass->getPointsForPlaces().value(1)==12.5,"used standings allow a safe rename without changing points");
    backend->classAction("copy");check(draft->getClassificationsReference().size()==2 && !backend->classLocked() && draft->getClassificationsReference().last()->getIDStr()!=draftClass->getIDStr(),"copy of used standings has a new identity and editable scoring");
    draftMains[0]->setPlayed(false);manager.refreshCalendarState();backend->activateCalendar();check(save.getActualSeason()->getActualCalendar()==draft,"explicit activation changes playback calendar");
    check(manager.findChild<ClassificationResultsTableView *>()->getClassification()==draftClass,"activation synchronizes overview standings immediately");
    const int addedCalendar=backend->calendarIndex();
    backend->selectCalendar(1-addedCalendar);check(backend->calendarName()=="Workflow calendar","switching calendars restores original events");
    check(save.getActualSeason()->getActualCalendar()==draft,"switching editor back leaves playback unchanged");backend->activateCalendar();check(save.getActualSeason()->getActualCalendar()==calendar,"active calendar can be explicitly restored");
    check(manager.findChild<ClassificationResultsTableView *>()->getClassification()==classification,"restoring active calendar also restores its overview standings");
    backend->selectCalendar(addedCalendar);backend->catalogAction("remove");check(backend->calendars().size()==1 && backend->calendarIndex()==0,"removing current calendar selects remaining calendar");
    backend->undo();check(backend->calendars().size()==2 && manager.getEditingCalendar()==draft,"undo deletion restores the exact calendar and its complete content");backend->redo();check(backend->calendars().size()==1,"redo removes the restored calendar again");
    render(&manager,"calendar-workflow",true);
    // Turn the fixture into an entirely unplayed calendar to exercise the empty state.
    history->setPlayed(false);save.updateNextCompetitionIndex();editor->getModel()->setDontModifyBefore(0);
    backend->catalogAction("remove");check(backend->calendars().isEmpty() && backend->events().isEmpty() && editor->getModel()->getCalendar()==nullptr,"removing last calendar clears editor safely");
    backend->catalogAction("add");backend->add();check(backend->events().size()==1,"creating calendar after empty state restores editing");
    const QString presetName="Quick calendar regression";
    const auto confirmPreset=[&](bool saving){
        bool opened=false;int ticks=0;QTimer picker;
        QObject::connect(&picker,&QTimer::timeout,[&]{
            check(++ticks<100,"preset picker opens");
            for(auto *widget:QApplication::allWidgets())if(auto *input=qobject_cast<QInputDialog *>(widget);input&&input->isVisible()) {
                picker.stop();opened=true;input->setTextValue(presetName);
                auto *source=input->findChild<QDialogButtonBox *>();check(source,"preset picker buttons");
                auto *accept=source->button(QDialogButtonBox::Ok);check(accept,"preset picker confirm action");
                auto *dialogBridge=input->findChild<QuickBridge *>();dialogBridge->refresh();QTest::qWait(50);
                clickControl(scene(input),"action-"+(accept->objectName().isEmpty()?accept->text().remove('&'):accept->objectName()));break;
            }
        });picker.start(20);backend->preset(saving);picker.stop();check(opened,"preset workflow opens the picker");
    };
    auto *presetEvent=editor->getModel()->getCalendar()->getCompetitionsReference().first();backend->select(presetEvent->getIDStr());backend->update({{"trainings",2},{"trial",true},{"trainingImportance",3.5},{"trialImportance",4.0}});
    confirmPreset(true);backend->add();confirmPreset(false);
    check(backend->events().size()==1,"loading preset replaces calendar events");
    backend->undo();check(backend->events().size()==2,"preset replacement supports undo without losing old event objects");backend->redo();check(backend->events().size()==1,"preset replacement supports redo");
    auto *loaded=editor->getModel()->getCalendar()->getCompetitionsReference().last();
    check(loaded->getTrainings().size()==2 && loaded->getTrialRound() && loaded->getTrainings().first()->getJumpsImportance()==3.5 && loaded->getTrialRound()->getJumpsImportance()==4.0,"presets retain associated events and their independent importance");
    for(auto *event:editor->getModel()->getCalendar()->getCompetitionsReference())check(save.getHillsReference().contains(event->getHill()),"loaded preset uses hills owned by the save");
    for(auto *event:editor->getModel()->getCalendar()->getCompetitionsReference()) {
        check(event->getResultsReference().getCompetition()==event,"imported preset results belong to the newly created calendar event");
        JumpData jump(save.getJumpersReference().first(),event->getHill());jump.setPoints(100);
        CompetitionResults fresh=event->getResultsReference();fresh.addJump(save.getJumpersReference().first(),jump);
        check(fresh.getResultsReference().first().getCompetition()==event && jump.getCompetition()==event,"new jumps after preset import refer to the live event rather than the source preset");
    }
    manager.reject();qInfo()<<"PASS: real season tools, form navigation, direct calendar edits, multi-selection, history protection and undo";
}
static void rosterRegressions(MainWindow &main) {
    main.show();main.resize(1440,900);
    QVector<Jumper *> pool;for(auto &j:GlobalDatabase::get()->getEditableGlobalJumpers())pool.append(&j);
    check(pool.size()>20,"roster fixture");
    CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition);config.setCompetitionJumpers(pool.mid(0,8));config.show();QTest::qWait(180);
    auto *bridge=config.findChild<QuickBridge *>();auto *roster=bridge->findChild<QuickRoster *>();check(roster && roster->count()==8 && roster->library()->rowCount()==pool.size(),QString("roster exposes full library and current start list separately (%1 selected, %2 candidates, %3 expected)").arg(roster?roster->count():-1).arg(roster?roster->library()->rowCount():-1).arg(pool.size()));
    QAbstractItemModelTester libraryModel(roster->library(),QAbstractItemModelTester::FailureReportingMode::Fatal);QAbstractItemModelTester orderModel(roster->ordered(),QAbstractItemModelTester::FailureReportingMode::Fatal);
    clickControl(scene(&config),"editStartList");check(item(scene(&config)->rootObject(),"rosterWorkspace"),"overview opens dedicated roster workspace");
    roster->mark(QuickRoster::key(pool[1]));roster->moveMarked(4);check(config.getCompetitionJumpers()[3]==pool[1],"position field uses final one-based position");roster->undo();
    roster->mark(QuickRoster::key(pool[0]));roster->mark(QuickRoster::key(pool[2]),Qt::ControlModifier);roster->moveBefore(QuickRoster::key(pool[0]),QuickRoster::key(pool[5]),true);
    check(config.getCompetitionJumpers().mid(4,2)==QVector<Jumper *>({pool[0],pool[2]}),"moving multiple athletes preserves their relative order");roster->undo();
    roster->removeMarked();check(roster->count()==6 && !config.getCompetitionJumpers().contains(pool[0]),"remove selected updates source competition");roster->undo();check(config.getCompetitionJumpers()==pool.mid(0,8),"undo restores all original start list identities and order");
    auto *view=scene(&config);clickControl(view,"rosterSearch");QTest::keyClicks(view,pool[12]->getCountryCode());QTest::qWait(100);check(roster->query()==pool[12]->getCountryCode(),"pointer and keyboard filter the athlete library");
    for(int i=0;i<roster->library()->rowCount();++i)check(roster->library()->get(i).value("countryCode").toString()==pool[12]->getCountryCode(),"country text filter excludes other countries");
    roster->addVisible();const auto filtered=config.getCompetitionJumpers();QSet<Jumper *> distinct;for(auto *j:filtered)distinct.insert(j);check(distinct.size()==filtered.size(),"bulk selection cannot duplicate participants");roster->undo();roster->setQuery("");
    auto *libraryView=item(view->rootObject(),"rosterLibrary");check(QMetaObject::invokeMethod(libraryView,"positionViewAtIndex",Q_ARG(int,9),Q_ARG(int,3)),"library supports direct positioning");QTest::qWait(100);
    clickControl(view,"roster-candidate-"+QuickRoster::key(pool[9]));check(config.getCompetitionJumpers().contains(pool[9]),"one click adds an athlete without transfer arrows");roster->undo();
    roster->mark(QuickRoster::key(pool[0]));QTest::qWait(80);
    auto *grip=item(view->rootObject(),"roster-grip-"+QuickRoster::key(pool[0]));auto *target=item(view->rootObject(),"roster-entry-"+QuickRoster::key(pool[3]));check(grip&&target,"drag handles and target rows exist");
    const auto begin=grip->mapToItem(view->rootObject(),grip->boundingRect().center()).toPoint();const auto end=target->mapToItem(view->rootObject(),QPointF(target->width()/2,target->height()*.85)).toPoint();
    QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,begin);
    for(int step=1;step<=12;++step){const QPoint at=begin+(end-begin)*step/12;QMouseEvent move(QEvent::MouseMove,at,view->mapToGlobal(at),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(20);}
    QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,end);QTest::qWait(100);
    check(config.getCompetitionJumpers()[3]==pool[0],"real pointer drag reorders the original start list");roster->undo();
    clickControl(view,"roster-grip-"+QuickRoster::key(pool[1]));clickControl(view,"rosterMoveConfirm");check(config.getCompetitionJumpers().first()==pool[1],"clicking a grip offers accessible move-to-position alternative");roster->undo();
    render(&config,"roster-workspace",true);config.reject();
    JumpersListEditorWindow saved;saved.setAllJumpers(pool.mid(0,12));saved.setSelectedJumpers(pool.mid(0,3));saved.updateUnselectedJumpers();saved.show();QTest::qWait(120);
    auto *savedRoster=saved.findChild<QuickBridge *>()->findChild<QuickRoster *>();check(savedRoster && savedRoster->count()==3,"saved lists use the same roster editor");savedRoster->toggle(QuickRoster::key(pool[5]));check(saved.getSelectedJumpers().contains(pool[5]) && !saved.getUnselectedJumpers().contains(pool[5]),"saved-list membership and its complement stay consistent");savedRoster->undo();check(saved.getSelectedJumpers()==pool.mid(0,3),"saved list undo restores its draft");render(&saved,"saved-roster",true);saved.reject();
    Team team;team.setCountryCode("POL");team.setJumpers(pool.mid(0,6));QDialog dialog;auto *layout=new QVBoxLayout(&dialog);auto *editor=new TeamEditorWidget(&team,4,&dialog);layout->addWidget(editor);dialog.show();QTest::qWait(120);
    auto *teamRoster=dialog.findChild<QuickBridge *>()->findChild<QuickRoster *>();check(teamRoster && !teamRoster->membership() && teamRoster->activeLimit()==4,"team lineup distinguishes starters from reserves");teamRoster->mark(QuickRoster::key(pool[5]));teamRoster->moveMarked(1);check(editor->constructTeamFromWidgetInput().getJumpers().first()==pool[5],"reserve can move directly into starting squad");teamRoster->undo();check(editor->getJumpers()==pool.mid(0,6),"team lineup undo preserves identities");render(&dialog,"team-roster",true);dialog.reject();
    qInfo()<<"PASS: roster search, batch membership, stable models, pointer drag, position moves, undo, saved lists and team reserves";
}

static void geometryRegressions(MainWindow &main) {
    main.show();SimulationSave save;QVector<Jumper *> pool;for(auto &j:GlobalDatabase::get()->getEditableGlobalJumpers())pool.append(new Jumper(j));save.setJumpers(pool);save.fixJumpersFormInstabilities();
    JumpersFormGeneratorConfigWindow form(&main);form.getTableModel()->setGeneratorsSettings(JumperFormGeneratorSettings::constructJumperFormGeneratorsSettingsVector(pool));form.setSave(&save);form.updateTable();form.show();QTest::qWait(180);
    auto *view=scene(&form);auto *bridge=form.findChild<QuickBridge *>();check(bridge->root()->values().value("kind")=="formGenerator","form generator uses a dedicated bounded workspace");
    for(const QSize size:{QSize(1440,900),QSize(1024,640),QSize(520,600),QSize(360,480)}){
        main.resize(size);QTest::qWait(120);auto *submit=item(view->rootObject(),"formGeneratorSubmit");auto *table=item(view->rootObject(),"formGeneratorTable");
        check(submit && submit->isVisible() && submit->isEnabled(),"form generation action remains visible");const auto submitPosition=submit->mapToItem(view->rootObject(),QPointF());check(submitPosition.y()>=0&&submitPosition.y()+submit->height()<=view->height()+1,"form action stays completely inside viewport");
        check(table && table->isVisible() && table->height()>=100,"form table retains a usable viewport");
        QStringList names;for(int i=0;i<60;++i)names.append(QString("Preset %1").arg(i+1));
        QInputDialog preset(&form);preset.setWindowTitle("Wybierz preset generatora formy");preset.setLabelText("Wybierz preset generatora formy z listy. Parametry wybranego presetu zostaną zastosowane do zawodników.");preset.setComboBoxItems(names);preset.show();QTest::qWait(200);
        auto *picker=scene(&preset);check(picker&&picker->status()==QQuickWidget::Ready,"preset picker QML loads");check(preset.width()<=main.width() && preset.height()<=main.height(),"native size hints cannot enlarge preset beyond application");
        auto *buttons=item(picker->rootObject(),"modalButtons");check(buttons&&buttons->height()>0,"preset actions exist");const auto point=buttons->mapToItem(picker->rootObject(),QPointF());check(point.y()>=0&&point.y()+buttons->height()<=picker->height()+1,"preset actions stay pinned inside dialog");
        std::function<QQuickItem *(QQuickItem *)> choice=[&](QQuickItem *root)->QQuickItem *{if(root->isVisible()&&QString(root->metaObject()->className()).startsWith("SChoice"))return root;for(auto *child:root->childItems())if(auto *result=choice(child))return result;return nullptr;};
        auto *combo=choice(picker->rootObject());check(combo,"preset choice exists");const auto comboPoint=combo->mapToItem(picker->rootObject(),combo->boundingRect().center()).toPoint();check(picker->rect().contains(comboPoint),"preset choice is reachable without hidden overflow");QTest::mouseClick(picker,Qt::LeftButton,Qt::NoModifier,comboPoint);QTest::qWait(100);
        auto *popup=combo->findChild<QObject *>("choicePopup");check(popup&&popup->property("visible").toBool(),"preset list opens by pointer");auto *contents=popup->property("contentItem").value<QQuickItem *>();check(contents&&contents->height()>=30,"preset list has a usable scroll viewport");const auto bounds=contents->mapRectToItem(picker->rootObject(),contents->boundingRect());check(bounds.top()>=-1&&bounds.bottom()<=picker->height()+1&&bounds.left()>=-1&&bounds.right()<=picker->width()+1,"expanded preset list fits inside its native window");
        picker->grabFramebuffer().save(output+QString("/preset-popup-%1.png").arg(size.width()));++renders;
        QTest::keyClick(picker,Qt::Key_End);QTest::keyClick(picker,Qt::Key_Return);QTest::qWait(80);check(preset.textValue()==names.last(),"keyboard can select last preset in a long popup");preset.reject();
    }
    main.resize(1200,800);
    auto &presets=GlobalDatabase::get()->getEditableFormGeneratorPresets();const auto originals=presets;
    const auto presetName=QString("UI regression preset");
    const auto withPreset=[&](const char *slot){QTimer::singleShot(100,&form,[&]{for(auto *widget:QApplication::allWidgets())if(auto *input=qobject_cast<QInputDialog *>(widget);input&&input->isVisible()){input->setTextValue(presetName);input->accept();}});check(QMetaObject::invokeMethod(&form,slot,Qt::DirectConnection),"preset action is callable");};
    form.findChild<QDoubleSpinBox *>("doubleSpinBox_formBonus")->setValue(3.5);withPreset("on_pushButton_saveAsPreset_clicked");const int count=presets.size();
    form.findChild<QDoubleSpinBox *>("doubleSpinBox_formBonus")->setValue(6.5);withPreset("on_pushButton_saveAsPreset_clicked");check(presets.size()==count&&presets.last().getSettings().getFormBonus()==6.5,"saving the same form preset replaces that preset instead of a calendar preset");
    const auto athletesBefore=form.getTableModel()->getGeneratorsSettingsReference();withPreset("on_pushButton_loadPreset_clicked");
    check(form.getTableModel()->getGeneratorsSettingsReference().first().getFormBonus()!=6.5,"loaded preset is staged until explicit application");form.applyParameters(false);
    for(int i=0;i<athletesBefore.size();++i){const auto settings=form.getTableModel()->getGeneratorsSettingsReference()[i];check(settings.getJumper()==athletesBefore[i].getJumper()&&settings.getFormBonus()==6.5,"loading a form preset applies settings and preserves every athlete identity");}
    auto *formTable=qobject_cast<QuickTableModel *>(qobject_cast<QuickNode *>(bridge->find("tableView"))->values().value("table").value<QObject *>());check(formTable&&formTable->data(formTable->index(0),Qt::UserRole+1).toList()[2].toMap().value("text").toString().toDouble()==6.5,"loaded preset refreshes visible table values");
    presets=originals;GlobalDatabase::get()->writeFormGeneratorPresets();
    render(&form,"form-generator-responsive",true);form.reject();qInfo()<<"PASS: form table and pinned action, bounded native preset dialogs, constrained popups and long-list keyboard selection";
}

static void auditDataRegressions() {
    Jumper a("A","One","POL"),b("B","Two","NOR"),c("C","Three","GER");
    const auto places=CompetitionResults::getResultsWithJumpersPositionsForClassificationArchiveResults({{&a,100},{&b,100},{&c,80}});
    const auto teams=CompetitionResults::getResultsWithTeamsPositionsForClassificationArchiveResults({{"POL",100},{"NOR",100},{"GER",80}});
    check(places.value(&a)==1&&places.value(&b)==1&&places.value(&c)==3&&teams.value("GER")==3,"audit 1: archive ties use competition ranking 1,1,3 for athletes and teams");
    Classification one("One"),two("Two"),three("Three");QVector<Classification *> classes{&one,&two,&three};
    ClassificationsCheckBoxesWidget filters;filters.setClassificationsList(&classes);
    for(int pass=0;pass<5;++pass){filters.setupCheckBoxes();auto boxes=filters.findChildren<QCheckBox *>();check(boxes.size()==3,"audit 3: rebuilding classification filters leaves no stale or duplicate checkboxes");for(auto *box:boxes)box->setChecked(true);check(filters.getClassifications().size()==3,"audit 3: every rebuilt filter maps to its classification");}
    Hill hill;CompetitionInfo played(&hill),future(&hill),later(&hill);played.setPlayed(true);later.setPlayed(true);
    SeasonCalendar calA("A"),calB("B");calA.setCompetitions({&played,&future});calB.setCompetitions({&later});
    Season season;season.setCalendars({&calA,&calB});QVector<Season> seasons{season};
    CompetitionsRangeComboBoxesWidget range;range.setSeasonsList(&seasons);range.setMergeCalendars(true);range.setupComboBoxes();
    auto *first=range.findChild<QComboBox *>("comboBox_firstCompetition");auto *last=range.findChild<QComboBox *>("comboBox_secondCompetition");
    check(range.getCompetition(1)==&played&&range.getCompetition(2)==&later,"audit 2: merged range endpoints resolve to displayed played competitions, skipping unplayed tail");
    first->setCurrentIndex(0);check(!range.getCompetition(1),"audit 2: a season heading is never interpreted as a competition");
    range.setCalendarFilter("B");range.setMergeCalendars(false);range.setupComboBoxes();check(range.getCompetition(1)==&later&&range.getCompetition(2)==&later,"audit 2: filtered range uses direct event identity");
    calA.setCompetitions({});calB.setCompetitions({});
    CompetitionResults prefix;
    for(auto *athlete:{&a,&b,&c})for(int round=0;round<2;++round){JumpData jump;jump.setJumper(athlete);jump.setPoints(round==0?(athlete==&b?90:100):(athlete==&b?50:10));prefix.addJump(athlete,jump);}
    check(prefix.getPositionAfterJumps(*prefix.getResultOfIndividualJumper(&a),1)==1 && prefix.getPositionAfterJumps(*prefix.getResultOfIndividualJumper(&b),1)==3,"previous-round ranking handles ties as 1,1,3");
    check(prefix.getPositionAfterJumps(*prefix.getResultOfIndividualJumper(&b),2)==1,"previous-round ranking excludes later jumps that change the leader");
    check(prefix.getPositionAfterJumps(*prefix.getResultOfIndividualJumper(&a),0)==0 && prefix.getPositionAfterJumps(*prefix.getResultOfIndividualJumper(&a),3)==0,"previous-round ranking rejects unavailable rounds");
    qInfo()<<"PASS: audit 1-3 archive ties, competition ranges and filter rebuilding";
}

static void auditFormRegressions() {
    Jumper a("A","One","POL"),b("B","Two","NOR");QVector<Jumper *> athletes{&a,&b};
    JumperFormGeneratorsSettingsTableModel model;model.setGeneratorsSettings(JumperFormGeneratorSettings::constructJumperFormGeneratorsSettingsVector(athletes));
    check(!model.setData(model.index(0,2),QString("abc"),Qt::EditRole),"audit 4: invalid text cannot silently become zero bonus");
    check(model.setData(model.index(0,3),80,Qt::EditRole)&&!model.setData(model.index(0,4),40,Qt::EditRole),"audit 4: maximum cannot be lower than minimum");
    check(!model.setData(model.index(0,1),QVariant(std::numeric_limits<double>::quiet_NaN()),Qt::EditRole),"audit 4: non-finite generator input is rejected");
    JumpersFormGeneratorConfigWindow form;form.getTableModel()->setGeneratorsSettings(JumperFormGeneratorSettings::constructJumperFormGeneratorsSettingsVector(athletes));form.updateTable();
    form.findChild<QDoubleSpinBox *>("doubleSpinBox_minForm")->setValue(80);QMetaObject::invokeMethod(&form,"on_doubleSpinBox_minForm_editingFinished");
    form.findChild<QDoubleSpinBox *>("doubleSpinBox_maxForm")->setValue(40);QMetaObject::invokeMethod(&form,"on_doubleSpinBox_maxForm_editingFinished");
    for(const auto &settings:form.getTableModel()->getGeneratorsSettings())check(settings.getMinForm()<=settings.getMaxForm(),"audit 4: bulk form bounds always stay consistent");
    CompetitionRules rules;rules.setHasDsq(true);JumpManipulator manipulation;manipulation.setExactDSQProbability(0);Hill hill=GlobalDatabase::get()->getEditableGlobalHills().first();JumpSimulator simulator(&a,{Wind(0,0),Wind(0,0),Wind(0,0)},&hill);simulator.setGate(0);simulator.setCompetitionStartGate(0);simulator.setJumpsImportance(5);simulator.setCompetitionRules(&rules);simulator.setManipulator(&manipulation);simulator.setDSQBaseProbability(1);
    for(int attempt=0;attempt<3;++attempt){simulator.simulateJump();check(!simulator.getJumpData().getDSQ(),"audit 5: exact zero DSQ overrides even guaranteed base disqualification");}
    manipulation.setExactDSQProbability(1);simulator.simulateJump();check(simulator.getJumpData().getDSQ(),"audit 5: exact one still guarantees DSQ");
    manipulation.setExactDSQProbability(-1);simulator.setDSQBaseProbability(0);simulator.simulateJump();check(!simulator.getJumpData().getDSQ(),"audit 5: default zero DSQ remains disabled");
    auto *view=form.findChild<QTableView *>("tableView");view->selectAll();QMetaObject::invokeMethod(&form,"onRemoveFromTable",Q_ARG(bool,true));check(form.getTableModel()->rowCount()==2,"audit 6: keep selected preserves all rows when every athlete is selected");
    view->selectionModel()->select(form.getTableModel()->index(1,0),QItemSelectionModel::ClearAndSelect);QMetaObject::invokeMethod(&form,"onRemoveFromTable",Q_ARG(bool,true));check(form.getTableModel()->rowCount()==1&&form.getTableModel()->getGeneratorsSettings().first().getJumper()==&b,"audit 6: keep selected accepts cell selection and preserves that athlete");
    view->clearSelection();QMetaObject::invokeMethod(&form,"onRemoveFromTable",Q_ARG(bool,true));check(form.getTableModel()->rowCount()==1,"audit 6: empty selection never erases the list");
    qInfo()<<"PASS: audit 4-6 form validation, DSQ overrides and keep-selected behavior";
}

static void auditStatisticsRegressions(MainWindow &main) {
    SimulationSave save;save.setName("Audit statistics");
    auto *hill=new Hill(GlobalDatabase::get()->getEditableGlobalHills().first());save.setHills({hill});
    auto *a=new Jumper("A","One","POL"),*b=new Jumper("B","Two","NOR");save.setJumpers({a,b});
    auto *calendar=new SeasonCalendar("Audit calendar");auto *competition=new CompetitionInfo(hill);CompetitionRules rules=GlobalDatabase::get()->getEditableCompetitionRules().first();rules.setRounds({RoundInfo(2,true)});rules.setHasJudgesPoints(true);competition->setRules(rules);competition->setPlayed(true);
    auto &results=competition->getResultsReference();results.setCompetition(competition);
    for(auto *athlete:{a,b}){JumpData jump;jump.setJumper(athlete);jump.setCompetition(competition);jump.setHill(hill);jump.setDistance(80);jump.setPoints(100);jump.setJudges({18,18,18,18,18});jump.setDSQ(false);results.addJump(athlete,jump);}results.sortInDescendingOrder();
    calendar->setCompetitions({competition});Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.getSeasonsReference().append(season);save.setActualSeason(&save.getSeasonsReference().last());
    SimulationRatingsWindow stats(&save,&main);stats.getClassificationsCheckBoxes()->setClassificationsList(&calendar->getClassificationsReference());stats.getClassificationsCheckBoxes()->setupCheckBoxes();stats.setFilteredJumpers(save.getJumpersReference());
    auto *range=stats.getRangeComboBoxes();range->setSeasonsList(&save.getSeasonsReference());range->setMergeCalendars(false);range->setCalendarFilter(calendar->getName());range->setupComboBoxes();stats.setupComboBox();stats.fillWindow();
    check(stats.findChild<QLabel *>("label_biggerWin")->text().startsWith("0"),"audit 8: tied winners record a valid zero winning margin");
    check(stats.findChild<QLabel *>("label_10Largestdifference")->text()==QString::fromUtf8("—"),"audit 8: fewer than ten entrants shows unavailable margin without dereferencing null");
    competition->getRulesPointer()->setHasJudgesPoints(false);competition->getRulesPointer()->setHasWindCompensations(false);stats.fillWindow();
    check(stats.findChild<QTableView *>("tableView_avgJudges")->model()->rowCount()==0&&stats.findChild<QTableView *>("tableView_avgWind")->model()->rowCount()==0,"audit 7: unavailable scoring components are omitted instead of ranked as NaN");
    for(auto &result:results.getResultsReference())for(auto &jump:result.getJumpsReference())jump.setDSQ(true);stats.fillWindow();
    check(stats.findChild<QTableView *>("tableView_avgDistance")->model()->rowCount()==0,"audit 7: all-DSQ athletes have no invented distance average");
    for(auto *label:stats.findChildren<QLabel *>())check(!label->text().contains(QRegularExpression("\\b(?:nan|inf)\\b",QRegularExpression::CaseInsensitiveOption)),"audit 7: facts contain no non-finite averages or percentages");
    auto &presets=GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets();const auto originalPresets=presets;presets.clear();stats.findChild<QComboBox *>("comboBox_preset")->clear();stats.fillWindow();
    check(stats.getGeneralClassificationModel()->rowCount()==0,"audit 9: absent point presets do not crash or invent a places classification");
    stats.findChild<QCheckBox *>("checkBox_generalClassificationCompPoints")->setChecked(true);stats.fillWindow();check(stats.getGeneralClassificationModel()->rowCount()==2,"audit 9: competition-point classification works without any places preset");presets=originalPresets;
    stats.setFilteredJumpers({});stats.fillWindow();
    for(auto *table:stats.findChildren<QTableView *>())if(table->model())check(table->model()->rowCount()==0,"audit 10: empty filters clear rankings, records and general classification");
    check(stats.findChild<QLabel *>("label_biggerWin")->text()==QString::fromUtf8("—"),"audit 10: facts cannot retain values from the preceding filter");
    auto *emptyCalendar=new SeasonCalendar("Empty");save.getActualSeason()->getCalendarsReference().append(emptyCalendar);stats.setupComboBox();stats.getCalendarComboBox()->setCurrentIndex(1);stats.fillWindow();
    check(!range->getCompetition(1)&&!range->getCompetition(2),"audit 11: switching to an empty calendar safely clears the range");
    check(CompetitionInfo::getCompetitionsByStartAndEnd({},nullptr,nullptr).isEmpty(),"audit 11: missing range endpoints return an empty result");
    check(CompetitionInfo::getCompetitionsByStartAndEnd({competition},nullptr,competition).isEmpty(),"audit 11: a missing starting competition cannot dereference null");
    JumpData partial;partial.setInSingleJumps(true);JumpDataDetailedInfoWindow detail(&partial);detail.fillJumpInformations();check(!detail.findChild<QPushButton *>("pushButton")->isEnabled(),"audit 12: a jump with missing athlete/hill is safely shown as unavailable");
    partial.setJumper(a);detail.fillJumpInformations();check(!detail.findChild<QPushButton *>("pushButton")->isEnabled(),"audit 12: missing hill alone cannot crash jump details");
    detail.setJumpData(&results.getResultsReference().first().getJumpsReference().first());detail.fillJumpInformations();check(detail.findChild<QPushButton *>("pushButton")->isEnabled(),"audit 12: the same details view recovers with complete data");
    CompetitionInfo detachedCompetition(hill);JumpData orphan(a,hill);orphan.setCompetition(&detachedCompetition);check(orphan.getPositionInRound()==0,"audit 12: an unlinked jump has no fictitious round or null result dereference");
    results.getResultsReference()[0].getJumpsReference().clear();
    JumperStatsWindow athleteStats(&main);athleteStats.setSave(&save);athleteStats.setJumper(results.getResultsReference()[0].getJumper());athleteStats.getClassificationsCheckBoxes()->setClassificationsList(&calendar->getClassificationsReference());athleteStats.getClassificationsCheckBoxes()->setupCheckBoxes();athleteStats.setupComboBox();
    auto *athleteRange=athleteStats.getRangeComboBoxes();athleteRange->setSeasonsList(&save.getSeasonsReference());athleteRange->setCalendarFilter(calendar->getName());athleteRange->setMergeCalendars(false);athleteRange->setupComboBoxes();athleteStats.fillWindow();
    check(athleteStats.findChild<QLabel *>("label_averagePos")->text()==QString::fromUtf8("—"),"audit 13: DNS result without jumps has no invalid first jump or fabricated chart average");
    athleteStats.getCalendarComboBox()->setCurrentIndex(1);athleteStats.fillWindow();check(athleteStats.findChild<QLabel *>("label_bestPos")->text()==QString::fromUtf8("—"),"audit 13: empty athlete charts remain safe after selecting an empty calendar");
    qInfo()<<"PASS: audit 7-13 finite statistics, small fields, missing presets, empty filters/calendars, detached jumps and DNS charts";
}

static void spaceRegressions(MainWindow &main) {
    main.show();main.resize(1440,900);DatabaseEditorWindow database(&main);database.show();QTest::qWait(150);
    auto *bridge=database.findChild<QuickBridge *>();auto *tabs=database.findChild<QTabWidget *>();
    for(int page=0;page<tabs->count();++page){
        bridge->edit(id(bridge,tabs->objectName()),page);QTest::qWait(100);
        auto *entry=qobject_cast<QuickNode *>(bridge->find("listView"));check(entry,"expanding database list exists");
        auto *table=item(scene(&database)->rootObject(),"dataTable-"+QString::number(entry->values().value("id").toInt()));check(table&&table->isVisible(),"active database table is visible");
        const auto bounds=table->mapRectToItem(scene(&database)->rootObject(),table->boundingRect());
        check(bounds.bottom()>=scene(&database)->height()-40&&table->width()>scene(&database)->width()*.8,"database list uses the available height and hidden editors reserve no empty column");
        const double previous=table->height();main.resize(1440,1100);QTest::qWait(100);check(table->height()>=previous+180,"database lists grow when the window grows instead of stopping at a fixed cap");main.resize(1440,900);QTest::qWait(60);
        auto *model=qobject_cast<QuickTableModel *>(entry->values().value("table").value<QObject *>());
        if(model&&model->count()){model->select(0,0,0,true);QTest::qWait(100);table=item(scene(&database)->rootObject(),"dataTable-"+QString::number(entry->values().value("id").toInt()));check(table,"database table survives editor navigation");const auto edited=table->mapRectToItem(scene(&database)->rootObject(),table->boundingRect());check(edited.bottom()>=scene(&database)->height()-40&&edited.bottom()<=scene(&database)->height(),"opening a long editor does not push the database list beyond the viewport");}
    }
    render(&database,"space-database",true);database.reject();
    SimulationSavesWindow saves;saves.show();QTest::qWait(100);auto *saveBridge=saves.findChild<QuickBridge *>();
    for(const auto &size:{QSize(360,480),QSize(1440,900)}){
        main.resize(size);QTest::qWait(100);main.grab();auto *entry=qobject_cast<QuickNode *>(saveBridge->find("listView_simulationSaves"));auto *table=item(scene(&saves)->rootObject(),"dataTable-"+QString::number(entry->values().value("id").toInt()));const auto bounds=table->mapRectToItem(scene(&saves)->rootObject(),table->boundingRect());check(bounds.bottom()>=scene(&saves)->height()-24&&bounds.bottom()<=scene(&saves)->height(),"save list fills available height at desktop and smallest size");check(item(scene(&saves)->rootObject(),"action-pushButton_add")->property("text").toString()==QString::fromUtf8("Nowy zapis")&&item(scene(&saves)->rootObject(),"action-pushButton_OK")->height()<=52,"save actions have concise labels and a compact one-row layout");
    }
    saves.reject();qInfo()<<"PASS: database categories and save lists fill available space while editors and secondary controls stay bounded";
}

static void uxRegressions(MainWindow &main) {
    main.show();main.resize(1440,900);SimulationSave save;QVector<Jumper *> pool;for(int i=0;i<6;++i)pool.append(new Jumper(GlobalDatabase::get()->getEditableGlobalJumpers()[i]));save.setJumpers(pool);save.fixJumpersFormInstabilities();
    {
        JumpersFormGeneratorConfigWindow form(&main);form.getTableModel()->setGeneratorsSettings(JumperFormGeneratorSettings::constructJumperFormGeneratorsSettingsVector(pool));form.setSave(&save);form.updateTable();form.show();QTest::qWait(150);auto *bridge=form.findChild<QuickBridge *>();
        bridge->edit(id(bridge,"doubleSpinBox_formBonus"),8.0);QTest::qWait(80);
        for(const auto &settings:form.getTableModel()->getGeneratorsSettings())check(settings.getFormBonus()==0,"UX 1: draft form parameters never silently overwrite athletes");
        check(!form.findChild<QPushButton *>("pushButton_generate")->isEnabled(),"UX 1: pending parameters must be applied before generation");
        auto *source=form.findChild<QTableView *>("tableView");source->selectionModel()->select(form.getTableModel()->index(1,0),QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);QTest::qWait(40);
        auto *scope=item(scene(&form)->rootObject(),"formBulkScope");check(scope&&scope->isVisible(),"UX 1: batch scope is directly available");scope->setProperty("currentIndex",1);clickControl(scene(&form),"formApplyParameters");
        const auto settings=form.getTableModel()->getGeneratorsSettings();for(int i=0;i<settings.size();++i)check(settings[i].getFormBonus()==(i==1?8:0),"UX 1: applying to selected athletes preserves every other setting");
        check(form.findChild<QPushButton *>("pushButton_generate")->isEnabled(),"UX 1: generation becomes available after applying the draft");
        auto *tableNode=qobject_cast<QuickNode *>(bridge->find("tableView"));auto *adapter=qobject_cast<QuickTableModel *>(tableNode->values().value("table").value<QObject *>());const int tableId=tableNode->values().value("id").toInt();
        auto *search=item(scene(&form)->rootObject(),"tableSearch-"+QString::number(tableId));check(search&&search->isVisible(),"UX 2: desktop tables expose search without an icon hunt");
        main.activateWindow();scene(&form)->setFocus();clickControl(scene(&form),search->objectName());QTest::keyClick(scene(&form),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&form),"zzzx");QTest::qWait(100);
        check(adapter->count()==0&&adapter->selectedCount()==0&&source->model()->rowCount()==pool.size(),"UX 2: filters hide rows without modifying data or retaining hidden bulk targets");
        check(bridge->root()->values().value("participantCount").toInt()==pool.size(),"UX 1: All scope counts the complete roster independently of search");
        clickControl(scene(&form),"clearTableFilter-"+QString::number(tableId));check(adapter->count()==pool.size(),"UX 2: the empty state offers a direct filter reset");
        check(scope->property("currentIndex").toInt()==1,"UX 1: changing selection counts cannot silently switch the batch scope to All");
        item(scene(&form)->rootObject(),"formGeneratorTable")->forceActiveFocus();QTest::keyClick(scene(&form),Qt::Key_F,Qt::ControlModifier);QTest::qWait(50);check(search->hasActiveFocus(),"UX 2: Ctrl+F focuses the active table search");
        source->selectionModel()->select(form.getTableModel()->index(0,0),QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);source->selectionModel()->select(form.getTableModel()->index(2,0),QItemSelectionModel::Select|QItemSelectionModel::Rows);QTest::qWait(50);
        auto *selection=item(scene(&form)->rootObject(),"tableSelection-"+QString::number(tableId));check(selection&&selection->isVisible()&&adapter->selectedCount()==2,"UX 2: batch selection has a persistent count");adapter->clearSelection();check(adapter->selectedCount()==0,"UX 2: selection can be cleared independently of data");
        source->selectionModel()->select(form.getTableModel()->index(1,1),QItemSelectionModel::ClearAndSelect);QTest::qWait(30);
        check(adapter->selectedCount()==1&&adapter->data(adapter->index(1,0),Qt::UserRole+2).toBool(),"UX 2: a selected editable cell highlights and counts its athlete");
        adapter->clearSelection();check(!adapter->data(adapter->index(1,0),Qt::UserRole+2).toBool(),"UX 2: the keyboard cursor cannot masquerade as a selected target");
        render(&form,"ux-explicit-form",true);form.reject();
    }
    save.setHills({new Hill(GlobalDatabase::get()->getEditableGlobalHills().first())});auto rules=GlobalDatabase::get()->getEditableCompetitionRules().first();rules.setCompetitionType(CompetitionRules::Individual);rules.setRounds({RoundInfo(6,true),RoundInfo(3,true)});save.setCompetitionRules({rules});
    auto *calendar=new SeasonCalendar("UX calendar");auto *event=new CompetitionInfo(save.getHillsReference().first());event->setRules(rules);calendar->setCompetitions({event});Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.getSeasonsReference().append(season);save.setActualSeason(&save.getSeasonsReference().last());save.updateNextCompetitionIndex();
    SaveJumpersList preset("Skład A");preset.setIsDefault(true);preset.setJumpers({pool[2],pool[0]});save.setJumpersLists({preset});
    {
        main.resize(1440,900);CompetitionConfigWindow config(CompetitionConfigWindow::SeasonCompetition,&main,&save);config.show();QTest::qWait(150);auto *bridge=config.findChild<QuickBridge *>();auto *roster=bridge->findChild<QuickRoster *>();clickControl(scene(&config),"editStartList");
        check(roster->count()==6,"UX 3: default saved list keeps the initial squad available");clickControl(scene(&config),"rosterPresetRemove");check(roster->count()==4,"UX 3: saved lists offer direct removal without nested menus");roster->undo();clickControl(scene(&config),"rosterPresetOrder");check(config.getSeasonCompetitionJumpers().first()==pool[2],"UX 3: applying saved order is a direct distinct action");roster->undo();
        render(&config,"ux-saved-rosters",true);main.activateWindow();scene(&config)->setFocus();clickControl(scene(&config),"rosterOpenPresets");clickControl(scene(&config),"rosterPresetRemove");check(roster->count()==4,"UX 3: the narrow popover offers the same direct actions");
        clickControl(scene(&config),"rosterOpenPresets");clickControl(scene(&config),"rosterPresetAdd");check(roster->count()==6,"UX 3: adding a saved list restores membership on small screens");config.reject();
    }
    for(int i=0;i<2;++i){auto *next=new CompetitionInfo(save.getHillsReference().first());next->setRules(rules);calendar->getCompetitionsReference().append(next);}save.updateNextCompetitionIndex();
    {
        main.resize(520,600);SimulationSaveManagerWindow manager(&save);manager.fillNextCompetitionInformations();manager.show();QTest::qWait(150);auto *bridge=manager.findChild<QuickBridge *>();bridge->edit(bridge->root()->values().value("id").toInt(),4);QTest::qWait(120);auto *backend=bridge->findChild<QuickCalendar *>();auto *workspace=item(scene(&manager)->rootObject(),"calendarWorkspace");check(backend&&workspace,"UX 5: calendar workspace exists");
        const auto events=backend->events();const auto firstId=events[0].toMap().value("id").toString(),secondId=events[1].toMap().value("id").toString();clickControl(scene(&manager),"calendar-select-"+firstId);check(!workspace->property("editing").toBool(),"UX 5: selecting a calendar event does not navigate away from the list");
        auto *rows=item(scene(&manager)->rootObject(),"calendarEvents");QMetaObject::invokeMethod(rows,"positionViewAtIndex",Q_ARG(int,1),Q_ARG(int,1));QTest::qWait(60);auto *second=item(scene(&manager)->rootObject(),"calendar-select-"+secondId);check(second&&second->isVisible(),"UX 5: second calendar event is visible");const auto point=second->mapToItem(scene(&manager)->rootObject(),second->boundingRect().center()).toPoint();QTest::mouseClick(scene(&manager),Qt::LeftButton,Qt::ControlModifier,point);QTest::qWait(60);
        if(backend->selection().value("count").toInt()!=2){main.grab().save("/tmp/sj-calendar-modifiers.png");qInfo()<<"MODIFIERS"<<backend->selection()<<rows->height()<<rows->mapToItem(scene(&manager)->rootObject(),QPointF())<<point;}
        check(backend->selection().value("count").toInt()==2&&!workspace->property("editing").toBool(),"UX 5: modifier selection remains on the list and accumulates targets");clickControl(scene(&manager),"calendarEditSelection");check(workspace->property("editing").toBool()&&backend->selection().value("count").toInt()==2,"UX 5: explicit Edit opens one inspector for the complete selection");
        render(&manager,"ux-calendar-selection",true);clickControl(scene(&manager),"calendarReturnToList");clickControl(scene(&manager),"calendarClearSelection");check(backend->selection().value("count").toInt()==0,"UX 5: clearing selection leaves no ambiguous edit targets");
        rows=item(scene(&manager)->rootObject(),"calendarEvents");QMetaObject::invokeMethod(rows,"positionViewAtIndex",Q_ARG(int,0),Q_ARG(int,1));QTest::qWait(60);auto *firstRow=item(scene(&manager)->rootObject(),"calendar-row-"+firstId);check(firstRow&&firstRow->isVisible(),"UX 5: an event row supports keyboard focus");main.activateWindow();scene(&manager)->setFocus();QTest::qWait(50);firstRow->forceActiveFocus();check(firstRow->hasActiveFocus(),"UX 5: keyboard target owns active focus");QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(60);check(workspace->property("editing").toBool()&&backend->selection().value("count").toInt()==1,"UX 5: Enter opens the focused event for editing");manager.reject();
    }
    qInfo()<<"PASS: five UX workflows: scoped form edits, visible search, saved lists, contextual competition controls and explicit calendar editing";
}

static void advancementRegressions() {
    Jumper a("A","One","POL"), b("B","Two","NOR"), c("C","Three","GER"), d("D","Four","AUT"), e("E","Five","FIN");
    CompetitionResults results;
    auto add=[&](Jumper *athlete,double points){JumpData jump; jump.setJumper(athlete);jump.setPoints(points);results.addJump(athlete,jump);results.sortInDescendingOrder();};
    check(QuickRaceStats::biggestAdvancement(results,2,1).isEmpty(),"empty results have no advancement");
    add(&a,120);add(&b,110);add(&c,100);add(&d,90);add(&e,80);
    check(QuickRaceStats::biggestAdvancement(results,1,1).isEmpty() && QuickRaceStats::biggestAdvancement(results,2,1).isEmpty(),"first round and unjumped second round have no advancement");
    add(&c,100);add(&d,100);
    auto gain=QuickRaceStats::biggestAdvancement(results,2,1);
    check(gain.value("places").toInt()==2 && gain.value("name")==c.getNameAndSurname() && gain.value("entries").toList().size()==2,"equal biggest gains retain both athletes with a stable current-rank order");
    const auto entries=gain.value("entries").toList();
    check(entries[0].toMap().value("from").toInt()==3 && entries[0].toMap().value("to").toInt()==1 && entries[1].toMap().value("from").toInt()==4 && entries[1].toMap().value("to").toInt()==2,"advancement compares the previous full field with current cumulative positions");
    results.getResultOfIndividualJumper(&e)->setPosition(1);
    check(QuickRaceStats::biggestAdvancement(results,2,1)==gain,"an athlete without a current-round jump is excluded from advancement");results.sortInDescendingOrder();
    add(&a,0);add(&a,300);add(&c,0);add(&d,0);
    gain=QuickRaceStats::biggestAdvancement(results,3,1);
    check(gain.value("places").toInt()==2 && gain.value("name")==a.getNameAndSurname(),"third-round advancement uses the completed second-round baseline");
    CompetitionResults tied;
    for(auto *athlete:{&a,&b,&c}){JumpData jump;jump.setJumper(athlete);jump.setPoints(athlete==&c?90:100);tied.addJump(athlete,jump);}
    JumpData jump;jump.setJumper(&c);jump.setPoints(50);tied.addJump(&c,jump);tied.sortInDescendingOrder();
    check(QuickRaceStats::biggestAdvancement(tied,2,1).value("places").toInt()==2,"previous-round ties use places 1,1,3 rather than dense ranking");
    CompetitionResults teams;Team pol("POL"),nor("NOR");
    for(auto *team:{&pol,&nor})for(int i=0;i<4;++i){JumpData entry;entry.setPoints(team==&pol?25:30);teams.addJump(team,entry);}teams.sortInDescendingOrder();
    check(QuickRaceStats::biggestAdvancement(teams,2,4).isEmpty(),"team advancement waits for a jump in the new round");
    JumpData teamJump;teamJump.setPoints(40);teams.addJump(&pol,teamJump);teams.sortInDescendingOrder();
    gain=QuickRaceStats::biggestAdvancement(teams,2,4);
    check(gain.value("places").toInt()==1 && gain.value("name").toString()=="POL","team advancement compares complete previous-round team totals");
    qInfo()<<"PASS: biggest advancement, shared gains, tied places, third rounds and team baselines";
}

static void presetSerializationRegression() {
    Hill hill=GlobalDatabase::get()->getEditableGlobalHills().first();
    CompetitionInfo training(&hill),competition(&hill);training.setSerieType(CompetitionInfo::Training);competition.setTrainings({&training});
    SeasonCalendar calendar("Serialization regression");calendar.setCompetitions({&training,&competition});
    SeasonCalendarPreset preset(&calendar);preset.setName("Regression preset");preset.setHills({{hill.getName(),hill.getHSPoint()},{hill.getName(),hill.getHSPoint()}});
    for(auto *event:preset.getCalendarReference().getCompetitionsReference())check(event->getResultsReference().getCompetition()==event,"preset competition result containers belong to their cloned events");
    const auto first=SeasonCalendarPreset::getJsonObject(preset),second=SeasonCalendarPreset::getJsonObject(preset);
    for(auto *event:preset.getCalendarReference().getCompetitionsReference())check(event->getHill()==&hill,"serializing a calendar preset preserves its in-memory hill pointers");
    check(competition.getHill()==&hill && training.getHill()==&hill,"serializing a preset leaves the source calendar intact");
    check(first==second,"repeated preset serialization is stable");
    for(const auto &event:first.value("calendar").toObject().value("competitions").toArray())check(!event.toObject().contains("hill-id"),"preset JSON omits save-specific hill identities");
    check(first.value("hills").toArray().size()==2,"preset JSON retains its named hill references");
    qDeleteAll(preset.getCalendarReference().getCompetitionsReference());
    qInfo()<<"PASS: calendar preset serialization preserves live hill assignments";
}

static void csvCollisionRegression() {
    QTemporaryDir directory;check(directory.isValid(),"isolated CSV export directory");
    const auto path=directory.path()+"/results.csv";
    QFile original(path);check(original.open(QIODevice::WriteOnly),"create an existing competition export");original.write("original results\n");original.close();
    CompetitionInfo competition;
    check(competition.saveToCsvFile(directory.path()+"/","results.csv"),"CSV export succeeds when its requested name is already used");
    check(original.open(QIODevice::ReadOnly) && original.readAll()==QByteArray("original results\n"),"CSV export never overwrites earlier competition results");original.close();
    check(competition.saveToCsvFile(directory.path(),"results.csv"),"CSV export accepts a directory without a trailing separator");
    const auto files=QDir(directory.path()).entryList({"*.csv"},QDir::Files);
    check(files.size()==3 && files.contains("results (2).csv") && files.contains("results (3).csv"),"repeated exports create distinct numbered files");
    for(const auto &name:files){QFile file(directory.path()+"/"+name);check(file.open(QIODevice::ReadOnly) && !file.readAll().isEmpty(),"every preserved or new CSV contains its data");}
    qInfo()<<"PASS: automatic CSV exports preserve existing files and repeated results";
}

static void randomBoundsRegression() {
    check(MyRandom::randomInt(7,7)==7 && MyRandom::randomDouble(2.5,2.5)==2.5,"a fixed random range returns its only valid value");
    for(int i=0;i<200;++i) {
        const double narrow=MyRandom::randomDouble(1.001,1.009,2);
        check(narrow>=1.001 && narrow<1.009,"a random interval narrower than the requested precision stays within bounds");
        const double grid=MyRandom::randomDouble(1.011,1.039,2);
        check(grid>=1.011 && grid<1.039 && std::abs(grid*100-std::round(grid*100))<1e-8,"decimal sampling respects both exact bounds and requested precision");
        const double negative=MyRandom::randomDouble(-.009,0,2);
        check(negative>=-.009 && negative<0,"a negative fractional interval ending at zero does not collapse to zero");
        const int edge=MyRandom::randomInt(std::numeric_limits<int>::max()-1,std::numeric_limits<int>::max());
        check(edge>=std::numeric_limits<int>::max()-1,"inclusive integer sampling does not overflow at INT_MAX");
        const int any=MyRandom::randomInt(std::numeric_limits<int>::min(),std::numeric_limits<int>::max());Q_UNUSED(any)
        const double large=MyRandom::randomDouble(-std::numeric_limits<double>::max(),std::numeric_limits<double>::max());
        check(std::isfinite(large),"large finite bounds do not overflow during random interpolation");
    }
    qInfo()<<"PASS: singleton, subprecision, zero-ended and extreme random bounds";
}

static void fractionalPointsPresetRegression() {
    PointsForPlacesPreset preset("Fractional points");preset.setPointsForPlaces({{1,12.5},{2,7.25},{3,0.5},{4,0.0}});
    auto restored=PointsForPlacesPreset::getFromJson(PointsForPlacesPreset::getJsonObject(preset));
    check(restored.getPointsForPlaces()==preset.getPointsForPlaces(),"points preset JSON round-trip preserves fractional and zero awards");
    const QJsonObject collection{{"pointsForPlacesPresets",QJsonArray{PointsForPlacesPreset::getJsonObject(preset)}}};
    const auto loaded=PointsForPlacesPreset::getPointsForPlacesPresetsVectorFromJson(QJsonDocument(collection).toJson());
    check(loaded.size()==1 && loaded.first().getPointsForPlaces()==preset.getPointsForPlaces(),"global points preset collection preserves decimal awards on reload");
    qInfo()<<"PASS: fractional points presets survive single and collection reloads";
}

static void backendRegressions() {
    fractionalPointsPresetRegression();
    randomBoundsRegression();
    csvCollisionRegression();
    presetSerializationRegression();
    advancementRegressions();
    { Classification source("Existing classification");source.setAltPointsForPlaces({{1,25.0}});ClassificationEditorWidget editor(&source);check(editor.getClassification()==&source,"classification constructor initializes the supplied source");editor.setClassification(nullptr);check(!editor.getClassification(),"classification source can be cleared safely"); }

    const QVector<Wind> input{Wind(0,2),Wind(180,1)};
    for(int i=0;i<100;++i)check(WindsCalculator::getAveragedWind(input,WindsCalculator::Original).getStrengthToAveragedWind()==.5,"averaging fixed winds is deterministic");
    QVector<Wind> sectors{Wind(0,2),Wind(0,2),Wind(0,2)};
    const auto exposed=WindsCalculator::getScoringWinds(sectors,100,90,JumpSimulator::Full);
    check(exposed[2].getStrength()==1 && sectors[2].getStrength()==2,"distance weighting uses the current sector fraction without mutating winds");
    check(std::isfinite(WindsGenerator::getRangeOfWindSector(1,10).first),"single-sector hill has a finite range");
    auto *db=GlobalDatabase::get();Hill hill=db->getEditableGlobalHills().first();
    hill.setKPoint(100);hill.setHSPoint(110);hill.setPointsForKPoint(60);hill.setPointsForMeter(2);hill.setPointsForGate(5);
    Jumper a=db->getEditableGlobalJumpers()[0],b=db->getEditableGlobalJumpers()[1],c=db->getEditableGlobalJumpers()[2],d=db->getEditableGlobalJumpers()[3];
    CompetitionRules rules=db->getEditableCompetitionRules().first();rules.setHasDsq(false);rules.setHasJudgesPoints(false);rules.setHasWindCompensations(false);rules.setHasGateCompensations(false);rules.setRounds({RoundInfo(4,true),RoundInfo(2,true)});
    CompetitionInfo competition;competition.setHill(&hill);competition.setRules(rules);
    auto &results=competition.getResultsReference();results.setCompetition(&competition);
    check(results.getResultByIndex(-1)==nullptr,"negative result index is rejected");
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&competition);manager.setCompetitionRules(competition.getRulesPointer());manager.setResults(&results);manager.setRoundsJumpers({{&a,&b,&c,&d}});manager.setActualRound(1);manager.setRoundsStartingGates({0});manager.setActualGate(0);manager.setActualWinds(&sectors);manager.setStartListStatuses({{&a,StartListCompetitorStatus::Finished},{&b},{&c},{&d}});manager.setActualStartListIndex(1);
    JumpData first;first.setJumper(&a);first.setCompetition(&competition);first.setPoints(100);results.addJump(&a,first);
    manager.updateToBeatLineDistance();check(manager.getToBeatLineDistance()==120.5,"leader line requires the first half metre that actually takes the lead");
    for(int repeat=0;repeat<20;++repeat){manager.updateToBeatLineDistance();check(manager.getToBeatLineDistance()==120.5,"leader line is stable on refresh");}
    manager.updateToAdvanceLineDistance();check(manager.getToAdvanceLineDistance()==-1,"no guaranteed advancement line before enough athletes finish");
    JumpData second=first;second.setJumper(&b);second.setPoints(80);results.addJump(&b,second);manager.getStartListStatusesReference()[1].setJumpStatus(StartListCompetitorStatus::Finished);manager.setActualStartListIndex(2);
    manager.updateToAdvanceLineDistance();check(manager.getToAdvanceLineDistance()==120,"advancement threshold uses completed competitors and remaining places");
    JumpData third=first;third.setJumper(&c);third.setPoints(90);results.addJump(&c,third);manager.getStartListStatusesReference()[2].setJumpStatus(StartListCompetitorStatus::Finished);manager.setActualStartListIndex(3);
    manager.updateToAdvanceLineDistance();check(manager.getToAdvanceLineDistance()==115,"advancement line moves to the second completed result for the final athlete");
    results.getResultOfIndividualJumper(&a)->setPointsSum(70);results.getResultOfIndividualJumper(&b)->setPointsSum(60);results.getResultOfIndividualJumper(&c)->setPointsSum(50);
    competition.getRulesPointer()->setHasGateCompensations(true);manager.setCoachGateForNextJumper(true);manager.setActualCoachGate(-2);manager.updateToBeatLineDistance();check(manager.getToBeatLineDistance()==104.5,"coach gate compensation is awarded only at 95 percent HS");
    {
        competition.getRulesPointer()->setHasGateCompensations(false);manager.setCoachGateForNextJumper(false);
        RoundInfo duel(4,true);duel.setKO(true);duel.setAdvancingFromKOGroup(1);competition.getRulesPointer()->setRounds({duel,RoundInfo(3,true)});
        results.getResultOfIndividualJumper(&a)->setPointsSum(100);results.getResultOfIndividualJumper(&b)->setPointsSum(80);results.getResultOfIndividualJumper(&c)->setPointsSum(120);
        KOGroup firstGroup,secondGroup;firstGroup.setJumpers({&a,&b});secondGroup.setJumpers({&c,&d});QVector<KOGroup> groups{firstGroup,secondGroup};
        KORoundManager ko(&groups,&results,&competition.getRulesPointer()->getRoundsReference()[0],&manager);ko.setLuckyLosersCount(1);ko.updateActualGroup(&d);ko.updateStatuses();manager.setKOManager(&ko);
        manager.updateToAdvanceLineDistance();check(manager.getToAdvanceLineDistance()==110,"KO advancement line includes an already guaranteed lucky-loser place");manager.setKOManager(nullptr);
    }
    JumpManipulator manipulation;manipulation.setExactJudges({16,17,18,19,20});
    rules.setHasJudgesPoints(true);JumpSimulator simulator(&a,sectors,&hill);simulator.setCompetitionRules(&rules);simulator.setManipulator(&manipulation);simulator.setGate(0);simulator.setCompetitionStartGate(0);simulator.setDSQBaseProbability(0);simulator.setJumpsImportance(5);simulator.simulateJump();
    check(simulator.getJumpData().getJudgesPoints()==54,"judging drops the minimum and maximum independently for ascending marks");
    rules.setWindCompensationDistanceEffect(JumpSimulator::Disabled);simulator.setHasCoachGate(true);simulator.setCoachGate(-1);simulator.simulateJump();
    check(simulator.getJumpData().getAveragedWind()==2 && simulator.getJumpData().getWindCompensation()==0,"wind measurements remain available when wind points are disabled");
    check(simulator.getJumpData().getBeats95HSPercents()==JumpSimulator::isJumperBeat95HSPercents(simulator.getJumpData().getDistance(),hill.getHSPoint()),"coach threshold metadata remains correct when gate points are disabled");
    WindsGeneratorSettingsWidgetInputItem item;QSignalSpy submitted(&item,&WindsGeneratorSettingsWidgetInputItem::submitted);item.findChild<QPushButton *>("pushButton_submit")->click();check(submitted.count()==1,"wind bulk-fill confirmation is connected");
    QVector<WindGenerationSettings> settings(6);const double originalStrength=settings[0].getBaseWindStrength();WindsGeneratorSettingsEditorWidget editor;editor.setWindGenerationSettings(&settings);editor.setKPoint(120);editor.fillSettingsInputs();
    QTimer accept;QObject::connect(&accept,&QTimer::timeout,[&]{for(auto *widget:QApplication::allWidgets())if(auto *dialog=qobject_cast<QDialog *>(widget);dialog && (dialog->parentWidget()==&editor || dialog->property("sjQuickOwner").value<QObject *>()==&editor) && dialog->isVisible()){auto *input=dialog->findChild<WindsGeneratorSettingsWidgetInputItem *>();if(input){input->findChild<QDoubleSpinBox *>("doubleSpinBox_baseWindStrength")->setValue(3.25);input->findChild<QPushButton *>("pushButton_submit")->click();accept.stop();}}});accept.start(20);editor.askForSettingsForFillAll();accept.stop();
    auto *box=editor.findChild<QToolBox *>("toolBox");check(box->count()==6,"bulk-fill retains all wind sectors");
    for(int i=0;i<box->count();++i){auto *input=qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(i));check(input->getSettings()==&settings[i] && input->getBaseWindStrengthFromInput()==3.25,"wind sector controls reference persistent settings after bulk fill");}
    check(settings[0].getBaseWindStrength()==originalStrength,"bulk fill changes input state without committing canceled parent dialogs");
    qInfo()<<"PASS: scoring, target-line, wind and bulk-editor regressions";
}

static void interactionRegressions(MainWindow &main) {
    main.show();main.resize(1024,640);
    QDialog fixture(&main);auto *layout=new QVBoxLayout(&fixture);
    auto *integer=new QSpinBox;integer->setObjectName("rapidInteger");integer->setRange(0,100);integer->setValue(20);layout->addWidget(integer);
    auto *fraction=new QDoubleSpinBox;fraction->setObjectName("rapidFraction");fraction->setRange(-10,10);fraction->setSingleStep(.1);fraction->setValue(0);layout->addWidget(fraction);
    fixture.show();QTest::qWait(220);auto *view=scene(&fixture);
    const auto press=[&](const QString &name,int count,int hold=0){
        auto *button=item(view->rootObject(),name);check(button&&button->isVisible(),"numeric step button exists");
        const auto at=button->mapToItem(view->rootObject(),button->boundingRect().center()).toPoint();
        for(int i=0;i<count;++i){QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,at);QTest::qWait(hold?hold:10);QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,at);QTest::qWait(10);}
    };
    press("numberIncrease-rapidInteger",10);check(integer->value()==30,"ten rapid presses increment ten times without click cooldown");
    press("numberDecrease-rapidInteger",10);check(integer->value()==20,"ten rapid decrement presses are preserved");
    press("numberIncrease-rapidInteger",1,700);check(integer->value()>=28,"holding a step button repeats smoothly");
    integer->setValue(99);QTest::qWait(100);press("numberIncrease-rapidInteger",5);check(integer->value()==100,"repeated increment clamps at upper bound");
    press("numberIncrease-rapidFraction",10);check(std::abs(fraction->value()-1)<.001,"rapid decimal stepping preserves all fractional steps");
    fixture.reject();QTest::qWait(120);
    {
        QMessageBox message(QMessageBox::Question,"Zapis sezonu","Czy zapisać zmiany?",QMessageBox::Yes|QMessageBox::No,&main);message.show();QTest::qWait(200);auto *quick=scene(&message);quick->grabFramebuffer();
        check(message.button(QMessageBox::Yes)->text()==QString::fromUtf8("Tak") && message.button(QMessageBox::No)->text()==QString::fromUtf8("Nie"),"standard confirmation actions use Polish captions");
        auto *body=item(quick->rootObject(),"modalMessage-0");check(body && body->width()>500 && body->height()<40 && body->y()>35,"short message uses available width and sits below its title without wrapping into four lines");message.reject();
    }
    for(const auto &size:QList<QSize>{{1024,640},{360,480}}){
        main.resize(size);QTest::qWait(100);
        QMessageBox message(QMessageBox::Information,QString("Nowy rekord skoczni — ").repeated(6),QString("Zawodnik ustanowił nowy rekord skoczni. ").repeated(25),QMessageBox::Ok|QMessageBox::Cancel,&main);
        message.show();QTest::qWait(250);
        // Standard Qt dialogs may restore a fixed native size after Quick attaches.
        message.setFixedSize(320,200);message.layout()->activate();QTest::qWait(250);
        auto *quick=scene(&message);check(quick&&quick->status()==QQuickWidget::Ready,"long record message loads Quick content");
        const auto checkFooter=[&]{
            quick->grabFramebuffer();auto *footer=item(quick->rootObject(),"modalButtons");check(footer&&footer->height()>0,"modal action footer exists");
            const auto bounds=footer->mapRectToItem(quick->rootObject(),footer->boundingRect());
            check(bounds.top()>=0&&bounds.bottom()<=quick->height(),"modal actions remain visible below scrollable title and content");
            check(quick->geometry()==message.rect() && message.height()<=main.height()-40,"native resize cannot crop Quick dialog or exceed application height");
        };checkFooter();main.resize(size.width(),size.height()-40);QTest::qWait(250);checkFooter();
        auto *button=message.button(QMessageBox::Ok);auto *bridge=message.findChild<QuickBridge *>();bridge->refresh();QTest::qWait(50);
        QSignalSpy accepted(&message,&QDialog::accepted);clickControl(quick,"action-"+(button->objectName().isEmpty()?button->text().remove('&'):button->objectName()));check(accepted.count()==1,"visible message action closes the actual native dialog");
    }
    main.resize(1024,640);qInfo()<<"PASS: rapid clicks, hold repeat, decimal steps and unclipped modal actions";
}

int main(int argc,char **argv) {
    QApplication app(argc,argv);
    app.setOrganizationName("SJ-Sim-UI-Test"); app.setApplicationName("SjSimUI");
    QTemporaryDir data; check(data.isValid(),"isolated data");
    const QString build=QCoreApplication::applicationDirPath();
    output=build+"/ui-renders"; QDir().mkpath(output);
    for(const QString directory:{QString("userData"),QString("flags"),QString("translations")}) {
        QDirIterator files(build+"/"+directory,QDir::Files,QDirIterator::Subdirectories);
        while(files.hasNext()) {
            const QString file=files.next(),relative=QDir(build).relativeFilePath(file),destination=data.path()+"/"+relative;
            QDir().mkpath(QFileInfo(destination).absolutePath()); check(QFile::copy(file,destination),"copy fixture");
        }
    }
    QDir::setCurrent(data.path());
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,data.path()+"/settings");
    MainWindow main; applicationWindow=&main; main.setParentApplication(&app); new QuickUi(&main);
    check(QApplication::font().family()=="Barlow","bundled interface font is active");
    check(main.findChild<QuickBridge *>()->version()=="1.3.0 beta-3","one release version is shared with the interface");
    backendRegressions();
    if(app.arguments().contains("--team-only")){main.show();runCompetition(main,2);return 0;}
    if(app.arguments().contains("--interaction-only")){interactionRegressions(main);return 0;}
    if(app.arguments().contains("--space-only")){spaceRegressions(main);return 0;}
    if(app.arguments().contains("--ux-only")){uxRegressions(main);runCompetition(main,0);return 0;}
    if(app.arguments().contains("--audit-only")){auditDataRegressions();auditFormRegressions();auditStatisticsRegressions(main);return 0;}
    if(app.arguments().contains("--audit-stats-only")){auditStatisticsRegressions(main);return 0;}
    if(app.arguments().contains("--audit-form-only")){auditFormRegressions();return 0;}
    if(app.arguments().contains("--audit-data-only")){auditDataRegressions();return 0;}
    if(app.arguments().contains("--geometry-only")){geometryRegressions(main);return 0;}
    if(app.arguments().contains("--rosters-only")){rosterRegressions(main);return 0;}
    if(app.arguments().contains("--menus-only")){menuRegressions(main);return 0;}
    if(app.arguments().contains("--workflow-only")){seasonWorkflowRegressions(main);return 0;}
    if(app.arguments().contains("--navigation-only")){navigationRegressions(main);return 0;}
    if(app.arguments().contains("--backend-only"))return 0;
    if(app.arguments().contains("--competitions-only")) {
        main.show();QTest::qWait(100);GlobalAppSettings::get()->setLiveCompetition(false);
        for(int scenario=0;scenario<4;++scenario) runCompetition(main,scenario);
        seasons();qInfo()<<"PASS: full individual, KO and team competitions and season analysis";return 0;
    }
    if(!app.arguments().contains("--layout-only")){auditDataRegressions();auditFormRegressions();auditStatisticsRegressions(main);interactionRegressions(main);spaceRegressions(main);uxRegressions(main);navigationRegressions(main);menuRegressions(main);seasonWorkflowRegressions(main);rosterRegressions(main);geometryRegressions(main);}
    render(&main,"home",true);
    {
        DatabaseEditorWindow dialog; render(&dialog,"database",true);
        auto *bridge=dialog.findChild<QuickBridge *>();
        auto *listNode=qobject_cast<QuickNode *>(bridge->find("listView"));
        auto *table=qobject_cast<QuickTableModel *>(listNode->values().value("table").value<QObject *>());if(table)qInfo()<<"Database rows"<<table->count();check(table&&table->count()>5,"lazy original database model");
        const int fullCount=table->count();table->select(0,0,0,true);QTest::qWait(100);table->select(2,0,Qt::ControlModifier);QTest::qWait(30);
        QAbstractItemView *view=nullptr;for(auto *candidate:dialog.findChildren<QAbstractItemView *>("listView")) if(candidate->model()&&candidate->model()->rowCount()==fullCount){view=candidate;break;}
        check(view&&view->selectionModel()->selectedRows().size()==2,"multi-selection reaches original model");
        auto *quick=scene(&dialog);auto *quickTable=item(quick->rootObject(),"dataTable-"+QString::number(listNode->values().value("id").toInt()));check(quickTable!=nullptr,"database table keyboard target");
        quickTable->forceActiveFocus();QTest::keyClick(quick,Qt::Key_A,Qt::ControlModifier);QTest::qWait(50);
        check(table->count()==fullCount && view->selectionModel()->selectedRows().size()==fullCount,"Ctrl+A selects all records without adding an unwanted record");
        table->setFilter("Casey");check(table->count()>0&&table->count()<fullCount,"table search filters data");table->setFilter("");check(table->count()==fullCount,"clearing search restores all rows");
        render(&dialog,"database-editor",true);
        auto *tabs=dialog.findChild<QTabWidget *>();
        for(int i=0;i<tabs->count();++i) { bridge->edit(id(bridge,tabs->objectName()),i); QTest::qWait(100); render(&dialog,QString("database-tab%1").arg(i)); }
        dialog.hide();
    }
    {
        NewSimulationSaveConfigurationWindow dialog({}); render(&dialog,"new-save",true);
        auto *bridge=dialog.findChild<QuickBridge *>();
        auto *line=dialog.findChild<QLineEdit *>("lineEdit_name");
        auto *quickInput=item(scene(&dialog)->rootObject(),"quick-lineEdit_name");
        check(quickInput!=nullptr,"visible new-save name");
        auto *quick=scene(&dialog);auto *input=item(quick->rootObject(),"input-lineEdit_name");check(input!=nullptr,"actual Quick text input");
        input->forceActiveFocus();QTest::keyClick(quick,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(quick,"Test season");QTest::keyClick(quick,Qt::Key_Tab);
        QTest::qWait(50); check(line->text()=="Test season","text round trip");
        check(dialog.findChild<QPushButton *>("pushButton_submit")->isEnabled(),"live validation enables submit");
        bridge->activate(id(bridge,"pushButton_submit")); QTest::qWait(60);
        check(dialog.result()==QDialog::Accepted,"original save validation/accept");
    }
    if(app.arguments().contains("--layout-only")) return 0;
    {
        SimulationSavesWindow dialog; render(&dialog,"saves",true); dialog.hide();
    }
    {
        SingleJumpsConfigWindow dialog; render(&dialog,"jump-config",true); dialog.hide();
    }
    {
        CompetitionConfigWindow dialog(CompetitionConfigWindow::SingleCompetition); render(&dialog,"competition-config",true);
        auto *bridge=dialog.findChild<QuickBridge *>();
        auto *spin=dialog.findChild<QSpinBox *>("spinBox_startGate");
        check(spin!=nullptr,"original gate exists"); bridge->edit(id(bridge,"spinBox_startGate"),19); check(spin->value()==19,"gate reaches original logic");
        for(auto *tool:dialog.findChildren<QToolBox *>()) for(int i=0;i<tool->count();++i) {
            bridge->edit(id(bridge,tool->objectName()),i); QTest::qWait(120);
            render(&dialog,QString("competition-section-%1-%2").arg(tool->objectName()).arg(i));
        }
        dialog.hide();
    }
    {
        QVector<WindGenerationSettings> settings(6);QDialog dialog;dialog.setWindowTitle("Wiatr");
        auto *layout=new QVBoxLayout(&dialog);auto *editor=new WindsGeneratorSettingsEditorWidget(&dialog);layout->addWidget(editor);
        editor->setWindGenerationSettings(&settings);editor->setKPoint(120);editor->fillSettingsInputs();
        render(&dialog,"wind-editor",true);
        auto *bridge=dialog.findChild<QuickBridge *>();auto *node=qobject_cast<QuickNode *>(bridge->find(editor->objectName()));
        check(node && node->values().value("kind")=="wind" && node->children().size()==6,"dedicated wind editor exposes all sectors");
        const int editorId=node->values().value("id").toInt();auto *box=editor->findChild<QToolBox *>("toolBox");
        const double initial=qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(0))->getBaseWindStrengthFromInput();
        bridge->setWind(editorId,2,{{"strength",3.25}},false);QTest::qWait(30);
        check(qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(2))->getBaseWindStrengthFromInput()==3.25,"single sector edit reaches original inputs");
        check(qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(0))->getBaseWindStrengthFromInput()==initial,"single sector edit leaves other sectors unchanged");
        bridge->copyWindSector(editorId,2);QTest::qWait(30);
        for(int i=0;i<6;++i)check(qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(i))->getBaseWindStrengthFromInput()==3.25,"bulk copy applies all sectors");
        bridge->undoWind(editorId);QTest::qWait(30);
        check(qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(0))->getBaseWindStrengthFromInput()==initial,"wind undo restores the previous settings");
        bridge->randomizeWind(editorId,0,0);QTest::qWait(30);
        for(int i=0;i<6;++i)check(qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(i))->getBaseWindDirectionFromInput()==0,"randomization respects the chosen direction");
        bridge->undoWind(editorId);QTest::qWait(30);
        check(settings[0].getBaseWindStrength()==initial,"canceling a wind editor leaves stored settings unchanged");dialog.reject();
    }
    {
        AppSettingsWindow dialog; dialog.setMainWindowParent(&main); dialog.setupLanguagesComboBox(); render(&dialog,"settings",true);
        auto *bridge=dialog.findChild<QuickBridge *>();bridge->edit(id(bridge,"comboBox_language"),0);bridge->edit(id(bridge,"comboBox_language"),1);QTest::qWait(150);
        auto *headline=item(scene(&main)->rootObject(),"homeDescription");check(headline&&headline->property("text").toString()=="Ski jumping simulator","English translation updates the live Quick scene");
        dialog.hide();render(&main,"home-en");bridge->edit(id(bridge,"comboBox_language"),0);QTest::qWait(100);dialog.hide();
    }
    {
        SingleJumpsManager manager;
        auto *db=GlobalDatabase::get();
        check(!db->getEditableGlobalJumpers().isEmpty()&&!db->getEditableGlobalHills().isEmpty(),"loaded fixture");
        manager.setJumper(db->getEditableGlobalJumpers().first());manager.setHill(db->getEditableGlobalHills().first());manager.setJumpsCount(12);manager.setGate(15);
        manager.simulate(); SingleJumpsResultsWindow dialog(&manager);dialog.fillWindow();dialog.installShortcuts();render(&dialog,"jump-results",true);
        auto *bridge=dialog.findChild<QuickBridge *>();
        for(auto *tool:dialog.findChildren<QToolBox *>()) for(int i=0;i<tool->count();++i) { bridge->edit(id(bridge,tool->objectName()),i);QTest::qWait(120);render(&dialog,QString("results-section-%1").arg(i)); }
        dialog.hide();
        JumpDataDetailedInfoWindow information(&manager.getJumpsReference().first());
        information.fillJumpInformations();render(&information,"jump-information",true);information.hide();
    }
    { HelpWindow dialog; render(&dialog,"help");dialog.hide(); }
    { ThanksWindow dialog; render(&dialog,"credits");dialog.hide(); }
    { CountriesEditorWindow dialog; render(&dialog,"countries");dialog.hide(); }
    { JumpDataInfoChoiceDialog dialog;dialog.setChoice(&GlobalAppSettings::get()->getJumpDataInfoChoiceReference());dialog.fillInputs();render(&dialog,"jump-information-choice");dialog.hide(); }
    { SingleJumpsResultsTableSortDialog dialog;render(&dialog,"sort-jump-results");dialog.hide(); }
    { CharacteristicInputDialog dialog;dialog.fillComboBox();render(&dialog,"characteristic-editor");dialog.hide(); }
    { NewJumpsImportancePresetDialog dialog; render(&dialog,"importance");dialog.hide(); }
    { RandomWindConfigWindow dialog; render(&dialog,"random-wind");dialog.hide(); }
    { JumpManipulatorConfigWindow dialog; render(&dialog,"manipulation");dialog.hide(); }
    { JumpersFormGeneratorConfigWindow dialog; render(&dialog,"form-generator");dialog.hide(); }
    { JumpersListEditorWindow dialog; render(&dialog,"jumper-list");dialog.hide(); }
    { QMessageBox dialog(QMessageBox::Question,"Confirm","Continue simulation?",QMessageBox::Yes|QMessageBox::No);render(&dialog,"confirmation");dialog.hide(); }
    { QInputDialog dialog;dialog.setLabelText("Value");dialog.setInputMode(QInputDialog::IntInput);render(&dialog,"input-dialog");dialog.hide(); }
    {
        QProgressDialog dialog("Simulation","Cancel",0,100);dialog.setMinimumDuration(0);dialog.setAutoClose(false);
        dialog.findChild<QProgressBar *>()->setObjectName("progressBar");dialog.findChild<QPushButton *>()->setObjectName("cancelProgress");
        dialog.setValue(37);render(&dialog,"progress-dialog",true);
        auto *bridge=dialog.findChild<QuickBridge *>();auto *progress=qobject_cast<QuickNode *>(bridge->find("progressBar"));
        check(progress && progress->values().value("value").toInt()==37,"progress value reaches the Quick interface");
        dialog.setValue(63);bridge->refresh();check(progress->values().value("value").toInt()==63,"progress updates during work");
        QSignalSpy canceled(&dialog,&QProgressDialog::canceled);bridge->activate(id(bridge,"cancelProgress"));QTest::qWait(50);
        check(canceled.size()==1 && dialog.wasCanceled(),"Quick cancellation reaches the original operation");dialog.hide();
    }
    {
        KOGroup group;group.setNumber(1);QVector<Jumper *> athletes;
        for(auto &athlete:GlobalDatabase::get()->getEditableGlobalJumpers())if(athletes.size()<4)athletes.append(&athlete);
        group.setJumpers(athletes.mid(0,2));KOGroupEditorWidget dialog;dialog.setGroup(&group);dialog.setJumpersList(&athletes);dialog.updateGroupNumber();dialog.updateComboBoxesLayout();
        render(&dialog,"ko-group-editor",true);
        auto *bridge=dialog.findChild<QuickBridge *>();const auto choices=dialog.findChildren<QComboBox *>();check(choices.size()==2,"KO editor has both athlete selectors");
        std::function<QuickNode *(QuickNode *)> choice=[&](QuickNode *node)->QuickNode * {
            if(node->values().value("kind")=="choice")return node;
            for(const auto &child:node->children())if(auto *found=choice(child.value<QuickNode *>()))return found;
            return nullptr;
        };
        auto *select=choice(bridge->root());check(select!=nullptr,"KO athlete selection is projected");bridge->edit(select->values().value("id").toInt(),3);
        check(dialog.getJumpersFromInputs().contains(athletes[3]),"KO selection reaches original group inputs");
        QSignalSpy submitted(&dialog,&KOGroupEditorWidget::submitted);bridge->activate(id(bridge,"pushButton_submit"));QTest::qWait(50);
        check(submitted.size()==1,"KO editor preserves the original submit action");dialog.hide();
    }
    {
        QDialog dialog;auto *layout=new QVBoxLayout(&dialog);auto *source=new QTableWidget(2,2,&dialog);source->setObjectName("keyboardTable");layout->addWidget(source);
        source->setItem(0,0,new QTableWidgetItem("Name"));source->setItem(0,1,new QTableWidgetItem("12"));
        auto *checked=new QTableWidgetItem("Enabled");checked->setCheckState(Qt::Unchecked);source->setItem(1,0,checked);source->setItem(1,1,new QTableWidgetItem("24"));
        render(&dialog,"keyboard-table");auto *bridge=dialog.findChild<QuickBridge *>();auto *node=qobject_cast<QuickNode *>(bridge->find("keyboardTable"));
        auto *model=qobject_cast<QuickTableModel *>(node->values().value("table").value<QObject *>());check(model!=nullptr,"editable table model");
        auto *quick=scene(&dialog);auto *table=item(quick->rootObject(),"dataTable-"+QString::number(node->values().value("id").toInt()));check(table!=nullptr,"keyboard table is visible");
        model->select(0,0,0);table->forceActiveFocus();QTest::keyClick(quick,Qt::Key_Right);QTest::qWait(30);check(model->currentColumn()==1,"keyboard cell navigation reaches the source model");
        QTest::keyClick(quick,Qt::Key_F2);QTest::qWait(50);check(quick->quickWindow()->activeFocusItem()->inherits("QQuickTextInput"),"F2 opens the cell editor");
        QTest::keyClicks(quick,"42");QTest::keyClick(quick,Qt::Key_Return);QTest::qWait(50);check(source->item(0,1)->text()=="42","keyboard editing commits to the original model");
        model->select(1,0,0);table->forceActiveFocus();QTest::keyClick(quick,Qt::Key_Space);QTest::qWait(30);check(checked->checkState()==Qt::Checked,QString("Space toggles checkable cells (focus %1, state %2)").arg(quick->quickWindow()->activeFocusItem()?quick->quickWindow()->activeFocusItem()->metaObject()->className():"none").arg(checked->checkState()));dialog.hide();
    }
    GlobalAppSettings::get()->setLiveCompetition(false);
    for(int scenario=0;scenario<4;++scenario) runCompetition(main,scenario);
    seasons();
    qInfo()<<"PASS: fresh Qt Quick screens, bridge validation and"<<renders<<"responsive renders";
    return 0;
}
