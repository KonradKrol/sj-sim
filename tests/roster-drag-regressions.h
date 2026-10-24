#pragma once
static void rosterLongDragRegression(CompetitionConfigWindow &config,QuickRoster *roster,const QVector<Jumper *> &pool) {
    const auto original=config.getCompetitionJumpers();
    config.setCompetitionJumpers(pool.mid(0,qMin(100,pool.size())));roster->sync();QTest::qWait(120);
    auto *view=scene(&config);auto *rows=item(view->rootObject(),"rosterOrder");auto *gesture=item(view->rootObject(),"rosterDragArea");
    check(rows&&gesture,"persistent roster drag surface exists");
    rows->setProperty("contentY",0.);QTest::qWait(80);roster->mark(QuickRoster::key(pool[0]));
    auto *grip=item(view->rootObject(),"roster-grip-"+QuickRoster::key(pool[0]));check(grip,"first long-list drag grip exists");
    const auto begin=grip->mapToItem(view->rootObject(),grip->boundingRect().center()).toPoint();
    const auto end=gesture->mapToItem(view->rootObject(),QPointF(20,gesture->height()-3)).toPoint();
    QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,begin);
    for(int step=1;step<=12;++step){const QPoint at=begin+(end-begin)*step/12;QMouseEvent move(QEvent::MouseMove,at,view->mapToGlobal(at),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);QTest::qWait(12);}
    QTest::qWait(550);
    auto *preview=item(view->quickWindow()->contentItem(),"rosterDragPreview");
    check(gesture->property("dragging").toBool() && rows->property("contentY").toReal()>300 && preview && preview->isVisible() && gesture->property("athleteName").toString()==pool[0]->getNameAndSurname(),"edge scrolling retains the dragged athlete and visible preview after the source row is virtualized away");
    const double beforeWheel=rows->property("contentY").toReal();QWheelEvent wheel(QPointF(end),QPointF(view->mapToGlobal(end)),QPoint(),QPoint(0,-120),Qt::LeftButton,Qt::NoModifier,Qt::NoScrollPhase,false);QApplication::sendEvent(view,&wheel);QTest::qWait(80);
    check(rows->property("contentY").toReal()>beforeWheel,"mouse wheel accelerates scrolling without releasing the dragged athlete");
    QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,end);QTest::qWait(120);
    check(config.getCompetitionJumpers().indexOf(pool[0])>10 && !gesture->property("dragging").toBool(),"long drag drops at the current scrolled destination rather than a stale delegate");
    roster->undo();check(config.getCompetitionJumpers().first()==pool[0],"long drag remains undoable");
    config.setCompetitionJumpers(original);roster->sync();QTest::qWait(80);
    qInfo()<<"PASS: persistent athlete preview, accelerated edge/wheel scrolling and long-distance roster drop";
}
