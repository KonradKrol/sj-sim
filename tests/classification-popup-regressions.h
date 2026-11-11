#pragma once
static void classificationPopupRegressions(MainWindow &main){
    SimulationSave save;save.setEngineVersion(2);save.setName("Popup regression");save.setAutosaveEnabled(false);QString error;
    for(int i=0;i<4;++i){auto profile=AthleteProfile::create();profile.name="Popup";profile.surname=QString::number(i);auto *athlete=new Jumper;check(athlete->setSimulationProfile(profile,error),"popup athlete");save.getJumpersReference().append(athlete);}
    auto *hill=new Hill;auto venue=HillProfile::create();venue.name="Popup hill";check(hill->setSimulationProfile(venue,error),"popup hill");save.setHills({hill});
    CompetitionRules rules;rules.setRounds({RoundInfo(4,true)});rules.setHasDsq(false);save.setCompetitionRules({rules});
    auto *calendar=new SeasonCalendar("Popup calendar");auto *event=new CompetitionInfo(hill);event->setRules(rules);calendar->setCompetitions({event});
    auto *classification=new Classification;classification->setName("PŚ");classification->setClassificationType(Classification::Individual);calendar->setClassifications({classification});
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.updateNextCompetitionIndex();save.fixJumpersFormInstabilities();
    CompetitionConfigWindow config(CompetitionConfigWindow::SeasonCompetition,&main,&save);config.show();QTest::qWait(150);
    auto *bridge=config.findChild<QuickBridge *>();check(bridge,"classification popup bridge");
    bridge->edit(bridge->root()->values().value("id").toInt(),1);QTest::qWait(100);
    auto *view=scene(&config);auto *choice=item(view->rootObject(),"choice-comboBox_classification");check(choice,"classification dropdown on start-list page");
    QPointer<QObject> stable(choice);clickControl(view,"choice-comboBox_classification");QTest::qWait(50);
    auto *popup=choice->property("popup").value<QObject *>();check(popup && popup->property("visible").toBool(),"classification popup opens");
    for(int i=0;i<8;++i){bridge->refresh();QTest::qWait(40);check(stable && item(view->rootObject(),"choice-comboBox_classification")==stable.data(),"refresh preserves the open dropdown instance");}
    check(popup->property("visible").toBool(),"popup stays open during refresh");QMetaObject::invokeMethod(popup,"close");config.hide();
    qInfo()<<"PASS: opening classification dropdown preserves delegates and popup across screen refreshes";
}
