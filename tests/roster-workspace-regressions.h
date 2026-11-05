#pragma once
#include "../source/UI/DatabaseEditor/ListModels/JumpersListsListModel.h"

static void rosterWorkspaceRegressions(MainWindow &main) {
    QVector<SaveJumpersList> lists;
    JumpersListsListModel model(&lists);QAbstractItemModelTester contract(&model,QAbstractItemModelTester::FailureReportingMode::Fatal);
    int published=0;QObject::connect(&model,&QAbstractItemModel::rowsInserted,&main,[&](const QModelIndex &,int first,int last){check(model.rowCount()==lists.size() && !model.data(model.index(last)).toString().isEmpty() && first>=0,"insert notification publishes actual roster data");++published;});
    for(int i=0;i<30;++i)check(model.insertRows(i,1),"insert a roster atomically");
    check(published==30 && lists.size()==30,"new lists cannot disappear between publication and mutation");
    for(int i=0;i<20;++i)check(model.removeRows(0,1),"remove a roster atomically");
    check(!model.insertRows(-1,1) && !model.removeRows(20,1),"invalid roster mutations are rejected");
    {
        QVector<SaveJumpersList> values;DatabaseItemsListView legacy(DatabaseItemsListView::JumpersListsItems,true,true,true);legacy.setJumpersLists(&values);legacy.setupListModel();QAbstractItemModelTester oldContract(legacy.getListModel(),QAbstractItemModelTester::FailureReportingMode::Fatal);
        check(QMetaObject::invokeMethod(&legacy,"onInsertActionTriggered") && values.size()==1,"legacy entry point creates exactly one complete list");
        check(QMetaObject::invokeMethod(&legacy,"onInsertActionTriggered") && values.size()==2,"legacy entry point stays synchronized on repeated insertion");
    }
    QVector<Jumper> actors;actors.reserve(600);QVector<Jumper *> pool,selected;
    for(int i=0;i<600;++i){actors.append(Jumper("Roster",QString::number(i),QStringList{"POL","NOR","GER"}[i%3]));pool.append(&actors.last());if(i%2==0)selected.append(&actors.last());}
    Jumper empty;empty.setName("");empty.setSurname("");Team team("POL");team.setJumpers({pool[0],pool[1],pool[2],pool[3],&empty});
    check(team.hasValidLineup(4),"an unnamed reserve does not invalidate four valid starters");
    std::swap(team.getJumpersReference()[0],team.getJumpersReference()[4]);
    check(!team.hasValidLineup(4) && !team.hasValidLineup(6),"an unnamed starter or too few athletes invalidates the lineup");
    team.setJumpers({pool[0],pool[3],pool[6],pool[0]});check(!team.hasValidLineup(4),"one athlete cannot fill two starting positions");
    check(Team::constructTeamsVectorByJumpersList({pool[0],pool[3],pool[6],&empty},4).isEmpty(),"unnamed athletes cannot form an incomplete team");
    check(Team::constructTeamsVectorByJumpersList({pool[0],pool[3],pool[6],pool[0]},4).isEmpty(),"duplicate athletes cannot form an incomplete team");
    check(Team::constructTeamsVectorByJumpersList({pool[0],pool[3],pool[6],pool[9],&empty},4).size()==1,"four distinct named athletes produce a valid team");
    JumpersListEditorWindow editor(&main,"Stress roster");editor.setAllJumpers(pool);editor.setSelectedJumpers(selected);editor.updateUnselectedJumpers();editor.show();QTest::qWait(150);
    auto *bridge=editor.findChild<QuickBridge *>();auto *roster=bridge?bridge->findChild<QuickRoster *>():nullptr;check(roster,"grouping stress backend");
    QAbstractItemModelTester library(roster->library(),QAbstractItemModelTester::FailureReportingMode::Fatal),order(roster->ordered(),QAbstractItemModelTester::FailureReportingMode::Fatal),grouped(roster->groupedRows(),QAbstractItemModelTester::FailureReportingMode::Fatal);
    for(int i=0;i<50;++i){clickControl(scene(&editor),i%2?"savedRosterView-0":"savedRosterView-1");QTest::qWait(25);check(editor.getSelectedJumpers()==selected,"switching group view preserves membership and real order");presentation(&editor)->grab();}
    editor.reject();
    {
        SimulationSave save;save.setName("Inline roster stress");QVector<Jumper *> owned;
        for(const auto &actor:actors)owned.append(new Jumper(actor));save.setJumpers(owned);
        Season season;season.setSeasonNumber(2026);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());
        SimulationSaveManagerWindow manager(&save,&main);manager.show();presentation(&manager)->resize(1440,900);QTest::qWait(100);
        const auto first=manager.createRoster();QTest::qWait(100);auto *inlineRoster=qobject_cast<QuickRoster *>(manager.rosterEditor());check(inlineRoster,"inline stress roster backend");
        inlineRoster->include(QuickRoster::key(owned[0]));inlineRoster->include(QuickRoster::key(owned[1]));QTest::qWait(50);
        QAbstractItemModelTester inlineLibrary(inlineRoster->library(),QAbstractItemModelTester::FailureReportingMode::Fatal),inlineGrouped(inlineRoster->groupedLibrary(),QAbstractItemModelTester::FailureReportingMode::Fatal);
        auto *view=scene(&manager);auto *grip=item(view->rootObject(),"savedRosterCandidateGrip-"+QuickRoster::key(owned[2]));auto *destination=item(view->rootObject(),"savedRosterSelected");
        check(grip && destination && grip->isVisible() && destination->isVisible(),"inline roster exposes drag source and destination");
        const auto start=grip->mapToItem(view->rootObject(),grip->boundingRect().center()).toPoint(),finish=destination->mapToItem(view->rootObject(),destination->boundingRect().center()).toPoint();
        QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,start);
        for(int i=1;i<=12;++i){const auto point=start+(finish-start)*i/12;QMouseEvent move(QEvent::MouseMove,point,view->mapToGlobal(point),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(20);}
        QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,finish);QTest::qWait(100);
        check(inlineRoster->count()==3,"dragging an athlete between inline panes adds that athlete");
        for(int i=0;i<30;++i){clickControl(scene(&manager),i%2?"savedRosterView-0":"savedRosterView-1");check(inlineRoster->count()==3,"inline grouping preserves the selected athletes with a 600-person library");}
        const auto filteredId=QuickRoster::key(owned[11]);inlineRoster->setQuery("Roster 11");inlineRoster->selectCandidate(filteredId);QTest::qWait(50);
        check(inlineRoster->candidateMarkedCount()==1 && inlineRoster->groupedLibrary()->rowCount()>0,"filtered inline library can mark an athlete");
        for(int i=0;i<6;++i){inlineRoster->setGrouped(i%2==0);presentation(&manager)->resize(i%2?1024:1440,i%2?640:900);QTest::qWait(30);check(inlineRoster->candidateMarkedCount()==1 && inlineRoster->count()==3,"filtering, resizing and regrouping preserve selection");}
        inlineRoster->selectCandidate(filteredId);inlineRoster->setQuery("");presentation(&manager)->resize(1440,900);QTest::qWait(80);
        const auto second=manager.createRoster();check(manager.selectRoster(first) && manager.selectRoster(second) && manager.selectRoster(first),"rapid roster switches retain identity");QTest::qWait(100);
        check(inlineRoster->count()==3,"rapid roster switches preserve membership after deferred updates");
        auto *name=item(view->rootObject(),"rosterName");check(name,"inline roster name field exists");name->forceActiveFocus();QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"Edited first");
        clickControl(view,"rosterCard-"+second);QTest::qWait(80);
        bool firstRenamed=false,secondUntouched=false;for(const auto &list:save.getJumpersLists()){if(list.getId()==first)firstRenamed=list.getName()=="Edited first";if(list.getId()==second)secondUntouched=list.getName()!="Edited first";}
        check(firstRenamed && secondUntouched,"changing rosters commits a pending name to the original roster only");
        auto *managerBridge=manager.findChild<QuickBridge *>();const auto active=manager.activeRosterId();
        managerBridge->edit(managerBridge->root()->values().value("id").toInt(),0);QTest::qWait(100);
        clickControl(view,"activeRosterCard-"+first);QTest::qWait(100);
        check(manager.editingRosterId()==first && manager.activeRosterId()==active && managerBridge->root()->values().value("value").toInt()>0,"overview roster cards open the chosen editor without changing the active roster");
        managerBridge->edit(managerBridge->root()->values().value("id").toInt(),0);QTest::qWait(100);manager.openRosterWorkspace(first);QTest::qWait(100);
        check(managerBridge->root()->values().value("value").toInt()>0,"Manage navigates even when the underlying native roster tab was already selected");
        const auto third=manager.createRoster();
        manager.setRosterMembers(first,{owned[0],owned[1]});
        manager.setRosterMembers(second,{owned[2]});manager.setRosterMembers(third,{owned[0]});
        manager.beginRosterBoard();
        check(manager.editRosterBoard(owned[0]->getIDStr(),second,{first,second},true),"board moves athlete between selected lists");
        check(manager.rosterBoardDirty() && save.getJumpersLists()[0].getJumpers().contains(owned[0]),"draft does not mutate live roster");
        check(manager.applyRosterBoard() && !manager.rosterBoardDirty(),"board applies the complete transaction");
        check(!save.getJumpersLists()[0].getJumpers().contains(owned[0]) && save.getJumpersLists()[1].getJumpers().contains(owned[0]) && save.getJumpersLists()[2].getJumpers().contains(owned[0]),"exclusive movement preserves hidden rosters");
        manager.beginRosterBoard();manager.editRosterBoard(owned[1]->getIDStr(),second,{first,second},false);
        manager.setRosterMembers(second,{owned[4]});
        check(!manager.applyRosterBoard() && !save.getJumpersLists()[1].getJumpers().contains(owned[1]),"conflicting external edits reject the complete draft");
        manager.discardRosterBoard();check(!manager.rosterBoardDirty(),"discard restores current live membership");
        auto *workspace=item(view->rootObject(),"seasonRostersWorkspace");check(workspace,"roster workspace remains available");
        manager.setRosterBoardOpen(true);QTest::qWait(100);
        check(item(view->rootObject(),"rosterBoardApply"),"combined roster board loads in the workspace");
        auto *board=item(view->rootObject(),"rosterBoard");
        auto *boardGrip=item(view->rootObject(),"rosterBoardGrip-"+owned[5]->getIDStr());
        auto *lane=item(view->rootObject(),"rosterBoardLane-"+first+"-GER");
        check(boardGrip && lane,"combined board exposes drag source and country/list target");
        const auto boardStart=boardGrip->mapToItem(view->rootObject(),boardGrip->boundingRect().center()).toPoint(),boardEnd=lane->mapToItem(view->rootObject(),lane->boundingRect().center()).toPoint();
        QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,boardStart);
        for(int i=1;i<=12;++i){const auto point=boardStart+(boardEnd-boardStart)*i/12;QMouseEvent move(QEvent::MouseMove,point,view->mapToGlobal(point),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(20);}
        QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,boardEnd);QTest::qWait(80);
        check(manager.rosterBoardDirty() && manager.rosterBoard().value("members").toMap().value(first).toStringList().contains(owned[5]->getIDStr()),"actual board drag changes the draft");
        view->grabFramebuffer().save("/tmp/sj-roster-board.png");
        board->setProperty("byCountry",false);presentation(&manager)->resize(1024,640);QTest::qWait(80);
        view->grabFramebuffer().save("/tmp/sj-roster-board-lists.png");
        manager.discardRosterBoard();
        manager.setRosterBoardOpen(false);
        manager.reject();
    }
    qInfo()<<"PASS: atomic roster insertion/removal and repeated live grouping transitions";
}
