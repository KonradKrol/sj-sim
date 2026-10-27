#pragma once
#include <QJsonObject>
#include <QString>
#include <array>

// A separate authoring model. Neither Jumper nor the simulation reads it.
struct AthleteProfile {
    QString id, name, surname, country="POL", portrait, photoPath, sourceId;
    int overall=70, form=70, landing=2, birthYear=0;
    double personalBest=0;
    // 0: none, +1/+2/+3: first perk I/II/III, negative: opposite perk.
    std::array<int,3> perks{{0,0,0}};
    static AthleteProfile create();
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &json,AthleteProfile &profile,QString &error);
};
