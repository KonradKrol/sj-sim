#include "TeamCompetitionManager.h"
#include "../../utilities/functions.h"
#include <random>

TeamCompetitionManager::TeamCompetitionManager() : AbstractCompetitionManager(CompetitionRules::Team)
{
    actualGroup = 0;
    connect(this, &TeamCompetitionManager::actualStartListIndexChanged, this, [this](){
        if(roundsTeams.count() > 0){
            actualJumper = startListStatuses[actualStartListIndex].getJumper();
            emit actualJumperChanged();
            actualTeam = roundsTeams[actualRound - 1][actualStartListIndex];
            emit actualTeamChanged();
        }
    });
}


void TeamCompetitionManager::setActualTeamToActualJumperTeam()
{
    for(auto & team : getActualRoundTeamsReference())
    {
        if(actualJumper == team->getJumpersReference()[actualGroup - 1])
            actualTeam = team;
    }
}

void TeamCompetitionManager::setupNextRound()
{
    actualRound++; //Przechodzi do następnej rundy
    roundsTeams.push_back(getFilteredTeamsForNextRound());
    setActualGroup(0);
    setupNextGroup();
    roundsStartingGates.push_back(actualGate);
}

void TeamCompetitionManager::setupStartListStatusesForActualRound(bool setupBeforeCompetition)
{
    if(setupBeforeCompetition != true)
        sortActualRoundJumpersForActualGroup();

    startListStatuses.clear();
    for(auto & team : getActualRoundTeamsReference()){
        startListStatuses.push_back(StartListCompetitorStatus(team->getJumpersReference()[actualGroup - 1]));
    }
}

void TeamCompetitionManager::sortActualRoundJumpersForActualGroup()
{
    QVector<Team *> teams;
    int sortAfterGroups = competitionRules->getRounds().at(actualRound - 1).getSortAfterGroups();
    if(sortAfterGroups == RoundInfo::EachGroup  || (sortAfterGroups == RoundInfo::BeforeLastGroup && actualGroup == competitionRules->getJumpersInTeamCount()) || (sortAfterGroups == RoundInfo::AfterFirstGroup && actualGroup == 2))
    {
        results->sortInDescendingOrder();
        for(auto & res : results->getResultsReference()){
            if(MyFunctions::vectorContains(getActualRoundTeamsReference(), res.getTeam())){
                teams.push_front(res.getTeam());
            }
        }
        roundsTeams[actualRound - 1] = teams;
    }
}

void TeamCompetitionManager::updateCompetitorsAdvanceStatuses()
{
    int limit = 0;
    if(competitionRules->getRounds().count() > actualRound)
        limit = competitionRules->getRounds().at(actualRound).getCount();
    if(altQualifiersLimit > 0 && actualRound == competitionRules->getRounds().count())
        limit = altQualifiersLimit;

    teamsAdvanceStatuses.clear();
    results->updatePositions();
    for(auto & team : getActualRoundTeamsReference())
    {
        int status = StartListCompetitorStatus::Waiting;
        if(actualRound == competitionRules->getRounds().count())
            status = StartListCompetitorStatus::Waiting;
        else if(getActualRoundTeamsReference().count() <= limit){
            status = StartListCompetitorStatus::SureAdvanced;
        }

        else if(lastQualifiedResult == nullptr || results->getResultOfTeam(team) == nullptr)
            status = StartListCompetitorStatus::Waiting;
        else{
            if(results->getResultOfTeam(team)->getPosition() <= lastQualifiedResult->getPosition()){
                status = StartListCompetitorStatus::SureAdvanced;
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
                if(results->getResultOfTeam(team)->getPosition() <= lastWaiting)
                    status = StartListCompetitorStatus::Waiting;
                else
                    status = StartListCompetitorStatus::SureDroppedOut;
            }
        }
        teamsAdvanceStatuses.push_back(QPair<Team *, int>(team, status));
    }
}

int TeamCompetitionManager::getAdvanceStatusOfTeam(Team *team)
{
    for(auto & status : teamsAdvanceStatuses){
        if(status.first == team)
            return status.second;
    }
    return (-1); // nie znaleziono
}

QVector<Team *> TeamCompetitionManager::getFilteredTeamsForNextRound()
{
    QVector<Team *> teams;
    if(competitionRules->getRounds().at(actualRound - 1).getSortStartList() == true){
        results->sortInDescendingOrder();
        QVector<CompetitionSingleResult *> roundResults;
        for(auto & res : results->getResultsReference()){
            if(getAdvanceStatusOfTeam(res.getTeam()) != (-1))
                roundResults.push_back(&res);
        }

        for(auto & res : roundResults){
            if(getAdvanceStatusOfTeam(res->getTeam()) == StartListCompetitorStatus::SureAdvanced)
                teams.push_front(res->getTeam());
        }
    }
    else{
        for(auto & status : teamsAdvanceStatuses){
            if(status.second == StartListCompetitorStatus::SureAdvanced)
                teams.push_back(status.first);
        }
    }
    return teams;
}

void TeamCompetitionManager::updateTeamsVectorByQualifications(QVector<Team> &teamsVectorToModify)
{
    QVector<Team *> teams = getFilteredTeamsForNextRound();

    for(auto & team : teamsVectorToModify)
    {
        if(Team::containsTeamByCode(teams, team.getCountryCode()) == false)
            MyFunctions::removeFromVector(teamsVectorToModify, &team);
    }
}

QVector<Team> TeamCompetitionManager::getFilteredTeamsAfterQualifications(CompetitionInfo *competition)
{
    CompetitionResults * results = &competition->getAdvancementCompetition()->getResultsReference();
    int lastQualifiedPosition = competition->getRulesPointer()->getRoundsReference()[0].getCount();

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

    QVector<Team> toReturn;
    for(auto & result : results->getResultsReference())
    {
        if(result.getPosition() <= lastQualifiedPosition){
            Team team;
            team.setCountryCode(result.getTeam()->getCountryCode());
            team.setJumpers(result.getTeam()->getJumpersReference());
            toReturn.push_back(team);
        }
    }
    return toReturn;
}

QVector<Team> TeamCompetitionManager::getFilteredTeamsByClassification(CompetitionInfo *competition, Classification *classification, QVector<Team> &teams)
{
    int lastQualifiedPosition = competition->getRulesPointer()->getRoundsReference()[0].getCount();

    bool exaequo = true;
    for(auto & res : classification->getResultsReference()){ // Sprawdzamy, czy wystąpiło ex aequo.
        if(res->getPosition() == lastQualifiedPosition){
            exaequo = false;
            break;
        }
    }
    if(exaequo == true) // Jeżeli wystąpiło ex aequo
    {
        for(auto & res : classification->getResultsReference()){
            if(res->getPosition() > lastQualifiedPosition){
                lastQualifiedPosition = res->getPosition();
                break;
            }
        }
    }

    QVector<Team> toReturn;
    for(auto & team : teams)
    {
        if(classification->getResultOfTeam(team.getCountryCode()) != nullptr)
        {
            if(classification->getResultOfTeam(team.getCountryCode())->getPosition() <= lastQualifiedPosition)
                toReturn.push_back(team);
        }
    }
    return toReturn;
}

void TeamCompetitionManager::setStartListOrderByClassification(QVector<Team> & teams, Classification *classification)
{
    if(classification->getClassificationType() == Classification::Team)
    {
        QVector<Team> tempTeams;
        for(auto & result : classification->getResultsReference())
        {
            if(MyFunctions::vectorContains(teams, Team::getTeamByCountryCode(&teams, result->getTeamCode())) && result->getPointsSum() != 0)
                tempTeams.push_back(*Team::getTeamByCountryCode(&teams, result->getTeamCode()));
        }

        QVector<Team> others;
        std::reverse(teams.begin(), teams.end());
        for(auto & team : teams)
        {
            if(MyFunctions::vectorContains(tempTeams, Team::getTeamByCountryCode(&tempTeams, team.getCountryCode())) == false)
                others.push_back(team);
        }
        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(others.begin(), others.end(), g);

        tempTeams.append(others);

        std::reverse(tempTeams.begin(), tempTeams.end());
        teams = tempTeams;
    }
}

void TeamCompetitionManager::setStartListOrderByCompetitionResults(QVector<Team> &teams, CompetitionInfo *competition)
{
    if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
    {
        QVector<Team> tempTeams;
        for(auto & result : competition->getResultsReference().getResultsReference())
        {
            if(MyFunctions::vectorContains(teams, Team::getTeamByCountryCode(&teams, result.getTeam()->getCountryCode())) && result.getPointsSum() != 0)
            {
                tempTeams.push_back(*Team::getTeamByCountryCode(&teams, result.getTeam()->getCountryCode()));
            }
        }
        QVector<Team> others;
        std::reverse(teams.begin(), teams.end());
        for(auto & team : teams)
        {
            if(MyFunctions::vectorContains(tempTeams, Team::getTeamByCountryCode(&tempTeams, team.getCountryCode())) == false)
                others.push_back(team);
        }
        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(others.begin(), others.end(), g);

        tempTeams.append(others);

        std::reverse(tempTeams.begin(), tempTeams.end());
        teams = tempTeams;
    }
}

void TeamCompetitionManager::setStartListOrderRandomly(QVector<Team> &teams)
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(teams.begin(), teams.end(), g);
}

bool TeamCompetitionManager::checkCompetitionEnd()
{
    return (checkGroupEnd() && checkRoundEnd() && (actualRound == competitionRules->getRoundsReference().count() || actualRound == competitionInfo->getExceptionalRoundsCount()));
}

bool TeamCompetitionManager::checkRoundEnd()
{
    return (isAllJumpsAreFinished() == true && (actualGroup == competitionRules->getJumpersInTeamCount()));
}

bool TeamCompetitionManager::checkGroupEnd()
{
    return (isAllJumpsAreFinished() == true);
}

void TeamCompetitionManager::setupNextGroup()
{
    if(actualGroup + 1 <= competitionRules->getJumpersInTeamCount()){
        setActualGroup(actualGroup + 1);
        setupStartListStatusesForActualRound();
        updateCompetitorsAdvanceStatuses();
        setActualStartListIndex(0);
    }
}

int TeamCompetitionManager::getActualGroup() const
{
    return actualGroup;
}

void TeamCompetitionManager::setActualGroup(int newActualGroup)
{
    actualGroup = newActualGroup;
    emit actualGroupChanged();
}

QVector<QPair<Team *, int> > TeamCompetitionManager::getTeamsAdvanceStatuses() const
{
    return teamsAdvanceStatuses;
}

QVector<QPair<Team *, int> > & TeamCompetitionManager::getTeamsAdvanceStatusesReference()
{
    return teamsAdvanceStatuses;
}

void TeamCompetitionManager::setTeamsAdvanceStatuses(const QVector<QPair<Team *, int> > &newTeamsAdvanceStatuses)
{
    teamsAdvanceStatuses = newTeamsAdvanceStatuses;
}

QVector<QVector<Team *> > TeamCompetitionManager::getRoundsTeams() const
{
    return roundsTeams;
}

QVector<QVector<Team *> > &TeamCompetitionManager::getRoundsTeamsReference()
{
    return roundsTeams;
}

void TeamCompetitionManager::setRoundsTeams(const QVector<QVector<Team *> > &newRoundTeams)
{
    roundsTeams = newRoundTeams;
}

QVector<Team *> & TeamCompetitionManager::getActualRoundTeamsReference()
{
    return roundsTeams[actualRound - 1];
}

Team *TeamCompetitionManager::getActualTeam() const
{
    return actualTeam;
}

void TeamCompetitionManager::setActualTeam(Team *newActualTeam)
{
    actualTeam = newActualTeam;
}
