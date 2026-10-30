#pragma once
#include "../source/hills/HillStudioController.h"
#include "../source/simulation-v2/ProfileBridge.h"

static void hillStudioRegressions(MainWindow &main){
    QTemporaryDir storage;check(storage.isValid(),"isolated hill library");
    HillProfile hill=HillProfile::create(),decoded;QString error;
    check(HillProfile::fromJson(hill.toJson(),decoded,error),"hill profile round trip");
    hill.headwindPoints=10;hill.tailwindPoints=2;auto oldWind=hill.toJson();oldWind["tailwindPoints"]=2;check(HillProfile::fromJson(oldWind,decoded,error) && decoded.tailwindPoints==15,"loaded hill derives tailwind points from headwind");check(SimulationV2::hillInput(hill).tailwindPoints==15,"simulation uses the derived tailwind points");hill.headwindPoints=100;check(hill.toJson()["tailwindPoints"].toDouble()==100,"derived compensation respects the supported maximum");hill=HillProfile::create();
    {Hill value;check(value.setSimulationProfile(hill,error),"hill draft fixture");SimulationProfileDraft draft(DataPack::Hills);draft.load(value);draft.setValue("headwindPoints",13);check(draft.profile()["tailwindPoints"].toDouble()==19.5,"season hill draft derives tailwind points");draft.setValue("tailwindPoints",1);check(draft.profile()["tailwindPoints"].toDouble()==19.5,"season hill draft rejects direct tailwind edits");}
    for(int pair=0;pair<4;++pair)for(int value=-4;value<=4;++value){hill.perks[pair]=value;check(HillProfile::fromJson(hill.toJson(),decoded,error) && decoded.perks[pair]==value,"all hill perk sides and strengths round trip");hill.perks[pair]=0;}
    auto threePairs=hill.toJson();threePairs["perks"]=QJsonArray{2,-1,3};check(HillProfile::fromJson(threePairs,decoded,error) && decoded.id==hill.id && decoded.perks[0]==2 && decoded.perks[1]==-1 && decoded.perks[2]==3 && decoded.perks[3]==0,"old three-pair profile preserves identity and perks with neutral direction");
    check(decoded.toJson()["perks"].toArray().size()==4,"new saves include fourth hill perk");
    hill.perks[3]=4;check(qAbs(hill.headwindProbability()-.94)<1e-9,"headwind preference tier IV");hill.perks[3]=-4;check(qAbs(hill.headwindProbability()-.06)<1e-9,"tailwind preference tier IV");hill.perks[3]=0;
    hill.hs=225;hill.perks[1]=4;check(qAbs(hill.realHs()-247.5)<.01,"forgiving flying hill extends landing threshold independently of HS");hill.hs=140;hill.perks[1]=-1;check(qAbs(hill.realHs()-138.425)<.01,"demanding hill can become difficult before HS");
    hill.perks[0]=4;check(hill.takeoffBias()>.5,"fourth-tier takeoff preference is pronounced");hill.perks[0]=-4;check(hill.takeoffBias()<-.5,"opposite hill preference rewards flight");hill.perks[2]=4;check(hill.windVariability()>2,"lottery extreme tier IV has a pronounced domain effect");hill.perks[2]=-4;check(hill.windVariability()<.5,"stability extreme tier IV suppresses variation");
    auto invalid=HillProfile::create().toJson();invalid["hs"]=10;check(!HillProfile::fromJson(invalid,decoded,error),"HS below K rejected");invalid=HillProfile::create().toJson();invalid["perks"]=QJsonArray{0,0,5,0};check(!HillProfile::fromJson(invalid,decoded,error),"hill tier V rejected");
    HillStudioController backend(nullptr,storage.path()+"/library");const auto legacy=GlobalDatabase::get()->packsSnapshot();
    backend.add();QSignalSpy broadHillUpdates(&backend,&HillStudioController::changed),hillPerkUpdates(&backend,&HillStudioController::perksChanged);
    backend.setPerk(1,1,3,false);check(broadHillUpdates.isEmpty() && hillPerkUpdates.count()==1 && backend.realHs()>134,"hill perk refreshes its derived threshold without rebuilding the library");backend.remove();
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
    check(item(scene(&window)->rootObject(),"hillStudio"),"modern hill studio opens in hills catalog");
    clickControl(scene(&window),"manageStudioPack");presentation(&window)->grab().save(output+"/hill-pack-settings.png");auto *packName=item(scene(&window)->quickWindow()->contentItem(),"studioPackName");check(packName,"hill pack settings show the name");packName->setProperty("text","Hill pack renamed");clickControl(scene(&window),"saveStudioPackName");check(uiBackend->packName()=="Hill pack renamed","hill pack settings save the selected pack name");clickControl(scene(&window),"closeStudioPack");QTest::qWait(200);
    clickControl(scene(&window),"hillAdd");QTest::qWait(60);
    uiBackend->setValue("name","Vikersund");uiBackend->setValue("hs",225);uiBackend->setPerk(1,1,3,false);QTest::qWait(60);
    uiBackend->setValue("headwindPoints",10);uiBackend->setValue("tailwindPoints",1);QTest::qWait(60);check(uiBackend->profile()["tailwindPoints"].toDouble()==15,"studio derives tailwind points and rejects direct edits");auto *tailwind=item(scene(&window)->rootObject(),"hill-tailwindPoints");check(tailwind && !tailwind->isEnabled(),"derived tailwind points remain visible but cannot be edited");
    main.resize(1440,900);QTest::qWait(100);const auto originalHill=uiBackend->profile()["id"].toString();uiBackend->add();uiBackend->setValue("country","AUT");uiBackend->select(0);QTest::qWait(80);
    clickControl(scene(&window),"sortByCountry");clickControl(scene(&window),"cancelCountrySort");QTest::qWait(200);check(uiBackend->profiles()[0].toMap()["id"]==originalHill,"cancelling country sort keeps hill order");
    clickControl(scene(&window),"sortByCountry");clickControl(scene(&window),"confirmCountrySortAction");QTest::qWait(100);check(uiBackend->profiles()[0].toMap()["country"]=="AUT" && uiBackend->profile()["id"]==originalHill,"confirmed country sort groups hills and preserves selection");
    HillStudioController afterSort(nullptr,storage.path()+"/ui");check(afterSort.profiles()[0].toMap()["country"]=="AUT","hill country order persists");uiBackend->undo();QTest::qWait(60);check(uiBackend->profiles()[0].toMap()["id"]==originalHill,"hill country sorting can be undone");uiBackend->select(1);uiBackend->remove();uiBackend->select(0);QTest::qWait(80);
    check(item(scene(&window)->rootObject(),"hillScoring")->isVisible(),"hill scoring is expanded by default");
    uiBackend->setValue("country","NOR");QTest::qWait(20);
    check(item(scene(&window)->rootObject(),"hillListFlag-0")->property("source").toUrl()==QUrl(uiBackend->profile()["flag"].toString()),"hill list flag follows the edited country");
    check(item(scene(&window)->rootObject(),"hillListHs-0")->property("text").toString()=="HS225","hill list HS has no extra space");
    clickControl(scene(&window),"hillPerk-takeoff-1");check(uiBackend->profile()["perks"].toList()[0].toInt()==1,"hill strength selector applies light preference");
    clickControl(scene(&window),"hillPerk-takeoff-3");check(uiBackend->profile()["perks"].toList()[0].toInt()==3,"strong hill preference can be selected directly");
    check(item(scene(&window)->rootObject(),"hillPerk-takeoff-3")->property("checked").toBool() && !item(scene(&window)->rootObject(),"hillPerk-takeoff-1")->property("checked").toBool(),"current hill strength is highlighted");
    clickControl(scene(&window),"hillPerk-takeoff-3");check(uiBackend->profile()["perks"].toList()[0].toInt()==0,"clicking selected strength removes the hill perk");
    clickControl(scene(&window),"hillPerk-takeoff-2");clickControl(scene(&window),"hillPerk-flying-3");check(uiBackend->selectedPerks()[0].toInt()==-3,"opposite hill badge directly replaces the selected strength");
    clickControl(scene(&window),"hillPerk-flying");check(uiBackend->selectedPerks()[0].toInt()==0,"clicking the selected hill badge header removes the perk");
    clickControl(scene(&window),"hillPerk-takeoff-1");
    auto *view=scene(&window);
    for(const QString name:{QString("hillPerk-takeoff-1"),QString("hillPerk-takeoff-3")}){
        const auto point=controlPoint(view,name);QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,point);
        QMouseEvent move(QEvent::MouseMove,QPointF(1,1),QPointF(view->mapToGlobal(QPoint(1,1))),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(view,&move);
        QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,QPoint(1,1));
        check(uiBackend->profile()["perks"].toList()[0].toInt()==1,QString("cancelled badge gesture leaves perk unchanged: %1, value %2").arg(name).arg(uiBackend->profile()["perks"].toList()[0].toInt()));
    }
    {
        QQmlComponent component(view->engine());component.setData(R"(import QtQuick 2.15
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
    uiBackend->setPerk(0,1,1,false);uiBackend->setPerk(1,1,2,false);uiBackend->setPerk(2,1,3,false);
    main.resize(1366,768);render(&window,"hill-studio-1366",true);main.resize(360,480);render(&window,"hill-studio-small",true);check(item(scene(&window)->rootObject(),"hillEditorScroll"),"small hill editor remains scrollable");window.hide();
    qInfo()<<"PASS: hill profiles, perk strengths, atomic persistence, packs, undo and responsive hill editor";
}
