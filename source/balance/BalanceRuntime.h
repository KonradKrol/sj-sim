#pragma once
#include <QJsonObject>
#include <QString>
namespace BalanceRuntime {
QJsonObject defaults();
QJsonObject capture();
QJsonObject gameCapture();
QString activeName();
QString lastError();
bool activate(const QJsonObject &values,const QString &name,QString &error);
bool reset(QString &error);
}
