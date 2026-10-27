#pragma once

static void motionFeedbackRegressions(MainWindow &main) {
    main.show();QTest::qWait(50);
    auto *view=scene(&main);auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const bool reduced=appearance->reducedMotion();
    appearance->setReducedMotion(false);
    QQmlComponent pulse(view->engine());pulse.setData(R"(import QtQuick
import "qrc:/ui"
Item { id: sample; property int revision: 0; readonly property real progress: feedback.progress; ChangeFeedback { id: feedback; target: sample; trigger: sample.revision; duration: 200 } })",QUrl("qrc:/ui/MotionRegression.qml"));
    std::unique_ptr<QObject> object(pulse.create(view->rootContext()));check(bool(object),"state-feedback fixture loads");
    check(object->property("progress").toReal()==1,"state feedback starts settled");object->setProperty("revision",1);QTest::qWait(30);check(object->property("progress").toReal()>0 && object->property("progress").toReal()<1,"state change animates feedback");
    appearance->setReducedMotion(true);check(object->property("progress").toReal()==1,"reduced motion immediately settles active feedback");object->setProperty("revision",2);check(object->property("progress").toReal()==1,"reduced motion prevents another transition");
    QQmlComponent field(view->engine());field.setData(R"(import QtQuick
import "qrc:/ui"
NumberField { direct: false; fields: ({id:"motionValue",name:"motionValue",value:12,minimum:0,maximum:100,step:1,decimals:0}) })",QUrl("qrc:/ui/NumberMotionRegression.qml"));
    std::unique_ptr<QObject> number(field.create(view->rootContext()));check(bool(number),"numeric validation fixture loads");QSignalSpy committed(number.get(),SIGNAL(committed(double)));
    number->setProperty("draft","invalid");QMetaObject::invokeMethod(number.get(),"commitInput");check(number->property("draft").toString()=="12" && number->property("currentValue").toReal()==12 && committed.isEmpty(),"invalid localized numeric input restores the saved value without committing");
    number->setProperty("draft","13");QMetaObject::invokeMethod(number.get(),"commitInput");check(number->property("currentValue").toReal()==13 && committed.count()==1,"valid numeric input remains immediate");
    appearance->setReducedMotion(reduced);
    qInfo()<<"PASS: state feedback, reduced-motion cancellation and numeric validation feedback";
}
