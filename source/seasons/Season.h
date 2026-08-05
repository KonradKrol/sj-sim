#ifndef SEASON_H
#define SEASON_H

#include <QVector>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "SeasonCalendar.h"
#include "Classification.h"
#include "../global/IdentifiableObjectsStorage.h"

class Season : public Identifiable
{
public:
    Season();
    ~Season();

    static QJsonObject getJsonObject(const Season &season);
    static Season getFromJson(QJsonObject obj, IdentifiableObjectsStorage *storage);
    bool containsCalendarByName(QString n);

private:
    QVector<SeasonCalendar *> calendars;
    SeasonCalendar * actualCalendar;
    int seasonNumber;

public:
    int getSeasonNumber() const;
    void setSeasonNumber(int newSeasonNumber);
    QVector<SeasonCalendar *> getCalendars() const;
    QVector<SeasonCalendar *> & getCalendarsReference();
    void setCalendars(const QVector<SeasonCalendar *> &newCalendars);
    SeasonCalendar *getActualCalendar() const;
    void setActualCalendar(SeasonCalendar *newActualCalendar);
};

#endif // SEASON_H
