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
    clickControl(scene(&window),"weatherPreset-2");check(window.getWeatherSetup()->values()["preset"].toInt()==2,"preset card updates the actual weather backend");
    const auto selectedSeed=window.getWeatherSetup()->values()["seed"];
    render(&window,"simulation-v2-weather",true);
    check(window.getWeatherSetup()->values()["seed"]==selectedSeed,"screen renders never reroll selected weather");
    main.resize(1024,640);QTest::qWait(100);clickControl(scene(&window),"weatherTuningToggle");QTest::qWait(50);
    check(item(scene(&window)->rootObject(),"weatherDirection"),"customization is optional and reveals the four simple controls");
    clickControl(scene(&window),"weatherAdvancedToggle");QTest::qWait(50);
    check(item(scene(&window)->rootObject(),"weather-gustProbability"),"advanced weather controls remain available inline");
    scene(&window)->grabFramebuffer().save(output+"/simulation-v2-weather-advanced.png");
    window.reject();QTest::qWait(50);
    // New batch jump identity and random quality must not depend on worker scheduling.
    SingleJumpsManager manager;manager.setJumper(window.configuredJumper());manager.setHill(window.configuredHill());
    manager.setGate(10);manager.setChangeableWind(false);manager.getRules().setHasDsq(false);manager.getRules().setHasJudgesPoints(true);
    manager.setSimulationWeather(window.getWeatherSetup()->field());manager.setChangeableWind(true);
    manager.setSimulationSeed(123456789);manager.setSimulationEvent(SimulationV2::EventKind::Training);
    auto one=manager.simulateAsync(2048,1);one.waitForFinished();const auto sequential=one.result();
    auto many=manager.simulateAsync(2048,4);many.waitForFinished();const auto parallel=many.result();
    check(sequential.size()==2048 && parallel.size()==sequential.size(),"v2 batch produces all requested jumps");
    check(sequential[0].getSimulationV2Data()["experiencedWind"]!=sequential[1].getSimulationV2Data()["experiencedWind"],"actual continuous weather evolves between indexed batch jumps");
    for(int i=0;i<sequential.size();++i) {
        check(sequential[i].getSimulationV2Data()==parallel[i].getSimulationV2Data() && sequential[i].getPoints()==parallel[i].getPoints(),"changing-weather v2 batch is identical with one or four workers");
        const auto metadata=sequential[i].getSimulationV2Data();
        check(metadata["startSeconds"].toDouble()==i*30. && metadata["event"].toInt()==int(SimulationV2::EventKind::Training),"each batch jump has its own logical time and selected category");
    }
    auto restored=sequential.first();auto metadata=restored.getSimulationV2Data();metadata["startSeconds"]=30000.;restored.setSimulationV2Data(metadata);manager.setJumps({restored});
    auto continued=manager.simulateAsync(1,1);continued.waitForFinished();
    check(continued.result().first().getSimulationV2Data()["startSeconds"].toDouble()==30030.,"continuing a partial batch advances past the last completed index rather than reusing time and seed");
    // Empty canonical sources must not pretend an editable old profile is usable.
    SimulationProfileDraft empty(DataPack::Jumpers);check(!empty.valid() && empty.profile().isEmpty(),"unselected drafts expose a clear empty state");
    main.resize(1024,640);qInfo()<<"PASS: v2 setup, local drafts, canonical data protection, auto gate and indexed parallel jump seeds";
}
