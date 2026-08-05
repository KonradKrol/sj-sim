#include "KORoundManager.h"

#include "IndividualCompetitionManager.h"

KORoundManager::KORoundManager(QVector<KOGroup> *groups, CompetitionResults *results, RoundInfo *roundInfo, IndividualCompetitionManager *indManager)
    : groups(groups), results(results), roundInfo(roundInfo), indManager(indManager)
{

}

void KORoundManager::updateStatuses()
{
    statuses.clear();
    for(auto & group : *groups)
    {
        for(auto & jumper : group.getJumpersReference())
        {
            QVector<Jumper *> sortedGroupJumpers = group.getJumpersReference();
            results->sortJumpersByResults(sortedGroupJumpers);

            if(sortedGroupJumpers.count() > 0)
            {
                int remaining = 0;
                for(auto & jumper : group.getJumpersReference())
                {
                    if(results->getResultOfIndividualJumper(jumper) != nullptr)
                    {
                        if(results->getResultOfIndividualJumper(jumper)->getJumpsReference().count() < indManager->getActualRound())
                            remaining++;
                    }
                    else
                        remaining++;
                }
                QVector<int> positions = results->getJumpersPositions(&sortedGroupJumpers);
                if(positions[sortedGroupJumpers.indexOf(jumper)] + remaining <= roundInfo->getAdvancingFromKOGroup())
                    statuses.insert(jumper, KORoundManager::Winner);
                else if(positions[sortedGroupJumpers.indexOf(jumper)] > roundInfo->getAdvancingFromKOGroup())
                    statuses.insert(jumper, KORoundManager::Loser);
                else
                    statuses.insert(jumper, KORoundManager::Waiting);
            }
        }
    }
    updateLuckyLosers();
}

void KORoundManager::updateLuckyLosers()
{
    QVector<Jumper *> losers;
    for(auto & key : statuses.keys())
        if(statuses.value(key) == KORoundManager::Loser)
            losers.push_back(key);
    results->sortJumpersByResults(losers);

    luckyLosers.clear();
    QVector<int> losersPositions = results->getJumpersPositions(&losers);
    for(int i=0; i<losers.count(); i++)
    {
        if(losersPositions[i] <= luckyLosersCount)
            luckyLosers.push_back(losers[i]);
        else
            break; //Przerywamy bo skoro tu nie spełniało wymogów to nie ma szans że później będzie jako że loserzy są posortowani
    }
}

void KORoundManager::updateJumpersSortedByResults()
{
    jumpersSortedByResults.clear();
    for(auto & res : results->getResultsReference())
        jumpersSortedByResults.push_back(res.getJumper());

    /*for(auto & group : *groups)
        for(auto & jumper : group.getJumpersReference())
            if(jumpersSortedByResults.contains(jumper) == false)
                jumpersSortedByResults.push_back(jumper);*/
}

void KORoundManager::updateActualGroup(Jumper *actualJumper)
{
    for(auto & group : *groups)
    {
        if(group.getJumpersReference().contains(actualJumper))
            actualGroup = &group;
    }
}

IndividualCompetitionManager *KORoundManager::getIndManager() const
{
    return indManager;
}

void KORoundManager::setIndManager(IndividualCompetitionManager *newIndManager)
{
    indManager = newIndManager;
}

KOGroup *KORoundManager::getActualGroup() const
{
    return actualGroup;
}

void KORoundManager::setActualGroup(KOGroup *newActualGroup)
{
    actualGroup = newActualGroup;
}

int KORoundManager::getLuckyLosersCount() const
{
    return luckyLosersCount;
}

void KORoundManager::setLuckyLosersCount(int newLuckyLosersCount)
{
    luckyLosersCount = newLuckyLosersCount;
}

QVector<KOGroup> *KORoundManager::getGroups() const
{
    return groups;
}

void KORoundManager::setGroups(QVector<KOGroup> *newGroups)
{
    groups = newGroups;
}

RoundInfo *KORoundManager::getRoundInfo() const
{
    return roundInfo;
}

void KORoundManager::setRoundInfo(RoundInfo *newRoundInfo)
{
    roundInfo = newRoundInfo;
}

CompetitionResults *KORoundManager::getResults() const
{
    return results;
}

void KORoundManager::setResults(CompetitionResults *newResults)
{
    results = newResults;
}

QVector<Jumper *> KORoundManager::getJumpersSortedByResults() const
{
    return jumpersSortedByResults;
}

QVector<Jumper *> &KORoundManager::getJumpersSortedByResultsReference()
{
    return jumpersSortedByResults;
}

void KORoundManager::setJumpersSortedByResults(const QVector<Jumper *> &newJumpersSortedByResults)
{
    jumpersSortedByResults = newJumpersSortedByResults;
}

QVector<Jumper *> KORoundManager::getLuckyLosers() const
{
    return luckyLosers;
}

QVector<Jumper *> &KORoundManager::getLuckyLoserReference()
{
    return luckyLosers;
}

void KORoundManager::setLuckyLosers(const QVector<Jumper *> &newLuckyLosers)
{
    luckyLosers = newLuckyLosers;
}

QMap<Jumper *, int> KORoundManager::getStatuses() const
{
    return statuses;
}

QMap<Jumper *, int> &KORoundManager::getStatusesReference()
{
    return statuses;
}

void KORoundManager::setStatuses(const QMap<Jumper *, int> &newStatuses)
{
    statuses = newStatuses;
}
