#pragma once
static void simulationV2CompetitionRegressions() {
    auto low=AthleteProfile::create();low.name="Low";low.overall=40;low.form=50;
    auto high=AthleteProfile::create();high.name="High";high.overall=95;high.form=90;
    auto venue=HillProfile::create();venue.name="New hill";
    Jumper a,b;Hill hill;QString error;
    check(a.setSimulationProfile(low,error) && b.setSimulationProfile(high,error) && hill.setSimulationProfile(venue,error),"competition v2 gate fixtures");
    a.getJumperSkillsPointer()->setTakeoffTechnique(9999);a.getJumperSkillsPointer()->setFlightTechnique(9999);
    b.getJumperSkillsPointer()->setTakeoffTechnique(0);b.getJumperSkillsPointer()->setFlightTechnique(0);
    CompetitionConfigWindow window(CompetitionConfigWindow::SingleCompetition);
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
