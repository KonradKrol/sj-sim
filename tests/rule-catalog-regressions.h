#pragma once
#include "../source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.h"
#include "../source/UI/DatabaseEditor/ListModels/CompetitionRulesListModel.h"
#include "../source/UI/DatabaseEditor/ListModels/PointsForPlacesPresetsListModel.h"

static void ruleCatalogModelRegressions() {
    QVector<CompetitionRules> rules{CompetitionRules("A"),CompetitionRules("B")};
    CompetitionRulesListModel model(&rules);QAbstractItemModelTester contract(&model,QAbstractItemModelTester::FailureReportingMode::Fatal);
    QItemSelectionModel selection(&model);selection.setCurrentIndex(model.index(1),QItemSelectionModel::ClearAndSelect);
    const auto selectedId=rules[1].getID();
    check(model.insertRows(0,1) && rules.size()==3 && selection.currentIndex().row()==2 && rules[2].getID()==selectedId,"inserting rules preserves selected identity and reports the new data atomically");
    check(model.moveRow({},2,{},0) && selection.currentIndex().row()==0 && rules[0].getID()==selectedId,"moving rules preserves Qt selection and identity");
    check(model.removeRows(1,1) && rules.size()==2 && rules[0].getID()==selectedId,"removing another rule cannot change the edited rule");
    QVector<PointsForPlacesPreset> presets{PointsForPlacesPreset("A"),PointsForPlacesPreset("B")};
    PointsForPlacesPresetsListModel points(&presets);QAbstractItemModelTester pointsContract(&points,QAbstractItemModelTester::FailureReportingMode::Fatal);
    QItemSelectionModel selectedPoints(&points);selectedPoints.setCurrentIndex(points.index(1),QItemSelectionModel::ClearAndSelect);
    check(points.insertRows(0,1) && selectedPoints.currentIndex().row()==2 && presets[2].getName()=="B","points preset insertion keeps selection bound to its original data");
    check(points.moveRow({},2,{},0) && selectedPoints.currentIndex().row()==0 && presets[0].getName()=="B","points preset moves use real Qt model notifications");
    check(points.removeRows(1,1) && presets.size()==2 && !points.removeRows(-1,1),"points removal validates bounds and changes the backing collection once");
    qInfo()<<"PASS: rule and points catalog model contracts and stable selection";
}

static void ruleCatalogEditorRegressions(MainWindow &main) {
    auto *db=GlobalDatabase::get();
    CompetitionRules rule("Catalog fixture");rule.setRounds({RoundInfo(50,true)});db->getEditableCompetitionRules().append(rule);
    PointsForPlacesPreset points("Points fixture");points.setPointsForPlaces({{1,100},{2,80}});db->getEditableGlobalPointsForPlacesPresets().append(points);
    DatabaseEditorWindow window(&main);window.show();QTest::qWait(150);
    auto *tabs=window.findChild<QTabWidget *>("tabWidget_main");check(tabs,"database tabs exist");
    DatabaseItemsListView *rulesList=nullptr,*pointsList=nullptr;
    for(auto *list:window.findChildren<DatabaseItemsListView *>()){if(list->getType()==DatabaseItemsListView::CompetitionRulesItems)rulesList=list;if(list->getType()==DatabaseItemsListView::PointsForPlacesPresetsItems)pointsList=list;}
    check(rulesList && pointsList,"rule and points catalogs exist");
    tabs->setCurrentIndex(2);const int row=db->getEditableCompetitionRules().size()-1;rulesList->getListView()->setCurrentIndex(rulesList->getListModel()->index(row));QTest::qWait(40);
    auto *ruleEditor=window.findChild<CompetitionRulesEditorWidget *>();auto *name=ruleEditor->findChild<QLineEdit *>("lineEdit_name");
    const auto id=db->getEditableCompetitionRules()[row].getID();name->setText("Edited catalog fixture");QMetaObject::invokeMethod(ruleEditor,"configurationChanged");QTest::qWait(40);
    check(db->getEditableCompetitionRules()[row].getID()==id && db->getEditableCompetitionRules()[row].getName()=="Edited catalog fixture","live rule editing preserves identity and updates the selected entry");
    QMetaObject::invokeMethod(rulesList,"onUpActionTriggered");QTest::qWait(40);
    check(ruleEditor->property("packRow").toInt()==row-1 && db->getEditableCompetitionRules()[row-1].getID()==id && name->text()=="Edited catalog fixture","moving an edited rule keeps the editor bound to the moved entry");
    tabs->setCurrentIndex(3);const int pointsRow=db->getEditableGlobalPointsForPlacesPresets().size()-1;pointsList->getListView()->setCurrentIndex(pointsList->getListModel()->index(pointsRow));QTest::qWait(40);
    auto *pointsEditor=window.findChild<PointsForPlacesPresetEditorWidget *>();auto *table=pointsEditor->findChild<QTableView *>("tableView_pointsForPlaces");
    check(table->model()->setData(table->model()->index(0,1),42.5,Qt::EditRole),"points remain directly editable");QTest::qWait(40);
    check(db->getEditableGlobalPointsForPlacesPresets()[pointsRow].getPointsForPlaces().value(1)==42.5,"point edits reach the correct preset immediately");
    pointsEditor->findChild<QPushButton *>("classificationAddPlace")->click();QTest::qWait(40);
    check(db->getEditableGlobalPointsForPlacesPresets()[pointsRow].getPointsForPlaces().size()==3,"adding a place does not lose edited point values");
    QTest::qWait(80);auto *grid=item(scene(&window)->rootObject(),"classificationPointsRows");
    check(grid && grid->isVisible() && grid->width()>0 && grid->property("count").toInt()==3,"the real Quick points editor renders its editable rows");
    render(&window,"rule-points-catalog",true);window.hide();
    qInfo()<<"PASS: live catalog editors, stable rule IDs, selection and points UI";
}
