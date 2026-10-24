#include "TeamCompetitionManager.h"
#include "../CompetitionAdvancement.h"
#include "../../utilities/functions.h"
#include <random>

TeamCompetitionManager::TeamCompetitionManager() : AbstractCompetitionManager(CompetitionRules::Team)
{
    actualGroup = 0;
    actualTeam = nullptr;
    connect(this, &TeamCompetitionManager::actualStartListIndexChanged, this, [this](){
        const int roundIndex = actualRound - 1;
        if(roundIndex < 0 || roundIndex >= roundsTeams.count()
            || actualStartListIndex < 0 || actualStartListIndex >= roundsTeams.at(roundIndex).count()
            || actualStartListIndex >= startListStatuses.count())
            return;
        actualJumper = startListStatuses.at(actualStartListIndex).getJumper();
        emit actualJumperChanged();
        actualTeam = roundsTeams.at(roundIndex).at(actualStartListIndex);
        emit actualTeamChanged();
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
    for(auto *team : getActualRoundTeamsReference())
        if(auto *result=results->getResultOfTeam(team)) {
            result->updatePointsSum(actualRound);result->updateTeamJumpersResults();
        }
    results->sortInDescendingOrder();
}

void TeamCompetitionManager::setupStartListStatusesForActualRound(bool setupBeforeCompetition)
{
    if(setupBeforeCompetition != true)
        sortActualRoundJumpersForActualGroup();

    startListStatuses.clear();
    for(auto & team : getActualRoundTeamsReference()){
        startListStatuses.push_back(StartListCompetitorStatus(team->getJumpersReference()[actualGroup - 1]));
        if(auto *result=results->getResultOfTeam(team);result && result->isTeamDisqualified())startListStatuses.last().setJumpStatus(StartListCompetitorStatus::Dsq);
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
    teamsAdvanceStatuses.clear();
    if(!results || !competitionRules || actualRound<1 || actualRound>roundsTeams.size())return;
    results->sortInDescendingOrder();
    QVector<CompetitionAdvancement::Entry> entries;
    for(auto *team:getActualRoundTeamsReference()) {
        CompetitionAdvancement::Entry entry;
        entry.competitor=team;entry.round=entry.maximumRound=actualRound;
        auto *jumper=team?team->getJumpersReference().value(actualGroup-1,nullptr):nullptr;
        const auto *status=StartListCompetitorStatus::getStatusOfJumper(jumper,startListStatuses);
        entry.pending=actualGroup<competitionRules->getJumpersInTeamCount() || !status || status->getJumpStatus()==StartListCompetitorStatus::Unfinished;
        if(auto *result=results->getResultOfTeam(team)){entry.points=result->getRankingPoints();entry.excluded=result->isTeamDisqualified();}
        entries.append(entry);
    }
    for(const auto &entry:entries)
        teamsAdvanceStatuses.append({static_cast<Team *>(const_cast<void *>(entry.competitor)),CompetitionAdvancement::status(entry,entries,advancementLimit())});
    updateQualificationStatuses();
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
    for(int i = teamsVectorToModify.count() - 1; i >= 0; --i)
        if(!Team::containsTeamByCode(teams, teamsVectorToModify.at(i).getCountryCode()))
            teamsVectorToModify.removeAt(i);
}

QVector<Team> TeamCompetitionManager::getFilteredTeamsAfterQualifications(CompetitionInfo *competition)
{
    QVector<Team> qualified;
    if(!competition || !competition->getAdvancementCompetition() || competition->getRulesPointer()->getRoundsReference().isEmpty())return qualified;
    const int limit = qMax(0,competition->getRulesPointer()->getRoundsReference().first().getCount());
    if(limit == 0)return qualified;
    CompetitionResults standings = competition->getAdvancementCompetition()->getResultsReference();
    standings.sortInDescendingOrder();
    QSet<QString> selected;
    for(const auto &result : qAsConst(standings.getResultsReference())) {
        auto *team = result.getTeam();
        if(!team || selected.contains(team->getCountryCode()) || result.getPosition() <= 0 || result.getPosition() > limit)continue;
        Team copy;
        copy.setCountryCode(team->getCountryCode());copy.setJumpers(team->getJumpersReference());
        qualified.append(copy);selected.insert(copy.getCountryCode());
    }
    return qualified;
}

QVector<Team> TeamCompetitionManager::getFilteredTeamsByClassification(CompetitionInfo *competition, Classification *classification, QVector<Team> &teams)
{
    QVector<Team> qualified;
    if(!competition || !classification || competition->getRulesPointer()->getRoundsReference().isEmpty())return qualified;
    const int limit = qMax(0,competition->getRulesPointer()->getRoundsReference().first().getCount());
    const auto ranked = classification->getRankedResults();
    QSet<QString> selected;
    for(const auto &team : qAsConst(teams)) {
        auto *result = classification->getResultOfTeam(team.getCountryCode());
        if(result && ranked.contains(result) && result->getPosition() > 0 && result->getPosition() <= limit && !selected.contains(team.getCountryCode())) {
            qualified.append(team);selected.insert(team.getCountryCode());
        }
    }
    return qualified;
}

void TeamCompetitionManager::setStartListOrderByClassification(QVector<Team> & teams, Classification *classification)
{
    if(classification->getClassificationType() == Classification::Team)
    {
        QVector<Team> tempTeams;
        for(auto & result : classification->getRankedResults())
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
    if(actualRound>0 && actualRound<=roundsTeams.size() && !getActualRoundTeamsReference().isEmpty() && std::all_of(getActualRoundTeamsReference().cbegin(),getActualRoundTeamsReference().cend(),[this](Team *team){auto *result=results->getResultOfTeam(team);return result && result->isTeamDisqualified();}))return true;
    if(competitionInfo && (competitionInfo->getCancelled() || (competitionInfo->getExceptionalRoundsCount()>0 && actualRound>competitionInfo->getExceptionalRoundsCount())))return true;
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
        const int next=getFirstUnfinishedStartListStatus();
        setActualStartListIndex(next>=0 && next<startListStatuses.size()?next:0);
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
