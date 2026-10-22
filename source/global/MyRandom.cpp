#include "MyRandom.h"

#include <QVector>
#include <QPair>
#include <QRandomGenerator>
#include <QDebug>
#include <random>
#include <cmath>

std::random_device rd{};
std::mt19937 generator{rd()};

MyRandom::MyRandom()
{

}

double MyRandom::randomDouble(double min, double max, short precision)
{
    if(!std::isfinite(min) || !std::isfinite(max) || min > max)
        return 0;
    if(min == max)
        return min;

    const double scale = std::pow(10., precision);
    const double first = std::ceil(min * scale), pastLast = std::ceil(max * scale);
    const double count = pastLast - first;
    const double fraction = QRandomGenerator::global()->generateDouble();
    const double ceiling = std::nextafter(max, min);
    if(scale > 0 && std::isfinite(scale) && std::isfinite(first) && std::isfinite(pastLast) && std::isfinite(count) && count > 0)
        return qBound(min, (first + std::floor(fraction * count)) / scale, ceiling);

    return qBound(min, min * (1 - fraction) + max * fraction, ceiling);
}

int MyRandom::randomInt(int min, int max)
{
    if(min > max)
        return 0;
    return std::uniform_int_distribution<int>(min, max)(*QRandomGenerator::global());
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
