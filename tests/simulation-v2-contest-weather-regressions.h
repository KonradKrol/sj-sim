#pragma once
#include "../source/UI/Quick/QuickWindProfile.h"
#include "../source/global/GlobalTranslators.h"

static void simulationV2ContestWeatherRegressions(MainWindow &main) {
    const bool previousLive=GlobalAppSettings::get()->getLiveCompetition();GlobalAppSettings::get()->setLiveCompetition(false);
    for(int mode=0;mode<6;++mode) {
        const bool teams=mode==4,ko=mode==5;
        QVector<Jumper> athletes;athletes.reserve(4);QVector<Jumper *> pointers;QString error;
        for(int i=0;i<4;++i) {
            auto profile=AthleteProfile::create();profile.name="Weather";profile.surname=QString::number(i);profile.personalBest=200;profile.country=i<2?"POL":"NOR";
            Jumper jumper;check(jumper.setSimulationProfile(profile,error),"new contest athlete fixture");athletes.append(jumper);pointers.append(&athletes.last());
        }
        auto profile=HillProfile::create();profile.name="Contest weather";profile.record=200;Hill hill;check(hill.setSimulationProfile(profile,error),"new contest hill fixture");
        CompetitionInfo info(&hill);CompetitionRules rules;rules.setCompetitionType(teams?CompetitionRules::Team:CompetitionRules::Individual);rules.setJumpersInTeamCount(2);rules.setHasDsq(false);rules.setHasJudgesPoints(true);rules.setWindAverageCalculatingType(WindsCalculator::Original);
        rules.setRounds({RoundInfo(teams?2:4,true),RoundInfo(2,true)});
        if(ko){rules.getRoundsReference()[0].setKO(true);rules.getRoundsReference()[0].setCountInKOGroup(2);rules.getRoundsReference()[0].setAdvancingFromKOGroup(1);}
        info.setRules(rules);
        std::unique_ptr<AbstractCompetitionManager> manager;
        if(teams)manager=std::make_unique<TeamCompetitionManager>();else manager=std::make_unique<IndividualCompetitionManager>();
        manager->setCompetitionInfo(&info);manager->setCompetitionRules(info.getRulesPointer());manager->setResults(&info.getResultsReference());manager->setActualRound(1);manager->setActualGate(10);manager->getRoundsStartingGatesReference().append(10);
        if(teams) {
            Team a("POL"),b("NOR");a.setJumpers(pointers.mid(0,2));b.setJumpers(pointers.mid(2,2));info.setTeams({a,b});info.getTeamsReference().detach();
            auto *teamManager=static_cast<TeamCompetitionManager *>(manager.get());QVector<Team *> entries;for(auto &team:info.getTeamsReference())entries.append(&team);
            teamManager->getRoundsTeamsReference().append(entries);teamManager->setActualGroup(1);teamManager->setupStartListStatusesForActualRound(true);
        } else {
            auto *individual=static_cast<IndividualCompetitionManager *>(manager.get());
            info.getRoundsKOGroupsReference().append(ko?KOGroup::constructKOGroups(&info.getRulesPointer()->getRoundsReference()[0],&pointers,KOGroup::Classic,&info):QVector<KOGroup>());
            individual->getRoundsKOGroupsReference().append(&info.getRoundsKOGroupsReference().first());
            individual->setFirstRoundJumpers(ko?KOGroup::getJumpersFromGroups(&info.getRoundsKOGroupsReference().first()):pointers);individual->setupStartListStatusesForActualRound();
        }
        manager->setActualStartListIndex(0);manager->setActualJumperToNextUnfinished();
        SimulationWeatherSetup setup;setup.setHill(profile);setup.applyPreset(2);
        CompetitionManagerWindow window(manager.get(),&main,true);window.bindWeatherSetup(&setup,123456);
        check(window.findChild<QSpinBox *>("spinBox_actualGate")->maximum()==200,"new contest gate UI respects the engine range");
        QTimer dialogs;QObject::connect(&dialogs,&QTimer::timeout,&window,[&]{for(auto *widget:QApplication::topLevelWidgets()) {
            if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){auto *answer=box->button(QMessageBox::Yes);if(!answer)answer=box->button(QMessageBox::Ok);if(answer)answer->click();else box->accept();}
            if(auto *input=qobject_cast<QInputDialog *>(widget);input && input->isVisible()){input->setIntValue(2);input->accept();}
        }});dialogs.start(20);
        if(mode==0) {
            auto *first=manager->getActualJumper();QMetaObject::invokeMethod(&window,"on_pushButton_jump_clicked",Qt::DirectConnection);
            auto *second=manager->getActualJumper();window.skipCurrentJump();
            auto *third=manager->getActualJumper();QMetaObject::invokeMethod(&window,"on_pushButton_jump_clicked",Qt::DirectConnection);
            const auto a=info.getResultsReference().getResultOfIndividualJumper(first)->getJumps().last();const auto b=info.getResultsReference().getResultOfIndividualJumper(second)->getJumps().last();const auto c=info.getResultsReference().getResultOfIndividualJumper(third)->getJumps().last();
            check(a.getSimulationV2Data()["startSeconds"]==0 && b.getSimulationV2Data()["startSeconds"]==30 && c.getSimulationV2Data()["startSeconds"]==60,"manual jumps and DNS share the continuous logical clock");
            check(b.getDNS() && !b.getDSQ() && b.getPoints()==0 && window.getSimulationSeconds()==90,"DNS keeps zero points and advances weather without disqualification");
            QMetaObject::invokeMethod(&window,"on_pushButton_generateNewWinds_clicked",Qt::DirectConnection);check(window.getSimulationSeconds()==120,"waiting advances the field rather than independently rerolling it");
            const auto historical=a.getSimulationV2Data();setup.applyPreset(0);
            check(window.getSimulationSeconds()==120 && info.getResultsReference().getResultOfIndividualJumper(first)->getJumps().last().getSimulationV2Data()==historical,"changing conditions keeps the clock and historical jump weather intact");
            window.show();QTest::qWait(150);bool editorOpened=false;int editorTicks=0;QTimer closeEditor;
            auto *bridge=window.findChild<QuickBridge *>();bridge->refresh();QTest::qWait(80);
            const auto recorded=bridge->jumpSummary().value("windProfile").toMap();
            check(recorded==QuickWindProfile::fromMetadata(c.getSimulationV2Data()) && recorded.value("during").toList().size()==6,"visible jump wind comes from that jump's recorded exposure, independent of changed current weather");
            clickControl(scene(&window),"lastJumpWind");QTest::qWait(80);
            auto *loader=item(scene(&window)->rootObject(),"jumpWindLoader");
            auto *wind=loader?loader->property("item").value<QObject *>():nullptr;
            auto *slope=item(scene(&window)->quickWindow()->contentItem(),"recordedWindSlope");
            check(wind && wind->property("visible").toBool() && slope && !slope->property("interactive").toBool(),"wind metric opens a read-only profile without enabling configuration edits");
            presentation(&window)->grab().save(output+"/recorded-wind-profile.png");
            auto *phase=item(scene(&window)->quickWindow()->contentItem(),"jumpWindPhase");check(phase,"recorded wind exposes three clear phases");
            QMetaObject::invokeMethod(phase,"selected",Q_ARG(int,0));QTest::qWait(20);
            const auto samples=[&]{const auto value=slope->property("samples");return value.userType()==qMetaTypeId<QJSValue>()?value.value<QJSValue>().toVariant().toList():value.toList();};
            check(samples()==recorded.value("before").toList(),"before phase uses the persisted pre-takeoff field");
            QMetaObject::invokeMethod(phase,"selected",Q_ARG(int,2));QTest::qWait(20);
            check(samples()==recorded.value("after").toList(),"after phase uses the persisted field at landing time");
            main.resize(360,480);QTest::qWait(80);
            check(wind->property("y").toDouble()>=0 && wind->property("y").toDouble()+wind->property("height").toDouble()<=scene(&window)->height(),"recorded wind popup fits the smallest viewport");
            presentation(&window)->grab().save(output+"/recorded-wind-profile-small.png");
            QMetaObject::invokeMethod(wind,"close");main.resize(1024,640);QTest::qWait(80);
            const auto forecast=bridge->context().value("windPreview").toList();
            check(forecast.size()==6 && forecast.first().toMap()["head"].toDouble()==setup.field()->wind(forecast.first().toMap()["distance"].toDouble(),120).head,"pre-jump preview samples the current field at the actual contest time");
            clickControl(scene(&window),"nextJumpWind");QTest::qWait(60);
            auto *nextLoader=item(scene(&window)->rootObject(),"nextJumpWindLoader");auto *nextWind=nextLoader?nextLoader->property("item").value<QObject *>():nullptr;
            check(nextWind && nextWind->property("visible").toBool() && nextWind->property("forecast").toBool() && window.getSimulationSeconds()==120,"optional pre-jump preview opens without advancing time or changing weather");
            QMetaObject::invokeMethod(nextWind,"close");QTest::qWait(40);
            QObject::connect(&closeEditor,&QTimer::timeout,&window,[&]{
                check(++editorTicks<500,"continuous weather editor opens without getting stuck");
                for(auto *widget:QApplication::allWidgets())if(auto *dialog=qobject_cast<QDialog *>(widget);dialog && dialog->objectName()=="continuousWeatherEditor" && dialog->isVisible()) {
                    auto *view=scene(dialog);if(!view || view->status()!=QQuickWidget::Ready)continue;
                    check(item(view->rootObject(),"weatherWorkspace") && setup.previewOrigin()==120,"runtime conditions use the same weather UI at current contest time");
                    editorOpened=true;closeEditor.stop();dialog->reject();return;
                }
            });closeEditor.start(20);
            QMetaObject::invokeMethod(&window,"on_pushButton_windsGeneratorSettings_clicked",Qt::DirectConnection);closeEditor.stop();
            check(editorOpened && setup.previewOrigin()==0 && window.getSimulationSeconds()==120,"closing conditions restores setup preview without advancing the contest");
        } else {
            if(mode==1)window.autoSimulateJumps();else if(mode==3)window.autoSimulateRound();else if(mode==4)window.autoSimulateGroup();else window.autoSimulateCompetition();
            int jumps=0;QSet<int> times;
            for(const auto &result:info.getResultsReference().getResultsReference())for(const auto &jump:result.getJumps()) {
                const auto data=jump.getSimulationV2Data();check(data["engineVersion"]==2,"every automatic jump uses the new engine and weather");times.insert(data["startSeconds"].toInt());++jumps;
            }
            if(!(jumps>0 && times.size()==jumps && window.getSimulationSeconds()==jumps*30))qCritical()<<"WEATHER CLOCK"<<mode<<jumps<<times<<window.getSimulationSeconds();
            check(jumps>0 && times.size()==jumps && window.getSimulationSeconds()==jumps*30,"automatic N/group/round/contest paths use one unique time slot per jump");
            if(mode==1 || mode==4)check(jumps==2,"N and team-group simulation keep their requested scope");
            if(mode==2 || ko)check(manager->checkCompetitionEnd(),"whole-competition simulation completes both normal and KO contests");
            if(mode==2) {
                auto *button=window.findChild<QPushButton *>("pushButton_goToNext");
                const auto original=QJsonDocument(CompetitionResults::getJsonObject(info.getResultsReference())).toJson();
                auto *app=qobject_cast<QApplication *>(QCoreApplication::instance());
                check(GlobalTranslators::get()->setLanguage(app,GlobalAppSettings::English),"English catalog loads for dynamic competition captions");QTest::qWait(30);
                check(button->text()=="Finish competition","already-visible end-competition button updates to English");
                check(QCoreApplication::translate("QObject","FIS — przybliżenie")==QString::fromUtf8("FIS — approximation"),"new measurement controls have a compiled English catalog entry");
                check(GlobalTranslators::get()->setLanguage(app,GlobalAppSettings::Czech),"Czech catalog loads for dynamic competition captions");QTest::qWait(30);
                check(button->text()==QString::fromUtf8("Ukončit závod"),"already-visible end-competition button updates to Czech");
                GlobalTranslators::get()->setLanguage(app,GlobalAppSettings::Polish);QTest::qWait(30);
                check(button->text()==QString::fromUtf8("Zakończ konkurs") && QJsonDocument(CompetitionResults::getJsonObject(info.getResultsReference())).toJson()==original,"language change restores Polish without resetting scores or competition state");
                QSignalSpy accepted(&window,&QDialog::accepted);button->click();
                check(accepted.count()==1,"retranslation cannot attach duplicate finish handlers");
                QDialog archive(&main);archive.setObjectName("competitionResultsArchive");auto *layout=new QVBoxLayout(&archive);
                auto *history=new ResultsHistoryWidget(&info,false,&archive);layout->addWidget(history);archive.show();QTest::qWait(100);
                auto *table=history->tableView();table->clicked(table->model()->index(0,0));
                auto *bridge=archive.findChild<QuickBridge *>();bridge->refresh();QTest::qWait(50);
                check(!bridge->jumpSummary().value("windProfile").toMap().isEmpty(),"archive jump card exposes recorded corridors through the same profile view");archive.reject();
            }
            if(mode==3 || mode==4) {
                auto *app=qobject_cast<QApplication *>(QCoreApplication::instance());
                GlobalTranslators::get()->setLanguage(app,GlobalAppSettings::English);QTest::qWait(20);
                GlobalTranslators::get()->setLanguage(app,GlobalAppSettings::Czech);QTest::qWait(20);
                GlobalTranslators::get()->setLanguage(app,GlobalAppSettings::Polish);QTest::qWait(20);
                window.findChild<QPushButton *>("pushButton_goToNext")->click();
                check(mode==3?manager->getActualRound()==2:static_cast<TeamCompetitionManager *>(manager.get())->getActualGroup()==2,"language changes cannot multiply next-round or next-group actions");
            }
        }
        dialogs.stop();window.reject();
    }
    GlobalAppSettings::get()->setLiveCompetition(previousLive);
    qInfo()<<"PASS: continuous contest clock, manual/DNS/waiting, N/group/round/contest and KO weather";
}
