#include "Country.h"

QString Country::getName() const
{
    return name;
}

void Country::setName(const QString &newName)
{
    name = newName;
}

QString Country::getAlpha2() const
{
    return alpha2;
}

void Country::setAlpha2(const QString &newAlpha2)
{
    alpha2 = newAlpha2;
}

QString Country::getAlpha3() const
{
    return alpha3;
}

void Country::setAlpha3(const QString &newAlpha3)
{
    alpha3 = newAlpha3;
}

Country::Country(const QString &name, const QString &alpha2, const QString &alpha3) : name(name),
    alpha2(alpha2),
    alpha3(alpha3)
{}
