#include "Landing.h"

#include <QObject>

Landing::Landing(short type, double imbalance) : type(type)
{}

QString Landing::getTextLandingType() const
{
    switch(getType())
    {
    case TelemarkLanding: return QObject::tr("Telemark");
    case BothLegsLanding: return QObject::tr("Na dwie nogi");
    case SupportLanding: return QObject::tr("Podpórka");
    case Fall: return QObject::tr("Upadek");
    }
    return QObject::tr("error");
}

short Landing::getType() const
{
    return type;
}

void Landing::setType(short newType)
{
    type = newType;
}

QString Landing::getShortLandingTypeTextInfo(short type)
{
    switch(type)
    {
    case Landing::TelemarkLanding:
        return QObject::tr("T");
    case Landing::BothLegsLanding:
        return QObject::tr("DN");
    case Landing::SupportLanding:
        return QObject::tr("P");
    case Landing::Fall:
        return QObject::tr("U");
    }
    return "ERROR";
}

QString Landing::getStyleSheetForLandingTypeTextInfo(short type)
{
    switch(type)
    {
    case Landing::TelemarkLanding:
        return "QLabel{color: rgb(23, 138, 71);}";
    case Landing::BothLegsLanding:
        return "QLabel{color: rgb(194, 209, 59);}";
    case Landing::SupportLanding:
        return "QLabel{color: rgb(214, 147, 64);}";
    case Landing::Fall:
        return "QLabel{color: rgb(196, 88, 67);}";
    }
    return "QLabel{color: rgb(0, 0, 0);}";
}
