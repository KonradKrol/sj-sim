#include "InrunSnowGenerator.h"
#include "../global/MyRandom.h"
#include <QDebug>

InrunSnowGenerator::InrunSnowGenerator(double baseInrunSnow, double inrunSnowDeviation) : baseInrunSnow(baseInrunSnow), inrunSnowDeviation(inrunSnowDeviation)
{

}

double InrunSnowGenerator::generateInrunSnow()
{
    double toReturn;
    if(inrunSnowDeviation > 0){
        double base = MyRandom::randomDouble(-baseInrunSnow / 20, baseInrunSnow / 20);
        double deviation = inrunSnowDeviation / 5.2;
        toReturn = baseInrunSnow + MyRandom::normalDistributionRandom(base, deviation);
    }
    else
        toReturn = baseInrunSnow;

    if(toReturn > 0)
        return toReturn;
    else
        return 0;
}

double InrunSnowGenerator::getInrunSnowDeviation() const
{
    return inrunSnowDeviation;
}

void InrunSnowGenerator::setInrunSnowDeviation(double newInrunSnowDeviation)
{
    inrunSnowDeviation = newInrunSnowDeviation;
}

double InrunSnowGenerator::getBaseInrunSnow() const
{
    return baseInrunSnow;
}

void InrunSnowGenerator::setBaseInrunSnow(double newBaseInrunSnow)
{
    baseInrunSnow = newBaseInrunSnow;
}
