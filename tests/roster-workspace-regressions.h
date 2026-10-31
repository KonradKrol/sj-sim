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
    JumpersListEditorWindow editor(&main,"Stress roster");editor.setAllJumpers(pool);editor.setSelectedJumpers(selected);editor.updateUnselectedJumpers();editor.show();QTest::qWait(150);
    auto *bridge=editor.findChild<QuickBridge *>();auto *roster=bridge?bridge->findChild<QuickRoster *>():nullptr;check(roster,"grouping stress backend");
    QAbstractItemModelTester library(roster->library(),QAbstractItemModelTester::FailureReportingMode::Fatal),order(roster->ordered(),QAbstractItemModelTester::FailureReportingMode::Fatal),grouped(roster->groupedRows(),QAbstractItemModelTester::FailureReportingMode::Fatal);
    for(int i=0;i<50;++i){clickControl(scene(&editor),i%2?"savedRosterView-0":"savedRosterView-1");QTest::qWait(25);check(editor.getSelectedJumpers()==selected,"switching group view preserves membership and real order");presentation(&editor)->grab();}
    editor.reject();qInfo()<<"PASS: atomic roster insertion/removal and repeated live grouping transitions";
}
