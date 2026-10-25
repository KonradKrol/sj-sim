#pragma once

static void competitionPresentationRegressions() {
    Jumper a("A","One","POL"),b("B","Two","NOR"),c("C","Three","AUT");Hill hill;
    CompetitionInfo info(&hill);CompetitionRules rules;RoundInfo second(3,true);second.setPreviousRoundsWeights({.5});rules.setRounds({RoundInfo(3,true),second});info.setRules(rules);
    auto &results=info.getResultsReference();
    auto add=[&](Jumper *jumper,double points){JumpData jump(jumper,&hill);jump.setPoints(points);results.addJump(jumper,jump);};
    add(&a,100);add(&b,200);add(&c,80);
    auto *own=results.getResultOfIndividualJumper(&a);
    auto rivals=[&]{QVector<const CompetitionSingleResult *> field;for(const auto &result:results.getResultsReference())field.append(&result);return field;};
    check(QuickRaceStats::roundStartGap(*own,rivals(),2).value("gap").toDouble()==50,"pre-jump leader gap applies the new round's weights");
    add(&b,80);own=results.getResultOfIndividualJumper(&a);
    check(QuickRaceStats::roundStartGap(*own,rivals(),2).value("gap").toDouble()==50,"round-start gap excludes the leader's extra current-round jump");
    check(QuickRaceStats::currentLeaderPreviousGap(*own,rivals(),2).value("gap").toDouble()==50,"current leader comparison excludes that leader current-series jump");
    check(QuickRaceStats::roundStartGap(*own,{results.getResultOfIndividualJumper(&c)},2).value("gap").toDouble()==-10,"KO group gap can expose a negative lead independent of the overall leader");
    check(QuickRaceStats::roundStartGap(*own,rivals(),1).isEmpty(),"first round has no inherited-points gap");
    add(&a,60);add(&c,100);RoundInfo third(3,true);third.setPreviousRoundsWeights({.25,.5});auto rounds=info.getRulesPointer()->getRounds();rounds.append(third);info.getRulesPointer()->setRounds(rounds);
    own=results.getResultOfIndividualJumper(&a);
    check(QuickRaceStats::roundStartGap(*own,rivals(),3).value("gap").toDouble()==35,"third-series gap uses each completed round's new weight");
    add(&b,200);own=results.getResultOfIndividualJumper(&a);
    check(QuickRaceStats::roundStartGap(*own,rivals(),3).value("gap").toDouble()==35,"another current-series attempt cannot inflate the inherited gap");
    check(QuickRaceStats::roundStartGap(*own,{results.getResultOfIndividualJumper(&c)},3).value("gap").toDouble()==15,"KO gap uses the same completed-round boundary as the overall gap");
    Team ta("POL"),tb("NOR");CompetitionInfo teamInfo(&hill);CompetitionRules teamRules;teamRules.setCompetitionType(CompetitionRules::Team);teamRules.setJumpersInTeamCount(2);teamRules.setRounds({RoundInfo(2,true),second});teamInfo.setRules(teamRules);
    auto &teamResults=teamInfo.getResultsReference();
    const auto teamJump=[&](Team *team,double points){JumpData jump(&a,&hill);jump.setPoints(points);teamResults.addJump(team,jump);};
    teamJump(&ta,100);teamJump(&ta,80);teamJump(&tb,90);teamJump(&tb,70);teamJump(&tb,150);
    check(QuickRaceStats::roundStartGap(*teamResults.getResultOfTeam(&ta),{teamResults.getResultOfTeam(&tb)},2).value("gap").toDouble()==-10,"team gaps exclude extra members' jumps from the current series");
    info.getRulesPointer()->getRoundsReference()[1].setResetPoints(true);info.getRulesPointer()->getRoundsReference()[1].setPreviousRoundsWeights({});
    check(QuickRaceStats::roundStartGap(*own,rivals(),2).isEmpty() && QuickRaceStats::currentLeaderPreviousGap(*own,rivals(),2).isEmpty(),"reset points suppress both previous and current inherited gaps");
    {
        CompetitionInfo changed(&hill);CompetitionRules weighted;weighted.setRounds({RoundInfo(3,true),second});changed.setRules(weighted);auto &scores=changed.getResultsReference();
        auto jump=[&](Jumper *jumper,double points){JumpData data(jumper,&hill);data.setPoints(points);scores.addJump(jumper,data);};
        jump(&a,100);jump(&b,200);jump(&c,80);
        QVector<const CompetitionSingleResult *> before;for(const auto &entry:scores.getResultsReference())before.append(&entry);
        check(QuickRaceStats::currentLeaderPreviousGap(*scores.getResultOfIndividualJumper(&a),before,2).isEmpty(),"first athlete in a series has no lead or deficit message");
        jump(&b,80);jump(&c,300);
        QVector<const CompetitionSingleResult *> field;for(const auto &entry:scores.getResultsReference())field.append(&entry);
        const auto gap=QuickRaceStats::currentLeaderPreviousGap(*scores.getResultOfIndividualJumper(&a),field,2);
        check(gap.value("gap").toDouble()==-10,"compare against the actual new leader but only their weighted points from before the current series");
        scores.getResultsReference().clear();jump(&a,100);jump(&b,100);jump(&c,80);jump(&b,50);field.clear();for(const auto &entry:scores.getResultsReference())field.append(&entry);
        check(QuickRaceStats::currentLeaderPreviousGap(*scores.getResultOfIndividualJumper(&a),field,2).isEmpty(),"equal weighted previous-series points never display a zero-point deficit");
        changed.getRulesPointer()->getRoundsReference()[1].setResetPoints(true);changed.getRulesPointer()->getRoundsReference()[1].setPreviousRoundsWeights({});
        check(QuickRaceStats::currentLeaderPreviousGap(*scores.getResultOfIndividualJumper(&a),field,2).isEmpty(),"reset hides the single leader comparison");
    }
    results.getResultsReference().clear();add(&a,100);add(&b,100);add(&c,90);results.sortInDescendingOrder();
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&info);manager.setCompetitionRules(info.getRulesPointer());manager.setResults(&results);manager.setActualRound(1);manager.setFirstRoundJumpers({&a,&b,&c});manager.setupStartListStatusesForActualRound();
    ResultsTableModel model(CompetitionRules::Individual,&results,&manager);model.setStartListStatuses(&manager.getStartListStatusesReference());
    const QColor gold(255,247,205),bronze(255,232,215);
    check(model.index(0,0).data(Qt::BackgroundRole).value<QColor>()!=gold,"provisional first place has no podium color");
    for(auto &status:manager.getStartListStatusesReference())status.setJumpStatus(StartListCompetitorStatus::Finished);
    check(!manager.checkCompetitionEnd() && model.index(0,0).data(Qt::BackgroundRole).value<QColor>()!=gold,"finishing an intermediate series does not show a podium");
    info.setExceptionalRoundsCount(1);
    check(manager.checkCompetitionEnd() && model.index(0,0).data(Qt::BackgroundRole).value<QColor>()==gold && model.index(1,0).data(Qt::BackgroundRole).value<QColor>()==gold && model.index(2,0).data(Qt::BackgroundRole).value<QColor>()==bronze,"final podium follows tied places 1,1,3 rather than row indexes");
    manager.setActualRound(2);for(auto &status:manager.getStartListStatusesReference())status.setJumpStatus(StartListCompetitorStatus::Unfinished);
    check(manager.checkCompetitionEnd() && model.index(1,0).data(Qt::BackgroundRole).value<QColor>()==gold,"cancellation ending the competition retains the previous series' tied podium");
    qInfo()<<"PASS: weighted round-start and KO gaps, reset suppression and final-only tied podium";
}

static void simulationOnboardingRegressions(MainWindow &main) {
    auto *database=GlobalDatabase::get();const auto snapshot=database->packsSnapshot();
    const auto original=database->activePackId(DataPack::Jumpers);
    DataPack pack=*database->activePack(DataPack::Jumpers);pack.name="Onboarding test";pack.jumpers.resize(2);
    const auto custom=database->addPack(pack);database->selectPack(original);
    {
        NewSimulationSaveConfigurationWindow dialog({},&main);render(&dialog,"new-simulation-packs",true);
        check(item(scene(&dialog)->rootObject(),"newSimulationPage"),"simulation creation uses its dedicated single-screen layout");
        auto *choice=dialog.findChild<QComboBox *>("simulationPack0");choice->setCurrentIndex(choice->findData(custom));QTest::qWait(80);
        check(dialog.selectedPack(DataPack::Jumpers).jumpers.size()==2 && dialog.findChild<QLabel *>("simulationPackCount0")->text().contains('2'),"pack selection updates its count without editing the source data");
        check(database->activePackId(DataPack::Jumpers)==original,"new simulation selections do not change the active global database");
        dialog.findChild<QLineEdit *>("lineEdit_name")->setText("Pack test");dialog.findChild<QCheckBox *>("newSeasonDates")->setChecked(true);
        QMetaObject::invokeMethod(&dialog,"on_pushButton_submit_clicked");check(dialog.result()==QDialog::Accepted && dialog.getDatesEnabledFromInput(),"one-screen creation accepts selected packs and dates");
    }
    {
        NewSimulationSaveConfigurationWindow dialog({},&main);check(dialog.selectedPack(DataPack::Jumpers).id==custom,"accepted pack choices are remembered for the next creation");dialog.reject();
    }
    QSettings().remove("newSimulation");QString error;check(database->restorePacks(snapshot,error),"restore onboarding database fixture");
    SimulationSave save;save.setName("Empty onboarding");save.setHills({new Hill(database->getEditableGlobalHills().first())});save.setCompetitionRules({database->getEditableCompetitionRules().first()});Season season;season.setSeasonNumber(2026);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.updateNextCompetitionIndex();
    SimulationSaveManagerWindow manager(&save,&main);manager.show();QTest::qWait(120);auto *bridge=manager.findChild<QuickBridge *>();bridge->refresh();QTest::qWait(80);
    const int rootId=bridge->root()->values().value("id").toInt();manager.findChild<QTabWidget *>("toolBox")->setCurrentIndex(3);QTest::qWait(60);bridge->edit(rootId,0);QTest::qWait(80);
    check(save.getActualSeason()->getCalendarsReference().isEmpty(),"new simulation starts without a hidden calendar");
    bridge->activate(id(bridge,"pushButton_competitionConfig"));QTest::qWait(180);bridge->refresh();
    check(save.getActualSeason()->getCalendarsReference().size()==1 && bridge->root()->values().value("value").toInt()==4,"overview create-calendar action creates and navigates even when the native calendar tab was already selected");
    auto *calendar=bridge->findChild<QuickCalendar *>();check(calendar && calendar->calendarIndex()==0 && calendar->newCalendar(),"new calendar is selected and its name is editable");
    render(&manager,"empty-calendar-onboarding",true);
    main.resize(1024,640);QTest::qWait(100);
    auto *action=item(scene(&manager)->rootObject(),"calendarEmptyAction");check(action && action->isVisible(),"empty calendar has a visible primary action inside its viewport");
    const auto bounds=action->mapRectToItem(scene(&manager)->rootObject(),action->boundingRect());check(bounds.top()>=0 && bounds.bottom()<=scene(&manager)->height(),"empty-calendar action is not clipped below its available space");
    clickControl(scene(&manager),"calendarEmptyAction");check(calendar->events().size()==1,"empty-calendar action creates the first editable competition");
    calendar->undo();QTest::qWait(80);bridge->edit(rootId,0);QTest::qWait(80);
    auto *addFromOverview=manager.findChild<QPushButton *>("pushButton_competitionConfig");check(addFromOverview->text().contains("Dodaj"),"empty active calendar offers adding a competition in overview");
    bridge->activate(id(bridge,"pushButton_competitionConfig"));QTest::qWait(160);bridge->refresh();QTest::qWait(80);
    auto *workspace=item(scene(&manager)->rootObject(),"calendarWorkspace");
    check(calendar->events().size()==1 && calendar->selection().value("count").toInt()==1 && workspace && workspace->property("editing").toBool(),"overview adds and selects the competition and opens its editor, rather than only switching tabs");
    manager.reject();main.resize(1024,640);
    qInfo()<<"PASS: simplified simulation setup, pack counts and preferences, dates, and direct calendar creation";
}
