#include "MyRandom.h"

#include <QVector>
#include <QPair>
#include <QRandomGenerator>
#include <QDebug>
#include <random>

std::random_device rd{};
std::mt19937 generator{rd()};

MyRandom::MyRandom()
{

}

double MyRandom::randomDouble(double min, double max, short precision)
{
    if(min >= max)
        return 0;
    if(max == 0)
        return 0;

    QVector<double> results;
    ulong toMultipy = std::pow(10, precision);
    for(int i=0; i<1000; i++)
    {
        double result = (QRandomGenerator::global()->bounded((int(double(double(min * toMultipy)))), int(double(double(max) * toMultipy))));
        result = double(result / toMultipy);
        results.push_back(result);
    }
    return results[QRandomGenerator::global()->bounded(0, results.size() - 1)];

}

int MyRandom::randomInt(int min, int max)
{
    if(min >= max)
        return 0;
    if(max == 0)
        return 0;

    QVector<int> results;
    for(int i=0; i<1000; i++)
    {
        int result = QRandomGenerator::global()->bounded(min, max + 1);
        results.push_back(result);
    }
    return results[QRandomGenerator::global()->bounded(0, results.size() - 1)];
}

double MyRandom::reducingChancesRandom(double min, double max, double resultNumbersChange, double startingChance, double chancesChangeMultiplier, short drawType, short resultNumbersType)
{
    QVector<QPair<double, double>> drawInfo; // 1. liczba    2. szansa

    if(drawType == AddingChances)
    {
        double chance = startingChance;
        if(resultNumbersType == FromSmallerToLarger){
            for(double i=min; i<max; i+=resultNumbersChange)
            {
                drawInfo.push_back(QPair<double, double>(i, chance));
                chance /= chancesChangeMultiplier;
            }
        }
        else if(resultNumbersType == FromLargerToSmaller){
            for(double i=max; i>min; i-=resultNumbersChange)
            {
                drawInfo.push_back(QPair<double, double>(i, chance));
                chance /= chancesChangeMultiplier;
            }
        }

        // losowanie
        double sum = 0;
        for(const auto & random : drawInfo)
            sum += random.second;

        double drawRandom = randomDouble(0, sum);
        double sum2=0;
        for(int i=drawInfo.size() - 1; i>=0; i--)
        {
            sum2 += drawInfo[i].second;

            if(drawRandom <= sum2)
                return drawInfo[i].first;
        }
    }
    else if(drawType == InTurnFromTheHighestChanceNumber)
    {
        double chance = startingChance;
        if(resultNumbersType == FromSmallerToLarger){
            for(double i=min; i<max; i+=resultNumbersChange)
            {
                drawInfo.push_back(QPair<double, double>(i, chance));
                chance *= chancesChangeMultiplier;
            }
        }
        else if(resultNumbersType == FromLargerToSmaller){
            for(double i=max; i>min; i-=resultNumbersChange)
            {
                drawInfo.push_back(QPair<double, double>(i, chance));
                chance *= chancesChangeMultiplier;
            }
        }

        // 1, 2.2, 6.6, (..) 150, 400

        QVector<double> probabilities;
        for(const auto & random : drawInfo)
            probabilities.push_back(random.second);

        for(int i=probabilities.size() - 1; i >= 0; i--)
        {
            double chance = 1 / probabilities[i] * 100;
            double drawRandom = randomDouble(0, 100, 5);
            if(drawRandom <= chance)
                return drawInfo[i].first;
        }
    }

    return 0;
}

double MyRandom::normalDistributionRandom(double base, double deviation)
{
    std::normal_distribution<double> distribution(base, deviation);
    return distribution(generator);
}

double MyRandom::lognormalDistributionRandom(double base, double deviation)
{
    std::lognormal_distribution<double> distribution(base, deviation);
    return distribution(generator);
}

double MyRandom::gammaDistributionRandom(double scale, double shape)
{
    std::gamma_distribution<double> distribution(scale, shape);
    return distribution(generator);
}

double MyRandom::normalDistributionRandomHalf(double base, double deviation, short half)
{
    while(true)
    {
        double random = normalDistributionRandom(base, deviation);
        if(half == Positive)
        {
            if(random >= 0)
                return random;
        }
        else if(half == Negative)
        {
            if(random < 0)
                return random;
        }
        else
            continue;
    }
    return 1000;
}
