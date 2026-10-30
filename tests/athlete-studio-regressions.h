#pragma once
#include "../source/athletes/AthleteStudioController.h"
#include "../source/athletes/AthleteProfile.h"
#include "../source/hills/HillStudioController.h"
#include "../source/simulation-v2/ProfileDraft.h"
#include <QStandardPaths>

static void athleteLibraryRegressions(QQuickWidget *view,AthleteStudioController *backend){
    auto *library=item(view->rootObject(),"athleteLibrary");auto *rows=item(view->rootObject(),"athleteList");auto *editor=item(view->rootObject(),"athleteEditor");
    if(!library || !rows || !editor)qCritical()<<"Athlete workspace"<<library<<rows<<editor<<backend->selectedIndex()<<backend->message();
    check(library && rows && editor,"athlete list and editor share the workspace");
    check(library->mapToScene(QPointF()).x()+library->width()<=editor->mapToScene(QPointF()).x(),"athlete list stays to the left of the editor");
    for(int n=1;n<=40;++n){backend->add();backend->setValue("name",QString("Test%1").arg(n,2,10,QLatin1Char('0')));backend->setValue("surname","Lista");}
    backend->setValue("country","NOR");backend->select(0);QTest::qWait(80);check(rows->property("count").toInt()==41,"all profiles appear in the side list");
    const auto movedId=backend->profile()["id"].toString();auto *handle=item(view->rootObject(),"athleteDragHandle-0");auto *target=item(view->rootObject(),"athleteListRow-2");check(handle && target,"drag handle and destination are visible");
    const auto start=handle->mapToItem(view->rootObject(),handle->boundingRect().center()).toPoint(),finish=target->mapToItem(view->rootObject(),target->boundingRect().center()).toPoint();
    QTest::mousePress(view,Qt::LeftButton,Qt::NoModifier,start);for(int i=1;i<=8;++i)QTest::mouseMove(view,start+(finish-start)*i/8,30);QTest::mouseRelease(view,Qt::LeftButton,Qt::NoModifier,finish);QTest::qWait(100);
    check(backend->profiles()[2].toMap()["id"]==movedId && backend->profile()["id"]==movedId,"dragging changes order and preserves the selected athlete");
    AthleteStudioController afterDrag(nullptr,QFileInfo(backend->packDirectory()).dir().absolutePath());check(afterDrag.profiles()[2].toMap()["id"]==movedId,"dragged order persists when the pack is reopened");backend->undo();QTest::qWait(60);check(backend->profiles()[0].toMap()["id"]==movedId,"undo restores the previous athlete order");
    rows->forceActiveFocus();QTest::keyClick(view,Qt::Key_Down,Qt::AltModifier);QTest::qWait(60);check(backend->profiles()[1].toMap()["id"]==movedId && backend->profile()["id"]==movedId,"Alt+Down moves the selected athlete without changing selection");backend->undo();QTest::qWait(60);
    backend->select(1);const auto otherId=backend->profile()["id"].toString();backend->move(0,2);check(backend->profile()["id"]==otherId && backend->selectedIndex()==0,"moving a different athlete keeps the same selection");backend->undo();backend->select(0);QTest::qWait(60);
    clickControl(view,"sortByCountry");clickControl(view,"cancelCountrySort");QTest::qWait(200);check(backend->profiles()[0].toMap()["id"]==movedId,"cancelling country sort keeps athlete order");
    clickControl(view,"sortByCountry");clickControl(view,"confirmCountrySortAction");QTest::qWait(100);check(backend->profiles()[0].toMap()["country"]=="NOR" && backend->profile()["id"]==movedId,"confirmed country sort groups athletes and preserves selection");
    AthleteStudioController afterSort(nullptr,QFileInfo(backend->packDirectory()).dir().absolutePath());check(afterSort.profiles()[0].toMap()["country"]=="NOR","athlete country order persists");backend->undo();QTest::qWait(80);check(backend->profiles()[0].toMap()["id"]==movedId,"athlete country sorting can be undone");
    auto *name=item(view->rootObject(),"athleteName");name->forceActiveFocus();QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(view,"Aleksander zmieniony");
    auto *second=item(view->rootObject(),"athleteListRow-1");check(second,"second athlete row is visible");const auto point=second->mapToScene(QPointF(second->width()/2,second->height()/2)).toPoint();QTest::mouseClick(view,Qt::LeftButton,Qt::NoModifier,point);QTest::qWait(60);
    check(backend->selectedIndex()==1 && backend->profile()["name"]=="Test01","single click changes athlete without writing old input into the next profile");
    check(backend->profiles()[0].toMap()["name"]=="Aleksander zmieniony","pending name edit commits before changing athlete");backend->select(0);backend->setValue("name","Aleksander");
    auto *search=item(view->rootObject(),"athleteListSearch");search->forceActiveFocus();QTest::keyClicks(view,"Test35");QTest::qWait(40);check(rows->property("count").toInt()==1,"athlete list filters names");clickControl(view,"athleteListRow-35");check(backend->selectedIndex()==35,"filtered row selects original profile index");
    library->setProperty("query","NOR");QTest::qWait(30);check(rows->property("count").toInt()==1,"athlete list filters countries");clickControl(view,"athleteListRow-40");check(backend->selectedIndex()==40,"country result selects the correct athlete");library->setProperty("query","");QTest::qWait(50);
    rows->setProperty("contentY",350);QTest::qWait(30);const auto scroll=rows->property("contentY").toReal();backend->setValue("overall",88);QTest::qWait(50);check(qAbs(rows->property("contentY").toReal()-scroll)<1,"editing a profile preserves list scroll");
    clickControl(view,"athleteRemove");QTest::qWait(30);check(rows->property("count").toInt()==40 && backend->selectedIndex()==39,"deleting selected athlete synchronizes list selection");clickControl(view,"athleteUndo");QTest::qWait(30);check(rows->property("count").toInt()==41 && backend->selectedIndex()==40,"undo restores removed athlete and selection");rows->forceActiveFocus();QTest::keyClick(view,Qt::Key_Delete);QTest::qWait(30);check(rows->property("count").toInt()==40,"Delete removes the selected athlete directly from the library");backend->undo();QTest::qWait(30);
    rows->forceActiveFocus();QTest::keyClick(view,Qt::Key_Home);QTest::qWait(30);check(backend->selectedIndex()==0,"Home selects first athlete");QTest::keyClick(view,Qt::Key_Down);QTest::qWait(30);check(backend->selectedIndex()==1,"arrow keys select neighboring athletes");
    for(int n=40;n>0;--n){backend->select(n);backend->remove();}backend->select(0);
    const auto originalPack=backend->packIndex();library->setProperty("query","missing");backend->createPack();QTest::qWait(40);check(library->property("query").toString().isEmpty() && rows->property("count").toInt()==0,"changing pack clears stale filter and list");backend->selectPack(originalPack);QTest::qWait(60);check(rows->property("count").toInt()==1 && backend->selectedIndex()==0,"returning to original pack restores its list");
    qInfo()<<"PASS: athlete side list, pending edits, filtered selection, keyboard navigation, preserved scroll, deletion, undo and pack switching";
}

static void athleteStudioRegressions(MainWindow &main,bool patchesOnly=false){
    QStandardPaths::setTestModeEnabled(true);
    QTemporaryDir data;check(data.isValid(),"athlete studio isolated data");
    {
        AthleteStudioController imported(nullptr,data.path()+"/men-2026");
        const auto file=QFINDTESTDATA("../data/packs/men-2026/pack.json");check(!file.isEmpty() && imported.importFrom(file),"2026 men pack imports through the real editor");
        const auto profiles=imported.profiles();check(profiles.size()==98,"union of ranked men contains 98 distinct profiles");
        QString country;QSet<QString> ids;
        for(auto row:profiles){const auto p=row.toMap();check(p["country"].toString()>=country,"new pack is sorted by country");country=p["country"].toString();check(!ids.contains(p["id"].toString()),"new pack profile identities are unique");ids.insert(p["id"].toString());check(p["overall"].toInt()==70 && p["form"].toInt()==70 && p["landing"].toInt()==2 && p["perks"].toList()==QVariantList({0,0,0}),"new pack skills are editable model defaults");check(p["photoPath"].toString().startsWith(country+"_") && p["photoPath"].toString().endsWith(".png"),"new pack photo references use country and name");}
    }
    auto *database=GlobalDatabase::get();const auto original=database->packsSnapshot();
    AthleteProfile decoded;QString error;auto fresh=AthleteProfile::create();
    check(AthleteProfile::fromJson(fresh.toJson(),decoded,error),"new profile round trip");
    auto invalid=fresh.toJson();invalid["overall"]=0;check(!AthleteProfile::fromJson(invalid,decoded,error),"OVR zero rejected");invalid=fresh.toJson();invalid["portrait"]="../escape.png";check(!AthleteProfile::fromJson(invalid,decoded,error),"portrait traversal rejected");invalid=fresh.toJson();invalid["perks"]=QJsonArray{1,2,5};check(!AthleteProfile::fromJson(invalid,decoded,error),"invalid perk tier rejected");
    invalid["perks"]=QJsonArray{4,-4,0};check(AthleteProfile::fromJson(invalid,decoded,error) && decoded.perks[0]==4 && decoded.perks[1]==-4,"both extreme fourth-tier perk sides survive serialization");auto oldLevels=fresh.toJson();oldLevels.remove("perkLevels");check(!AthleteProfile::fromJson(oldLevels,decoded,error),"three-level data is not silently reinterpreted as four levels");
    AthleteStudioController controller(nullptr,data.path()+"/library");
    controller.add();
    QSignalSpy broadUpdates(&controller,&AthleteStudioController::changed),perkUpdates(&controller,&AthleteStudioController::perksChanged),historyUpdates(&controller,&AthleteStudioController::historyChanged);
    controller.setPerk(0,1,3,false);
    check(broadUpdates.isEmpty() && perkUpdates.count()==1 && historyUpdates.count()==1 && controller.selectedPerks()[0].toInt()==3,"perk edits notify only perks and history without rebuilding the library");
    controller.remove();
    check(controller.profiles().isEmpty(),"empty studio leaves game data alone");controller.add();controller.setValue("name","Ada");controller.setValue("surname","Test");controller.setValue("overall",93);controller.setValue("form",81);controller.setValue("landing",4);controller.setValue("personalBest",215.5);controller.setPerk(0,1,2,false);controller.setPerk(0,-1,1,false);
    check(controller.profile()["perks"].toList()[0].toInt()==-1,"opposite replaces selected perk");controller.undo();check(controller.profile()["perks"].toList()[0].toInt()==2,"undo restores advanced opposite");controller.redo();check(controller.profile()["perks"].toList()[0].toInt()==-1,"redo restores replacement");controller.setPerk(0,-1,1,true);check(controller.profile()["perks"].toList()[0].toInt()==0,"clicking selected perk clears it");
    controller.setValue("overall",101);check(controller.profile()["overall"].toInt()==93,"invalid rating does not replace saved value");
    const QString relativePhoto="zdjecia/photo with spaces.png";const auto photo=controller.packDirectory()+"/"+relativePhoto;check(QDir().mkpath(QFileInfo(photo).absolutePath()),"photo folder belongs to pack");QImage image(120,160,QImage::Format_RGB32);image.fill(Qt::darkCyan);check(image.save(photo),"photo fixture saved");check(controller.setPhoto(photo),"photo path assigned from within pack");
    check(controller.profile()["photoPath"].toString()==relativePhoto && controller.profile()["portrait"].toString().isEmpty() && QFileInfo(QUrl(controller.profile()["photo"].toString()).toLocalFile()).exists(),"photo is a pack-relative reference without another copy");
    check(!controller.setPhoto(data.path()+"/outside.png") && !controller.setPhoto("../escape.png") && controller.profile()["photoPath"].toString()==relativePhoto,"photo references cannot leave pack folder");
    check(controller.exportTo(data.path()+"/exported"),"profile package exported with referenced assets");check(!controller.exportTo(data.path()+"/exported"),"export refuses overwrite");QFile::remove(photo);
    AthleteStudioController reopened(nullptr,data.path()+"/library");check(reopened.profile()["overall"].toInt()==93 && reopened.profile()["photoPath"].toString()==relativePhoto && reopened.profile()["photo"].toString().isEmpty(),"missing relative photo preserves path and profile");
    check(reopened.importFrom(data.path()+"/exported/pack.json"),"profile package imported");check(reopened.packs().size()==2 && reopened.profile()["photoPath"].toString()==relativePhoto && reopened.profile()["photoAvailable"].toBool(),"pack import resolves copied assets inside its own folder");
    check(reopened.setPhoto("zdjecia/later.png"),"missing in-pack photo can be referenced before it exists");QSignalSpy photoUpdates(&reopened,&AthleteStudioController::changed);check(image.save(reopened.packDirectory()+"/zdjecia/later.png"),"in-pack image appears later");QTest::qWait(1150);check(photoUpdates.count()>0 && reopened.profile()["photoAvailable"].toBool(),"photo availability refreshes when referenced file appears");

    auto oldProfile=fresh.toJson();oldProfile.remove("photoPath");check(AthleteProfile::fromJson(oldProfile,decoded,error),"profiles from before path-only photos remain readable");
    const QString oldPhoto="portraits/"+QString(64,'a')+".png";const auto oldFolder=data.path()+"/old-format";check(QDir().mkpath(oldFolder+"/portraits") && image.save(oldFolder+"/"+oldPhoto),"legacy owned-photo fixture");oldProfile["portrait"]=oldPhoto;
    QJsonObject oldPack{{"format","sj-sim-athlete-pack"},{"version",1},{"id",QUuid::createUuid().toString(QUuid::WithoutBraces)},{"name","Old photo pack"},{"profiles",QJsonArray{oldProfile}}};QFile oldManifest(oldFolder+"/pack.json");check(oldManifest.open(QIODevice::WriteOnly),"legacy photo manifest opened");oldManifest.write(QJsonDocument(oldPack).toJson());oldManifest.close();check(reopened.importFrom(oldManifest.fileName()) && QFileInfo(QUrl(reopened.profile()["photo"].toString()).toLocalFile()).exists(),"older owned-photo packages still import with their images");

    reopened.duplicate();check(reopened.profiles().size()==2,"duplicate creates independent profile");reopened.remove();reopened.undo();check(reopened.profiles().size()==2,"delete can be undone");
    DataPack legacy;legacy.name="Studio legacy fixture";legacy.kind=DataPack::Jumpers;Jumper old("Jan","Kopia","POL");auto skills=old.getJumperSkills();skills.setTakeoffTechnique(80);skills.setFlightTechnique(60);skills.setForm(65);skills.setLandingStyle(15);old.setJumperSkills(skills);old.setPersonalBest(201.5);old.setBirthYear(2000);legacy.jumpers.append(old);const auto id=database->addPack(legacy);
    const auto before=database->packsSnapshot();int source=-1;for(int i=0;i<controller.legacyPacks().size();++i)if(controller.legacyPacks()[i].toMap()["id"].toString()==id)source=i;check(source>=0,"legacy source available for optional copy");controller.copyLegacy(source,0);const auto copy=controller.profile();check(copy["name"]=="Jan" && copy["birthYear"].toInt()==2000 && copy["landing"].toInt()==3 && copy["perks"].toList()==QVariantList({0,0,0}),"copy keeps identity and approximate landing but starts perks from zero");check(database->packsSnapshot()==before,"studio does not change legacy database");QString restoreError;check(database->restorePacks(original,restoreError),"legacy fixture restored");
    QFile blocked(data.path()+"/blocked");check(blocked.open(QIODevice::WriteOnly),"blocked storage fixture");blocked.close();AthleteStudioController denied(nullptr,blocked.fileName());denied.add();check(denied.profiles().isEmpty() && !denied.message().isEmpty(),"failed write preserves current model and reports failure");
    const QString previousApp=QCoreApplication::applicationName();QCoreApplication::setApplicationName("SjSimAthleteTest-"+QUuid::createUuid().toString(QUuid::WithoutBraces));const auto isolatedStorage=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    main.show();main.resize(1440,1000);DatabaseEditorWindow window(&main);auto *backend=new AthleteStudioController(&window,data.path()+"/ui");window.setProperty("athleteStudio",QVariant::fromValue(static_cast<QObject *>(backend)));window.show();QTest::qWait(200);check(backend,"database provides isolated athlete controller");QTest::qWait(80);check(item(scene(&window)->rootObject(),"athleteStudio"),"athlete editor opens directly in the database section");check(!item(scene(&window)->rootObject(),"athleteNewMode") && !item(scene(&window)->rootObject(),"athleteClassicMode"),"database has no old/new editor switch");clickControl(scene(&window),"manageStudioPack");auto *packName=item(scene(&window)->quickWindow()->contentItem(),"studioPackName");check(packName,"athlete pack settings show the name");packName->setProperty("text","Athlete pack renamed");clickControl(scene(&window),"saveStudioPackName");check(backend->packName()=="Athlete pack renamed","athlete pack settings save the selected pack name");clickControl(scene(&window),"closeStudioPack");QTest::qWait(200);clickControl(scene(&window),"athleteAdd");backend->setValue("name","Aleksander");backend->setValue("surname","Nowak");backend->setValue("overall",87);backend->setValue("form",76);backend->setValue("personalBest",218.5);backend->setPerk(1,1,2,false);backend->setPerk(2,-1,1,false);QTest::qWait(80);
    if(!patchesOnly)athleteLibraryRegressions(scene(&window),backend);
    for(int view=0;view<5;++view){
        if(patchesOnly && view!=1)continue;
        qApp->setProperty("athleteTestStage",QString("main %1").arg(view));
        auto *editor=item(scene(&window)->rootObject(),"athleteEditor");check(editor,"athlete editor loaded");editor->setProperty("viewMode",view);main.resize(1440,900);QTest::qWait(80);
        auto *name=item(scene(&window)->rootObject(),"athleteName");auto *rating=item(scene(&window)->rootObject(),"athleteOverall");check(name && rating && name->mapToScene(QPointF()).y()<rating->mapToScene(QPointF()).y(),"identity precedes ratings in every concept");
        backend->setPerk(0,1,1,false);backend->setPerk(0,1,1,true);QTest::qWait(30);
        clickControl(scene(&window),"athletePerk-training");check(backend->profile()["perks"].toList()[0].toInt()==1,"badge equips perk in every concept");
        clickControl(scene(&window),"athletePerk-training-3");check(backend->profile()["perks"].toList()[0].toInt()==3,"new intermediate third tier can be selected");clickControl(scene(&window),"athletePerk-training-4");check(backend->profile()["perks"].toList()[0].toInt()==4,"extreme fourth tier can be selected in every concept");
        if(view==1){
            auto *appearance=scene(&window)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();const bool previousMotion=appearance->reducedMotion();
            appearance->setReducedMotion(true);QTest::qWait(20);auto *badge=item(scene(&window)->rootObject(),"athletePerk-training")->parentItem();
            check(badge->property("burstProgress").toReal()==1 && badge->property("perkPulse").toReal()==1,"reduced motion stops decorative perk effects immediately");appearance->setReducedMotion(previousMotion);
        }
        check(item(scene(&window)->rootObject(),"athletePerk-training-4")->property("checked").toBool(),"selected extreme fourth tier is visible");
        clickControl(scene(&window),"athletePerk-training-1");check(backend->profile()["perks"].toList()[0].toInt()==1,"ordinary tier can be selected directly");
        clickControl(scene(&window),"athletePerk-training-2");
        if(view==1){clickControl(scene(&window),"athleteAssigned-0");check(backend->profile()["perks"].toList()[0].toInt()==0,"clicking assigned patch removes it");backend->undo();check(backend->profile()["perks"].toList()[0].toInt()==2,"removing an assigned patch is one undoable action");clickControl(scene(&window),"athletePerk-training");check(backend->profile()["perks"].toList()[0].toInt()==0,"clicking selected patch also removes it");clickControl(scene(&window),"athletePerk-training");check(backend->profile()["perks"].toList()[0].toInt()==1,"new patch starts at ordinary level");clickControl(scene(&window),"athletePerk-training-2");check(backend->profile()["perks"].toList()[0].toInt()==2,"direct strength action restores level II");}
        clickControl(scene(&window),"athletePerk-reserve");check(backend->profile()["perks"].toList()[0].toInt()==-1,"opposite badge replaces the equipped perk in every concept");
        auto *badge=item(scene(&window)->rootObject(),"athletePerk-steady");badge->forceActiveFocus();QTest::keyClick(scene(&window),Qt::Key_F1);QTest::qWait(30);check(badge->property("helpRequested").toBool(),"perk help is available on explicit keyboard request");QTest::keyClick(scene(&window),Qt::Key_Tab);QTest::qWait(30);
        const QStringList descriptions{QStringLiteral("Lepsze treningi i serie próbne. Słabsze konkursy."),QStringLiteral("Mniejszy rozrzut jakości skoków.")};for(auto *label:scene(&window)->rootObject()->findChildren<QQuickItem *>())if(label->isVisible())check(!descriptions.contains(label->property("text").toString()),"perk descriptions stay out of the visible layout");
        backend->setPerk(0,-1,1,true);backend->setValue("overall",100);QTest::qWait(30);check(rating->property("contentWidth").toReal()<=rating->width()+1,QString("three-digit OVR fits in concept %1 (content %2, width %3)").arg(view).arg(rating->property("contentWidth").toReal()).arg(rating->width()));backend->setValue("overall",87);render(&window,QString("athlete-studio-view-%1").arg(view),true);
    }
    {
        auto *quick=scene(&window);auto *testEditor=item(quick->rootObject(),"athleteEditor");testEditor->setProperty("viewMode",1);
        auto *appearance=quick->rootContext()->contextProperty("appearance").value<QuickAppearance *>();
        const qreal oldText=appearance->textScale(),oldUi=appearance->uiScale();const bool oldMotion=appearance->reducedMotion();appearance->setReducedMotion(true);
        const QStringList keys{"training","reserve","flight","small","steady","volatile"};
        for(qreal uiScale:{.7,1.})for(qreal textScale:{1.,1.5}) {
            appearance->setUiScale(uiScale);appearance->setTextScale(textScale);
            for(int width=640;width<=1500;width+=20) {
                main.resize(width,1000);QTest::qWait(8);quick->grabFramebuffer();
                for(const auto &key:keys) {
                    auto *pick=item(quick->rootObject(),"athletePerk-"+key);check(pick,"responsive perk card exists");auto *card=pick->parentItem();
                    for(int level=1;level<=4;++level) {auto *button=item(quick->rootObject(),"athletePerk-"+key+"-"+QString::number(level));if(button && !card->boundingRect().adjusted(-1,-1,1,1).contains(button->mapRectToItem(card,button->boundingRect()))){qWarning()<<"PERK GEOMETRY"<<card->boundingRect()<<button->mapRectToItem(card,button->boundingRect())<<button->parentItem()->boundingRect()<<button->parentItem()->property("columns")<<button->parentItem()->property("buttonWidth");quick->grabFramebuffer().save(output+"/perk-layout-failure.png");}check(button && card->boundingRect().adjusted(-1,-1,1,1).contains(button->mapRectToItem(card,button->boundingRect())),QString("perk strength fits its card at width %1, UI %2, text %3: %4/%5").arg(width).arg(uiScale).arg(textScale).arg(key).arg(level));}
                }
            }
        }
        appearance->setUiScale(oldUi);appearance->setTextScale(oldText);main.resize(1440,1000);QTest::qWait(40);
        const int oldPerk=backend->selectedPerks().value(0).toInt();QVector<QColor> fills;
        for(int level:{2,3,4}) {backend->setPerk(0,1,level,false);QTest::qWait(20);auto *card=item(quick->rootObject(),"athletePerk-training")->parentItem();auto *background=card->childItems().first();fills.append(background->property("color").value<QColor>());presentation(&window)->grab().save(output+QString("/athlete-perk-strength-%1.png").arg(level));}
        check(fills[0]!=fills[1] && fills[1]!=fills[2],"perk levels two, three and extreme have distinct card fills");
        backend->setPerk(0,1,4,true);if(oldPerk)backend->setPerk(0,oldPerk>0?1:-1,std::abs(oldPerk),false);
        appearance->setReducedMotion(oldMotion);
    }
    auto *editor=item(scene(&window)->rootObject(),"athleteEditor");editor->setProperty("viewMode",patchesOnly?1:2);QTest::qWait(80);main.resize(1440,1000);QTest::qWait(80);auto *scroll=item(scene(&window)->rootObject(),"athleteEditorScroll");check(scroll,"editor has bounded scrolling");auto *flick=scroll->property("contentItem").value<QQuickItem *>();if(flick)flick->setProperty("contentY",0);QTest::qWait(60);auto *ovr=item(scene(&window)->rootObject(),"athleteOverall");check(ovr,"rating input exists");ovr->forceActiveFocus();QTest::keyClick(scene(&window),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&window),"96");QTest::keyClick(scene(&window),Qt::Key_Tab);QTest::qWait(80);check(backend->profile()["overall"].toInt()==96,"real OVR text input commits on focus change");
    if(flick){flick->setProperty("contentY",0);QTest::qWait(50);}clickControl(scene(&window),"athletePerk-training");check(backend->profile()["perks"].toList()[0].toInt()==1,"real badge chooses ordinary tier");clickControl(scene(&window),"athletePerk-training-2");check(backend->profile()["perks"].toList()[0].toInt()==2,"real level II button strengthens selected perk");clickControl(scene(&window),"athletePerk-reserve");check(backend->profile()["perks"].toList()[0].toInt()==-1,"real opposite badge replaces advanced perk with ordinary tier");
    const auto originalOverall=backend->profile()["overall"].toInt();clickControl(scene(&window),"athleteOverallIncrease");check(backend->profile()["overall"].toInt()==originalOverall+1,"small plus changes OVR immediately");clickControl(scene(&window),"athleteOverallDecrease");check(backend->profile()["overall"].toInt()==originalOverall,"small minus changes OVR immediately");const int originalForm=backend->profile()["form"].toInt();clickControl(scene(&window),"athleteFormIncrease");check(backend->profile()["form"].toInt()==originalForm+1,"small plus changes form");clickControl(scene(&window),"athleteFormDecrease");check(backend->profile()["form"].toInt()==originalForm,"small minus changes form");
    const auto repeatPoint=controlPoint(scene(&window),"athleteOverallDecrease");QTest::mousePress(scene(&window),Qt::LeftButton,Qt::NoModifier,repeatPoint);QTest::qWait(390);QTest::mouseRelease(scene(&window),Qt::LeftButton,Qt::NoModifier,repeatPoint);check(backend->profile()["overall"].toInt()<originalOverall-1,"holding small minus repeats without click cooldown");backend->setValue("overall",originalOverall);
    for(bool form:{false,true}) {
        const QString field=form?"form":"overall";const int original=backend->profile()[field].toInt();
        auto *slider=item(scene(&window)->rootObject(),form?"athleteFormSlider":"athleteOverallSlider");auto *number=item(scene(&window)->rootObject(),form?"athleteForm":"athleteOverall");check(slider && number,"rating slider and numeric preview exist");check(QTest::qWaitFor([&]{return qRound(slider->property("value").toDouble())==original && number->property("text").toString().toInt()==original;},1000),"programmatic rating setup has reached the controls before starting the drag");
        const auto begin=slider->mapToScene(QPointF(slider->width()*(original-1)/99.,slider->height()/2)).toPoint();const auto end=slider->mapToScene(QPointF(slider->width()*.35,slider->height()/2)).toPoint();
        QTest::mousePress(scene(&window),Qt::LeftButton,Qt::NoModifier,begin);QMouseEvent move(QEvent::MouseMove,end,scene(&window)->mapToGlobal(end),Qt::NoButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(scene(&window),&move);QTest::qWait(30);
        const int preview=qRound(slider->property("value").toDouble());check(slider->property("pressed").toBool() && preview!=original && number->property("text").toString().toInt()==preview,"drag previews the changing rating after earlier typing and step-button edits");
        check(backend->profile()[field].toInt()==original,"dragging previews without filling persistent undo history");QTest::mouseRelease(scene(&window),Qt::LeftButton,Qt::NoModifier,end);QTest::qWait(30);check(backend->profile()[field].toInt()==preview,"releasing rating slider commits the preview");backend->undo();QTest::qWait(30);check(backend->profile()[field].toInt()==original,"one undo restores the whole slider gesture");
    }
    QTest::qWait(400);auto *recordFeedback=scene(&window)->rootObject()->findChild<QObject *>("athleteRecordFeedback");check(recordFeedback,"record transition has an observable controller");const auto beforeEditor=item(scene(&window)->rootObject(),"athleteEditor");const int oldLanding=backend->profile()["landing"].toInt();clickControl(scene(&window),"athleteLanding-"+QString::number((oldLanding+1)%5));QTest::qWait(20);check(item(scene(&window)->rootObject(),"athleteEditor")==beforeEditor && recordFeedback->property("progress").toDouble()==1,"landing selection neither recreates nor fades the whole editor");backend->undo();
    const auto uiPhoto=backend->packDirectory()+"/ui-photo.png";QImage portraitImage(150,200,QImage::Format_RGB32);portraitImage.fill(QColor("#497b9b"));check(portraitImage.save(uiPhoto),"in-pack UI photo fixture saved");
    editor->setProperty("viewMode",1);QTest::qWait(50);main.resize(1440,1000);QTest::qWait(50);auto *photoField=item(scene(&window)->rootObject(),"athletePhotoPath");check(photoField,"relative photo path input exists");photoField->forceActiveFocus();QTest::keyClick(scene(&window),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&window),"ui-photo.png");QTest::keyClick(scene(&window),Qt::Key_Tab);QTest::qWait(50);check(backend->profile()["photoPath"]=="ui-photo.png" && backend->profile()["photoAvailable"].toBool(),"typed relative path previews image from the pack folder");

    photoField->setProperty("dirty",true);photoField->setProperty("text","old-uncommitted.png");
    const int originalSelection=backend->selectedIndex();backend->add();QTest::qWait(30);
    check(photoField->property("text").toString().isEmpty() && !photoField->property("dirty").toBool(),"switching jumper clears the previous photo draft");backend->select(originalSelection);QTest::qWait(30);
    for(int view=0;view<5;++view){if(patchesOnly && view!=1)continue;qApp->setProperty("athleteTestStage",QString("photo %1").arg(view));editor->setProperty("viewMode",view);QTest::qWait(50);render(&window,QString("athlete-concept-photo-%1").arg(view));}backend->clearPhoto();
    qApp->setProperty("athleteTestStage","large text");editor->setProperty("viewMode",patchesOnly?1:2);QTest::qWait(50);
    auto *appearance=scene(&window)->rootContext()->contextProperty("appearance").value<QuickAppearance *>();appearance->setTextScale(1.5);render(&window,"athlete-studio-large-text");appearance->setTextScale(1);
    qApp->setProperty("athleteTestStage","copy screen");auto *studio=item(scene(&window)->rootObject(),"athleteStudio");studio->setProperty("copying",true);main.resize(360,480);QTest::qWait(100);clickControl(scene(&window),"athleteCopyClose");check(!studio->property("copying").toBool(),"copy workflow stays navigable on the smallest screen");
    check(item(scene(&window)->rootObject(),"athleteStudio"),"athlete studio remains the only editor in its database section");window.hide();QDir(isolatedStorage).removeRecursively();QCoreApplication::setApplicationName(previousApp);qInfo()<<"PASS: separate athlete model, safe copies, perk opposites/tiers, undo, persistence, pack-relative photos, legacy photo compatibility and responsive athlete concepts";
}


static void athleteFieldCommitRegressions(MainWindow &main) {
    QTemporaryDir storage;check(storage.isValid(),"private athlete input persistence fixture");
    QJsonArray initial;for(int i=0;i<200;++i){auto p=AthleteProfile::create();p.name="Fixture";p.surname=QString::number(i);initial.append(p.toJson());}
    const QString packId=QUuid::createUuid().toString(QUuid::WithoutBraces);const QString folder=storage.path()+"/athletes/"+packId;check(QDir().mkpath(folder),"large private pack folder");QFile seed(folder+"/pack.json");check(seed.open(QIODevice::WriteOnly),"large private pack fixture");seed.write(QJsonDocument(QJsonObject{{"format","sj-sim-athlete-pack"},{"version",1},{"id",packId},{"name","Input regression"},{"profiles",initial}}).toJson());seed.close();
    main.findChild<QuickUi *>()->setLanguage(0);main.show();main.resize(1200,900);QTest::qWait(50);
    DatabaseEditorWindow window(&main);auto *backend=new AthleteStudioController(&window,storage.path()+"/athletes");window.setProperty("athleteStudio",QVariant::fromValue(static_cast<QObject *>(backend)));window.show();QTest::qWait(120);
    auto *quick=scene(&window);
    auto enter=[&](const QString &field,const QString &value) {clickControl(quick,field);QTest::keyClick(quick,Qt::Key_A,Qt::ControlModifier);if(value.isEmpty())QTest::keyClick(quick,Qt::Key_Backspace);else QTest::keyClicks(quick,value);QTest::keyClick(quick->window()->windowHandle(),Qt::Key_Return);QTest::qWait(50);};
    for(int attempt=0;attempt<6;++attempt) {
        clickControl(quick,"athleteAdd");const auto athleteId=backend->profile()["id"].toString();
        enter("athletePersonalBest","212,5");check(backend->profile()["personalBest"].toDouble()==212.5,"Enter commits a comma-decimal personal best on a newly created athlete");
        enter("athleteName","Nowy");enter("athleteSurname","Test");
        enter("athleteOverall","93");enter("athleteForm","64");
        QFile manifest(backend->packDirectory()+"/pack.json");check(manifest.open(QIODevice::ReadOnly),"pack is durably present after Enter");const auto profiles=QJsonDocument::fromJson(manifest.readAll()).object()["profiles"].toArray();bool found=false;
        for(auto profile:profiles)if(profile.toObject()["id"].toString()==athleteId) {const auto p=profile.toObject();found=p["personalBest"].toDouble()==212.5 && p["name"]=="Nowy" && p["surname"]=="Test" && p["overall"]==93 && p["form"]==64;}
        check(found,"personal best and preceding edits are saved before normal application shutdown");
    }
    auto checkFieldLifecycle=[&](QObject *editor,const QString &key,double value) {
        QQmlContext context(quick->rootContext());context.setContextProperty("commitBackend",editor);context.setContextProperty("commitKey",key);context.setContextProperty("commitNumber",value);
        QQmlComponent component(quick->engine());component.setData(R"(import QtQuick 2.15
import QtQuick.Controls 2.15
TextField { width: 240; height: 40; z: 10000; property bool committing: false; property bool refreshedDuringCommit: false; readonly property var observedProfile: commitBackend.profile; onObservedProfileChanged: if(committing)refreshedDuringCommit=true; text: "213,5"; onEditingFinished: { committing=true; commitBackend.setValue(commitKey,commitNumber); committing=false } })",QUrl("qrc:/ui/FieldCommitLifecycle.qml"));
        QPointer<QQuickItem> field(qobject_cast<QQuickItem *>(component.create(&context)));check(field,"field lifecycle fixture loads");field->setParentItem(quick->rootObject());field->forceActiveFocus();QTest::qWait(20);
        QTest::keyClick(quick->window()->windowHandle(),Qt::Key_Return);QTest::qWait(30);
        check(!field->property("refreshedDuringCommit").toBool(),"profile bindings do not rebuild the editor inside its editingFinished callback");
        check(field->property("observedProfile").toMap()[key].toDouble()==value,"profile binding publishes the saved value after the input event");delete field.data();
    };
    checkFieldLifecycle(backend,"personalBest",213.5);
    HillStudioController hillEditor(nullptr,storage.path()+"/hill-lifecycle");hillEditor.add();checkFieldLifecycle(&hillEditor,"record",137.5);
    SimulationProfileDraft athleteDraft(DataPack::Jumpers);Jumper draftJumper;QString draftError;check(draftJumper.setSimulationProfile(AthleteProfile::create(),draftError),"athlete draft lifecycle fixture");athleteDraft.load(draftJumper,{});checkFieldLifecycle(&athleteDraft,"personalBest",213.5);
    SimulationProfileDraft hillDraft(DataPack::Hills);Hill draftHill;check(draftHill.setSimulationProfile(HillProfile::create(),draftError),"hill draft lifecycle fixture");hillDraft.load(draftHill);checkFieldLifecycle(&hillDraft,"record",137.5);
    QSignalSpy domainUpdates(&athleteDraft,&SimulationProfileDraft::changed);athleteDraft.setValue("personalBest",214.5);check(domainUpdates.count()==1 && athleteDraft.configuredJumper().getSimulationProfile()->personalBest==214.5,"domain updates remain synchronous while only QML publication is deferred");
    enter("athletePersonalBest","");check(backend->profile()["personalBest"].toDouble()==0,"clearing personal best commits an absent record");
    const auto closedRoot=storage.path()+"/closed-immediately";
    auto *closing=new AthleteStudioController(nullptr,closedRoot);closing->add();closing->setValue("personalBest",212.5);delete closing;
    QCoreApplication::processEvents();AthleteStudioController reopened(nullptr,closedRoot);check(reopened.profile()["personalBest"].toDouble()==212.5,"deleting the controller before queued view refresh does not lose the already-saved field");
    qInfo()<<"PASS: repeated athlete creation, comma-decimal personal best, Enter commits and durable field persistence";
}
