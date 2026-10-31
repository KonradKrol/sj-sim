#pragma once
#include <QQmlComponent>
#include <QStandardItemModel>

static void choicePopupRegressions(MainWindow &main) {
    main.show();main.resize(1024,700);
    auto *bridge=main.findChild<QuickBridge *>();bridge->launch("dismissWelcome");QTest::qWait(80);
    auto *view=scene(&main);auto *appearance=view->rootContext()->contextProperty("appearance").value<QuickAppearance *>();
    const auto oldScale=appearance->uiScale();const auto oldDark=appearance->dark();
    QQmlComponent component(view->engine());component.setData("import QtQuick 2.15\nimport \"qrc:/ui\"\nSChoice { x: 50; y: 70; width: 250 }",QUrl("qrc:/ui/ChoiceProbe.qml"));
    check(component.isReady(),component.errorString());
    std::unique_ptr<QQuickItem> choice(qobject_cast<QQuickItem *>(component.create(view->rootContext())));check(bool(choice),"choice popup fixture");choice->setParentItem(view->rootObject());
    QStandardItemModel standard;for(const auto &label:QStringList{"Pierwsza","Druga","Trzecia"})standard.appendRow(new QStandardItem(label));
    const QVariantList objects{QVariantMap{{"label","Pierwsza"}},QVariantMap{{"label","Druga"}},QVariantMap{{"label","Trzecia"}}};
    for(double scale:{.7,1.})for(bool dark:{false,true})for(int model=0;model<3;++model)for(bool searchable:{false,true}){
        appearance->setUiScale(scale);appearance->setDark(dark);
        choice->setProperty("model",QVariant());choice->setProperty("textRole",model==1?"label":model==2?"display":"");
        if(model==0)choice->setProperty("model",QStringList{"Pierwsza","Druga","Trzecia"});
        else if(model==1)choice->setProperty("model",objects);
        else choice->setProperty("model",QVariant::fromValue(static_cast<QObject *>(&standard)));
        choice->setProperty("searchable",searchable);choice->setProperty("currentIndex",0);
        auto *popup=choice->property("popup").value<QObject *>();check(popup,"choice popup exists");QMetaObject::invokeMethod(popup,"open");QTest::qWait(220);view->grabFramebuffer();
        auto *rows=popup->findChild<QQuickItem *>("choiceResults");check(rows && rows->height()>20,"popup assigns a visible viewport before delegates are measured");
        for(int i=0;i<3;++i){auto *label=item(view->quickWindow()->contentItem(),"choiceOptionLabel-"+QString::number(i));check(label && label->isVisible() && label->width()>10 && label->height()>5,"every option has visible text and usable dimensions");check(label->property("text").toString()==QStringList{"Pierwsza","Druga","Trzecia"}[i],"popup resolves labels for string, object and abstract-item models");}
        auto *last=item(view->quickWindow()->contentItem(),"choiceOption-2");const auto point=last->mapToItem(view->rootObject(),last->boundingRect().center()).toPoint();QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,point);QTest::qWait(160);
        check(choice->property("currentIndex").toInt()==2,"clicking a visible popup option selects its source index");QMetaObject::invokeMethod(popup,"close");QTest::qWait(90);
    }
    appearance->setUiScale(oldScale);appearance->setDark(oldDark);
    qInfo()<<"PASS: visible popup content and selection across models, themes, scales and search modes";
}
