#pragma once

static void calendarInspectorSpaceRegression(MainWindow &main,QuickCalendar *backend,QQuickWidget *view,QCheckBox *dates,SeasonCalendar *calendar) {
    auto *workspace=item(view->rootObject(),"calendarWorkspace");
    check(workspace,"calendar workspace exists for inspector space regression");
    workspace->setProperty("monthView",false);
    workspace->setProperty("editing",true);
    for(bool dated:{true,false}) {
        dates->setChecked(dated);QTest::qWait(80);backend->sync();
        for(const auto &size:{QSize(1440,900),QSize(1024,640),QSize(800,600),QSize(520,640),QSize(360,640)}) {
            main.resize(size);QTest::qWait(100);
            for(int section:{0,2,1,0}) {
                backend->select(calendar->getCompetitionsReference()[section%2]->getIDStr());
                auto *inspector=item(view->rootObject(),"calendarInspector");
                inspector->setProperty("section",section);QTest::qWait(100);view->grabFramebuffer();
                auto *fields=item(view->rootObject(),"calendarInspectorFields");
                auto *heading=item(view->rootObject(),"calendarInspectorHeading");
                auto *content=inspector->property("contentItem").value<QQuickItem *>();
                check(fields && heading && content && fields->isVisible(),"inspector fields stay visible after changing event and tab");
                const double gap=inspector->property("compact").toBool()?8:12;
                const double expected=content->height()-heading->height()-gap;
                check(fields->height()>=40 && std::abs(fields->height()-expected)<2,"inspector fields use all space below the header across desktop, laptop and narrow sizes");
                const auto bounds=fields->mapRectToItem(view->rootObject(),fields->boundingRect());
                check(bounds.top()>=0 && bounds.bottom()<=view->height()+1,"inspector field viewport stays inside the screen");
            }
            view->grabFramebuffer().save(output+QString("/calendar-inspector-space-%1-%2.png").arg(size.width()).arg(dated));
        }
    }
    main.resize(1440,1000);
    qInfo()<<"PASS: calendar inspector uses available vertical space after event, tab, date mode and window changes";
}
