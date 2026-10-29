#include "BalanceRuntime.h"
#include "BalanceLabModel.h"
#include <QStandardPaths>
#include <QSaveFile>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QFileInfo>

namespace {
QMutex lock;
QJsonObject cached;
QString loadedPath;
QString path(){const auto root=QCoreApplication::instance()->property("balanceLabStorageRoot").toString();return (root.isEmpty()?QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/balance-lab":root)+"/active.json";}
void load(){const auto filePath=path();if(loadedPath==filePath)return;loadedPath=filePath;cached={};QFile f(filePath);if(!f.open(QIODevice::ReadOnly))return;auto o=QJsonDocument::fromJson(f.readAll()).object();QString error;if(o["perkLevels"].toDouble()==4 && o["schema"].toInt()==1 && o["values"].isObject() && BalanceLab::validateProfile(o["values"].toObject(),error))cached=o;}
}
namespace BalanceRuntime {
QJsonObject capture(){QMutexLocker guard(&lock);load();return cached["values"].toObject();}
QString activeName(){QMutexLocker guard(&lock);load();return cached["name"].toString();}
bool activate(const QJsonObject &values,const QString &name,QString &error){
    if(!BalanceLab::validateProfile(values,error))return false;
    QMutexLocker guard(&lock);load();const auto p=path();if(!QDir().mkpath(QFileInfo(p).absolutePath())){error=QObject::tr("Nie można utworzyć folderu profilu balansu.");return false;}
    const QJsonObject next{{"perkLevels",4},{"schema",1},{"name",name},{"values",values}};const auto bytes=QJsonDocument(next).toJson();QSaveFile f(p);
    if(!f.open(QIODevice::WriteOnly) || f.write(bytes)!=bytes.size() || !f.commit()){error=QObject::tr("Nie udało się zapisać aktywnego profilu balansu.");return false;}cached=next;return true;
}
bool reset(QString &error){return activate({},QObject::tr("Standardowy balans"),error);}
}
