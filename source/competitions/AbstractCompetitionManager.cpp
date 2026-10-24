#include "AbstractCompetitionManager.h"
#include "CompetitionAdvancement.h"
#include "CompetitionManagers/TeamCompetitionManager.h"
#include "CompetitionManagers/IndividualCompetitionManager.h"
#include "../utilities/functions.h"
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <functional>

AbstractCompetitionManager::AbstractCompetitionManager(short type) : type(type)
{
    actualGate = actualRound = 0;
    results = nullptr;
    competitionRules = nullptr;
    competitionInfo = nullptr;
    toBeatLineDistance = toAdvanceLineDistance = 0;
    coachGateForNextJumper = false;
    actualCoachGate = 0;
    setActualStartListIndex(0);
    actualCompetitorPointsToTheLeader = 0;
    leaderResult = nullptr;
    lastQualifiedResult = nullptr;
    altQualifiersLimit = 0;
    actualJumper=nullptr;actualWinds=nullptr;actualJumpManipulator=nullptr;last10Judges=54; baseDSQProbability=0;
}

int AbstractCompetitionManager::effectiveRoundsCount() const
{
    if(!competitionRules)return 0;
    const int configured=competitionRules->getRoundsReference().size();
    const int exceptional=competitionInfo?competitionInfo->getExceptionalRoundsCount():0;
    return exceptional>0?qMin(configured,exceptional):configured;
}

CompetitionInfo *AbstractCompetitionManager::nearestQualificationTarget() const
{
    if(!competitionInfo)return nullptr;
    for(auto *target:competitionInfo->getQualifyingCompetitionsReference())
        if(target && target!=competitionInfo && !target->getCancelled() && !target->getPlayed()
            && target->getAdvancementCompetition()==competitionInfo && (!competitionRules || target->getRulesPointer()->getCompetitionType()==competitionRules->getCompetitionType()) && !target->getRulesPointer()->getRoundsReference().isEmpty())return target;
    return nullptr;
}

int AbstractCompetitionManager::qualificationLimit() const
{
    if(auto *target=nearestQualificationTarget())return qMax(0,target->getRulesPointer()->getRoundsReference().first().getCount());
    if(competitionInfo && !competitionInfo->getQualifyingCompetitionsReference().isEmpty())return 0;
    const int rounds=effectiveRoundsCount();
    if(competitionRules && rounds>0 && rounds<competitionRules->getRoundsReference().size())
        return qMax(0,competitionRules->getRoundsReference()[rounds].getCount());
    return qMax(0,altQualifiersLimit);
}

bool AbstractCompetitionManager::qualificationIncludesTies() const
{
    if(auto *target=nearestQualificationTarget())return !target->getRulesPointer()->getRoundsReference().first().getKO();
    return !qualificationFixedLimit;
}

int AbstractCompetitionManager::advancementLimit() const
{
    if(!competitionRules || actualRound<1 || actualRound>effectiveRoundsCount())return -1;
    if(actualRound<effectiveRoundsCount())return qMax(0,competitionRules->getRoundsReference()[actualRound].getCount());
    return qualificationLimit()>0?qualificationLimit():-1;
}

int AbstractCompetitionManager::qualificationStatus(Jumper *jumper) const
{
    return qualificationJumpers.value(jumper,-1);
}

int AbstractCompetitionManager::qualificationStatus(Team *team) const
{
    return qualificationTeams.value(team,-1);
}

void AbstractCompetitionManager::updateQualificationStatuses()
{
    qualificationJumpers.clear();qualificationTeams.clear();
    const int limit=qualificationLimit(), rounds=effectiveRoundsCount();
    if(limit<=0 || !results || actualRound<1 || actualRound>rounds || rounds<1)return;
    auto *individual=dynamic_cast<IndividualCompetitionManager *>(this);
    auto *teamManager=dynamic_cast<TeamCompetitionManager *>(this);
    QVector<CompetitionAdvancement::Entry> entries;
    const auto append=[&](Jumper *jumper,Team *team,int order) {
        auto *result=team?results->getResultOfTeam(team):results->getResultOfIndividualJumper(jumper);
        const auto *current=StartListCompetitorStatus::getStatusOfJumper(jumper,startListStatuses);
        CompetitionAdvancement::Entry entry;
        entry.competitor=team?static_cast<const void *>(team):jumper;
        entry.order=order;entry.round=result?result->getRankingRound():1;
        entry.maximumRound=rounds;entry.points=result?result->getRankingPoints():0;
        entry.excluded=current && !team && (current->getJumpStatus()==StartListCompetitorStatus::Dsq || current->getJumpStatus()==StartListCompetitorStatus::Dns);
        if(result && result->isTeamDisqualified())entry.excluded=true;
        if(result && !team && !result->getJumpsReference().isEmpty()) {
            const auto &last=result->getJumpsReference().last();
            entry.excluded=entry.excluded || last.getDSQ() || last.getDNS();
        }
        if(current) {
            entry.pending=current->getJumpStatus()==StartListCompetitorStatus::Unfinished;
            if(teamManager && teamManager->getActualGroup()<competitionRules->getJumpersInTeamCount())entry.pending=true;
            if(actualRound<rounds && !entry.excluded) {
                const int advancement=team?teamManager->getAdvanceStatusOfTeam(team):current->getAdvanceStatus();
                if(advancement!=StartListCompetitorStatus::SureDroppedOut) {
                    entry.pending=true;
                    if(advancement==StartListCompetitorStatus::SureAdvanced)entry.round=qMax(entry.round,actualRound+1);
                }
            }
            entry.protectedByDistance=current->getQualifiedBy95HSRule();
        } else {
            entry.pending=!result;
            if(result)entry.maximumRound=entry.round;
        }
        entries.append(entry);
    };
    if(individual) {
        const auto field=individual->getRoundsJumpers();
        if(!field.isEmpty())for(auto *jumper:field.first())if(jumper)append(jumper,nullptr,entries.size());
        for(const auto &result:results->getResultsReference()) {
            bool found=false;for(const auto &entry:entries)if(entry.competitor==result.getJumper()){found=true;break;}
            if(!found && result.getJumper())append(result.getJumper(),nullptr,entries.size());
        }
    } else if(teamManager) {
        const auto field=teamManager->getRoundsTeams();
        if(!field.isEmpty())for(auto *team:field.first())if(team) {
            Jumper *jumper=team->getJumpersReference().value(teamManager->getActualGroup()-1,nullptr);
            append(jumper,team,entries.size());
        }
    }
    for(auto &entry:entries) {
        int order=0;
        for(const auto &result:results->getResultsReference()) {
            if((individual && entry.competitor==result.getJumper()) || (teamManager && entry.competitor==result.getTeam())){entry.order=order;break;}
            ++order;
        }
    }
    for(const auto &entry:entries) {
        const int state=CompetitionAdvancement::status(entry,entries,limit,qualificationIncludesTies());
        if(individual)qualificationJumpers.insert(static_cast<Jumper *>(const_cast<void *>(entry.competitor)),state);
        else qualificationTeams.insert(static_cast<Team *>(const_cast<void *>(entry.competitor)),state);
    }
}

bool AbstractCompetitionManager::checkRoundEnd()
{
    return (isAllJumpsAreFinished() == true);
}

bool AbstractCompetitionManager::checkCompetitionEnd()
{
    if(competitionInfo && (competitionInfo->getCancelled() || (competitionInfo->getExceptionalRoundsCount()>0 && actualRound>competitionInfo->getExceptionalRoundsCount())))return true;
    return (checkRoundEnd() && (actualRound == competitionRules->getRoundsReference().count() || actualRound == competitionInfo->getExceptionalRoundsCount()));
}

namespace {
bool lineContextValid(AbstractCompetitionManager *manager) {
    return manager->getCompetitionInfo() && manager->getCompetitionInfo()->getHill() && manager->getCompetitionRules()
        && manager->getResults() && manager->getActualWinds() && manager->getActualJumper()
        && manager->getActualRound()>0 && manager->getActualRound()<=manager->getRoundsStartingGatesReference().size()
        && manager->getActualStartListIndex()>=0 && manager->getActualStartListIndex()<manager->getStartListStatusesReference().size()
        && manager->getStartListStatusesReference()[manager->getActualStartListIndex()].getJumpStatus()==StartListCompetitorStatus::Unfinished;
}
double currentPoints(AbstractCompetitionManager *manager) {
    CompetitionSingleResult *result=nullptr;
    if(auto *team=dynamic_cast<TeamCompetitionManager *>(manager))result=manager->getResults()->getResultOfTeam(team->getActualTeam());
    else result=manager->getResults()->getResultOfIndividualJumper(manager->getActualJumper());
    return result?result->getPointsSum():0.;
}
double distanceForPoints(AbstractCompetitionManager *manager,double target,bool strict) {
    auto *hill=manager->getCompetitionInfo()->getHill();auto *rules=manager->getCompetitionRules();
    if(hill->getPointsForMeter()<=0 || !std::isfinite(target))return -1;
    const double previous=currentPoints(manager);
    // Search half metres: distance-dependent wind and the coach's 95% HS rule make a linear inverse incorrect.
    const double ceiling=qMin(10000.,qMax(1000.,hill->getKPoint()+qMax(0.,target-previous)/hill->getPointsForMeter()+500.));
    for(double distance=0;distance<=ceiling;distance+=.5) {
        double points=hill->getPointsForKPoint()+(distance-hill->getKPoint())*hill->getPointsForMeter();
        if(rules->getHasJudgesPoints())points+=manager->getLast10Judges();
        if(rules->getHasWindCompensations()) {
            const auto winds=WindsCalculator::getScoringWinds(*manager->getActualWinds(),distance,hill->getKPoint(),rules->getWindCompensationDistanceEffect());
            points+=WindsCalculator::getWindCompensation(WindsCalculator::getAveragedWind(winds,rules->getWindAverageCalculatingType()),hill);
        }
        if(rules->getHasGateCompensations()) {
            double gates=WindsCalculator::getGateCompensation(manager->getRoundsStartingGatesReference()[manager->getActualRound()-1],manager->getActualGate(),hill);
            if(manager->getCoachGateForNextJumper() && JumpSimulator::isJumperBeat95HSPercents(distance,hill->getHSPoint()))
                gates+=WindsCalculator::getGateCompensation(manager->getActualGate(),manager->getActualCoachGate(),hill);
            points+=roundDoubleToOnePlace(gates);
        }
        const double total=previous+qMax(0.,roundDoubleToOnePlace(points));
        if(strict?total>target+0.00001:total>=target-0.00001)return distance;
    }
    return -1;
}
}

void AbstractCompetitionManager::updateToBeatLineDistance()
{
    toBeatLineDistance=-1;
    if(!lineContextValid(this))return;
    updateLeaderResult();
    if(!leaderResult || leaderResult->getJumpsReference().isEmpty())return;
    toBeatLineDistance=distanceForPoints(this,leaderResult->getPointsSum(),true);
}

void AbstractCompetitionManager::updateToAdvanceLineDistance()
{
    toAdvanceLineDistance=-1;
    if(!lineContextValid(this))return;
    auto *individual=dynamic_cast<IndividualCompetitionManager *>(this);
    auto *ko=individual?individual->getKOManager():nullptr;
    if(ko){
        const double target=ko->guaranteedAdvancePoints(actualJumper);
        if(target>=0)toAdvanceLineDistance=distanceForPoints(this,target,false);
        return;
    }
    updateLastQualifiedResult();
    if(lastQualifiedResult)toAdvanceLineDistance=distanceForPoints(this,lastQualifiedResult->getPointsSum(),false);
}


void AbstractCompetitionManager::updateLeaderResult()
{
    results->sortInDescendingOrder();
    leaderResult=nullptr;
    for(auto &result:results->getResultsReference())if(!result.isTeamDisqualified()){leaderResult=&result;break;}
}

void AbstractCompetitionManager::updateLastQualifiedResult()
{
    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager*>(this);

    if(!results || !competitionRules || actualRound<1 || actualRound>competitionRules->getRoundsReference().size()) { lastQualifiedResult=nullptr;return; }
    if(!indManager || !indManager->getKOManager()) {
        lastQualifiedResult=nullptr;
        const int limit=advancementLimit();
        if(limit<=0)return;
        if(auto *team=dynamic_cast<TeamCompetitionManager *>(this)) {
            if(team->getActualGroup()<competitionRules->getJumpersInTeamCount() || team->getActualRoundTeamsReference().size()<=limit)return;
        } else if(indManager->getActualRoundJumpersReference().size()<=limit)return;
        QVector<CompetitionSingleResult *> completed;
        int remaining=0;
        for(const auto &status:startListStatuses) {
            if(status.getJumpStatus()==StartListCompetitorStatus::Unfinished) { ++remaining;continue; }
            if(status.getJumpStatus()!=StartListCompetitorStatus::Finished)continue;
            CompetitionSingleResult *result=nullptr;
            if(auto *team=dynamic_cast<TeamCompetitionManager *>(this)) {
                auto *squad=Team::getTeamByCountryCode(&competitionInfo->getTeamsReference(),status.getJumper()->getCountryCode());
                result=results->getResultOfTeam(squad);
            } else result=results->getResultOfIndividualJumper(status.getJumper());
            if(result && !completed.contains(result))completed.append(result);
        }
        const bool pending=actualStartListIndex>=0 && actualStartListIndex<startListStatuses.size() && startListStatuses[actualStartListIndex].getJumpStatus()==StartListCompetitorStatus::Unfinished;
        const int place=limit-remaining+int(pending);
        std::sort(completed.begin(),completed.end(),[](auto *left,auto *right){return left->getPointsSum()>right->getPointsSum();});
        if(place>0 && place<=completed.size())lastQualifiedResult=completed[place-1];
        return;
    }
    lastQualifiedResult=nullptr;
    auto *ko=indManager->getKOManager();
    const double target=ko->guaranteedAdvancePoints(actualJumper);
    if(target<0)return;
    for(auto &result:results->getResultsReference())
        if(result.getPointsSum()>=target && (!lastQualifiedResult || result.getPointsSum()<lastQualifiedResult->getPointsSum()))lastQualifiedResult=&result;
}

void AbstractCompetitionManager::updateActualCompetitorPointsToTheLeader()
{
    if(actualRound == 1)
        actualCompetitorPointsToTheLeader = (-1);
    else{
        double actualJumperPoints = 0;
        if(type == CompetitionRules::Individual){
            actualJumperPoints = results->getResultOfIndividualJumper(actualJumper)->getPointsSum();
        }
        else if(type == CompetitionRules::Team){
            actualJumperPoints = results->getResultOfTeam(dynamic_cast<TeamCompetitionManager*>(this)->getActualTeam())->getPointsSum();
        }
        double actualLeaderPointsWithoutActualRound = leaderResult->getPointsSum() - leaderResult->getJumps().at(leaderResult->getJumps().count() - 1).getPoints();
        actualCompetitorPointsToTheLeader = roundDoubleToOnePlace(actualJumperPoints - actualLeaderPointsWithoutActualRound);
    }
}

void AbstractCompetitionManager::updateLast10Judges()
{
    last10Judges = 0;
    int howMany = 0;
    if(competitionInfo == nullptr || competitionInfo->getRulesPointer() == nullptr || results == nullptr)
        return;

    const int startIndex = qMin(actualStartListIndex, startListStatuses.count() - 1);
    for(int i = startIndex; i >= 0 && howMany < 10; --i)
    {
        if(startListStatuses.at(i).getJumpStatus() == StartListCompetitorStatus::Unfinished)
        {
            if(i == startIndex)
                continue;
            break;
        }
        if(startListStatuses.at(i).getJumpStatus() == StartListCompetitorStatus::Finished)
        {
            CompetitionSingleResult *jumperResult = nullptr;
            if (competitionInfo->getRulesPointer()->getCompetitionType()
                == CompetitionRules::Individual)
            {
                jumperResult = results->getResultOfIndividualJumper(startListStatuses.at(i).getJumper());
            }
            else if(startListStatuses.at(i).getJumper() != nullptr)
            {
                Team *team = Team::getTeamByCountryCode(&competitionInfo->getTeamsReference(), startListStatuses.at(i).getJumper()->getCountryCode());
                CompetitionSingleResult *teamResult = results->getResultOfTeam(team);
                if(teamResult != nullptr)
                    jumperResult = teamResult->getTeamJumperResult(startListStatuses.at(i).getJumper());
            }
            if(jumperResult != nullptr && !jumperResult->getJumpsReference().isEmpty())
            {
                last10Judges += jumperResult->getJumpsReference().last().getJudgesPoints();
                howMany++;
            }
        }
    }
    if(howMany > 0)
        last10Judges /= double(howMany);
    else last10Judges=54;
    qDebug()<<"last 10 judges average : "<<last10Judges;
}

int AbstractCompetitionManager::getFirstUnfinishedStartListStatus()
{
    int index = 0;
    for(auto & status : startListStatuses){
        if(status.getJumpStatus() == StartListCompetitorStatus::Unfinished)
            return index;
        else
            index++;
    }
    return index; // 0
}

bool AbstractCompetitionManager::isAllJumpsAreFinished()
{
    for(auto & status : startListStatuses){
        if(status.getJumpStatus() == StartListCompetitorStatus::Unfinished)
            return false;
    }
    return true;
}

void AbstractCompetitionManager::setActualJumperToNextUnfinished()
{
    const int index = getFirstUnfinishedStartListStatus();
    if(index < 0 || index >= startListStatuses.count())
    {
        setActualJumper(nullptr);
        return;
    }
    setActualStartListIndex(index);
    setActualJumper(startListStatuses.at(index).getJumper());
}

double AbstractCompetitionManager::getLast10Judges() const
{
    return last10Judges;
}

void AbstractCompetitionManager::setLast10Judges(double newLast10Judges)
{
    last10Judges = newLast10Judges;
}

int AbstractCompetitionManager::getAltQualifiersLimit() const
{
    return altQualifiersLimit;
}

void AbstractCompetitionManager::setAltQualifiersLimit(int newAltQualifiersLimit)
{
    altQualifiersLimit = newAltQualifiersLimit;
}

QVector<StartListCompetitorStatus> AbstractCompetitionManager::getStartListStatuses() const
{
    return startListStatuses;
}

QVector<StartListCompetitorStatus> &AbstractCompetitionManager::getStartListStatusesReference()
{
    return startListStatuses;
}

void AbstractCompetitionManager::setStartListStatuses(const QVector<StartListCompetitorStatus> &newStartListStatuses)
{
    startListStatuses = newStartListStatuses;
}

QVector<WindGenerationSettings> AbstractCompetitionManager::getWindGenerationSettings() const
{
    return windGenerationSettings;
}

QVector<WindGenerationSettings> & AbstractCompetitionManager::getWindGenerationSettingsReference()
{
    return windGenerationSettings;
}

void AbstractCompetitionManager::setWindGenerationSettings(const QVector<WindGenerationSettings> &newWindGenerationSettings)
{
    windGenerationSettings = newWindGenerationSettings;
}

int AbstractCompetitionManager::getBaseDSQProbability() const
{
    return baseDSQProbability;
}

void AbstractCompetitionManager::setBaseDSQProbability(int newBaseDSQProbability)
{
    baseDSQProbability = newBaseDSQProbability;
}

JumpManipulator *AbstractCompetitionManager::getActualJumpManipulator() const
{
    return actualJumpManipulator;
}

void AbstractCompetitionManager::setActualJumpManipulator(JumpManipulator *newActualJumpManipulator)
{
    actualJumpManipulator = newActualJumpManipulator;
}

QVector<Wind> *AbstractCompetitionManager::getActualWinds() const
{
    return actualWinds;
}

void AbstractCompetitionManager::setActualWinds(QVector<Wind> *newActualWinds)
{
    actualWinds = newActualWinds;
}

Jumper *AbstractCompetitionManager::getActualJumper() const
{
    return actualJumper;
}

void AbstractCompetitionManager::setActualJumper(Jumper *newActualJumper)
{
    actualJumper = newActualJumper;
}

short AbstractCompetitionManager::getType() const
{
    return type;
}

void AbstractCompetitionManager::setType(short newType)
{
    type = newType;
}

CompetitionInfo *AbstractCompetitionManager::getCompetitionInfo() const
{
    return competitionInfo;
}

void AbstractCompetitionManager::setCompetitionInfo(CompetitionInfo *newCompetitionInfo)
{
    competitionInfo = newCompetitionInfo;
}

CompetitionRules *AbstractCompetitionManager::getCompetitionRules() const
{
    return competitionRules;
}

void AbstractCompetitionManager::setCompetitionRules(CompetitionRules *newCompetitionRules)
{
    competitionRules = newCompetitionRules;
}

CompetitionResults *AbstractCompetitionManager::getResults() const
{
    return results;
}

void AbstractCompetitionManager::setResults(CompetitionResults *newResults)
{
    results = newResults;
}

QVector<int> AbstractCompetitionManager::getRoundsStartingGates() const
{
    return roundsStartingGates;
}

QVector<int> & AbstractCompetitionManager::getRoundsStartingGatesReference()
{
    return roundsStartingGates;
}

void AbstractCompetitionManager::setRoundsStartingGates(const QVector<int> &newRoundsStartingGates)
{
    roundsStartingGates = newRoundsStartingGates;
}

int AbstractCompetitionManager::getActualGate() const
{
    return actualGate;
}

void AbstractCompetitionManager::setActualGate(int newActualGate)
{
    actualGate = newActualGate;
}

int AbstractCompetitionManager::getActualRound() const
{
    return actualRound;
}

void AbstractCompetitionManager::setActualRound(int newActualRound)
{
    actualRound = newActualRound;
}

double AbstractCompetitionManager::getToBeatLineDistance() const
{
    return toBeatLineDistance;
}

void AbstractCompetitionManager::setToBeatLineDistance(double newToBeatLineDistance)
{
    toBeatLineDistance = newToBeatLineDistance;
}

double AbstractCompetitionManager::getToAdvanceLineDistance() const
{
    return toAdvanceLineDistance;
}

void AbstractCompetitionManager::setToAdvanceLineDistance(double newToAdvanceLineDistance)
{
    toAdvanceLineDistance = newToAdvanceLineDistance;
}

bool AbstractCompetitionManager::getCoachGateForNextJumper() const
{
    return coachGateForNextJumper;
}

void AbstractCompetitionManager::setCoachGateForNextJumper(bool newCoachGateForNextJumper)
{
    coachGateForNextJumper = newCoachGateForNextJumper;
}

int AbstractCompetitionManager::getActualCoachGate() const
{
    return actualCoachGate;
}

void AbstractCompetitionManager::setActualCoachGate(int newActualCoachGate)
{
    actualCoachGate = newActualCoachGate;
}

int AbstractCompetitionManager::getActualStartListIndex() const
{
    return actualStartListIndex;
}

void AbstractCompetitionManager::setActualStartListIndex(int newActualStartListIndex)
{
    actualStartListIndex = newActualStartListIndex;
    emit actualStartListIndexChanged();
}

double AbstractCompetitionManager::getActualCompetitorPointsToTheLeader() const
{
    return actualCompetitorPointsToTheLeader;
}

void AbstractCompetitionManager::setActualCompetitorPointsToTheLeader(double newActualCompetitorPointsToTheLeader)
{
    actualCompetitorPointsToTheLeader = newActualCompetitorPointsToTheLeader;
}

CompetitionSingleResult *AbstractCompetitionManager::getLeaderResult() const
{
    return leaderResult;
}

void AbstractCompetitionManager::setLeaderResult(CompetitionSingleResult *newLeaderResult)
{
    leaderResult = newLeaderResult;
}

CompetitionSingleResult *AbstractCompetitionManager::getLastQualifiedResult() const
{
    return lastQualifiedResult;
}

void AbstractCompetitionManager::setLastQualifiedResult(CompetitionSingleResult *newLastQualifiedResult)
{
    lastQualifiedResult = newLastQualifiedResult;
}
