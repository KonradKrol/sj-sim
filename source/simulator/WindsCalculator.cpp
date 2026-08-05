#include "WindsCalculator.h"

#include "../utilities/functions.h"
#include "../global/MyRandom.h"

#include <QRandomGenerator>
#include <QDebug>
#include <functional>

Wind WindsCalculator::getAveragedWind(QVector<Wind> winds, short type)
{
    double percent = 100 / winds.count();
    double windAvg = 0;
    int i=0;

    for(auto & wind : winds)
    {
        wind.setStrength(wind.getStrength() + MyRandom::normalDistributionRandom(0, wind.getStrength() / 13));
        wind.setDirection(wind.getDirection() + MyRandom::normalDistributionRandom(0, wind.getStrength() * 2));
        if(type == Simple){
            if(wind.getDirectionType() == Wind::Front || wind.getDirectionType() == Wind::FrontSide){
                windAvg += wind.getStrength() * percent;
            }
            else if(wind.getDirectionType() == Wind::Back || wind.getDirectionType() == Wind::BackSide){
                windAvg -= wind.getStrength() * percent;
            }
        }
        else if(type == SimpleImproved){
            if(wind.getDirectionType() == Wind::Front)
                windAvg += wind.getStrength() * percent;
            else if(wind.getDirectionType() == Wind::FrontSide)
                windAvg += wind.getStrength() * 0.5 * percent;
            else if(wind.getDirectionType() == Wind::BackSide)
                windAvg -= wind.getStrength() * 0.5 * percent;
            else if(wind.getDirectionType() == Wind::Back)
                windAvg -= wind.getStrength() * percent;
        }
        else if(type == Original)
        {
            double addition = cos(wind.getDirection() * M_PI / 180) * wind.getStrength() * percent;
            if(wind.getDirection() > 270 || wind.getDirection() < 90)
                windAvg += addition;
            else
                windAvg += addition;
            //qDebug()<<"addition: "<<addition<<" ("<<wind.getDirection()<<" stopni, "<<wind.getStrength()<<" m/s)";
        }
        i++;
    }
    windAvg /= 100;
    windAvg = roundDoubleToTwoPlaces(windAvg);

    return Wind(windAvg);
}

double WindsCalculator::getWindCompensation(Wind avgWind, Hill *hill)
{
    double compensation = 0;

    if(avgWind.getDirectionType() == Wind::Back)
        compensation = (avgWind.getStrength() * hill->getPointsForBackWind());
    else if(avgWind.getDirectionType() == Wind::Front)
        compensation = (-(avgWind.getStrength() * hill->getPointsForFrontWind()));

    compensation = (roundDoubleToOnePlace(compensation));
    return compensation;
}

double WindsCalculator::getGateCompensation(int startGate, int actualGate, Hill *hill)
{
    return (startGate - actualGate) * hill->getPointsForGate();
}
