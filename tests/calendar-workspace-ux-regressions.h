#pragma once

static void calendarWorkspaceUxRegressions(QuickCalendar *backend,QQuickWidget *view,CompetitionInfo *first,CompetitionInfo *second) {
    QAbstractItemModelTester contract(backend->eventModel(),QAbstractItemModelTester::FailureReportingMode::Fatal);
    QSignalSpy resets(backend->eventModel(),&QAbstractItemModel::modelReset);
    const int original=backend->events().size();
    backend->addMany(30);QTest::qWait(100);
    QPointer<QQuickItem> rows=item(view->rootObject(),"calendarEvents");check(rows,"calendar has a stable scrolling list");
    rows->setProperty("contentY",500.);QTest::qWait(80);const double scroll=rows->property("contentY").toReal();check(scroll>100,"calendar scroll regression uses a genuinely long list");
    backend->add();QTest::qWait(100);view->grabFramebuffer();
    check(rows && std::abs(rows->property("contentY").toReal()-scroll)<2 && resets.isEmpty(),"adding a competition preserves the scrolled position without resetting the list model");
    backend->undo();backend->undo();QTest::qWait(80);check(backend->events().size()==original,"temporary long-list additions undo completely");
    rows->setProperty("contentY",0.);backend->select(first->getIDStr());QTest::qWait(80);
    hillSearchRegression(view,"calendar-field-hill",backend->hills());
    const int searched=backend->selection().value("hill").toInt();
    backend->select(second->getIDStr());QTest::qWait(100);
    check(item(view->rootObject(),"calendar-field-hill")->property("currentIndex").toInt()==backend->selection().value("hill").toInt(),"hill selector follows another event after using its searchable popup");
    backend->select(first->getIDStr());QTest::qWait(100);
    check(item(view->rootObject(),"calendar-field-hill")->property("currentIndex").toInt()==searched,"returning to the edited event restores its own hill selection");
    backend->undo();backend->select(first->getIDStr());
    const int rule=qMin(2,backend->rules().size()-1);backend->update({{"rules",rule},{"type",CompetitionInfo::Qualifications}});QTest::qWait(80);
    backend->select(second->getIDStr());QTest::qWait(100);
    check(item(view->rootObject(),"calendar-field-rules")->property("currentIndex").toInt()==backend->selection().value("rules").toInt(),"rules selector follows the selected event immediately");
    backend->select(first->getIDStr());QTest::qWait(100);
    check(item(view->rootObject(),"calendar-field-rules")->property("currentIndex").toInt()==rule,"returning to an event refreshes its configured rules");
    if(backend->datesEnabled()) {
        backend->assignDate("2026-10-25");QTest::qWait(80);bool found=false;
        for(const auto &value:backend->events()){const auto event=value.toMap();if(event.value("id").toString()==first->getIDStr()){check(!event.value("weekday").toString().isEmpty() && !event.value("flag").toString().isEmpty() && event.value("qualification").toBool(),"dated calendar entries expose weekday, hill flag and qualification marker");found=true;}}
        check(found,"dated edited event remains in the calendar");
        auto *badge=item(view->rootObject(),"calendar-qualification-"+first->getIDStr());check(badge && badge->isVisible(),"qualification badge is directly visible in the compact event row");backend->undo();
    }
    backend->undo();
    check(resets.isEmpty() && backend->eventModel()->rowCount()==backend->events().size(),"selection, edits and undo keep the calendar model contract without resetting delegates");
    qInfo()<<"PASS: calendar scroll retention, live hill/rules rebinding, flags and dated qualification markers";
}
