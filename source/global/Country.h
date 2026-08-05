#ifndef COUNTRY_H
#define COUNTRY_H

#include <QString>

class Country
{
public:
    Country(const QString &name = "", const QString &alpha2 = "", const QString &alpha3 = "");

private:
    QString name;
    QString alpha2;
    QString alpha3;
public:
    QString getName() const;
    void setName(const QString &newName);
    QString getAlpha2() const;
    void setAlpha2(const QString &newAlpha2);
    QString getAlpha3() const;
    void setAlpha3(const QString &newAlpha3);
};

#endif // COUNTRY_H
