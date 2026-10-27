#pragma once
#include "../source/hills/HillStudioController.h"

static void hillStudioRegressions(MainWindow &main){
    QTemporaryDir storage;check(storage.isValid(),"isolated hill library");
    HillProfile hill=HillProfile::create(),decoded;QString error;
    check(HillProfile::fromJson(hill.toJson(),decoded,error),"hill profile round trip");
    for(int pair=0;pair<3;++pair)for(int value=-3;value<=3;++value){hill.perks[pair]=value;check(HillProfile::fromJson(hill.toJson(),decoded,error) && decoded.perks[pair]==value,"all hill perk sides and strengths round trip");hill.perks[pair]=0;}
    hill.hs=225;hill.perks[1]=3;check(qAbs(hill.realHs()-247.5)<.01,"forgiving flying hill extends landing threshold independently of HS");hill.hs=140;hill.perks[1]=-1;check(qAbs(hill.realHs()-137.9)<.01,"demanding hill can become difficult before HS");
    hill.perks[0]=3;check(hill.takeoffBias()>.5,"third-tier takeoff preference is pronounced");hill.perks[0]=-3;check(hill.takeoffBias()<-.5,"opposite hill preference rewards flight");hill.perks[2]=3;check(hill.windVariability()>2,"lottery tier III has a pronounced domain effect");hill.perks[2]=-3;check(hill.windVariability()<.5,"stability tier III suppresses variation");
    auto invalid=HillProfile::create().toJson();invalid["hs"]=10;check(!HillProfile::fromJson(invalid,decoded,error),"HS below K rejected");invalid=HillProfile::create().toJson();invalid["perks"]=QJsonArray{0,0,4};check(!HillProfile::fromJson(invalid,decoded,error),"hill tier IV rejected");
    HillStudioController backend(nullptr,storage.path()+"/library");const auto legacy=GlobalDatabase::get()->packsSnapshot();
    backend.add();backend.setValue("name","Test Hill");backend.setPerk(1,1,3,false);check(backend.profile()["perks"].toList()[1].toInt()==3,"controller stores third tier");backend.setPerk(1,-1,1,false);check(backend.profile()["perks"].toList()[1].toInt()==-1,"opposite replaces selected side");backend.undo();check(backend.profile()["perks"].toList()[1].toInt()==3,"undo restores tier III");backend.redo();check(backend.profile()["perks"].toList()[1].toInt()==-1,"redo restores opposite");backend.setValue("hs",10);check(backend.profile()["hs"].toDouble()==134,"invalid value leaves saved hill unchanged");
    HillStudioController reopened(nullptr,storage.path()+"/library");check(reopened.profile()["name"]=="Test Hill","hill edits persist across sessions");check(reopened.exportTo(storage.path()+"/export.json") && reopened.importFrom(storage.path()+"/export.json"),"hill pack exports and imports");check(reopened.packs().size()==2 && reopened.profile()["id"]!=backend.profile()["id"],"import creates independent pack and profile identities");backend.remove();backend.undo();check(backend.profiles().size()==1,"hill removal can be undone");check(GlobalDatabase::get()->packsSnapshot()==legacy,"authoring does not modify simulation data");
    {
        auto large=HillProfile::create().toJson();large["sourceId"]=QString(7*1024*1024,'x');
        QJsonObject pack{{"format","sj-sim-hill-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},{"name","Large pack"},{"profiles",QJsonArray{large}}};
        QFile file(storage.path()+"/large.json");check(file.open(QIODevice::WriteOnly),"large pack fixture");file.write(QJsonDocument(pack).toJson());file.close();
        HillStudioController largeBackend(nullptr,storage.path()+"/large-library");check(largeBackend.importFrom(file.fileName()),"readable large pack imports");largeBackend.duplicate();check(largeBackend.profiles().size()==2,"pack below read limit can grow");largeBackend.duplicate();check(largeBackend.profiles().size()==2,"write exceeding read limit is rejected without losing saved state");
        HillStudioController largeReopened(nullptr,storage.path()+"/large-library");check(largeReopened.profiles().size()==2,"large pack remains readable after rejected edit");
    }
    DatabaseEditorWindow window;auto *uiBackend=new HillStudioController(&window,storage.path()+"/ui");window.setProperty("hillStudio",QVariant::fromValue(static_cast<QObject *>(uiBackend)));window.findChild<QTabWidget *>()->setCurrentIndex(1);render(&window,"hill-studio-empty",true);QTest::qWait(80);
    check(item(scene(&window)->rootObject(),"hillStudio"),"modern hill studio opens in hills catalog");clickControl(scene(&window),"hillAdd");QTest::qWait(60);
    uiBackend->setValue("name","Vikersund");uiBackend->setValue("hs",225);uiBackend->setPerk(1,1,3,false);QTest::qWait(60);
    clickControl(scene(&window),"athletePerk-takeoff");check(uiBackend->profile()["perks"].toList()[0].toInt()==1,"hill badge selects tier I");clickControl(scene(&window),"athletePerkLevel-takeoff");clickControl(scene(&window),"athletePerkLevel-takeoff");check(uiBackend->profile()["perks"].toList()[0].toInt()==3,"hill badge upgrades through II to III");clickControl(scene(&window),"athletePerkLevel-takeoff");check(uiBackend->profile()["perks"].toList()[0].toInt()==1,"tier control returns from III to I");
    auto *view=scene(&window);
    for(const QString name:{QString("athletePerk-takeoff"),QString("athletePerkLevel-takeoff")}){
        const auto point=controlPoint(view,name);QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,point);QTest::mouseMove(view,QPoint(1,1));QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,QPoint(1,1));
        check(uiBackend->profile()["perks"].toList()[0].toInt()==1,"cancelled badge gesture leaves perk unchanged");
    }
    {
        QQmlComponent component(view->engine());component.setData(R"(import QtQuick
import "qrc:/ui"
SChoice { property int selected: 0; currentIndex: selected; width: 300; searchable: true; model: ["First","Disabled","Third"]; optionEnabled: [true,false,true] })",QUrl("qrc:/ui/ChoiceRegression.qml"));
        std::unique_ptr<QObject> choice(component.create(view->rootContext()));check(bool(choice),"searchable choice fixture loads");
        auto *results=choice->findChild<QQuickItem *>("choiceResults");check(results,"search result list exists");
        results->setProperty("currentIndex",1);QSignalSpy activated(choice.get(),SIGNAL(activated(int)));QMetaObject::invokeMethod(choice.get(),"chooseMatch");check(activated.isEmpty() && choice->property("currentIndex").toInt()==0,"search cannot activate a disabled option");
        results->setProperty("currentIndex",2);QMetaObject::invokeMethod(choice.get(),"chooseMatch");check(activated.count()==1 && choice->property("currentIndex").toInt()==2,QString("search updates selected index before activation (signals %1, index %2)").arg(activated.count()).arg(choice->property("currentIndex").toInt()));
        choice->setProperty("selected",1);check(choice->property("currentIndex").toInt()==1,"search selection preserves caller's index binding");
        check(choice->property("searchPlaceholder").toString()==QString::fromUtf8("Szukaj…") && choice->property("emptySearchText").toString()==QString::fromUtf8("Brak wyników"),"generic choice does not describe countries as hills");
    }
    {
        auto *number=item(view->rootObject(),"hill-k");check(number,"hill K number exists");
        uiBackend->setValue("hs",500);QTest::qWait(10);check(number->property("fields").value<QJSValue>().property("maximum").toNumber()==400,"K UI limit matches domain maximum");
        number->setProperty("draft","999");auto *input=item(number,"input-hill-k");check(input,"hill numeric draft input exists");input->setProperty("dirty",true);
        uiBackend->add();uiBackend->setValue("k",90);QTest::qWait(10);check(number->property("draft").toString()!=QString("999") && !input->property("dirty").toBool(),"changing hill discards the previous numeric draft");
        auto *library=item(view->rootObject(),"hillLibrary");check(library,"hill library fixture");library->setProperty("query","Vikersund");QTest::qWait(10);auto *list=item(library,"hillList");check(list && list->property("count").toInt()==1 && list->property("currentIndex").toInt()==-1,"filtered-out selection leaves no stale keyboard row");library->setProperty("query","");
    }
    main.resize(1366,768);render(&window,"hill-studio-1366",true);main.resize(360,480);render(&window,"hill-studio-small",true);check(item(scene(&window)->rootObject(),"hillEditorScroll"),"small hill editor remains scrollable");window.hide();
    qInfo()<<"PASS: hill profiles, perk strengths, atomic persistence, packs, undo and responsive hill editor";
}
