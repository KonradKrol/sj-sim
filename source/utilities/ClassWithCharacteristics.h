#ifndef CLASSWITHCHARACTERISTICS_H
#define CLASSWITHCHARACTERISTICS_H

#include <QSet>
#include <QString>

//class Characteristic;
#include "../simulator/Characteristic.h"

class ClassWithCharacteristics
{
protected:
    QSet<Characteristic> characteristics;
public:
    ClassWithCharacteristics();

    void insertCharacteristic(const Characteristic & characteristic);
    void insertCharacteristic(const QString & type, double level);
    void removeCharacteristic(const Characteristic & characteristic);
    void removeCharacteristic(const QString & type);
    bool characteristicsContains(const QString & type);
    double getLevelOfCharacteristic(const QString & characteristicType);
    QSet<Characteristic> getCharacteristics() const;
    void setCharacteristics(const QSet<Characteristic> &newCharacteristics);
};

#endif // CLASSWITHCHARACTERISTICS_H
