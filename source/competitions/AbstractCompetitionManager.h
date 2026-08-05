#ifndef ABSTRACTCOMPETITIONMANAGER_H
#define ABSTRACTCOMPETITIONMANAGER_H

#include <QObject>
#include "../simulator/JumpSimulator.h"
#include "../simulator/JumpManipulator.h"
#include "CompetitionRules.h"
#include "CompetitionInfo.h"
#include "CompetitionResults.h"
#include "../simulator/wind-generation/WindGenerationSettings.h"
#include "../simulator/wind-generation/WindsGenerator.h"
#include "StartListCompetitorStatus.h"

class AbstractCompetitionManager : public QObject
{
    Q_OBJECT
public:
    AbstractCompetitionManager(short type = CompetitionRules::Individual);

    virtual bool checkRoundEnd();
    virtual bool checkCompetitionEnd();

    void updateToBeatLineDistance();
    void updateToAdvanceLineDistance();
    void updateLeaderResult();
    void updateLastQualifiedResult();
    void updateActualCompetitorPointsToTheLeader();
    void updateLast10Judges();

    int getFirstUnfinishedStartListStatus();
    bool isAllJumpsAreFinished();
    void setActualJumperToNextUnfinished();
    virtual void setupNextRound() {}
    virtual void setupStartListStatusesForActualRound() {}
    virtual void updateCompetitorsAdvanceStatuses() {}

signals:
    void actualStartListIndexChanged();
    void actualJumperChanged();
    void roundEnd();
    void competitionEnd();

protected:
    short type;

    CompetitionInfo * competitionInfo;
    CompetitionRules * competitionRules;
    CompetitionResults * results;

    QVector<StartListCompetitorStatus> startListStatuses;

    QVector<int> roundsStartingGates;
    int actualGate;
    int actualRound; //liczone od 1.
    QVector<Wind> * actualWinds;
    double last10Judges;

    double toBeatLineDistance;
    double toAdvanceLineDistance;
    double actualCompetitorPointsToTheLeader;

    bool coachGateForNextJumper;
    int actualCoachGate;

    Jumper * actualJumper;
    int actualStartListIndex;
    JumpManipulator * actualJumpManipulator;
    QVector<WindGenerationSettings> windGenerationSettings;

    CompetitionSingleResult * leaderResult;
    CompetitionSingleResult * lastQualifiedResult;\

    int baseDSQProbability;

    int altQualifiersLimit;

public:
    short getType() const;
    void setType(short newType);
    CompetitionInfo *getCompetitionInfo() const;
    void setCompetitionInfo(CompetitionInfo *newCompetitionInfo);
    CompetitionRules *getCompetitionRules() const;
    void setCompetitionRules(CompetitionRules *newCompetitionRules);
    CompetitionResults *getResults() const;
    void setResults(CompetitionResults *newResults);
    QVector<int> getRoundsStartingGates() const;
    void setRoundsStartingGates(const QVector<int> &newRoundsStartingGates);
    int getActualGate() const;
    void setActualGate(int newActualGate);
    int getActualRound() const;
    void setActualRound(int newActualRound);
    double getToBeatLineDistance() const;
    void setToBeatLineDistance(double newToBeatLineDistance);
    double getToAdvanceLineDistance() const;
    QVector<int> & getRoundsStartingGatesReference();
    void setToAdvanceLineDistance(double newToAdvanceLineDistance);
    bool getCoachGateForNextJumper() const;
    void setCoachGateForNextJumper(bool newCoachGateForNextJumper);
    int getActualCoachGate() const;
    void setActualCoachGate(int newActualCoachGate);
    int getActualStartListIndex() const;
    void setActualStartListIndex(int newActualStartListIndex);
    double getActualCompetitorPointsToTheLeader() const;
    void setActualCompetitorPointsToTheLeader(double newActualCompetitorPointsToTheLeader);
    CompetitionSingleResult *getLeaderResult() const;
    void setLeaderResult(CompetitionSingleResult *newLeaderResult);
    CompetitionSingleResult *getLastQualifiedResult() const;
    void setLastQualifiedResult(CompetitionSingleResult *newLastQualifiedResult);
    Jumper *getActualJumper() const;
    void setActualJumper(Jumper *newActualJumper);
    QVector<Wind> *getActualWinds() const;
    void setActualWinds(QVector<Wind> *newActualWinds);
    JumpManipulator *getActualJumpManipulator() const;
    void setActualJumpManipulator(JumpManipulator *newActualJumpManipulator);
    int getBaseDSQProbability() const;
    void setBaseDSQProbability(int newBaseDSQProbability);
    QVector<WindGenerationSettings> getWindGenerationSettings() const;
    void setWindGenerationSettings(const QVector<WindGenerationSettings> &newWindGenerationSettings);
    QVector<WindGenerationSettings> & getWindGenerationSettingsReference();
    QVector<StartListCompetitorStatus> getStartListStatuses() const;
    QVector<StartListCompetitorStatus> & getStartListStatusesReference();
    void setStartListStatuses(const QVector<StartListCompetitorStatus> &newStartListStatuses);
    int getAltQualifiersLimit() const;
    void setAltQualifiersLimit(int newAltQualifiersLimit);
    double getLast10Judges() const;
    void setLast10Judges(double newLast10Judges);
};

#endif // ABSTRACTCOMPETITIONMANAGER_H
