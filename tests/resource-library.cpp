#include <QApplication>
#include <QTemporaryDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlError>
#include "../source/UI/mainwindow.h"
#include "../source/UI/Quick/QuickUi.h"
#include <QEventLoop>
#include <QTimer>
#include "../source/UI/Seasons/NewSimulationSaveConfigurationWindow.h"
#include <cstdlib>
#include <memory>
#include "../source/global/UserDataPaths.h"
#include "../source/global/ResourceBundle.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/simulation-v2/SimulationCatalog.h"

static void check(bool value,const QString &message){if(!value){qCritical().noquote()<<"FAIL:"<<message;std::exit(1);}}
static QByteArray read(const QString &path){QFile f(path);check(f.open(QIODevice::ReadOnly),path);return f.readAll();}
static void write(const QString &path,const QByteArray &bytes){QFile f(path);check(f.open(QIODevice::WriteOnly)&&f.write(bytes)==bytes.size(),path);}
int main(int argc,char **argv){
    QApplication app(argc,argv);QCoreApplication::setOrganizationName("SjSim-Library-Test");QCoreApplication::setApplicationName("Isolated");
    check(argc==2,"Pass the repository directory");QTemporaryDir test;check(test.isValid(),"temporary folder");
    app.setProperty("sjSimDataRoot",test.path()+"/source");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");
    QString error;check(UserDataPaths::initialize(error),error);
    auto catalog=SimulationV2::SimulationCatalog().load({},100,{});check(catalog.issues.isEmpty(),catalog.issues.join("; "));
    check(catalog.packs.size()==3,"exactly three starter packs");
    check(catalog.selected(DataPack::Jumpers)&&catalog.selected(DataPack::Jumpers)->data.jumpers.size()==83,"83 athletes");
    check(catalog.selected(DataPack::Hills)&&catalog.selected(DataPack::Hills)->data.hills.size()==128,"128 hills");
    auto *db=GlobalDatabase::get();check(db->loadFromJson(),"load library adapters");
    check(db->getGlobalJumpers().size()==83&&db->getGlobalHills().size()==128,"game sees defaults");
    check(db->getGlobalCalendarPresets().size()==6,"six calendar presets");
    check(!db->getGlobalPointsForPlacesPresets().isEmpty(),"independent point presets");
    NewSimulationSaveConfigurationWindow setup({},nullptr);QEventLoop wait;QTimer poll;
    QObject::connect(&poll,&QTimer::timeout,[&]{if(setup.catalogReady())wait.quit();});poll.start(20);QTimer::singleShot(15000,&wait,&QEventLoop::quit);wait.exec();
    check(setup.catalogReady(),"example season catalog ready");auto save=setup.createExampleSave();check(bool(save),"example season created");check(save->getActualSeason()->getDatesEnabled() && save->formEngine().enabled && save->getAutosaveEnabled(),"example enables dates, automatic form and autosave");
    for(auto *calendar:save->getActualSeason()->getCalendarsReference())for(auto *event:calendar->getCompetitionsReference())check(event->getScheduledDate().isValid(),"example preserves preset dates");check(save->saveToFile(UserDataPaths::saves()+"/"),"save uses new library folder");
    const auto saveFile=QDir(UserDataPaths::saves()).entryList({"*.json"},QDir::Files).value(0);check(!saveFile.isEmpty(),"save visible");
    const auto saveBundle=test.path()+"/season.sjsim-pack";check(ResourceBundle::exportSelection(saveBundle,{"Saves/"+saveFile},error),error);
    const auto all=ResourceBundle::entries();QStringList keys;QString athleteKey;int calendars=0;
    for(const auto &v:all){const auto item=v.toMap();if(item["kind"]=="Athletes"){athleteKey=item["key"].toString();keys<<athleteKey;}if(item["kind"]=="Calendars"){keys<<item["key"].toString();++calendars;}}
    check(calendars==6,"six visible calendars");const auto bundle=test.path()+"/mixed.sjsim-pack";
    check(ResourceBundle::exportSelection(bundle,keys,error),error);
    const QByteArray magic("SJSIM-PACK\n1\n");const auto bytes=read(bundle);auto object=QJsonDocument::fromJson(bytes.mid(magic.size())).object();auto resources=object["resources"].toArray();
    QSet<QString> kinds;int photoCount=0;for(const auto &v:resources){const auto r=v.toObject();kinds.insert(r["kind"].toString());photoCount+=r["files"].toArray().size();}
    check(kinds.contains("Hills")&&kinds.contains("Rules")&&kinds.contains("Points")&&kinds.contains("Athletes")&&photoCount>0,"calendar dependencies and athlete photos");
    app.setProperty("sjSimDataRoot",test.path()+"/target");check(UserDataPaths::initialize(error),error);QStringList installed;
    const auto original=read(UserDataPaths::path("Packs/"+athleteKey+"/pack.json"));
    check(ResourceBundle::importFile(bundle,installed,error),error);check(installed.size()==resources.size(),"all resources imported");
    check(read(UserDataPaths::path("Packs/"+athleteKey+"/pack.json"))==original,"existing pack preserved");
    auto imported=SimulationV2::SimulationCatalog().load({},100,{});check(imported.packs.size()==6&&imported.issues.isEmpty(),"colliding packs receive new identities");
    for(const auto &entry:installed)if(entry.startsWith("Athletes/")){
        const auto dir=UserDataPaths::path("Packs/"+entry);const auto doc=QJsonDocument::fromJson(read(dir+"/pack.json")).object();int found=0;
        for(const auto &v:doc["profiles"].toArray()){const auto path=v.toObject()["photoPath"].toString();if(!path.isEmpty()&&QFileInfo::exists(dir+"/"+path))++found;}check(found>0,"imported photos resolved relative to new pack");
    }
    check(ResourceBundle::importFile(saveBundle,installed,error),error);
    check(QFileInfo::exists(UserDataPaths::path("Saves/"+saveFile)),"season imported");
    std::unique_ptr<SimulationSave> loaded(SimulationSave::loadFromFile(UserDataPaths::path("Saves/"+saveFile)));
    check(bool(loaded)&&loaded->getActualSeason()->getCalendarsReference().size()==2,"imported season opens with both calendars");
    const int before=ResourceBundle::entries().size();
    const auto reject=[&](QJsonObject modified,const QString &label){const auto path=test.path()+"/bad.sjsim-pack";write(path,magic+QJsonDocument(modified).toJson(QJsonDocument::Compact));check(!ResourceBundle::importFile(path,installed,error),label);check(ResourceBundle::entries().size()==before&&installed.isEmpty(),"invalid bundle changes nothing: "+label);};
    {auto bad=object;auto a=resources;for(int i=0;i<a.size();++i){auto r=a[i].toObject();auto files=r["files"].toArray();if(files.isEmpty())continue;auto f=files[0].toObject();f["path"]="../escape.png";files[0]=f;r["files"]=files;a[i]=r;break;}bad["resources"]=a;reject(bad,"reject traversal");}
    {auto bad=object;auto a=resources;for(int i=0;i<a.size();++i){auto r=a[i].toObject();auto files=r["files"].toArray();if(files.isEmpty())continue;auto f=files[0].toObject();f["sha256"]="bad";files[0]=f;r["files"]=files;a[i]=r;break;}bad["resources"]=a;reject(bad,"reject corrupt photo");}
    {auto bad=object;auto a=resources;a.append(a[0]);bad["resources"]=a;reject(bad,"reject duplicate resources");}
    check(!QFileInfo::exists(UserDataPaths::path("GlobalDatabase")),"no legacy database created");
    const auto marker=read(UserDataPaths::path("library.json"));write(UserDataPaths::path("Settings/keep.txt"),"custom");check(UserDataPaths::initialize(error),error);check(read(UserDataPaths::path("Settings/keep.txt"))=="custom"&&read(UserDataPaths::path("library.json"))==marker,"initialization preserves edits");
    // Settings are redirected too: no real user preferences are modified.
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,test.path()+"/preferences");
    const auto destination=test.path()+"/relocated";QDir().mkpath(destination);app.setProperty("pendingDataDirectory",destination);write(UserDataPaths::path("Settings/late.txt"),"last edit");
    check(UserDataPaths::finishRelocation(error),error);check(read(destination+"/Settings/late.txt")=="last edit"&&read(UserDataPaths::path("Settings/late.txt"))=="last edit","relocation captures final edits and preserves originals");
    check(QSettings(QSettings::defaultFormat(),QSettings::UserScope,"SJ-Sim","SJ-Sim").value("storage/dataDirectory").toString()==destination,"new folder persisted only after copying");
    QSettings().setValue("onboarding/welcomeDismissed",true);
    {MainWindow main;new QuickUi(&main);main.resize(1000,720);main.show();
        check(QMetaObject::invokeMethod(&main,"showResourceLibrary",Qt::DirectConnection),"library navigation action");
        QEventLoop render;QTimer::singleShot(350,&render,&QEventLoop::quit);render.exec();bool visibleLibrary=false;
        for(auto *view:main.findChildren<QQuickWidget*>())if(view->rootObject()&&view->rootObject()->findChild<QObject*>("resourceLibraryView")) {visibleLibrary=view->isVisible();check(view->status()==QQuickWidget::Ready,"library QML ready");}
        check(visibleLibrary,"library opens as an actual Quick navigation page");
        main.resize(720,480);QTimer::singleShot(100,&render,&QEventLoop::quit);render.exec();
    }
    qInfo()<<"PASS: starter data, game adapters, calendar dependencies, photos, collisions, invalid imports, idempotence and relocation";
}
