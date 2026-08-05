#include "WindsGenerator.h"

#include "../../global/MyRandom.h"
#include "../../utilities/functions.h"

#include <QDebug>
#include <random> //std

WindsGenerator::WindsGenerator(const QVector<WindGenerationSettings> &generationSettings) :
    generationSettings(generationSettings)
{}

int WindsGenerator::getWindsCount() const
{
    return generationSettings.count();
}

QVector<WindGenerationSettings> WindsGenerator::getGenerationSettings() const
{
    return generationSettings;
}

void WindsGenerator::setGenerationSettings(const QVector<WindGenerationSettings> &newGenerationSettings)
{
    generationSettings = newGenerationSettings;
}


QVector<Wind> WindsGenerator::generateWinds()
{
    QVector<Wind> winds;

    for(int i=0; i<getWindsCount(); i++)
    {
        Wind wind;
        WindGenerationSettings * settings = &generationSettings[i];

        double windStrength = settings->getBaseWindStrength();
        if(settings->getWindStrengthInstability() > 0){

            double base = MyRandom::randomDouble(-settings->getBaseWindStrength() / 20, settings->getBaseWindStrength() / 20);
            double deviation = settings->getWindStrengthInstability() / 5.2;

            double random = 0;
            if(deviation > 0)
                random = MyRandom::normalDistributionRandom(base, deviation);
            windStrength += random;
        }
        double windDirection = settings->getBaseDirection();
        if(settings->getWindDirectionInstability() > 0){
            double base = 0;
            double deviation = settings->getWindDirectionInstability() / 5.2;

            double random = 0;
            if(deviation > 0)
                random = MyRandom::normalDistributionRandom(base, deviation);
            windDirection = settings->getBaseDirection() + random;
            if(windDirection < 0)
            {
                windDirection = 360 - (-windDirection);
            }
            if(windDirection > 360)
            {
                windDirection -= (int(windDirection / 360)) * 360;
            }
        }

        /*short windDirection = Wind::Back;

        double backProb = 250;
        double backSideProb = 250;
        double sideProb = 250;
        double frontSideProb = 250;
        double frontProb = 250;

        const double baseDirectionChanceMultiplier = 95 - ((settings->getWindDirectionInstability()) * 18.5);

        switch(settings->getBaseDirection())
        {
        case Wind::Back:
            backSideProb *= 2.55;
            sideProb *= 0.55;
            frontSideProb *= 0.07;
            frontProb *= 0.01;

            backProb *= baseDirectionChanceMultiplier;
            break;
        case Wind::BackSide:
            backProb *= 2.4;
            sideProb *= 2.0;
            frontSideProb *= 0.172;
            frontProb *= 0.028;

            backSideProb *= baseDirectionChanceMultiplier;
            break;
        case Wind::Side:
            backProb *= 0.13;
            backSideProb *= 2.5;
            frontSideProb *= 2.5;
            frontProb *= 0.13;

            sideProb *= baseDirectionChanceMultiplier;
            break;
        case Wind::FrontSide:
            backProb *= 0.028;
            backSideProb *= 0.172;
            sideProb *= 2.0;
            frontProb *= 2.4;

            frontSideProb *= baseDirectionChanceMultiplier;
            break;
        case Wind::Front:
            backProb *= 0.01;
            backSideProb *= 0.07;
            sideProb *= 0.55;
            frontSideProb *= 2.55;

            frontProb *= baseDirectionChanceMultiplier;
            break;
        }
        int level = settings->getLevelOfCharacteristic("back-wind-probability");
        if(level > 0)
            backProb *= level;
        else if(level < 0)
            backProb /= -level;
        level = settings->getLevelOfCharacteristic("back-side-wind-probability");
        if(level > 0)
            backSideProb *= level;
        else if(level < 0)
            backSideProb /= -level;
        level = settings->getLevelOfCharacteristic("side-wind-probability");
        if(level > 0)
            sideProb *= level;
        else if(level < 0)
            sideProb /= -level;
        level = settings->getLevelOfCharacteristic("front-side-wind-probability");
        if(level > 0)
            frontSideProb *= level;
        else if(level < 0)
            frontSideProb /= -level;
        level = settings->getLevelOfCharacteristic("front-wind-probability");
        if(level > 0)
            frontProb *= level;
        else if(level < 0)
            frontProb /= -level;


        if(settings->getWindDirectionInstability() == 0){
            wind.setDirection(settings->getBaseDirection());
        }
        else{
            QVector<double> probabilities;
            double drawSum = backProb + backSideProb + sideProb + frontSideProb + frontProb;
            probabilities.push_back(backProb);
            probabilities.push_back(backSideProb);
            probabilities.push_back(sideProb);
            probabilities.push_back(frontSideProb);
            probabilities.push_back(frontProb);
            double drawRandom = MyRandom::randomDouble(0, drawSum);

            double actualSum = 0;
            for(int i=0; i < 5; i++)
            {
                actualSum += probabilities[i];
                if(drawRandom < actualSum){
                    windDirection = i+1;
                    break;
                }
            }


            wind.setDirection(windDirection);
        }*/

        if(windStrength < 0)
            windStrength = (-windStrength);
        wind.setStrength(windStrength);
        wind.setDirection(windDirection);

        winds.push_back(wind);
    }
    return winds;
}

int WindsGenerator::calculateWindsCountByKPoint(double KPoint)
{
    if(KPoint <= 15)
        return 1;
    else if(KPoint <= 36)
        return 2;
    else if(KPoint <= 60.5)
        return 3;
    else if(KPoint <= 85)
        return 4;
    else if(KPoint <= 110)
        return 5;
    else if(KPoint <= 132)
        return 6;
    else if(KPoint <= 170)
        return 7;
    else if(KPoint <= 210)
        return 8;
    else
    {
        int i = 9;
        i += (KPoint - 210) / (55 * ((double(i) - 8) / 10));
        return i;
    }
}

QPair<double, double> WindsGenerator::getRangeOfWindSector(int sector, double KPoint)
{
    int windsCount = WindsGenerator::calculateWindsCountByKPoint(KPoint);
    double segmentDistance = (KPoint + (KPoint / (windsCount ))) / (windsCount - 1);
    if(sector == WindsGenerator::calculateWindsCountByKPoint(KPoint))
        return QPair<double, double>(roundDoubleToHalf(((double)windsCount - 1) * segmentDistance), (-1));

    double min = 0, max = 0;
    min = roundDoubleToHalf((sector - 1) * (segmentDistance));
    max = roundDoubleToHalf(sector * segmentDistance);

    return QPair<double, double>(min, max);
}
