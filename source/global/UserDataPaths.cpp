#include "UserDataPaths.h"
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTemporaryDir>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
bool seed(const QString &source,const QString &destination,QString &error) {
    if(!QDir().mkpath(destination)){error=QObject::tr("Nie można utworzyć folderu danych: %1").arg(destination);return false;}
    for(const auto &entry:QDir(source).entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot|QDir::Hidden)) {
        if(entry.isSymLink()||entry.fileName()==".sj-sim.lock")continue;
        const auto target=QDir(destination).filePath(entry.fileName());
        if(entry.isDir()){if(!seed(entry.filePath(),target,error))return false;}
        else if(!QFileInfo::exists(target) && !QFile::copy(entry.filePath(),target)){
            error=QObject::tr("Nie można skopiować zasobu startowego: %1").arg(entry.fileName());return false;
        }
    }
    return true;
}
}
bool UserDataPaths::initialize(QString &error) {
    for(const auto &folder:{"Packs/Athletes","Packs/Hills","Packs/Rules","Packs/Points","Calendars","Saves","Settings","Tools/FormPresets","Tools/ImportancePresets","Tools/BalanceLab","Tools/FormEngine"})
        if(!QDir().mkpath(path(folder))){error=QObject::tr("Nie można zapisać danych w %1.").arg(root());return false;}
    if(QFileInfo::exists(path("library.json")))return true;
    QString defaults=QCoreApplication::applicationDirPath()+"/Defaults";
    if(auto *app=QCoreApplication::instance();app && !app->property("sjSimDefaultsRoot").toString().isEmpty())defaults=app->property("sjSimDefaultsRoot").toString();
    if(!QFileInfo::exists(defaults+"/Packs/catalog.json")){error=QObject::tr("Brakuje zasobów startowych w folderze Defaults aplikacji.");return false;}
    if(!seed(defaults,root(),error))return false;
    QSaveFile marker(path("library.json"));const auto bytes=QJsonDocument(QJsonObject{{"format","sj-sim-library"},{"version",1}}).toJson();
    if(!marker.open(QIODevice::WriteOnly)||marker.write(bytes)!=bytes.size()||!marker.commit()){error=QObject::tr("Nie można zakończyć przygotowania folderu danych.");return false;}
    return true;
}

bool UserDataPaths::finishRelocation(QString &error) {
    const auto target=QCoreApplication::instance()->property("pendingDataDirectory").toString();if(target.isEmpty())return true;
    if(!QDir(target).entryList(QDir::AllEntries|QDir::Hidden|QDir::NoDotAndDotDot).isEmpty()){error=QObject::tr("Nowy folder nie jest pusty. Dane pozostają w %1.").arg(root());return false;}
    QTemporaryDir staging(QFileInfo(target).absolutePath()+"/.sjsim-copy-XXXXXX");
    if(!staging.isValid()||!seed(root(),staging.path(),error))return false;
    if(!QDir().rmdir(target)){error=QObject::tr("Nie można przygotować nowego folderu danych.");return false;}
    if(!QDir().rename(staging.path(),target)){QDir().mkpath(target);error=QObject::tr("Nie udało się przenieść kopii danych. Poprzedni folder pozostał bez zmian.");return false;}
    staging.setAutoRemove(false);QSettings(QSettings::defaultFormat(),QSettings::UserScope,"SJ-Sim","SJ-Sim").setValue("storage/dataDirectory",target);return true;
}
