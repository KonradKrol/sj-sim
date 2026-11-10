#pragma once

static void seasonPresentationRegressions(MainWindow &main) {
    QString error;SimulationSave save;save.setEngineVersion(2);save.setName("Season presentation regression");save.setAutosaveEnabled(false);
    for(int i=0;i<4;++i){auto profile=AthleteProfile::create();profile.name="Season";profile.surname=QString::number(i);auto *jumper=new Jumper;check(jumper->setSimulationProfile(profile,error),"season presentation athlete");save.getJumpersReference().append(jumper);}
    auto hill=HillProfile::create();hill.name="Season presentation hill";auto *venue=new Hill;check(venue->setSimulationProfile(hill,error),"season presentation hill");save.setHills({venue});
    CompetitionRules rules;rules.setName("Presentation rules");rules.setRounds({RoundInfo(4,true)});rules.setHasDsq(false);save.setCompetitionRules({rules});
    auto *calendar=new SeasonCalendar("Presentation calendar");
    for(int i=0;i<8;++i){auto *event=new CompetitionInfo(venue);event->setRules(rules);calendar->getCompetitionsReference().append(event);}
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    main.show();SimulationSaveManagerWindow window(&save,&main);window.fillNextCompetitionInformations();window.show();QTest::qWait(180);
    auto *view=scene(&window);auto *bridge=window.findChild<QuickBridge *>();check(bridge && view->status()==QQuickWidget::Ready,"actual season overview loads");
    auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const auto oldScale=appearance->uiScale(),oldText=appearance->textScale();
    for(const auto &size:{QSize(1440,900),QSize(1024,640),QSize(800,600)})for(double scale:{.5,.7,1.}) {
        main.resize(size);appearance->setUiScale(scale);appearance->setTextScale(scale==1.?1.4:1.);QTest::qWait(100);view->grabFramebuffer();
        auto *overview=item(view->rootObject(),"seasonOverview");auto *card=item(view->rootObject(),"seasonUpcomingEvent");auto *bar=item(view->rootObject(),"seasonBar");
        check(overview && card && card->isVisible() && bar,"overview and upcoming event remain visible after live scale changes");
        check(qAbs(card->height()-card->implicitHeight())<2 && card->height()>20,"event card keeps natural height instead of filling empty space");
        check(bar->height()>0 && bar->height()<main.height()*.45,"season header stays bounded after scale changes");
    }
    appearance->setUiScale(.7);appearance->setTextScale(1);main.resize(1200,800);QTest::qWait(100);view->grabFramebuffer();
    auto *overview=item(view->rootObject(),"seasonOverview");const auto fields=overview->property("fields").value<QJSValue>().toVariant().toMap();
    check(fields.value("events").toList().size()==7,"overview shows seven upcoming competitions");
    auto *next=save.getNextCompetition();bool opened=false;int attempts=0;
    QTimer responder;QObject::connect(&responder,&QTimer::timeout,&window,[&]{
        check(++attempts<250,"season action finishes without hanging");
        for(auto *widget:QApplication::allWidgets())if(auto *config=qobject_cast<CompetitionConfigWindow *>(widget);config && config->isVisible()){
            check(config->getSeasonCompetition()==next,"Play opens preparation for the displayed event");opened=true;config->reject();
        }
    });responder.start(20);clickControl(view,"playUpcomingEvent");QTest::qWait(100);responder.stop();
    check(opened && save.getNextCompetition()==next && !next->getPlayed(),"leaving preparation preserves the pending event");
    bool accept=false,questionSeen=false;attempts=0;QObject::disconnect(&responder,nullptr,&window,nullptr);
    QObject::connect(&responder,&QTimer::timeout,&window,[&]{
        check(++attempts<250,"cancellation confirmation finishes without hanging");
        for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){
            questionSeen=true;for(auto *button:box->buttons())if(box->buttonRole(button)==(accept?QMessageBox::DestructiveRole:QMessageBox::RejectRole)){button->click();break;}
        }
    });responder.start(20);clickControl(view,"cancelUpcomingEvent");QTest::qWait(100);
    check(questionSeen && !next->getCancelled() && save.getNextCompetition()==next,"rejecting cancellation preserves the event");
    accept=true;questionSeen=false;attempts=0;clickControl(view,"cancelUpcomingEvent");QTest::qWait(150);responder.stop();
    check(questionSeen && next->getCancelled() && save.getNextCompetition()!=next,"confirming cancellation advances the overview");
    view->grabFramebuffer();presentation(&window)->grab().save(output+"/season-presentation-final.png");
    window.hide();appearance->setUiScale(oldScale);appearance->setTextScale(oldText);
    qInfo()<<"PASS: real season overview scaling, seven events, Play and confirmed cancellation";
}
