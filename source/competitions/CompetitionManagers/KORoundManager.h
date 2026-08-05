#ifndef KOROUNDMANAGER_H
#define KOROUNDMANAGER_H

#include <QVector>
#include "../KOSystem/KOGroup.h"
#include "../StartListCompetitorStatus.h"
#include "../CompetitionResults.h"

class IndividualCompetitionManager;

class KORoundManager
{
public:
    KORoundManager(QVector<KOGroup> * groups, CompetitionResults * results, RoundInfo * roundInfo, IndividualCompetitionManager * indManager);

    enum KOJumperStatus{
        Winner,
        Loser,
        Waiting
    };

    void updateStatuses();
    void updateLuckyLosers();
    void updateJumpersSortedByResults();
    void updateActualGroup(Jumper * actualJumper);

private:
    IndividualCompetitionManager * indManager;
    QVector<KOGroup> * groups;
    KOGroup * actualGroup;

    QMap<Jumper *, int> statuses;
    QVector<Jumper *> luckyLosers;
    int luckyLosersCount;

    CompetitionResults * results;
    RoundInfo * roundInfo;
    QVector<Jumper *> jumpersSortedByResults;
public:
    QMap<Jumper *, int> getStatuses() const;
    QMap<Jumper *, int> & getStatusesReference();
    void setStatuses(const QMap<Jumper *, int> &newStatuses);
    QVector<Jumper *> getLuckyLosers() const;
    QVector<Jumper *>&  getLuckyLoserReference();
    void setLuckyLosers(const QVector<Jumper *> &newLuckyLosers);
    QVector<Jumper *> getJumpersSortedByResults() const;
    QVector<Jumper *> & getJumpersSortedByResultsReference();
    void setJumpersSortedByResults(const QVector<Jumper *> &newJumpersSortedByResults);
    CompetitionResults *getResults() const;
    void setResults(CompetitionResults *newResults);
    RoundInfo *getRoundInfo() const;
    void setRoundInfo(RoundInfo *newRoundInfo);
    QVector<KOGroup> *getGroups() const;
    void setGroups(QVector<KOGroup> *newGroups);
    int getLuckyLosersCount() const;
    void setLuckyLosersCount(int newLuckyLosersCount);
    KOGroup *getActualGroup() const;
    void setActualGroup(KOGroup *newActualGroup);
    IndividualCompetitionManager *getIndManager() const;
    void setIndManager(IndividualCompetitionManager *newIndManager);
};

#endif // KOROUNDMANAGER_H
