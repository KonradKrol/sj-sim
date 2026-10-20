#ifndef WINDSCALCULATOR_H
#define WINDSCALCULATOR_H

#include "../utilities/Identifiable.h"
#include "Wind.h"
#include "Hill.h"
#include "JumpManipulator.h"

#include <QVector>

class WindsCalculator
{
public:
    enum AverageWindType{
        Simple,
        SimpleImproved,
        Original
    };
    static Wind getAveragedWind(QVector<Wind> winds, short type);
    static QVector<Wind> getScoringWinds(QVector<Wind> winds, double distance, double kPoint, short distanceEffect);
    static double getWindCompensation(Wind avgWind, Hill * hill);
    static double getGateCompensation(int startGate, int actualGate, Hill * hill);
};

#endif // WINDSCALCULATOR_H
