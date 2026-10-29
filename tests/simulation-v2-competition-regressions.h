#pragma once
static void simulationV2CompetitionRegressions() {
    auto low=AthleteProfile::create();low.name="Low";low.overall=40;low.form=50;
    auto high=AthleteProfile::create();high.name="High";high.overall=95;high.form=90;
    auto venue=HillProfile::create();venue.name="New hill";
    Jumper a,b;Hill hill;QString error;
    check(a.setSimulationProfile(low,error) && b.setSimulationProfile(high,error) && hill.setSimulationProfile(venue,error),"competition v2 gate fixtures");
    a.getJumperSkillsPointer()->setTakeoffTechnique(9999);a.getJumperSkillsPointer()->setFlightTechnique(9999);
    b.getJumperSkillsPointer()->setTakeoffTechnique(0);b.getJumperSkillsPointer()->setFlightTechnique(0);
    CompetitionConfigWindow window(CompetitionConfigWindow::SingleCompetition,nullptr,nullptr,CompetitionConfigWindow::DataSource::LegacyCompatibility);
    window.getHillEditor()->setHill(&hill);window.getHillEditor()->fillHillInputs();
    window.setCompetitionJumpers({&a,&b});const int both=window.suggestedGate();window.setCompetitionJumpers({&b});
    check(both>=0 && both==window.suggestedGate(),"competition auto gate selects strongest new baseline instead of obsolete techniques");
    Jumper old("Old","Athlete","POL");window.setCompetitionJumpers({&old});
    check(window.suggestedGate()==-1,"mixed-version preview safely returns unavailable rather than throwing");
    bool blocked=false;QTimer closer;QObject::connect(&closer,&QTimer::timeout,&window,[&]{for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){blocked=box->windowTitle()==QString::fromUtf8("Niezgodne profile");box->accept();}});closer.start(20);
    check(QMetaObject::invokeMethod(&window,"on_pushButton_submit_clicked",Qt::DirectConnection),"competition submission validation is callable");closer.stop();
    check(blocked,"mixed versions are rejected before entering a competition manager");
    qInfo()<<"PASS: competition v2 quality-based gate and mixed-version validation";
}

static void simulationV2CanonicalCompetitionRegressions(MainWindow &main) {
    QTemporaryDir storage;check(storage.isValid(),"isolated canonical competition");
    SimulationV2::CatalogPaths paths{storage.path()+"/athletes",storage.path()+"/hills",storage.path()+"/catalog.json",storage.path()+"/photos"};
    SimulationV2::ProfilePackRepository athletes(SimulationV2::ProfilePackKind::Athletes,paths.athletes),hills(SimulationV2::ProfilePackKind::Hills,paths.hills);
    QJsonArray entries;for(int i=0;i<6;++i){auto p=AthleteProfile::create();p.name="Canonical";p.surname=QString::number(i);p.country=i<3?"POL":"NOR";if(i==5){p.name.clear();p.surname.clear();}entries.append(p.toJson());}
    QJsonObject pack{{"format","sj-sim-athlete-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},{"name","Canonical athletes"},{"profiles",entries}};
    auto other=pack;other["id"]=QUuid::createUuid().toString(QUuid::WithoutBraces);other["name"]="Second pack";other["profiles"]=QJsonArray{AthleteProfile::create().toJson()};
    auto venue=HillProfile::create();venue.name="Canonical hill";
    QJsonObject hillPack{{"format","sj-sim-hill-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},{"name","Canonical hills"},{"profiles",QJsonArray{venue.toJson()}}};
    QString error;check(athletes.save(pack,error) && athletes.save(other,error) && hills.save(hillPack,error),"canonical competition packs fixture");
    const auto globals=GlobalDatabase::get()->packsSnapshot();
    CompetitionConfigWindow window(CompetitionConfigWindow::SingleCompetition,&main,nullptr,CompetitionConfigWindow::DataSource::Canonical,paths);
    check(!window.findChild<QPushButton *>("pushButton_submit")->isEnabled(),"competition cannot start while catalog is loading");
    QElapsedTimer loading;loading.start();while(!window.catalogReady() && loading.elapsed()<15000)QTest::qWait(10);
    check(window.catalogReady() && window.configuredHill().getSimulationProfile(),"default competition loads new hill profile");
    auto *selector=window.findChild<QComboBox *>("comboBox_athletePack");selector->setCurrentIndex(selector->findData(pack["id"].toString()));
    const auto original=window.getCompetitionJumpers();check(original.size()==6,"canonical athlete pack populates the actual start list");
    window.show();QTest::qWait(150);auto *bridge=window.findChild<QuickBridge *>();auto *roster=bridge->findChild<QuickRoster *>();check(roster,"canonical competition has roster editor");
    const auto setupTabs=bridge->root()->values()["options"].toStringList();const auto hillTab=setupTabs.indexOf(QString::fromUtf8("Skocznia"));check(hillTab>=0,"hill configuration section exists");
    bridge->edit(bridge->root()->values()["id"].toInt(),hillTab);QTest::qWait(80);
    check(item(scene(&window)->rootObject(),"inlineHillPack") && item(scene(&window)->rootObject(),"inlineHillChoice"),"advanced hill editor exposes the existing pack and hill selectors inline");
    bridge->edit(bridge->root()->values()["id"].toInt(),0);QTest::qWait(50);
    selector->setCurrentIndex(selector->findData(other["id"].toString()));bridge->refresh();roster->sync();
    check(window.getCompetitionJumpers().size()==1 && roster->library()->rowCount()==1,"pack change clears old roster candidates");
    check(original.first()->getName()=="Canonical","previous pack pointers remain valid after switching packs");
    selector->setCurrentIndex(selector->findData(pack["id"].toString()));check(window.getCompetitionJumpers().first()==original.first(),"returning to a pack keeps stable actor identity");
    CompetitionRules rules;rules.setRounds({RoundInfo(4,true),RoundInfo(2,true)});rules.setHasDsq(false);rules.setWindAverageCalculatingType(WindsCalculator::Original);
    window.getCompetitionRulesEditor()->setCompetitionRules(&rules);window.getCompetitionRulesEditor()->fillCompetitionRulesInputs();
    window.getWeatherSetup()->applyPreset(4);window.getWeatherSetup()->setValue("precipitationKind",1);window.getWeatherSetup()->setValue("precipitationLevel",1);
    window.getHillDraft()->setPerk(0,1,2,false);check(window.configuredHill().getSimulationProfile()->perks[0]==2,"hill draft is the actual competition input");
    for(auto *checkBox:window.findChildren<QCheckBox *>())if(checkBox->text().contains(QString::fromUtf8("kwalifikac")))checkBox->setChecked(true);
    const auto seed=window.getWeatherSetup()->values()["seed"];
    const auto labels=bridge->root()->values()["options"].toStringList();bridge->edit(bridge->root()->values()["id"].toInt(),labels.indexOf(QString::fromUtf8("Warunki")));QTest::qWait(50);
    check(item(scene(&window)->rootObject(),"weatherWorkspace"),"normal competition exposes the shared simplified weather screen");
    const bool oldLive=GlobalAppSettings::get()->getLiveCompetition();GlobalAppSettings::get()->setLiveCompetition(false);
    const bool oldRecords=GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords();GlobalSimulationSettings::get()->setUpdateGlobalDatabaseRecords(true);
    original.first()->setPersonalBest(300); // A pending record must merge into a freshly edited pack.
    auto edited=pack;auto editedEntries=entries;auto editedProfile=editedEntries[0].toObject();editedProfile["surname"]="Changed on disk";editedEntries[0]=editedProfile;edited["profiles"]=editedEntries;check(athletes.save(edited,error),"parallel authoring edit fixture");
    QTimer dialogs,runner;bool running=false;int phases=0,ticks=0;QSet<int> times;
    QObject::connect(&dialogs,&QTimer::timeout,&window,[&]{check(++ticks<1500,"canonical competition finishes without hanging");for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){auto *answer=box->button(QMessageBox::Yes);if(!answer)answer=box->button(QMessageBox::Ok);if(answer)answer->click();else box->accept();}});
    QObject::connect(&runner,&QTimer::timeout,&window,[&]{if(running)return;for(auto *widget:QApplication::allWidgets())if(auto *contest=qobject_cast<CompetitionManagerWindow *>(widget);contest && contest->isVisible()) {
        running=true;++phases;contest->autoSimulateCompetition();
        for(const auto &result:contest->getManager()->getResults()->getResultsReference())for(const auto &jump:result.getJumps()) {
            const auto data=jump.getSimulationV2Data();check(data["engineVersion"]==2 && data["precipitationKind"].toInt()==1,"normal contest uses v2 engine and selected rain");
            check(!times.contains(data["startSeconds"].toInt()),"qualifications and main contest do not repeat weather time slots");times.insert(data["startSeconds"].toInt());
        }
        if(contest->isVisible())contest->findChild<QPushButton *>("pushButton_goToNext")->click();running=false;break;
    }});
    dialogs.start(20);runner.start(30);check(QMetaObject::invokeMethod(&window,"on_pushButton_submit_clicked",Qt::DirectConnection),"canonical submission uses the normal launch path");runner.stop();dialogs.stop();
    check(phases==2 && times.size()>=12 && times.contains(0) && *std::max_element(times.begin(),times.end())==(times.size()-1)*30,"qualifications and two-round competition share one continuous weather clock");
    check(window.getWeatherSetup()->values()["seed"]==seed,"launching never rerolls the accepted weather variant");
    QJsonObject saved;check(athletes.read(athletes.filePath(pack),saved,error) && saved["profiles"].toArray()[0].toObject()["surname"]=="Changed on disk","record persistence preserves newer authoring edits");
    check(saved["profiles"].toArray()[0].toObject()["personalBest"].toDouble()>0,"new records persist in canonical athlete pack");
    check(GlobalDatabase::get()->packsSnapshot()==globals,"competition never overwrites legacy global packs");
    GlobalAppSettings::get()->setLiveCompetition(oldLive);GlobalSimulationSettings::get()->setUpdateGlobalDatabaseRecords(oldRecords);window.reject();
    qInfo()<<"PASS: canonical contest sources, stable ownership, pack switching, shared weather, qualification clock and safe record merge";
}
