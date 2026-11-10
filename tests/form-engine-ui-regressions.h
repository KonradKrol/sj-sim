#pragma once
#include "../source/form-engine/FormLab.h"

static void formEngineUiRegressions(MainWindow &main){
    FormLabWindow lab(&main);lab.show();QTest::qWait(150);
    auto *view=scene(&lab);check(view && view->status()==QQuickWidget::Ready && item(view->rootObject(),"formEngineLabScreen"),"form lab QML loads");
    auto *controller=lab.findChild<FormLabController *>();check(controller,"form lab controller exists");
    controller->setOption("count",12);controller->setOption("days",30);controller->setOption("repeats",2);controller->setOption("threads",2);
    clickControl(view,"formParameter-perk");QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"-3");QTest::keyClick(view,Qt::Key_Return);QTest::qWait(30);
    check(controller->options().value("perk").toInt()==-3,"negative form perk can be typed without reverting the draft");
    clickControl(view,"formLabRun");for(int i=0;i<500 && controller->busy();++i)QTest::qWait(20);
    check(!controller->busy() && controller->result().contains("views") && controller->progress()==1,"asynchronous lab finishes with complete progress and results");
    const auto result=controller->result();controller->start();for(int i=0;i<500 && controller->busy();++i)QTest::qWait(20);
    check(controller->result()==result,"identical lab seed and inputs reproduce the same result");
    controller->freezeReference();controller->setParameter("formDeviation",4);controller->setOption("compare",true);controller->start();for(int i=0;i<500 && controller->busy();++i)QTest::qWait(20);
    check(controller->result().value("metrics").toList().size()==2,"A/B form lab produces both parameter sets");
    auto *screen=item(view->rootObject(),"formEngineLabScreen");screen->setProperty("page",2);QTest::qWait(40);
    auto *group=item(view->rootObject(),"formLabGroup");group->setProperty("currentIndex",2);QTest::qWait(40);
    clickControl(view,"formParameter-formHalfLife");QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"7,5");QTest::keyClick(view,Qt::Key_Return);QTest::qWait(40);
    check(controller->config().value("values").toMap().value("formHalfLife").toDouble()==7.5,"localized decimal parameter commits without reverting the input");screen->setProperty("page",0);
    main.resize(1024,640);QTest::qWait(100);view->grabFramebuffer();presentation(&lab)->grab().save(output+"/form-lab.png");
    controller->setOption("usePack",true);screen->setProperty("page",1);QTest::qWait(80);
    auto *athletes=qobject_cast<QAbstractItemModel *>(controller->athleteRows());check(athletes && athletes->rowCount()>=2,"form lab exposes pack athletes as stable rows");
    const auto roles=athletes->roleNames();const int idRole=roles.key("athleteId"),formRole=roles.key("initialForm"),perkRole=roles.key("formPerk");
    const auto firstId=athletes->index(0,0).data(idRole).toString(),secondId=athletes->index(1,0).data(idRole).toString();
    const int originalForm=athletes->index(0,0).data(formRole).toInt(),originalPerk=athletes->index(0,0).data(perkRole).toInt();
    QSignalSpy resets(athletes,&QAbstractItemModel::modelReset);
    controller->selectAthletes(false);controller->start();check(!controller->busy() && controller->message().contains(QString::fromUtf8("co najmniej")),"empty manual selection cannot silently simulate the full pack");
    controller->setAthlete(firstId,"selected",true);controller->setAthlete(secondId,"selected",true);
    controller->setAthlete(firstId,"form",88);controller->setAthlete(firstId,"perk",-4);controller->resetAthlete(firstId);
    check(athletes->index(0,0).data(formRole).toInt()==originalForm && athletes->index(0,0).data(perkRole).toInt()==originalPerk,"one click restores an athlete's original pack parameters");
    clickControl(view,"formLabInitialForm-"+firstId);QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"88");QTest::keyClick(view,Qt::Key_Return);
    auto *perkChoice=item(view->rootObject(),"formLabPerk-"+firstId);check(perkChoice && QMetaObject::invokeMethod(perkChoice,"activated",Q_ARG(int,0)),"individual form perk can be selected in the row");
    controller->setAthlete(secondId,"form",33);controller->setAthlete(secondId,"perk",4);
    check(resets.isEmpty(),"selection and attribute edits preserve rows, focus and scroll position");
    controller->filterAthletes("__not_an_athlete__");check(athletes->rowCount()==0 && controller->selectedAthletes()==2,"filtering does not discard hidden selected athletes");controller->filterAthletes("");
    clickControl(view,"formLabRun");for(int i=0;i<500 && controller->busy();++i)QTest::qWait(20);
    const auto selected=controller->result().value("roster").toList();
    check(selected.size()==2 && selected[0].toMap().value("form").toInt()==88 && selected[0].toMap().value("perk").toInt()==-4 && selected[1].toMap().value("form").toInt()==33 && selected[1].toMap().value("perk").toInt()==4,"experiment uses only selected athletes and their temporary parameters");
    FormLabController unmodified;auto *originals=qobject_cast<QAbstractItemModel *>(unmodified.athleteRows());
    check(originals->index(0,0).data(formRole).toInt()==originalForm && originals->index(0,0).data(perkRole).toInt()==originalPerk,"lab edits never change the authoring pack");
    view->grabFramebuffer();presentation(&lab)->grab().save(output+"/form-lab-roster.png");
    controller->useForNewGames();SimulationSave newSave;check(newSave.formEngine().config.formDeviation==4,"lab profile becomes the default for newly created saves");
    lab.reject();

    SimulationSave save;save.setEngineVersion(2);save.setName("Automatic form integration");save.setAutosaveEnabled(false);
    for(int i=0;i<8;++i){auto p=AthleteProfile::create();p.name="Form";p.surname=QString::number(i);p.form=50+i;p.perks[3]=i%9-4;auto *athlete=new Jumper;QString error;check(athlete->setSimulationProfile(p,error),"form season athlete");save.getJumpersReference().append(athlete);}
    auto venue=HillProfile::create();venue.name="Form hill";auto *hill=new Hill;QString error;check(hill->setSimulationProfile(venue,error),"form season hill");save.setHills({hill});
    CompetitionRules rules;rules.setName("Form rules");rules.setRounds({RoundInfo(8,true)});rules.setHasDsq(false);save.setCompetitionRules({rules});
    auto *calendar=new SeasonCalendar("Form calendar");auto *first=new CompetitionInfo(hill);first->setRules(rules);first->setScheduledDate(QDate(2026,11,1));auto *second=new CompetitionInfo(hill);second->setRules(rules);second->setScheduledDate(QDate(2026,11,15));calendar->setCompetitions({first,second});
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);check(season.setDatesEnabled(true),"dated form season");save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.updateNextCompetitionIndex();save.fixJumpersFormInstabilities();
    save.formEngine().seed=123;const auto originalConfig=save.formEngine().config.toJson();
    controller->setParameter("formDeviation",8);controller->useForNewGames();
    check(!save.formEngine().setEnabled(true,false,error),"cannot enable automatic form without dates");
    SimulationSaveManagerWindow window(&save,&main);window.fillNextCompetitionInformations();window.show();QTest::qWait(150);
    auto *bridge=window.findChild<QuickBridge *>();check(bridge,"automatic form season bridge exists");
    clickControl(scene(&window),"seasonAutomaticForm");check(save.formEngine().enabled,"visible season switch enables automatic form");
    scene(&window)->grabFramebuffer();presentation(&window)->grab().save(output+"/season-automation.png");
    check(save.formEngine().update(QDate(2026,11,1),save.getJumpersReference(),true,error),"initialize all season athletes");
    for(int i=0;i<8;++i)check(save.getJumpersReference()[i]->getSimulationProfile()->form==50+i,"initialization preserves each original form");
    first->setPlayed(true);save.updateNextCompetitionIndex();window.fillNextCompetitionInformations();bridge->refresh();QTest::qWait(60);
    const auto before=save.formEngine().toJson();bool sawConfig=false;int attempts=0;
    QTimer responder;QObject::connect(&responder,&QTimer::timeout,&window,[&]{
        check(++attempts<250,"form preparation cannot hang");
        for(auto *widget:QApplication::allWidgets())if(auto *config=qobject_cast<CompetitionConfigWindow *>(widget);config && config->isVisible()){
            sawConfig=true;for(const auto &state:save.formEngine().states)check(state.date==QDate(2026,11,15),"form advances before preparation opens");config->reject();
        }
    });responder.start(20);clickControl(scene(&window),"playUpcomingEvent");QTest::qWait(100);responder.stop();
    check(sawConfig && save.formEngine().toJson()==before,"cancelled preparation rolls back latent state and RNG");
    for(int i=0;i<8;++i)check(save.getJumpersReference()[i]->getSimulationProfile()->form==50+i,"cancelled preparation rolls back visible forms");
    AutoForm::Checkpoint prediction(save.formEngine(),save.getJumpersReference());
    check(save.formEngine().update(QDate(2026,11,15),save.getJumpersReference(),true,error),"predict cancellation time progression");const auto expectedCancellation=save.formEngine().toJson();prediction.restore();
    QObject::disconnect(&responder,nullptr,&window,nullptr);QObject::connect(&responder,&QTimer::timeout,&window,[]{for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible())for(auto *button:box->buttons())if(box->buttonRole(button)==QMessageBox::DestructiveRole){button->click();break;}});
    responder.start(20);check(window.cancelSeasonEvents({second->getIDStr()}),"confirmed event cancellation");responder.stop();
    check(second->getCancelled() && save.formEngine().toJson()==expectedCancellation,"cancelled event advances forms to its date exactly once");
    const auto after=save.formEngine().toJson();check(save.formEngine().update(QDate(2026,11,15),save.getJumpersReference(),true,error) && save.formEngine().toJson()==after,"same-day events do not double count time");
    check(save.formEngine().setEnabled(false,true,error),"disable auto form");const auto paused=save.formEngine().toJson();
    check(save.formEngine().update(QDate(2027,1,1),save.getJumpersReference(),true,error) && save.formEngine().toJson()==paused,"disabled generator does not advance");
    check(save.formEngine().setEnabled(true,true,error) && save.formEngine().update(QDate(2027,1,1),save.getJumpersReference(),true,error),"resume at current date");
    for(auto it=save.formEngine().states.cbegin();it!=save.formEngine().states.cend();++it){const auto old=after["states"].toObject()[it.key()].toObject();check(it.value().form==old["form"].toDouble() && it.value().base==old["base"].toDouble() && it.value().remainingDays==old["remainingDays"].toDouble(),"resume preserves F, base and waiting time without catch-up");}
    DatabaseItemsListView *athleteList=nullptr;for(auto *list:window.findChildren<DatabaseItemsListView *>())if(list->getType()==DatabaseItemsListView::SeasonJumpersItems)athleteList=list;
    bridge->edit(bridge->root()->values().value("id").toInt(),1);QTest::qWait(60);
    check(athleteList,"season athlete list exists");athleteList->listViewDoubleClicked(athleteList->getListModel()->index(0,0));
    auto *draft=window.findChild<SimulationProfileDraft *>("seasonAthleteDraft");const auto form=save.getJumpersReference()[0]->getSimulationProfile()->form;
    check(draft && draft->profile().value("formLocked").toBool() && draft->profile().value("formDiagnostic").toMap().contains("base"),"season editor has a locked form and diagnostic state");
    check(draft->profile().value("formDiagnostic").toMap().value("rows").toList().size()>=25,"season diagnostics expose detailed state, dynamics and transition probabilities");
    QTest::qWait(60);auto *formInput=item(scene(&window)->rootObject(),"athleteForm");check(formInput && formInput->property("readOnly").toBool(),"automatic form is read-only in the visible editor");
    clickControl(scene(&window),"formDiagnosticsToggle");scene(&window)->grabFramebuffer();presentation(&window)->grab().save(output+"/form-diagnostics.png");
    QQmlComponent statusComponent(scene(&window)->engine(),QUrl("qrc:/ui/FormStatus.qml"));std::unique_ptr<QObject> status(statusComponent.create(scene(&window)->rootContext()));check(bool(status),"form status component loads");
    const auto statusHeight=status->property("implicitHeight").toDouble();
    for(const auto &range:QVector<QPair<int,int>>{{1,0},{19,0},{20,1},{34,1},{35,2},{44,2},{45,3},{55,3},{56,4},{69,4},{70,5},{84,5},{85,6},{94,6},{95,7},{100,7}}){status->setProperty("value",range.first);check(qAbs(status->property("implicitHeight").toDouble()-statusHeight)<.01,"form status height remains constant");check(status->property("band").toInt()==range.second,"all form description boundaries match their specified intervals");}
    draft->setValue("form",form==100?1:100);check(save.getJumpersReference()[0]->getSimulationProfile()->form==form && !draft->message().isEmpty(),"backend rejects manual form changes");
    draft->setValue("overall",85);check(save.getJumpersReference()[0]->getSimulationProfile()->overall==85,"automatic form never blocks manual OVR changes");
    const auto serialized=SimulationSave::getJsonObject(save);std::unique_ptr<SimulationSave> loaded(SimulationSave::getFromJson(serialized,nullptr));
    check(loaded && loaded->formEngine().toJson()==save.formEngine().toJson(),"full season JSON preserves automatic form state");
    auto older=serialized;older.remove("automatic-form");std::unique_ptr<SimulationSave> oldSave(SimulationSave::getFromJson(older,nullptr));
    check(oldSave && !oldSave->formEngine().enabled && oldSave->formEngine().states.isEmpty(),"existing saves without automatic-form metadata load with the feature off");
    QTemporaryDir saveDirectory;check(save.saveToFile(saveDirectory.path(),"automatic-form"),"automatic form saves atomically to a real file");
    std::unique_ptr<SimulationSave> fromFile(SimulationSave::loadFromFile(saveDirectory.filePath("automatic-form.json")));
    check(fromFile && fromFile->formEngine().toJson()==save.formEngine().toJson(),"real save file restores the full automatic form state");
    check(save.formEngine().update(QDate(2027,2,1),save.getJumpersReference(),true,error) && loaded->formEngine().update(QDate(2027,2,1),loaded->getJumpersReference(),true,error) && loaded->formEngine().toJson()==save.formEngine().toJson(),"loaded season continues the exact same trajectories");
    controller=nullptr;check(save.formEngine().config.toJson()==originalConfig,"existing save keeps its configuration snapshot");
    window.hide();
    AppSettingsWindow settings(&main);settings.show();QTest::qWait(100);
    for(const auto *name:{"comboBox_language","checkBox_liveCompetition","spinBox_dsqProbability"})check(settings.findChild<QWidget *>(name)->isHidden(),"duplicated preference is hidden in general settings");
    clickControl(scene(&settings),"action-openFormEngineLab");QTest::qWait(100);
    auto *opened=settings.findChild<FormLabWindow *>();check(opened && opened->isVisible() && scene(opened)->status()==QQuickWidget::Ready,"form lab opens from application settings");opened->reject();settings.reject();
    qInfo()<<"PASS: form lab batching, A/B, default profile, date integration, rollback, manual lock and save continuation";
}
