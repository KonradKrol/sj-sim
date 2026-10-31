#pragma once

#include "../source/simulation-v2/ProfileDraft.h"

static void simulationV2NextSeasonDraftRegressions(MainWindow &main) {
    QString error;auto athlete=AthleteProfile::create();athlete.name="Next";athlete.surname="Season";athlete.overall=84;athlete.form=39;athlete.perks={{3,-2,1}};athlete.photoPath="photos/jumper.png";
    auto venue=HillProfile::create();venue.name="Next hill";venue.perks={{-2,3,1,-3}};
    Jumper original;Hill originalHill;check(original.setSimulationProfile(athlete,error) && originalHill.setSimulationProfile(venue,error),"next season fixtures");original.setSimulationPackContext(QUuid::createUuid().toString(QUuid::WithoutBraces),"portable-pack");
    NewSeasonConfiguratorWindow window(true,&main);window.setProperty("simulationV2",true);window.setJumpers({new Jumper(original)});window.setHills({new Hill(originalHill)});
    auto *draft=window.findChild<SimulationProfileDraft *>("nextSeasonAthleteDraft");auto *hillDraft=window.findChild<SimulationProfileDraft *>("nextSeasonHillDraft");check(draft && hillDraft && draft->valid() && hillDraft->valid(),"next season uses real v2 drafts");
    draft->setValue("overall",93);draft->setPerk(2,-1,3,false);hillDraft->setValue("k",125);
    auto *changed=window.getJumpersReference().first();check(changed->getSimulationProfile()->overall==93 && changed->getSimulationProfile()->perks[2]==-3 && changed->getID()==original.getID() && changed->getSimulationAssetDirectory()==original.getSimulationAssetDirectory() && changed->getSimulationProfile()->photoPath==athlete.photoPath,"next season edit preserves identity, photo path and full v2 profile");check(window.getHillsReference().first()->getSimulationProfile()->k==125 && window.getHillsReference().first()->getSimulationProfile()->perks==venue.perks,"next season hill retains all perk pairs");
    auto lists=window.findChildren<DatabaseItemsListView *>();int inserted=0;for(auto *list:lists)if(list->property("simulationV2").toBool()){check(QMetaObject::invokeMethod(list,"onInsertActionTriggered"),"next season insertion action");++inserted;}check(inserted==2 && window.getJumpersReference().last()->getSimulationProfile() && window.getHillsReference().last()->getSimulationProfile(),"next season insertions are canonical before publication");
    window.reject();check(original.getSimulationProfile()->overall==84 && originalHill.getSimulationProfile()->k==120,"canceling next season leaves source data untouched");
    qInfo()<<"PASS: next season v2 drafts, insertions, UUID/photo/perk preservation and cancel isolation";
}

static void simulationV2SeasonEditorRegressions(MainWindow &main)
{
    simulationV2NextSeasonDraftRegressions(main);
    SaveJumpersList limitsFixture("Limits");
    SaveJumpersList sameName("Limits");check(limitsFixture.getId()!=sameName.getId(),"separate rosters with the same name have distinct identities");
    check(limitsFixture.setCountryLimit("pol",4) && limitsFixture.setCountryLimit("NOR",0),"country limits accept normalized codes and zero");
    check(!limitsFixture.setCountryLimit("invalid",3) && !limitsFixture.setCountryLimit("POL",10001),"invalid country limits are rejected");
    auto limitsJson=SaveJumpersList::getJsonObject(limitsFixture);
    auto limitsReloaded=SaveJumpersList::getFromJson(limitsJson,nullptr);
    check(limitsReloaded.getId()==limitsFixture.getId() && limitsReloaded.countryLimit("POL")==4 && limitsReloaded.countryLimit("nor")==0 && limitsReloaded.countryLimit("CZE")==-1,"roster identity and advisory limits survive serialization");
    check(limitsReloaded.setCountryLimit("POL",-1) && limitsReloaded.countryLimit("POL")==-1,"a country limit can be cleared");
    limitsJson.remove("id");limitsJson.remove("country-limits");
    auto oldList=SaveJumpersList::getFromJson(limitsJson,nullptr);
    check(!oldList.getId().isEmpty() && oldList.getCountryLimits().isEmpty(),"legacy rosters gain an identity without imposed limits");
    {
        auto a=AthleteProfile::create();a.name="A";a.surname="Pol";a.country="POL";
        auto b=a;b.name="B";auto c=a;c.name="C";c.country="NOR";
        Jumper ja,jb,jc;QString fixtureError;
        check(ja.setSimulationProfile(a,fixtureError) && jb.setSimulationProfile(b,fixtureError) && jc.setSimulationProfile(c,fixtureError),"roster limit fixtures");
        JumpersListEditorWindow editor;editor.setAllJumpers({&ja,&jb,&jc});editor.setSelectedJumpers({&ja,&jc});editor.updateUnselectedJumpers();editor.show();QTest::qWait(150);
        auto *rosterBridge=editor.findChild<QuickBridge *>();auto *roster=rosterBridge?rosterBridge->findChild<QuickRoster *>():nullptr;
        check(roster && roster->savedList() && roster->grouped() && item(scene(&editor)->rootObject(),"savedRosterWorkspace"),"new saved roster workspace starts grouped by country");
        roster->selectCandidate(QuickRoster::key(&jb));
        check(roster->candidateMarkedCount()==1 && !editor.getSelectedJumpers().contains(&jb),"candidate selection does not alter membership");
        roster->addMarkedCandidates();
        check(editor.getSelectedJumpers().contains(&jb) && roster->candidateMarkedCount()==0,"batch adding candidates commits membership and clears added selections");
        roster->undo();check(!editor.getSelectedJumpers().contains(&jb),"batch candidate addition can be undone");
        {
            QTest::qWait(80);presentation(&editor)->grab();auto *view=scene(&editor);
            auto *grip=item(view->rootObject(),"savedRosterCandidateGrip-"+QuickRoster::key(&jb));auto *target=item(view->rootObject(),"savedRosterSelected");
            check(grip && target,"new roster has direct drag handles and a receiving list");
            const auto begin=grip->mapToItem(view->rootObject(),grip->boundingRect().center()).toPoint();const auto end=target->mapToItem(view->rootObject(),target->boundingRect().center()).toPoint();
            QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,begin);
            for(int step=1;step<=12;++step){const QPoint at=begin+(end-begin)*step/12;QMouseEvent move(QEvent::MouseMove,at,view->mapToGlobal(at),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(20);}
            QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,end);QTest::qWait(100);
            check(editor.getSelectedJumpers().contains(&jb),"dragging from all athletes to selected adds the athlete without a dialog");
            roster->undo();check(!editor.getSelectedJumpers().contains(&jb),"cross-panel drag can be undone");
        }
        QAbstractItemModelTester limitContract(roster->limitRows(),QAbstractItemModelTester::FailureReportingMode::Fatal);
        QSignalSpy limitResets(roster->limitRows(),&QAbstractItemModel::modelReset);
        check(roster->setCountryLimit("POL",1),"country limit is editable");
        roster->toggle(QuickRoster::key(&jb));QTest::qWait(80);
        check(editor.getSelectedJumpers().contains(&jb),"exceeding an advisory limit never blocks adding a jumper");
        QVariantMap pol;for(const auto &value:roster->countryLimits())if(value.toMap().value("country").toString()=="POL")pol=value.toMap();
        check(pol.value("count").toInt()==2 && pol.value("limit").toInt()==1 && pol.value("over").toBool() && roster->exceededCountries()==1,"country summary reports an exceeded limit");
        const auto originalOrder=editor.getSelectedJumpers();roster->setGrouped(true);
        check(editor.getSelectedJumpers()==originalOrder && roster->groupedRows()->get(0).value("countryCode").toString()=="NOR","grouped view sorts by country without changing saved order");
        roster->setGrouped(false);check(roster->ordered()->get(0).value("athleteId").toString()==QuickRoster::key(&ja),"returning to the normal view restores the original order");roster->setGrouped(true);
        auto *view=scene(&editor);
        for(int i=0;i<3;++i){clickControl(view,"savedRosterView-1");check(!roster->grouped(),"the order tab restores the normal view");clickControl(view,"savedRosterView-0");check(roster->grouped(),"the country tab opens the grouped view");}
        clickControl(view,"rosterLimitsToggle");QTest::qWait(100);
        presentation(&editor)->grab().save(output+"/saved-roster-limits.png");
        QPointer<QQuickItem> limitInput=item(view->rootObject(),"roster-limit-POL");check(limitInput,"country limit field is visible in the list editor");
        clickControl(view,"roster-limit-POL");QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClick(view,Qt::Key_Backspace);QTest::keyClick(view,Qt::Key_Tab);QTest::qWait(120);
        check(!editor.getCountryLimits().contains("POL"),"clearing the limit field removes the limit");
        check(limitInput && item(view->rootObject(),"roster-limit-POL")==limitInput,"editing a limit keeps the existing row and focus target");
        roster->undo();check(editor.getCountryLimits().value("POL",-1)==1,"undo restores the previous country limit");
        for(int limit=2;limit<=5;++limit){
            clickControl(view,"roster-limit-POL");QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,QString::number(limit));QTest::keyClick(view,Qt::Key_Tab);QTest::qWait(80);
            check(editor.getCountryLimits().value("POL",-1)==limit && limitInput,"repeated limit edits retain the row and commit each value");
        }
        roster->undo();check(editor.getCountryLimits().value("POL",-1)==4,"undo restores the last edited limit");
        check(limitResets.isEmpty(),"updating limits preserves the list model and its scroll position");
        editor.reject();
    }
    {
        QVector<Jumper> actors;actors.reserve(6);QString squadError;
        for(int i=0;i<6;++i){auto profile=AthleteProfile::create();profile.overall=99-i*10;Jumper actor;check(actor.setSimulationProfile(profile,squadError),"automatic squad fixture");actors.append(actor);}
        const QVector<QVector<int>> expected{{0},{1,0},{1,2,0},{2,3,1,0},{2,4,3,1,0}};
        for(int count=1;count<=5;++count){Team squad("POL");QVector<Jumper *> members;for(auto &actor:actors)members.append(&actor);std::reverse(members.begin(),members.end());squad.setJumpers(members);squad.arrangeSquad(count,0.);
            check(squad.getJumpers().size()==6,"automatic selection preserves all reserve athletes");
            for(int i=0;i<count;++i)check(squad.getJumpers()[i]==&actors[expected[count-1][i]],"automatic squad uses prescribed middle/worst/best order");
        }
    }
    QString error;
    auto profile=AthleteProfile::create();profile.name="Season";profile.surname="Athlete";profile.overall=83;profile.form=61;profile.perks={{1,-2,3}};
    auto venue=HillProfile::create();venue.name="Season hill";venue.perks={{1,-1,2,-3}};
    Jumper source;Hill sourceHill;
    check(source.setSimulationProfile(profile,error) && sourceHill.setSimulationProfile(venue,error),"season editor fixtures validate");
    source.setSimulationPackContext(QUuid::createUuid().toString(QUuid::WithoutBraces),"/test/pack");
    const QString saveName="Season autosave test "+QUuid::createUuid().toString(QUuid::WithoutBraces);
    SimulationSave save;save.setEngineVersion(2);save.setName(saveName);
    save.setJumpers({new Jumper(source),new Jumper(source)});save.getJumpersReference()[1]->reassign();
    save.setHills({new Hill(sourceHill)});
    Season season;season.setSeasonNumber(2026);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.updateNextCompetitionIndex();
    SimulationSaveManagerWindow manager(&save,&main);manager.show();QTest::qWait(120);
    auto *athlete=manager.findChild<SimulationProfileDraft *>("seasonAthleteDraft");auto *hill=manager.findChild<SimulationProfileDraft *>("seasonHillDraft");
    DatabaseItemsListView *jumpers=nullptr,*hills=nullptr;
    for(auto *list:manager.findChildren<DatabaseItemsListView *>()) {
        if(list->getType()==DatabaseItemsListView::SeasonJumpersItems)jumpers=list;
        if(list->getType()==DatabaseItemsListView::SeasonHillsItems)hills=list;
    }
    check(athlete && hill && jumpers && hills,"season owns separate profile drafts and lists");
    jumpers->listViewDoubleClicked(jumpers->getListModel()->index(0,0));
    auto *initialTimer=manager.findChild<QTimer *>("seasonAutosaveTimer");check(initialTimer && !initialTimer->isActive(),"viewing a jumper alone does not schedule an automatic save");
    auto *identity=save.getJumpersReference()[0];const auto id=identity->getIDStr();
    athlete->setValue("overall",91);athlete->setPerk(1,1,3,false);athlete->setValue("form",78);
    check(identity==save.getJumpersReference()[0] && identity->getIDStr()==id && identity->getSimulationProfile()->overall==91 && identity->getSimulationProfile()->form==78 && identity->getSimulationProfile()->perks[1]==3,"season edits update the existing owned actor without losing identity or full profile");
    check(source.getSimulationProfile()->overall==83 && source.getSimulationProfile()->perks[1]==-2 && identity->getSimulationAssetDirectory()=="/test/pack","season edits preserve portable photo context and leave source data unchanged");
    athlete->setValue("overall",0);check(identity->getSimulationProfile()->overall==91 && !athlete->message().isEmpty(),"invalid draft cannot corrupt the season actor");
    athlete->setValue("overall",94);athlete->reset();
    check(identity->getSimulationProfile()->overall==83 && identity->getSimulationProfile()->form==61,"reset restores selected season profile");
    jumpers->listViewDoubleClicked(jumpers->getListModel()->index(1,0));athlete->setValue("surname","Second");
    check(save.getJumpersReference()[1]->getSurname()=="Second" && identity->getSurname()=="Athlete","switching athletes cannot write into the previous selection");
    auto *tabs=manager.findChild<QTabWidget *>("toolBox");tabs->setCurrentIndex(0);
    jumpers->listViewDoubleClicked(jumpers->getListModel()->index(0,0));
    auto *bridge=manager.findChild<QuickBridge *>();bridge->refresh();
    bridge->edit(bridge->root()->values().value("id").toInt(),1);QTest::qWait(80);
    render(&manager,"season-athlete-profile",true);
    check(item(scene(&manager)->rootObject(),"simulationDraftWorkspace"),"season athlete uses bounded new profile workspace");
    presentation(&manager)->resize(1440,900);QTest::qWait(100);
    check(item(scene(&manager)->rootObject(),"athleteLibrary"),"season athletes reuse the database library component");
    clickControl(scene(&manager),"athleteListRow-1");QTest::qWait(80);
    check(athlete->profile().value("surname").toString()=="Second","shared season athlete library opens a profile with one click");
    clickControl(scene(&manager),"athleteListRow-0");QTest::qWait(80);
    tabs->setCurrentIndex(1);hills->listViewDoubleClicked(hills->getListModel()->index(0,0));
    hill->setPerk(3,1,3,false);hill->setValue("hs",145);
    check(save.getHillsReference()[0]->getSimulationProfile()->perks[3]==3 && save.getHillsReference()[0]->getHSPoint()==145 && sourceHill.getHSPoint()==venue.hs,"season hill edits update the owned venue and retain perks");
    render(&manager,"season-hill-profile",true);
    check(item(scene(&manager)->rootObject(),"simulationDraftWorkspace"),"season hill uses the new profile workspace");
    bool completeAtSignal=false;
    QObject::connect(jumpers->getListModel(),&QAbstractItemModel::rowsInserted,&manager,[&](const QModelIndex &,int first,int){completeAtSignal=save.getJumpersReference()[first]->getSimulationProfile().has_value();});
    check(QMetaObject::invokeMethod(jumpers,"onInsertActionTriggered") && completeAtSignal,"new season athlete is a complete v2 profile before rowsInserted observers run");
    check(QMetaObject::invokeMethod(hills,"onInsertActionTriggered") && save.getHillsReference().last()->getSimulationProfile().has_value(),"new season hills also use v2 profiles");
    std::unique_ptr<SimulationSave> restored(SimulationSave::getFromJson(SimulationSave::getJsonObject(save),nullptr));
    check(bool(restored),"edited and expanded v2 season remains serializable");
    check(restored->getAutosaveEnabled(),"older and new seasons default to automatic saving");
    auto oldSave=SimulationSave::getJsonObject(save);oldSave.remove("autosave-enabled");
    std::unique_ptr<SimulationSave> restoredOld(SimulationSave::getFromJson(oldSave,nullptr));
    check(restoredOld && restoredOld->getAutosaveEnabled(),"seasons saved before the toggle keep automatic saving enabled");
    auto *autoSquads=manager.findChild<QCheckBox *>("checkBox_autoSquads"),*autoGate=manager.findChild<QCheckBox *>("checkBox_autoGate");
    check(autoSquads && autoGate && save.getAutoSquadsEnabled() && save.getAutoGateEnabled(),"season preparation defaults enable automatic squads and gates");
    autoSquads->setChecked(false);autoGate->setChecked(false);
    std::unique_ptr<SimulationSave> preparationReload(SimulationSave::getFromJson(SimulationSave::getJsonObject(save),nullptr));
    check(preparationReload && !preparationReload->getAutoSquadsEnabled() && !preparationReload->getAutoGateEnabled(),"preparation preferences survive save and reload independently");
    autoSquads->setChecked(true);autoGate->setChecked(true);

    auto duplicatedIds=SimulationSave::getJsonObject(save);const auto oneList=SaveJumpersList::getJsonObject(limitsFixture);
    duplicatedIds["jumpers-lists"]=QJsonArray{oneList,oneList};
    std::unique_ptr<SimulationSave> normalized(SimulationSave::getFromJson(duplicatedIds,nullptr));
    check(normalized && normalized->getJumpersLists().size()==2 && normalized->getJumpersLists()[0].getId()!=normalized->getJumpersLists()[1].getId(),"duplicate roster identities are repaired without removing either list");
    SaveJumpersList savedRoster("Limit integration");savedRoster.setJumpers({save.getJumpersReference().first()});save.setJumpersLists({savedRoster});
    DatabaseItemsListView *savedLists=nullptr;for(auto *list:manager.findChildren<DatabaseItemsListView *>())if(list->getType()==DatabaseItemsListView::JumpersListsItems)savedLists=list;
    check(savedLists,"season has a saved roster list");savedLists->setupListModel();
    QTimer::singleShot(100,&manager,[&manager]{auto *editor=manager.findChild<JumpersListEditorWindow *>();check(editor,"saved roster editor opens");editor->setCountryLimits({{"POL",3}});editor->accept();});
    savedLists->listViewDoubleClicked(savedLists->getListModel()->index(0));
    check(save.getJumpersListsReference().first().countryLimit("POL")==3,"accepting a roster editor saves its advisory limits in the season");
    {
        const QString first=manager.createRoster(),second=manager.createRoster();
        check(!first.isEmpty() && !second.isEmpty() && first!=second && save.getJumpersLists().size()==3,"inline roster creation publishes distinct persistent lists");
        QTest::qWait(150);auto *rosterScene=scene(&manager);
        check(item(rosterScene->rootObject(),"seasonRostersWorkspace"),"new roster workspace opens inline");
        presentation(&manager)->grab().save(output+"/season-rosters-inline.png");
        clickControl(rosterScene,"rosterCreate");QTest::qWait(80);const auto fromUi=manager.editingRosterId();
        check(!fromUi.isEmpty() && fromUi!=second && save.getJumpersLists().size()==4,"visible new-roster action creates and selects a list");
        check(manager.removeRoster(fromUi),"UI-created roster can be removed");QTest::qWait(50);

        check(manager.renameRoster(first,"Test roster") && manager.selectRoster(first),"inline roster can be renamed and selected by identity");QTest::qWait(50);
        auto *editor=qobject_cast<QuickRoster *>(manager.rosterEditor());check(editor,"inline roster backend exists");
        const auto candidateName="rosterInclude-"+QuickRoster::key(save.getJumpersReference().first());
        auto *candidate=item(rosterScene->rootObject(),candidateName);check(candidate && candidate->isVisible(),"candidate action is visible in the inline workspace");
        clickControl(rosterScene,candidateName);QTest::qWait(60);
        check(manager.selectRoster(second) && manager.selectRoster(first),"switching inline rosters keeps their identities");QTest::qWait(50);
        check(editor->count()==1,"inline roster members survive selection changes");
        check(manager.removeRoster(first) && manager.restoreRoster(),"deleted inline roster can be restored");QTest::qWait(50);
        check(manager.editingRosterId()==first && editor->count()==1,"restoration preserves roster identity and members");
        check(manager.removeRoster(first) && manager.removeRoster(second) && save.getJumpersLists().size()==1,"repeated inline removal leaves existing lists intact");
    }
    SaveJumpersList emptyRoster("Empty");
    save.getJumpersListsReference().append(emptyRoster);
    check(save.setActiveJumpersListId(emptyRoster.getId()) && save.activeJumpers()==save.getJumpersReference(),"an empty roster includes all athletes");
    save.setActiveJumpersListId(QString());
    save.getJumpersListsReference().removeLast();
    if(!SimulationSave::activeRosterFeatureEnabled()) {
        auto safeLegacy=SimulationSave::getJsonObject(save);auto legacyLists=safeLegacy.value("jumpers-lists").toArray();
        auto legacyEntry=legacyLists.at(0).toObject();legacyEntry["is-default"]=true;legacyLists[0]=legacyEntry;safeLegacy["jumpers-lists"]=legacyLists;
        check(!safeLegacy.contains("active-jumpers-list-id"),"disabled active-roster work does not change the save format");
        std::unique_ptr<SimulationSave> safeReload(SimulationSave::getFromJson(safeLegacy,nullptr));
        check(safeReload && safeReload->getJumpersLists().size()==1 && safeReload->getJumpersLists().first().getIsDefault(),"disabled active-roster work preserves legacy list behavior");
    }
    if(SimulationSave::activeRosterFeatureEnabled()) {
    const auto rosterId=save.getJumpersListsReference().first().getId();
    SeasonCalendar assigned("Roster calendar");assigned.setJumpersListId(rosterId);
    auto assignedJson=SeasonCalendar::getJsonObject(assigned);
    check(assignedJson.value("jumpers-list-id").toString()==rosterId,"calendar roster association is saved by identity");
    auto assignedReload=SeasonCalendar::getFromJson(assignedJson,nullptr);
    check(assignedReload.jumpersListId()==rosterId,"calendar roster association survives loading");

    check(save.setActiveJumpersListId(rosterId) && save.activeJumpers()==save.getJumpersListsReference().first().getJumpers(),"selecting a roster uses exactly its members");
    check(!save.setActiveJumpersListId(QUuid::createUuid().toString(QUuid::WithoutBraces)) && save.activeJumpersListId()==rosterId,"an unknown roster cannot replace the active one");
    std::unique_ptr<SimulationSave> activeReload(SimulationSave::getFromJson(SimulationSave::getJsonObject(save),nullptr));
    check(activeReload && activeReload->activeJumpersListId()==rosterId && activeReload->activeJumpers().size()==1,"the active roster survives saving and loading");
    check(save.setActiveJumpersListId(QString()) && save.activeJumpers()==save.getJumpersReference(),"the all-jumpers choice is available without a saved roster");
    auto legacyRosterSave=SimulationSave::getJsonObject(save);legacyRosterSave.remove("active-jumpers-list-id");
    std::unique_ptr<SimulationSave> legacyRosterReload(SimulationSave::getFromJson(legacyRosterSave,nullptr));
    check(legacyRosterReload && legacyRosterReload->getJumpersLists().size()==2 && legacyRosterReload->activeJumpers().size()==save.getJumpersReference().size()-1,"legacy include/exclude lists migrate without changing their previous selection");
    }
    bridge->edit(bridge->root()->values().value("id").toInt(),0);QTest::qWait(150);
    check(item(scene(&manager)->rootObject(),"seasonAutosave"),"season overview exposes the automatic saving switch");
    presentation(&manager)->grab().save(output+"/season-autosave.png");
    clickControl(scene(&manager),"seasonAutosave");check(!manager.autosaveEnabled(),"the visible switch disables automatic saving");
    clickControl(scene(&manager),"seasonAutosave");check(manager.autosaveEnabled(),"the visible switch enables automatic saving");
    auto *timer=manager.findChild<QTimer *>("seasonAutosaveTimer");check(timer && timer->interval()==15000,"automatic saving uses a fifteen second idle delay");
    manager.setAutosaveEnabled(false);check(!manager.autosaveEnabled() && !timer->isActive(),"turning off automatic saving cancels the pending save");
    SimulationSave anotherSave;check(anotherSave.getAutosaveEnabled(),"automatic saving preference is independent for each season save");
    QFile saved(QCoreApplication::applicationDirPath()+"/simulationSaves/"+saveName+".json");
    auto stored=[&](){check(saved.open(QIODevice::ReadOnly),"season autosave file is readable");const auto root=QJsonDocument::fromJson(saved.readAll()).object();saved.close();return root.value("simulation-save").toObject();};
    check(!stored().value("autosave-enabled").toBool(true),"automatic saving preference is stored in this season");
    check(stored().value("jumpers-lists").toArray().at(0).toObject().value("country-limits").toObject().value("POL").toInt()==3,"season file stores roster country limits");
    std::unique_ptr<SimulationSave> disabledReload(SimulationSave::getFromJson(stored(),nullptr));
    check(disabledReload && !disabledReload->getAutosaveEnabled(),"disabled automatic saving survives reloading the season");
    manager.markSeasonChanged();check(!timer->isActive(),"disabled automatic saving does not start a timer");
    manager.setAutosaveEnabled(true);timer->setInterval(400);save.setShowForm(false);
    auto *calendarEditor=manager.findChild<CalendarEditorWidget *>();check(calendarEditor && QMetaObject::invokeMethod(calendarEditor,"changed"),"calendar changes can schedule saving");
    check(timer->isActive(),"a calendar change schedules automatic saving");
    QTest::qWait(50);manager.markSeasonChanged();QTest::qWait(50);check(timer->isActive(),"a further change restarts the idle delay");
    QTest::qWait(550);check(!timer->isActive() && !stored().value("show-form").toBool(true),"the idle timer saves the changed season");
    manager.reject();main.resize(1024,640);
    qInfo()<<"PASS: season v2 profile editors, ownership, reset, validation and insertion";
}

static void simulationV2SeasonCreationRegressions(MainWindow &main)
{
    QTemporaryDir storage;check(storage.isValid(),"isolated canonical season creation");
    SimulationV2::CatalogPaths paths{storage.path()+"/athletes",storage.path()+"/hills",storage.path()+"/catalog.json",storage.path()+"/photos"};
    SimulationV2::ProfilePackRepository athletes(SimulationV2::ProfilePackKind::Athletes,paths.athletes),hills(SimulationV2::ProfilePackKind::Hills,paths.hills);
    QJsonArray entries;for(int i=0;i<8;++i){auto p=AthleteProfile::create();p.name="Season";p.surname=QString::number(i);p.country=i<4?"POL":"NOR";p.overall=70+i;p.form=60+i;p.perks={{1,-2,3}};entries.append(p.toJson());}
    QJsonObject pack{{"format","sj-sim-athlete-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},{"name","Season athletes"},{"profiles",entries}};
    auto venue=HillProfile::create();venue.name="Season venue";venue.perks={{1,-1,2,-3}};
    QJsonObject hillPack{{"format","sj-sim-hill-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},{"name","Season venues"},{"profiles",QJsonArray{venue.toJson()}}};
    QString error;check(athletes.save(pack,error) && hills.save(hillPack,error),"canonical season fixtures saved");
    const auto globals=GlobalDatabase::get()->packsSnapshot();QSettings().remove("newSimulation");
    NewSimulationSaveConfigurationWindow config({},&main,NewSimulationSaveConfigurationWindow::DataSource::Canonical,paths);
    config.findChild<QLineEdit *>("lineEdit_name")->setText("Canonical season");
    check(!config.createSave() && !config.findChild<QPushButton *>("pushButton_submit")->isEnabled(),"season cannot be created while its owned catalog is loading");
    QElapsedTimer loading;loading.start();while(!config.catalogReady() && loading.elapsed()<15000)QTest::qWait(10);
    check(config.catalogReady() && config.engineVersion()==2,"new season uses the canonical catalog and new engine by default");
    auto *selector=config.findChild<QComboBox *>("simulationPack0");selector->setCurrentIndex(selector->findData(pack["id"].toString()));
    auto *hillSelector=config.findChild<QComboBox *>("simulationPack1");hillSelector->setCurrentIndex(hillSelector->findData(hillPack["id"].toString()));
    check(config.selectedPack(DataPack::Jumpers).jumpers.size()==8 && config.findChild<QLabel *>("simulationPackCount0")->text().contains('8'),"canonical pack choice updates the season count");
    auto modified=pack;auto changed=entries;auto first=changed[0].toObject();first["overall"]=99;changed[0]=first;modified["profiles"]=changed;check(athletes.save(modified,error),"later authoring edit fixture");
    auto save=config.createSave();check(save && save->getEngineVersion()==2 && save->getJumpersReference().size()==8 && save->getHillsReference().size()==1,"production season factory builds complete owned v2 data");
    check(save->getJumpersReference()[0]->getSimulationProfile()->overall==70 && save->getJumpersReference()[0]->getSimulationAssetDirectory()==QDir(paths.athletes).filePath(pack["id"].toString()),"season creation keeps its chosen snapshot and portable photo context");
    render(&config,"new-canonical-season",true);config.reject();
    NewSimulationSaveConfigurationWindow fresh({},&main,NewSimulationSaveConfigurationWindow::DataSource::Canonical,paths);
    loading.restart();while(!fresh.catalogReady() && loading.elapsed()<15000)QTest::qWait(10);
    check(fresh.catalogReady() && fresh.selectedPack(DataPack::Jumpers).jumpers.first().getSimulationProfile()->overall==99,"next creation rereads canonical authoring files");fresh.reject();
    CompetitionRules rules;rules.setRounds({RoundInfo(8,true)});rules.setHasDsq(false);
    auto *calendar=new SeasonCalendar("Season weather");
    auto *individual=new CompetitionInfo(save->getHillsReference()[0]);individual->setRules(rules);individual->setSerieType(CompetitionInfo::Training);individual->setScheduledDate(QDate(2027,1,4));
    rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(4);
    auto *team=new CompetitionInfo(save->getHillsReference()[0]);team->setRules(rules);team->setScheduledDate(QDate(2027,1,5));
    CompetitionRules qualificationRules;bool foundQualification=false;
    for(const auto &preset:GlobalDatabase::get()->getEditableCompetitionRules())if(preset.getName()=="Qualifications"){qualificationRules=preset;foundQualification=true;break;}
    check(foundQualification,"Qualifications preset is available");qualificationRules.setHasDsq(false);
    auto *qualification=new CompetitionInfo(save->getHillsReference()[0]);qualification->setRules(qualificationRules);qualification->setSerieType(CompetitionInfo::Qualifications);qualification->setScheduledDate(QDate(2027,1,6));
    calendar->setCompetitions({individual,team,qualification});save->getActualSeason()->setCalendars({calendar});save->getActualSeason()->setActualCalendar(calendar);save->getActualSeason()->setDatesEnabled(true);
    Season empty;empty.setSeasonNumber(2025);save->getSeasonsReference().append(empty);save->setActualSeason(&save->getSeasonsReference().first());save->updateNextCompetitionIndex();
    {
        SaveJumpersList firstRoster("First"),secondRoster("Calendar roster");firstRoster.setJumpers(save->getJumpersReference().mid(0,4));secondRoster.setJumpers(save->getJumpersReference().mid(4,4));save->setJumpersLists({firstRoster,secondRoster});save->setActiveJumpersListId(firstRoster.getId());calendar->setJumpersListId(secondRoster.getId());
        calendar->setJumpersListId(QUuid::createUuid().toString(QUuid::WithoutBraces));
        {
            CompetitionConfigWindow stale(CompetitionConfigWindow::SeasonCompetition,&main,save.get());
            check(stale.getSeasonCompetitionJumpers()==firstRoster.getJumpers() && calendar->jumpersListId().isEmpty(),"a missing calendar roster falls back to the current active list and clears the stale assignment");stale.reject();
        }
        calendar->setJumpersListId(secondRoster.getId());
        {
            CompetitionConfigWindow setup(CompetitionConfigWindow::SeasonCompetition,&main,save.get());setup.show();QTest::qWait(100);
            check(setup.getSeasonCompetitionJumpers()==secondRoster.getJumpers() && save->activeJumpersListId()==secondRoster.getId(),"calendar assignment activates its own exact athlete list");
            auto *bridge=setup.findChild<QuickBridge *>();auto *picker=bridge?bridge->findChild<QuickRoster *>():nullptr;check(picker,"competition roster bridge");picker->applyPreset(0,0);
            check(setup.getSeasonCompetitionJumpers().size()==8 && calendar->jumpersListId()==secondRoster.getId() && save->activeJumpersListId()==secondRoster.getId(),"manual preparation can add other athletes without changing the calendar assignment");setup.reject();
        }
        save->setNextCompetition(team);save->setNextCompetitionIndex(1);
        {
            CompetitionConfigWindow setup(CompetitionConfigWindow::SeasonCompetition,&main,save.get());setup.show();QTest::qWait(100);
            check(setup.getCompetitionTeams().size()==1 && setup.getCompetitionTeams().first().getCountryCode()=="NOR","team preparation uses the calendar roster instead of all season athletes");
            auto &members=setup.getCompetitionTeamsReference().first().getJumpersReference();std::swap(members[0],members[1]);const auto manual=members;
            bool asked=false;QTimer answer;answer.setSingleShot(true);QObject::connect(&answer,&QTimer::timeout,&setup,[&]{for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){asked=true;box->button(QMessageBox::No)->click();return;}});answer.start(50);setup.autoCompleteTeams();answer.stop();
            check(asked && members==manual,"automatic completion asks before replacing manual squads and cancellation preserves them");setup.reject();
        }
        calendar->setJumpersListId(QString());save->setActiveJumpersListId(QString());save->setJumpersLists({});save->updateNextCompetitionIndex();
    }
    SimulationSaveManagerWindow manager(save.get(),&main);manager.show();QTest::qWait(100);
    const bool oldLive=GlobalAppSettings::get()->getLiveCompetition();GlobalAppSettings::get()->setLiveCompetition(false);
    QTimer dialogs,runner;bool running=false;int phases=0,ticks=0;QSet<CompetitionConfigWindow *> configured;
    QObject::connect(&dialogs,&QTimer::timeout,&manager,[&]{check(++ticks<1500,"season competition finishes without hanging");for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){auto *answer=box->button(QMessageBox::Yes);if(!answer)answer=box->button(QMessageBox::Ok);if(answer)answer->click();else box->accept();}});
    QObject::connect(&runner,&QTimer::timeout,&manager,[&]{
        if(running)return;
        for(auto *widget:QApplication::allWidgets()) {
            if(auto *setup=qobject_cast<CompetitionConfigWindow *>(widget);setup && setup->isVisible() && !configured.contains(setup)) {
                configured.insert(setup);check(setup->getWeatherSetup() && setup->getWeatherSetup()->field(),"season configuration owns the same continuous weather editor as standalone contests");
                setup->getWeatherSetup()->applyPreset(4);setup->getWeatherSetup()->setValue("precipitationKind",1);setup->getWeatherSetup()->setValue("precipitationLevel",1);
                check(setup->findChild<QSpinBox *>("spinBox_startGate")->value()<=200 && setup->property("suggestedGate").isValid(),"season has a concrete bounded gate and cached weather suggestion");
                running=true;QMetaObject::invokeMethod(setup,"on_pushButton_submit_clicked",Qt::DirectConnection);running=false;break;
            }
            if(auto *contest=qobject_cast<CompetitionManagerWindow *>(widget);contest && contest->isVisible()) {
                running=true;++phases;contest->autoSimulateCompetition();int jumps=0;QSet<int> times;
                for(const auto &result:contest->getManager()->getResults()->getResultsReference())for(const auto &jump:result.getJumps()) {
                    const auto data=jump.getSimulationV2Data();check(!data.isEmpty() && data["precipitationKind"].toInt()==1,"season jumps consume accepted rain and persist new engine metadata");
                    check(data["event"].toInt()==(phases==1?int(SimulationV2::EventKind::Training):phases==3?int(SimulationV2::EventKind::Qualification):int(SimulationV2::EventKind::Competition)),"season engine uses actual event importance");
                    times.insert(data["startSeconds"].toInt());++jumps;
                }
                check(jumps==8 && times.size()==8 && contest->getSimulationSeconds()==240,"individual and team seasons advance the shared weather clock exactly once per athlete");
                contest->findChild<QPushButton *>("pushButton_goToNext")->click();running=false;break;
            }
        }
    });
    dialogs.start(20);runner.start(30);
    for(int i=0;i<3;++i){configured.clear();check(QMetaObject::invokeMethod(&manager,"on_pushButton_competitionConfig_clicked",Qt::DirectConnection),"normal season launch slot runs");}
    runner.stop();dialogs.stop();
    check(phases==3 && individual->getPlayed() && team->getPlayed() && qualification->getPlayed(),"normal season workflow finishes both event types");
    std::unique_ptr<SimulationSave> restored(SimulationSave::getFromJson(SimulationSave::getJsonObject(*save),nullptr));
    check(restored && restored->getEngineVersion()==2 && restored->getActualSeason()->getActualCalendar()->getCompetitionsReference()[0]->getPlayed(),"completed canonical season reloads with played events");
    check(GlobalDatabase::get()->packsSnapshot()==globals,"season creation and competition never mutate global source packs");
    {
        CompetitionRules koRules;bool foundKO=false;
        for(const auto &preset:GlobalDatabase::get()->getEditableCompetitionRules())if(preset.getName()=="Classical KO"){koRules=preset;foundKO=true;break;}
        check(foundKO,"Classical KO preset is available for qualification transition regression");
        auto *koEvent=new CompetitionInfo(save->getHillsReference()[0]);koEvent->setRules(koRules);koEvent->setScheduledDate(QDate(2027,1,6));koEvent->setAdvancementCompetition(qualification);
        calendar->getCompetitionsReference().append(koEvent);save->updateNextCompetitionIndex();
        CompetitionConfigWindow koSetup(CompetitionConfigWindow::SeasonCompetition,&manager,save.get());koSetup.show();QTest::qWait(120);
        check(!koSetup.getSeasonCompetitionJumpers().isEmpty(),"played individual qualification feeds a nonempty Classical KO start list");
        check(KOGroup::getJumpersFromGroups(&koSetup.getSeasonCompetitionGroupsReference()).size()==koSetup.getSeasonCompetitionJumpers().size(),"Classical KO prepares groups for every qualified jumper");
        koSetup.reject();
        {
            std::unique_ptr<SimulationSave> reopened(SimulationSave::getFromJson(SimulationSave::getJsonObject(*save),nullptr));
            check(reopened && reopened->getNextCompetition(),"qualification transition reload fixture");
            CompetitionConfigWindow reloadedKO(CompetitionConfigWindow::SeasonCompetition,&main,reopened.get());reloadedKO.show();QTest::qWait(120);
            check(reloadedKO.getSeasonCompetitionJumpers().size()==koSetup.getSeasonCompetitionJumpers().size(),"qualified identities survive saving and loading before Classical KO");
            reloadedKO.reject();
        }
        calendar->getCompetitionsReference().removeAll(koEvent);delete koEvent;save->updateNextCompetitionIndex();
    }
    for(int n=0;n<7;++n){
        auto *event=new CompetitionInfo(save->getHillsReference()[0]);event->setRules(rules);event->setScheduledDate(QDate(2027,1,6+n));
        if(n==0){auto *training=new CompetitionInfo(event->getHill());training->setSerieType(CompetitionInfo::Training);training->setRules(rules);training->setScheduledDate(event->getScheduledDate());event->setTrainings({training});auto *trial=new CompetitionInfo(event->getHill());trial->setSerieType(CompetitionInfo::TrialRound);trial->setRules(rules);trial->setScheduledDate(event->getScheduledDate());event->setTrialRound(trial);}
        calendar->getCompetitionsReference().append(event);
    }
    calendar->normalizeCompetitionEvents();manager.refreshCalendarState();auto *overviewBridge=manager.findChild<QuickBridge *>();overviewBridge->refresh();QTest::qWait(80);
    auto *overviewNode=qobject_cast<QuickNode *>(overviewBridge->find("quickSeasonOverview"));check(overviewNode,"season overview has a dedicated data projection");
    const auto upcoming=overviewNode->values()["events"].toList();check(upcoming.size()==5,"overview projects five upcoming main events, not seven or separate training rows");
    check(upcoming[0].toMap()["trainingCount"].toInt()==1 && upcoming[0].toMap()["trial"].toBool() && !upcoming[0].toMap()["flag"].toString().isEmpty(),"training, trial and flag stay inside the parent competition preview");
    render(&manager,"season-overview-upcoming",true);
    GlobalAppSettings::get()->setLiveCompetition(oldLive);manager.reject();main.resize(1024,640);QSettings().remove("newSimulation");
    qInfo()<<"PASS: canonical season creation, owned source snapshot, individual/team runtime, continuous weather and persisted results";
}

static void exampleSeasonRegressions(MainWindow &main) {
    QTemporaryDir data;check(data.isValid(),"example season isolated storage");
    auto *database=GlobalDatabase::get();const auto originalSaves=database->getEditableGlobalSimulationSaves();const auto packs=database->packsSnapshot();const bool loaded=database->getLoadedSimulationSaves();
    const auto oldName=QCoreApplication::applicationName();QCoreApplication::setApplicationName("ExampleSeasonTest-"+QUuid::createUuid().toString(QUuid::WithoutBraces));database->getEditableGlobalSimulationSaves().clear();database->setLoadedSimulationSaves(true);const auto privateCatalogRoot=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto oldRoot=qApp->property("exampleSeasonStorageRoot");qApp->setProperty("exampleSeasonStorageRoot",data.path());QSettings().remove("onboarding/welcomeDismissed");
    QFile occupied(data.path()+"/Przykładowy sezon.json");check(occupied.open(QIODevice::WriteOnly) && occupied.write("existing save")>0,"existing file name fixture");occupied.close();
    main.show();main.resize(1280,800);auto *bridge=main.findChild<QuickBridge *>();bridge->refresh();QTest::qWait(50);check(bridge->context()["showWelcome"].toBool() && item(scene(&main)->quickWindow()->contentItem(),"homeWelcomeExample"),"first launch offers the example directly");
    render(&main,"first-launch-welcome",true);
    QStringList errors;QTimer dismissErrors;QObject::connect(&dismissErrors,&QTimer::timeout,&main,[&]{for(auto *widget:QApplication::allWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){errors.append(box->text());box->accept();}});dismissErrors.start(20);
    QString firstName;
    for(int run=0;run<2;++run) {
        bridge->launch("example");QElapsedTimer timer;timer.start();SimulationSaveManagerWindow *manager=nullptr;
        while(!manager && errors.isEmpty() && timer.elapsed()<30000){QTest::qWait(20);for(auto *window:main.findChildren<SimulationSaveManagerWindow *>())if(window->isVisible()){manager=window;break;}}
        check(manager,qPrintable("one-click example opens its season: "+errors.join("; ")));auto *save=manager->getSimulationSave();
        check(save->getEngineVersion()==2 && save->getActualSeason()->getCalendarsReference().size()==1 && save->getNextCompetition(),"example owns complete v2 packs and one playable calendar");
        check(!save->getJumpersReference().isEmpty() && !save->getHillsReference().isEmpty() && save->getJumpersReference()[0]->getSimulationProfile(),"example uses familiar ready athlete and hill data");
        auto *calendar=save->getActualSeason()->getActualCalendar();check(calendar && calendar->getCompetitionsReference().size()>20 && !calendar->getClassificationsReference().isEmpty(),"example includes a full winter calendar and classification");
        for(auto *event:calendar->getCompetitionsReference())check(!event->getPlayed() && save->getHillsReference().contains(event->getHill()),"every example event references an owned hill and starts unplayed");
        if(run==0)firstName=save->getName();else check(save->getName()!=firstName,"another example uses a unique save name");
        check(QFileInfo::exists(data.path()+"/"+save->getName()+".json"),"one-click example is saved before opening");manager->reject();QTest::qWait(80);
    }
    dismissErrors.stop();check(occupied.open(QIODevice::ReadOnly) && occupied.readAll()=="existing save","example never overwrites an occupied save file");occupied.close();bridge->refresh();check(!bridge->context()["showWelcome"].toBool() && item(scene(&main)->rootObject(),"homeExample"),"welcome dismisses after success while the menu retains the example action");
    check(database->packsSnapshot()==packs,"examples never modify global authoring packs");qDeleteAll(database->getEditableGlobalSimulationSaves());database->getEditableGlobalSimulationSaves()=originalSaves;database->setLoadedSimulationSaves(loaded);qApp->setProperty("exampleSeasonStorageRoot",oldRoot);QCoreApplication::setApplicationName(oldName);QDir(privateCatalogRoot).removeRecursively();bridge->refresh();
    qInfo()<<"PASS: first-launch welcome, one-click winter season, owned packs, playable events, unique names and occupied-file protection";
}
