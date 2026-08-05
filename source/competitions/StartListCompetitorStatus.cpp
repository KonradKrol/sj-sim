#include "StartListCompetitorStatus.h"

StartListCompetitorStatus::StartListCompetitorStatus(Jumper *jumper, int jumpStatus, int advanceStatus) : jumper(jumper), jumpStatus(jumpStatus), advanceStatus(advanceStatus)
{
    qualifiedBy95HSRule = false;
}

bool StartListCompetitorStatus::getQualifiedBy95HSRule() const
{
    return qualifiedBy95HSRule;
}

void StartListCompetitorStatus::setQualifiedBy95HSRule(bool newQualifiedBy95HSRule)
{
    qualifiedBy95HSRule = newQualifiedBy95HSRule;
}

StartListCompetitorStatus *StartListCompetitorStatus::getStatusOfJumper(Jumper *jumper, QVector<StartListCompetitorStatus> &statuses)
{
    for(auto & status : statuses){
        if(jumper == status.getJumper())
            return &status;
    }
    return nullptr;
}

int StartListCompetitorStatus::remainingJumpers(const QVector<StartListCompetitorStatus> &statuses)
{
    int count = 0;
    for(auto & status : statuses)
    {
        if(status.getJumpStatus() == Unfinished)
            count++;
    }
    return count;
}

Jumper *StartListCompetitorStatus::getJumper() const
{
    return jumper;
}

void StartListCompetitorStatus::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

int StartListCompetitorStatus::getAdvanceStatus() const
{
    return advanceStatus;
}

void StartListCompetitorStatus::setAdvanceStatus(int newAdvanceStatus)
{
    advanceStatus = newAdvanceStatus;
}

int StartListCompetitorStatus::getJumpStatus() const
{
    return jumpStatus;
}

void StartListCompetitorStatus::setJumpStatus(int newJumpStatus)
{
    jumpStatus = newJumpStatus;
}
