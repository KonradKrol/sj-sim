#pragma once

#include "../source/simulation-v2/ProfileDraft.h"

static void simulationV2SeasonEditorRegressions(MainWindow &main)
{
    QString error;
    auto profile=AthleteProfile::create();profile.name="Season";profile.surname="Athlete";profile.overall=83;profile.form=61;profile.perks={{1,-2,3}};
    auto venue=HillProfile::create();venue.name="Season hill";venue.perks={{1,-1,2,-3}};
    Jumper source;Hill sourceHill;
    check(source.setSimulationProfile(profile,error) && sourceHill.setSimulationProfile(venue,error),"season editor fixtures validate");
    source.setSimulationPackContext(QUuid::createUuid().toString(QUuid::WithoutBraces),"/test/pack");
    SimulationSave save;save.setEngineVersion(2);save.setName("Season editor");
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
    calendar->setCompetitions({individual,team});save->getActualSeason()->setCalendars({calendar});save->getActualSeason()->setActualCalendar(calendar);save->getActualSeason()->setDatesEnabled(true);
    Season empty;empty.setSeasonNumber(2025);save->getSeasonsReference().append(empty);save->setActualSeason(&save->getSeasonsReference().first());save->updateNextCompetitionIndex();
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
                    check(data["event"].toInt()==(phases==1?int(SimulationV2::EventKind::Training):int(SimulationV2::EventKind::Competition)),"season engine uses actual event importance");
                    times.insert(data["startSeconds"].toInt());++jumps;
                }
                check(jumps==8 && times.size()==8 && contest->getSimulationSeconds()==240,"individual and team seasons advance the shared weather clock exactly once per athlete");
                contest->findChild<QPushButton *>("pushButton_goToNext")->click();running=false;break;
            }
        }
    });
    dialogs.start(20);runner.start(30);
    for(int i=0;i<2;++i){configured.clear();check(QMetaObject::invokeMethod(&manager,"on_pushButton_competitionConfig_clicked",Qt::DirectConnection),"normal season launch slot runs");}
    runner.stop();dialogs.stop();
    check(phases==2 && individual->getPlayed() && team->getPlayed(),"normal season workflow finishes both event types");
    std::unique_ptr<SimulationSave> restored(SimulationSave::getFromJson(SimulationSave::getJsonObject(*save),nullptr));
    check(restored && restored->getEngineVersion()==2 && restored->getActualSeason()->getActualCalendar()->getCompetitionsReference()[0]->getPlayed(),"completed canonical season reloads with played events");
    check(GlobalDatabase::get()->packsSnapshot()==globals,"season creation and competition never mutate global source packs");
    GlobalAppSettings::get()->setLiveCompetition(oldLive);manager.reject();main.resize(1024,640);QSettings().remove("newSimulation");
    qInfo()<<"PASS: canonical season creation, owned source snapshot, individual/team runtime, continuous weather and persisted results";
}
