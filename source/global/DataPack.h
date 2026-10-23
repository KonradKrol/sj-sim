#ifndef DATAPACK_H
#define DATAPACK_H
#include "../simulator/Jumper.h"
#include "../simulator/Hill.h"
#include "../competitions/CompetitionRules.h"
#include "PointsForPlacesPreset.h"
#include <QDate>
#include <QJsonObject>
#include <QUuid>

struct DataPack {
    enum Kind { Jumpers, Hills, Rules };
    QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString name;
    Kind kind = Jumpers;
    bool ageMode = false;
    int referenceYear = QDate::currentDate().year();
    QVector<Jumper> jumpers;
    QVector<Hill> hills;
    QVector<CompetitionRules> rules;
    QVector<PointsForPlacesPreset> points;
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &object, DataPack &pack, QString &error);
};
#endif
