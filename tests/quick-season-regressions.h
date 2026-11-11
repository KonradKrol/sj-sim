#pragma once
static void quickSeasonRegressions(MainWindow &main){
    SimulationSave save;save.setEngineVersion(2);save.setName("Quick season regression");save.setAutosaveEnabled(false);QString error;
    for(int i=0;i<4;++i){auto profile=AthleteProfile::create();profile.name="Quick";profile.surname=QString::number(i);profile.personalBest=500;auto *athlete=new Jumper;check(athlete->setSimulationProfile(profile,error),"quick season athlete");save.getJumpersReference().append(athlete);}
    auto *hill=new Hill;auto venue=HillProfile::create();venue.name="Quick hill";venue.record=500;check(hill->setSimulationProfile(venue,error),"quick season hill");save.setHills({hill});
    CompetitionRules rules;rules.setRounds({RoundInfo(4,true)});rules.setHasDsq(false);save.setCompetitionRules({rules});
    auto *calendar=new SeasonCalendar("Quick calendar");auto *first=new CompetitionInfo(hill),*second=new CompetitionInfo(hill);first->setRules(rules);second->setRules(rules);first->setScheduledDate(QDate(2026,11,1));second->setScheduledDate(QDate(2026,11,2));calendar->setCompetitions({first,second});
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);season.setDatesEnabled(true);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.updateNextCompetitionIndex();save.fixJumpersFormInstabilities();
    SimulationSaveManagerWindow window(&save,&main);window.fillNextCompetitionInformations();window.show();QTest::qWait(100);
    const auto originalAuto=QSettings().value("quickUi/raceAutomaticGate");
    QTimer timeout;timeout.setSingleShot(true);QObject::connect(&timeout,&QTimer::timeout,[]{check(false,"quick season simulation must not wait for modal dialogs");});timeout.start(20000);
    auto *button=window.findChild<QPushButton *>("quickSimulateNextCompetition");check(button,"quick season action exists");button->click();timeout.stop();
    check(first->getPlayed() && first->getResultsReference().getResultsReference().size()==4,"quick simulation finishes and records every athlete");
    check(save.getNextCompetition()==second,"quick simulation advances season");
    check(QSettings().value("quickUi/raceAutomaticGate")==originalAuto,"forced quick auto gate does not change the user's preference");
    check(item(scene(&window)->rootObject(),"seasonOverview"),"quick simulation returns to overview");
    window.hide();
    Team pol,nor;pol.setCountryCode("POL");nor.setCountryCode("NOR");
    pol.setJumpers({save.getJumpersReference()[0],save.getJumpersReference()[1]});nor.setJumpers({save.getJumpersReference()[2],save.getJumpersReference()[3]});
    CompetitionInfo teamInfo(hill);CompetitionRules teamRules;teamRules.setCompetitionType(CompetitionRules::Team);teamRules.setJumpersInTeamCount(2);teamRules.setRounds({RoundInfo(2,true)});teamRules.setHasDsq(false);teamInfo.setRules(teamRules);
    TeamCompetitionManager manager;manager.setCompetitionInfo(&teamInfo);manager.setCompetitionRules(teamInfo.getRulesPointer());manager.setResults(&teamInfo.getResultsReference());manager.setActualRound(1);manager.setActualGroup(1);manager.setActualGate(10);manager.getRoundsStartingGatesReference().append(10);manager.getRoundsTeamsReference().append(QVector<Team *>{&pol,&nor});manager.setupStartListStatusesForActualRound(true);manager.setActualStartListIndex(0);manager.setActualJumperToNextUnfinished();
    const bool previousLive=GlobalAppSettings::get()->getLiveCompetition();GlobalAppSettings::get()->setLiveCompetition(false);
    CompetitionManagerWindow race(&manager,&main,true);race.show();QTest::qWait(100);
    for(int i=0;i<2;++i){check(QMetaObject::invokeMethod(&race,"on_pushButton_jump_clicked",Qt::DirectConnection),"team jump executes");QTest::qWait(40);}
    check(manager.checkGroupEnd() && !manager.checkRoundEnd(),"first team group ends before the round");
    race.findChild<QuickBridge *>()->refresh();QTest::qWait(50);
    auto *raceView=scene(&race);auto *primary=item(raceView->rootObject(),"racePrimaryAction");check(primary && primary->property("enabled").toBool() && !primary->property("jumping").toBool(),"primary action switches to next group");
    clickControl(raceView,"racePrimaryAction");QTest::qWait(40);check(manager.getActualGroup()==2,"next group opens from the primary action");
    race.hide();GlobalAppSettings::get()->setLiveCompetition(previousLive);
    qInfo()<<"PASS: quick season simulation records results and advances without opening contest or result pages";
}
