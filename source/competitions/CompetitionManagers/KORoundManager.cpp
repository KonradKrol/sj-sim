#include "KORoundManager.h"

#include "IndividualCompetitionManager.h"
#include "../CompetitionAdvancement.h"

KORoundManager::KORoundManager(QVector<KOGroup> *groups, CompetitionResults *results, RoundInfo *roundInfo, IndividualCompetitionManager *indManager)
    : indManager(indManager), groups(groups), actualGroup(nullptr), luckyLosersCount(0), results(results), roundInfo(roundInfo)
{

}

namespace {
CompetitionAdvancement::Entry koEntry(Jumper *jumper,IndividualCompetitionManager *manager,CompetitionResults *results) {
    CompetitionAdvancement::Entry entry;entry.competitor=jumper;
    entry.round=entry.maximumRound=manager->getActualRound();
    const auto *status=StartListCompetitorStatus::getStatusOfJumper(jumper,manager->getStartListStatusesReference());
    auto *result=results->getResultOfIndividualJumper(jumper);
    if(result)entry.points=result->getRankingPoints();
    entry.pending=status?status->getJumpStatus()==StartListCompetitorStatus::Unfinished:!result || result->getJumpsReference().size()<manager->getActualRound();
    entry.excluded=status && (status->getJumpStatus()==StartListCompetitorStatus::Dsq || status->getJumpStatus()==StartListCompetitorStatus::Dns);
    if(result && !entry.pending && !result->getJumpsReference().isEmpty())entry.excluded=entry.excluded || result->getJumpsReference().last().getDSQ() || result->getJumpsReference().last().getDNS();
    return entry;
}
}

void KORoundManager::updateStatuses()
{
    statuses.clear();
    if(!groups || !results || !roundInfo || !indManager)return;
    for(const auto &group:*groups) {
        QVector<CompetitionAdvancement::Entry> entries;
        for(auto *jumper:group.getJumpers())if(jumper)entries.append(koEntry(jumper,indManager,results));
        for(const auto &entry:entries) {
            const int state=CompetitionAdvancement::status(entry,entries,qMax(0,roundInfo->getAdvancingFromKOGroup()));
            statuses.insert(static_cast<Jumper *>(const_cast<void *>(entry.competitor)),state==StartListCompetitorStatus::SureAdvanced?Winner:state==StartListCompetitorStatus::SureDroppedOut?Loser:Waiting);
        }
    }
    updateLuckyLosers();
}

int KORoundManager::advancementStatus(Jumper *jumper) const
{
    if(!jumper || !groups || !results || !roundInfo || !indManager)return StartListCompetitorStatus::Waiting;
    const auto candidate=koEntry(jumper,indManager,results);
    if(candidate.excluded)return StartListCompetitorStatus::SureDroppedOut;
    if(candidate.pending)return StartListCompetitorStatus::Waiting;
    const int groupStatus=statuses.value(jumper,Waiting);
    if(groupStatus==Winner)return StartListCompetitorStatus::SureAdvanced;
    if(groupStatus==Waiting)return StartListCompetitorStatus::Waiting;
    if(getLuckyLosersCount()<=0)return StartListCompetitorStatus::SureDroppedOut;
    int bestHigher=0,worstHigher=0;
    for(const auto &group:*groups) {
        int potentialHigher=0;
        for(auto *other:group.getJumpers()) {
            if(!other || other==jumper)continue;
            const auto entry=koEntry(other,indManager,results);
            if(entry.excluded)continue;
            const bool higher=entry.points>candidate.points;
            if(entry.pending || higher)++potentialHigher;
            if(!entry.pending && higher && statuses.value(other,Waiting)==Loser)++bestHigher;
        }
        worstHigher+=qMax(0,potentialHigher-qMax(0,roundInfo->getAdvancingFromKOGroup()));
    }
    if(bestHigher>=getLuckyLosersCount())return StartListCompetitorStatus::SureDroppedOut;
    if(worstHigher<getLuckyLosersCount())return StartListCompetitorStatus::SureAdvanced;
    return StartListCompetitorStatus::Waiting;
}

bool KORoundManager::wouldSurelyAdvance(Jumper *jumper,qint64 points) const
{
    if(!jumper || !groups || !results || !roundInfo || !indManager)return false;
    QMap<Jumper *,CompetitionAdvancement::Entry> entries;
    QMap<Jumper *,int> groupStates;
    bool found=false;
    const int quota=qMax(0,roundInfo->getAdvancingFromKOGroup());
    for(const auto &group:*groups){
        QVector<CompetitionAdvancement::Entry> field;
        for(auto *member:group.getJumpers())if(member){
            auto entry=koEntry(member,indManager,results);
            if(member==jumper){entry.points=points;entry.pending=false;entry.excluded=false;found=true;}
            field.append(entry);entries.insert(member,entry);
        }
        for(const auto &entry:field){
            const int state=CompetitionAdvancement::status(entry,field,quota);
            groupStates.insert(static_cast<Jumper *>(const_cast<void *>(entry.competitor)),state);
        }
    }
    if(!found)return false;
    if(groupStates.value(jumper)==StartListCompetitorStatus::SureAdvanced)return true;
    if(getLuckyLosersCount()<=0)return false;
    // Every unresolved group can still contribute losers stronger than this score.
    int worstHigher=0;
    for(const auto &group:*groups){
        int higher=0;
        for(auto *member:group.getJumpers())if(member && member!=jumper){const auto entry=entries.value(member);if(!entry.excluded && (entry.pending || entry.points>points))++higher;}
        worstHigher+=qMax(0,higher-quota);
    }
    return worstHigher<getLuckyLosersCount();
}

double KORoundManager::guaranteedAdvancePoints(Jumper *jumper) const
{
    if(!groups || !results || !indManager || !jumper)return -1;
    qint64 upper=0;
    for(const auto &group:*groups)for(auto *member:group.getJumpers())if(auto *result=results->getResultOfIndividualJumper(member))upper=qMax(upper,result->getRankingPoints());
    ++upper;
    if(!wouldSurelyAdvance(jumper,upper))return -1;
    qint64 lower=0;
    while(lower<upper){const qint64 middle=lower+(upper-lower)/2;if(wouldSurelyAdvance(jumper,middle))upper=middle;else lower=middle+1;}
    return lower/10.;
}

void KORoundManager::updateLuckyLosers()
{
    luckyLosers.clear();
    if(getLuckyLosersCount()<=0)return;
    QVector<Jumper *> losers;
    for(auto & key : statuses.keys())
        if(statuses.value(key) == KORoundManager::Loser && !koEntry(key,indManager,results).excluded)
            losers.push_back(key);
    results->sortJumpersByResults(losers);

    QVector<int> losersPositions = results->getJumpersPositions(&losers);
    for(int i=0; i<losers.count(); i++)
    {
        if(losersPositions[i] <= getLuckyLosersCount())
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
    if(!roundInfo || !roundInfo->getKO() || !indManager || indManager->getActualRound()>=indManager->effectiveRoundsCount())return 0;
    return qMax(0,luckyLosersCount);
}

void KORoundManager::setLuckyLosersCount(int newLuckyLosersCount)
{
    luckyLosersCount = qMax(0,newLuckyLosersCount);
    if(getLuckyLosersCount()==0)luckyLosers.clear();
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
    return getLuckyLosersCount()>0?luckyLosers:QVector<Jumper *>();
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
