#ifndef SEASONCALENDAR_H
#define SEASONCALENDAR_H

#include <QVector>
#include <QSharedPointer>
#include <QJsonObject>
#include "../competitions/CompetitionInfo.h"
#include "../utilities/Identifiable.h"

class SeasonCalendar : public Identifiable
{
public:
    SeasonCalendar(QString name = "");
    ~SeasonCalendar();

    void debugCalendar(CompetitionInfo *next) const;

    void fixCompetitionsClassifications();
    void fixCompetitionsHills(QVector<Hill *> *hillsList, Hill *defaultHill);
    void fixAdvancementCompetitions();
    void fixAdvancementClassifications();
    void updateCompetitionsQualifyingCompetitions();

    static SeasonCalendar getFromJson(QJsonObject json, IdentifiableObjectsStorage *storage, QMap<ulong, Identifiable *> *before120Map = nullptr);
    static QJsonObject getJsonObject(SeasonCalendar & calendar);
    static int getCompetitionMainIndex(QVector<CompetitionInfo *> & competitions, CompetitionInfo * competition);
    static CompetitionInfo * getMainCompetitionByIndex(QVector<CompetitionInfo *> & competitions, int index);
    static CompetitionInfo * getCompetitionByIndexAndType(QVector<CompetitionInfo *> & competitions, int index, int type);
    static CompetitionInfo * getCompetitionAfterCompetitionFilteredByType(QVector<CompetitionInfo *> & competitions, int actualIndex, int type, int howMany);

    bool getAllPlayed();
    int howManyCompetitionsPlayed();

private:
    QVector<CompetitionInfo *> competitions;
    QVector<Classification *> classifications;
    QString name;

public:
    QVector<CompetitionInfo *> getCompetitions() const;
    QVector<CompetitionInfo *>  & getCompetitionsReference();
    void setCompetitions(const QVector<CompetitionInfo *> &newCompetitions);
    QVector<Classification *> getClassifications() const;
    QVector<Classification *> &getClassificationsReference();
    void setClassifications(const QVector<Classification *> &newClassifications);
    QString getName() const;
    void setName(const QString &newName);
};

#endif // SEASONCALENDAR_H
