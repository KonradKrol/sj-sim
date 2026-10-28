#pragma once

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
        }
        dialogs.stop();window.reject();
    }
    GlobalAppSettings::get()->setLiveCompetition(previousLive);
    qInfo()<<"PASS: continuous contest clock, manual/DNS/waiting, N/group/round/contest and KO weather";
}
