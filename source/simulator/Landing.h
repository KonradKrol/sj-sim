#ifndef LANDING_H
#define LANDING_H

#include <QString>

class Landing
{
public:
    Landing(short type = 0, double imbalance = 0);
private:
    short type;

public:
    enum Type{
        TelemarkLanding,
        BothLegsLanding,
        SupportLanding,
        Fall
    };

    QString getTextLandingType() const;
    short getType() const;
    void setType(short newType);

    static QString getShortLandingTypeTextInfo(short type);
    static QString getStyleSheetForLandingTypeTextInfo(short type);
};

#endif // LANDING_H
