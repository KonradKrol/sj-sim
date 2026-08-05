#ifndef WINDGENERATIONSETTINGS_H
#define WINDGENERATIONSETTINGS_H

#include "../Wind.h"
#include "../Characteristic.h"
#include "../../utilities/ClassWithCharacteristics.h"

#include <QVector>

class WindGenerationSettings : public ClassWithCharacteristics
{
public:
    WindGenerationSettings(short baseDirection = 0, double windDirectionInstability = 0, double baseWindStrength = 0, double windStrengthInstability = 0);
    static QVector<WindGenerationSettings> getRandomSettings(double lotability = (-1));
    static void randomizeSettings(QVector<WindGenerationSettings> * settings, double lotability = (-1), double prefferedDirection = (-1));
private:
    double baseDirection;
    double windDirectionInstability;
    double baseWindStrength;
    double windStrengthInstability; // maksymalnie 10.00
public:
    double getWindDirectionInstability() const;
    void setWindDirectionInstability(double newWindDirectionStability);
    double getBaseWindStrength() const;
    void setBaseWindStrength(double newBaseWindStrength);
    double getWindStrengthInstability() const;
    void setWindStrengthInstability(double newWindStrengthStability);
    double getBaseDirection() const;
    void setBaseDirection(double newBaseDirection);
};

#endif // WINDGENERATIONSETTINGS_H
