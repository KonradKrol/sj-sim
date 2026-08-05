#ifndef STARTLISTCOMPETITORSTATUS_H
#define STARTLISTCOMPETITORSTATUS_H

#include "../simulator/Jumper.h"
#include "../simulator/Team.h"

class StartListCompetitorStatus
{
public:
    StartListCompetitorStatus(Jumper * jumper = nullptr, int jumpStatus = Unfinished, int advanceStatus = Waiting);

    enum JumpStatus{
        Finished,
        Unfinished,
        Dsq,
        Dns
    };
    enum AdvanceStatus{
        SureAdvanced,
        SureDroppedOut,
        Waiting
    };

private:
    Jumper * jumper;
    int jumpStatus;
    int advanceStatus;
    bool qualifiedBy95HSRule;

public:
    int getJumpStatus() const;
    void setJumpStatus(int newJumpStatus);
    int getAdvanceStatus() const;
    void setAdvanceStatus(int newAdvanceStatus);
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
    bool getQualifiedBy95HSRule() const;
    void setQualifiedBy95HSRule(bool newQualifiedBy95HSRule);

    static StartListCompetitorStatus *getStatusOfJumper(Jumper * jumper, QVector<StartListCompetitorStatus> &statuses);
    static int remainingJumpers(const QVector<StartListCompetitorStatus> &statuses);
};

#endif // STARTLISTCOMPETITORSTATUS_H
