#include "Wind.h"

#include <QDebug>
#include <QString>
#include <QObject>
#include "../global/GlobalAppSettings.h"

double Wind::getStrengthToAveragedWind() const
{
    if(getDirectionType() == Wind::Front || getDirectionType() == Wind::FrontSide) return getStrength();
    else return -getStrength();
}

QString Wind::getStyleSheetForAveragedWind(double avgWind)
{
    if(avgWind > 0)
        return "QLabel{color: rgb(74, 189, 68);}";
    else if(avgWind < 0)
        return "QLabel{color: rgb(214, 41, 28);}";
    else return "QLabel{color: rgb(50, 50, 50);}";

    return "QLabel{color: yellow;}";
}

double Wind::getAbsForFrontWindDistance(double wind)
{
    if(wind <= 90)
        return abs(0 - wind);
    else if(wind >= 270)
        return abs(360 - wind);
    return 0;
}

short Wind::getDirectionType() const
{
    if(direction <= 32 || direction >= 328)
        return Front;
    else if(direction <= 70 || direction >= 290)
        return FrontSide;
    else if(direction <= 110 || direction >= 250)
        return Side;
    else if(direction <= 147 || direction >= 213)
        return BackSide;
    else
        return Back;
}

Wind::Wind(short direction, double value) : direction(direction),
    strength(value),
    Identifiable()
{}

Wind::Wind(double value) : strength(value)
{
    if(value > 0)
        direction = 0;
    else if(value < 0)
        direction = 180;
    else
        direction = 90;
    if(strength < 0)
        strength = -strength;
}

double Wind::getDirection() const
{
    return direction;
}

void Wind::setDirection(double newDirection)
{
    direction = newDirection;
}

double Wind::getStrength() const
{
    return strength;
}

void Wind::setStrength(double newStrength)
{
    strength = newStrength;
}
