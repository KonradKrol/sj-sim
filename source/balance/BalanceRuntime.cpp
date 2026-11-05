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
#include <QDateTime>
#include <QMessageBox>

namespace {
QMutex lock;
QJsonObject cached;
QString loadedPath,loadError,notifiedError;
QString path(){
    const auto root=QCoreApplication::instance()->property("balanceLabStorageRoot").toString();
    if(!root.isEmpty())return root+"/active.json";
    const auto portable=QCoreApplication::applicationDirPath()+"/userData/BalanceLab";
    if(QDir(portable).exists())return portable+"/active.json";
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/balance-lab/active.json";
}
void load(){
    const auto filePath=path();const QFileInfo info(filePath);
    const auto signature=filePath+":"+QString::number(info.lastModified().toMSecsSinceEpoch())+":"+QString::number(info.size());
    if(loadedPath==signature)return;
    loadedPath=signature;cached={};loadError.clear();
    if(!info.exists())return;
    QFile file(filePath);QJsonParseError parse;QString validation;
    if(!file.open(QIODevice::ReadOnly))loadError=QObject::tr("Nie można odczytać aktywnego profilu balansu: %1").arg(filePath);
    else {
        const auto doc=QJsonDocument::fromJson(file.readAll(),&parse);const auto object=doc.object();
        if(parse.error==QJsonParseError::NoError && doc.isObject() && object["perkLevels"].toDouble()==4 && object["schema"].toInt()==1 && object["values"].isObject() && BalanceLab::validateProfile(object["values"].toObject(),validation))cached=object;
        else loadError=QObject::tr("Aktywny profil balansu jest uszkodzony: %1").arg(filePath);
    }
}

}
namespace BalanceRuntime {
QJsonObject defaults(){static const QJsonObject values=[] {QFile f(":/data/beta/default-balance.json");return f.open(QIODevice::ReadOnly)?QJsonDocument::fromJson(f.readAll()).object():QJsonObject();}();return values;}
QJsonObject capture(){QMutexLocker guard(&lock);load();
    if(!loadError.isEmpty() && notifiedError!=loadedPath){
        notifiedError=loadedPath;const auto message=loadError;
        QMetaObject::invokeMethod(QCoreApplication::instance(),[message]{if(BalanceRuntime::lastError()!=message)return;QMessageBox::warning(nullptr,QObject::tr("Błąd profilu balansu"),message+"\n\n"+QObject::tr("Używany jest standardowy balans. Plik pozostał bez zmian. W laboratorium możesz przywrócić standardowy profil lub wczytać własny."));},Qt::QueuedConnection);
    }
    auto values=defaults();const auto overrides=cached["values"].toObject();for(auto i=overrides.begin();i!=overrides.end();++i)values[i.key()]=i.value();return values;}
QString activeName(){QMutexLocker guard(&lock);load();return loadError.isEmpty()?cached["name"].toString():QObject::tr("Błąd aktywnego profilu");}
QString lastError(){QMutexLocker guard(&lock);load();return loadError;}
bool activate(const QJsonObject &values,const QString &name,QString &error){
    if(!BalanceLab::validateProfile(values,error))return false;
    QMutexLocker guard(&lock);load();const auto p=path();if(!QDir().mkpath(QFileInfo(p).absolutePath())){error=QObject::tr("Nie można utworzyć folderu profilu balansu.");return false;}
    const QJsonObject next{{"perkLevels",4},{"schema",1},{"name",name},{"values",values}};const auto bytes=QJsonDocument(next).toJson();QSaveFile f(p);
    if(!f.open(QIODevice::WriteOnly) || f.write(bytes)!=bytes.size() || !f.commit()){error=QObject::tr("Nie udało się zapisać aktywnego profilu balansu.");return false;}cached=next;loadError.clear();notifiedError.clear();return true;
}
bool reset(QString &error){return activate({},QObject::tr("Standardowy balans"),error);}
}
