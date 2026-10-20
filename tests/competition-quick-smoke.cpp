#include "../source/UI/Competition/Quick/CompetitionQuickBridge.h"
#include "../source/UI/Competition/CompetitionConfigWindow.h"
#include "../source/UI/EditorWidgets/HillEditorWidget.h"
#include "../source/UI/EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../source/UI/Competition/CompetitionManagerWindow.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/global/GlobalAppSettings.h"
#include "../source/global/GlobalSimulationSettings.h"
#include "../source/UI/Competition/Quick/CompetitionLiveBridge.h"
#include "../source/UI/EditorWidgets/RoundInfoEditorWidget.h"
#include <QWheelEvent>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QDialog>
#include <QApplication>
#include <QDir>
#include <QQuickWidget>
#include <QQuickItem>
#include <QSpinBox>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QTemporaryDir>
#include <QDirIterator>
#include <QFile>
#include <QtTest/QTest>
#include <QDebug>
#include <cstdlib>

static void check(bool condition, const char *message) {
    if (!condition) { qCritical() << "FAIL:" << message; std::exit(1); }
}
static QQuickItem *findItem(QQuickItem *root, const QString &name) {
    if (root->objectName() == name) return root;
    for (auto *child : root->childItems())
        if (auto *found = findItem(child, name)) return found;
    return nullptr;
}
static QQuickItem *findVisibleItem(QQuickItem *root,const QString &name) {
    if(!root->isVisible()) return nullptr;
    if(root->objectName()==name) return root;
    for(auto *child:root->childItems()) if(auto *found=findVisibleItem(child,name)) return found;
    return nullptr;
}
static QVariantMap findField(CompetitionQuickBridge *bridge, const QString &label) {
    for (const auto &card : bridge->cards())
        for (const auto &field : card.toMap().value("fields").toList())
            if (field.toMap().value("label").toString() == label) return field.toMap();
    qFatal("Missing field: %s", qPrintable(label));
    return {};
}
static void set(CompetitionQuickBridge *bridge, const QString &label, const QVariant &value) {
    bridge->setField(findField(bridge, label).value("id").toInt(), value);
    QTest::qWait(30);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("SJ-Sim-Quick-Smoke");
    QCoreApplication::setApplicationName("SJ-Sim-Quick-Smoke");
    // Simulation writes results/database records. Always test in a disposable
    // copy, never in the preview or user's live data directory.
    QTemporaryDir data;
    check(data.isValid(), "temporary test data directory");
    const QString source = QCoreApplication::applicationDirPath();
    for (const QString directory : {QString("userData"), QString("flags")}) {
        QDirIterator files(source + "/" + directory, QDir::Files, QDirIterator::Subdirectories);
        while (files.hasNext()) {
            const QString file = files.next();
            const QString relative = QDir(source).relativeFilePath(file);
            const QString destination = data.path() + "/" + relative;
            QDir().mkpath(QFileInfo(destination).absolutePath());
            check(QFile::copy(file, destination), "copy test data");
        }
    }
    QDir::setCurrent(data.path());
    GlobalDatabase::get()->loadFromJson();
    GlobalAppSettings::get()->loadFromJson();
    GlobalSimulationSettings::get()->loadFromFile();
    CompetitionQuickWindow window;
    window.show();
    QTest::qWait(200);
    check(window.view()->status() == QQuickWidget::Ready, "QML loads");
    auto *bridge = window.bridge();
    auto *original = bridge->configuration();
    // Exercise the actual QML popup: a populated C++ model alone does not
    // guarantee that the choices are visible or selectable in Qt Quick.
    auto *hillChoice = findItem(window.view()->rootObject(), "quick-Skocznia z bazy");
    check(hillChoice != nullptr, "QML hill selector");
    check(hillChoice->property("count").toInt() > 1, "hill choices loaded");
    auto *hillPopup = hillChoice->property("popup").value<QObject *>();
    check(hillPopup != nullptr, "hill popup exists");
    QMetaObject::invokeMethod(hillPopup, "open");
    QTest::qWait(100);
    check(hillPopup->property("visible").toBool(), "hill popup opens");
    check(hillPopup->property("height").toDouble() >= 100, "hill choices have visible popup height");
    auto *hillList = hillPopup->property("contentItem").value<QObject *>();
    check(hillList && hillList->property("count").toInt() == hillChoice->property("count").toInt(), "popup shows all hill choices");
    window.activateWindow();
    window.view()->setFocus();
    hillChoice->forceActiveFocus();
    QTest::keyClick(window.view(), Qt::Key_Down);
    QTest::keyClick(window.view(), Qt::Key_Return);
    QTest::qWait(100);
    check(hillChoice->property("currentIndex").toInt() == 1, "select hill from open popup using keyboard");
    check(original->getHillEditor()->getKPointFromInput() > 0, "visible hill selection reaches C++ configuration");
    QMetaObject::invokeMethod(hillPopup, "close");
    set(bridge, "Skocznia z bazy", 1);
    set(bridge, "Zasady z bazy", 1);
    set(bridge, "Belka startowa", 17);
    check(original->getStartingGateFromInput() == 17, "QML -> C++ gate");
    original->findChild<QSpinBox *>("spinBox_startGate")->setValue(23);
    bridge->refresh();
    check(findField(bridge, "Belka startowa").value("value").toInt() == 23, "Widgets -> QML gate");
    set(bridge, "Przeprowadź kwalifikacje", true);
    check(bridge->summary().value("qualifications").toBool(), "qualifications state");
    set(bridge, "Przeprowadź kwalifikacje", false);
    auto *gateInput = findItem(window.view()->rootObject(), "quick-Belka startowa");
    check(gateInput != nullptr, "QML gate control");
    window.activateWindow();
    window.view()->setFocus();
    gateInput->forceActiveFocus();
    QTest::qWait(30);
    QTest::keyClick(window.view(), Qt::Key_A, Qt::ControlModifier);
    QTest::keyClicks(window.view(), "31");
    findItem(window.view()->rootObject(), "commitFocus")->forceActiveFocus();
    QTest::qWait(60);
    check(original->getStartingGateFromInput() == 31, "keyboard input commits to original configuration");
    bridge->setSection(2);
    set(bridge, "Liczba serii", 3);
    check(original->getCompetitionRulesEditor()->getRoundsFromInput().size() == 3, "round creation");
    check(bridge->cards().size() == 5, "all rounds exposed");
    set(bridge, "Rodzaj konkursu", 1);
    check(bridge->summary().value("team").toBool(), "team configuration");
    set(bridge, "Rodzaj konkursu", 0);
    set(bridge, "Seria KO", true);
    check(bridge->summary().value("ko").toBool(), "KO configuration");
    set(bridge, "Seria KO", false);
    bridge->setSection(1);
    set(bridge, "Punkt K · m", 120.0);
    check(original->getHillEditor()->getKPointFromInput() == 120, "hill geometry");
    bridge->setSection(3);
    check(bridge->cards().size() > 1, "wind sectors created");
    set(bridge, "Bazowa siła · m/s", 1.75);
    check(qAbs(original->getWindGeneratorSettingsEditor()->getWindsGenerationSettingsFromInputs().first().getBaseWindStrength() - 1.75) < .001, "wind input survives bridge");
    const int count = original->getCompetitionJumpers().size();
    bridge->action("remove", {1, 3});
    QTest::qWait(100);
    check(original->getCompetitionJumpers().size() == count - 2, "multi-selection removal");

    // The real Widgets dialog reuses exactly the same configuration instance.
    QTimer::singleShot(100, original, [original] {
        original->findChild<QSpinBox *>("spinBox_startGate")->setValue(29);
        original->reject();
    });
    bridge->action("widgets");
    QTest::qWait(200);
    check(bridge->summary().value("gate").toInt() == 29, "Widgets round trip");

    // Render every section at desktop, intermediate and compact widths.
    for (const QSize size : {QSize(1440, 900), QSize(900, 700), QSize(520, 720)}) {
        window.resize(size);
        for (int section = 0; section < 5; ++section) {
            bridge->setSection(section);
            QTest::qWait(250);
            check(window.view()->rootObject()->width() == window.view()->width(), "responsive root width");
            const auto screenshot = window.view()->grabFramebuffer();
            check(!screenshot.isNull(), "rendered frame");
            screenshot.save(source + QString("/quick-%1-%2.png").arg(size.width()).arg(section));
        }
    }
    // Invalid input must still use the original C++ validation.
    bridge->setSection(2);
    set(bridge, "Liczba serii", 0);
    bool validationShown = false;
    QTimer closer;
    QObject::connect(&closer, &QTimer::timeout, [&] {
        for (auto *widget : QApplication::topLevelWidgets()) {
            if (auto *message = qobject_cast<QMessageBox *>(widget); message && message->isVisible()) {
                validationShown = true;
                message->accept();
            }
        }
    });
    closer.start(40);
    bridge->action("start");
    QTest::qWait(200);
    closer.stop();
    check(validationShown, "original validation invoked");
    // A valid setup reaches the actual competition manager (not a mock).
    bridge->setSection(0);
    set(bridge, "Zasady z bazy", 1);
    set(bridge, "Skocznia z bazy", 1);
    // Selecting an already active preset does not reload it in the legacy editor.
    set(bridge, "Zasady z bazy", 0);
    set(bridge, "Zasady z bazy", 1);
    const auto allJumpers=original->getCompetitionJumpers();
    int liveScenarios=0;
    QVariantList removeForLive;
    for(int i=16;i<original->getCompetitionJumpers().size();++i) removeForLive.append(i);
    bridge->action("remove",removeForLive);
    bool managerShown = false;
    QTimer managerCloser;
    QObject::connect(&managerCloser, &QTimer::timeout, [&] {
        for (auto *widget : QApplication::topLevelWidgets()) {
            if (auto *manager = qobject_cast<CompetitionManagerWindow *>(widget); manager && manager->isVisible()) {
                managerCloser.stop();
                managerShown = true;
                ++liveScenarios;
                auto *live = manager->findChild<CompetitionLiveBridge *>();
                auto *view = manager->findChild<QQuickWidget *>("competitionLiveQuickView");
                check(live && view && view->status()==QQuickWidget::Ready, "live competition QML loads");
                std::function<QVariantMap(QVariantList,QString)> find = [&](QVariantList nodes, QString name) -> QVariantMap {
                    for(const auto &v:nodes) { auto n=v.toMap(); if(n.value("name")==name) return n; auto child=find(n.value("children").toList(),name); if(!child.isEmpty()) return child; }
                    return {};
                };
                auto field = [&](QString name) { QVariantList nodes; for(auto section:live->sections()) nodes.append(section.toMap().value("children").toList()); auto result=find(nodes,name); check(!result.isEmpty(),qPrintable("live field: "+name)); return result; };
                manager->findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(false);
                live->edit(field("spinBox_actualGate").value("id").toInt(),19);
                check(manager->getManager()->getActualGate()==19,"live gate reaches simulator");
                QTest::qWait(100);
                if(liveScenarios==1) for(auto size:{QSize(1440,900),QSize(1024,700),QSize(520,640)}) {
                    manager->resize(size); QTest::qWait(80);
                    for(bool dark:{false,true}) {
                        live->setDark(dark);
                        for(int page=0;page<5;++page) {
                            view->rootObject()->setProperty("page",page); QTest::qWait(220);
                            if(size.width()>=1024) {
                                auto *start=findVisibleItem(view->rootObject(),"persistentStartList");
                                check(start!=nullptr,"desktop keeps the start list visible on every view");
                                const auto origin=start->mapToScene(QPointF(0,0));
                                check(origin.x()>=0 && origin.y()>=0 && origin.x()+start->width()<=view->width()+1 && origin.y()+start->height()<=view->height()+1,"start list fits inside the desktop viewport");
                                check(findVisibleItem(view->rootObject(),"automaticJumpPreview")!=nullptr,"desktop keeps jump preview beside results");
                            }
                            auto image=view->grabFramebuffer(); check(!image.isNull(),"live framebuffer");
                            image.save(source+QString("/live-%1-%2-%3.png").arg(size.width()).arg(page).arg(dark));
                        }
                    }
                }
                if(liveScenarios==1) {
                    view->rootObject()->setProperty("page",0); QTest::qWait(180);
                    auto *gate=findVisibleItem(view->rootObject(),"input-spinBox_actualGate");
                    check(gate!=nullptr,"visible Quick gate input");
                    gate->forceActiveFocus(); QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier); QTest::keyClicks(view,"21"); QTest::keyClick(view,Qt::Key_Tab); QTest::qWait(60);
                    check(manager->getManager()->getActualGate()==21,"keyboard gate input commits to competition");
                }
                // All local modal editors must also load Quick and retain their original result semantics.
                if(liveScenarios==1) for(const QString name:{QString("pushButton_windsGeneratorSettings"),QString("pushButton_inrunSnowGeneratorSettings"),QString("pushButton_manipulateJump"),QString("pushButton_coachGate")}) {
                    bool seen=false;
                    QTimer dismiss;
                    QObject::connect(&dismiss,&QTimer::timeout,[&] {
                        for(auto *w:QApplication::topLevelWidgets()) if(auto *d=qobject_cast<QDialog *>(w); d && d!=manager && d->isVisible() && d->parentWidget()==manager) {
                            auto *quick=d->findChild<QQuickWidget *>("competitionLiveQuickView");
                            check(quick && quick->status()==QQuickWidget::Ready,"competition dialog uses working QML"); seen=true; dismiss.stop();
                            quick->grabFramebuffer().save(source+"/live-dialog-"+name+".png");
                            if(name=="pushButton_inrunSnowGeneratorSettings") {
                                auto *editor=d->findChild<CompetitionLiveBridge *>();
                                check(!editor->sections().isEmpty(),"dialog schema initialized before show");
                                auto nodes=editor->sections().first().toMap().value("children").toList();
                                auto base=find(nodes,"doubleSpinBox_base"),submit=find(nodes,"pushButton_submit");
                                check(!base.isEmpty()&&!submit.isEmpty(),"snow editor controls exposed");
                                editor->edit(base.value("id").toInt(),2.5); editor->activate(submit.value("id").toInt());
                            } else {
                                if(name=="pushButton_windsGeneratorSettings") {
                                    auto *editor=d->findChild<CompetitionLiveBridge *>();
                                    check(!editor->sections().isEmpty(),"wind dialog schema ready");
                                    auto nodes=editor->sections().first().toMap().value("children").toList();
                                    auto input=find(nodes,"doubleSpinBox_baseWindStrength");
                                    auto *scroll=findItem(quick->rootObject(),"liveMainScroll");
                                    auto *flick=scroll ? scroll->property("contentItem").value<QObject *>() : nullptr;
                                    check(flick && !input.isEmpty(),"wind dialog scroll and editor exposed");
                                    flick->setProperty("contentY",250);
                                    const double position=flick->property("contentY").toDouble();
                                    check(position>0,"wind editor has scrollable content");
                                    editor->edit(input.value("id").toInt(),input.value("value").toDouble()+.1);
                                    QTest::qWait(60);
                                    check(qAbs(flick->property("contentY").toDouble()-position)<1,"editing preserves dialog scroll position");
                                }
                                d->reject();
                            }
                        }
                    });
                    dismiss.start(100); live->activate(field(name).value("id").toInt()); QTest::qWait(220); dismiss.stop(); check(seen,"competition editor opened");
                    if(name=="pushButton_inrunSnowGeneratorSettings") check(qAbs(manager->getInrunSnowGenerator().getBaseInrunSnow()-2.5)<.001,"Quick dialog submission updates snow generator");
                }
                live->classic(); check(view->isHidden(),"switch to Widgets");
                // Return with the visible Widgets control.
                for(auto *button:manager->findChildren<QToolButton *>()) if(button->text()=="Widok Qt Quick") button->click();
                check(!view->isHidden(),"return to Quick with same simulation");
                QTimer messages;
                QObject::connect(&messages,&QTimer::timeout,[&] {
                    for(auto *w:QApplication::topLevelWidgets()) if(auto *box=qobject_cast<QMessageBox *>(w); box && box->isVisible()) {
                        if(auto *yes=box->button(QMessageBox::Yes)) yes->click(); else box->accept();
                    }
                }); messages.start(15);
                manager->resize(1024,700); view->rootObject()->setProperty("page",1); QTest::qWait(80);
                int jumps=0;
                while(!manager->getManager()->checkCompetitionEnd() && jumps<400) {
                    auto *next=manager->findChild<QPushButton *>("pushButton_goToNext");
                    if(!next->isHidden()) { next->click(); QTest::qWait(15); }
                    auto *jump=manager->findChild<QPushButton *>("pushButton_jump");
                    check(jump->isEnabled(),"jump is enabled during competition");
                    live->refresh(); live->activate(field("pushButton_jump").value("id").toInt()); QTest::qWait(20); ++jumps;
                    if(jumps==1) {
                        live->refresh(); QTest::qWait(200);
                        check(findVisibleItem(view->rootObject(),"persistentStartList")!=nullptr,"start list stays visible after jumping");
                        auto *automatic=findVisibleItem(view->rootObject(),"automaticPreviewAthlete");
                        auto *distance=findVisibleItem(view->rootObject(),"automaticPreviewDistance");
                        check(automatic && !automatic->property("text").toString().isEmpty(),"last jumper appears automatically without changing views");
                        auto *nativePreview=manager->findChild<JumperCompetitionResultsWidget *>();
                        check(nativePreview && nativePreview->getJumperResult() && automatic->property("text").toString()==nativePreview->getJumperResult()->getJumper()->getNameAndSurname(),"automatic preview matches the jumper shown by the classic flow");
                        check(distance && distance->property("text").toString()!="—","jump distance appears automatically beside results");
                        view->rootObject()->setProperty("page",1); QTest::qWait(200);
                        auto image=view->grabFramebuffer(); image.save(source+QString("/live-results-%1.png").arg(liveScenarios));
                        if(manager->getType()==CompetitionRules::Team) {
                            auto table=live->sections()[1].toMap().value("children").toList().first().toMap();
                            const int count=table.value("rows").toList().size(),id=table.value("id").toInt();
                            live->toggleTree(id,0);
                            check(live->sections()[1].toMap().value("children").toList().first().toMap().value("rows").toList().size()<count,"team tree collapses");
                            live->toggleTree(id,0);
                        }
                        if(manager->getType()==CompetitionRules::Individual) {
                            auto *resultRows=findItem(view->rootObject(),"rows-tableView_results");
                            check(resultRows!=nullptr,"Quick result list exists");
                            QTest::mouseDClick(view,Qt::LeftButton,Qt::NoModifier,resultRows->mapToScene(QPointF(100,20)).toPoint());
                            QTest::qWait(80);
                            check(view->rootObject()->property("page").toInt()==3,"double click opens jump details");
                            check(!live->sections()[3].toMap().value("children").toList().isEmpty(),"result details populated");
                            view->rootObject()->setProperty("page",3); QTest::qWait(200);
                            view->grabFramebuffer().save(source+QString("/live-details-%1.png").arg(liveScenarios));
                        }
                        view->rootObject()->setProperty("page",2); QTest::qWait(200);
                        auto *rows=findItem(view->rootObject(),"rows-listView_startList");
                        check(rows && rows->height()>100,"start list has a visible scroll viewport");
                        if(rows->property("contentHeight").toReal()>rows->height()) {
                            const auto pos=rows->mapToScene(QPointF(100,50));
                            const double before=rows->property("contentY").toReal();
                            QWheelEvent wheel(pos,view->mapToGlobal(pos.toPoint()),QPoint(),QPoint(0,-120),Qt::NoButton,Qt::NoModifier,Qt::NoScrollPhase,false);
                            QApplication::sendEvent(view,&wheel); QTest::qWait(180);
                            check(rows->property("contentY").toReal()>before,"mouse wheel scrolls the start list");
                        }
                        view->rootObject()->setProperty("page",1);
                        // Exercise the real automatic-simulation menu and its Quick confirmation.
                        if(liveScenarios==1 || liveScenarios==3) {
                            const int beforeBatch=manager->getManager()->getActualStartListIndex();
                            live->activate(field("toolButton_autoSimulationActions").value("id").toInt(),1);
                            QTest::qWait(150);
                            check(manager->getManager()->getActualStartListIndex()!=beforeBatch || manager->getManager()->checkRoundEnd(),"automatic simulation advances the competition");
                        }
                    }
                }
                check(manager->getManager()->checkCompetitionEnd(),"full competition finishes through Quick");
                check(jumps>1 && manager->getManager()->getResults()->getResultsReference().size()>0,"full competition has results");
                messages.stop(); manager->reject();
            }
            if (auto *message = qobject_cast<QMessageBox *>(widget); message && message->isVisible())
                message->accept();
        }
    });
    managerCloser.start(40);
    bridge->action("start");
    QTest::qWait(300);
    managerCloser.stop();
    check(managerShown, "actual competition manager opens");
    // KO uses the same competition controller, including group results and advancement.
    bridge->setSection(2);
    set(bridge,"Seria KO",true);
    set(bridge,"Zawodników w grupie KO",2);
    set(bridge,"Awansujących z grupy KO",1);
    set(bridge,"Dobór grup KO",1);
    auto rounds=original->getCompetitionRulesEditor()->findChildren<RoundInfoEditorWidget *>();
    check(rounds.size()>=2,"two rounds for live KO test");
    rounds[1]->findChild<QSpinBox *>("spinBox_count")->setValue(8);
    auto koRound=original->getCompetitionRulesEditor()->getRoundsFromInput().first();
    auto koJumpers=original->getCompetitionJumpers();
    original->setCompetitionGroups(KOGroup::constructKOGroups(&koRound,&koJumpers,KOGroup::Classic,nullptr));
    managerCloser.start(40); bridge->action("start"); QTest::qWait(200); managerCloser.stop();
    check(liveScenarios==2,"full KO competition tested");
    // Four real squads, four athletes per squad, two rounds and all group transitions.
    set(bridge,"Seria KO",false);
    set(bridge,"Rodzaj konkursu",1);
    set(bridge,"Zawodników w drużynie",4);
    auto teams=Team::constructTeamsVectorByJumpersList(allJumpers,4);
    check(teams.size()>=4,"team fixture has four countries"); teams.resize(4);
    QVector<Jumper *> teamJumpers;
    for(auto &team:teams) { Team::cutTeamJumpers(&team,4); teamJumpers+=team.getJumpers(); }
    original->setCompetitionJumpers(teamJumpers);
    original->setCompetitionTeams(teams);
    rounds=original->getCompetitionRulesEditor()->findChildren<RoundInfoEditorWidget *>();
    rounds[1]->findChild<QSpinBox *>("spinBox_count")->setValue(3);
    managerCloser.start(40); bridge->action("start"); QTest::qWait(200); managerCloser.stop();
    check(liveScenarios==3,"full team competition tested");
    qInfo() << "PASS: QML loading, bridge round trip, dynamic rounds/wind/KO/teams, multi-selection, validation, full individual/KO/team competitions, Quick dialogs, wheel scrolling, live result details, 45 responsive renders";
    return 0;
}
