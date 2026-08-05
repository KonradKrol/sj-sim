#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <qhashfunctions.h>


class Characteristic
{
public:
    Characteristic(double level = 0, const QString &type = "");
    Characteristic(const QString &type);
    operator QString() const {return type;}
private:

    double level; // od -10 do 10
    QString type;

public:
    double getLevel() const;
    void setLevel(double newLevel);
    QString getType() const;
    void setType(const QString &newType);

    enum ParentType{
        Jumper,
        Hill,
        WindsGeneratorSettings
    };

    static QStringList characteristicTypesForSpecificParent(short parentType, bool transformToDisplay = false);
    static QString getTypeToDisplay(const Characteristic & characteristic);
    static int getCharacteristicsForParentTypeCount(short parentType);
    static int getAdditionToCharacteristicComboBoxIndexToGetCharacteristic(short parentType);
};

inline bool operator==(const Characteristic & char1, const Characteristic char2)
{
    return char1.getType() == char2.getType();
}

inline size_t qHash(const Characteristic & k, size_t seed) noexcept
{
    return(k.getType(), seed);
    /*QtPrivate::QHashCombine hash;
    seed = hash(seed, k.getType());
    return seed;*/
}

#endif // CHARACTERISTIC_H
