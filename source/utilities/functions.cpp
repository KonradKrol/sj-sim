#include "functions.h"
#include <math.h>
#include <QFile>
#include <iostream>

double roundDoubleToHalf(double number)
{
    return (std::floor((number * 2) + 0.5) / 2);
}

double roundDoubleToOnePlace(double number)
{
    return static_cast<float>(static_cast<int>(number * 10.)) / 10.;
}

double roundDoubleToTwoPlaces(double number)
{
    return std::ceil(number * 100.0) / 100.0;
}

QVector<QWidget *> MyFunctions::getWidgetsVector(QWidget *parent, QString search)
{
    QRegularExpression exp(search);
    return parent->findChildren<QWidget *>(exp).toVector();
}

void MyFunctions::fileMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << '\n';
    ts.flush();
}
