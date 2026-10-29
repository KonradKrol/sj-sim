#pragma once

static void motionFeedbackRegressions(MainWindow &main) {
    main.show();QTest::qWait(50);
    auto *view=scene(&main);auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const bool reduced=appearance->reducedMotion();
    appearance->setReducedMotion(false);
    QQmlComponent pulse(view->engine());pulse.setData(R"(import QtQuick 2.15
import "qrc:/ui"
Item { id: sample; property int revision: 0; readonly property real progress: feedback.progress; ChangeFeedback { id: feedback; target: sample; trigger: sample.revision; duration: 200 } })",QUrl("qrc:/ui/MotionRegression.qml"));
    std::unique_ptr<QObject> object(pulse.create(view->rootContext()));check(bool(object),"state-feedback fixture loads");
    check(object->property("progress").toReal()==1,"state feedback starts settled");object->setProperty("revision",1);QTest::qWait(30);check(object->property("progress").toReal()>0 && object->property("progress").toReal()<1,"state change animates feedback");
    appearance->setReducedMotion(true);check(object->property("progress").toReal()==1,"reduced motion immediately settles active feedback");object->setProperty("revision",2);check(object->property("progress").toReal()==1,"reduced motion prevents another transition");
    QQmlComponent field(view->engine());field.setData(R"(import QtQuick 2.15
import "qrc:/ui"
NumberField { direct: false; fields: ({id:"motionValue",name:"motionValue",value:12,minimum:0,maximum:100,step:1,decimals:0}) })",QUrl("qrc:/ui/NumberMotionRegression.qml"));
    std::unique_ptr<QObject> number(field.create(view->rootContext()));check(bool(number),"numeric validation fixture loads");QSignalSpy committed(number.get(),SIGNAL(committed(double)));
    number->setProperty("draft","invalid");QMetaObject::invokeMethod(number.get(),"commitInput");check(number->property("draft").toString()=="12" && number->property("currentValue").toReal()==12 && committed.isEmpty(),"invalid localized numeric input restores the saved value without committing");
    number->setProperty("draft","13");QMetaObject::invokeMethod(number.get(),"commitInput");check(number->property("currentValue").toReal()==13 && committed.count()==1,"valid numeric input remains immediate");
    appearance->setReducedMotion(false);
    QQmlComponent arrival(view->engine());arrival.setData(R"(import QtQuick 2.15
import "qrc:/ui"
Item { id: sample; width: 60; height: 40; z: 10000; property int clicks: 0; MouseArea { anchors.fill: parent; onClicked: sample.clicks++ } property int revision: 0; readonly property bool animating: entering.motion.running; Item { id: heading; width: 20; height: 10 } ViewArrival { id: entering; target: sample; expressive: true; emphasis: heading } })",QUrl("qrc:/ui/ArrivalRegression.qml"));
    std::unique_ptr<QObject> arriving(arrival.create(view->rootContext()));check(bool(arriving),"view-arrival fixture loads");auto *arrivingItem=qobject_cast<QQuickItem *>(arriving.get());arrivingItem->setParentItem(view->rootObject());QTest::qWait(30);check(arriving->property("animating").toBool(),"screen arrival starts when it enters the visible scene");check(arrivingItem->opacity()>0 && arrivingItem->scale()==1,"arrival keeps interactive geometry fixed and never removes the view from mouse hit testing");QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,arrivingItem->mapToItem(view->rootObject(),QPointF(30,20)).toPoint());check(arriving->property("clicks").toInt()==1,"a real mouse click works while the entry animation is running");
    appearance->setReducedMotion(true);QTest::qWait(40);check(!arriving->property("animating").toBool() && arrivingItem->opacity()==1 && arrivingItem->scale()==1,"reduced motion cancels arrival and restores legible geometry");
    appearance->setReducedMotion(false);arrivingItem->setVisible(false);arrivingItem->setVisible(true);QTest::qWait(30);check(arriving->property("animating").toBool(),"showing a panel starts a new entry transition");arrivingItem->setVisible(false);QTest::qWait(20);check(!arriving->property("animating").toBool(),"hidden views do not keep animating");appearance->setReducedMotion(true);arrivingItem->setVisible(true);QTest::qWait(20);check(!arriving->property("animating").toBool(),"reduced motion suppresses future arrivals");appearance->setReducedMotion(false);arriving->setProperty("revision",1);check(!arriving->property("animating").toBool(),"ordinary field updates never restart page arrival");
    appearance->setReducedMotion(reduced);
    qInfo()<<"PASS: state feedback, reduced-motion cancellation and numeric validation feedback";
}
