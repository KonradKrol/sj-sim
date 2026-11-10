#pragma once

class AutoGateTestWeather final:public SimulationV2::WeatherField {
    double head;
public:
    explicit AutoGateTestWeather(double value):head(value){}
    SimulationV2::Wind wind(double,double)const override{return {head,0};}
    double precipitation(double)const override{return 0;}
};
static void raceAutoGateRegressions(MainWindow &main){
    const bool oldLive=GlobalAppSettings::get()->getLiveCompetition();GlobalAppSettings::get()->setLiveCompetition(false);
    const auto oldPreference=QSettings().value("quickUi/raceAutomaticGate");QSettings().setValue("quickUi/raceAutomaticGate",false);
    QVector<Jumper> athletes;athletes.reserve(8);QVector<Jumper *> pointers;QString error;
    for(int i=0;i<8;++i){auto p=AthleteProfile::create();p.name="Gate";p.surname=QString::number(i);p.personalBest=500;Jumper athlete;check(athlete.setSimulationProfile(p,error),"auto gate athlete");athletes.append(athlete);pointers.append(&athletes.last());}
    auto profile=HillProfile::create();profile.name="Auto gate hill";profile.record=500;Hill hill;check(hill.setSimulationProfile(profile,error),"auto gate hill");
    CompetitionInfo info(&hill);CompetitionRules rules;rules.setHasDsq(false);rules.setRounds({RoundInfo(8,true),RoundInfo(4,true)});rules.setWindMeasurementType(CompetitionRules::FisApproximationWindMeasurement);info.setRules(rules);
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&info);manager.setCompetitionRules(info.getRulesPointer());manager.setResults(&info.getResultsReference());manager.setActualRound(1);manager.setActualGate(10);manager.getRoundsStartingGatesReference().append(10);
    info.getRoundsKOGroupsReference().append(QVector<KOGroup>());manager.getRoundsKOGroupsReference().append(&info.getRoundsKOGroupsReference().first());manager.setFirstRoundJumpers(pointers);manager.setupStartListStatusesForActualRound();manager.setActualStartListIndex(0);manager.setActualJumperToNextUnfinished();
    CompetitionManagerWindow window(&manager,&main,true);window.setSimulationWeather(std::make_shared<AutoGateTestWeather>(0),123);window.show();QTest::qWait(150);
    QTimer dialogs;QObject::connect(&dialogs,&QTimer::timeout,&window,[]{for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){auto *button=box->button(QMessageBox::Ok);if(!button)button=box->button(QMessageBox::Yes);if(button)button->click();else box->accept();}});dialogs.start(20);
    auto *gate=window.findChild<QSpinBox *>("spinBox_actualGate");window.suggestCurrentGate(false);const int target=manager.getActualGate();check(target>1 && target<197,"auto gate fixture leaves room for threshold cases");
    const auto jump=[&]{auto *athlete=manager.getActualJumper();check(QMetaObject::invokeMethod(&window,"on_pushButton_jump_clicked",Qt::DirectConnection),"execute auto gate jump");QTest::qWait(20);auto *result=manager.getResults()->getResultOfIndividualJumper(athlete);check(result && !result->getJumps().isEmpty(),"jump recorded");return result->getJumps().last();};
    gate->setValue(target+3);check(jump().getGate()==target+3,"disabled auto gate preserves manual gate");
    clickControl(scene(&window),"raceAutomaticGate");check(QSettings().value("quickUi/raceAutomaticGate").toBool(),"race toggle persists the application-wide choice");
    gate->setValue(target+1);check(jump().getGate()==target+1,"difference of one above recommendation is preserved");
    gate->setValue(target-1);check(jump().getGate()==target-1,"difference of one below recommendation is preserved");
    gate->setValue(target+3);check(jump().getGate()==target && manager.getActualGate()==target,"difference greater than one applies the recommended gate before jumping");
    gate->setValue(target+3);window.lowerCoachGateOne();const auto coached=jump();check(coached.getGate()==target-1 && coached.getHasCoachGate(),"automatic jury gate preserves the coach's one-gate reduction");
    JumpManipulator manipulation;manipulation.setExactSimulationWind(std::make_shared<AutoGateTestWeather>(3));window.setCurrentInputJumpManipulator(manipulation);
    window.suggestCurrentGate(false);const int windTarget=manager.getActualGate();check(windTarget<target,"gate recommendation respects the one-jump headwind override");
    gate->setValue(target+3);const auto overridden=jump();check(overridden.getGate()==windTarget && std::abs(overridden.getAveragedWind()-3)<.01,"auto gate and jump use the temporary wind");
    check(!window.getCurrentInputJumpManipulator().getExactSimulationWind() && std::abs(manager.property("predictedMeasuredWind").toDouble())<1e-9,"wind manipulation is consumed without modifying the contest field");
    check(jump().getGate()==target,"following jump returns to the contest weather and its recommended gate");
    // Finished participants remain part of the recommendation population.
    auto stronger=*athletes.first().getSimulationProfile();stronger.overall=100;stronger.form=100;
    check(athletes.first().setSimulationProfile(stronger,error),"strongest finished athlete profile");
    window.suggestCurrentGate(false);check(manager.getActualGate()<target,"finished strongest participant still influences the gate");
    jump();check(manager.checkRoundEnd(),"fixture completes round one");
    gate->setValue(190);
    window.findChild<QPushButton *>("pushButton_goToNext")->click();QTest::qWait(100);
    check(manager.getActualRound()==2 && manager.getActualGate()<190,"automatic gate applies before the first jump of a new round");
    window.hide();GlobalAppSettings::get()->setLiveCompetition(oldLive);if(oldPreference.isValid())QSettings().setValue("quickUi/raceAutomaticGate",oldPreference);else QSettings().remove("quickUi/raceAutomaticGate");
    qInfo()<<"PASS: automatic gate threshold, persisted choice, coach reduction and one-jump wind isolation";
}
