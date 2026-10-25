#include "IndividualCompetitionManager.h"
#include "../CompetitionAdvancement.h"
#include "../../utilities/functions.h"
#include <QVector>
#include <random>

IndividualCompetitionManager::IndividualCompetitionManager() : AbstractCompetitionManager(CompetitionRules::Individual)
{
    connect(this, &IndividualCompetitionManager::actualStartListIndexChanged, this, [this](){
        const int roundIndex = actualRound - 1;
        if(roundIndex < 0 || roundIndex >= roundsJumpers.count()
            || actualStartListIndex < 0 || actualStartListIndex >= roundsJumpers.at(roundIndex).count())
            return;
        actualJumper = roundsJumpers.at(roundIndex).at(actualStartListIndex);
        emit actualJumperChanged();
    });
    KOManager = nullptr;
}

void IndividualCompetitionManager::updateCompetitorsAdvanceStatuses()
{
    if(!results || !competitionRules || actualRound<1 || actualRound>roundsJumpers.size())return;
    results->sortInDescendingOrder();
    const int limit=advancementLimit();
    const bool hasNextRound=actualRound<effectiveRoundsCount();
    const bool includeTies=hasNextRound?!competitionRules->getRoundsReference()[actualRound].getKO():qualificationIncludesTies();
    const bool knockOut=KOManager && hasNextRound && competitionRules->getRoundsReference()[actualRound-1].getKO();
    if(knockOut)KOManager->updateStatuses();
    QVector<CompetitionAdvancement::Entry> entries;
    for(const auto &status:startListStatuses) {
        CompetitionAdvancement::Entry entry;
        entry.competitor=status.getJumper();entry.round=entry.maximumRound=actualRound;
        entry.pending=status.getJumpStatus()==StartListCompetitorStatus::Unfinished;
        entry.excluded=status.getJumpStatus()==StartListCompetitorStatus::Dns || status.getJumpStatus()==StartListCompetitorStatus::Dsq;
        entry.protectedByDistance=status.getQualifiedBy95HSRule();
        if(auto *result=results->getResultOfIndividualJumper(status.getJumper())) {
            entry.points=result->getRankingPoints();
            entry.order=results->getIndexOfJumperResult(status.getJumper());
        }
        entries.append(entry);
    }
    for(int i=0;i<startListStatuses.size();++i) {
        const int state=knockOut?KOManager->advancementStatus(startListStatuses[i].getJumper())
            :CompetitionAdvancement::status(entries[i],entries,limit,includeTies);
        startListStatuses[i].setAdvanceStatus(state);
    }
    updateQualificationStatuses();
}

//W filtered jumpers bierzemy zawodnikóW z uprzednio ustalonych grup. Grupy ustalamy w innej funkcji, w getFilteredGroups

QVector<KOGroup> IndividualCompetitionManager::getFilteredGroupsForNextRound()
{
    QVector<KOGroup> groups;
    QVector<Jumper *> jumpers = getActualRoundJumpersReference();
    //std::reverse(jumpers.begin(), jumpers.end());

    if(competitionRules->getRoundsReference()[actualRound - 1].getKO() == true)
    {
        int selectionType = competitionRules->getRoundsReference()[actualRound - 1].getKoGroupsSelectionType();
        if(selectionType != CompetitionRules::Manual)
        {
            groups = KOGroup::constructKOGroups(&competitionRules->getRoundsReference()[actualRound - 1], &jumpers, selectionType, competitionInfo);
        }
    }
    return groups;
}

QVector<Jumper *> IndividualCompetitionManager::getFilteredJumpersForNextRound(bool checkKO)
{
    QVector<Jumper *> jumpers;
    int i = 1;
    for(auto & status : startListStatuses){
        i++;
    }
    if(checkKO){
        if(competitionRules->getRoundsReference()[actualRound - 1].getKO() == true)
        {
            int selectionType = competitionRules->getRoundsReference()[actualRound - 1].getKoGroupsSelectionType();
            return KOGroup::getJumpersFromGroups(getRoundsKOGroupsReference()[actualRound - 1]);
        }
    }

    if(competitionRules->getRoundsReference()[actualRound - 1].getSortStartList() == true){
        results->sortInAscendingOrder();
        QVector<CompetitionSingleResult *> roundResults;
        for(auto & res : results->getResultsReference()){
            if(StartListCompetitorStatus::getStatusOfJumper(res.getJumper(), startListStatuses) != nullptr)
                roundResults.push_back(&res);
        }

        for(auto & res : roundResults){
            if(StartListCompetitorStatus::getStatusOfJumper(res->getJumper(), startListStatuses)->getAdvanceStatus() == StartListCompetitorStatus::SureAdvanced)
                jumpers.push_back(res->getJumper());
        }
        results->sortInDescendingOrder();
    }
    else{
        for(auto & status : startListStatuses){
            if(status.getAdvanceStatus() == StartListCompetitorStatus::SureAdvanced)
                jumpers.push_back(status.getJumper());
        }
    }
    return jumpers;
}

QVector<Jumper *> IndividualCompetitionManager::getFilteredJumpersAfterQualifications(CompetitionInfo *competition, QVector<Jumper *> jumpers)
{
    QVector<Jumper *> qualified;
    if(!competition || !competition->getAdvancementCompetition() || competition->getRulesPointer()->getRoundsReference().isEmpty())return qualified;
    auto *source = competition->getAdvancementCompetition();
    const auto &firstRound = competition->getRulesPointer()->getRoundsReference().first();
    const int limit = qMax(0,firstRound.getCount());
    if(limit == 0)return qualified;
    CompetitionResults standings = source->getResultsReference();
    standings.sortInDescendingOrder();
    const auto eligible = [](const CompetitionSingleResult &result) {
        const auto jumps = result.getJumps();
        return jumps.isEmpty() || (!jumps.last().getDNS() && !jumps.last().getDSQ());
    };
    auto &eligibleResults=standings.getResultsReference();
    for(int i=eligibleResults.size()-1;i>=0;--i)if(!eligible(eligibleResults[i]))eligibleResults.removeAt(i);
    standings.sortInDescendingOrder();
    if(firstRound.getKO()) {
        for(const auto &result : qAsConst(standings.getResultsReference())) {
            auto *jumper = result.getJumper();
            if(jumper && jumpers.contains(jumper) && eligible(result) && !qualified.contains(jumper))qualified.append(jumper);
            if(qualified.size() >= limit)break;
        }
        std::reverse(qualified.begin(),qualified.end());
        return qualified;
    }
    for(auto *jumper : qAsConst(jumpers)) {
        auto *result = standings.getResultOfIndividualJumper(jumper);
        if(!result || !eligible(*result) || qualified.contains(jumper))continue;
        bool protectedFall = false;
        const auto &attempts = result->getJumpsReference();
        if(source->getRulesPointer()->getHas95HSRule() && source->getHill() && !attempts.isEmpty()) {
            const auto &jump = attempts.last();
            protectedFall = jump.getLanding().getType() == Landing::Fall &&
                JumpSimulator::isJumperBeat95HSPercents(jump.getDistance(),source->getHill()->getHSPoint());
        }
        if((result->getPosition() > 0 && result->getPosition() <= limit) || protectedFall)qualified.append(jumper);
    }
    return qualified;
}

QVector<Jumper *> IndividualCompetitionManager::getFilteredJumpersByClassification(CompetitionInfo *competition, Classification *classification, QVector<Jumper *> &jumpers)
{
    QVector<Jumper *> qualified;
    if(!competition || !classification || competition->getRulesPointer()->getRoundsReference().isEmpty())return qualified;
    const auto &round = competition->getRulesPointer()->getRoundsReference().first();
    const int limit = qMax(0,round.getCount());
    if(limit == 0)return qualified;
    const auto ranked = classification->getRankedResults();
    if(round.getKO()) {
        QVector<Jumper *> selected;
        for(auto *result : ranked) {
            auto *jumper = result->getJumper();
            if(jumper && jumpers.contains(jumper) && !selected.contains(jumper))selected.append(jumper);
            if(selected.size() >= limit)break;
        }
        for(auto *jumper : qAsConst(jumpers))if(selected.contains(jumper) && !qualified.contains(jumper))qualified.append(jumper);
    } else {
        for(auto *jumper : qAsConst(jumpers)) {
            auto *result = classification->getResultOfIndividualJumper(jumper);
            if(result && ranked.contains(result) && result->getPosition() > 0 && result->getPosition() <= limit && !qualified.contains(jumper))qualified.append(jumper);
        }
    }
    return qualified;
}

void IndividualCompetitionManager::setStartListOrderByClassification(QVector<Jumper *> & jumpers, Classification *classification)
{
    if(classification->getClassificationType() == Classification::Individual)
    {
        QVector<Jumper *> tempJumpers;
        for(auto & result : classification->getRankedResults())
        {
            if(jumpers.contains(result->getJumper()) && result->getPointsSum() != 0)
                tempJumpers.push_back(result->getJumper());
        }

        QVector<Jumper *> others;
        std::reverse(jumpers.begin(), jumpers.end());
        for(auto & jumper : jumpers)
        {
            if(tempJumpers.contains(jumper) == false)
                others.push_back(jumper);
        }
        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(others.begin(), others.end(), g);

        tempJumpers.append(others);

        std::reverse(tempJumpers.begin(), tempJumpers.end());
        jumpers = tempJumpers;
    }
}

void IndividualCompetitionManager::setStartListOrderByCompetitionResults(QVector<Jumper *> & jumpers, CompetitionInfo *competition)
{
    if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
    {
        QVector<Jumper *> tempJumpers;
        for(auto & result : competition->getResultsReference().getResultsReference())
        {
            if(jumpers.contains(result.getJumper()))
                tempJumpers.push_back(result.getJumper());
        }
        std::reverse(jumpers.begin(), jumpers.end());
        for(auto & jumper : jumpers)
        {
            if(tempJumpers.contains(jumper) == false)
                tempJumpers.push_back(jumper);
        }

        std::reverse(tempJumpers.begin(), tempJumpers.end());
        jumpers = tempJumpers;
    }
}

void IndividualCompetitionManager::setStartListOrderByDefault(QVector<Jumper *> *jumpersList, QVector<Jumper *> &startList)
{
    QVector<Jumper *> tempJumpers;
    for(auto & jumper : *jumpersList)
    {
        if(startList.contains(jumper))
            tempJumpers.push_back(jumper);
    }
    startList = tempJumpers;
}

void IndividualCompetitionManager::setStartListOrderRandomly(QVector<Jumper *> &startList)
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(startList.begin(), startList.end(), g);
}

void IndividualCompetitionManager::setStartListOrderByCountries(QVector<QString> countries, QVector<Jumper *> &startList)
{
    QVector<Jumper *> tempJumpers;
    for(auto & country : countries)
    {
        for(auto & j : Team::getJumpersFilteredByCountryCode(startList, country))
        {
            tempJumpers.push_back(j);
        }
    }
    startList = tempJumpers;
}

void IndividualCompetitionManager::setStartListOrderByJumpersList(SaveJumpersList *list, QVector<Jumper *> &startList)
{
    QVector<Jumper *> tempJumpers;
    for(auto & jumperFromList : list->getJumpersReference())
    {
        if(startList.contains(jumperFromList) == true)
        {
            tempJumpers.push_back(jumperFromList);
        }
    }
    int i=0;
    for(auto & j : startList)
    {
        if(tempJumpers.contains(j) == false){
            tempJumpers.insert(i, j);
            i++;
        }
    }
    startList = tempJumpers;
}

QVector<QVector<KOGroup> *> IndividualCompetitionManager::getRoundsKOGroups() const
{
    return roundsKOGroups;
}

QVector<QVector<KOGroup> *> &IndividualCompetitionManager::getRoundsKOGroupsReference()
{
    return roundsKOGroups;
}

void IndividualCompetitionManager::setRoundsKOGroups(const QVector<QVector<KOGroup> *> &newRoundsKOGroups)
{
    roundsKOGroups = newRoundsKOGroups;
}

KORoundManager *IndividualCompetitionManager::getKOManager() const
{
    return KOManager;
}

void IndividualCompetitionManager::setKOManager(KORoundManager *newKOManager)
{
    KOManager = newKOManager;
}


QVector<StartListCompetitorStatus> IndividualCompetitionManager::getStartListStatuses() const
{
    return startListStatuses;
}

bool IndividualCompetitionManager::isLuckyLoser(Jumper *jumper) const {
    const bool currentKO=competitionRules && actualRound>0 && actualRound<=effectiveRoundsCount() && competitionRules->getRoundsReference()[actualRound-1].getKO();
    return currentKO ? KOManager && KOManager->getLuckyLosers().contains(jumper) : previousLuckyLosers.contains(jumper);
}

void IndividualCompetitionManager::setupNextRound(QVector<KOGroup> manualGroups)
{
    previousLuckyLosers.clear();
    if(KOManager){KOManager->updateStatuses();for(auto *jumper:KOManager->getLuckyLosers())if(KOManager->advancementStatus(jumper)==StartListCompetitorStatus::SureAdvanced)previousLuckyLosers.insert(jumper);}
    updateCompetitorsAdvanceStatuses();
    actualRound++; //Przechodzi do następnej rundy
    roundsJumpers.push_back(getFilteredJumpersForNextRound());

    if(manualGroups.count() > 0){
        competitionInfo->getRoundsKOGroupsReference().push_back(manualGroups);
        roundsKOGroups.push_back(&competitionInfo->getRoundsKOGroupsReference().last());
    }
    else{
        competitionInfo->getRoundsKOGroupsReference().push_back(getFilteredGroupsForNextRound());
        roundsKOGroups.push_back(&competitionInfo->getRoundsKOGroupsReference().last());
    }

    if(competitionRules->getRoundsReference()[actualRound - 1].getKO())
        roundsJumpers.last() = getFilteredJumpersForNextRound(true);
    setActualStartListIndex(0);
    roundsStartingGates.push_back(actualGate);
    setupStartListStatusesForActualRound();
    for(auto *jumper : getActualRoundJumpersReference())
        if(auto *result=results->getResultOfIndividualJumper(jumper))result->updatePointsSum(actualRound);
    results->sortInDescendingOrder();
}

void IndividualCompetitionManager::setupStartListStatusesForActualRound()
{
    startListStatuses.clear();
    for(auto & jumper : getActualRoundJumpersReference()){
        startListStatuses.push_back(StartListCompetitorStatus(jumper));
    }
}

QVector<QVector<Jumper *> > IndividualCompetitionManager::getRoundsJumpers() const
{
    return roundsJumpers;
}

QVector<QVector<Jumper *> > & IndividualCompetitionManager::getRoundsJumpersReference()
{
    return roundsJumpers;
}

void IndividualCompetitionManager::setRoundsJumpers(const QVector<QVector<Jumper *> > &newRoundsJumpers)
{
    roundsJumpers = newRoundsJumpers;
}

void IndividualCompetitionManager::setFirstRoundJumpers(QVector<Jumper *> jumpers)
{
    if(roundsJumpers.count() == 0)
        roundsJumpers.append(jumpers);
    else
        roundsJumpers[0] = jumpers;
}
