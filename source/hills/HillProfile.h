#pragma once
#include <QJsonObject>
#include <QString>
#include <array>

// Authoring data for the next simulation model; the current engine does not read it.
struct HillProfile {
    QString id, name, country="POL", sourceId;
    double k=120, hs=134, record=0;
    double pointsAtK=60, pointsPerMetre=1.8, gatePoints=5, headwindPoints=8, tailwindPoints=12;
    // Take-off/flight, forgiving/demanding landing, variable/stable wind,
    // prevailing headwind/tailwind (weather traits affect automatic proposals).
    // Sign chooses a side; magnitude 1..4 selects its strength; zero means neither.
    std::array<int,4> perks{{0,0,0,0}};
    static HillProfile create();
    static double automaticTailwindPoints(double headwindPoints);
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &json,HillProfile &profile,QString &error);
    double realHs() const;
    double takeoffBias() const;
    double windVariability() const;
    double headwindProbability() const;
};
