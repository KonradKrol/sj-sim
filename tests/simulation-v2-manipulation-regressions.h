#pragma once
#include "../source/simulation-v2/JumpDraft.h"
#include "../source/UI/JumpManipulation/JumpManipulatorConfigWindow.h"

static void simulationV2ManipulationRegressions(MainWindow &main)
{
    qApp->setProperty("athleteTestStage","manipulation backend");
    const auto venue=HillProfile::create();QVector<::Wind> winds(6);for(auto &wind:winds){wind.setDirection(0);wind.setStrength(1);}
    JumpManipulator previous;previous.setDistanceBonus(2);previous.setDistanceRange({100,120});previous.setExactJudges({18,-1,19,-1,18.5});previous.setExactDSQProbability(500);
    SimulationJumpDraft draft;draft.load(previous,venue,winds,true);
    check(draft.canApply() && draft.values()["distanceActive"].toBool(),"new manipulation loads compatible pending settings");
    auto input=draft.manipulator().simulationInput();
    check(input.distance.bonus==2 && input.distance.range->minimum==100 && std::abs(*input.dsqProbability-.002)<1e-12 && !input.judges[1],"legacy ranges, partial judges and inverse DSQ chance keep their meaning");
    draft.setValue("distanceMin",130);check(!draft.canApply() && draft.values()["distanceMin"].toDouble()==130 && draft.message().contains(QString::fromUtf8("minimum")),"invalid interval remains editable with a clear error instead of rolling input back");
    draft.setValue("distanceMax",140);check(draft.canApply() && previous.getDistanceRange().first==100,"repairing interval is immediate and never mutates pending source");
    draft.reset();check(draft.changesCount()==0 && !draft.manipulator().simulationInput().active(),"clear removes all one-jump overrides");
    draft.setValue("qualityActive",true);draft.setValue("qualityBonus",8);draft.setJudge(0,19.3);
    check(draft.manipulator().simulationInput().quality.bonus==8 && *draft.manipulator().simulationInput().judges[0]==19.5,"new quality reaches the engine and typed judges snap to valid half points");
    draft.setValue("windActive",true);auto *weather=qobject_cast<SimulationWeatherSetup *>(draft.wind());
    weather->setStation(2,"direction",190);weather->setStation(2,"strength",2);
    const auto overridden=draft.manipulator();check(overridden.getExactSimulationWind()!=nullptr,"manual corridor editing produces an immutable wind field");
    SimulationJumpDraft reopened;reopened.load(overridden,venue,winds,true);
    for(double x:{0.,24.,70.,113.,150.}) {
        const auto a=overridden.getExactSimulationWind()->wind(x,0),b=reopened.manipulator().getExactSimulationWind()->wind(x,1000);
        check(std::abs(a.head-b.head)<1e-9 && std::abs(a.cross-b.cross)<1e-9,"reopening wind adjustment preserves continuous positions and strengths without resampling drift");
    }
    SimulationJumpDraft noDsq;noDsq.load(previous,venue,winds,false);noDsq.setValue("dsq",100);
    check(!noDsq.manipulator().simulationInput().dsqProbability,"rules-disabled DSQ cannot be introduced by the new editor");
    qApp->setProperty("athleteTestStage","manipulation configure");
    JumpManipulatorConfigWindow editor(&main);editor.configureSimulationV2(JumpManipulator(),venue,winds,true);editor.setProperty("manipulationAthlete","Test athlete");editor.setProperty("manipulationCountry","POL");
    QObject::connect(&editor,&JumpManipulatorConfigWindow::submitted,&editor,&QDialog::accept);
    qApp->setProperty("athleteTestStage","manipulation render sweep");
    render(&editor,"jump-manipulation",true);main.resize(1024,760);QTest::qWait(80);
    auto *view=scene(&editor);auto *workspace=item(view->rootObject(),"jumpManipulationWorkspace");auto *backend=editor.findChild<SimulationJumpDraft *>("jumpAdjustmentDraft");
    check(workspace && backend,"v2 manipulation opens its dedicated bounded workspace");
    qApp->setProperty("athleteTestStage","manipulation quality input");
    clickControl(view,"adjustment-quality");QTest::qWait(30);check(backend->values()["qualityActive"].toBool(),"quality override is explicitly optional");
    clickControl(view,"input-qualityBonus");QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"8,5");QTest::keyClick(view,Qt::Key_Tab);QTest::qWait(30);
    check(backend->values()["qualityBonus"].toDouble()==8.5,"quality accepts a typed decimal without reverting");
    qApp->setProperty("athleteTestStage","manipulation judges input");
    workspace->setProperty("section",1);QTest::qWait(50);clickControl(view,"overrideJudge-0");
    clickControl(view,"input-judge-0");QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"19.3");QTest::keyClick(view,Qt::Key_Tab);QTest::qWait(30);
    check(backend->values()["judges"].toList()[0].toDouble()==19.5,"judge input visibly normalizes a typed mark");
    qApp->setProperty("athleteTestStage","manipulation wind input");
    workspace->setProperty("section",2);QTest::qWait(50);clickControl(view,"manipulatedWind");QTest::qWait(50);
    check(item(view->rootObject(),"windStation-2"),"physical wind can be edited inline on the main slope graphic");
    editor.reject();main.resize(1024,640);
    QString error;auto athleteProfile=AthleteProfile::create();athleteProfile.personalBest=300;Jumper athlete;::Hill hill;
    auto hillProfile=venue;hillProfile.record=300;check(athlete.setSimulationProfile(athleteProfile,error) && hill.setSimulationProfile(hillProfile,error),"manipulation engine fixtures validate");
    CompetitionRules rules;rules.setHasDsq(true);rules.setHasWindCompensations(true);rules.setHasJudgesPoints(true);rules.setWindMeasurementType(CompetitionRules::FisApproximationWindMeasurement);
    SimulationV2::WindParameters calm;calm.references={{0,{0,0}},{200,{0,0}}};calm.temporalAmplitude=calm.spatialAmplitude=calm.gustAmplitude=0;
    SimulationV2::PrecipitationParameters rain;rain.kind=SimulationV2::PrecipitationKind::Rain;rain.intensity=2;rain.scenario=SimulationV2::PrecipitationScenario::Stable;
    const auto globalWeather=std::make_shared<SimulationV2::ContinuousWeather>(1,calm,rain);
    JumpSimulator simulator;JumpManipulator manipulation;simulator.setJumper(&athlete);simulator.setHill(&hill);simulator.setCompetitionRules(&rules);simulator.setManipulator(&manipulation);simulator.setGate(10);simulator.setCompetitionStartGate(10);simulator.setSimulationWeather(globalWeather,std::make_shared<SimulationV2::FieldWindMeasurement>(120,rules));
    const auto simulate=[&]{simulator.setSimulationTime(0);simulator.setSimulationSeed(12345);simulator.simulateJump();return simulator.getJumpData();};
    const auto baseline=simulate();SimulationV2::JumpManipulation changes;changes.quality.bonus=8;manipulation.setSimulationInput(changes);
    check(simulate().getDistance()>baseline.getDistance(),"quality adjustment changes the actual new-model flight");
    changes.quality.bonus=0;changes.precipitation.range=SimulationV2::ValueRange{6,6};changes.measuredWind.range=SimulationV2::ValueRange{-1.3,-1.3};changes.judges[0]=19.5;changes.landing=SimulationV2::LandingKind::Telemark;manipulation.setSimulationInput(changes);manipulation.setExactSimulationWind(overridden.getExactSimulationWind());
    const auto manipulated=simulate();
    check(manipulated.getAveragedWind()==-1.3 && manipulated.getJudges()[0]==19.5 && manipulated.getSimulationV2Data()["precipitation"].toDouble()==6 && manipulated.getSimulationV2Data()["precipitationKind"].toInt()==1,"physical wind overrides preserve rain while independent measurement, judges and intensity overrides apply");
    check(globalWeather->wind(70,0).head==0 && globalWeather->precipitation(0)==2,"one-jump overrides never modify the shared competition weather");
    changes.dsqProbability=1;manipulation.setSimulationInput(changes);check(simulate().getDSQ(),"direct 100 percent DSQ is effective when enabled in rules");rules.setHasDsq(false);check(!simulate().getDSQ(),"disabled DSQ wins over an explicit override");
    qApp->setProperty("athleteTestStage","manipulation contest workflow");
    const bool wasLive=GlobalAppSettings::get()->getLiveCompetition();GlobalAppSettings::get()->setLiveCompetition(false);
    CompetitionInfo info(&hill);rules.setRounds({RoundInfo(3,true)});rules.setHasDsq(false);info.setRules(rules);
    Jumper second=athlete,third=athlete;second.reassign();third.reassign();
    IndividualCompetitionManager manager;manager.setCompetitionInfo(&info);manager.setCompetitionRules(info.getRulesPointer());manager.setResults(&info.getResultsReference());
    manager.setActualRound(1);manager.setActualGate(10);manager.getRoundsStartingGatesReference().append(10);
    info.getRoundsKOGroupsReference().append(QVector<KOGroup>());manager.getRoundsKOGroupsReference().append(&info.getRoundsKOGroupsReference().first());
    manager.setFirstRoundJumpers({&athlete,&second,&third});manager.setupStartListStatusesForActualRound();manager.setActualStartListIndex(0);manager.setActualJumperToNextUnfinished();
    SimulationWeatherSetup contestWeather;contestWeather.setHill(hillProfile);contestWeather.setValue("variability",0);contestWeather.setValue("differences",0);contestWeather.setValue("strength",0);contestWeather.setValue("precipitationKind",1);contestWeather.setValue("precipitationScenario",1);contestWeather.setValue("precipitationIntensity",2);
    CompetitionManagerWindow contest(&manager,&main,true);contest.bindWeatherSetup(&contestWeather,12345);contest.show();QTest::qWait(100);
    const auto originalField=contestWeather.field();
    int opening=0,ticks=0;QTimer closeEditor;
    QObject::connect(&closeEditor,&QTimer::timeout,&contest,[&]{
        check(++ticks<400,"normal manipulation navigation cannot hang");
        for(auto *widget:QApplication::allWidgets())if(auto *dialog=qobject_cast<JumpManipulatorConfigWindow *>(widget);dialog && dialog->isVisible()) {
            auto *pending=dialog->findChild<SimulationJumpDraft *>("jumpAdjustmentDraft");check(pending,"normal competition opens the new manipulation model");
            ++opening;
            if(opening==1) {
                pending->setValue("qualityActive",true);pending->setValue("qualityBonus",8);
                pending->setValue("precipitationActive",true);pending->setValue("precipitationBounded",true);pending->setValue("precipitationMax",6);pending->setValue("precipitationMin",6);
                pending->setValue("windActive",true);auto *local=qobject_cast<SimulationWeatherSetup *>(pending->wind());for(int i=0;i<6;++i){local->setStation(i,"direction",0);local->setStation(i,"strength",2);}
                QMetaObject::invokeMethod(dialog,"on_pushButton_submit_2_clicked",Qt::DirectConnection);
            } else {
                check(pending->values()["qualityBonus"].toDouble()==8 && pending->values()["windActive"].toBool(),"reopening loads the accepted next-jump draft");
                pending->setValue("qualityBonus",20);dialog->reject();
            }
            closeEditor.stop();return;
        }
    });
    closeEditor.start(20);QMetaObject::invokeMethod(&contest,"on_pushButton_manipulateJump_clicked",Qt::DirectConnection);closeEditor.stop();check(opening==1,"normal action applies a draft");
    closeEditor.start(20);QMetaObject::invokeMethod(&contest,"on_pushButton_manipulateJump_clicked",Qt::DirectConnection);closeEditor.stop();check(opening==2,"normal action supports cancelling a later edit");
    auto *firstJumper=manager.getActualJumper();QMetaObject::invokeMethod(&contest,"on_pushButton_jump_clicked",Qt::DirectConnection);
    const auto firstJump=info.getResultsReference().getResultOfIndividualJumper(firstJumper)->getJumps().last();
    auto *nextJumper=manager.getActualJumper();QMetaObject::invokeMethod(&contest,"on_pushButton_jump_clicked",Qt::DirectConnection);
    const auto nextJump=info.getResultsReference().getResultOfIndividualJumper(nextJumper)->getJumps().last();
    check(firstJump.getSimulationV2Data()["quality"].toObject()["manipulation"].toDouble()==8 && firstJump.getSimulationV2Data()["precipitation"].toDouble()==6,"only the accepted correction reaches the next real competition jump");
    check(nextJump.getSimulationV2Data()["quality"].toObject()["manipulation"].toDouble()==0 && nextJump.getSimulationV2Data()["precipitation"].toDouble()==2 && nextJump.getAveragedWind()==0,"the following jumper restores normal quality, rain and shared wind");
    check(contestWeather.field()==originalField && contest.getSimulationSeconds()==60,"local edits and cancellation leave the shared weather unchanged while jumps advance the clock");
    contest.reject();GlobalAppSettings::get()->setLiveCompetition(wasLive);
    const auto widgetMode=qgetenv("SJ_WIDGETS_UI");qputenv("SJ_WIDGETS_UI","1");
    {
        JumpManipulatorConfigWindow nativeEditor(&main);nativeEditor.setProperty("sjQuickUtility",true);nativeEditor.configureSimulationV2(overridden,venue,winds,true);
        auto *native=nativeEditor.findChild<QWidget *>("nativeJumpDraftEditor");check(native,"optional Widgets presentation uses the same new-model draft");
        nativeEditor.show();QTest::qWait(50);
        check(native->isVisible() && !nativeEditor.findChild<QLabel *>("label_title")->isVisible(),"native presentation shows the new controls and hides obsolete controls");
        auto *quality=native->findChild<QDoubleSpinBox *>("nativeAdjustment-qualityBonus");check(quality && quality->value()==8,"native new-model inputs load the pending quality correction");
        quality->setValue(12);check(nativeEditor.getJumpManipulatorFromInputs().simulationInput().quality.bonus==12,"native correction and new engine share the same validated model");
        nativeEditor.reject();
    }
    if(widgetMode.isNull())qunsetenv("SJ_WIDGETS_UI");else qputenv("SJ_WIDGETS_UI",widgetMode);
    qInfo()<<"PASS: manipulation draft, validation, partial overrides, typed UI, continuous wind and independent one-jump engine controls";
}
