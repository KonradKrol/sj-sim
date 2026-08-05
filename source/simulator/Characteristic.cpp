#include "Characteristic.h"

#include <QDebug>
#include <QObject>

Characteristic::Characteristic(double level, const QString &type) : level(level),
    type(type)
{
}

Characteristic::Characteristic(const QString &type) :
    level(0),
    type(type)
{
}

double Characteristic::getLevel() const
{
    return level;
}

void Characteristic::setLevel(double newLevel)
{
    level = newLevel;
}

QString Characteristic::getType() const
{
    return type;
}

void Characteristic::setType(const QString &newType)
{
    type = newType;
}

QStringList Characteristic::characteristicTypesForSpecificParent(short parentType, bool transformToDisplay)
{
    QStringList characteristicTypes;
    switch(parentType)
    {
    case Jumper:
        characteristicTypes.push_back("takeoff-height");
        characteristicTypes.push_back("flight-height");
        characteristicTypes.push_back("takeoff-power");
        characteristicTypes.push_back("landing-skill");
        characteristicTypes.push_back("both-legs-landing-tendence");
        characteristicTypes.push_back("dsq-probability");
        characteristicTypes.push_back("less-important-jumps");
        characteristicTypes.push_back("more-important-jumps");
        characteristicTypes.push_back("front-wind-use");
        characteristicTypes.push_back("back-wind-protection");
        break;
    case Hill:
        characteristicTypes.push_back("real-hs-point");
        characteristicTypes.push_back("takeoff-technique-effect");
        characteristicTypes.push_back("takeoff-power-effect");
        characteristicTypes.push_back("takeoff-form-effect");
        characteristicTypes.push_back("takeoff-randomness-effect");
        characteristicTypes.push_back("flight-technique-effect");
        characteristicTypes.push_back("flight-form-effect");
        characteristicTypes.push_back("flight-randomness-effect");
        break;
    case WindsGeneratorSettings:
        characteristicTypes.push_back("back-wind-probability");
        characteristicTypes.push_back("back-side-wind-probability");
        characteristicTypes.push_back("side-wind-probability");
        characteristicTypes.push_back("front-side-wind-probability");
        characteristicTypes.push_back("front-wind-probability");
        break;
    }

    if(transformToDisplay == true){
        for(auto & type : characteristicTypes)
        {
            type = getTypeToDisplay(Characteristic(0, type));
        }
    }

    return characteristicTypes;
}

QString Characteristic::getTypeToDisplay(const Characteristic &characteristic)
{
    if(characteristic.getLevel() > 0){
        /// Jumper
        if(characteristic.getType() == "takeoff-height")
            return QObject::tr("Wysoko wychodzi z progu");
        else if(characteristic.getType() == "takeoff-power")
            return QObject::tr("Zwiększona siła wybicia");
        else if(characteristic.getType() == "flight-height")
            return QObject::tr("Lata wysoko nad zeskokiem");
        else if(characteristic.getType() == "landing-skill")
            return QObject::tr("Dobrze ląduje");
        else if(characteristic.getType() == "both-legs-landing-tendence")
            return QObject::tr("Zwiększona tendencja do lądowania na dwie nogi");
        else if(characteristic.getType() == "dsq-probability")
            return QObject::tr("Zwiększone prawdopodobieństwo dyskwalifikacji");
        else if(characteristic.getType() == "less-important-jumps")
            return QObject::tr("Skacze dobrze w mało ważnych seriach");
        else if(characteristic.getType() == "more-important-jumps")
            return QObject::tr("Skacze dobrze w ważnych seriach");
        else if(characteristic.getType() == "front-wind-use")
            return QObject::tr("Dobrze wykorzystuje wiatr pod narty");
        else if(characteristic.getType() == "back-wind-protection")
            return QObject::tr("Jest odporny na wiatr tylny");

        /// Hill
        else if(characteristic.getType() == "real-hs-point")
            return QObject::tr("Bezpieczne skoki za punktem HS");
        else if(characteristic.getType() == "takeoff-technique-effect")
            return QObject::tr("Zwiększony wpływ techniki wybicia");
        else if(characteristic.getType() == "takeoff-power-effect")
            return QObject::tr("Zwiększony wpływ siły wybicia");
        else if(characteristic.getType() == "takeoff-form-effect")
            return QObject::tr("Zwiększony wpływ formy przy wybiciu");
        else if(characteristic.getType() == "takeoff-randomness-effect")
            return QObject::tr("Zwiększony wpływ losowości przy wybiciu");
        else if(characteristic.getType() == "flight-technique-effect")
            return QObject::tr("Zwiększony wpływ techniki lotu");
        else if(characteristic.getType() == "flight-form-effect")
            return QObject::tr("Zwiększony wpływ formy w locie");
        else if(characteristic.getType() == "flight-randomness-effect")
            return QObject::tr("Zwiększony wpływ losowości w locie");

        /// WindsGenerator
        else if(characteristic.getType() == "back-wind-probability")
            return QObject::tr("Zwiększona szansa na tylni wiatr");
        else if(characteristic.getType() == "back-side-wind-probability")
            return QObject::tr("Zwiększona szansa na tylnio-boczny wiatr");
        else if(characteristic.getType() == "side-wind-probability")
            return QObject::tr("Zwiększona szansa na boczny wiatr");
        else if(characteristic.getType() == "front-side-wind-probability")
            return QObject::tr("Zwiększona szansa na przednio-boczny wiatr");
        else if(characteristic.getType() == "front-wind-probability")
            return QObject::tr("Zwiększona szansa na przedni wiatr");
    }
    else if(characteristic.getLevel() < 0)
    {
        /// Jumper
        if(characteristic.getType() == "takeoff-height")
            return QObject::tr("Nisko wychodzi z progu");
        else if(characteristic.getType() == "takeoff-power")
            return QObject::tr("Zmniejszona siła wybicia");
        else if(characteristic.getType() == "flight-height")
            return QObject::tr("Lata nisko nad zeskokiem");
        else if(characteristic.getType() == "landing-skill")
            return QObject::tr("Słabo ląduje");
        else if(characteristic.getType() == "both-legs-landing-tendence")
            return QObject::tr("Zmniejszona tendencja do lądowania na dwie nogi");
        else if(characteristic.getType() == "dsq-probability")
            return QObject::tr("Zmniejszone prawdopodobieństwo dyskwalifikacji");
        else if(characteristic.getType() == "less-important-jumps")
            return QObject::tr("Skacze słabo w mało ważnych seriach");
        else if(characteristic.getType() == "more-important-jumps")
            return QObject::tr("Skacze słabo w ważnych seriach");
        else if(characteristic.getType() == "front-wind-use")
            return QObject::tr("Słabo wykorzystuje wiatr pod narty");
        else if(characteristic.getType() == "back-wind-protection")
            return QObject::tr("Jest słabo odporny na wiatr tylny");

        /// Hill
        else if(characteristic.getType() == "real-hs-point")
            return QObject::tr("Niebezpieczne skoki za punktem HS");
        else if(characteristic.getType() == "takeoff-technique-effect")
            return QObject::tr("Zmniejszony wpływ techniki wybicia");
        else if(characteristic.getType() == "takeoff-power-effect")
            return QObject::tr("Zmniejszony wpływ siły wybicia");
        else if(characteristic.getType() == "takeoff-form-effect")
            return QObject::tr("Zmniejszony wpływ formy przy wybiciu");
        else if(characteristic.getType() == "takeoff-randomness-effect")
            return QObject::tr("Zmniejszony wpływ losowości przy wybiciu");
        else if(characteristic.getType() == "flight-technique-effect")
            return QObject::tr("Zmniejszony wpływ techniki lotu");
        else if(characteristic.getType() == "flight-form-effect")
            return QObject::tr("Zmniejszony wpływ formy w locie");
        else if(characteristic.getType() == "flight-randomness-effect")
            return QObject::tr("Zmniejszony wpływ losowości w locie");

        /// WindsGenerator
        else if(characteristic.getType() == "back-wind-probability")
            return QObject::tr("Zmniejszona szansa na tylni wiatr");
        else if(characteristic.getType() == "back-side-wind-probability")
            return QObject::tr("Zmniejszona szansa na tylnio-boczny wiatr");
        else if(characteristic.getType() == "side-wind-probability")
            return QObject::tr("Zmniejszona szansa na boczny wiatr");
        else if(characteristic.getType() == "front-side-wind-probability")
            return QObject::tr("Zmniejszona szansa na przednio-boczny wiatr");
        else if(characteristic.getType() == "front-wind-probability")
            return QObject::tr("Zmniejszona szansa na przedni wiatr");
    }
    else if(characteristic.getLevel() == 0)
    {
        /// Jumper
        if(characteristic.getType() == "takeoff-height")
            return QObject::tr("Wysokość wyjścia z progu");
        else if(characteristic.getType() == "takeoff-power")
            return QObject::tr("Siła wybicia");
        else if(characteristic.getType() == "flight-height")
            return QObject::tr("Wysokość lotu");
        else if(characteristic.getType() == "landing-skill")
            return QObject::tr("Umiejętność lądowania");
        else if(characteristic.getType() == "both-legs-landing-tendence")
            return QObject::tr("Tendencja do lądowania na dwie nogi");
        else if(characteristic.getType() == "dsq-probability")
            return QObject::tr("Prawdopodobieństwo dyskwalifikacji");
        else if(characteristic.getType() == "less-important-jumps")
            return QObject::tr("Skoki w mało ważnych seriach");
        else if(characteristic.getType() == "more-important-jumps")
            return QObject::tr("Skoki w ważnych seriach");
        else if(characteristic.getType() == "front-wind-use")
            return QObject::tr("Umiejętność wykorzystania wiatru pod narty");
        else if(characteristic.getType() == "back-wind-protection")
            return QObject::tr("Odporność na wiatr tylny");

        /// Hill
        else if(characteristic.getType() == "real-hs-point")
            return QObject::tr("Bezpieczeństwo skoków za punktem HS");
        else if(characteristic.getType() == "takeoff-technique-effect")
            return QObject::tr("Wpływ techniki wybicia");
        else if(characteristic.getType() == "takeoff-power-effect")
            return QObject::tr("Wpływ siły wybicia");
        else if(characteristic.getType() == "takeoff-form-effect")
            return QObject::tr("Wpływ formy przy wybiciu");
        else if(characteristic.getType() == "takeoff-randomness-effect")
            return QObject::tr("Wpływ losowości przy wybiciu");
        else if(characteristic.getType() == "flight-technique-effect")
            return QObject::tr("Wpływ techniki lotu");
        else if(characteristic.getType() == "flight-form-effect")
            return QObject::tr("Wpływ formy w locie");
        else if(characteristic.getType() == "flight-randomness-effect")
            return QObject::tr("Wpływ losowości w locie");

        /// WindsGenerator
        else if(characteristic.getType() == "back-wind-probability")
            return QObject::tr("Szansa na tylni wiatr");
        else if(characteristic.getType() == "back-side-wind-probability")
            return QObject::tr("Szansa na tylnio-boczny wiatr");
        else if(characteristic.getType() == "side-wind-probability")
            return QObject::tr("Szansa na boczny wiatr");
        else if(characteristic.getType() == "front-side-wind-probability")
            return QObject::tr("Szansa na przednio-boczny wiatr");
        else if(characteristic.getType() == "front-wind-probability")
            return QObject::tr("Szansa na przedni wiatr");
    }
    return "<Nieprawidłowa cecha>";
}

int Characteristic::getCharacteristicsForParentTypeCount(short parentType)
{
    return characteristicTypesForSpecificParent(parentType).count();
}

int Characteristic::getAdditionToCharacteristicComboBoxIndexToGetCharacteristic(short parentType)
{
    int addition = 0;
    for(int i=0; i < parentType + 1; i++)
    {
        addition += getCharacteristicsForParentTypeCount(i);
    }
    return addition;
}
