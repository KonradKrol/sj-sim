#ifndef JUMPERFORMTENDENCE_H
#define JUMPERFORMTENDENCE_H

#include <QJsonObject>
#include "../simulator/Jumper.h"
#include "../global/IdentifiableObjectsStorage.h"

class JumperFormTendence
{
public:
    JumperFormTendence(Jumper * jumper = nullptr, double tendence = 0);

    enum TendenceType
    {
        Increase,
        Decrease,
        Neutral
    };

    int getTendenceType();
    static QJsonObject getJsonObject(const JumperFormTendence &tendence);
    static JumperFormTendence getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage);

private:
    Jumper * jumper;
    double tendence; //Jeżeli na minusie - siła spadkowej tendencji, jeżeli na plusie - siła wzrostowej tendencji
public:
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
    double &getTendenceReference();
    void setTendence(double newTendence);
    double getTendence() const;
};

#endif // JUMPERFORMTENDENCE_H
