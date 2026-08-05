#include "AbstractCompetitionManager.h"
#include "CompetitionManagers/TeamCompetitionManager.h"
#include "CompetitionManagers/IndividualCompetitionManager.h"
#include "../utilities/functions.h"
#include <QDebug>

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
}

bool AbstractCompetitionManager::checkRoundEnd()
{
    return (isAllJumpsAreFinished() == true);
}

bool AbstractCompetitionManager::checkCompetitionEnd()
{
    return (checkRoundEnd() && (actualRound == competitionRules->getRoundsReference().count() || actualRound == competitionInfo->getExceptionalRoundsCount()));
}

void AbstractCompetitionManager::updateToBeatLineDistance()
{
    results->sortInDescendingOrder();
    Hill * hill = competitionInfo->getHill();
    double leaderPoints = leaderResult->getPointsSum();

    double windCompensation = WindsCalculator::getWindCompensation(WindsCalculator::getAveragedWind(*actualWinds, competitionRules->getWindAverageCalculatingType()).getStrengthToAveragedWind(), competitionInfo->getHill());
    int tempGate = actualGate;
    if(coachGateForNextJumper == true) tempGate = actualCoachGate;
    double gateCompensation = WindsCalculator::getGateCompensation(roundsStartingGates.at(actualRound - 1), tempGate, competitionInfo->getHill());
    toBeatLineDistance = 0;
    if((getType() == CompetitionRules::Individual && actualRound == 1) || (getType() == CompetitionRules::Team && dynamic_cast<TeamCompetitionManager *>(this)->getActualGroup() == 1 && actualRound == 1)){
        toBeatLineDistance = leaderPoints;
    }
    else{
        double actualJumperPoints = 0;
        if(type == CompetitionRules::Individual){
            actualJumperPoints = results->getResultOfIndividualJumper(actualJumper)->getPointsSum();
        }
        else if(type == CompetitionRules::Team){
            actualJumperPoints = results->getResultOfTeam(dynamic_cast<TeamCompetitionManager*>(this)->getActualTeam())->getPointsSum();
        }

        toBeatLineDistance = leaderPoints - actualJumperPoints;
    }
    toBeatLineDistance -= hill->getPointsForKPoint();
    if(competitionRules->getHasJudgesPoints())
    {
        toBeatLineDistance -= last10Judges;
    }
    if(competitionRules->getHasGateCompensations())
        toBeatLineDistance -= gateCompensation;
    if(competitionRules->getHasWindCompensations())
        toBeatLineDistance -= windCompensation;

    toBeatLineDistance /= hill->getPointsForMeter();
    toBeatLineDistance += hill->getKPoint();
    toBeatLineDistance = std::round(toBeatLineDistance * 2) / 2;
}

void AbstractCompetitionManager::updateToAdvanceLineDistance()
{
    results->sortInDescendingOrder();  
    Hill * hill = competitionInfo->getHill();

    int shouldBeQualified = 0;
    if(actualRound < competitionRules->getRoundsReference().count())
        shouldBeQualified = competitionRules->getRounds().at(actualRound).getCount();
    else if(actualRound == competitionRules->getRoundsReference().count() && altQualifiersLimit > 0)
        shouldBeQualified = altQualifiersLimit;
    toAdvanceLineDistance = 0;

    int competitorsCount = 0;
    if(type == CompetitionRules::Individual){
        IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager*>(this);
        competitorsCount = indManager->getActualRoundJumpersReference().count();
    }
    else if(type == CompetitionRules::Team){
        competitorsCount = dynamic_cast<TeamCompetitionManager*>(this)->getActualRoundTeamsReference().count();
    }

    if((competitorsCount - shouldBeQualified - (actualStartListIndex) > 0 && competitionRules->getRoundsReference()[actualRound - 1].getKO() == false) || (actualRound == competitionRules->getRounds().count() && altQualifiersLimit == 0) || competitorsCount <= shouldBeQualified || lastQualifiedResult == nullptr)
    {
        toAdvanceLineDistance = -1;
    }
    else{
        double lastQualifiedPoints = lastQualifiedResult->getPointsSum();
        if((getType() == CompetitionRules::Individual && actualRound == 1) || (getType() == CompetitionRules::Team && dynamic_cast<TeamCompetitionManager *>(this)->getActualGroup() == 1  && actualRound == 1)){
            toAdvanceLineDistance = lastQualifiedPoints;
        }
        else{
            double actualJumperPoints = 0;
            if(type == CompetitionRules::Individual){
                actualJumperPoints = results->getResultOfIndividualJumper(actualJumper)->getPointsSum();
            }
            else if(type == CompetitionRules::Team){
                actualJumperPoints = results->getResultOfTeam(dynamic_cast<TeamCompetitionManager*>(this)->getActualTeam())->getPointsSum();
            }
            toAdvanceLineDistance = lastQualifiedPoints - actualJumperPoints;
        }
        double windCompensation = WindsCalculator::getWindCompensation(WindsCalculator::getAveragedWind(*actualWinds, competitionRules->getWindAverageCalculatingType()).getStrengthToAveragedWind(), competitionInfo->getHill());
        int tempGate = actualGate;
        if(coachGateForNextJumper == true) tempGate = getActualCoachGate();
        double gateCompensation = WindsCalculator::getGateCompensation(roundsStartingGates[actualRound - 1], tempGate, competitionInfo->getHill());

        toAdvanceLineDistance -= hill->getPointsForKPoint();
        if(competitionRules->getHasJudgesPoints())
        {
            toAdvanceLineDistance -= last10Judges;
        }
        if(competitionRules->getHasGateCompensations())
            toAdvanceLineDistance -= gateCompensation;
        if(competitionRules->getHasWindCompensations())
            toAdvanceLineDistance -= windCompensation;

        toAdvanceLineDistance /= hill->getPointsForMeter();
        toAdvanceLineDistance += hill->getKPoint();
        toAdvanceLineDistance = std::round(toAdvanceLineDistance * 2) / 2;
    }
}


void AbstractCompetitionManager::updateLeaderResult()
{
    results->sortInDescendingOrder();
    leaderResult = results->getResultByIndex(0);
}

void AbstractCompetitionManager::updateLastQualifiedResult()
{
    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager*>(this);

    results->sortInDescendingOrder();

    int competitorsCount = 0;
    if(type == CompetitionRules::Individual){
        competitorsCount = dynamic_cast<IndividualCompetitionManager*>(this)->getActualRoundJumpersReference().count();
    }
    else if(type == CompetitionRules::Team){
        competitorsCount = dynamic_cast<TeamCompetitionManager*>(this)->getActualRoundTeamsReference().count();
    }

    bool condition = false;
    if(indManager != nullptr)
        if(indManager->getKOManager()){
            int finished = 0; //Zawodnicy którzy skoczyli w aktualnej rundzie
            for(auto & jumper : indManager->getKOManager()->getActualGroup()->getJumpersReference())
                if(results->getResultOfIndividualJumper(jumper) != nullptr)
                    if(results->getResultOfIndividualJumper(jumper)->getJumpsReference().count() == actualRound)
                        finished++;
            if(finished >= indManager->getKOManager()->getRoundInfo()->getAdvancingFromKOGroup() && finished < indManager->getKOManager()->getActualGroup()->getJumpersReference().count())
                condition = true;
        }

    int lastQualifiedPosition = 0;
    int koLastQualifiedPosition = (-1);

    int limit = 0;
    if(competitionRules->getRounds().count() > actualRound)
        limit = competitionRules->getRounds().at(actualRound).getCount();
    if(altQualifiersLimit > 0 && actualRound == competitionRules->getRounds().count())
        limit = altQualifiersLimit;
    if((competitorsCount - limit - (actualStartListIndex) > 0 || (actualRound == competitionRules->getRounds().count() && altQualifiersLimit == 0) || competitorsCount <= limit) && condition == false){
        lastQualifiedPosition = -1;
    }
    else{
        bool condition = indManager != nullptr;
        if(condition == true)
        {
            if(indManager->getKOManager() == nullptr)
                condition = false;
        }

        if(condition == true)
        {
            //DLA KO
            QVector<Jumper *> sortedGroupJumpers = indManager->getKOManager()->getActualGroup()->getJumpersReference();
            results->sortJumpersByResults(sortedGroupJumpers);
            int remaining = 0;
            for(auto & jumper : indManager->getKOManager()->getActualGroup()->getJumpersReference())
            {
                if(results->getResultOfIndividualJumper(jumper) != nullptr)
                {
                    if(results->getResultOfIndividualJumper(jumper)->getJumpsReference().count() < indManager->getActualRound())
                        remaining++;
                }
                else
                    remaining++;
            }
            int addition = 0;
            //int addition = int(indManager->getKOManager()->getRoundInfo()->getAdvancingFromKOGroup() == 1);
            if(remaining <= indManager->getKOManager()->getRoundInfo()->getAdvancingFromKOGroup() && remaining > 0){
                Jumper * lastQualified = nullptr;
                if(indManager->getKOManager()->getActualGroup()->getJumpersReference().count() <= indManager->getKOManager()->getRoundInfo()->getAdvancingFromKOGroup())
                    lastQualified = sortedGroupJumpers.last();
                else
                    lastQualified = sortedGroupJumpers[indManager->getKOManager()->getRoundInfo()->getAdvancingFromKOGroup() - remaining];
                lastQualifiedPosition = results->getResultOfIndividualJumper(lastQualified)->getPosition();
            }
        }
        else{
            lastQualifiedPosition = abs(competitorsCount - limit - (actualStartListIndex + 1));
            koLastQualifiedPosition = lastQualifiedPosition;
        }
    }
    bool isLastQualifiedPositionValid = lastQualifiedPosition > 0 && lastQualifiedPosition <= results->getResultsReference().count();
    bool isActualRoundGoodOne = actualRound < competitionRules->getRounds().count();
    bool isActualRoundGoodSecond = altQualifiersLimit > 0 && actualRound == competitionRules->getRounds().count();
    if(isLastQualifiedPositionValid && (isActualRoundGoodOne || isActualRoundGoodSecond)){
        lastQualifiedResult = results->getResultByIndex(lastQualifiedPosition - 1);
        if(getType() == CompetitionRules::Individual)
        {
            if(competitionRules->getRoundsReference().count() > actualRound){
                if(competitionRules->getRoundsReference()[actualRound - 1].getKO() == false && competitionRules->getRoundsReference()[actualRound].getKO())// && competitionRules->getRoundsReference()[actualRound].getKoGroupsSelectionType() == CompetitionRules::Classic) //KO w next rundzie
                {
                    lastQualifiedResult = results->getResultByIndex(koLastQualifiedPosition - 1);
                }
            }
        }
    }
    else
        lastQualifiedResult = nullptr;
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
    int i=actualStartListIndex;
    int howMany = 0;
    while(true)
    {
        if(startListStatuses.count() > actualStartListIndex)
        {
            if(startListStatuses[i].getJumpStatus() == StartListCompetitorStatus::Finished)
            {
                if (competitionInfo->getRulesPointer()->getCompetitionType()
                    == CompetitionRules::Individual)
                    last10Judges += results
                                        ->getResultOfIndividualJumper(
                                            startListStatuses[i].getJumper())
                                        ->getJumpsReference()
                                        .last()
                                        .getJudgesPoints();
                else
                    last10Judges += results->getResultOfTeam(Team::getTeamByCountryCode(&competitionInfo->getTeamsReference(),startListStatuses[i].getJumper()->getCountryCode()))->getTeamJumperResult(startListStatuses[i].getJumper())->getJumpsReference().last().getJudgesPoints();
                howMany++;
            }
            i--;
            if(i == -1 || startListStatuses[i].getJumpStatus() == StartListCompetitorStatus::Unfinished)
                break;
        }
    }
    last10Judges /= double(howMany);
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
    setActualStartListIndex(getFirstUnfinishedStartListStatus());
    setActualJumper(startListStatuses[getFirstUnfinishedStartListStatus()].getJumper());
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
