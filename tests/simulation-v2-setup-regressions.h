#pragma once

static SimulationV2::CatalogPaths isolatedCatalogPaths(const QString &root) {
    return {root+"/athletes",root+"/hills",root+"/state/catalog.json",root+"/photos"};
}
static void waitForCatalog(SingleJumpsConfigWindow &window) {
    QElapsedTimer timer;timer.start();while(!window.catalogReady() && timer.elapsed()<15000)QTest::qWait(10);
    check(window.catalogReady(),"asynchronous simulation catalog finishes loading");
}
static void simulationV2SetupRegressions(MainWindow &main) {
    QTemporaryDir storage;check(storage.isValid(),"isolated v2 setup storage");const auto paths=isolatedCatalogPaths(storage.path());
    SimulationV2::ProfilePackRepository athletes(SimulationV2::ProfilePackKind::Athletes,paths.athletes);
    SimulationV2::ProfilePackRepository hills(SimulationV2::ProfilePackKind::Hills,paths.hills);
    auto first=AthleteProfile::create();first.name="First";first.overall=90;first.form=75;
    auto second=AthleteProfile::create();second.name="Second";second.overall=65;second.form=80;
    auto venue=HillProfile::create();venue.name="Test hill";
    const QJsonObject athletePack{{"format","sj-sim-athlete-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},
        {"name","Ready athletes"},{"profiles",QJsonArray{first.toJson(),second.toJson()}}};
    const QJsonObject hillPack{{"format","sj-sim-hill-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},
        {"name","Ready hills"},{"profiles",QJsonArray{venue.toJson()}}};
    QString error;check(athletes.save(athletePack,error) && hills.save(hillPack,error),"ready setup packs fixture");
    const auto global=GlobalDatabase::get()->packsSnapshot();
    SingleJumpsConfigWindow window(nullptr,paths);
    check(!window.findChild<QPushButton *>("pushButton_submit")->isEnabled(),"cannot submit while catalog loads");
    waitForCatalog(window);
    check(window.configuredJumper().getSimulationProfile()->id==first.id && window.configuredHill().getSimulationProfile()->id==venue.id,"normal setup prefers ready canonical v2 profiles");
    auto *draft=window.getAthleteDraft();draft->setValue("overall",97);draft->setValue("form",26);draft->setValue("landing",4);draft->setPerk(1,-1,3);
    auto profile=window.configuredJumper().getSimulationProfile();
    check(profile->overall==97 && profile->form==26 && profile->landing==4 && profile->perks[1]==-3,"local profile editing changes the actual game input");
    draft->setValue("overall",101);check(window.configuredJumper().getSimulationProfile()->overall==97 && !draft->message().isEmpty(),"invalid draft edits leave the valid input intact");
    draft->reset();check(window.configuredJumper().getSimulationProfile()->overall==90 && draft->message().isEmpty(),"reset restores the selected source profile");
    auto *selector=window.findChild<QComboBox *>("comboBox_existingJumper");selector->setCurrentIndex(1);
    check(window.configuredJumper().getSimulationProfile()->id==second.id && window.configuredJumper().getSimulationProfile()->overall==65,"changing athlete refreshes the full draft rather than only the title");
    selector->setCurrentIndex(-1);check(selector->currentIndex()==0,"required selection cannot remain invalid");
    window.getHillDraft()->setValue("hs",160);window.getHillDraft()->setValue("k",140);window.getHillDraft()->setPerk(3,-1,3);
    check(window.configuredHill().getSimulationProfile()->k==140 && window.getWindsGeneratorSettingsEditor()->getKPoint()==140,"local hill changes synchronize the wind editor");
    QJsonObject unchangedAthletes,unchangedHill;check(athletes.read(athletes.filePath(athletePack),unchangedAthletes,error) && hills.read(hills.filePath(hillPack),unchangedHill,error),"source profiles remain readable");
    check(unchangedAthletes==athletePack && unchangedHill==hillPack && GlobalDatabase::get()->packsSnapshot()==global,"setup edits never change canonical files or legacy global data");
    auto *event=window.findChild<QComboBox *>("comboBox_simulationEvent");event->setCurrentIndex(0);
    check(window.simulationEvent()==SimulationV2::EventKind::Training,"event category is explicit, replacing old numeric importance");
    check(window.suggestedGate()>=0 && window.suggestedGate()==window.suggestedGate(),"v2 automatic gate is deterministic and available in normal setup");
    render(&window,"simulation-v2-setup",true);
    auto *bridge=window.findChild<QuickBridge *>();check(bridge && scene(&window)->status()==QQuickWidget::Ready,"normal setup loads Quick UI");
    const auto labels=bridge->root()->values().value("options").toStringList();
    for(const auto &page:QStringList{QString::fromUtf8("Zawodnik"),QString::fromUtf8("Skocznia")}) {
        bridge->edit(bridge->root()->values().value("id").toInt(),labels.indexOf(page));QTest::qWait(120);
        check(item(scene(&window)->rootObject(),"simulationDraftWorkspace"),"new profile editor replaces ignored legacy parameters");
        scene(&window)->grabFramebuffer().save(output+(page==QString::fromUtf8("Zawodnik")?"/simulation-v2-athlete.png":"/simulation-v2-hill.png"));
    }
    main.resize(1024,640);QTest::qWait(100);
    bridge->edit(bridge->root()->values().value("id").toInt(),labels.indexOf(QString::fromUtf8("Warunki")));QTest::qWait(150);
    check(item(scene(&window)->rootObject(),"weatherWorkspace"),"new weather workspace replaces the old sector editor");
    check(window.getWeatherSetup()->values()["preset"].toInt()==-1,"weather initially uses automatic hill-based proposal");
    check(!item(scene(&window)->rootObject(),"weatherPresetPicker"),"wind scenarios are visible tiles rather than a dropdown");
    check(!item(scene(&window)->rootObject(),"weatherPreset-2"),"presets are hidden until explicitly expanded");
    clickControl(scene(&window),"weatherPresetsToggle");QTest::qWait(30);
    for(int index=0;index<SimulationV2::weatherPresetCount;++index)check(item(scene(&window)->rootObject(),QString("weatherPreset-%1").arg(index)),"every wind scenario has a direct tile");
    clickControl(scene(&window),"weatherPreset-2");check(window.getWeatherSetup()->values()["preset"].toInt()==2,"lottery tile updates the actual weather scenario");
    check(item(scene(&window)->rootObject(),"weatherPreset-2")->property("checked").toBool(),"selected wind tile remains marked");
    clickControl(scene(&window),"weatherSuggest");check(window.getWeatherSetup()->values()["preset"].toInt()==-1,"automatic hill-based choice can be restored directly");
    clickControl(scene(&window),"weatherPreset-2");
    const auto selectedSeed=window.getWeatherSetup()->values()["seed"];
    clickControl(scene(&window),"weatherPresetsToggle");
    render(&window,"simulation-v2-weather",true);
    check(window.getWeatherSetup()->values()["seed"]==selectedSeed,"screen renders never reroll selected weather");
    auto *slopePreview=item(scene(&window)->rootObject(),"weatherSlopePreview");check(slopePreview,"the wind diagram exposes its displayed values");
    const QVariantList beforeEditing=slopePreview->property("samples").toList();
    main.resize(1024,640);QTest::qWait(100);clickControl(scene(&window),"weatherTuningToggle");QTest::qWait(50);
    check(slopePreview->property("samples").toList()==beforeEditing,"opening wind customization does not change the displayed field");
    check(item(scene(&window)->rootObject(),"weatherDirection"),"customization is optional and reveals the four simple controls");
    window.getWeatherSetup()->setValue("direction",37);auto *direction=item(scene(&window)->rootObject(),"weatherDirection");direction->forceActiveFocus();QTest::keyClick(scene(&window),Qt::Key_Right);
    check(std::abs(window.getWeatherSetup()->values()["direction"].toDouble()-38)<.01,"direction dial supports one-degree keyboard changes rather than four rigid directions");
    auto *pointer=item(scene(&window)->rootObject(),"weatherDirectionPointer");check(pointer,"direction selector has a direct pointer surface");
    controlPoint(scene(&window),"weatherDirectionPointer");
    for(auto *ancestor=pointer->parentItem();ancestor;ancestor=ancestor->parentItem())if(ancestor->inherits("QQuickFlickable")) {
        const double centre=pointer->mapToItem(ancestor,QPointF(pointer->width()/2,pointer->height()/2)).y();
        const double offset=ancestor->property("contentY").toDouble()+centre-ancestor->height()/2;
        ancestor->setProperty("contentY",qBound(0.,offset,qMax(0.,ancestor->property("contentHeight").toDouble()-ancestor->height())));
    }
    QTest::qWait(50);
    const auto chooseAngle=[&](qreal x,qreal y){QTest::mouseClick(scene(&window),Qt::LeftButton,Qt::NoModifier,pointer->mapToScene(QPointF(x,y)).toPoint());QTest::qWait(10);};
    chooseAngle(pointer->width()/2,3);check(std::abs(window.getWeatherSetup()->values()["direction"].toDouble())<.01,"wind from above the skis means headwind");
    chooseAngle(pointer->width()/2,pointer->height()-3);check(std::abs(window.getWeatherSetup()->values()["direction"].toDouble()-180)<.01,"wind from behind the skier means tailwind");
    auto *strength=item(scene(&window)->rootObject(),"input-weather-strength");check(strength,"wind strength text input exists");
    for(const auto entry:QList<QPair<QString,double>>{{"2.4",2.5},{"2,6",2.5}}) {
        strength->forceActiveFocus();QTest::keyClick(scene(&window),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&window),entry.first);QTest::keyClick(scene(&window),Qt::Key_Tab);QTest::qWait(20);
        check(std::abs(window.getWeatherSetup()->values()["strength"].toDouble()-entry.second)<1e-8,"wind input accepts a dot or comma without reverting the value");
    }
    auto *directionPointer=item(scene(&window)->rootObject(),"weatherDirectionPointer");check(directionPointer,"global wind direction has a drag target");
    const auto pointerPoint=[&](double x,double y){return directionPointer->mapToItem(scene(&window)->rootObject(),QPointF(x,y)).toPoint();};
    const auto pointerSize=directionPointer->size();
    QTest::mousePress(scene(&window),Qt::LeftButton,Qt::NoModifier,pointerPoint(pointerSize.width()*.85,pointerSize.height()/2));
    const auto dragTo=[&](QPoint target) {
        QMouseEvent move(QEvent::MouseMove,target,scene(&window)->mapToGlobal(target),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(scene(&window),&move);QTest::qWait(30);
    };
    dragTo(pointerPoint(pointerSize.width()/2,-25));
    dragTo(pointerPoint(-25,pointerSize.height()/2));
    QTest::mouseRelease(scene(&window),Qt::LeftButton,Qt::NoModifier,pointerPoint(-25,pointerSize.height()/2));QTest::qWait(30);
    if(std::abs(window.getWeatherSetup()->values()["direction"].toDouble()-90)>=1){qInfo()<<"DIAL DRAG"<<window.getWeatherSetup()->values()["direction"]<<directionPointer->mapRectToScene(directionPointer->boundingRect())<<scene(&window)->size();main.grab().save("/tmp/sj-wind-drag-failure.png");}
    check(std::abs(window.getWeatherSetup()->values()["direction"].toDouble()-90)<1,"direction dragging keeps its grab after moving outside the dial and never scrolls the page instead");
    clickControl(scene(&window),"windStation-3");QTest::qWait(30);
    check(slopePreview->property("samples").toList()==window.getWeatherSetup()->basePreview(),"opening one wind station retains the same starting field");
    check(item(scene(&window)->rootObject(),"weather-station-strength") && !item(scene(&window)->rootObject(),"weatherLocalToggle"),"local editing opens directly inside the main slope without a separate point form");
    QMetaObject::invokeMethod(item(scene(&window)->rootObject(),"weather-station-strength"),"committed",Q_ARG(double,2.4));
    check(std::abs(window.getWeatherSetup()->station(3)["strength"].toDouble()-2.4)<1e-8 && window.getWeatherSetup()->references().size()==6,"station UI edits the actual field while keeping exactly six simple controls");
    check(!item(scene(&window)->rootObject(),"weatherDirection"),"opening a local station closes whole-hill tuning instead of stacking similar editors");
    QMetaObject::invokeMethod(item(scene(&window)->rootObject(),"weather-station-degrees"),"committed",Q_ARG(double,215));QTest::qWait(20);
    check(std::abs(window.getWeatherSetup()->station(3)["strength"].toDouble()-2.4)<1e-8,"typing a local direction preserves local strength in the real UI");
    auto *stationInput=item(scene(&window)->rootObject(),"input-wind-station-strength");check(stationInput,"direct station strength has an editable input");
    stationInput->forceActiveFocus();QTest::keyClick(scene(&window),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&window),"3,2");
    clickControl(scene(&window),"windStation-4");QTest::qWait(20);
    check(std::abs(window.getWeatherSetup()->station(3)["strength"].toDouble()-3.2)<1e-8,"switching arrows commits the previous point before editing another");
    check(!item(scene(&window)->rootObject(),"weatherAdvancedToggle") && !item(scene(&window)->rootObject(),"weather-gustProbability"),"technical weather form is not exposed by customization");
    check(item(scene(&window)->rootObject(),"weatherPrecipitationKind") && item(scene(&window)->rootObject(),"weatherPrecipitationLevel"),"precipitation exposes a short independent kind and intensity choice");
    window.getWeatherSetup()->setValue("precipitationType",0);QTest::qWait(30);
    check(item(scene(&window)->rootObject(),"weatherPrecipitationKind")->property("currentIndex").toInt()==0 && !item(scene(&window)->rootObject(),"weatherPrecipitationLevel")->isVisible(),"no precipitation hides intensity and selects one unambiguous type option");
    window.getWeatherSetup()->setValue("precipitationType",2);window.getWeatherSetup()->setValue("precipitationLevel",3);
    scene(&window)->grabFramebuffer().save(output+"/simulation-v2-weather-custom.png");
    window.reject();QTest::qWait(50);
    // New batch jump identity and random quality must not depend on worker scheduling.
    SingleJumpsManager manager;manager.setJumper(window.configuredJumper());manager.setHill(window.configuredHill());
    manager.setGate(10);manager.setChangeableWind(false);manager.getRules().setHasDsq(false);manager.getRules().setHasJudgesPoints(true);
    manager.getRules().setWindMeasurementType(CompetitionRules::FisApproximationWindMeasurement);manager.getRules().setHasWindCompensations(true);
    manager.setSimulationWeather(window.getWeatherSetup()->field());manager.setChangeableWind(true);
    manager.setSimulationSeed(123456789);manager.setSimulationEvent(SimulationV2::EventKind::Training);
    auto one=manager.simulateAsync(2048,1);one.waitForFinished();const auto sequential=one.result();
    auto many=manager.simulateAsync(2048,4);many.waitForFinished();const auto parallel=many.result();
    check(sequential.size()==2048 && parallel.size()==sequential.size(),"v2 batch produces all requested jumps");
    check(sequential[0].getSimulationV2Data()["experiencedWind"]!=sequential[1].getSimulationV2Data()["experiencedWind"],"actual continuous weather evolves between indexed batch jumps");
    for(int i=0;i<sequential.size();++i) {
        check(sequential[i].getSimulationV2Data()==parallel[i].getSimulationV2Data() && sequential[i].getPoints()==parallel[i].getPoints(),"changing-weather v2 batch is identical with one or four workers");
        const auto metadata=sequential[i].getSimulationV2Data();
        check(metadata["windMeasurementType"].toInt()==CompetitionRules::FisApproximationWindMeasurement,"parallel single jumps retain the selected FIS approximation");
        check(metadata["startSeconds"].toDouble()==i*30. && metadata["event"].toInt()==int(SimulationV2::EventKind::Training),"each batch jump has its own logical time and selected category");
    }
    auto restored=sequential.first();auto metadata=restored.getSimulationV2Data();metadata["startSeconds"]=30000.;restored.setSimulationV2Data(metadata);manager.setJumps({restored});
    auto continued=manager.simulateAsync(1,1);continued.waitForFinished();
    check(continued.result().first().getSimulationV2Data()["startSeconds"].toDouble()==30030.,"continuing a partial batch advances past the last completed index rather than reusing time and seed");
    // Empty canonical sources must not pretend an editable old profile is usable.
    SimulationProfileDraft empty(DataPack::Jumpers);check(!empty.valid() && empty.profile().isEmpty(),"unselected drafts expose a clear empty state");
    main.resize(1024,640);qInfo()<<"PASS: v2 setup, local drafts, canonical data protection, auto gate and indexed parallel jump seeds";
}
