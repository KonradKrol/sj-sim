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
#include "../source/UI/Competition/Results/TeamResultsTreeModel.h"
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
#include "../source/UI/AuthorWindow.h"
#include <QDesktopServices>
#include <QQmlComponent>
#include <atomic>
#include <cstdio>
#include "../source/UI/ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../source/UI/ResultsShowing/ResultsHistoryWidget.h"
#include "../source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.h"
#include "../source/UI/Seasons/ItemModels/ClassificationsArchiveListModel.h"
#include "../source/single-jumps/SingleJumpsManager.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/global/CountryCodes.h"
#include "../source/UI/EditorWidgets/RoundInfoEditorWidget.h"
#include "../source/global/CountryFlagsManager.h"
#include "../source/global/GlobalAppSettings.h"
#include "../source/global/GlobalSimulationSettings.h"
#include "../source/global/MyRandom.h"
#include "../source/seasons/SeasonCalendarPreset.h"
#include <QApplication>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlDebuggingEnabler>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickStyle>
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
#include <QSpinBox>
#include <QFileDialog>
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
#include "../source/UI/EditorWidgets/KOSystem/KOGroupsListView.h"
#include "../source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h"
#include "../source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../source/competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include <memory>
#include <functional>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

static std::atomic<int> recursiveLayoutWarnings{0};
static QtMessageHandler originalMessageHandler=nullptr;
static void observeLayoutWarnings(QtMsgType type,const QMessageLogContext &context,const QString &message) {
    if(message.contains("Detected recursive rearrange"))++recursiveLayoutWarnings;
    if(originalMessageHandler)originalMessageHandler(type,context,message);
    else std::fprintf(stderr,"%s\n",qPrintable(qFormatLogMessage(type,context,message)));
}
struct LayoutWarningRegressionGuard {
    LayoutWarningRegressionGuard(){originalMessageHandler=qInstallMessageHandler(observeLayoutWarnings);}
    ~LayoutWarningRegressionGuard(){qInstallMessageHandler(originalMessageHandler);if(recursiveLayoutWarnings.load()){std::fprintf(stderr,"FAIL: %d recursive Qt Quick Layout warnings\n",recursiveLayoutWarnings.load());std::exit(1);}}
};

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
static QPoint controlPoint(QQuickWidget *view,const QString &name);
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
            if(name=="author") {
                auto *photo=item(view->rootObject(),"authorPortrait");auto *bio=item(view->rootObject(),"authorBiography");auto *heading=item(view->rootObject(),"authorHeading");
                check(photo && bio && heading,"author portrait and biography exist at every screen size");
                const auto photoBounds=photo->mapRectToItem(view->rootObject(),QRectF(0,0,photo->width(),photo->height())),bioBounds=bio->mapRectToItem(view->rootObject(),bio->boundingRect());
                check(photo->width()>0 && photo->height()>0 && !photoBounds.intersects(bioBounds) && bioBounds.left()>=0 && bioBounds.right()<=view->width(),"author uses available width without clipping or overlapping the portrait and text");
                check(size.width()<900 ? photoBounds.bottom()<=bioBounds.top() : photoBounds.right()<=bioBounds.left(),"author portrait precedes the full-width biography on narrow screens and sits beside it on desktop");
                if(size.width()<900){const auto headingBounds=heading->mapRectToItem(view->rootObject(),heading->boundingRect());check(photoBounds.right()<=headingBounds.left() && photoBounds.top()<headingBounds.bottom() && headingBounds.top()<photoBounds.bottom() && bioBounds.width()>photo->width()+heading->width(),"small-screen author portrait sits naturally beside the heading with full-width biography below");}
                if(size.width()<900)check(photo->property("fillMode").toInt()==1 && photo->height()<=200 && photo->width()<=150 && photo->property("paintedWidth").toReal()<=photo->width()+1 && photo->property("paintedHeight").toReal()<=photo->height()+1,"small-screen portrait shows the full photo in a compact frame");
            }
            if(name=="jump-results") {
                auto *table=item(view->rootObject(),"singleJumpsResultsTable");check(table&&table->isVisible(),"single-jump results remain the default workspace at all sizes");
                const auto bounds=table->mapRectToItem(view->rootObject(),table->boundingRect());
                check(table->height()>=100 && bounds.top()>=0 && std::abs(bounds.bottom()-(view->height()-12))<=1,"single-jump table fills the remaining vertical space and stays inside the viewport");
            }
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
static void raceOptionsRegressions(MainWindow &main,CompetitionManagerWindow *window) {
    auto *view=scene(window);
    for(const auto &size:QList<QSize>{{1240,900},{520,900}}) {
        main.resize(size);QTest::qWait(150);
        for(const auto &category:QList<QPair<QString,int>>{{"raceOptions",0},{"raceConditionsOptions",1},{"raceJumpOptions",2}}) {
            const auto before=view->grabFramebuffer();
            clickControl(view,category.first);QTest::qWait(120);
            auto *panel=view->rootObject()->findChild<QObject *>("raceOptionsPanel");
            auto *workspace=item(view->quickWindow()->contentItem(),"raceOptionsWorkspace");
            check(panel && panel->property("visible").toBool() && panel->property("opened").toBool(),"competition option panel actually opens: "+category.first);
            check(workspace && workspace->isVisible() && workspace->property("section").toInt()==category.second,"competition option has the requested visible section: "+category.first);
            const auto bounds=workspace->mapRectToScene(workspace->boundingRect());
            check(workspace->window()==view->quickWindow() && bounds.width()>200 && bounds.height()>100 && QRectF(QPointF(),view->size()).contains(bounds.center()),"competition options are inside the visible application viewport");
            check(before!=view->grabFramebuffer(),"opening competition options changes the rendered frame");
            main.grab().save(output+QString("/race-options-%1-%2.png").arg(category.second).arg(size.width()));
            clickControl(view,"raceOptionsClose");QTest::qWait(100);
            check(!panel->property("visible").toBool(),"competition options close and leave the competition visible");
        }
    }
    qInfo()<<"PASS: Przebieg, Warunki and Skok open visibly and close at desktop and narrow widths";
}
#include "competition-ties-gate-regressions.h"
#include "competition-advancement-regressions.h"

static void advanceColorRegressions(MainWindow &main) {
    main.show();auto *view=scene(&main);auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const bool dark=appearance->dark();
    QQmlComponent tableComponent(view->engine(),QUrl("qrc:/ui/DataTable.qml"));std::unique_ptr<QObject> tableObject(tableComponent.create(view->rootContext()));auto *table=qobject_cast<QQuickItem *>(tableObject.get());check(table,"color fixture table loads");table->setWidth(300);
    QQmlComponent rowComponent(view->engine(),QUrl("qrc:/ui/TableRow.qml"));std::unique_ptr<QObject> rowObject(rowComponent.beginCreate(view->rootContext()));auto *row=qobject_cast<QQuickItem *>(rowObject.get());check(row,"color fixture row loads");row->setProperty("tableView",QVariant::fromValue(table));row->setProperty("rowCells",QVariantList());rowComponent.completeCreate();row->setParentItem(view->rootObject());row->setWidth(300);
    for(bool night:{false,true}){
        appearance->setDark(night);
        for(bool teams:{false,true})for(bool external:{false,true}){
            table->setProperty("teamResults",teams);row->setProperty("rowDepth",0);row->setProperty("advanceState",external?-1:0);row->setProperty("qualificationState",external?0:-1);row->setProperty("rowSelected",true);row->setProperty("latestJump",true);QTest::qWait(20);
            const auto color=row->property("color").value<QColor>();check(color.green()>color.red()+6 && color.green()>color.blue()+4,"confirmed individual, KO, team and external advancement remains visibly green despite selection and latest-jump emphasis");
            const auto before=color;QTest::mouseMove(view,QPoint(150,qRound(row->height()/2)));QTest::qWait(20);check(row->property("color").value<QColor>()==before,"hover cannot replace confirmed advancement color");
            auto *marker=item(row,"latestJumpIndicator-0");check(marker && marker->isVisible(),"latest-jump marker coexists with green qualification rows");
        }
        row->setProperty("advanceState",1);row->setProperty("qualificationState",-1);QTest::qWait(20);const auto color=row->property("color").value<QColor>();check(color.green()<=color.blue(),"confirmed elimination remains neutral rather than red or green");
    }
    appearance->setDark(dark);qInfo()<<"PASS: confirmed advancement color across selection, hover, latest jump, qualifications, teams and both themes";
}

static void latestJumpRegressions() {
    Jumper a("A","Latest","POL"),b("B","Other","NOR");Hill hill;CompetitionInfo info(&hill);
    CompetitionRules rules;rules.setRounds({RoundInfo(2,true),RoundInfo(2,true)});info.setRules(rules);auto &results=info.getResultsReference();
    auto add=[&](Jumper *jumper,double points){JumpData jump(jumper,&hill);jump.setPoints(points);results.addJump(jumper,jump);return jump.getIDStr();};
    add(&a,100);const auto second=add(&b,90);results.sortInDescendingOrder();
    ResultsTableModel native(CompetitionRules::Individual,&results);QTableView view;view.setModel(&native);QuickTableModel adapter(&view,&view);
    auto owner=[&]{QString name;int count=0;for(int row=0;row<adapter.count();++row)if(adapter.data(adapter.index(row),Qt::UserRole+13).toBool()){++count;name=adapter.data(adapter.index(row),Qt::UserRole+1).toList().value(1).toMap().value("text").toString();}check(count==1,"exactly one individual result is marked latest");return name;};
    check(owner()==b.getNameAndSurname() && results.getLatestJumpId()==second,"latest jump is independent of leader and table order");
    native.setLastJumper(nullptr);check(owner()==b.getNameAndSurname(),"clearing legacy round-end selection cannot clear latest jump");
    adapter.setFrozen(true);const auto third=add(&a,80);results.sortInDescendingOrder();
    check(owner()==b.getNameAndSurname(),"live snapshot retains the previous marker until reveal");adapter.setFrozen(false);
    check(owner()==a.getNameAndSurname() && results.getLatestJumpId()==third,"live reveal moves the marker to the accepted jump");
    adapter.select(results.getIndexOfJumperResult(&b),0,0);check(owner()==a.getNameAndSurname(),"selecting another athlete never moves the latest-jump marker");
    results.sortInAscendingOrder();adapter.invalidateText();check(owner()==a.getNameAndSurname(),"latest identity survives a standings reorder");
    results.getResultOfIndividualJumper(&a)->getJumpsReference().removeLast();
    for(const auto &result:results.getResultsReference())check(!results.containsLatestJump(result),"removing the latest cancelled jump leaves no stale marker");
    Team team("POL"),other("NOR");team.setJumpers({&a,&b});other.setJumpers({&b});CompetitionInfo teams(&hill);rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(2);teams.setRules(rules);auto &standings=teams.getResultsReference();
    JumpData first(&a,&hill);first.setPoints(100);standings.addJump(&team,first);JumpData last(&b,&hill);last.setPoints(90);standings.addJump(&team,last);
    standings.getResultOfTeam(&team)->updateTeamJumpersResults();TeamResultsTreeModel tree(nullptr);tree.setResults(&standings);tree.setupTreeItems();
    check(tree.index(0,0).data(CompetitionAdvancement::LatestJumpRole).toBool(),"team summary marks its latest jump");
    const auto parent=tree.index(0,0);check(!tree.index(0,0,parent).data(CompetitionAdvancement::LatestJumpRole).toBool() && tree.index(1,0,parent).data(CompetitionAdvancement::LatestJumpRole).toBool(),"only the team member who actually jumped is marked");
    standings.getResultOfTeam(&team)->getJumpsReference().last().setDSQ(true);JumpData blocked(&a,&hill);standings.addJump(&team,blocked);
    check(standings.getLatestJumpId()==last.getIDStr(),"a rejected post-DSQ jump cannot replace the accepted latest identity");
    qInfo()<<"PASS: latest-jump identity, sorting, round ends, frozen reveal, selection, cancellation and team members";
}

static void checkLatestJumpPresentation(CompetitionManagerWindow *window) {
    auto *bridge=window->findChild<QuickBridge *>();bridge->refresh();QTest::qWait(30);
    auto *table=item(scene(window)->rootObject(),"raceResults");auto *model=table->property("table").value<QuickTableModel *>();check(model && !model->frozen(),"result presentation is released before checking latest jump");
    int summaries=0;for(int row=0;row<model->count();++row)if(model->data(model->index(row),Qt::UserRole+13).toBool() && model->data(model->index(row),Qt::UserRole+3).toInt()==0)++summaries;
    check(summaries==1,"manual and automatic competition paths retain exactly one latest result summary");
    for(int row=0;row<model->count();++row)if(model->data(model->index(row),Qt::UserRole+13).toBool()){
        auto *rows=item(scene(window)->rootObject(),"tableRows-"+QString::number(table->property("ownerId").toInt()));
        QMetaObject::invokeMethod(rows,"positionViewAtIndex",Q_ARG(int,row),Q_ARG(int,3));QTest::qWait(30);scene(window)->grabFramebuffer();
        auto *marker=item(scene(window)->rootObject(),"latestJumpIndicator-"+QString::number(row));check(marker && marker->isVisible(),"latest marker stays visible regardless of advancement, podium and current selection");break;
    }
}

static void jumpHistoryRegressions(MainWindow &main,CompetitionManagerWindow *window,int scenario) {
    main.resize(1440,900);auto *bridge=window->findChild<QuickBridge *>();bridge->refresh();QTest::qWait(80);
    auto *preview=window->findChild<JumperCompetitionResultsWidget *>();check(preview && preview->roundsCount()==3 && preview->selectedRound()==2,"three-series fixture opens its latest jump");
    auto *footer=main.findChild<QQuickWidget *>("quickApplicationFooter");auto *hint=item(footer->rootObject(),"jumpSeriesShortcutHint");check(hint && hint->isVisible() && hint->property("text").toString().contains("Shift+klik") && !hint->property("text").toString().contains("Alt+"),"competition footer advertises historical-series selection instead of the Back shortcut");
    auto *view=scene(window);auto *choice=item(view->rootObject(),"jumpRoundChoice");check(choice && choice->isVisible(),"jump card exposes an earlier-series choice");
    QMetaObject::invokeMethod(choice,"activated",Q_ARG(int,0));QTest::qWait(80);check(preview->selectedRound()==0 && !bridge->jumpSummary().value("weighted").toBool(),"card can open the first unweighted jump");
    QMetaObject::invokeMethod(choice,"activated",Q_ARG(int,1));QTest::qWait(80);
    auto *result=preview->getJumperResult();check(preview->selectedRound()==1 && bridge->jumpSummary().value("weighted").toBool(),"card opens a weighted historical series");
    check(std::abs(bridge->jumpSummary().value("weightedTotal").toDouble()-result->pointsForJumps(2,2))<.001 && bridge->jumpSummary().value("weightTerms").toList().size()==2,"weight explanation and total belong to the selected series");
    clickControl(view,"jumpWeightToggle");check(item(view->rootObject(),"jumpWeightCalculation")->isVisible(),"weight calculation expands inside the jump card");render(window,"weighted-jump-history",true);clickControl(view,"jumpWeightToggle");main.resize(1440,900);QTest::qWait(80);
    bridge->selectPreviewRound(2);QTest::qWait(60);
    auto &lastRule=result->getCompetition()->getRulesPointer()->getRoundsReference()[2];const bool resetBefore=lastRule.getResetPoints();lastRule.setResetPoints(true);bridge->refresh();QTest::qWait(40);
    check(bridge->jumpSummary().value("resetPoints").toBool() && !bridge->jumpSummary().value("weighted").toBool() && item(view->rootObject(),"jumpResetInfo")->isVisible() && !item(view->rootObject(),"jumpWeightToggle")->isVisible(),"reset series shows only the discreet reset label instead of weights");
    lastRule.setResetPoints(resetBefore);bridge->refresh();QTest::qWait(30);
    auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const int oldMode=appearance->teamResultsMode();if(scenario==2){appearance->setTeamResultsMode(2);QTest::qWait(60);}
    auto *table=item(view->rootObject(),"raceResults");auto *model=table->property("table").value<QuickTableModel *>();int row=-1;
    for(int r=0;r<model->count();++r){const auto cells=model->data(model->index(r),Qt::UserRole+1).toList();const int name=scenario==2?2:1;if(cells.value(name).toMap().value("text").toString()==result->getJumper()->getNameAndSurname()){row=r;break;}}
    if(scenario==2)check(!table->property("displayColumns").value<QJSValue>().toVariant().toList().contains(4),"team results omit the combined distance column");
    check(row>=0,"selected jumper is present in its individual or team results row");auto *rows=item(view->rootObject(),"tableRows-"+QString::number(table->property("ownerId").toInt()));QMetaObject::invokeMethod(rows,"positionViewAtIndex",Q_ARG(int,row),Q_ARG(int,3));QTest::qWait(80);
    const int column=scenario==2?6:5;const auto columns=table->property("displayColumns").value<QJSValue>().toVariant().toList();qreal x=0;
    for(const auto &entry:columns){QVariant width;QMetaObject::invokeMethod(table,"columnWidth",Q_RETURN_ARG(QVariant,width),Q_ARG(QVariant,columns.indexOf(entry)));if(entry.toInt()==column){x+=width.toReal()/2;break;}x+=width.toReal();}
    auto *marker=item(view->rootObject(),"latestJumpIndicator-"+QString::number(row));check(marker,"target row is materialized");auto *rowItem=marker->parentItem();const auto at=rowItem->mapToItem(view->rootObject(),QPointF(x,rowItem->height()/2)).toPoint();
    QTest::mouseClick(view,Qt::LeftButton,Qt::ShiftModifier,at);QTest::qWait(100);check(preview->selectedRound()==1 && preview->roundsCount()==3,"real Shift-click on the second-series column opens series two while retaining all series choices");
    QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,at);QTest::qWait(100);check(preview->selectedRound()==2,"ordinary result click still opens the latest jump");
    if(scenario==2)appearance->setTeamResultsMode(oldMode);
    check(preview->findChildren<JumpDataDetailedInfoWindow *>().size()==3,"repeated preview rebuilding releases old jump detail pages");qInfo()<<"PASS: jump history, Shift-click series selection, weights and bounded preview pages"<<scenario;
}

static void runCompetition(MainWindow &main,int scenario,bool bulk=false,int bulkAction=0) {
    Q_UNUSED(main)
    auto *db=GlobalDatabase::get();
    const bool superTeam=QCoreApplication::arguments().contains("--super-team-only");
    const bool forceTeamDsq=QCoreApplication::arguments().contains("--team-dsq-only");
    CompetitionRules rules=db->getEditableCompetitionRules().first();
    rules.setHasDsq(false);rules.setCompetitionType(scenario==2?CompetitionRules::Team:CompetitionRules::Individual);
    rules.setJumpersInTeamCount(4);
    if(forceTeamDsq)rules.setHasDsq(true);
    RoundInfo first(bulk&&scenario!=2?100:16,true),second(scenario==2?3:bulk?(scenario==1?60:30):8,true);
    if(scenario==1) { first.setKO(true);first.setCountInKOGroup(2);first.setAdvancingFromKOGroup(1);first.setKoGroupsSelectionType(KOGroup::Classic); }
    const bool multiKo=QCoreApplication::arguments().contains("--ko-multi-round-only");
    if(multiKo){second.setKO(true);second.setCountInKOGroup(2);second.setAdvancingFromKOGroup(1);second.setKoGroupsSelectionType(KOGroup::Classic);second.setPreviousRoundsWeights({.5});}
    const bool weighted=QCoreApplication::arguments().contains("--round-points-bulk-only");
    if(QCoreApplication::arguments().contains("--history-dns-only"))second.setPreviousRoundsWeights({.5});
    if(weighted) { if(scenario==2)second.setPreviousRoundsWeights({.5});else second.setResetPoints(true); }
    rules.setRounds({first,second});
    if(QCoreApplication::arguments().contains("--jump-history-only")){second.setPreviousRoundsWeights({.95});RoundInfo third(scenario==2?2:4,true);third.setPreviousRoundsWeights({.5,.95});rules.setRounds({first,second,third});}
    if(superTeam){
        bool found=false;for(const auto &preset:db->getEditableCompetitionRules())if(preset.getName()=="Super Team"){rules=preset;found=true;break;}
        check(found && rules.getJumpersInTeamCount()==2 && rules.getRounds().size()==3,"real Super Team preset");
        rules.setHasDsq(false);
    }
    if(QCoreApplication::arguments().contains("--superko-only")){bool found=false;for(const auto &preset:db->getEditableCompetitionRules())if(preset.getName().compare("SuperKO by fortonse",Qt::CaseInsensitive)==0){rules=preset;rules.setHasDsq(false);found=true;break;}check(found,"real SuperKO by fortonse preset exists");}
    if(QCoreApplication::arguments().contains("--manual-ko-only")){rules.getRoundsReference()[1].setKO(true);rules.getRoundsReference()[1].setKoGroupsSelectionType(CompetitionRules::Classic);}
    CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition);
    config.getHillEditor()->setHill(&db->getEditableGlobalHills().first());config.getHillEditor()->fillHillInputs();
    config.getCompetitionRulesEditor()->setCompetitionRules(&rules);config.getCompetitionRulesEditor()->fillCompetitionRulesInputs();
    auto jumpers=config.getCompetitionJumpers();check(jumpers.size()>=16,"competition fixture");jumpers.resize(QCoreApplication::arguments().contains("--superko-only")?20:bulk?qMin(100,jumpers.size()):16);
    if(scenario==2) {
        QVector<Jumper *> all;for(auto &jumper:db->getEditableGlobalJumpers())all.append(&jumper);
        const int teamSize=superTeam?2:4;
        auto teams=Team::constructTeamsVectorByJumpersList(all,teamSize);check(teams.size()>=(superTeam?13:4),"team fixture");teams.resize(superTeam?qMin(20,teams.size()):4);jumpers.clear();
        for(auto &team:teams) { Team::cutTeamJumpers(&team,teamSize);jumpers+=team.getJumpers(); } config.setCompetitionTeams(teams);
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
            if(!bulk)competitionCoachGateRegressions(window,check);
            auto *resultsTable=item(scene(window)->rootObject(),"raceResults");check(resultsTable && !resultsTable->property("searchEnabled").toBool() && resultsTable->property("plain").toBool(),"all live result tables are plain and cannot search");
            if(scenario==2){
                auto *appearance=scene(window)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const int oldMode=appearance->teamResultsMode();
                auto *table=resultsTable->property("table").value<QuickTableModel *>();check(table,"team results adapter exists");
                appearance->setTeamResultsMode(1);QTest::qWait(40);QuickAppearance stored;check(stored.teamResultsMode()==1,"team results mode is persisted");
                appearance->setTeamResultsMode(oldMode);QTest::qWait(40);
                if(forceTeamDsq){window->getManager()->setBaseDSQProbability(1);window->autoSimulateCompetition();check(window->getManager()->checkCompetitionEnd(),"automatic team DSQ competition finishes");
                    for(const auto &result:window->getManager()->getResults()->getResultsReference())check(result.isTeamDisqualified() && result.getPosition()==0 && result.getJumps().size()==1,"whole-team DSQ stops all subsequent members and removes classified place");
                    qInfo()<<"PASS: automatic whole-team DSQ, notifications, no later jumps and unclassified results";window->accept();return;
                }
            }

            if(auto *start=item(scene(window)->rootObject(),"raceStartList"))check(!start->property("searchEnabled").toBool() && !start->property("searchOpen").toBool(),"competition start list never exposes athlete filtering");
            if(!bulk && scenario==0) {
                auto *info=window->getManager()->getCompetitionInfo();const int type=info->getSerieType(),rounds=info->getExceptionalRoundsCount();
                info->setSerieType(CompetitionInfo::Qualifications);info->setExceptionalRoundsCount(1);bridge->refresh();QTest::qWait(30);
                check(item(scene(window)->rootObject(),"raceRoundStatus")->property("text").toString()==QString::fromUtf8("Kwalifikacje"),"single-round qualifications do not display a fictitious second round");
                info->setSerieType(CompetitionInfo::Competition);bridge->refresh();QTest::qWait(30);
                check(item(scene(window)->rootObject(),"raceRoundStatus")->property("text").toString().contains("1 / 1"),"ordinary single-round competition retains its 1 / 1 round counter");
                info->setSerieType(type);info->setExceptionalRoundsCount(rounds);bridge->refresh();
            }
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
            if(superTeam){
                auto *manager=dynamic_cast<TeamCompetitionManager *>(window->getManager());check(manager,"Super Team manager");
                const auto checkPreview=[&]{
                    auto *preview=window->findChild<JumperCompetitionResultsWidget *>();check(preview && !preview->isHidden(),"automatic Super Team preview is visible");
                    bool current=false;for(auto &team:manager->getResults()->getResultsReference())for(auto &member:team.getTeamJumpersResultsReference())if(&member==preview->getJumperResult())current=true;
                    check(current,"automatic Super Team preview belongs to the current result buffers");
                    const auto &jumps=preview->getJumperResult()->getJumpsReference();
                    for(auto *details:preview->findChildren<JumpDataDetailedInfoWindow *>())if(details->isVisibleTo(preview)){
                        bool currentJump=false;for(const auto &jump:jumps)if(&jump==details->getJumpData())currentJump=true;
                        check(currentJump,"Super Team jump details point to a current jump after buffer rebuilding");
                    }
                    bridge->refresh();QTest::qWait(60);
                };
                for(int round=1;round<=3;++round){
                    qInfo()<<"PASS: starting Super Team automatic round"<<round<<"teams"<<manager->getActualRoundTeamsReference().size();
                    check(manager->getActualRound()==round,"Super Team round sequence");
                    bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(60);
                    if(round==2){
                        window->autoSimulateGroup();checkPreview();
                        check(manager->checkGroupEnd(),"automatic Super Team group ends at its boundary");
                        bridge->activate(id(bridge,"pushButton_goToNext"));QTest::qWait(60);
                        QTimer acceptCount;QObject::connect(&acceptCount,&QTimer::timeout,[&]{for(auto *widget:QApplication::allWidgets())if(auto *input=qobject_cast<QInputDialog *>(widget);input && input->isVisible()){input->setIntValue(3);input->accept();}});acceptCount.start(20);
                        window->autoSimulateJumps();acceptCount.stop();checkPreview();
                    }
                    if(round==3)window->autoSimulateCompetition();else window->autoSimulateRound();
                    checkPreview();
                    check(manager->checkRoundEnd(),"Super Team automatic round completes both athlete groups");
                    for(auto *team:manager->getActualRoundTeamsReference()){
                        auto *result=manager->getResults()->getResultOfTeam(team);
                        check(result && result->getJumps().size()==round*2 && std::isfinite(result->getPointsSum()),"Super Team keeps complete finite results");
                    }
                    qInfo()<<"PASS: completed Super Team automatic round"<<round;
                    if(round<3){bridge->activate(id(bridge,"pushButton_goToNext"));checkPreview();QTest::qWait(100);}
                }
                check(manager->checkCompetitionEnd(),"Super Team finishes after three automatic rounds");window->accept();return;
            }
            if(QCoreApplication::arguments().contains("--history-dns-only")) {
                auto *history=window->findChild<ResultsHistoryWidget *>();check(history,"live competition owns shared history widget");window->autoSimulateRound();bridge->refresh();QTest::qWait(30);
                check(history->roundChoice()->count()==1 && history->roundChoice()->currentData().toInt()==0,"a just-finished series identical to current results has only Teraz");
                bridge->activate(id(bridge,"pushButton_goToNext"));QTest::qWait(120);bridge->refresh();QTest::qWait(30);
                check(history->roundChoice()->count()==1,"previous series becomes selectable only when the next series actually starts");bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(80);bridge->refresh();QTest::qWait(30);
                check(window->getManager()->getActualRound()==2 && history->roundChoice()->count()==2,"completed first series is available in compact history choice");
                const auto before=QJsonDocument(CompetitionResults::getJsonObject(*window->getManager()->getResults())).toJson();
                auto *resultChrome=main.findChild<QQuickWidget *>("quickApplicationHeader");QQuickItem *roundPicker=nullptr;
                if(resultChrome->width()<1100){clickControl(resultChrome,"raceViewMenu");roundPicker=item(scene(window)->quickWindow()->contentItem(),"historyRoundSelector");}
                else roundPicker=item(resultChrome->rootObject(),"historyRoundSelector");
                if(scenario==2){auto *choices=roundPicker?roundPicker->parentItem():nullptr;auto *ranking=choices?item(choices,"historyStandingsMode"):nullptr;auto *teamMode=choices?item(choices,"teamResultsMode"):nullptr;check(ranking&&ranking->isVisible()&&teamMode&&ranking->parentItem()==roundPicker->parentItem()&&teamMode->parentItem()==roundPicker->parentItem(),"individual/team ranking shares the series and team-view control bar");}
                check(roundPicker && roundPicker->isVisible(),"round selector is reachable from application toolbar without a table toolbar row");clickControl(resultChrome->width()<1100?scene(window):resultChrome,"historyRoundSelector");QTest::qWait(50);auto *choice=item(scene(window)->quickWindow()->contentItem(),"raceResultViewOption-1");check(choice && choice->isVisible(),"series choices open below the header in the competition surface");const auto bounds=choice->mapRectToScene(choice->boundingRect());auto *popup=scene(window)->rootObject()->findChild<QObject *>("raceViewOptions");auto *background=popup?popup->property("background").value<QQuickItem *>():nullptr;check(background && background->mapRectToScene(background->boundingRect()).contains(bounds),"dropdown background encloses the full last option including content padding");scene(window)->grabFramebuffer().save(output+QString("/result-dropdown-open-%1.png").arg(scenario));check(bounds.top()>=0 && bounds.bottom()<=scene(window)->height(),"round dropdown options are fully visible below the application header");if(scenario==1){QTest::keyClick(scene(window),Qt::Key_Down);QTest::keyClick(scene(window),Qt::Key_Return);}else clickControl(scene(window),"raceResultViewOption-1");bridge->refresh();QTest::qWait(30);check(history->active() && history->paused() && window->property("resultHistoryPaused").toBool(),"historical round selection pauses competition");
                window->autoSimulateCompetition();window->autoSimulateRound();window->skipCurrentJump();bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(30);
                check(QJsonDocument(CompetitionResults::getJsonObject(*window->getManager()->getResults())).toJson()==before,"historical preview cannot mutate competition through manual, automatic or DNS actions");
                render(window,QString("history-live-%1").arg(scenario),scenario==2);
                auto *chrome=main.findChild<QQuickWidget *>("quickApplicationHeader");check(item(chrome->rootObject(),"returnToCompetition") && item(chrome->rootObject(),"returnToCompetition")->isVisible(),"historical mode offers a visible return action in application header");clickControl(chrome,"returnToCompetition");bridge->refresh();QTest::qWait(30);check(!history->paused(),"header return action restores live competition");auto *jumper=window->getManager()->getActualJumper();auto *team=scenario==2?static_cast<TeamCompetitionManager *>(window->getManager())->getActualTeam():nullptr;
                window->skipCurrentJump();bridge->refresh();QTest::qWait(30);
                auto *result=scenario==2?window->getManager()->getResults()->getResultOfTeam(team)->getTeamJumperResult(jumper):window->getManager()->getResults()->getResultOfIndividualJumper(jumper);
                check(result && result->getJumpsReference().last().getDNS() && !result->getJumpsReference().last().getDSQ() && result->getJumpsReference().last().getPoints()==0,"skip records exactly one zero-point DNS rather than DSQ");
                if(scenario==2){
                    auto copied=window->getManager()->getResults()->getResultOfTeam(team)->getJumpsReference().first();check(copied.getPositionInGroupForTeamCompetition()>0 && copied.getPositionInRoundForTeamCompetition()>0,"copied historical team jump keeps valid round and group positions");
                    JumpData missing(jumper,window->getManager()->getCompetitionInfo()->getHill());missing.setCompetition(window->getManager()->getCompetitionInfo());check(missing.getPositionInGroupForTeamCompetition()==0 && missing.getPositionInRoundForTeamCompetition()==0,"missing team jump identity cannot index a negative series");
                    check(!window->getManager()->getResults()->getResultOfTeam(team)->isTeamDisqualified(),"DNS does not disqualify the team");history->modeChoice()->setCurrentIndex(1);bridge->refresh();QTest::qWait(30);
                    check(history->previewWidget()->getJumperResult()->getJumper()==jumper,"live individual team ranking retains the latest jumper card rather than switching to its leader");
                    QHash<Jumper *,double> expected;for(const auto &squad:window->getManager()->getResults()->getResultsReference())for(const auto &jump:squad.getJumps())expected[jump.getJumper()]+=jump.getPoints();
                    for(const auto &row:history->snapshot.getResultsReference())check(qAbs(expected.value(row.getJumper())-row.getPointsSum())<.001,"team individual ranking sums raw jump points");
                    check(history->paused() && window->property("resultHistoryPaused").toBool(),"individual ranking pauses the team competition just like a prior series");const int attempts=window->getManager()->getResults()->getResultOfTeam(team)->getJumpsReference().size();window->autoSimulateRound();check(window->getManager()->getResults()->getResultOfTeam(team)->getJumpsReference().size()==attempts,"individual preview cannot advance the team competition");clickControl(main.findChild<QQuickWidget *>("quickApplicationHeader"),"returnToCompetition");QTest::qWait(50);check(!history->paused() && !history->individualRanking(),"return restores live team results from individual preview");
                    window->autoSimulateRound();check(window->getManager()->getResults()->getResultOfTeam(team)->getJumpsReference().size()==8,"all team members continue after one athlete DNS");
                }
                QDialog archive(&main);archive.setObjectName("competitionResultsArchive");archive.setWindowTitle("History fixture");auto *layout=new QVBoxLayout(&archive);auto *archived=new ResultsHistoryWidget(window->getManager()->getCompetitionInfo(),false,&archive);layout->addWidget(archived);archive.show();QTest::qWait(100);
                archived->roundChoice()->setCurrentIndex(1);QTest::qWait(50);check(archived->active() && !archived->paused() && !archived->snapshot.getResultsReference().isEmpty(),"archive uses read-only historical results for all event types");render(&archive,QString("history-archive-%1").arg(scenario),scenario==2);
                archived->roundChoice()->setCurrentIndex(0);QTest::qWait(40);auto *archiveBridge=archive.findChild<QuickBridge *>();auto *node=qobject_cast<QuickNode *>(archiveBridge->find("historyResultsView"));auto *adapter=qobject_cast<QuickTableModel *>(node->values().value("table").value<QObject *>());check(adapter,"archive history has a table adapter");adapter->select(0,scenario==2?5:3,Qt::ShiftModifier);QTest::qWait(40);check(archived->previewWidget()->selectedRound()==0,"Shift-click in archived snapshot selects the requested jump series");archive.reject();
                qInfo()<<"PASS: historical pause, DNS and archive round selection"<<scenario;window->reject();return;
            }
            if(QCoreApplication::arguments().contains("--superko-only")) {
                auto *individual=dynamic_cast<IndividualCompetitionManager *>(window->getManager());check(individual,"SuperKO individual manager");
                for(int round=1;round<=4;++round){
                    if(round==2 || round==3){check(individual->getKOManager()->getLuckyLosersCount()==0,"SuperKO later KO rounds have zero LL quota");for(auto *jumper:individual->getActualRoundJumpersReference())check(!individual->isLuckyLoser(jumper),"previous-series LL must not leak into a fresh zero-quota KO round");}
                    window->autoSimulateRound();bridge->refresh();
                    if(round==1)check(individual->getKOManager()->getLuckyLosersCount()==2 && !individual->getKOManager()->getLuckyLosers().isEmpty(),"SuperKO first round genuinely produces two LL places");
                    if(round==2 || round==3)for(int row=0;row<individual->getResults()->getResultsReference().size();++row)check(!window->findChild<ResultsTableModel *>()->index(row,0).data(CompetitionAdvancement::LuckyLoserRole).toBool(),"SuperKO zero-quota results do not expose LL graphics");
                    if(round<4){bridge->activate(id(bridge,"pushButton_goToNext"));QTest::qWait(100);}
                }
                check(individual->checkCompetitionEnd(),"SuperKO completes all four actual preset rounds");qInfo()<<"PASS: real SuperKO preset, first-round LL and no stale LL in rounds two/three";window->accept();return;
            }
            if(QCoreApplication::arguments().contains("--manual-ko-only")) {
                window->autoSimulateRound();auto *individual=dynamic_cast<IndividualCompetitionManager *>(window->getManager());const auto qualified=individual->getFilteredJumpersForNextRound();
                bool cancelled=false;QTimer cancelTimer;QObject::connect(&cancelTimer,&QTimer::timeout,[&]{for(auto *widget:QApplication::allWidgets())if(widget->objectName()=="manualKoGroupsDialog" && widget->isVisible()){cancelled=true;cancelTimer.stop();qobject_cast<QDialog *>(widget)->reject();return;}});cancelTimer.start(20);bridge->editNextKoGroups();QTest::qWait(120);cancelTimer.stop();check(cancelled && individual->getActualRound()==1,"cancelled group editing leaves the finished series intact");
                bool edited=false;QTimer editTimer;QObject::connect(&editTimer,&QTimer::timeout,[&]{for(auto *widget:QApplication::allWidgets())if(widget->objectName()=="manualKoGroupsDialog" && widget->isVisible()){
                    editTimer.stop();auto *editorBridge=widget->findChild<QuickBridge *>();check(editorBridge,"in-competition group editor has Quick bridge");editorBridge->refresh();auto *editorView=scene(widget);check(item(editorView->rootObject(),"koGroupsWorkspace") && item(editorView->rootObject(),"koDragArea"),"between-series editor reuses searchable drag-and-drop workspace");
                    const auto fields=editorBridge->root()->values();check(fields.value("methodIndex").toInt()==4 && fields.value("athletes").toList().size()==qualified.size(),"manual editor starts randomly and contains only qualified athletes");
                    QMetaObject::invokeMethod(item(editorView->rootObject(),"koGroupingMethod"),"activated",Q_ARG(int,2));QTest::qWait(30);check(editorBridge->root()->values().value("methodIndex").toInt()==3,"between-series editor supports basket draws");
                    edited=true;editorBridge->activate(editorBridge->root()->values().value("submitId").toInt());return;
                }});editTimer.start(20);bridge->editNextKoGroups();QTest::qWait(120);editTimer.stop();check(edited && individual->getActualRound()==2,"confirmed manual groups start the next KO series");
                const auto grouped=KOGroup::getJumpersFromGroups(individual->getRoundsKOGroupsReference()[1]);check(grouped.size()==qualified.size(),"manual groups preserve the entire qualified field");for(auto *jumper:grouped)check(qualified.contains(jumper),"eliminated athletes cannot enter manual next-round groups");qInfo()<<"PASS: shared between-series KO workspace, cancellation, random startup and basket selection";window->reject();return;
            }
            if(QCoreApplication::arguments().contains("--result-scroll-only")) {
                presentation(window)->resize(1024,600);QTest::qWait(50);
                auto *view=scene(window);auto *table=item(view->rootObject(),"raceResults");
                auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const int oldMode=appearance->teamResultsMode();if(scenario==2)appearance->setTeamResultsMode(2);
                for(int i=0;i<12;++i){bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(20);}
                bridge->refresh();QTest::qWait(100);
                auto *rows=item(table,"tableRows-"+table->property("ownerId").toString());check(rows,"result rows exist");
                const auto maximum=rows->property("contentHeight").toDouble()-rows->height();check(maximum>40,QString("real results must scroll: scenario %1, content %2, height %3, count %4, view %5x%6").arg(scenario).arg(rows->property("contentHeight").toDouble()).arg(rows->height()).arg(rows->property("count").toInt()).arg(view->width()).arg(view->height()));
                const double position=qMin(80.,maximum);rows->setProperty("contentY",rows->property("originY").toDouble()+position);
                QMetaObject::invokeMethod(table,"forceActiveFocus");
                bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(120);bridge->refresh();QTest::qWait(30);
                const double after=rows->property("contentY").toDouble()-rows->property("originY").toDouble();
                check(qAbs(after-position)<2,"adding an actual jump preserves a manually scrolled result table even with table focus");
                QMetaObject::invokeMethod(table,"forceActiveFocus");QTest::keyClick(view,Qt::Key_Down);QTest::qWait(50);
                appearance->setTeamResultsMode(oldMode);qInfo()<<"PASS: result scroll survives added jump"<<scenario;window->reject();return;
            }
            if(QCoreApplication::arguments().contains("--race-options-only")) {
                raceOptionsRegressions(main,window);window->reject();return;
            }
            if(bulk) {
                QSignalSpy resets(window->findChild<StartListModel *>(),&QAbstractItemModel::modelReset);
                QSignalSpy rowChanges(window->findChild<StartListModel *>(),&QAbstractItemModel::dataChanged);
                QTimer acceptCount;QObject::connect(&acceptCount,&QTimer::timeout,[&] {for(auto *widget:QApplication::allWidgets())if(auto *input=qobject_cast<QInputDialog *>(widget);input && input->isVisible()){input->setIntValue(37);input->accept();}});acceptCount.start(20);
                QElapsedTimer elapsed;elapsed.start();
                if(bulkAction==1)window->autoSimulateRound();else if(bulkAction==2)window->autoSimulateJumps();else if(bulkAction==3)window->autoSimulateGroup();else window->autoSimulateCompetition();
                const auto duration=elapsed.elapsed();acceptCount.stop();
                checkLatestJumpPresentation(window);
                check(bulkAction || window->getManager()->checkCompetitionEnd(),"bulk simulation completes the real competition");
                check(rowChanges.count()<=2 && resets.count()<=2,"automatic simulation batches start-list presentation updates");
                if(bulkAction==1)check(window->getManager()->checkRoundEnd() && window->getManager()->getActualRound()==1,"automatic round simulation stops at the round boundary");
                if(bulkAction==3)check(dynamic_cast<TeamCompetitionManager *>(window->getManager())->checkGroupEnd(),"automatic team group simulation stops at the group boundary");
                QLabel *athleteLabel=nullptr;for(auto *label:window->findChildren<QLabel *>("label_nameAndSurname")){bool preview=false;for(auto *parent=label->parentWidget();parent;parent=parent->parentWidget())if(qobject_cast<JumperCompetitionResultsWidget *>(parent)){preview=true;break;}if(!preview)athleteLabel=label;}
                if(!athleteLabel || athleteLabel->text()!=window->getManager()->getActualJumper()->getNameAndSurname())qInfo()<<"BULK ATHLETE"<<scenario<<bulkAction<<(athleteLabel?athleteLabel->text():QString())<<window->getManager()->getActualJumper()->getNameAndSurname()<<window->getManager()->getActualStartListIndex();
                check(athleteLabel && athleteLabel->text()==window->getManager()->getActualJumper()->getNameAndSurname(),"batched athlete presentation is synchronized before returning control");
                int jumps=0;for(const auto &result:window->getManager()->getResults()->getResults()) {jumps+=result.getJumps().size();check(result.getCompetition()==window->getManager()->getCompetitionInfo() && std::isfinite(result.getPointsSum()),"bulk results retain competition owners and finite scores");}
                if(weighted) {
                    const int perRound=scenario==2?4:1;
                    for(const auto &result:window->getManager()->getResults()->getResults()) {
                        double expected=0;const auto history=result.getJumps();
                        for(int index=0;index<history.size();++index)expected+=history[index].getPoints()*(history.size()>perRound && index<perRound?(scenario==2?.5:0):1.);
                        check(std::abs(result.getPointsSum()-qRound64(expected*10)/10.)<.001,"real individual, KO and team simulations use reset or fractional previous-round points");
                    }
                    window->cancelActualRound();
                    check(window->getManager()->getCompetitionInfo()->getExceptionalRoundsCount()==1,"cancelled final round is excluded from saved completed-round metadata");
                    for(const auto &result:window->getManager()->getResults()->getResults()) {
                        double expected=0;for(const auto &jump:result.getJumps())expected+=jump.getPoints();
                        check(result.getJumps().size()==perRound && std::abs(result.getPointsSum()-expected)<.001,"cancelling a weighted round restores original completed-round scores and team groups");
                    }
                    qInfo()<<"PASS: weighted competition and cancelled-round restoration"<<scenario;
                }
                qInfo()<<"BENCH bulk"<<scenario<<"jumps"<<jumps<<"ms"<<duration<<"start-list resets"<<resets.count()<<"row changes"<<rowChanges.count();
                check(bulkAction==2 ? jumps==37 && window->getManager()->getActualStartListIndex()==37 : jumps>0,"bulk simulation records the requested jumps and leaves the next athlete ready");window->accept();return;
            }
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
                    {
                        auto *manager=window->getManager();auto *source=manager->getCompetitionInfo();
                        const auto oldTargets=source->getQualifyingCompetitions();const int oldRounds=source->getExceptionalRoundsCount();
                        CompetitionInfo target(source->getHill());CompetitionRules targetRules;targetRules.setRounds({RoundInfo(manager->getStartListStatusesReference().size(),true)});target.setRules(targetRules);target.setAdvancementCompetition(source);source->setQualifyingCompetitions({&target});source->setExceptionalRoundsCount(1);
                        manager->updateCompetitorsAdvanceStatuses();adapter->invalidateText();bridge->refresh();QTest::qWait(80);scene(window)->grabFramebuffer();
                        auto *badge=item(scene(window)->rootObject(),"qualificationBadge-0");
                        check(badge && badge->isVisible() && badge->property("text").toString()=="Q" && adapter->data(adapter->index(0),Qt::UserRole+9).toInt()==-1 && adapter->data(adapter->index(0),Qt::UserRole+10).toInt()==StartListCompetitorStatus::SureAdvanced,"external advancement uses a visible neutral Q instead of a next-round green state");
                        const auto color=badge->property("color").value<QColor>();check(color==QColor("#51657c") || color==QColor("#b0c2d8"),"qualification Q uses neutral text in either theme");
                        render(window,"qualification-markers",true);
                        source->setQualifyingCompetitions(oldTargets);source->setExceptionalRoundsCount(oldRounds);manager->updateCompetitorsAdvanceStatuses();manager->updateLastQualifiedResult();adapter->invalidateText();bridge->refresh();main.resize(1024,640);QTest::qWait(100);
                    }
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
                clickControl(scene(window),"raceJumpOptions");QTest::qWait(120);
                optionsWorkspace=item(scene(window)->quickWindow()->contentItem(),"raceOptionsWorkspace");optionsScroll=item(scene(window)->quickWindow()->contentItem(),"raceOptionsScroll");
                check(optionsScroll->property("contentHeight").toReal()>optionsScroll->height() && optionsScroll->property("interactive").toBool(),"jump details really overflow the options panel");
                for(int section:{1,0,2,0,1}) {
                    optionsWorkspace->setProperty("section",section);QTest::qWait(80);scene(window)->grabFramebuffer();
                    check(section==2 || (optionsScroll->property("contentHeight").toReal()<=optionsScroll->height() && !optionsScroll->property("interactive").toBool() && optionsScroll->property("contentY").toReal()==0),"returning from jump details unloads their height and hides scrolling in fitted options tabs");
                }
                optionsWorkspace->setProperty("section",0);QTest::qWait(100);
                check(!item(scene(window)->quickWindow()->contentItem(),"action-toolButton_autoSimulationActions"),"automatic simulation has direct controls without a parent menu button");
                messages.stop();const int untouchedIndex=window->getManager()->getActualStartListIndex();
                for(int operation=0;operation<3;++operation) {
                    if(operation>0){main.activateWindow();scene(window)->setFocus();clickControl(scene(window),"raceOptions");}
                    bool opened=false;QTimer cancel;
                    QObject::connect(&cancel,&QTimer::timeout,[&] {for(auto *widget:QApplication::allWidgets())if(auto *dialog=qobject_cast<QDialog *>(widget);dialog && dialog->isVisible() && (qobject_cast<QInputDialog *>(dialog)||qobject_cast<QMessageBox *>(dialog))){cancel.stop();opened=true;const auto title=dialog->windowTitle();check(operation==0 ? qobject_cast<QInputDialog *>(dialog)!=nullptr : title.contains(operation==1?QString::fromUtf8("serii"):QString::fromUtf8("konkursu")),"each direct simulation control opens its intended operation");dialog->reject();}});
                    cancel.start(20);clickControl(scene(window),"raceSimulate-"+QString::number(operation));QTest::qWait(80);cancel.stop();check(opened && window->getManager()->getActualStartListIndex()==untouchedIndex,"cancelling a direct simulation operation preserves the competition");
                }
                messages.start(20);
                check(!scene(window)->rootObject()->findChild<QObject *>("raceOptionsPanel")->property("visible").toBool(),"starting a direct simulation operation closes the options drawer");
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
            int jumps=0;bool previousRankTested=false;bool koPreviewTested=false;
            QSet<QString> checkedGates;
            while(!window->getManager()->checkCompetitionEnd() && jumps++<250) {
                auto *next=window->findChild<QPushButton *>("pushButton_goToNext");
                if(scenario==1 && !koPreviewTested && window->getManager()->getActualRound()==1) {
                    auto *model=window->getKOGroupsResultsModel();
                    auto group=model->getGroup()->getJumpers();window->getManager()->getResults()->sortJumpersByResults(group);
                    for(int row=0;row<group.size();++row)if(auto *result=window->getManager()->getResults()->getResultOfIndividualJumper(group[row]);result && !result->getJumpsReference().isEmpty() && window->getManager()->getResults()->getIndexOfJumperResult(group[row])>0) {
                        auto *native=window->findChild<QTableView *>("tableView_KOGroupResults");
                        QMetaObject::invokeMethod(native,"clicked",Q_ARG(QModelIndex,model->index(row,2)));bridge->refresh();
                        check(window->findChild<JumperCompetitionResultsWidget *>()->getJumperResult()->getJumper()==group[row],"clicking a KO result opens the matching athlete instead of treating the group row as an overall row");koPreviewTested=true;break;
                    }
                }
                if(multiKo && window->getManager()->getActualRound()>1 && !window->getManager()->checkRoundEnd()) {
                    bridge->refresh();check(!bridge->context().contains("groupGap") && !bridge->context().contains("liveLeaderGap"),"multi-round KO has only one previous-series comparison");
                    if(window->getManager()->getActualStartListIndex()==0)check(!bridge->context().contains("currentGap"),"first KO jumper of a series has no leader comparison");
                }
                auto *jump=window->findChild<QPushButton *>("pushButton_jump");
                if(jump && jump->isEnabled() && StartListCompetitorStatus::remainingJumpers(window->getManager()->getStartListStatusesReference())==1) {
                    const QString key=QString::number(window->getManager()->getActualRound())+":"+QString::number(window->getManager()->getType()==CompetitionRules::Team?static_cast<TeamCompetitionManager *>(window->getManager())->getActualGroup():0);
                    if(!checkedGates.contains(key)){competitionCoachGateRegressions(window,check);checkedGates.insert(key);}
                }
                if(next && !next->isHidden() && next->isEnabled()) bridge->activate(id(bridge,"pushButton_goToNext"));
                else if(scenario==0 && window->getManager()->getActualRound()>1 && !previousRankTested) {
                    bridge->refresh();const int previous=bridge->context().value("previousPosition").toInt();check(previous>0,"athlete on gate shows place after previous round");
                    check(!bridge->context().contains("currentGap"),"first athlete of the new series has no premature leader comparison");
                    const auto beforeJump=bridge->context().value("currentGap");
                    const double speed=GlobalAppSettings::get()->getLiveCompetitionSpeedMulti();GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(.005);window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(true);
                    const auto advancementBefore=bridge->context().value("biggestAdvancement").toMap();
                    QObject watcher;bool seen=false;
                    QObject::connect(window,&CompetitionManagerWindow::liveStageChanged,&watcher,[&](int stage){if(stage==2){bridge->refresh();check(!bridge->jumpSummary().contains("positionNumber"),"last jump leaves position blank during flight");check(!bridge->jumpSummary().contains("leaderGap"),"current leader gap is not revealed during flight");check(!bridge->jumpSummary().contains("roundPositionNumber"),"round jump rank is not revealed during flight");check(bridge->context().value("currentGap")==beforeJump,"pre-jump gap retains its pre-jump value during live flight");check(bridge->context().value("biggestAdvancement").toMap()==advancementBefore,"biggest advancement stays frozen until live results are revealed");seen=true;}});
                    bridge->activate(id(bridge,"pushButton_jump"));QTest::qWait(300);bridge->refresh();check(seen&&!bridge->jumpSummary().contains("previousRound")&&bridge->jumpSummary().value("positionNumber").toInt()>0,"final result replaces previous place with current rank");
                    window->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(false);GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(speed);previousRankTested=true;
                }
                else { check(jump&&jump->isEnabled(),"jump remains operable");if(scenario==2 && window->getManager()->getActualRound()>1){bridge->refresh();check(bridge->context().value("previousPosition").toInt()>0,"team on gate shows the team place after previous round");}bridge->activate(id(bridge,"pushButton_jump")); }
                QTest::qWait(10);bridge->refresh();checkStartList();
                check(bridge->jumpSummary().value("roundPositionNumber").toInt()>0,"the main jump card shows that jump's rank within its own round");
            }
            check(scenario!=1||koPreviewTested,"KO result preview selection was exercised");
            check(scenario!=0||previousRankTested,"previous-round rank transition was exercised in a real second round");
            check(window->getManager()->checkCompetitionEnd(),"competition finishes");
            check(!window->getManager()->getResults()->getResultsReference().isEmpty(),"competition has actual results");
            if(scenario==2){
                auto *table=item(scene(window)->rootObject(),"raceResults");
                        auto *appearance=scene(window)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const int mode=appearance->teamResultsMode();auto *model=table->property("table").value<QuickTableModel *>();
                        appearance->setTeamResultsMode(1);QTest::qWait(30);const int summaries=model->count();
                        appearance->setTeamResultsMode(0);QTest::qWait(30);const int last=model->count();
                        appearance->setTeamResultsMode(2);QTest::qWait(30);check(summaries<last && last<=model->count(),"team modes expand none, the latest squad, or all jumped members");
                        appearance->setTeamResultsMode(mode);QTest::qWait(30);
                    }
            if(QCoreApplication::arguments().contains("--jump-history-only"))jumpHistoryRegressions(main,window,scenario);
            checkLatestJumpPresentation(window);
            render(window,QString("live-results-%1").arg(scenario),scenario==2);
            bridge->refresh();
            check(bridge->context().value("biggestAdvancement").toMap()==QuickRaceStats::biggestAdvancement(*window->getManager()->getResults(),window->getManager()->getActualRound(),scenario==2?4:1,StartListCompetitorStatus::remainingJumpers(window->getManager()->getStartListStatusesReference())),"competition header exposes the largest revealed advancement");
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
class AuthorLinkRecorder : public QObject {
    Q_OBJECT
public:
    QList<QUrl> urls;
public slots:
    void record(const QUrl &url) {urls.append(url);}
};

static void appearanceRegressions(MainWindow &main) {
    main.show();main.resize(1024,640);auto *appearance=scene(&main)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();
    const bool oldCompact=appearance->compactUi();const qreal oldScale=appearance->textScale(),oldUi=appearance->uiScale();
    appearance->setCompactUi(false);appearance->setUiScale(.7);appearance->setTextScale(1);
    QuickAppearance persisted;check(persisted.uiScale()==.7 && persisted.textScale()==1,"UI scale and text size persist independently");
    auto *navigation=main.findChild<QuickUi *>();navigation->showAppearance();QTest::qWait(120);
    QWidget *popup=nullptr;for(auto *widget:QApplication::allWidgets())if(widget->isVisible() && widget->property("sjQuickUtility").toBool())popup=widget;
    check(popup,"appearance settings open");auto *view=popup->findChild<QQuickWidget *>();
    auto typeSetting=[&](const QString &name,const QString &text){
        auto *spin=item(view->quickWindow()->contentItem(),name);check(spin,"appearance percentage control exists");
        auto *input=spin->property("contentItem").value<QQuickItem *>();check(input,"percentage input exists");
        input->forceActiveFocus();QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,text);
        check(input->property("text").toString()==text,"appearance input preserves the draft while typing");QTest::keyClick(view,Qt::Key_Return);QTest::qWait(100);
    };
    typeSetting("appearanceUiScale","85");check(qFuzzyCompare(appearance->uiScale(),.85),"typing applies UI percentage");
    typeSetting("appearanceUiScale","100");check(qFuzzyCompare(appearance->uiScale(),1.),"UI percentage supports successive edits");
    typeSetting("appearanceTextSize","125");check(qFuzzyCompare(appearance->textScale(),1.25),"typing applies text size independently");
    typeSetting("appearanceTextSize","90");check(qFuzzyCompare(appearance->textScale(),.9),"text percentage supports successive edits");
    QTest::keyClick(view,Qt::Key_Up);QTest::qWait(40);check(qFuzzyCompare(appearance->textScale(),.95),"appearance text size also supports increment keys");
    QTest::keyClick(view,Qt::Key_Down);QTest::qWait(40);check(qFuzzyCompare(appearance->textScale(),.9),"appearance text size also supports decrement keys");
    navigation->closeAppearance();appearance->setUiScale(.7);appearance->setTextScale(1);main.resize(360,480);QTest::qWait(80);navigation->showAppearance();QTest::qWait(120);
    popup=nullptr;for(auto *widget:QApplication::allWidgets())if(widget->isVisible() && widget->property("sjQuickUtility").toBool())popup=widget;
    check(popup && popup->height()<=main.height()-24,"appearance controls fit the smallest application height");navigation->closeAppearance();
    appearance->setCompactUi(oldCompact);appearance->setTextScale(oldScale);appearance->setUiScale(oldUi);main.resize(1024,640);
    qInfo()<<"PASS: independent UI/text scales, repeated keyboard edits, persistence and bounded appearance popup";
}

static void authorRegressions(MainWindow &main) {
    main.show();main.resize(1240,900);QTest::qWait(120);clickControl(scene(&main),"openAuthor");QTest::qWait(150);
    auto *dialog=main.findChild<AuthorWindow *>();auto *navigation=main.findChild<QuickUi *>();
    check(dialog && dialog->isVisible() && dialog->property("sjQuickPage").toBool() && !QApplication::activeModalWidget(),"author opens from home as a regular navigation page");
    auto *bridge=dialog->findChild<QuickBridge *>();check(navigation->current()==bridge,"author page uses shared navigation");
    const auto projects=bridge->context().value("projects").toList();check(projects.size()==2 && projects.first().toMap().value("name")=="Sj.Sim Predazzo Edition" && projects.last().toMap().value("name")=="Ski Jump Draft" && projects.last().toMap().value("description").toString().contains("2025"),"author projects include Ski Jump Draft and its 2025 beta");
    check(!bridge->context().value("work").toString().contains("SJ Manager") && !bridge->context().value("work").toString().contains("SJ Draft") && bridge->context().value("availability").toString().contains(QString::fromUtf8("płatnym zadaniu")),"author retains the work invitation and removes the extra project sentence");
    render(dialog,"author",true);
    auto *view=scene(dialog);main.resize(1240,900);QTest::qWait(180);view->grabFramebuffer();
    auto *photo=item(view->rootObject(),"authorPortrait");check(photo && photo->property("status").toInt()==1,"packaged author portrait loads without external files");
    check(!item(view->rootObject(),"authorCredits"),"acknowledgements stay discreetly collapsed initially");
    AuthorLinkRecorder receiver;QDesktopServices::setUrlHandler("https",&receiver,"record");
    for(const auto &name:{"authorProject-0","authorProject-1","authorGitHub","authorLinkedIn"})clickControl(view,name);
    const QList<QUrl> expected{QUrl("https://github.com/KonradKrol/sjsim-2026"),QUrl("https://github.com/Ski-Jump-Draft"),QUrl("https://github.com/KonradKrol"),QUrl("https://www.linkedin.com/in/konrad-kr%C3%B3l-bb51a72a7/")};
    check(receiver.urls==expected,"all author links open their exact intended destinations");QDesktopServices::unsetUrlHandler("https");
    bool seen=false;QTimer closeCredits;
    QObject::connect(&closeCredits,&QTimer::timeout,[&] {for(auto *widget:QApplication::allWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible() && box->parentWidget()==dialog){closeCredits.stop();seen=true;check(box->text().contains("Jonek") && box->text().contains("flaticon"),"acknowledgements dialog preserves existing contributors");check(scene(box) && !box->property("sjQuickPage").toBool(),"acknowledgements opens as a Quick dialog rather than expanding the author page");render(box,"author-credits");box->accept();}});closeCredits.start(20);clickControl(view,"authorCreditsButton");QTest::qWait(160);closeCredits.stop();check(seen && navigation->current()==bridge,"closing acknowledgements keeps the author page in place");
    for(const auto &name:{"socialIcon-github","socialIcon-linkedin"})check(item(view->rootObject(),name)->property("status").toInt()==1,"social icons load from packaged vectors");
    navigation->back();QTest::qWait(150);check(!navigation->canGoBack(),"author returns to the home screen with shared Back");
    qInfo()<<"PASS: author navigation, responsive portrait, project/social links, icons and acknowledgements dialog";
}

class TaskPresentationProbe : public QObject {
public:
    TaskPresentationProbe() { qApp->installEventFilter(this); }
    ~TaskPresentationProbe() override { qApp->removeEventFilter(this); }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        auto *dialog=qobject_cast<QDialog *>(object);
        if(event->type()==QEvent::Show && dialog && !qobject_cast<QMessageBox *>(dialog)
            && !qobject_cast<QInputDialog *>(dialog) && !qobject_cast<QProgressDialog *>(dialog)
            && !qobject_cast<QFileDialog *>(dialog) && !dialog->property("sjQuickUtility").toBool()) {
            check(dialog->testAttribute(Qt::WA_DontShowOnScreen) && dialog->testAttribute(Qt::WA_ShowWithoutActivating),"task controller is suppressed before its first Show reaches native window mapping");
        }
        return false;
    }
};
class ReentrantDialogShowProbe : public QObject {
public:
    explicit ReentrantDialogShowProbe(QDialog *dialog) : m_dialog(dialog) { qApp->installEventFilter(this); }
    ~ReentrantDialogShowProbe() override { qApp->removeEventFilter(this); }
    bool triggered = false;
protected:
    bool eventFilter(QObject *,QEvent *event) override {
        if(!triggered && event->type()==QEvent::ChildAdded
            && m_dialog->property("sjQuickInstalled").toBool()) {
            auto *bridge=m_dialog->findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly);
            if(bridge && !bridge->view()) {
                triggered=true;
                QEvent nestedShow(QEvent::Show);
                QCoreApplication::sendEvent(m_dialog,&nestedShow);
            }
        }
        return false;
    }
private:
    QDialog *m_dialog;
};
static void navigationRegressions(MainWindow &main) {
    TaskPresentationProbe presentationProbe;
    main.show();QTest::qWait(100);
    auto *navigation=main.findChild<QuickUi *>();check(navigation && !navigation->canGoBack(),"navigation starts at home");
    QDialog first(&main);first.setWindowTitle("First page");auto *layout=new QVBoxLayout(&first);auto *name=new QLineEdit("Retained",&first);layout->addWidget(name);
    ReentrantDialogShowProbe reentrantShow(&first);
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
    check(reentrantShow.triggered,"navigation handles a reentrant Show while its Quick view is being constructed");
    check(!navigation->canGoBack() && navigation->current()==main.findChild<QuickBridge *>(),"navigation returns home after nested pages finish");
    qInfo()<<"PASS: one application window, nested navigation, state retention and shared appearance";
}
static void homeLaunchRegressions(MainWindow &main) {
    TaskPresentationProbe presentationProbe;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    check(QQuickStyle::name()==QStringLiteral("Basic"),"custom controls use a customizable style on Qt 6");
#endif
    main.show();QTest::qWait(100);
    auto *navigation=main.findChild<QuickUi *>();
    auto *home=main.findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly);
    check(navigation && home && !navigation->canGoBack(),"home launch starts at the main menu");
    QTimer confirmations;
    QObject::connect(&confirmations,&QTimer::timeout,[]{
        for(auto *widget:QApplication::topLevelWidgets())
            if(auto *message=qobject_cast<QMessageBox *>(widget);message && message->isVisible()) {
                check(message->standardButtons().testFlag(QMessageBox::No),"home smoke only dismisses a discard-changes confirmation");
                message->done(QMessageBox::No);
            }
    });
    confirmations.start(20);
    const QList<QPair<QString,QString>> destinations{
        {"seasons","SimulationSavesWindow"},{"competition","CompetitionConfigWindow"},
        {"jumps","SingleJumpsConfigWindow"},{"database","DatabaseEditorWindow"},
        {"settings","AppSettingsWindow"},{"author","AuthorWindow"},
        {"help","HelpWindow"},{"credits","ThanksWindow"}
    };
    for(const auto &destination:destinations) {
        bool visited=false;
        QTimer inspect,deadline;deadline.setSingleShot(true);
        QObject::connect(&deadline,&QTimer::timeout,[&]{check(false,"home launch timed out: "+destination.first);});
        QObject::connect(&inspect,&QTimer::timeout,[&]{
            if(visited || !navigation->canGoBack())return;
            auto *page=navigation->current();
            check(page && page->profile()==destination.second,"home launches the expected controller: "+destination.first);
            check(page->view() && page->view()->rootObject() && page->view()->status()==QQuickWidget::Ready,"home page has a fully constructed Quick view: "+destination.first);
#ifdef Q_OS_WIN
            auto *controller=qobject_cast<QWidget *>(page->parent());
            check(controller && (!controller->internalWinId() || !IsWindowVisible(reinterpret_cast<HWND>(controller->internalWinId()))),"Windows never maps the task controller as a separate visible window: "+destination.first);
#endif
            check(page->view()->window()==&main,"task content is rendered inside the original main window");
            main.grab().save(output+"/home-"+destination.first+".png");
            visited=true;inspect.stop();navigation->back();
        });
        inspect.start(20);deadline.start(15000);home->launch(destination.first);
        while(!visited || navigation->canGoBack())QTest::qWait(20);
        deadline.stop();
        check(navigation->current()==home,"closing a home destination returns to the original menu: "+destination.first);
        qInfo()<<"PASS: home destination"<<destination.first;
        QTest::qWait(30);
    }
    qInfo()<<"PASS: all eight local home destinations open and return safely";
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
static QPoint controlPoint(QQuickWidget *view,const QString &name) {
    // Synthetic QWidget mouse events bypass native mouse activation and focus.
    QApplication::setActiveWindow(view->window());view->setFocus(Qt::MouseFocusReason);
    view->grabFramebuffer();
    auto *control=item(view->quickWindow()->contentItem(),name);
    if(!control || !control->isVisible() || !control->isEnabled()){applicationWindow->grab().save("/tmp/sj-click-failure.png");qInfo()<<"Control failure"<<name<<control<<(control?control->isVisible():false)<<(control?control->isEnabled():false);}
    check(control && control->isVisible() && control->isEnabled(),"clickable control: "+name);
    control->forceActiveFocus(Qt::MouseFocusReason);
    // Windows can constrain requested test sizes to the monitor's work area.
    // Scroll fields into view with real wheel events before clicking them.
    for(auto *ancestor=control->parentItem();ancestor;ancestor=ancestor->parentItem()) {
        if(!ancestor->inherits("QQuickFlickable"))continue;
        const auto viewport=ancestor->mapRectToScene(ancestor->boundingRect()).intersected(QRectF(view->rect()));
        for(int attempt=0;attempt<24 && !viewport.isEmpty();++attempt) {
            const auto center=control->mapToScene(QPointF(control->width()/2,control->height()/2));
            if(center.y()>=viewport.top()+4 && center.y()<=viewport.bottom()-4)break;
            const QPointF position(viewport.left()+3,viewport.center().y());
            QWheelEvent wheel(position,view->mapToGlobal(position.toPoint()),QPoint(),QPoint(0,center.y()>viewport.bottom()?-120:120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
            QApplication::sendEvent(view,&wheel);QTest::qWait(80);
            for(int settle=0;settle<100 && ancestor->property("moving").toBool();++settle)QTest::qWait(20);
            // A nested control can consume wheel events. Reveal it as an
            // accessibility scroll-to action, then exercise its real pointer hit.
            if(attempt==2){
                QMetaObject::invokeMethod(ancestor,"cancelFlick");
                const auto current=control->mapToScene(QPointF(control->width()/2,control->height()/2));
                const double offset=ancestor->property("contentY").toDouble()+current.y()-viewport.center().y();
                ancestor->setProperty("contentY",qBound(0.,offset,qMax(0.,ancestor->property("contentHeight").toDouble()-ancestor->height())));
                QTest::qWait(100);
            }
        }
    }
    view->grabFramebuffer();
    const auto point=control->mapToItem(view->rootObject(),QPointF(control->width()/2,control->height()/2)).toPoint();
    if(!view->rect().contains(point)) {
        applicationWindow->grab().save(output+"/click-outside-viewport.png");
        qWarning()<<"Control outside viewport"<<name<<point<<view->rect();
        for(auto *ancestor=control->parentItem();ancestor;ancestor=ancestor->parentItem())if(ancestor->inherits("QQuickFlickable"))
            qCritical()<<"FAIL: scroll diagnostic"<<ancestor->objectName()<<ancestor->height()<<ancestor->property("contentHeight")<<ancestor->property("contentY")<<ancestor->mapToScene(QPointF());
    }
    check(view->rect().contains(point),"control inside viewport: "+name);
    return point;
}
static void clickControl(QQuickWidget *view,const QString &name) {
    const auto point=controlPoint(view,name);
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
static void nextSeasonTransactionRegression(MainWindow &main) {
    SimulationSave save;save.setName("Next season transaction");
    auto *jumper=new Jumper("Original","Athlete","POL");auto *hill=new Hill(GlobalDatabase::get()->getEditableGlobalHills().first());hill->setName("Original hill");save.setJumpers({jumper});save.setHills({hill});
    CompetitionRules rule;rule.setName("Original rules");save.setCompetitionRules({rule});
    auto *calendar=new SeasonCalendar("Completed calendar");Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    SimulationSaveManagerWindow manager(&save,&main);manager.show();QTest::qWait(80);
    const auto jumperId=jumper->getID(),hillId=hill->getID(),ruleId=rule.getID();
    for(bool accepted:{false,true}) {
        bool opened=false;QTimer watcher;
        QObject::connect(&watcher,&QTimer::timeout,[&] {
            for(auto *widget:QApplication::allWidgets())if(auto *draft=qobject_cast<NewSeasonConfiguratorWindow *>(widget);draft && draft->isVisible() && draft->getNextSeason()) {
                watcher.stop();opened=true;
                draft->getJumpersReference().first()->setName("Edited athlete");draft->getHillsReference().first()->setName("Edited hill");draft->getCompetitionsRulesReference().first().setName("Edited rules");
                if(accepted) {
                    auto *jumperEditor=draft->findChild<JumperEditorWidget *>();auto *hillEditor=draft->findChild<HillEditorWidget *>();auto *rulesEditor=draft->findChild<CompetitionRulesEditorWidget *>();
                    check(jumperEditor->getJumper()==draft->getJumpersReference().first() && hillEditor->getHill()==draft->getHillsReference().first() && rulesEditor->getCompetitionRules()==&draft->getCompetitionsRulesReference().first(),"next-season editors bind to the save draft rather than global defaults");
                    jumperEditor->findChild<QLineEdit *>("lineEdit_name")->setText("Accepted athlete");QMetaObject::invokeMethod(jumperEditor,"submitted");
                    hillEditor->findChild<QLineEdit *>("lineEdit_name")->setText("Accepted hill");QMetaObject::invokeMethod(hillEditor,"submitted");
                    rulesEditor->findChild<QLineEdit *>("lineEdit_name")->setText("Accepted rules");QMetaObject::invokeMethod(rulesEditor,"submitted");
                    draft->getJumpersReference().append(new Jumper("Added","Athlete","NOR"));
                    auto hills=draft->getHillsReference();auto *addedHill=new Hill(*hill);addedHill->reassign();hills.append(addedHill);draft->setHills(hills);
                    auto additionalRule=rule;additionalRule.reassign();additionalRule.setName("Added rules");draft->getCompetitionsRulesReference().append(additionalRule);
                    draft->accept();
                } else draft->reject();
                break;
            }
        });watcher.start(20);manager.configNextSeason();watcher.stop();check(opened,"next-season configuration opens");
        if(!accepted)check(jumper->getName()=="Original" && hill->getName()=="Original hill" && save.getCompetitionRulesReference().first().getName()=="Original rules" && save.getSeasonsReference().size()==1,"cancelling next season discards all draft edits without changing the live save");
    }
    check(save.getJumpersReference().first()==jumper && save.getHillsReference().first()==hill && jumper->getID()==jumperId && hill->getID()==hillId,"accepting a new season retains existing object identities and historical references");
    check(jumper->getName()=="Accepted athlete" && hill->getName()=="Accepted hill" && save.getCompetitionRulesReference().first().getName()=="Accepted rules" && save.getCompetitionRulesReference().first().getID()==ruleId,"accepted next-season edits include jumpers, hills and rules");
    check(save.getJumpersReference().size()==2 && save.getHillsReference().size()==2 && save.getCompetitionRulesReference().size()==2 && save.getSeasonsReference().size()==2 && save.getActualSeason()->getSeasonNumber()==2027,"accepting new season keeps additions and advances the season once");
    check(save.getJumperFormInstability(save.getJumpersReference().last()),"added athletes have form instability settings");
    manager.reject();qInfo()<<"PASS: next-season drafts cancel cleanly and accept edits, additions and existing identities";
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
    const auto popupBounds=popup->property("contentItem").value<QQuickItem *>()->mapRectToItem(view->rootObject(),popup->property("contentItem").value<QQuickItem *>()->boundingRect());
    check(popupBounds.top()>=0 && popupBounds.bottom()<=view->height() && popupBounds.height()>=search->height(),"empty hill search remains within the viewport with its search field visible");
    QTest::keyClick(view,Qt::Key_Return);QTest::qWait(30);check(control->property("currentIndex").toInt()==previous,"empty search does not change the selected hill");
    QTest::keyClick(view,Qt::Key_Escape);QTest::qWait(80);check(!popup->property("visible").toBool() && control->property("currentIndex").toInt()==previous,"cancelling hill search keeps the original selection");
    clickControl(view,name);search->setProperty("text",labels[target].toUpper());QTest::qWait(60);
    check(choices->property("count").toInt()==1,"hill search is case insensitive and filters the actual options");
    QTest::keyClick(view,Qt::Key_Down);QTest::keyClick(view,Qt::Key_Up);QTest::keyClick(view,Qt::Key_Return);QTest::qWait(100);
    check(!popup->property("visible").toBool() && control->property("currentIndex").toInt()==target,"filtered hill selection maps back to the original source index");
    if(name=="calendar-field-hill") {
        auto *fields=item(view->rootObject(),"calendarInspectorFields");
        check(fields && fields->height()>60,"calendar inspector keeps usable field height after a hill selection");
    }
    clickControl(view,name);
    check(search->property("text").toString().isEmpty() && choices->property("count").toInt()==labels.size(),"reopening hill search restores the full list");QTest::keyClick(view,Qt::Key_Escape);QTest::qWait(60);
}
static void seasonWorkflowRegressions(MainWindow &main) {
    main.show();main.resize(1200,800);
    nextSeasonTransactionRegression(main);
    {
        CompetitionConfigWindow preparation(CompetitionConfigWindow::SingleCompetition);
        auto *hill=preparation.findChild<QComboBox *>("comboBox_existingHill");
        preparation.show();QTest::qWait(150);
        auto *preparationBridge=preparation.findChild<QuickBridge *>();
        auto *hillNode=qobject_cast<QuickNode *>(preparationBridge->find("comboBox_existingHill"));
        hillSearchRegression(scene(&preparation),"choice-comboBox_existingHill",hillNode->values().value("options").toStringList());
        main.resize(360,480);QTest::qWait(80);hillSearchRegression(scene(&preparation),"choice-comboBox_existingHill",hillNode->values().value("options").toStringList());main.resize(1200,800);QTest::qWait(60);
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
    check(judges,"custom auxiliary rules expose full editor");judges->setChecked(!first->getRules().getHasJudgesPoints());backend->select(second->getIDStr());backend->setRulesTarget(0);const bool secondJudges=second->getRules().getHasJudgesPoints();judges->setChecked(!secondJudges);QTest::qWait(80);check(second->getRules().getHasJudgesPoints()!=secondJudges,"a retired rule-save callback cannot cancel a new selection edit");backend->undo();backend->select(first->getIDStr());backend->setRulesTarget(1);
    check(first->getTrainings().first()->getRules().getHasJudgesPoints()!=first->getRules().getHasJudgesPoints(),"training rules save automatically before changing the selected competition");backend->undo();backend->setRulesTarget(0);
    const int oldRounds=first->getRules().getRounds().size();custom->findChild<QSpinBox *>("spinBox_roundsCount")->setValue(oldRounds+1);QTest::qWait(100);check(first->getRules().getRounds().size()==oldRounds+1,"new rounds save without Apply");auto *roundEditor=custom->findChildren<RoundInfoEditorWidget *>().last();auto *roundCount=roundEditor->findChild<QSpinBox *>("spinBox_count");const int originalCount=roundCount->value();roundCount->setValue(originalCount+1);QTest::qWait(100);check(first->getRules().getRounds().last().getCount()==originalCount+1,"dynamically created round inputs also save automatically");backend->undo();backend->undo();check(first->getRules().getRounds().size()==oldRounds,"automatic rule edits retain normal undo without duplicate saves");
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
#include "roster-drag-regressions.h"

static void koGroupingRegressions(MainWindow &main) {
    main.show();main.resize(1024,720);
    QVector<Jumper *> athletes;for(auto &jumper:GlobalDatabase::get()->getEditableGlobalJumpers())athletes.append(&jumper);
    athletes.resize(qMin(QCoreApplication::arguments().contains("--ko-stress")?145:31,athletes.size()));check(athletes.size()>10,"KO grouping fixture");
    CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition);config.setCompetitionJumpers(athletes);
    CompetitionRules rules=GlobalDatabase::get()->getEditableCompetitionRules().first();RoundInfo first(athletes.size(),true);first.setKO(true);first.setCountInKOGroup(2);rules.setCompetitionType(CompetitionRules::Individual);rules.setRounds({first,RoundInfo(16,true)});
    config.getCompetitionRulesEditor()->setCompetitionRules(&rules);config.getCompetitionRulesEditor()->fillCompetitionRulesInputs();config.show();QTest::qWait(100);
    auto *choice=config.findChild<QComboBox *>("comboBox_groupsSelectionType");auto *groups=config.findChild<KOGroupsListView *>();check(choice && groups,"real pre-competition KO grouping controls exist");
    for(int iteration=0;iteration<32;++iteration){
        const int mode=iteration%4+1;choice->setCurrentIndex(mode);QMetaObject::invokeMethod(choice,"activated",Q_ARG(int,mode));QTest::qWait(5);
        const auto assigned=config.getCompetitionGroups();QSet<Jumper *> seen;int count=0;
        for(const auto &group:assigned)for(auto *jumper:group.getJumpers()){check(jumper && !seen.contains(jumper),"regrouping never duplicates or loses athlete identity");seen.insert(jumper);++count;}
        check(count==athletes.size(),"all grouping methods preserve odd-sized start lists");
        check(groups->findChildren<KOGroupInfoWidget *>().size()==assigned.size(),"old KO cards are fully removed on every regrouping");
    }
    auto *bridge=config.findChild<QuickBridge *>();bridge->refresh();
    const int tab=bridge->root()->values().value("options").toStringList().indexOf(QString::fromUtf8("Grupy KO"));check(tab>=0,"KO workspace is a clearly named preparation section");bridge->edit(bridge->root()->values().value("id").toInt(),tab);QTest::qWait(100);
    auto *view=scene(&config);check(item(view->rootObject(),"koGroupsWorkspace"),"KO uses its own single workspace rather than nested legacy cards");
    auto *method=item(view->rootObject(),"koGroupingMethod");check(method,"grouping method is directly available");QMetaObject::invokeMethod(method,"activated",Q_ARG(int,2));QTest::qWait(100);check(choice->currentIndex()==3,"QML basket choice drives actual KO grouping");
    auto *koNode=bridge->root()->children().first().value<QuickNode *>();
    int idleUpdates=0;QObject::connect(koNode,&QuickNode::valuesChanged,&config,[&idleUpdates](){++idleUpdates;});
    auto *methodPopup=method->property("popup").value<QObject *>();check(methodPopup,"KO choice has a popup");
    QMetaObject::invokeMethod(methodPopup,"open");QTest::qWait(300);
    QElapsedTimer refreshTiming;refreshTiming.start();
    for(int refresh=0;refresh<20;++refresh)bridge->refresh();
    qInfo()<<"KO idle refresh, 20 iterations, ms:"<<refreshTiming.elapsed()<<"QML objects:"<<view->rootObject()->findChildren<QObject *>().size();
    check(idleUpdates==0,"idle bridge refresh preserves dedicated KO node instead of rebuilding all groups");
    check(methodPopup->property("visible").toBool(),"KO method popup stays open across periodic refreshes");
    QMetaObject::invokeMethod(methodPopup,"close");
    QObject::disconnect(koNode,nullptr,&config,nullptr);
    auto before=config.getCompetitionGroups();check(before.size()>1,"KO assignment fixture has several groups");
    const auto firstMember=before[0].getJumpers().first(),second=before[1].getJumpers().first();
    auto *member=item(view->rootObject(),"koMember-"+before[0].getIDStr()+"-0");check(member,"group members are directly editable");
    QMetaObject::invokeMethod(member,"activated",Q_ARG(int,athletes.indexOf(second)));QTest::qWait(100);
    const auto after=config.getCompetitionGroups();check(after[0].getJumpers().first()==second && after[1].getJumpers().first()==firstMember,"inline member choice swaps both occupied slots without a dialog or duplication");
    member=item(view->rootObject(),"koMember-"+before[0].getIDStr()+"-0");
    auto at=member->mapToItem(view->rootObject(),member->boundingRect().center()).toPoint();
    QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,at);QTest::qWait(80);
    auto *picker=view->rootObject()->findChild<QObject *>("koAthletePicker");check(picker && picker->property("visible").toBool(),"clicking a KO member opens searchable athlete picker");
    auto *search=item(view->quickWindow()->contentItem(),"koAthleteSearch");check(search,"KO athlete search field exists");
    search->setProperty("text",QStringLiteral("zzzz-no-athlete"));QTest::qWait(30);
    check(picker->property("matches").toList().isEmpty(),"unmatched KO athlete query produces an empty result list");
    search->setProperty("text",firstMember->getNameAndSurname());QTest::qWait(300);
    check(search->property("text").toString()==firstMember->getNameAndSurname() && picker->property("visible").toBool(),"KO search and popup survive periodic refreshes");
    const auto matches=picker->property("matches").toList();check(matches.contains(athletes.indexOf(firstMember)),"athlete search retains original pool indices");
    QTest::keyClick(view,Qt::Key_Return);QTest::qWait(100);
    check(!picker->property("visible").toBool() && config.getCompetitionGroups()[0].getJumpers().first()==firstMember,"keyboard search selection swaps the chosen athlete into the group");
    auto *from=item(view->rootObject(),"koMember-"+before[0].getIDStr()+"-0"),*to=item(view->rootObject(),"koMember-"+before[1].getIDStr()+"-0");
    check(from && to,"drag fixture has two visible group slots");
    const auto begin=from->mapToItem(view->rootObject(),QPointF(12,from->height()/2)).toPoint(),end=to->mapToItem(view->rootObject(),to->boundingRect().center()).toPoint();
    QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,begin);
    for(int step=1;step<=10;++step){const QPoint point=begin+(end-begin)*step/10;QMouseEvent move(QEvent::MouseMove,point,view->mapToGlobal(point),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(15);}
    auto *gesture=item(view->rootObject(),"koDragArea");check(gesture && gesture->property("dragging").toBool(),"KO handle starts a drag");
    QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,end);QTest::qWait(100);
    check(config.getCompetitionGroups()[0].getJumpers().first()==second && config.getCompetitionGroups()[1].getJumpers().first()==firstMember,"real drag swaps occupied KO slots without shifting other athletes");
    auto *scrollingGrid=item(view->rootObject(),"koGroupsGrid");
    auto *handle=item(view->rootObject(),"koMember-"+before[0].getIDStr()+"-0");
    const auto scrollStart=handle->mapToItem(view->rootObject(),QPointF(12,handle->height()/2)).toPoint(),scrollEnd=scrollingGrid->mapToItem(view->rootObject(),QPointF(20,scrollingGrid->height()-4)).toPoint();
    const auto unchanged=config.getCompetitionGroups()[0].getJumpers().first();
    QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,scrollStart);
    QMouseEvent scrollMove(QEvent::MouseMove,scrollEnd,view->mapToGlobal(scrollEnd),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&scrollMove);QTest::qWait(300);
    check(scrollingGrid->property("contentY").toDouble()>0 && gesture->property("dragging").toBool(),"KO drag autoscroll retains the dragged athlete when its original row leaves the viewport");
    QTest::keyClick(view,Qt::Key_Escape);QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,scrollEnd);QTest::qWait(30);
    check(config.getCompetitionGroups()[0].getJumpers().first()==unchanged,"Escape cancels KO drag without changing group membership");
    scrollingGrid->setProperty("contentY",0);
    bridge->assignKoMember(bridge->root()->children().first().value<QuickNode *>()->values().value("id").toInt(),before[0].getIDStr(),-1,QuickRoster::key(firstMember));
    render(&config,"ko-groups-workspace",true);
    auto *grid=item(view->rootObject(),"koGroupsGrid");check(grid && grid->height()>100,"KO groups use the remaining viewport on a small screen");
    config.reject();KOGroupsListView empty;empty.fillListLayout();
    qInfo()<<"PASS: repeated KO method changes, basket draws, odd counts and group-card lifetime";
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
    rosterLongDragRegression(config,roster,pool);
    render(&config,"roster-workspace",true);config.reject();
    JumpersListEditorWindow saved;saved.setAllJumpers(pool.mid(0,12));saved.setSelectedJumpers(pool.mid(0,3));saved.updateUnselectedJumpers();saved.show();QTest::qWait(120);
    auto *savedRoster=saved.findChild<QuickBridge *>()->findChild<QuickRoster *>();check(savedRoster && savedRoster->count()==3,"saved lists use the same roster editor");savedRoster->toggle(QuickRoster::key(pool[5]));check(saved.getSelectedJumpers().contains(pool[5]) && !saved.getUnselectedJumpers().contains(pool[5]),"saved-list membership and its complement stay consistent");savedRoster->undo();check(saved.getSelectedJumpers()==pool.mid(0,3),"saved list undo restores its draft");render(&saved,"saved-roster",true);saved.reject();
    Team team;team.setCountryCode("POL");team.setJumpers(pool.mid(0,6));QDialog dialog;auto *layout=new QVBoxLayout(&dialog);auto *editor=new TeamEditorWidget(&team,4,&dialog);layout->addWidget(editor);dialog.show();QTest::qWait(120);
    auto *teamRoster=dialog.findChild<QuickBridge *>()->findChild<QuickRoster *>();check(teamRoster && !teamRoster->membership() && teamRoster->activeLimit()==4,"team lineup distinguishes starters from reserves");teamRoster->mark(QuickRoster::key(pool[5]));teamRoster->moveMarked(1);check(editor->constructTeamFromWidgetInput().getJumpers().first()==pool[5],"reserve can move directly into starting squad");teamRoster->undo();check(editor->getJumpers()==pool.mid(0,6),"team lineup undo preserves identities");auto *teamView=scene(&dialog);auto *move=item(teamView->rootObject(),"rosterMove");check(move && !move->isVisible(),"team lineup has no move-to-position button");
    clickControl(teamView,"roster-grip-"+QuickRoster::key(pool[1]));check(!item(teamView->rootObject(),"rosterMoveConfirm")->isVisible(),"clicking a team grip does not open the removed move form");teamRoster->moveBefore(QuickRoster::key(pool[5]),QuickRoster::key(pool[1]),true);
    auto expected=pool.mid(0,6);qSwap(expected[1],expected[5]);check(editor->getJumpers()==expected,"team drag swaps the target and reserve without shifting other slots");teamRoster->undo();
    render(&dialog,"team-roster",true);dialog.reject();
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

static void teamSetupCompactRegression(MainWindow &main) {
    main.show();main.resize(1024,720);CompetitionConfigWindow config(CompetitionConfigWindow::SingleCompetition);CompetitionRules rules=GlobalDatabase::get()->getEditableCompetitionRules().first();rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(4);config.getCompetitionRulesEditor()->setCompetitionRules(&rules);config.getCompetitionRulesEditor()->fillCompetitionRulesInputs();
    auto *native=config.getTeamsSquadsModel();native->setupTreeItems();config.show();QTest::qWait(120);auto *bridge=config.findChild<QuickBridge *>();auto *entry=qobject_cast<QuickNode *>(bridge->find("treeView"));check(entry && entry->values().value("teamSetup").toBool(),"team setup uses a dedicated compact list policy");
    auto *adapter=qobject_cast<QuickTableModel *>(entry->values().value("table").value<QObject *>());check(adapter && adapter->count()==native->rowCount() && adapter->count()>1,"team setup shows one row per team instead of every member as a separate row");
    auto *view=scene(&config);auto *table=item(view->rootObject(),"dataTable-"+entry->values().value("id").toString());check(table && !table->property("sortable").toBool() && !table->property("searchEnabled").toBool() && !table->property("searchOpen").toBool(),"team table has no sortable headings or search controls");
    const auto cells=adapter->data(adapter->index(0),Qt::UserRole+1).toList();check(cells.size()==2 && !cells[1].toMap().value("text").toString().isEmpty(),"compact team row retains a readable squad summary");
    check(cells[1].toMap().value("members").toStringList().size()==native->rowCount(native->index(0,0)),"team summary exposes separately readable numbered squad slots");
    const auto name=native->index(0,1,native->index(0,0)).data().toString();adapter->setFilter(name);check(adapter->count()>0 && adapter->count()<=native->rowCount(),"internal team filtering still understands squad members");adapter->setFilter("");
    for(const auto &cell:cells)check(!cell.toMap().value("editable").toBool(),"squad cannot be edited inline in the team list");
    adapter->select(0,0,0);QTest::qWait(30);bool opened=false;int ticks=0;QTimer watcher;QObject::connect(&watcher,&QTimer::timeout,[&]{check(++ticks<250,"team editor opens without getting stuck");for(auto *widget:QApplication::allWidgets())if(auto *dialog=qobject_cast<QDialog *>(widget);dialog && dialog!=&config && dialog->isVisible() && dialog->findChild<TeamEditorWidget *>()){if(!scene(dialog) || scene(dialog)->status()!=QQuickWidget::Ready)continue;watcher.stop();opened=true;dialog->reject();return;}});watcher.start(20);adapter->openCurrent();QTest::qWait(120);watcher.stop();check(opened,"opening a team enters the dedicated squad editor");
    render(&config,"team-setup-compact",true);config.reject();qInfo()<<"PASS: readable team setup, squad summaries, non-sortable headers and no search controls";
}

static void historyPerformanceRegression(MainWindow &main) {
    main.show();main.resize(1440,900);Hill hill;hill.setName("History benchmark");hill.setKPoint(100);hill.setHSPoint(120);CompetitionInfo info(&hill);CompetitionRules rules;rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(4);rules.setRounds({RoundInfo(40,true),RoundInfo(20,true),RoundInfo(10,true)});info.setRules(rules);
    QVector<Jumper> athletes;athletes.reserve(160);QVector<Team> teams;teams.reserve(40);
    for(int i=0;i<40;++i){Team team("POL");QVector<Jumper *> members;for(int slot=0;slot<4;++slot){athletes.append(Jumper("History",QString::number(i*4+slot),"POL"));members.append(&athletes.last());}team.setJumpers(members);teams.append(team);}info.setTeams(teams);
    for(auto &team:info.getTeamsReference())for(int round=0;round<3;++round)for(auto *jumper:team.getJumpersReference()){JumpData jump(jumper,&hill);jump.setCompetition(&info);jump.setDistance(110);jump.setPoints(100+round);info.getResultsReference().addJump(&team,jump);}
    for(auto &result:info.getResultsReference().getResultsReference()){result.updatePointsSum(3);result.updateTeamJumpersResults();}info.getResultsReference().sortInDescendingOrder();
    QDialog archive(&main);archive.setObjectName("competitionResultsArchive");auto *layout=new QVBoxLayout(&archive);auto *history=new ResultsHistoryWidget(&info,false,&archive);layout->addWidget(history);archive.show();QTest::qWait(120);auto *bridge=archive.findChild<QuickBridge *>();auto *node=qobject_cast<QuickNode *>(bridge->find("historyResultsView"));auto *adapter=qobject_cast<QuickTableModel *>(node->values().value("table").value<QObject *>());QSignalSpy resets(adapter,&QAbstractItemModel::modelReset);
    for(int step=0;step<2;++step){resets.clear();QElapsedTimer timer;timer.start();if(step==0)history->roundChoice()->setCurrentIndex(1);else history->modeChoice()->setCurrentIndex(1);bridge->refresh();QTest::qWait(40);qInfo()<<"BENCH history 40 teams/160 athletes, step"<<step<<"ms"<<timer.elapsed()<<"adapter resets"<<resets.count();if(QCoreApplication::arguments().contains("--history-perf-assert"))check(resets.count()<=2,"history switch batches model presentation instead of resetting once per row");}
    archive.reject();qInfo()<<"PASS: history performance fixture";
}

static void archiveHistoryRegressions(MainWindow &main) {
    Hill hill;hill.setKPoint(90);hill.setHSPoint(100);hill.setName("Archive hill");Jumper a("A","History","POL"),b("B","History","GER");CompetitionInfo first(&hill),second(&hill);CompetitionRules rules;rules.setRounds({RoundInfo(2,true)});first.setRules(rules);second.setRules(rules);
    QVector<CompetitionInfo *> events{&first,&second};second.setPlayed(true);CompetitionsArchiveListModel archiveModel(&events);QAbstractItemModelTester modelTester(&archiveModel,QAbstractItemModelTester::FailureReportingMode::Fatal);
    check(archiveModel.rowCount()==1 && archiveModel.competitionAt(0)==&second,"played events after a calendar gap remain visible in archive");QSignalSpy reset(&archiveModel,&QAbstractItemModel::modelReset);first.setPlayed(true);archiveModel.setSeasonCompetitions(&events);check(reset.count()==1 && archiveModel.rowCount()==2,"archive announces newly played events without reopening the game");
    Classification classification;classification.setClassificationType(Classification::Individual);classification.setPunctationType(Classification::CompetitionPoints);first.setClassifications({&classification});second.setClassifications({&classification});
    const auto jump=[&](CompetitionInfo &event,Jumper *jumper,double points){JumpData data(jumper,&hill);data.setCompetition(&event);data.setPoints(points);event.getResultsReference().addJump(jumper,data);};
    jump(first,&a,100);jump(first,&b,80);first.getResultsReference().sortInDescendingOrder();jump(second,&a,10);jump(second,&b,80);second.getResultsReference().sortInDescendingOrder();
    SeasonCalendar calendar;calendar.setCompetitions(events);ClassificationResultsTableView view(false,&classification);view.setHistoryCalendar(&calendar);view.fillTable();auto *choice=view.findChild<QComboBox *>("comboBox_classificationHistory");check(choice && choice->count()==3,"classification history exposes each participating played event");
    choice->setCurrentIndex(1);check(view.getTableView()->model()->index(0,1).data().toString()==a.getNameAndSurname() && view.getTableView()->model()->index(0,2).data().toDouble()==100,"classification after first event excludes later scores");choice->setCurrentIndex(2);check(view.getTableView()->model()->index(0,1).data().toString()==b.getNameAndSurname() && view.getTableView()->model()->index(0,2).data().toDouble()==160,"classification history recalculates correct ranking after second event");
    const auto original=QJsonDocument(CompetitionResults::getJsonObject(first.getResultsReference())).toJson();const auto snapshot=ResultsHistoryWidget::afterRound(first.getResultsReference(),1,false);check(snapshot.getResults().size()==2 && QJsonDocument(CompetitionResults::getJsonObject(first.getResultsReference())).toJson()==original,"historical ranking does not mutate persisted competition results");
    auto copied=first.getResultsReference().getResultOfIndividualJumper(&a)->getJumpsReference().first();check(copied.getPositionInRound()==1,"a copied historical jump resolves its series by stable identity");
    JumpData unknown(&a,&hill);unknown.setCompetition(&first);check(unknown.getPositionInRound()==0,"unknown jump identity has no invented series position");
    CompetitionInfo disqualified(&hill);disqualified.setRules(rules);JumpData dsq(&a,&hill);dsq.setCompetition(&disqualified);dsq.setDSQ(true);disqualified.getResultsReference().addJump(&a,dsq);ResultsTableModel dsqModel(CompetitionRules::Individual,&disqualified.getResultsReference());check(dsqModel.index(0,3).data().toString()=="DSQ","disqualified attempt has an explicit DSQ marker rather than only zero distance");
    Q_UNUSED(main);qInfo()<<"PASS: archive reset signals, non-contiguous played events and classification chronology";
}

static void jumperCatalogClickRegressions(MainWindow &main) {
    main.show();main.resize(1440,900);DatabaseEditorWindow database(&main);database.show();QTest::qWait(150);
    auto *bridge=database.findChild<QuickBridge *>();auto *entry=qobject_cast<QuickNode *>(bridge->find("listView"));check(entry,"jumper catalog node exists");
    auto *view=scene(&database);auto *table=item(view->rootObject(),"dataTable-"+QString::number(entry->values().value("id").toInt()));
    check(table && table->property("singleClickOpen").toBool() && table->property("expandedActions").toBool(),"jumper catalog enables single-click editing and direct actions");
    auto *rows=item(table,"tableRows-"+QString::number(entry->values().value("id").toInt()));check(rows,"jumper rows exist");
    const auto click=rows->mapToItem(view->rootObject(),QPointF(70,60)).toPoint();QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,click);QTest::qWait(100);
    auto *editor=database.findChild<JumperEditorWidget *>();check(editor && !editor->isHidden() && editor->property("packRow").toInt()==1,"one click opens the second athlete for editing");
    check(item(view->rootObject(),"catalogAdd")->isVisible(),"add athlete remains available as plus");
    check(item(view->rootObject(),"catalogSelectAll")->isVisible(),"select all is directly visible");
    check(!item(table,"table-options-"+QString::number(entry->values().value("id").toInt()))->isVisible(),"jumper list has no overflow menu");
    for(const auto &action:entry->values().value("tableActions").toList()) {
        const auto fields=action.toMap();auto *button=item(table,"catalogAction-"+fields.value("id").toString());
        check(fields.value("insert").toBool()? !button : button && button->isVisible(),"remaining actions are visible without a duplicate add action");
    }
    render(&database,"jumper-catalog-actions",true);database.reject();
    qInfo()<<"PASS: single-click athlete editing, direct actions and one add control";
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
    check(QuickRaceStats::biggestAdvancement(results,2,1,3).isEmpty(),"partial round cannot award an advancement created solely by athletes still waiting to jump");
    const auto partial=QuickRaceStats::biggestAdvancement(results,2,1,1);
    check(partial.value("places").toInt()==1 && partial.value("entries").toList().first().toMap().value("to").toInt()==2,"remaining athletes reduce both the displayed gain and its target place");
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
    check(QuickRaceStats::biggestAdvancement(teams,2,4,1).isEmpty(),"the first team in a new group cannot gain a place merely because its rival has not jumped");
    gain=QuickRaceStats::biggestAdvancement(teams,2,4);
    check(gain.value("places").toInt()==1 && gain.value("name").toString()=="POL","team advancement compares complete previous-round team totals");
    QVector<StartListCompetitorStatus> statuses{StartListCompetitorStatus(&a,StartListCompetitorStatus::Unfinished),StartListCompetitorStatus(&b,StartListCompetitorStatus::Dns),StartListCompetitorStatus(&c,StartListCompetitorStatus::Dsq),StartListCompetitorStatus(&d,StartListCompetitorStatus::Finished)};
    check(StartListCompetitorStatus::remainingJumpers(statuses)==1,"DNS and disqualified athletes do not count as competitors waiting for a jump");
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

static void csvFieldsRegression() {
    Jumper jumper("A.","B; \"C\"\nD","POL");CompetitionInfo competition;
    CompetitionSingleResult result(&competition,&jumper);result.setPosition(1);result.setPointsSum(123.5);
    JumpData jump; jump.setPoints(123.5);jump.setDistance(101.5);jump.setAveragedWind(-.25);result.setJumps({jump});
    const QString csv=result.getCsvResultsObject();
    check(csv.startsWith("1;\"A. B; \"\"C\"\"\nD\";POL;123,5;101,5;"),"CSV quotes name separators and preserves name punctuation while formatting numeric decimals");
    Team unknown("XXX");CompetitionSingleResult teamResult(&competition,&unknown);teamResult.setPosition(2);teamResult.setPointsSum(88.25);
    check(teamResult.getCsvResultsObject().startsWith("2;XXX;XXX;88,25;"),"CSV exports unknown team country safely using its code");
    qInfo()<<"PASS: CSV names, separators, quotes, line breaks, decimal points and unknown countries";
}

static void nextCompetitionReloadRegression() {
    SimulationSave source;source.setName("Next event regression");
    auto *hill=new Hill(GlobalDatabase::get()->getEditableGlobalHills().first());source.setHills({hill});
    auto *calendar=new SeasonCalendar("Calendar");
    auto *played=new CompetitionInfo(hill),*pending=new CompetitionInfo(hill),*later=new CompetitionInfo(hill);
    played->setPlayed(true);calendar->setCompetitions({played,pending,later});
    Season season;season.setCalendars({calendar});season.setActualCalendar(calendar);source.setSeasons({season});source.setActualSeason(&source.getSeasonsReference().first());source.updateNextCompetitionIndex();
    for(int index:{0,2,-1,100}) {
        auto json=SimulationSave::getJsonObject(source);json["next-competition-index"]=index;
        std::unique_ptr<SimulationSave> loaded(SimulationSave::getFromJson(json,nullptr));
        check(loaded && loaded->getNextCompetitionIndex()==1 && loaded->getNextCompetition() && loaded->getNextCompetition()->getID()==pending->getID(),"reload derives the first pending event instead of replaying or skipping from a stale index");
    }
    pending->setPlayed(true);later->setPlayed(true);auto json=SimulationSave::getJsonObject(source);json["next-competition-index"]=0;
    std::unique_ptr<SimulationSave> loaded(SimulationSave::getFromJson(json,nullptr));
    check(loaded && !loaded->getNextCompetition() && loaded->getNextCompetitionIndex()==3,"a finished calendar stays finished even when its saved next index is stale");
    qInfo()<<"PASS: stale next-event indices cannot replay or skip calendar events on reload";
}

static void historicalTeamPointsRegression() {
    Jumper jumper("A","One","POL"),other("B","Two","POL");Team team("POL");team.setJumpers({&jumper,&other});
    CompetitionInfo competition;CompetitionRules rules;rules.setCompetitionType(CompetitionRules::Team);competition.setRules(rules);
    auto &results=competition.getResultsReference();JumpData jump; jump.setJumper(&jumper);jump.setPoints(123.5);results.addJump(&team,jump);results.getResultsReference().first().updateTeamJumpersResults();
    Classification classification;classification.setClassificationType(Classification::Individual);classification.setPunctationType(Classification::CompetitionPoints);
    ClassificationSingleResult entry(&classification,&jumper);entry.setCompetitionsResults({&results});entry.updateSingleResults();entry.updatePointsSum();check(entry.getPointsSum()==123.5,"historic team result contributes its athlete's jump points");
    jumper.setCountryCode("NOR");entry.updateSingleResults();entry.updatePointsSum();
    check(entry.getPointsSum()==123.5,"changing an athlete's nationality cannot erase points earned in a previous team event");
    ClassificationSingleResult unrelated(&classification,&other);unrelated.setCompetitionsResults({&results});unrelated.updateSingleResults();unrelated.updatePointsSum();check(unrelated.getPointsSum()==0,"team membership alone does not copy another athlete's jump points");
    qInfo()<<"PASS: historical team jump points survive subsequent nationality edits";
}

static void previousStandingRegression() {
    Jumper leader("A","One","POL"),rival("B","Two","NOR"),athlete("C","Three","GER");CompetitionResults results;
    const auto add=[&](Jumper *jumper,double points){JumpData jump; jump.setJumper(jumper);jump.setPoints(points);results.addJump(jumper,jump);results.sortInDescendingOrder();};
    add(&leader,120);add(&rival,110);add(&athlete,99.5);
    const auto standing=[&](Jumper *jumper,int count){return QuickRaceStats::previousStanding(results,*results.getResultOfIndividualJumper(jumper),count);};
    check(standing(&athlete,1).value("gap").toDouble()==20.5 && standing(&athlete,1).value("position").toInt()==3,"pre-jump standing shows the previous-round gap and place");
    check(standing(&leader,1).value("gap").toDouble()==-10,"a leader's previous-round advantage is negative relative to the best rival");
    add(&athlete,200);check(standing(&athlete,1).value("gap").toDouble()==20.5 && standing(&leader,1).value("gap").toDouble()==-10,"jumps already made in the current round cannot change the previous-round gap");
    check(standing(&rival,0).isEmpty() && standing(&rival,2).isEmpty(),"first-round and incomplete baselines have no invented gap");
    add(&leader,179.5);add(&rival,189.5);check(standing(&athlete,2).value("gap").toDouble()==0,"shared leaders show zero previous-round gap");
    qInfo()<<"PASS: previous-round gaps, negative leads, ties and frozen baselines";
}

static void simulationBenchmark() {
    auto *db=GlobalDatabase::get();auto jumper=db->getEditableGlobalJumpers().first();auto hill=db->getEditableGlobalHills().first();
    for(const auto &type:Characteristic::characteristicTypesForSpecificParent(Characteristic::Jumper))jumper.getJumperSkillsPointer()->insertCharacteristic(type,1);
    for(const auto &type:Characteristic::characteristicTypesForSpecificParent(Characteristic::Hill))hill.insertCharacteristic(type,1);
    CompetitionRules rules;rules.setHasDsq(false);JumpManipulator manipulator;
    JumpSimulator simulator(&jumper,{Wind(0,1),Wind(180,.5),Wind(45,1)},&hill);simulator.setCompetitionRules(&rules);simulator.setManipulator(&manipulator);simulator.setGate(10);simulator.setCompetitionStartGate(10);simulator.setJumpsImportance(5);simulator.setDSQBaseProbability(0);
    QElapsedTimer elapsed;elapsed.start();double total=0;
    for(int i=0;i<10000;++i) {simulator.simulateJump();total+=simulator.getJumpData().getPoints();}
    qInfo()<<"BENCH engine 10000 jumps ms"<<elapsed.elapsed()<<"checksum"<<total;check(std::isfinite(total),"engine benchmark has finite scores");
}

static void characteristicLookupRegression() {
    JumperSkills skills;skills.setCharacteristics({Characteristic(2.5,"flight-height"),Characteristic(-3,"takeoff-power"),Characteristic(0,"landing-skill")});
    const auto copy=skills;
    check(copy.getLevelOfCharacteristic("flight-height")==2.5 && copy.getLevelOfCharacteristic("takeoff-power")==-3 && copy.getLevelOfCharacteristic("landing-skill")==0 && copy.getLevelOfCharacteristic("missing")==0,"constant characteristic lookups preserve positive, negative, zero and absent values");
    skills.removeCharacteristic("flight-height");skills.insertCharacteristic("flight-height",7.5);
    check(skills.getLevelOfCharacteristic("flight-height")==7.5 && copy.getLevelOfCharacteristic("flight-height")==2.5,"lookup sees current edits while copied athlete skills remain independent");
    QSet<Characteristic> shared=skills.getCharacteristics();skills.setCharacteristics({Characteristic(1,"replacement")});
    check(skills.getLevelOfCharacteristic("flight-height")==0 && shared.contains(Characteristic("flight-height")),"replacing characteristics does not retain stale values or alter shared snapshots");
    qInfo()<<"PASS: characteristic lookups preserve current values and shared-copy isolation";
}

static void newRoundDefaultsRegression() {
    for(int type:{int(CompetitionRules::Individual),int(CompetitionRules::Team)}) {
        CompetitionRules rules;rules.setCompetitionType(type);rules.setJumpersInTeamCount(4);rules.setRounds({RoundInfo(50,false),RoundInfo(30,false)});
        CompetitionRulesEditorWidget editor;editor.setCompetitionRules(&rules);editor.fillCompetitionRulesInputs();
        auto *count=editor.findChild<QSpinBox *>("spinBox_roundsCount");count->setValue(4);
        const auto rounds=editor.getRoundsFromInput();
        check(rounds.size()==4 && !rounds[0].getSortStartList() && !rounds[1].getSortStartList(),"adding rounds preserves explicitly disabled sorting in existing rounds");
        check(rounds[2].getSortStartList() && rounds[3].getSortStartList(),"all newly added rounds default to sorted start lists");
        check(rounds[2].getCount()==30 && !rounds[2].getKO(),"new round retains the previous field size without leaking KO settings into the sorting flag");
        count->setValue(2);count->setValue(3);check(editor.getRoundsFromInput().last().getSortStartList(),"a removed and re-added round also defaults to sorting");
    }
    qInfo()<<"PASS: new rounds default to sorting while existing choices remain unchanged";
}

static void normalSamplingRegression() {
    double sum=0,squares=0,halfSum=0;constexpr int count=40000;
    for(int i=0;i<count;++i) {
        const double x=MyRandom::normalDistributionRandom(3,2);
        sum+=x;squares+=(x-3)*(x-3);
        const double positive=MyRandom::normalDistributionRandomHalf(0,2,MyRandom::Positive);
        const double negative=MyRandom::normalDistributionRandomHalf(0,2,MyRandom::Negative);
        check(positive>=0 && negative<=0,"centered half-normal draws preserve the requested sign");halfSum+=positive;
    }
    check(std::abs(sum/count-3)<.1 && std::abs(squares/count-4)<.25,"cached standard-normal samples preserve shifted mean and variance");
    check(std::abs(halfSum/count-2*std::sqrt(2/M_PI))<.1,"reflected centered samples preserve the half-normal mean");
    check(MyRandom::normalDistributionRandom(7,0)==7,"zero-deviation normal remains constant");
    for(int i=0;i<100;++i)check(MyRandom::normalDistributionRandomHalf(1,2,MyRandom::Negative)<0 && MyRandom::normalDistributionRandomHalf(-1,2,MyRandom::Positive)>=0,"non-centered half-normal draws still truncate instead of reflecting");
    qInfo()<<"PASS: cached normal sampling and centered half-normal distributions";
}

static void singleGateRegression(MainWindow &main) {
    SingleJumpsConfigWindow config;
    auto *db=GlobalDatabase::get();
    config.getJumperEditor()->setJumper(&db->getEditableGlobalJumpers().first());config.getJumperEditor()->fillJumperInputs();
    config.getHillEditor()->setHill(&db->getEditableGlobalHills().first());config.getHillEditor()->fillHillInputs();
    QVector<WindGenerationSettings> settings(6,WindGenerationSettings(Wind::Front,0,2,0));
    auto *wind=config.getWindsGeneratorSettingsEditor();wind->setWindGenerationSettings(&settings);wind->setKPoint(config.getHillEditor()->getKPointFromInput());wind->fillSettingsInputs();
    auto *gate=config.findChild<QSpinBox *>("spinBox_gate");gate->setValue(17);
    const int headwind=config.suggestedGate();
    check(headwind>=gate->minimum() && headwind<=gate->maximum() && headwind==config.suggestedGate() && gate->value()==17,"single-jump gate preview is bounded, deterministic and preserves manual input");
    settings.fill(WindGenerationSettings(Wind::Back,0,2,0));wind->fillSettingsInputs();const int tailwind=config.suggestedGate();
    check(headwind<=tailwind,"single-jump gate accounts for configured headwind and tailwind");
    render(&config,"single-jump-auto-gate",true);
    main.resize(1024,640);QTest::qWait(100);clickControl(scene(&config),"action-pushButton_autoGate");
    check(gate->value()==tailwind,"auto gate action applies a concrete calculated number");
    config.getJumperEditor()->resetJumperInputs();gate->setValue(17);config.findChild<QPushButton *>("pushButton_autoGate")->click();
    check(config.suggestedGate()==-1 && gate->value()==17,"incomplete single-jump setup cannot replace a manual gate");
    config.reject();QTest::qWait(100);
    qInfo()<<"PASS: single-jump auto gate uses current athlete, hill and wind";
}

static void singleSimulationRegression() {
    SingleJumpsManager manager;
    auto *db=GlobalDatabase::get();manager.setJumper(db->getEditableGlobalJumpers().first());manager.setHill(db->getEditableGlobalHills().first());manager.setGate(15);manager.setImportance(5);manager.getRules().setHasDsq(false);
    manager.setChangeableWind(true);manager.setJumpsCount(50000);
    QVector<WindGenerationSettings> settings(6,WindGenerationSettings(0,2,1,2));manager.setWindsGeneratorSettings(settings);
    bool canceled=false;QTimer stop;
    QObject::connect(&stop,&QTimer::timeout,[&]{for(auto *widget:QApplication::allWidgets())if(auto *progress=qobject_cast<QProgressDialog *>(widget);progress&&progress->isVisible()&&progress->value()>0){stop.stop();canceled=true;progress->cancel();}});
    stop.start(5);manager.simulate();stop.stop();
    const int produced=manager.getJumpsReference().size();
    check(canceled && produced>0 && produced<50000 && manager.getJumpsCount()==produced,"batched single-jump progress remains cancellable and keeps partial results");
    const auto first=manager.getJumpsReference().first().getWinds();bool varied=false;
    for(const auto &jump:manager.getJumpsReference()) {
        check(std::isfinite(jump.getDistance())&&std::isfinite(jump.getPoints())&&jump.getWinds().size()==6,"batched simulation preserves finite full jump results and all wind sectors");
        if(jump.getWinds().first().getStrength()!=first.first().getStrength())varied=true;
    }
    check(varied,"changing winds are generated independently for each jump");
    manager.setChangeableWind(false);manager.setJumpsCount(3);manager.simulate();
    check(manager.getJumpsReference().size()==produced+3 && manager.getJumpsReference().first().getWinds().first().getStrength()==first.first().getStrength(),"reusing wind settings preserves past wind snapshots and appends requested results");
    for(int i=produced;i<produced+3;++i)check(manager.getJumpsReference()[i].getWinds().first().getStrength()==1,"fixed wind matches the configured base in every appended jump");
    qInfo()<<"PASS: cancellable single-jump batches, finite results and immutable wind history";
}

static void singleJumpsBenchmark(MainWindow &main) {
    main.show();main.resize(1200,800);auto *db=GlobalDatabase::get();
    for(bool changeable:{false,true}) {
        SingleJumpsManager manager;manager.setJumper(db->getEditableGlobalJumpers().first());manager.setHill(db->getEditableGlobalHills().first());manager.setJumpsCount(1000);manager.setGate(15);manager.setImportance(5);manager.setChangeableWind(changeable);manager.getRules().setHasDsq(false);manager.getRules().setHasGateCompensations(false);
        QVector<WindGenerationSettings> winds(6);for(auto &wind:winds){wind.setBaseWindStrength(1);wind.setWindStrengthInstability(2);wind.setWindDirectionInstability(2);}manager.setWindsGeneratorSettings(winds);
        QElapsedTimer elapsed;elapsed.start();manager.simulate();const auto simulationMs=elapsed.elapsed();check(manager.getJumpsReference().size()==1000,"single-jump simulation completes the requested count");
        elapsed.restart();SingleJumpsResultsWindow results(&manager);results.fillWindow();const auto setupMs=elapsed.elapsed();
        elapsed.restart();results.show();QApplication::processEvents();const auto showMs=elapsed.elapsed();
        qInfo()<<"BENCH single jumps 1000 varying wind"<<changeable<<"simulation ms"<<simulationMs<<"results setup ms"<<setupMs<<"show ms"<<showMs;
        if(QCoreApplication::arguments().contains("--single-jumps-only")) {
            render(&results,"jump-results",true);
            double shortHeight=0,previousViewport=0;
            for(int height:{700,1000,1400}) {
                main.resize(1440,height);QTest::qWait(100);scene(&results)->grabFramebuffer();
                auto *table=item(scene(&results)->rootObject(),"singleJumpsResultsTable");
                check(table && table->height()>height-300,"single-jump table expands vertically without the generic 540-pixel ceiling");
                if(shortHeight)check(std::abs((table->height()-shortHeight)-(scene(&results)->height()-previousViewport))<=1,"growing the viewport gives all additional height to results");
                shortHeight=table->height();previousViewport=scene(&results)->height();
            }
            auto *view=results.findChild<QTableView *>("tableView");auto *bridge=results.findChild<QuickBridge *>();auto *node=qobject_cast<QuickNode *>(bridge->find("tableView"));auto *adapter=qobject_cast<QuickTableModel *>(node->values().value("table").value<QObject *>());
            adapter->select(23,0,0);QTest::qWait(80);check(view->currentIndex().row()==23,"full-height results preserve native jump selection");
            main.resize(520,600);QTest::qWait(100);auto *workspace=item(scene(&results)->rootObject(),"singleJumpsResultsWorkspace");workspace->setProperty("section",1);QTest::qWait(100);scene(&results)->grabFramebuffer();
            check(!item(scene(&results)->rootObject(),"singleJumpsResultsTable")->isVisible() && item(scene(&results)->rootObject(),"singleJumpsResultsDetails")->isVisible(),"narrow-screen statistics use a separate full-height workspace");
        }
        results.reject();QTest::qWait(80);
    }
}

static void fisCountryRegressions() {
    Jumper german("Test","Athlete","DEU");
    auto legacyJumper=Jumper::getJsonObject(german);legacyJumper["country-code"]="DEU";
    auto loadedJumper=Jumper::getFromJson(legacyJumper);
    check(loadedJumper.getCountryCode()=="GER" && loadedJumper.getID()==german.getID(),"legacy ISO athlete loads with its FIS code and original identity");
    check(Jumper::getJsonObject(loadedJumper).value("country-code")=="GER","athletes persist FIS codes after loading old ISO data");
    Hill swiss("Test hill","CHE",90,100);
    auto legacyHill=Hill::getJsonObject(swiss);legacyHill["country-code"]="CHE";
    auto loadedHill=Hill::getFromJson(legacyHill);
    check(loadedHill.getCountryCode()=="SUI" && loadedHill.getID()==swiss.getID(),"legacy ISO hill loads as SUI without changing identity");
    Team slovenian("SVN");IdentifiableObjectsStorage storage;storage.add(&german);slovenian.getJumpersReference().append(&german);
    auto legacyTeam=Team::getJsonObject(slovenian);legacyTeam["country-code"]="SVN";
    auto loadedTeam=Team::getFromJson(legacyTeam,&storage);
    check(loadedTeam.getCountryCode()=="SLO" && loadedTeam.getJumpers().first()==&german,"legacy team keeps its athlete links while adopting FIS codes");
    QVector<Team> teams{loadedTeam};check(Team::getTeamByCountryCode(&teams,"SVN")==&teams[0] && Team::getTeamByCountryCode(&teams,"SLO")==&teams[0],"ISO and FIS aliases resolve to the same team");
    Classification classification;storage.add(&classification);ClassificationSingleResult points(&classification,QString("CHE"));points.setPointsSum(125.5);
    auto legacyPoints=ClassificationSingleResult::getJsonObject(points);legacyPoints["team-code"]="CHE";
    auto loadedPoints=ClassificationSingleResult::getFromJson(legacyPoints,&storage);classification.getResultsReference().append(&loadedPoints);
    check(loadedPoints.getTeamCode()=="SUI" && loadedPoints.getPointsSum()==125.5 && classification.getResultOfTeam("CHE")==&loadedPoints && classification.getResultOfTeam("SUI")==&loadedPoints,"historical classification points survive ISO-to-FIS aliases");
    auto *db=GlobalDatabase::get();check(db->getCountryByAlpha3("DEU")==db->getCountryByAlpha3("GER") && db->getCountryByAlpha3("GER")->getAlpha3()=="GER","country metadata accepts both old ISO and new FIS codes");
    check(!CountryFlagsManager::getFlagPixmap("GER").isNull() && CountryFlagsManager::getFlagPixmap("GER").toImage()==CountryFlagsManager::getFlagPixmap("DEU").toImage(),"FIS codes resolve the existing ISO flag assets");
    check(CountryCodes::toFis(" che ")=="SUI" && CountryCodes::toFis("SUI")=="SUI" && CountryCodes::toFis("HRV")=="CRO" && CountryCodes::toFis("TTO")=="TTO" && CountryCodes::toFis("ZZZ")=="ZZZ","FIS codes normalize aliases while preserving current FIS and custom countries");
    qInfo()<<"PASS: FIS countries, legacy databases, flags, team links and historical points";
}
static void roundPointsRegressions() {
    Jumper a("Round","One","POL"),b("Round","Two","GER"),c("Round","Three","SUI");
    CompetitionRules rules;rules.setRounds({RoundInfo(3,true),RoundInfo(2,true),RoundInfo(2,true),RoundInfo(2,true)});
    auto old=CompetitionRules::getJsonObject(rules);QJsonArray oldRounds;
    for(auto value:old.value("rounds").toArray()){auto round=value.toObject();round.remove("reset-points");round.remove("previous-rounds-weights");oldRounds.append(round);}old["rounds"]=oldRounds;
    check(CompetitionRules::getFromJson(old).getRoundPointWeights(4)==QVector<double>({1,1,1,1}),"legacy rules keep ordinary cumulative scoring");
    rules.getRoundsReference()[2].setPreviousRoundsWeights({0,.5});
    CompetitionInfo competition;competition.setRules(rules);auto &results=competition.getResultsReference();
    const auto add=[&](Jumper *jumper,double points){JumpData jump(jumper);jump.setPoints(points);results.addJump(jumper,jump);};
    add(&a,100);add(&a,200);add(&a,300);
    auto *own=results.getResultOfIndividualJumper(&a);
    check(own->getPointsSum()==400 && own->pointsForJumps(2)==300,"specific previous rounds use raw points with independent weights");
    check(own->getJumps()[0].getPoints()==100 && own->getJumps()[1].getPoints()==200,"weighting never changes original jump scores");
    competition.getRulesPointer()->getRoundsReference()[3].setPreviousRoundsWeights({.2,.2,.2});add(&a,400);
    check(own->getPointsSum()==520,"three previous rounds can each contribute twenty percent");
    rules.getRoundsReference()[2].setPreviousRoundsWeights({});rules.getRoundsReference()[1].setResetPoints(true);competition.setRules(rules);
    results.getResultsReference().clear();add(&a,100);add(&b,80);add(&c,70);
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&competition);manager.setCompetitionRules(competition.getRulesPointer());manager.setResults(&results);manager.setRoundsJumpers({{&a,&b,&c}});manager.setActualRound(1);manager.setRoundsStartingGates({0});manager.setStartListStatuses({{&a,StartListCompetitorStatus::Finished},{&b,StartListCompetitorStatus::Finished},{&c,StartListCompetitorStatus::Finished}});
    results.sortInDescendingOrder();manager.updateLastQualifiedResult();manager.setupNextRound();
    check(manager.getActualRoundJumpersReference().size()==2 && results.getResultOfIndividualJumper(&a)->getPointsSum()==0 && results.getResultOfIndividualJumper(&b)->getPointsSum()==0,"round transition resets qualifiers immediately, before their first jump");
    check(results.getResultOfIndividualJumper(&c)->getPointsSum()==70 && results.getResultOfIndividualJumper(&c)->getPosition()==3,"eliminated athletes keep their original result below the qualified round");
    add(&a,10);
    Hill hill("Target","POL",100,110);hill.setPointsForKPoint(60);hill.setPointsForMeter(2);competition.setHill(&hill);QVector<Wind> calm{Wind(0,0)};manager.setActualWinds(&calm);manager.updateToBeatLineDistance();
    check(manager.getToBeatLineDistance()==75.5,"leader target after a reset uses the new zero baseline");
    add(&b,20);results.sortInDescendingOrder();
    check(results.getResultOfIndividualJumper(&b)->getPosition()==1 && results.getResultOfIndividualJumper(&a)->getPosition()==2,"final ranking follows reset scores instead of the old leader");
    own=results.getResultOfIndividualJumper(&a);own->updatePointsSum(3);add(&a,30);
    check(own->getPointsSum()==40 && own->pointsForJumps(1)==100,"ordinary following rounds carry the reset total without reviving discarded points");
    const auto persisted=CompetitionRules::getFromJson(CompetitionRules::getJsonObject(rules));
    check(persisted.getRounds()[1].getResetPoints() && persisted.getRoundPointWeights(3)==QVector<double>({0,1,1}),"reset survives rules and preset serialization");
    CompetitionRulesEditorWidget editor;editor.setCompetitionRules(&rules);editor.fillCompetitionRulesInputs();
    auto *box=editor.findChild<QToolBox *>("toolBox_rounds");auto *third=qobject_cast<RoundInfoEditorWidget *>(box->widget(2));
    third->findChild<QCheckBox *>("checkBox_customRoundWeights")->setChecked(true);third->findChild<QDoubleSpinBox *>("doubleSpinBox_roundWeight_1")->setValue(0);third->findChild<QDoubleSpinBox *>("doubleSpinBox_roundWeight_2")->setValue(.5);
    const auto edited=CompetitionRules::getFromJson(CompetitionRules::getJsonObject(editor.getCompetitionRulesFromWidgetInputs()));
    check(edited.getRounds()[1].getResetPoints() && edited.getRounds()[2].getPreviousRoundsWeights()==QVector<double>({0,.5}),"competition editor saves reset and fractional advanced weights independently per round");
    editor.findChild<QSpinBox *>("spinBox_roundsCount")->setValue(5);
    check(editor.getRoundsFromInput()[2].getPreviousRoundsWeights()==QVector<double>({0,.5}),"adding a round preserves edited weights in existing rounds");
    Team team("POL");team.setJumpers({&a,&b});CompetitionInfo teamCompetition;rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(2);teamCompetition.setRules(rules);teamCompetition.setHill(&hill);
    auto &teamResults=teamCompetition.getResultsReference();
    const auto addTeam=[&](Jumper *jumper,double points){JumpData jump(jumper,&hill);jump.setPoints(points);teamResults.addJump(&team,jump);};
    addTeam(&a,100);addTeam(&b,110);auto *teamResult=teamResults.getResultOfTeam(&team);teamResult->updatePointsSum(2);
    check(teamResult->getPointsSum()==0,"team reset applies to the whole previous round");addTeam(&a,50);addTeam(&b,60);teamResult->updateTeamJumpersResults();
    check(teamResult->getPointsSum()==110 && teamResult->getTeamJumperResult(&a)->getPointsSum()==50 && teamResult->getTeamJumperResult(&b)->getPointsSum()==60,"team group jumps and individual contributions share the same reset policy");
    IdentifiableObjectsStorage storage;storage.add(&a);storage.add(&b);storage.add(&team);storage.add(&hill);
    auto reloaded=CompetitionSingleResult::getFromJson(CompetitionSingleResult::getJsonObject(*teamResult),&storage);reloaded.setCompetition(&teamCompetition);reloaded.updatePointsSum();
    check(reloaded.getPointsSum()==110 && reloaded.getJumps().size()==4 && reloaded.getTeamJumperResult(&a)->getPointsSum()==50,"weighted results reload with complete unmodified jump history and team contributions");
    qInfo()<<"PASS: round resets, fractional weights, inherited totals, rankings, teams, editors and legacy persistence";
}

static void roundPointsUiRegressions(MainWindow &main) {
    main.show();main.resize(1240,900);CompetitionRulesEditorWidget owner;
    QDialog fixture(&main);auto *layout=new QVBoxLayout(&fixture);
    auto *editor=new RoundInfoEditorWidget(true,30,true,0,false,0,0,0,&owner);editor->setRoundIndex(2);layout->addWidget(editor);
    fixture.show();QTest::qWait(200);auto *view=scene(&fixture);auto *bridge=fixture.findChild<QuickBridge *>();
    check(editor->findChild<QWidget *>("previousRoundWeights")->isHidden() && !item(view->rootObject(),"input-doubleSpinBox_roundWeight_1"),"advanced weights start collapsed");
    clickControl(view,"check-checkBox_resetPoints");check(editor->getResetPointsFromInput(),"visible reset control is clickable without advanced settings");
    check(!editor->findChild<QCheckBox *>("checkBox_customRoundWeights")->isEnabled(),"reset explicitly overrides custom weights");
    clickControl(view,"check-checkBox_resetPoints");clickControl(view,"check-checkBox_customRoundWeights");
    check(bridge->find("doubleSpinBox_roundWeight_1") && bridge->find("doubleSpinBox_roundWeight_2"),"advanced settings reveal one weight for each previous round");
    bridge->edit(id(bridge,"doubleSpinBox_roundWeight_1"),0);bridge->edit(id(bridge,"doubleSpinBox_roundWeight_2"),.5);QTest::qWait(100);
    check(editor->getPreviousRoundsWeightsFromInput()==QVector<double>({0,.5}),"Quick controls update the real fractional rules values");
    render(&fixture,"round-point-weights",true);
    QTest::qWait(150);
    clickControl(view,"input-doubleSpinBox_roundWeight_2");
    auto *clickedField=item(view->rootObject(),"input-doubleSpinBox_roundWeight_2");
    check(clickedField->hasActiveFocus(),"clicking the weight field focuses text input instead of its step buttons");
    QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,QLocale().toString(.2,'f',1));QTest::keyClick(view,Qt::Key_Tab);QTest::qWait(120);
    main.grab().save(output+"/round-point-weights-narrow-scrolled.png");
    const auto typedWeights=editor->getPreviousRoundsWeightsFromInput();
    auto *typedField=item(view->rootObject(),"input-doubleSpinBox_roundWeight_2");
    check(typedWeights==QVector<double>({0,.2}),QString("narrow weight keyboard input: weights=%1,%2 text=%3 locale=%4 focus=%5").arg(typedWeights.value(0)).arg(typedWeights.value(1)).arg(typedField->property("text").toString(),QLocale().name(),view->quickWindow()->activeFocusItem()?view->quickWindow()->activeFocusItem()->objectName():"none"));
    fixture.reject();
    qInfo()<<"PASS: visible round reset and expandable advanced weights in desktop and narrow Quick layouts";
}

static void dataPackRegressions() {
    Jumper athlete("Test", "Birth", "POL");
    check(athlete.ageInYear(2026) == -1, "old athletes have unknown age");
    athlete.setBirthDate(QDate(2000,12,31));
    check(athlete.ageInYear(2026) == 26 && athlete.ageInYear(2027) == 27, "age is annual and independent of birthday");
    check(Jumper::getFromJson(Jumper::getJsonObject(athlete)).getBirthDate() == QDate(2000,12,31), "birth date survives save serialization");
    DataPack p; p.name = "Roster 2026"; p.ageMode = true; p.referenceYear = 2026; p.jumpers = {athlete};
    DataPack loaded; QString error;
    check(DataPack::fromJson(p.toJson(),loaded,error) && loaded.jumpers.first().ageInYear(2027) == 27, "age pack round trip");
    auto invalid = p.toJson(); invalid.remove("reference-year");
    check(!DataPack::fromJson(invalid,loaded,error), "age packs require reference year");
    auto entries = p.toJson().value("jumpers").toArray(); auto entry = entries.first().toObject(); entry.insert("age",25); entries[0] = entry;
    invalid = p.toJson(); invalid.insert("jumpers",entries);
    check(!DataPack::fromJson(invalid,loaded,error), "conflicting age and birth year rejected");
    entry.remove("birth-date"); entry.remove("birth-year"); entry.insert("age",26); entries[0] = entry; invalid.insert("jumpers",entries);
    check(DataPack::fromJson(invalid,loaded,error) && loaded.jumpers.first().getBirthYear() == 2000 && !loaded.jumpers.first().getBirthDate().isValid(), "age-only import never invents a birthday");
    p.kind = DataPack::Rules; p.rules = {CompetitionRules()}; PointsForPlacesPreset points("Scoring"); points.getPointsForPlacesReference()[1] = 17.5; p.points = {points};
    check(DataPack::fromJson(p.toJson(),loaded,error) && loaded.rules.size() == 1 && loaded.points.first().getPointsForPlaces().value(1) == 17.5, "rules pack carries fractional points presets");
    p.kind = DataPack::Hills; p.hills = {Hill()};
    check(DataPack::fromJson(p.toJson(),loaded,error) && loaded.hills.size() == 1, "hill pack round trip");
    auto badId = p.toJson();
    auto badEntries = badId.value("hills").toArray(); auto badEntry = badEntries.first().toObject(); badEntry.insert("id", "a-b-c"); badEntries[0] = badEntry; badId.insert("hills",badEntries);
    check(!DataPack::fromJson(badId,loaded,error), "malformed entity identifiers rejected before legacy parser");
    auto *db = GlobalDatabase::get(); auto original = db->packsSnapshot();
    const auto originalId = db->activePackId(DataPack::Jumpers); const auto oldCount = db->getGlobalJumpers().size();
    p = DataPack(); p.name = "Second roster"; p.jumpers = {athlete};
    const auto secondId = db->addPack(p);
    check(secondId != originalId && db->getGlobalJumpers().size() == 1, "adding a pack activates an independent roster");
    db->getEditableGlobalJumpers()[0].setSurname("Edited");
    check(db->selectPack(originalId) && db->getGlobalJumpers().size() == oldCount, "switching restores original roster");
    check(db->selectPack(secondId) && db->getGlobalJumpers().first().getSurname() == "Edited", "switching preserves edits");
    check(db->writePacks(), "atomic pack catalog save");
    check(db->restorePacks(original,error) && db->loadPacks() && db->activePackId(DataPack::Jumpers) == secondId && db->getGlobalJumpers().first().getSurname() == "Edited", "disk reload preserves packs, selection and edits");
    check(db->restorePacks(original,error), "restore fixture packs");
    check(db->writePacks(), "restore disk fixture packs");
    qInfo() << "PASS: named data packs, age cohorts, serialization and disk persistence";
}

static void packCatalogRecoveryRegression()
{
    auto *db=GlobalDatabase::get();const auto original=db->packsSnapshot();QString error;
    const auto originalActive=db->activePackId(DataPack::Jumpers);
    DataPack extra;extra.name="Inactive catalog recovery fixture";extra.jumpers={Jumper("Preserved","Athlete","POL")};
    const auto extraId=db->addPack(extra);db->selectPack(originalActive);const auto catalog=db->packsSnapshot();
    QTemporaryDir temporary;check(temporary.isValid(),"pack recovery temporary directory");
    const auto working=QDir::currentPath();const auto source=QDir(working).absoluteFilePath("userData/GlobalDatabase");
    QDirIterator files(source,QDir::Files,QDirIterator::Subdirectories);
    while(files.hasNext()){
        const auto from=files.next();const auto to=temporary.path()+"/userData/GlobalDatabase/"+QDir(source).relativeFilePath(from);
        check(QDir().mkpath(QFileInfo(to).absolutePath()) && QFile::copy(from,to),"copy isolated catalog recovery fixture");
    }
    const auto packFile=temporary.path()+"/userData/GlobalDatabase/packs.json";
    {QFile file(packFile);check(file.open(QFile::WriteOnly) && file.write(QJsonDocument(catalog).toJson())>0,"write catalog fixture with an inactive pack");}
    check(QFile::remove(temporary.path()+"/userData/GlobalDatabase/globalJumpers.json"),"remove only the legacy jumper export");
    check(QDir::setCurrent(temporary.path()),"enter isolated recovery fixture");
    int warnings=0;QTimer dismiss;QObject::connect(&dismiss,&QTimer::timeout,[&]{for(auto *w:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(w);box && box->isVisible()){++warnings;box->accept();}});dismiss.start(20);
    const bool loaded=db->loadFromJson();dismiss.stop();const auto restored=db->packsSnapshot();const bool written=db->writePacks();
    QJsonObject onDisk;{QFile file(packFile);if(file.open(QFile::ReadOnly))onDisk=QJsonDocument::fromJson(file.readAll()).object();}
    check(QDir::setCurrent(working) && db->restorePacks(original,error),"restore database after isolated recovery fixture");
    check(loaded && warnings==0 && written && restored==catalog && onDisk==catalog,"valid catalog survives a missing legacy export without losing inactive packs");
    bool preserved=false;for(const auto &value:onDisk.value("packs").toArray())if(value.toObject().value("id").toString()==extraId)preserved=true;
    check(preserved,"inactive pack identity survives recovery and save");
    qInfo()<<"PASS: authoritative pack catalog survives missing legacy database files";
}

static void dataPackUiRegressions(MainWindow &main) {
    auto *db = GlobalDatabase::get(); const auto original = db->packsSnapshot(); QString error;
    DataPack pack; pack.name = "Age roster"; pack.ageMode = true; pack.referenceYear = 2020;
    Jumper athlete("Age", "Athlete", "POL"); athlete.setBirthYear(2000); pack.jumpers = {athlete};
    db->addPack(pack);
    const auto beforeEditing = db->packsSnapshot();
    DatabaseEditorWindow dialog(&main); render(&dialog,"data-packs",true);
    auto *bridge = dialog.findChild<QuickBridge *>();
    auto *choice = dialog.findChild<QComboBox *>("dataPackChoice");
    check(choice && choice->count() >= 2, "multiple jumper packs are selectable");
    auto *list = dialog.findChildren<DatabaseItemsListView *>().first();
    check(QMetaObject::invokeMethod(&dialog,"onJumpersListViewDoubleClicked",Qt::DirectConnection,Q_ARG(QModelIndex,list->getListModel()->index(0,0))), "open jumper editor");
    bridge->refresh(); QTest::qWait(100);
    auto *age = dialog.findChild<QSpinBox *>("jumperAge");
    check(age && !age->isHidden() && age->value() == 20, "pack base year controls annual age input");
    bridge->edit(id(bridge,"jumperAge"),22);
    bridge->edit(id(bridge,"dataPackChoice"),0);
    bridge->edit(id(bridge,"dataPackChoice"),choice->count()-1);
    check(db->getGlobalJumpers().first().getBirthYear() == 1998, "switching packs commits open jumper changes");
    QMetaObject::invokeMethod(&dialog,"onJumpersListViewDoubleClicked",Qt::DirectConnection,Q_ARG(QModelIndex,list->getListModel()->index(0,0)));
    bridge->edit(id(bridge,"dataPackAgeFormat"),0); bridge->refresh();
    auto *date = dialog.findChild<QLineEdit *>("jumperBirthDate");
    check(date && !date->isHidden() && date->text().isEmpty(), "switch to dates retains unknown birthday");
    bridge->edit(id(bridge,"jumperBirthDate"),QString("1998-12-31"));
    bridge->edit(id(bridge,"dataPackAgeFormat"),1);
    check(db->getGlobalJumpers().first().getBirthDate() == QDate(1998,12,31) && age->value() == 22, "date to age conversion preserves exact birth date");
    bridge->edit(id(bridge,"dataPackReferenceYear"),2026);
    check(age->value() == 28, "changing base year updates age without changing birth year");
    const auto exported = QDir::currentPath()+"/share-pack.json";
    QTimer::singleShot(1000,[exported] {
        QFileDialog *file = nullptr;
        for (auto *w : QApplication::topLevelWidgets()) if ((file=qobject_cast<QFileDialog *>(w)) && file->isVisible()) break;
        check(file && file->isVisible(),"export file picker opens"); file->setDirectory(QFileInfo(exported).absolutePath());
        auto *name = file->findChild<QLineEdit *>("fileNameEdit"); check(name != nullptr,"file picker filename input exists");
        name->setText(QFileInfo(exported).fileName());
        qInfo()<<"PASS: file picker selected"<<file->selectedFiles(); QMetaObject::invokeMethod(file,"accept",Qt::DirectConnection);
    });
    bridge->activate(id(bridge,"exportDataPack")); QTest::qWait(1400);
    QFile file(exported); check(file.open(QIODevice::ReadOnly),QString("shareable pack file written: %1, files: %2").arg(exported,QDir(QDir::currentPath()).entryList(QDir::Files).join(",")));
    DataPack imported; check(DataPack::fromJson(QJsonDocument::fromJson(file.readAll()).object(),imported,error) && imported.jumpers.first().getBirthDate() == QDate(1998,12,31), "export contains edited birth information"); file.close();
    qInfo()<<"PASS: pack export completed";
    const auto count = choice->count();
    QTimer::singleShot(1000,[exported] {
        QFileDialog *file = nullptr;
        for (auto *w : QApplication::topLevelWidgets()) if ((file=qobject_cast<QFileDialog *>(w)) && file->isVisible()) break;
        check(file && file->isVisible(),"import file picker opens"); file->setDirectory(QFileInfo(exported).absolutePath());
        auto *name = file->findChild<QLineEdit *>("fileNameEdit"); check(name != nullptr,"file picker filename input exists");
        name->setText(QFileInfo(exported).fileName());
        qInfo()<<"PASS: file picker selected"<<file->selectedFiles(); QMetaObject::invokeMethod(file,"accept",Qt::DirectConnection);
    });
    bridge->activate(id(bridge,"importDataPack")); QTest::qWait(1400);
    check(choice->count() == count + 1 && db->getGlobalJumpers().first().ageInYear(2026) == 28,"import adds independent named pack through visible action");
    qInfo()<<"PASS: pack import completed";
    main.resize(1024,720);
    clickControl(scene(&dialog),"manageDataPack");
    QTimer::singleShot(120,[] {
        QInputDialog *input = nullptr;
        for (auto *w : QApplication::topLevelWidgets()) if ((input=qobject_cast<QInputDialog *>(w)) && input->isVisible()) break;
        check(input && input->isVisible(),"new pack name prompt opens"); input->setTextValue("Empty pack"); input->accept();
    });
    clickControl(scene(&dialog),"action-createDataPack");
    check(choice->count() == count + 2 && db->getGlobalJumpers().isEmpty(),"new pack action creates a named empty roster");
    bridge->edit(id(bridge,"dataPackChoice"),choice->count()-2);
    QMetaObject::invokeMethod(&dialog,"onJumpersListViewDoubleClicked",Qt::DirectConnection,Q_ARG(QModelIndex,list->getListModel()->index(0,0)));
    qInfo()<<"PASS: empty pack created";
    render(&dialog,"data-packs-edited",true);
    // The responsive capture leaves the host at its smallest size. Restore
    // the interaction fixture before continuing the pack persistence checks.
    main.resize(1024,720);
    QTest::qWait(120);
    clickControl(scene(&dialog),"closeDataPackSettings");
    auto *tabs = dialog.findChild<QTabWidget *>("tabWidget_main");
    tabs->setCurrentIndex(2); bridge->refresh();
    check(dialog.findChild<QWidget *>("dataPackAgeFormat")->parentWidget()->isHidden(),"age settings only apply to jumper packs");
    const auto rulesId = db->activePackId(DataPack::Rules);
    tabs->setCurrentIndex(3); check(choice->currentData().toString() == rulesId,"points and rules select the same pack");
    qInfo()<<"PASS: pack settings popup closed, controller visible"<<dialog.isVisible();
    auto answer = [](QMessageBox::StandardButton value) {
        QTimer::singleShot(120,[value] {
            QMessageBox *message = nullptr;
            for (auto *w : QApplication::topLevelWidgets()) if ((message=qobject_cast<QMessageBox *>(w)) && message->isVisible()) break;
            check(message && message->isVisible(),"database close asks about saving"); message->button(value)->click();
        });
    };
    answer(QMessageBox::Cancel); check(!dialog.close(),"cancel keeps editor open");
    answer(QMessageBox::No); check(dialog.close(),"discard closes editor");
    check(db->packsSnapshot() == beforeEditing,"discard restores every pack, format, roster and active selection");
    {
        DatabaseEditorWindow saved(&main); render(&saved,"data-packs-save");
        auto *savedBridge = saved.findChild<QuickBridge *>();
        savedBridge->edit(id(savedBridge,"dataPackName"),QString("Saved roster"));
        answer(QMessageBox::Yes); check(saved.close(),"save closes editor");
        check(db->loadPacks() && db->activePack(DataPack::Jumpers)->name == "Saved roster", "save and reload preserve pack edits");
    }
    check(db->restorePacks(original,error) && db->writePacks(),"restore UI fixture packs");
    main.resize(1024,640); qInfo()<<"PASS: pack editor switching, birth formats, import, export, save, discard and responsive layouts";
}

#include "classification-regressions.h"
#include "calendar-workspace-ux-regressions.h"
#include "calendar-inspector-space-regressions.h"
#include "calendar-dates-regressions.h"
#include "navigation-performance.h"
#include "ui-duplication-regressions.h"

static void backendRegressions() {
    competitionAdvancementRegressions();
    competitionTieRegressions(check);
    packCatalogRecoveryRegression();
    calendarDateRegressions();
    classificationScoringRegressions();
    classificationTablePerformanceRegression();
    dataPackRegressions();
    roundPointsRegressions();
    fisCountryRegressions();
    check(SingleJumpsManager().getImportance()==5.,"standalone jumps have a defined default importance");
    normalSamplingRegression();
    newRoundDefaultsRegression();
    characteristicLookupRegression();
    previousStandingRegression();
    historicalTeamPointsRegression();
    nextCompetitionReloadRegression();
    fractionalPointsPresetRegression();
    csvFieldsRegression();
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
        auto *body=item(quick->rootObject(),"modalMessage-0");check(body && body->width()>=quick->width()-40 && body->height()<40 && body->y()>35,"short message uses available width and sits below its title without wrapping into four lines");message.reject();
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

#include "ux-roster-input-regressions.h"

#include "simulation-onboarding-regressions.h"
#include "engine-audit-regressions.h"
#include "roster-translation-regressions.h"

int main(int argc,char **argv) {
    QApplication app(argc,argv);
    LayoutWarningRegressionGuard layoutWarnings;
#if QT_CONFIG(qml_debug)
    if(app.arguments().contains("--qml-profile")) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QQmlDebuggingEnabler::enableDebugging(true);
#else
        static QQmlDebuggingEnabler debugging(true);
        Q_UNUSED(debugging)
#endif
    }
#endif
    if (app.arguments().contains("--packs-only")) QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
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
    check(main.findChild<QuickBridge *>()->version()=="2.0.0-beta","one release version is shared with the interface");
    if(app.arguments().contains("--engine-audit-only")){int selected=0;for(const auto &argument:app.arguments())if(argument.startsWith("--engine-case="))selected=argument.mid(14).toInt();engineAuditRegressions(selected);return 0;}
    engineAuditRegressions();
    rosterTranslationRegression();
    if(app.arguments().contains("--engine-async-benchmark-only")){engineAsyncRegression();return 0;}
    competitionPresentationRegressions();
    latestJumpRegressions();
    advanceColorRegressions(main);
    if(app.arguments().contains("--jump-history-only")){runCompetition(main,0);runCompetition(main,2);return 0;}
    if(app.arguments().contains("--ko-multi-round-only")){runCompetition(main,1);return 0;}
    if(app.arguments().contains("--onboarding-only")){simulationOnboardingRegressions(main);return 0;}
    if(app.arguments().contains("--simulation-benchmark")) {simulationBenchmark();main.show();for(int i=0;i<3;++i)runCompetition(main,0,true);runCompetition(main,1,true);runCompetition(main,2,true);return 0;}
    if(app.arguments().contains("--single-jumps-benchmark")){singleJumpsBenchmark(main);return 0;}
    if(app.arguments().contains("--single-jumps-only")){normalSamplingRegression();singleSimulationRegression();singleGateRegression(main);singleJumpsBenchmark(main);return 0;}
    if(app.arguments().contains("--navigation-benchmark")){navigationPerformance(main);return 0;}
    if(app.arguments().contains("--ui-duplicates-only")){uiDuplicationRegressions(main);return 0;}
    if(app.arguments().contains("--super-team-only")){main.show();runCompetition(main,2,true,1);return 0;}
    if(app.arguments().contains("--roster-inputs-only")){rosterInputRegressions(main);return 0;}
    backendRegressions();
    if(app.arguments().contains("--team-dsq-only")){main.show();runCompetition(main,2);return 0;}
    if(app.arguments().contains("--team-only")){main.show();runCompetition(main,2);return 0;}
    if(app.arguments().contains("--interaction-only")){interactionRegressions(main);return 0;}
    if(app.arguments().contains("--space-only")){spaceRegressions(main);return 0;}
    if(app.arguments().contains("--ux-only")){uxRegressions(main);runCompetition(main,0);return 0;}
    if(app.arguments().contains("--audit-only")){simulationOnboardingRegressions(main);auditDataRegressions();auditFormRegressions();auditStatisticsRegressions(main);return 0;}
    if(app.arguments().contains("--audit-stats-only")){auditStatisticsRegressions(main);return 0;}
    if(app.arguments().contains("--audit-form-only")){auditFormRegressions();return 0;}
    if(app.arguments().contains("--audit-data-only")){auditDataRegressions();return 0;}
    if(app.arguments().contains("--geometry-only")){geometryRegressions(main);return 0;}
    if(app.arguments().contains("--ko-groups-only")){koGroupingRegressions(main);return 0;}
    if(app.arguments().contains("--rosters-only")){rosterRegressions(main);return 0;}
    if(app.arguments().contains("--menus-only")){menuRegressions(main);return 0;}
    if(app.arguments().contains("--workflow-only")){seasonWorkflowRegressions(main);return 0;}
    if(app.arguments().contains("--appearance-only")){appearanceRegressions(main);return 0;}
    if(app.arguments().contains("--author-only")){authorRegressions(main);return 0;}
    if(app.arguments().contains("--navigation-only")){navigationRegressions(main);return 0;}
    if(app.arguments().contains("--home-launch-only")){homeLaunchRegressions(main);return 0;}
    if(app.arguments().contains("--team-setup-compact-only")){teamSetupCompactRegression(main);return 0;}
    if(app.arguments().contains("--history-perf-only")){historyPerformanceRegression(main);return 0;}
    if(app.arguments().contains("--archive-history-only")){archiveHistoryRegressions(main);return 0;}
    if(app.arguments().contains("--history-dns-only")){archiveHistoryRegressions(main);main.show();for(int scenario=0;scenario<3;++scenario)runCompetition(main,scenario);return 0;}
    if(app.arguments().contains("--superko-only")){main.show();runCompetition(main,1);return 0;}
    if(app.arguments().contains("--manual-ko-only")){main.show();runCompetition(main,0);return 0;}
    if(app.arguments().contains("--jumper-catalog-only")){jumperCatalogClickRegressions(main);return 0;}
    if(app.arguments().contains("--result-scroll-only")){main.show();for(int scenario=0;scenario<3;++scenario)runCompetition(main,scenario);return 0;}
    if(app.arguments().contains("--race-options-only")){main.show();runCompetition(main,0);return 0;}
    if(app.arguments().contains("--round-points-only")){roundPointsUiRegressions(main);return 0;}
    if(app.arguments().contains("--round-points-bulk-only")){main.show();for(int scenario=0;scenario<3;++scenario)runCompetition(main,scenario,true);return 0;}
    if(app.arguments().contains("--packs-only")){dataPackUiRegressions(main);return 0;}
    if(app.arguments().contains("--classifications-only")){classificationUiRegressions(main);return 0;}
    if(app.arguments().contains("--dates-only")){calendarDateUiRegressions(main);return 0;}
    if(app.arguments().contains("--backend-only"))return 0;
    if(app.arguments().contains("--bulk-only")) {main.show();for(int scenario=0;scenario<3;++scenario)runCompetition(main,scenario,true);runCompetition(main,0,true,1);runCompetition(main,0,true,2);runCompetition(main,2,true,3);return 0;}
    if(app.arguments().contains("--competitions-only")) {
        main.show();QTest::qWait(100);GlobalAppSettings::get()->setLiveCompetition(false);
        for(int scenario=0;scenario<4;++scenario) runCompetition(main,scenario);
        seasons();qInfo()<<"PASS: full individual, KO and team competitions and season analysis";return 0;
    }
    if(!app.arguments().contains("--layout-only")){simulationOnboardingRegressions(main);auditDataRegressions();auditFormRegressions();auditStatisticsRegressions(main);rosterInputRegressions(main);interactionRegressions(main);spaceRegressions(main);uxRegressions(main);navigationRegressions(main);homeLaunchRegressions(main);menuRegressions(main);seasonWorkflowRegressions(main);calendarDateUiRegressions(main);rosterRegressions(main);koGroupingRegressions(main);geometryRegressions(main);}
    if(!app.arguments().contains("--layout-only")){for(int scenario=0;scenario<3;++scenario)runCompetition(main,scenario,true);runCompetition(main,0,true,1);runCompetition(main,0,true,2);runCompetition(main,2,true,3);}
    singleSimulationRegression();
    singleGateRegression(main);
    appearanceRegressions(main);authorRegressions(main);
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

#include "quick-ui-smoke.moc"
