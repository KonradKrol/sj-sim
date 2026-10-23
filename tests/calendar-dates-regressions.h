#pragma once
#include "../source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.h"

static void calendarDateRegressions()
{
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
    backend.select(first->getIDStr());backend.assignDate("2026-04-20");check(!backend.dateWarnings().isEmpty(),"dates that contradict execution order are disclosed");
    backend.selectAll();backend.sortDates();check(calendar->getCompetitionsReference().last()==first,"date sorting changes future execution order explicitly");backend.undo();check(calendar->getMainCompetitionForEvent(calendar->getCompetitionsReference().first())==first,"date ordering is undoable");
    const auto oldDates=backend.events();backend.duplicate();check(backend.events().size()==12 && backend.events()[6].toMap().value("date")==oldDates.first().toMap().value("date"),"duplicates preserve event dates");backend.undo();
    first->setPlayed(true);backend.select(first->getIDStr());backend.planDates("2030-01-01","weekly",7,true);check(backend.datePlan().size()==5,"planning all future events skips played selections");backend.applyDatePlan();check(first->getScheduledDate()==QDate(2026,4,20) && backend.events()[1].toMap().value("date").toString()=="2030-01-01","future plans leave historical dates unchanged");
    backend.setDatesEnabled(false);check(!backend.datesEnabled() && !backend.canEnableDates() && first->getScheduledDate().isValid(),"dates can be disabled after season start without deleting the plan");backend.setDatesEnabled(true);check(!backend.datesEnabled(),"enabling dates after season start is rejected by the backend");
    IdentifiableObjectsStorage storage;storage.add(save.getHillsReference());
    auto restored=Season::getFromJson(Season::getJsonObject(*save.getActualSeason()),&storage);
    check(!restored.getDatesEnabled() && restored.hasStarted() && !restored.setDatesEnabled(true) && restored.getActualCalendar()->getCompetitionsReference().first()->getScheduledDate()==QDate(2026,4,20),"season reload preserves dates, mode and the started latch");
    SeasonCalendarPreset preset(calendar);auto *presetFirst=preset.getCalendarReference().getCompetitionsReference().first();check(!presetFirst->getPlayed() && presetFirst->getScheduledDate()==first->getScheduledDate(),"calendar presets retain dates without played state");
    IdentifiableObjectsStorage presetStorage;auto restoredPreset=SeasonCalendarPreset::getFromJson(SeasonCalendarPreset::getJsonObject(preset),&presetStorage);check(restoredPreset.getCalendarReference().getCompetitionsReference().first()->getScheduledDate()==first->getScheduledDate(),"preset JSON round trip retains dates");
    for(auto *cal:restored.getCalendarsReference()){qDeleteAll(cal->getCompetitionsReference());delete cal;}
    qDeleteAll(preset.getCalendarReference().getCompetitionsReference());qDeleteAll(restoredPreset.getCalendarReference().getCompetitionsReference());
    qInfo()<<"PASS: optional calendar dates, weekend planning, leap years, DST, offsets, undo, history locks, persistence and presets";
}

static void calendarDateUiRegressions(MainWindow &main)
{
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
    main.resize(1440,1000);QTest::qWait(120);backend->selectAll();
    clickControl(scene(&manager),"calendarPlanDates");QTest::qWait(100);
    check(item(scene(&manager)->rootObject(),"calendarDatePlanner"),"planner opens directly from calendar toolbar");
    auto *input=item(scene(&manager)->rootObject(),"date-input-plan-start");check(input,"planner date input exists");
    clickControl(scene(&manager),"date-input-plan-start");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),"2026-11-27");QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(80);
    clickControl(scene(&manager),"date-plan-preview");QTest::qWait(100);check(backend->datePlan().size()==10 && backend->datePlan().first().toMap().value("date").toString()=="2026-11-28","visible planner previews weekend dates");
    clickControl(scene(&manager),"date-plan-apply");QTest::qWait(120);check(calendar->getCompetitionsReference().first()->getScheduledDate()==QDate(2026,11,28),"visible apply button commits the date plan");
    workspace->setProperty("editing",false);QTest::qWait(80);clickControl(scene(&manager),"calendarMonthToggle");QTest::qWait(150);
    auto *overview=item(scene(&manager)->rootObject(),"calendarDateOverview");check(overview,"month overview opens");overview->setProperty("month","2026-12-01");
    auto *appearance=scene(&manager)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();
    for(bool dark:{false,true}){appearance->setDark(dark);for(const auto size:{QSize(1440,1000),QSize(1024,850),QSize(520,850),QSize(360,800)}){main.resize(size);QTest::qWait(180);presentation(&manager)->grab().save(output+QString("/date-month-%1-%2.png").arg(size.width()).arg(dark));}}
    appearance->setDark(false);main.resize(1440,1000);QTest::qWait(120);
    clickControl(scene(&manager),"date-month-next");QTest::qWait(100);check(overview->property("month").toString()=="2027-01-01","month navigation uses real year boundaries");
    clickControl(scene(&manager),"date-picker-month-jump");QTest::qWait(100);clickControl(scene(&manager),"date-day-2027-01-15");QTest::qWait(100);check(overview->property("selectedDay").toString()=="2027-01-15","date picker changes the selected day without changing events");
    settings->setChecked(false);QTest::qWait(100);backend->sync();check(!backend->datesEnabled() && !item(scene(&manager)->rootObject(),"calendarDateOverview"),"disabling dates hides and unloads the month overview");
    manager.hide();main.resize(1024,640);
    qInfo()<<"PASS: simulation date setting, visible date planner, month navigation, date picker and responsive calendar overview";
}
