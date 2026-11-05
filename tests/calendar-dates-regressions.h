#pragma once
#include "../source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.h"

static void calendarDateRegressions()
{
    {
        DataPack pack;pack.kind=DataPack::Rules;pack.name="Defaults";
        CompetitionRules firstRules("First"),qualifying("Qualifying"),teamRules("Team");firstRules.setRounds({RoundInfo(50,true)});qualifying.setRounds({RoundInfo(60,true)});teamRules.setCompetitionType(CompetitionRules::Team);teamRules.setJumpersInTeamCount(2);teamRules.setRounds({RoundInfo(10,true)});
        pack.rules={firstRules,qualifying,teamRules};pack.defaultRules["qualifications"]=qualifying.getIDStr();
        check(pack.defaultRule("competition")==0 && pack.defaultRule("qualifications")==1 && pack.defaultRule("teamTrial")==2,"pack defaults choose compatible fallback and explicit roles");
        DataPack loaded;QString error;check(DataPack::fromJson(pack.toJson(),loaded,error) && loaded.defaultRule("qualifications")==1,"rule defaults survive pack export/import");
        SimulationSave fixture;fixture.setHills({new Hill("Defaults hill","POL",120,134)});fixture.setCompetitionRules(pack.rules);
        auto *cal=new SeasonCalendar("Qualification planning");auto *event=new CompetitionInfo(fixture.getHillsReference().first());event->setRules(firstRules);event->setScheduledDate(QDate(2026,12,10));cal->setCompetitions({event});
        Season season;season.setSeasonNumber(2026);season.setCalendars({cal});season.setActualCalendar(cal);fixture.setSeasons({season});fixture.setActualSeason(&fixture.getSeasonsReference().first());
        CalendarEditorTableModel model(cal,&fixture.getHillsReference(),&fixture.getCompetitionRulesReference());CalendarEditorWidget editor(&model,&cal->getClassificationsReference());editor.setSave(&fixture);QuickCalendar planner(&editor,&editor);
        planner.select(event->getIDStr());planner.setDefaultRule("qualifications",2);
        planner.update({{"type",CompetitionInfo::Qualifications}});
        check(event->getRules().getIDStr()==qualifying.getIDStr(),"conversion to qualifications applies the calendar qualification default");
        planner.undo();check(event->getSerieType()==CompetitionInfo::Competition,"undo restores event type and rules together");
        planner.redo();planner.update({{"rules",1}});planner.update({{"type",CompetitionInfo::Competition}});planner.update({{"type",CompetitionInfo::Qualifications}});
        check(event->getRules().getIDStr()==qualifying.getIDStr(),"every conversion to qualifications reapplies the calendar default");
        planner.update({{"type",CompetitionInfo::Competition}});
        planner.setDefaultRule("training",2);planner.setDefaultRule("trial",2);
        planner.update({{"trainings",1},{"trial",true}});
        check(event->getTrainings().first()->getRules().getIDStr()==qualifying.getIDStr() && event->getTrialRound()->getRules().getIDStr()==qualifying.getIDStr(),"new trainings and trial rounds use their own calendar defaults");
        planner.undo();
        planner.setDatesEnabled(true);planner.setDefaultRule("qualifications",2);planner.select(event->getIDStr());planner.addQualifications(1);
        check(cal->getCompetitions().size()==2 && event->getAdvancementCompetition() && event->getAdvancementCompetition()->getScheduledDate()==QDate(2026,12,9) && event->getAdvancementCompetition()->getRules().getName()==qualifying.getName(),"bulk qualifications use calendar default, previous day and actual advancement link");
        planner.addQualifications(1);check(cal->getCompetitions().size()==2,"repeated qualification action never duplicates existing links");planner.undo();check(cal->getCompetitions().size()==1 && !event->getAdvancementCompetition(),"qualification planning can be undone as one operation");
        planner.addQualifications(0);check(event->getAdvancementCompetition() && event->getAdvancementCompetition()->getScheduledDate()==event->getScheduledDate(),"same day qualifications are supported");
        check(SeasonCalendar::getJsonObject(*cal)["default-rules"].toObject()["qualifications"].toString()==qualifying.getIDStr(),"calendar default overrides are persisted");
        Jumper a("A","One","POL"),b("B","Two","POL");CompetitionInfo main(fixture.getHillsReference().first()),trial(fixture.getHillsReference().first());main.setRules(teamRules);trial.setRules(firstRules);trial.setSerieType(CompetitionInfo::TrialRound);main.setTrialRound(&trial);Team squad("POL");squad.setJumpers({&a,&b});main.setTeams({squad});
        for(auto *jumper:QVector<Jumper *>{&a,&b}){CompetitionSingleResult result(&trial,jumper);JumpData jump(jumper,fixture.getHillsReference().first());jump.setPoints(100);result.setJumps({jump});result.setPointsSum(100);trial.getResultsReference().getResultsReference().append(result);}
        ResultsHistoryWidget history(&trial,false,nullptr);history.modeChoice()->setCurrentIndex(0);history.refresh();check(history.snapshot.getResults().size()==1 && history.snapshot.getResults().first().getType()==CompetitionSingleResult::TeamResult && history.snapshot.getResults().first().getPointsSum()==200,"individual team trial projects actual squad results without changing scoring");main.setTrialRound(nullptr);
    }

    SimulationSave save;
    save.setHills({new Hill("Date hill","POL",120,134)});save.setCompetitionRules({CompetitionRules()});
    auto *calendar=new SeasonCalendar("Dates");
    for(int i=0;i<6;++i)calendar->getCompetitionsReference().append(new CompetitionInfo(save.getHillsReference().first()));
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());
    CalendarEditorTableModel model(calendar,&save.getHillsReference(),&save.getCompetitionRulesReference());
    CalendarEditorWidget editor(&model,&calendar->getClassificationsReference());editor.setSave(&save);
    QuickCalendar backend(&editor,&editor);
    auto *first=calendar->getCompetitionsReference().first();
    check(!backend.datesEnabled() && backend.canEnableDates() && !first->getScheduledDate().isValid(),"legacy seasons start without dates");
    backend.setDatesEnabled(true);backend.selectAll();
    backend.planDates("2026-12-30","weekends",7);
    check(backend.datePlan().size()==6 && !first->getScheduledDate().isValid(),"date planner previews without changing events");
    backend.applyDatePlan();
    check(first->getScheduledDate()==QDate(2027,1,2) && calendar->getCompetitionsReference()[1]->getScheduledDate()==QDate(2027,1,3) && calendar->getCompetitionsReference()[2]->getScheduledDate()==QDate(2027,1,9),"weekend pairs cross year boundaries correctly");
    backend.undo();check(!first->getScheduledDate().isValid(),"undo restores all planned dates atomically");backend.redo();check(first->getScheduledDate()==QDate(2027,1,2),"redo restores the date plan");
    backend.select(first->getIDStr());backend.update({{"trainings",1},{"trial",true}});
    check(first->getTrainings().first()->getScheduledDate()==first->getScheduledDate() && first->getTrialRound()->getScheduledDate()==first->getScheduledDate(),"new sessions inherit the event date");
    backend.selectAll();backend.planDates("2030-01-01","interval",7);backend.applyDatePlan();backend.select(first->getIDStr());
    backend.planDates("2028-02-29","same",1,false,1);backend.applyDatePlan();
    check(first->getScheduledDate()==QDate(2028,2,29) && first->getTrainings().first()->getScheduledDate()==QDate(2028,2,28),"leap-day plan allows training on an earlier day");
    backend.shiftDates(1);check(first->getScheduledDate()==QDate(2028,3,1) && first->getTrainings().first()->getScheduledDate()==QDate(2028,2,29),"shifting preserves session offsets across leap days");
    backend.clearDates();check(!first->getScheduledDate().isValid() && !first->getTrainings().first()->getScheduledDate().isValid(),"clearing dates includes attached sessions");backend.undo();
    backend.planDates("2029-01-01","preserve",1);backend.applyDatePlan();check(first->getScheduledDate()==QDate(2029,1,1) && first->getTrainings().first()->getScheduledDate()==QDate(2028,12,31),"retargeting preserves offsets between all attached sessions");
    backend.selectAll();backend.planDates("2026-03-28","interval",1);backend.applyDatePlan();
    auto mainEvents=backend.events();check(mainEvents[1].toMap().value("date").toString()=="2026-03-29" && mainEvents[2].toMap().value("date").toString()=="2026-03-30","daily scheduling does not skip dates at DST boundaries");
    backend.planDates("2026-11-01","weekly",1);backend.update({{"importance",4}});check(backend.datePlan().isEmpty(),"other calendar edits invalidate stale date previews");
    backend.planDates("2026-02-30","interval",1);check(backend.datePlan().isEmpty() && first->getScheduledDate()==QDate(2026,3,28),"invalid dates do not mutate a calendar");
    const auto month=backend.monthData("2028-02-01");check(month.value("days").toList().size()==42 && month.value("weekdays").toStringList().size()==7,"month grids have complete Monday-first weeks");
    check(backend.normalizeDate("2028-02-29")=="2028-02-29" && backend.normalizeDate("2027-02-29").isEmpty(),"date input validates leap years");
    check(backend.normalizeDate("29.02.2028")=="2028-02-29" && backend.normalizeDate("29.02.2027").isEmpty(),"Polish numeric date input validates leap years and keeps ISO storage");
    const QLocale previousLocale;
    for(const auto &language:{"pl_PL","en_US","de_DE"}){QLocale::setDefault(QLocale(language));check(backend.dateText("2026-12-31")=="31.12.2026" && backend.dateText("2027-02-01")=="01.02.2027","event dates always display DD.MM.YYYY independently of locale");}
    QLocale::setDefault(previousLocale);
    backend.select(first->getIDStr());backend.assignDate("2026-04-20");check(first->getScheduledDate()==QDate(2026,3,28) && !backend.message().isEmpty(),"chronologically conflicting date is rejected without changing the calendar");
    const auto beforeMove=calendar->getCompetitionsReference();backend.move(1);check(calendar->getCompetitionsReference()==beforeMove,"moving a dated competition after a later event is blocked");
    backend.selectAll();const auto beforeCopy=backend.events();backend.duplicate();check(backend.events()==beforeCopy,"copying dated events into a conflicting position is blocked");
    auto *missingEvent=calendar->getCompetitionsReference().last();const auto datedFirst=first->getScheduledDate();backend.select(missingEvent->getIDStr());backend.clearDates();backend.selectAll();backend.planDates("2030-02-01","weekly",7,false,0,true);check(backend.datePlan().size()==1,"fill-missing mode previews only undated events");backend.applyDatePlan();check(first->getScheduledDate()==datedFirst && missingEvent->getScheduledDate()==QDate(2030,2,1),"fill-missing mode leaves established dates intact");backend.undo();backend.undo();
    save.getCompetitionRulesReference().first().setRounds({RoundInfo(50,true),RoundInfo(30,true)});
    for(bool saturday:{false,true})for(bool shared:{false,true}){
        const auto before=calendar->getCompetitionsReference();const int count=backend.events().size();backend.previewWeekend("2026-12-31",0,0,saturday,shared);check(backend.weekendPreview().size()==3 && calendar->getCompetitionsReference()==before,"weekend preview describes all three days without mutating calendar");check(backend.applyWeekend(),"weekend preview applies atomically");QVector<CompetitionInfo *> contests;for(auto *event:calendar->getCompetitionsReference())if(!before.contains(event) && event->getSerieType()==CompetitionInfo::Competition)contests.append(event);check(contests.size()==2 && backend.events().size()==count+(shared?3:4),"weekend creates exactly two main events and the required qualifiers");auto *sat=contests[0],*sun=contests[1],*source=sat->getAdvancementCompetition();check(source && source->getScheduledDate()==QDate(2027,1,saturday?2:1) && source->getTrainings().size()==(saturday?0:2) && source->getRules().getRounds().size()==backend.defaultRule("qualifications").getRounds().size() && source->getRules().getName()==backend.defaultRule("qualifications").getName(),"first qualifying day, default rules and Friday trainings follow chosen variant");check(sat->getScheduledDate()==QDate(2027,1,2) && bool(sat->getTrialRound())==!saturday && sun->getScheduledDate()==QDate(2027,1,3) && bool(sun->getTrialRound())==shared,"trials appear exactly when a day has no qualifications");check((sun->getAdvancementCompetition()==source)==shared && calendar->getCompetitionsReference().indexOf(source)<calendar->getCompetitionsReference().indexOf(sat),"qualification links are valid and shared only on request");backend.undo();check(calendar->getCompetitionsReference()==before,"one undo restores entire weekend creation including all auxiliary events");backend.redo();check(backend.events().size()==count+(shared?3:4),"redo restores complete weekend without duplicates");backend.undo();
    }
    first->setPlayed(true);backend.select(first->getIDStr());backend.planDates("2030-01-01","weekly",7,true);check(backend.datePlan().size()==5,"planning all future events skips played selections");backend.applyDatePlan();check(first->getScheduledDate()==QDate(2026,3,28) && backend.events()[1].toMap().value("date").toString()=="2030-01-01","future plans leave historical dates unchanged");
    backend.setDatesEnabled(false);check(!backend.datesEnabled() && !backend.canEnableDates() && first->getScheduledDate().isValid(),"dates can be disabled after season start without deleting the plan");backend.setDatesEnabled(true);check(!backend.datesEnabled(),"enabling dates after season start is rejected by the backend");
    IdentifiableObjectsStorage storage;storage.add(save.getHillsReference());
    auto restored=Season::getFromJson(Season::getJsonObject(*save.getActualSeason()),&storage);
    check(!restored.getDatesEnabled() && restored.hasStarted() && !restored.setDatesEnabled(true) && restored.getActualCalendar()->getCompetitionsReference().first()->getScheduledDate()==QDate(2026,3,28),"season reload preserves dates, mode and the started latch");
    SeasonCalendarPreset preset(calendar);auto *presetFirst=preset.getCalendarReference().getCompetitionsReference().first();check(!presetFirst->getPlayed() && presetFirst->getScheduledDate()==first->getScheduledDate(),"calendar presets retain dates without played state");
    IdentifiableObjectsStorage presetStorage;auto restoredPreset=SeasonCalendarPreset::getFromJson(SeasonCalendarPreset::getJsonObject(preset),&presetStorage);check(restoredPreset.getCalendarReference().getCompetitionsReference().first()->getScheduledDate()==first->getScheduledDate(),"preset JSON round trip retains dates");
    for(auto *cal:restored.getCalendarsReference()){qDeleteAll(cal->getCompetitionsReference());delete cal;}
    qDeleteAll(preset.getCalendarReference().getCompetitionsReference());qDeleteAll(restoredPreset.getCalendarReference().getCompetitionsReference());
    qInfo()<<"PASS: optional calendar dates, weekend planning, leap years, DST, offsets, undo, history locks, persistence and presets";
}

static void calendarDateUiRegressions(MainWindow &main)
{
    QTimer unexpectedDialog;
    QObject::connect(&unexpectedDialog,&QTimer::timeout,[]{
        for(auto *widget:QApplication::allWidgets())if(auto *message=qobject_cast<QMessageBox *>(widget);message && (message->isVisible() || message->property("sjQuickPage").toBool()))
            check(false,"unexpected date-test dialog: "+message->windowTitle()+": "+message->text());
    });
    unexpectedDialog.start(100);
    SimulationSave save;save.setName("Date calendar test");
    {NewSeasonConfiguratorWindow setup;save.setJumpers(setup.getJumpersReference());save.setHills(setup.getHillsReference());save.setCompetitionRules(setup.getCompetitionsRulesReference());}
    auto *calendar=new SeasonCalendar("Winter tour");
    for(int i=0;i<10;++i){auto *event=new CompetitionInfo(save.getHillsReference()[qMin(i/2,save.getHillsReference().size()-1)]);event->setRules(save.getCompetitionRulesReference().first());calendar->getCompetitionsReference().append(event);}
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    SimulationSaveManagerWindow manager(&save,&main);manager.fillNextCompetitionInformations();render(&manager,"date-manager");
    auto *bridge=manager.findChild<QuickBridge *>();auto *backend=bridge->findChild<QuickCalendar *>();check(backend,"calendar date backend exists");
    auto *settings=manager.findChild<QCheckBox *>("checkBox_calendarDates");check(settings && !settings->isChecked(),"date setting exists in simulation settings");settings->setChecked(true);QTest::qWait(100);backend->sync();check(backend->datesEnabled(),"simulation setting enables calendar dates");
    bridge->edit(bridge->root()->values().value("id").toInt(),4);QTest::qWait(150);
    auto *workspace=item(scene(&manager)->rootObject(),"calendarWorkspace");check(workspace,"calendar workspace exists");
    main.resize(1440,1000);QTest::qWait(120);
    calendarWorkspaceUxRegressions(backend,scene(&manager),calendar->getCompetitionsReference()[0],calendar->getCompetitionsReference()[1]);
    backend->selectAll();
    clickControl(scene(&manager),"calendarPanel-2");QTest::qWait(60);
    for(int i=0;i<8;++i){clickControl(scene(&manager),"calendarDefaultRule-qualifications");QTest::keyClick(scene(&manager),i%2?Qt::Key_Home:Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(30);check(backend->ruleDefaults().size()==6,"changing qualification defaults recreates controls safely after closing the popup");}
    scene(&manager)->grabFramebuffer().save("/tmp/sj-calendar-defaults.png");
    clickControl(scene(&manager),"calendarPanel-1");QTest::qWait(100);
    check(item(scene(&manager)->rootObject(),"calendarDatePlanner"),"planner opens directly from calendar toolbar");
    clickControl(scene(&manager),"date-plan-existing");QTest::qWait(80);
    auto *input=item(scene(&manager)->rootObject(),"date-input-plan-start");check(input,"planner date input exists");
    const auto originalText=input->property("text").toString();
    auto editDate=[&](const QString &text){clickControl(scene(&manager),"date-input-plan-start");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),text);QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(80);};
    editDate("2026-02-30");auto *dateField=item(scene(&manager)->rootObject(),"date-field-plan-start");check(dateField && !dateField->property("error").toString().isEmpty(),"invalid date displays a validation error");
    check(!item(scene(&manager)->rootObject(),"date-plan-preview")->isEnabled(),"invalid start dates disable the preview");
    editDate(originalText);check(dateField->property("error").toString().isEmpty() && item(scene(&manager)->rootObject(),"date-plan-preview")->isEnabled(),"restoring the original valid date clears the error and enables planning");
    clickControl(scene(&manager),"date-input-plan-start");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),"2026-11-27");QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(80);
    clickControl(scene(&manager),"date-plan-preview");QTest::qWait(100);check(backend->datePlan().size()==10 && backend->datePlan().first().toMap().value("date").toString()=="2026-11-28","visible planner previews weekend dates");
    clickControl(scene(&manager),"date-plan-apply");QTest::qWait(120);check(calendar->getCompetitionsReference().first()->getScheduledDate()==QDate(2026,11,28),"visible apply button commits the date plan");
    clickControl(scene(&manager),"date-open-weekend");clickControl(scene(&manager),"date-input-weekend-start");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),"2027-01-01");QTest::keyClick(scene(&manager),Qt::Key_Tab);clickControl(scene(&manager),"weekend-sunday");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);check(backend->weekendPreview().size()==3 && backend->weekendPreview().first().toMap().value("events").toString().contains("2") && !backend->weekendPreview().last().toMap().value("qualification").toString().isEmpty(),"weekend preview automatically updates for Friday trainings and shared Sunday qualifications");const int previewCount=backend->events().size();check(!item(scene(&manager)->rootObject(),"weekend-preview"),"weekend builder needs no preview button");clickControl(scene(&manager),"weekend-first-qualification");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);check(backend->weekendPreview().size()==3 && !backend->weekendPreview().first().toMap().value("events").toString().contains("2") && !backend->weekendPreview().last().toMap().value("qualification").toString().isEmpty() && item(scene(&manager)->rootObject(),"weekend-sunday")->property("currentText").toString().contains("soboty"),"automatic Saturday preview keeps the shared qualification choice and updates its label");clickControl(scene(&manager),"weekend-first-qualification");QTest::keyClick(scene(&manager),Qt::Key_Home);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);check(backend->events().size()==previewCount && backend->weekendPreview().first().toMap().value("events").toString().contains("2"),"changing weekend options previews without creating events");clickControl(scene(&manager),"date-input-weekend-start");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),"31.02.2026");QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(80);check(backend->weekendPreview().isEmpty() && !item(scene(&manager)->rootObject(),"weekend-apply")->isEnabled(),"invalid weekend dates remove stale previews and disable adding");clickControl(scene(&manager),"date-input-weekend-start");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),"01.01.2027");QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(100);check(backend->weekendPreview().size()==3,"restoring a date refreshes the weekend preview without another action");render(&manager,"date-weekend-builder",true);main.resize(1440,1000);QTest::qWait(100);const int beforeWeekend=backend->events().size();clickControl(scene(&manager),"weekend-apply");QTest::qWait(100);check(backend->events().size()==beforeWeekend+3 && backend->selection().value("count").toInt()==2,"visible weekend action creates and selects two new competitions with shared qualifying source");check(backend->weekendPreview().size()==3 && backend->normalizeDate(backend->weekendPreview().first().toMap().value("date").toString())=="2027-01-08","adding a weekend automatically prepares next week's preview");backend->undo();check(backend->events().size()==beforeWeekend,"visible builder adds a complete undoable weekend");clickControl(scene(&manager),"date-plan-existing");
    workspace->setProperty("panel","events");workspace->setProperty("editing",false);QTest::qWait(80);clickControl(scene(&manager),"calendarMonthToggle");QTest::qWait(150);
    auto *overview=item(scene(&manager)->rootObject(),"calendarDateOverview");check(overview,"month overview opens");overview->setProperty("month","2026-12-01");
    auto *appearance=scene(&manager)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();
    for(bool dark:{false,true}){appearance->setDark(dark);for(const auto size:{QSize(1440,1000),QSize(1024,850),QSize(520,850),QSize(360,800)}){main.resize(size);QTest::qWait(180);presentation(&manager)->grab().save(output+QString("/date-month-%1-%2.png").arg(size.width()).arg(dark));}}
    appearance->setDark(false);main.resize(1440,1000);QTest::qWait(120);
    clickControl(scene(&manager),"date-month-next");QTest::qWait(100);check(overview->property("month").toString()=="2027-01-01","month navigation uses real year boundaries");
    clickControl(scene(&manager),"date-picker-month-jump");QTest::qWait(100);clickControl(scene(&manager),"date-day-2027-01-15");QTest::qWait(100);check(overview->property("selectedDay").toString()=="2027-01-15","date picker changes the selected day without changing events");
    clickControl(scene(&manager),"date-picker-month-jump");appearance->setTextScale(1.3);main.resize(360,360);QTest::qWait(160);
    auto *popup=scene(&manager)->rootObject()->findChild<QObject *>("date-popup-month-jump");check(popup && popup->property("height").toReal()<=scene(&manager)->height(),"date picker stays within a short window at enlarged text size");
    clickControl(scene(&manager),"date-day-2027-02-07");QTest::qWait(100);check(overview->property("selectedDay").toString()=="2027-02-07","last-row dates remain selectable in a short window");
    appearance->setTextScale(1);main.resize(1440,1000);QTest::qWait(120);
    settings->setChecked(false);QTest::qWait(100);backend->sync();check(!backend->datesEnabled() && !item(scene(&manager)->rootObject(),"calendarDateOverview"),"disabling dates hides and unloads the month overview");
    calendarInspectorSpaceRegression(main,backend,scene(&manager),settings,calendar);
    manager.hide();main.resize(1024,640);
    qInfo()<<"PASS: simulation date setting, visible date planner, month navigation, date picker and responsive calendar overview";
}
