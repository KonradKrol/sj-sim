#ifndef SEASONCALENDARPRESET_H
#define SEASONCALENDARPRESET_H

#include <QVector>
#include <QPair>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include "SeasonCalendar.h"
#include "../global/IdentifiableObjectsStorage.h"

class SeasonCalendarPreset
{
public:
    SeasonCalendarPreset(SeasonCalendar * seasonCalendar = nullptr);
    ~SeasonCalendarPreset();

    static QJsonObject getJsonObject(SeasonCalendarPreset preset);
    static SeasonCalendarPreset getFromJson(QJsonObject json, IdentifiableObjectsStorage *storage, QMap<ulong, Identifiable *> * before120Map = nullptr);
    static QVector<SeasonCalendarPreset> getVectorFromJson(QByteArray json);

    bool saveToFile(QString dir = "");
    bool loadFromFile(QString fileName, QString dir = "");

private:
    QString name;
    SeasonCalendar calendar;
    QVector<QPair<QString, double>> hills; //Po koleji skocznia dla każdego konkursu tak jak są konkursy w kalendarzu.
public:
    SeasonCalendar & getCalendarReference();
    void setCalendar(const SeasonCalendar &newCalendar);
    QVector<QPair<QString, double> > & getHillsReference();
    void setHills(const QVector<QPair<QString, double> > &newHills);
    QString getName() const;
    void setName(const QString &newName);
};

#endif // SEASONCALENDARPRESET_H
