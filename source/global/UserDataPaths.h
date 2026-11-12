#pragma once
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

namespace UserDataPaths {
inline QString root() {
    if(auto *app=QCoreApplication::instance()) {
        const auto override=app->property("sjSimDataRoot").toString();
        if(!override.isEmpty())return QDir(override).absolutePath();
    }
    static const QString resolved=[] {
        if(QFileInfo::exists(QCoreApplication::applicationDirPath()+"/portable.txt"))return QCoreApplication::applicationDirPath()+"/SjSimData";
        const auto configured=QSettings(QSettings::defaultFormat(),QSettings::UserScope,"SJ-Sim","SJ-Sim").value("storage/dataDirectory").toString();
        if(!configured.isEmpty())return QDir(configured).absolutePath();
        return QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).filePath("SjSimData");
    }();
    return resolved;
}
inline QString path(const QString &relative=QString()) {return QDir(root()).filePath(relative);}
inline QString packs() {return path("Packs");}
inline QString saves() {return path("Saves");}
inline QString calendars() {return path("Calendars");}
bool initialize(QString &error);
bool finishRelocation(QString &error);
}
