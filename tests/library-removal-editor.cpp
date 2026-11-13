#include <QApplication>
#include <QTemporaryDir>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QAbstractButton>
#include <QTimer>
#include <QUuid>
#include <QDebug>
#include <cstdlib>
#include "../source/global/UserDataPaths.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/global/ResourceBundle.h"
#include "../source/UI/mainwindow.h"
#include "../source/UI/DatabaseEditor/DatabaseEditorWindow.h"
#include "../source/UI/Library/ResourceLibraryWindow.h"
static void check(bool value,const QString &text){if(!value){qCritical().noquote()<<"FAIL:"<<text;std::exit(1);}}
static QByteArray read(const QString &path){QFile f(path);check(f.open(QIODevice::ReadOnly),path);return f.readAll();}
static void write(const QString &path,const QByteArray &bytes){QFile f(path);check(f.open(QIODevice::WriteOnly),path);check(f.write(bytes)==bytes.size(),path);}
static void answer(QMessageBox::StandardButton response){QTimer::singleShot(100,[response]{for(auto *widget:QApplication::topLevelWidgets())if(auto *box=qobject_cast<QMessageBox *>(widget);box && box->isVisible()){check(box->button(response),"expected confirmation button");box->button(response)->click();return;}check(false,"expected confirmation dialog");});}
int main(int argc,char **argv){
    QTemporaryDir temp;QDir().mkpath(temp.path()+"/xdg");qputenv("XDG_DATA_HOME",(temp.path()+"/xdg").toUtf8());QApplication app(argc,argv);check(argc==2,"repository argument");
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temp.path()+"/settings");
    app.setProperty("sjSimDataRoot",temp.path()+"/library");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");QString error;check(UserDataPaths::initialize(error),error);
    auto *db=GlobalDatabase::get();check(db->loadFromJson(),"load defaults");
    const auto originalId=db->activePackId(DataPack::Jumpers);const auto folder=UserDataPaths::path("Packs/Athletes");const auto originalFile=folder+"/"+originalId+"/pack.json";const auto original=read(originalFile);
    const auto cloneId=QUuid::createUuid().toString(QUuid::WithoutBraces);auto clone=QJsonDocument::fromJson(original).object();clone["id"]=cloneId;clone["name"]="Delete me";QDir().mkpath(folder+"/"+cloneId);write(folder+"/"+cloneId+"/pack.json",QJsonDocument(clone).toJson());check(db->loadPacks(),"load second pack");
    const auto restoreBundle=temp.path()+"/restore.sjsim-pack";
    check(ResourceBundle::exportPack(restoreBundle,QJsonDocument::fromJson(original).object(),QString(argv[1])+"/data/defaults/Packs/Athletes/"+originalId,error),error);
    MainWindow main;
    {
        DatabaseEditorWindow editor(&main);auto altered=QJsonDocument::fromJson(original).object();altered["name"]="Unsaved rename";write(originalFile,QJsonDocument(altered).toJson());
        ResourceLibraryWindow library(&editor);library.toggle("Athletes/"+cloneId);answer(QMessageBox::Yes);library.removeSelected();
        check(!QFile::exists(folder+"/"+cloneId+"/pack.json") && library.selectedCount()==0,"delete works while the database editor is open");
        answer(QMessageBox::No);check(editor.close(),"discard closes editor");
        check(!QFile::exists(folder+"/"+cloneId+"/pack.json"),"discard never resurrects removed pack");check(read(originalFile)==original,"discard still restores unrelated unsaved changes");
    }
    {
        DatabaseEditorWindow editor(&main);ResourceLibraryWindow library(&editor);library.toggle("Athletes/"+originalId);library.toggle("Hills/"+db->activePackId(DataPack::Hills)+".json");library.toggle("Rules/"+db->activePackId(DataPack::Rules)+".json");answer(QMessageBox::Yes);library.removeSelected();
        check(!QFile::exists(originalFile) && db->getGlobalJumpers().isEmpty(),"last athlete pack can be removed");
        answer(QMessageBox::No);check(editor.close(),"discard with empty athlete library closes safely");check(!QFile::exists(originalFile),"last deleted pack remains deleted");check(db->loadPacks() && db->getGlobalJumpers().isEmpty() && db->getPacks().isEmpty(),"empty library reloads without fabricated packs");
    }
    {
        DatabaseEditorWindow editor(&main);ResourceLibraryWindow library(&editor);
        check(library.importFile(restoreBundle),library.message());check(db->getGlobalJumpers().size()==83,"pack can be imported into the empty library");
        answer(QMessageBox::No);check(editor.close(),"discard after import into an empty category closes safely");
        check(db->loadPacks() && db->getGlobalJumpers().size()==83,"discard never removes a newly imported pack from an empty category");
    }
    qInfo()<<"PASS deletion with open editor, independent discard, last-pack removal and empty-library reload";
}
