#include "WindsCalculator.h"

#include "../utilities/functions.h"
#include "wind-generation/WindsGenerator.h"
#include "../global/MyRandom.h"

#include <cmath>
#include <QRandomGenerator>
#include <QDebug>
#include <functional>

Wind WindsCalculator::getAveragedWind(QVector<Wind> winds, short type)
{
    if(winds.isEmpty())
        return Wind(0.0);

    double percent = 100.0 / winds.count();
    double windAvg = 0;
    int i=0;

    for(auto & wind : winds)
    {
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
    if(std::abs(windAvg)<1e-10)windAvg=0;
    windAvg = roundDoubleToTwoPlaces(windAvg);

    return Wind(windAvg);
}

QVector<Wind> WindsCalculator::getScoringWinds(QVector<Wind> winds,double distance,double kPoint,short distanceEffect)
{
    if(winds.isEmpty() || kPoint<=0)return winds;
    const double segment=WindsGenerator::getWindSegmentDistance(kPoint,winds.size());
    if(distanceEffect==1 && distance<=.75*kPoint)
        winds.resize(qBound(0,int(std::round(.75*kPoint/segment)),winds.size()));
    else if(distanceEffect==2)
        for(int i=0;i<winds.size();++i)
            winds[i].setStrength(winds[i].getStrength()*qBound(0.,(distance-i*segment)/segment,1.));
    return winds;
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
