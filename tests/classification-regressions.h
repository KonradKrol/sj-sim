#pragma once
#include "../source/UI/EditorWidgets/ClassificationEditorWidget.h"
#include "../source/UI/Seasons/Classifications/ClassificationResultsTableModel.h"
#include <QJsonArray>

static void classificationScoringRegressions()
{
    Jumper a("A","Junior","POL"),b("B","Senior","POL"),c("C","Veteran","POL"),d("D","Unknown","NOR");
    a.setBirthDate(QDate(2008,12,31));b.setBirthYear(2004);c.setBirthYear(1990);
    CompetitionInfo first,second,third;
    auto add=[](CompetitionInfo &event,Jumper *jumper,int place,double points){
        CompetitionSingleResult result(&event,jumper);result.setPosition(place);result.setPointsSum(points);
        event.getResultsReference().getResultsReference().append(result);
    };
    for(auto *event:{&first,&second,&third}){
        add(*event,&a,1,300);add(*event,&b,2,290);add(*event,&c,31,200);add(*event,&d,40,100);
        event->setPlayed(true);
    }
    Classification classification("Variable scoring");classification.setPointsForPlaces({{1,100},{2,80}});
    ClassificationScoringMethod top40;top40.name="Top 40";top40.points={{1,50},{2,40},{31,8},{40,1}};
    ClassificationScoringMethod winner20;winner20.name="20 for winner";winner20.points={{1,20},{2,10}};
    auto options=classification.getOptions();options.methods={top40,winner20};classification.setOptions(options);
    ClassificationCompetitionSettings selection;selection.methodId=top40.id;second.setScoringFor(&classification,selection);
    selection.methodId=winner20.id;third.setScoringFor(&classification,selection);
    ClassificationSingleResult ar(&classification,&a),br(&classification,&b),cr(&classification,&c),dr(&classification,&d);
    for(auto *row:{&ar,&br,&cr,&dr}){row->setCompetitionsResults({&first.getResultsReference(),&second.getResultsReference(),&third.getResultsReference()});row->updateSingleResults();row->updatePointsSum();}
    check(ar.getPointsSum()==170 && br.getPointsSum()==130 && cr.getPointsSum()==8 && dr.getPointsSum()==1,"each competition chooses an independent classification scoring method");
    classification.setResults({&ar,&br,&cr,&dr});classification.sortInDescendingOrder();
    options.contextParameters=true;classification.setOptions(options);
    selection.parameters={{"multiplier",2.5},{"points-limit",1}};third.setScoringFor(&classification,selection);
    ar.updatePointsSum();br.updatePointsSum();check(ar.getPointsSum()==200 && br.getPointsSum()==120,"optional context multiplier and scoring-place limit");
    options.contextParameters=false;classification.setOptions(options);ar.updatePointsSum();check(ar.getPointsSum()==170,"classification can disable competition parameter overrides");
    options.ageFilter=true;options.minimumAge=18;options.maximumAge=22;options.referenceYear=2026;classification.setOptions(options);
    for(auto *row:{&ar,&br,&cr,&dr})row->updatePointsSum();classification.sortInDescendingOrder();
    check(classification.getRankedResults().size()==2 && ar.getPointsSum()==170 && br.getPointsSum()==130 && cr.getPointsSum()==0 && dr.getPointsSum()==0,"inclusive annual age filter excludes older and unknown-age athletes");
    ClassificationResultsTableModel table(&classification);check(table.rowCount()==2,"age-ineligible athletes do not appear in the classification table");
    options.includeUnknownAge=true;classification.setOptions(options);dr.updatePointsSum();classification.sortInDescendingOrder();check(table.rowCount()==3,"unknown ages can be explicitly included");
    options.ageFilter=false;options.countrySelection=1;options.countryCount=2;classification.setOptions(options);
    for(auto *row:{&ar,&br,&cr,&dr})row->updatePointsSum();classification.sortInDescendingOrder();
    check(table.rowCount()==3 && !classification.getRankedResults().contains(&cr) && classification.getResultsReference().size()==4,"top N per country hides rows without discarding their history");
    options.countrySelection=2;options.countryCount=1;classification.setOptions(options);classification.sortInDescendingOrder();
    check(table.rowCount()==2 && classification.getRankedResults().contains(&cr) && !classification.getRankedResults().contains(&ar),"bottom N per country reverses the selection");
    options.countrySelection=1;options.countryCount=2;classification.setOptions(options);
    ar.setPointsSum(10);br.setPointsSum(10);cr.setPointsSum(10);classification.sortInDescendingOrder();
    const auto tied=classification.getRankedResults();check(tied.size()==3 && tied.contains(&ar) && tied.contains(&br) && !tied.contains(&cr),"ties never expand an exact country quota and selection is deterministic");
    cr.setPointsSum(20);classification.sortInDescendingOrder();check(classification.getRankedResults().contains(&cr),"a previously hidden athlete can return to the ranking");

    Classification nations("Nations");nations.setClassificationType(Classification::Team);nations.setPointsForPlaces({{1,400}});nations.setAltPointsForPlaces({{1,50},{2,40},{31,30},{40,20}});
    auto country=nations.getOptions();country.countrySelection=1;country.countryCount=2;nations.setOptions(country);
    ClassificationSingleResult pol(&nations,QString("POL"));pol.setCompetitionsResults({&first.getResultsReference()});pol.updateSingleResults();pol.updatePointsSum();check(pol.getPointsSum()==90,"nations classification sums only N best individual results");
    country.countrySelection=2;nations.setOptions(country);pol.updatePointsSum();check(pol.getPointsSum()==70,"nations classification can sum N worst individual results");
    first.getResultsReference().getResultsReference()[2].setPosition(2);pol.updatePointsSum();check(pol.getPointsSum()==80,"equal places still contribute exactly N individual results");
    first.getResultsReference().getResultsReference()[2].setPosition(31);
    CompetitionInfo teamEvent;CompetitionRules teamRules;teamRules.setCompetitionType(CompetitionRules::Team);teamEvent.setRules(teamRules);
    Team team("POL");team.setJumpers({&a,&b});CompetitionSingleResult teamResult(&teamEvent,&team,CompetitionSingleResult::TeamResult);teamResult.setPosition(1);teamResult.setPointsSum(600);teamEvent.getResultsReference().getResultsReference().append(teamResult);
    pol.setCompetitionsResults({&first.getResultsReference(),&teamEvent.getResultsReference()});pol.updateSingleResults();pol.updatePointsSum();check(pol.getPointsSum()==470,"team-event points and selected individual-event points are added together");
    country.ageFilter=true;country.minimumAge=18;country.maximumAge=22;country.referenceYear=2026;nations.setOptions(country);
    pol.updatePointsSum();check(pol.getPointsSum()==490,"age filter is applied before the country selection");
    team.setJumpers({&a,&c});pol.updatePointsSum();check(pol.getPointsSum()==90,"a team containing an ineligible athlete cannot receive team-event points");
    classification.setAltPointsForPlaces({{1,12}});classification.setPunctationType(Classification::PointsForPlaces);
    options.ageFilter=false;options.countrySelection=0;classification.setOptions(options);team.setJumpers({&a,&b});
    ClassificationSingleResult teamMember(&classification,&a);teamMember.setCompetitionsResults({&teamEvent.getResultsReference()});teamMember.updateSingleResults();teamMember.updatePointsSum();check(teamMember.getPointsSum()==12,"individual classification uses athlete points for the place of their team");

    Classification definition;definition.copyConfiguration(classification);definition.setResults({});
    IdentifiableObjectsStorage storage;std::unique_ptr<Classification> restored(Classification::getFromJson(Classification::getJsonObject(&definition),&storage));
    check(restored->getOptions().toJson()==definition.getOptions().toJson() && restored->getAltPointsForPlaces()==definition.getAltPointsForPlaces(),"all scoring methods and filters survive classification serialization");
    auto legacy=Classification::getJsonObject(&definition);legacy.remove("scoring-options");std::unique_ptr<Classification> old(Classification::getFromJson(legacy,&storage));
    check(old->getOptions().methods.isEmpty() && !old->getOptions().ageFilter && old->getOptions().countrySelection==0,"legacy classification defaults preserve existing scoring");

    SeasonCalendar calendar("Scoring calendar");calendar.setCompetitions({&first,&second,&third});calendar.setClassifications({&classification});
    for(auto *event:calendar.getCompetitionsReference())event->setClassifications({&classification});
    options.countrySelection=0;classification.setOptions(options);
    const auto archive=classification.constructJumpersArchiveResults(nullptr,&calendar);check(archive.value(&c).value(&second)==3,"classification history uses the selected competition method");
    SeasonCalendarPreset preset(&calendar);
    auto *presetClass=preset.getCalendarReference().getClassificationsReference().first();
    auto *presetEvent=preset.getCalendarReference().getCompetitionsReference()[1];
    check(presetClass!=&classification && presetClass->getResultsReference().isEmpty() && classification.getResultsReference().size()==4,"calendar preset contains independent classification definitions without live standings");
    check(presetEvent->scoringFor(presetClass).methodId==top40.id && !presetEvent->getPlayed() && first.getPlayed(),"preset remaps method assignments and removes history without modifying the source calendar");
    IdentifiableObjectsStorage presetStorage;auto loaded=SeasonCalendarPreset::getFromJson(SeasonCalendarPreset::getJsonObject(preset),&presetStorage);
    auto *loadedClass=loaded.getCalendarReference().getClassificationsReference().first();
    auto *loadedEvent=loaded.getCalendarReference().getCompetitionsReference()[1];
    check(loadedClass->getOptions().toJson()==classification.getOptions().toJson() && loadedEvent->getClassificationsReference().contains(loadedClass) && loadedEvent->scoringFor(loadedClass).methodId==top40.id,"calendar preset round trip includes classification methods, filters and per-event assignment");
    qDeleteAll(preset.getCalendarReference().getCompetitionsReference());qDeleteAll(preset.getCalendarReference().getClassificationsReference());
    qDeleteAll(loaded.getCalendarReference().getCompetitionsReference());qDeleteAll(loaded.getCalendarReference().getClassificationsReference());
    qInfo()<<"PASS: variable classification scoring, context parameters, annual age, exact top/bottom country limits and calendar presets";
}

static void classificationUiRegressions(MainWindow &main)
{
    SimulationSave save;save.setName("Classification test");
    {NewSeasonConfiguratorWindow setup;save.setJumpers(setup.getJumpersReference());save.setHills(setup.getHillsReference());save.setCompetitionRules(setup.getCompetitionsRulesReference());}
    auto *calendar=new SeasonCalendar("Classification calendar");auto *event=new CompetitionInfo(save.getHillsReference().first());event->setRules(save.getCompetitionRulesReference().first());calendar->setCompetitions({event});
    auto *classification=new Classification("Nations");classification->setClassificationType(Classification::Team);classification->setPointsForPlaces({{1,400},{2,350}});classification->setAltPointsForPlaces({{1,100},{2,80}});calendar->setClassifications({classification});event->setClassifications({classification});
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    SimulationSaveManagerWindow manager(&save,&main);manager.fillNextCompetitionInformations();render(&manager,"classification-manager");
    auto *bridge=manager.findChild<QuickBridge *>();auto *backend=bridge->findChild<QuickCalendar *>();check(backend,"calendar backend exists");
    bridge->edit(bridge->root()->values().value("id").toInt(),4);QTest::qWait(120);
    auto *workspace=item(scene(&manager)->rootObject(),"calendarWorkspace");check(workspace,"calendar workspace is visible");
    workspace->setProperty("section",1);QTest::qWait(100);
    auto *classes=item(scene(&manager)->rootObject(),"calendarClassesWorkspace");check(classes,"classification workspace exists");classes->setProperty("editing",true);
    backend->selectClass(classification->getIDStr());auto *editor=backend->classificationEditor();
    check(editor->findChild<QLabel *>("label_altPreset")->text().contains(QString::fromUtf8("indywidualnych")),"team classification explains its individual-result scoring");
    auto *help=editor->findChild<QPushButton *>("classificationAlternativeHelp");check(help && help->text()=="?" && help->toolTip().contains(QString::fromUtf8("Pucharu Narodów")),"question-mark help explains the Nations Cup mechanism");
    editor->findChild<QPushButton *>("classificationAddMethod")->click();QTest::qWait(80);
    check(classification->getOptions().methods.size()==1,"new scoring method is saved by the calendar editor");
    auto *methodName=editor->findChild<QLineEdit *>("classificationMethodName");methodName->setText("Winner 20");QMetaObject::invokeMethod(methodName,"editingFinished");
    auto *points=editor->findChild<QTableView *>("tableView_pointsForPlaces");check(points->model()->setData(points->model()->index(0,1),20.),"edit method point table");
    editor->findChild<QCheckBox *>("classificationAdvanced")->setChecked(true);editor->findChild<QCheckBox *>("classificationContextParameters")->setChecked(true);
    editor->findChild<QCheckBox *>("classificationAgeFilter")->setChecked(true);editor->findChild<QSpinBox *>("classificationMinimumAge")->setValue(18);editor->findChild<QSpinBox *>("classificationMaximumAge")->setValue(30);
    editor->findChild<QComboBox *>("classificationCountrySelection")->setCurrentIndex(1);QMetaObject::invokeMethod(editor->findChild<QComboBox *>("classificationCountrySelection"),"activated",Q_ARG(int,1));editor->findChild<QSpinBox *>("classificationCountryCount")->setValue(2);QTest::qWait(100);
    const auto options=classification->getOptions();check(options.contextParameters && options.ageFilter && options.minimumAge==18 && options.maximumAge==30 && options.countrySelection==1 && options.countryCount==2 && options.methods.first().points.value(1)==20,"filters and custom table edits persist without an extra submit");
    backend->select(event->getIDStr());backend->setClassificationScoring(0,{{"method",options.methods.first().id},{"multiplier",0.5},{"reference-year",2026}});
    check(event->scoringFor(classification).methodId==options.methods.first().id && event->scoringFor(classification).parameters.value("multiplier").toDouble()==0.5,"competition chooses the method and optional context parameters");
    backend->undo();check(event->scoringFor(classification).methodId.isEmpty(),"undo restores the previous competition scoring settings");backend->redo();check(event->scoringFor(classification).methodId==options.methods.first().id,"redo restores the scoring method");
    main.resize(1440,1000);workspace->setProperty("section",0);workspace->setProperty("editing",true);QTest::qWait(100);
    auto *inspector=item(scene(&manager)->rootObject(),"calendarInspector");check(inspector,"event inspector exists");inspector->setProperty("section",2);QTest::qWait(100);
    clickControl(scene(&manager),"calendar-scoring-method-0");QTest::keyClick(scene(&manager),Qt::Key_Home);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);
    check(event->scoringFor(classification).methodId.isEmpty(),"visible competition method selector chooses default");
    clickControl(scene(&manager),"calendar-scoring-method-0");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);
    check(event->scoringFor(classification).methodId==options.methods.first().id,"visible competition selector chooses the named method");
    clickControl(scene(&manager),"input-classification-parameter-multiplier-0");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);
    QTest::keyClicks(scene(&manager),QLocale().toString(.25,'f',2));QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(100);
    check(event->scoringFor(classification).parameters.value("multiplier").toDouble()==0.25,"visible context field commits the edited multiplier");
    backend->setClassificationScoring(0,{{"multiplier",0.5}});workspace->setProperty("section",1);QTest::qWait(80);classes->setProperty("editing",true);
    for(const auto size:{QSize(1440,900),QSize(1024,720),QSize(520,850),QSize(360,800)}){main.resize(size);QTest::qWait(150);presentation(&manager)->grab().save(output+QString("/classification-editor-%1.png").arg(size.width()));}
    // The real preset buttons must preserve the same definition and method IDs.
    const auto oldPresets=GlobalDatabase::get()->getGlobalCalendarPresets();
    QTimer::singleShot(200,[]{for(auto *w:QApplication::topLevelWidgets())if(auto *input=qobject_cast<QInputDialog *>(w);input && input->isVisible()){input->setTextValue("Classification preset test");input->accept();return;}check(false,"preset name prompt");});
    backend->preset(true);QTest::qWait(100);
    auto &presets=GlobalDatabase::get()->getEditableGlobalCalendarPresets();check(presets.size()==oldPresets.size()+1,"saving through the UI creates a calendar preset");
    auto &preset=presets.last();auto *pc=preset.getCalendarReference().getClassificationsReference().first();check(pc->getOptions().methods.first().points.value(1)==20 && preset.getCalendarReference().getCompetitionsReference().first()->scoringFor(pc).methodId==options.methods.first().id,"UI preset includes classification definitions and selected scoring method");
    QTimer::singleShot(200,[]{for(auto *w:QApplication::topLevelWidgets())if(auto *input=qobject_cast<QInputDialog *>(w);input && input->isVisible()){auto *combo=input->findChild<QComboBox *>();check(combo,"preset choice");combo->setCurrentIndex(combo->count()-1);input->accept();return;}check(false,"preset selection prompt");});
    backend->preset(false);QTest::qWait(150);
    auto *lc=calendar->getClassificationsReference().first();auto *le=calendar->getCompetitionsReference().first();check(lc->getOptions().toJson()==options.toJson() && le->getClassificationsReference().contains(lc) && le->scoringFor(lc).methodId==options.methods.first().id && le->scoringFor(lc).parameters.value("multiplier").toDouble()==0.5,"UI calendar preset loading restores classification options and context after reassigning IDs");
    GlobalDatabase::get()->setGlobalCalendarPresets(oldPresets);manager.hide();main.resize(1024,640);
    qInfo()<<"PASS: classification editor, dynamic alternative labels, per-event methods, parameter undo and real calendar preset save/load";
}
