#pragma once
#include <QMap>
#include <QVector>
#include <QJsonObject>
#include <QDate>
#include <QUuid>

struct ClassificationScoringMethod {
    QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString name;
    int type = 0;
    QMap<int, double> points, alternativePoints;
};

struct ClassificationOptions {
    QVector<ClassificationScoringMethod> methods;
    bool contextParameters = false;
    bool ageFilter = false;
    int minimumAge = 0, maximumAge = 150, referenceYear = QDate::currentDate().year();
    bool includeUnknownAge = false;
    // 0: all; 1: best N; 2: worst N, with an exact limit even for ties.
    int countrySelection = 0, countryCount = 1;
    QJsonObject toJson() const;
    static ClassificationOptions fromJson(const QJsonObject &object);
};

// Missing keys inherit classification defaults. Each competition selects its own
// method and can optionally override the supported numeric parameters.
struct ClassificationCompetitionSettings {
    QString methodId;
    QJsonObject parameters;
    QJsonObject toJson() const;
    static ClassificationCompetitionSettings fromJson(const QJsonObject &object);
};
