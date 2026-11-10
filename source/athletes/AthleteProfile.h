#pragma once
#include <QJsonObject>
#include <QString>
#include <array>

// Shared athlete parameters; latent automatic-form state belongs to the save.
struct AthleteProfile {
    QString id, name, surname, country="POL", portrait, photoPath, sourceId;
    int overall=70, form=70, landing=2, birthYear=0;
    double personalBest=0;
    // 0: none; magnitude 1..4: perk strength; negative: opposite perk.
    std::array<int,4> perks{{0,0,0,0}};
    static AthleteProfile create();
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &json,AthleteProfile &profile,QString &error);
};
