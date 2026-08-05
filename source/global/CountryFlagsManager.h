#ifndef COUNTRYFLAGSMANAGER_H
#define COUNTRYFLAGSMANAGER_H

class QPixmap;
class QString;
class QSize;

class CountryFlagsManager
{
public:
    CountryFlagsManager();

    static QPixmap getFlagPixmap(const QString & twoLettersCountryCode);
    static QSize getFlagPixmapSize(short type = 0);

    enum FlagSizeType{
        Normal
    };
};

#endif // COUNTRYFLAGSMANAGER_H
