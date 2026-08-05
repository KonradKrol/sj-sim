#ifndef WINDSGENERATOR_H
#define WINDSGENERATOR_H

#include "WindGenerationSettings.h"
#include "../Wind.h"

#include <QVector>
#include <QPair>
#include <QStringList>

class WindsGenerator
{
public:
    WindsGenerator(const QVector<WindGenerationSettings> &generationSettings = QVector<WindGenerationSettings>());
private:
    QVector<WindGenerationSettings> generationSettings;
public:
    QVector<Wind> generateWinds();

    static int calculateWindsCountByKPoint(double KPoint);
    static QPair<double, double> getRangeOfWindSector(int sector, double KPoint);
    QVector<WindGenerationSettings> getGenerationSettings() const;
    void setGenerationSettings(const QVector<WindGenerationSettings> &newGenerationSettings);
    int getWindsCount() const;
};

#endif // WINDSGENERATOR_H
