#ifndef WIND_H
#define WIND_H

#include "../utilities/Identifiable.h"

class QString;

class Wind : public Identifiable
{
public:
    enum DirectionType{
        Back,
        BackSide,
        Side,
        FrontSide,
        Front
    };

    Wind(short direction = 0, double value = 0);
    Wind(double value);

    static QString getStyleSheetForAveragedWind(double avgWind);
    static double getAbsForFrontWindDistance(double wind);
    short getDirectionType() const;

private:
    double direction;
    double strength;

public:
    double getStrengthToAveragedWind() const;
    double getStrength() const;
    void setStrength(double newStrength);
    double getDirection() const;
    void setDirection(double newDirection);
};

#endif // WIND_H
