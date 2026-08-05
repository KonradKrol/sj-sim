#include "IndividualCompetitionManager.h"
#include "../../utilities/functions.h"
#include <QVector>
#include <random>

IndividualCompetitionManager::IndividualCompetitionManager() : AbstractCompetitionManager(CompetitionRules::Individual)
{
    connect(this, &IndividualCompetitionManager::actualStartListIndexChanged, this, [this](){
        if(roundsJumpers.count() > 0){
            actualJumper = roundsJumpers[actualRound - 1].at(actualStartListIndex);
            emit actualJumperChanged();
        }
    });
    KOManager = nullptr;
}

void IndividualCompetitionManager::updateCompetitorsAdvanceStatuses()
{
    int limit = 0;
    if(competitionRules->getRounds().count() > actualRound)
        limit = competitionRules->getRounds().at(actualRound).getCount();
    if(altQualifiersLimit > 0 && actualRound == competitionRules->getRounds().count())
        limit = altQualifiersLimit;

    results->updatePositions();
    for(auto & status : startListStatuses)
    {
        bool condition1 = competitionRules->getRoundsReference().count() > actualRound; //Istnieje następna runda
        bool condition2 = false;
        if(condition1 == true)
        {
            condition2 = competitionRules->getRoundsReference()[actualRound - 1].getKO() == false && competitionRules->getRoundsReference()[actualRound].getKO();// && competitionRules->getRoundsReference()[actualRound].getKoGroupsSelectionType() == CompetitionRules::Classic;
        }
        if(condition1 && condition2)
        {
            if(lastQualifiedResult != nullptr && status.getJumpStatus() == StartListCompetitorStatus::Finished){
                int lastQualifiedJumperIndexInResults = results->getIndexOfJumperResult(lastQualifiedResult->getJumper());

                if(results->getIndexOfJumperResult(status.getJumper()) <= lastQualifiedJumperIndexInResults){
                    status.setAdvanceStatus(StartListCompetitorStatus::SureAdvanced);
                }
            }
        }
        else if(KOManager != nullptr && actualRound != competitionRules->getRoundsReference().count()){
            if(KOManager->getStatusesReference().value(status.getJumper()) == KORoundManager::Winner)
                status.setAdvanceStatus(StartListCompetitorStatus::SureAdvanced);
            else if(KOManager->getStatusesReference().value(status.getJumper()) == KORoundManager::Waiting)
                status.setAdvanceStatus(StartListCompetitorStatus::Waiting);
            else if(KOManager->getLuckyLoserReference().contains(status.getJumper()))
            {
                bool allFinished = true;
                for(auto & status : startListStatuses)
                    if(status.getJumpStatus() == StartListCompetitorStatus::Unfinished)
                        allFinished = false;
                if(allFinished)
                    status.setAdvanceStatus(StartListCompetitorStatus::SureAdvanced);
                else
                    status.setAdvanceStatus(StartListCompetitorStatus::Waiting);
            }
            else
                status.setAdvanceStatus(StartListCompetitorStatus::SureDroppedOut);
        }
        else if(actualRound == competitionRules->getRoundsReference().count() && (altQualifiersLimit == 0 && actualRound != competitionRules->getRounds().count())){
            status.setAdvanceStatus(StartListCompetitorStatus::Waiting);
        }
        else if(getActualRoundJumpersReference().count() <= limit || status.getQualifiedBy95HSRule()){
            status.setAdvanceStatus(StartListCompetitorStatus::SureAdvanced);
        }
        else if(lastQualifiedResult == nullptr || status.getJumpStatus() == StartListCompetitorStatus::Unfinished)
            status.setAdvanceStatus(StartListCompetitorStatus::Waiting);
        else{
            if(results->getResultOfIndividualJumper(status.getJumper())->getPosition() <= lastQualifiedResult->getPosition()){
                status.setAdvanceStatus(StartListCompetitorStatus::SureAdvanced);
            }
            else{
                int lastWaiting = limit;
                results->sortInDescendingOrder();
                QVector<CompetitionSingleResult> * res = &results->getResultsReference();
                int lastPosition = 0;
                for(int i = res->count() - 1; i>=0; i--){
                    if(res->at(i).getPosition() < limit){
                        if(res->at(i).getPosition() == lastPosition) //mamy egzekwo
                        {
                            int exaequos = 0;
                            for(int i = res->count() - 1; i>=0; i--){
                                if(res->at(i).getPosition() == lastPosition){
                                    exaequos++;
                                    lastPosition = res->at(i).getPosition();
                                }
                                else
                                    break;
                            }
                            if(exaequos >= (competitionRules->getRounds().at(actualRound - 1).getCount() - lastPosition))
                                lastWaiting += exaequos;
                        }
                    }
                }
                if(results->getResultOfIndividualJumper(status.getJumper())->getPosition() <= lastWaiting)
                    status.setAdvanceStatus(StartListCompetitorStatus::Waiting);
                else
                    status.setAdvanceStatus(StartListCompetitorStatus::SureDroppedOut);
            }
        }
    }
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
    CompetitionResults * results = &competition->getAdvancementCompetition()->getResultsReference();

    int lastQualifiedPosition = competition->getRulesPointer()->getRoundsReference()[0].getCount();
    int koLastQualifiedPosition = lastQualifiedPosition;

    bool exaequo = true;
    for(auto & res : results->getResultsReference()){ // Sprawdzamy, czy wystąpiło ex aequo.
        if(res.getPosition() == lastQualifiedPosition){
            exaequo = false;
            break;
        }
    }
    if(exaequo == true) // Jeżeli wystąpiło ex aequo
    {
        for(auto & res : results->getResultsReference()){
            if(res.getPosition() > lastQualifiedPosition){
                lastQualifiedPosition = res.getPosition();
                break;
            }
        }
    }

    QVector<Jumper *> toReturn;

    if(competition->getRulesPointer()->getRoundsReference()[0].getKO())
    {
        for(int i=koLastQualifiedPosition - 1; i>=0; i--)
        {
            toReturn.push_back(results->getResultByIndex(i)->getJumper());
        }
    }
    else{
        for(auto & jumper : jumpers)
        {
            if(results->getResultOfIndividualJumper(jumper) != nullptr)
                if(results->getResultOfIndividualJumper(jumper)->getPosition() <= lastQualifiedPosition)
                    toReturn.push_back(jumper);
        }
    }
    return toReturn;
}

QVector<Jumper *> IndividualCompetitionManager::getFilteredJumpersByClassification(CompetitionInfo *competition, Classification *classification, QVector<Jumper *> &jumpers)
{
    int lastQualifiedPosition = competition->getRulesPointer()->getRoundsReference()[0].getCount();

    bool exaequo = true;
    for(auto & res : classification->getResultsReference()){ // Sprawdzamy, czy wystąpiło ex aequo.
        if(res->getPosition() == lastQualifiedPosition){
            exaequo = false;
            break;
        }
    }
    if(exaequo == true)
    {
        for(auto & res : classification->getResultsReference()){
            if(res->getPosition() > lastQualifiedPosition){
                lastQualifiedPosition = res->getPosition();
                break;
            }
        }
    }

    QVector<Jumper *> toReturn;
    for(auto & jumper : jumpers)
    {
        if(classification->getResultOfIndividualJumper(jumper) != nullptr)
            if(classification->getResultOfIndividualJumper(jumper)->getPosition() <= lastQualifiedPosition)
                toReturn.push_back(jumper);
    }
    return toReturn;
}

void IndividualCompetitionManager::setStartListOrderByClassification(QVector<Jumper *> & jumpers, Classification *classification)
{
    if(classification->getClassificationType() == Classification::Individual)
    {
        QVector<Jumper *> tempJumpers;
        for(auto & result : classification->getResultsReference())
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

void IndividualCompetitionManager::setupNextRound(QVector<KOGroup> manualGroups)
{
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
