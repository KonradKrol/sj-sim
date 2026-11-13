#pragma once
#include <QJsonObject>
#include <QVariantList>
#include <QStringList>

namespace ResourceBundle {
QVariantList entries();
bool removeSelection(const QStringList &keys,QStringList &removed,QString &error);
bool exportSelection(const QString &file,const QStringList &keys,QString &error);
bool exportPack(const QString &file,const QJsonObject &pack,const QString &photoDirectory,QString &error);
bool importFile(const QString &file,QStringList &installed,QString &error);
}
