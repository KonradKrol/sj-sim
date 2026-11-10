#pragma once

static void livePresentationRegressions(MainWindow &main) {
    QVector<Jumper> athletes;athletes.reserve(8);QVector<Jumper *> pointers;QString error;
    for(int i=0;i<8;++i) {
        auto profile=AthleteProfile::create();profile.name="Live";profile.surname=QString::number(i);profile.country="POL";profile.personalBest=500;
        Jumper jumper;check(jumper.setSimulationProfile(profile,error),"live athlete fixture");athletes.append(jumper);pointers.append(&athletes.last());
    }
    auto profile=HillProfile::create();profile.name="Live regression";profile.record=500;
    Hill hill;check(hill.setSimulationProfile(profile,error),"live hill fixture");
    CompetitionInfo info(&hill);CompetitionRules rules;rules.setCompetitionType(CompetitionRules::Individual);
    rules.setHasDsq(false);rules.setHasJudgesPoints(true);rules.setHasWindCompensations(true);rules.setHasGateCompensations(true);
    rules.setWindMeasurementType(CompetitionRules::FisApproximationWindMeasurement);rules.setWindAverageCalculatingType(WindsCalculator::Original);
    rules.setRounds({RoundInfo(8,true),RoundInfo(4,true)});info.setRules(rules);
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&info);manager.setCompetitionRules(info.getRulesPointer());manager.setResults(&info.getResultsReference());
    manager.setActualRound(1);manager.setActualGate(10);manager.getRoundsStartingGatesReference().append(10);
    info.getRoundsKOGroupsReference().append(QVector<KOGroup>());manager.getRoundsKOGroupsReference().append(&info.getRoundsKOGroupsReference().first());
    manager.setFirstRoundJumpers(pointers);manager.setupStartListStatusesForActualRound();manager.setActualStartListIndex(0);manager.setActualJumperToNextUnfinished();
    SimulationWeatherSetup setup;setup.setHill(profile);setup.applyPreset(2);
    CompetitionManagerWindow window(&manager,&main,true);window.bindWeatherSetup(&setup,654321);window.show();QTest::qWait(200);
    auto *bridge=window.findChild<QuickBridge *>();check(bridge && scene(&window)->status()==QQuickWidget::Ready,"real live competition scene loads");
    GlobalAppSettings::get()->getJumpDataInfoChoiceReference()=JumpDataInfoChoice();
    QTimer dialogs;QObject::connect(&dialogs,&QTimer::timeout,&window,[]{for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()) {
        auto *answer=box->button(QMessageBox::Yes);if(!answer)answer=box->button(QMessageBox::Ok);if(answer)answer->click();else box->accept();
    }});dialogs.start(20);
    struct Snapshot {int stage; qint64 time; QVariantMap summary; int mask; bool finalWind;};
    QVector<Snapshot> stages;QElapsedTimer clock;
    QObject::connect(&window,&CompetitionManagerWindow::liveStageChanged,&window,[&](int stage){
        bridge->refresh();stages.append({stage,clock.isValid()?clock.elapsed():0,bridge->jumpSummary(),window.property("liveJudgeMask").toInt(),window.property("liveWindFinal").toBool()});
    });
    const auto jump=[&](int seconds){
        QSettings().setValue("quickUi/liveFlightSeconds",seconds);stages.clear();clock.start();
        check(QMetaObject::invokeMethod(&window,"on_pushButton_jump_clicked",Qt::DirectConnection),"execute real jump");
        bridge->refresh();return clock.elapsed();
    };
    window.findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(true);
    const auto elapsed=jump(10);check(elapsed>=9500 && elapsed<12500,"ten-second presentation duration");
    bool intermediate=false,landing=false,finalWind=false,completed=false;int previousMask=0,reveals=0;qint64 finalAt=0;
    for(const auto &state:stages) {
        const auto &summary=state.summary;
        if(state.stage==1 && summary.contains("wind")) {
            intermediate=true;check(state.time>=2200 && state.time<3400 && !summary.contains("distance"),"intermediate wind precedes landing");
        }
        if(state.stage==2) {
            landing=true;check(state.time>=4500 && summary.contains("distance") && summary.contains("landing") && !summary.contains("points"),"landing and distance appear together before points");
        }
        if(state.stage==4) {
            const auto marks=summary.value("judges").toList();check(marks.size()==5,"five reserved judge slots throughout reveal");
            int discarded=0;for(int i=0;i<marks.size();++i) {
                const auto mark=marks[i].toMap();check(mark.value("revealed").toBool()==bool(state.mask&(1<<i)),"judge stays in its assigned slot");discarded+=mark.value("discarded").toBool();
            }
            if(state.mask!=previousMask) {
                const int added=state.mask^previousMask;check((state.mask&previousMask)==previousMask && added && !(added&(added-1)),"one additional judge revealed at a time");++reveals;previousMask=state.mask;
            }
            check(discarded==(state.finalWind?2:0),"discard both extreme marks after all notes");
            check(!summary.contains("points") && !summary.contains("totalPoints"),"score remains hidden until final second");
            if(state.finalWind){finalWind=true;finalAt=state.time;check(state.mask==31 && state.time>=8500,"final wind follows five notes");}
        }
        if(state.stage==5){completed=true;check(state.time-finalAt>=900 && summary.contains("points"),"last second remains reserved for final result");}
    }
    check(intermediate && landing && finalWind && completed && reveals==5,"complete progressive sequence observed");
    QTest::qWait(80);scene(&window)->grabFramebuffer();presentation(&window)->grab().save(output+"/live-presentation-final.png");
    const auto shorter=jump(3);qint64 shortFinal=0,shortComplete=0;
    for(const auto &state:stages){if(state.finalWind && state.stage==4)shortFinal=state.time;if(state.stage==5)shortComplete=state.time;}
    check(shorter>=2700 && shorter<4500 && shortComplete-shortFinal>=900,"short presentation keeps the fixed final second");
    auto *activeRules=info.getRulesPointer();activeRules->setHasJudgesPoints(false);activeRules->setHasWindCompensations(false);activeRules->setHasGateCompensations(false);
    const auto withoutJudges=jump(3);check(withoutJudges>=1900 && withoutJudges<3300,"presentation without judges skips their window");
    const auto summary=bridge->jumpSummary();for(const auto *key:{"judges","judgesPoints","windPoints","gatePoints","compensation"})check(!summary.contains(key),"rules hide disabled jump information");
    activeRules->setHasDsq(true);manager.setBaseDSQProbability(1);
    check(jump(10)<1500 && bridge->jumpSummary().value("distance").toString()=="DSQ","disqualification bypasses progressive presentation");
    activeRules->setHasDsq(false);clock.restart();window.skipCurrentJump();bridge->refresh();
    check(clock.elapsed()<1500 && bridge->jumpSummary().value("distance").toString()=="DNS","skipped jump is immediate and labelled DNS");
    // A modal must pause the timer as soon as opening begins, before its fade completes.
    window.findChild<QCheckBox *>("checkBox_liveCompetition")->setChecked(false);
    QSettings().setValue("quickUi/liveWaitSeconds",1);bridge->setAutomaticPlayback(true);QTest::qWait(900);
    auto *toggle=item(scene(&window)->quickWindow()->contentItem(),"raceLiveToggle");check(toggle && toggle->isVisible(),"live settings toolbar is visible");
    check(QMetaObject::invokeMethod(toggle,"clicked"),"open actual live settings popup");
    check(window.property("liveConfigurationOpen").toBool(),"opening settings immediately pauses automatic jumps");
    auto *before=manager.getActualJumper();QTest::qWait(1400);check(manager.getActualJumper()==before,"no automatic jump while settings are open");
    auto *popup=scene(&window)->rootObject()->findChild<QObject *>("raceLiveSettings");check(popup,"settings popup is accessible");
    check(QMetaObject::invokeMethod(popup,"close"),"close live settings");QTest::qWait(1600);check(manager.getActualJumper()!=before,"automatic jumps resume after closing settings");
    QTest::qWait(5500);check(manager.checkRoundEnd() && !window.automaticPlayback(),"automatic jumps stop at the end of the round");
    window.hide();GlobalAppSettings::get()->setLiveCompetition(false);
    qInfo()<<"PASS: actual live timing, stable judge slots, rule visibility, settings pause and automatic round boundary";
}
