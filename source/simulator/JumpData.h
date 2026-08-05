#ifndef JUMPDATA_H
#define JUMPDATA_H

#include <QVector>
#include <QJsonObject>

#include "../utilities/Identifiable.h"
#include "../competitions/CompetitionRules.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "WindsCalculator.h"
#include "Landing.h"
#include "Jumper.h"
#include "Hill.h"
#include "JumpSimulationData.h"

class JumpSimulator;
class CompetitionSingleResult;
class CompetitionInfo;

class JumpData : public Identifiable
{
public:
    friend class JumpSimulator;
    JumpData(Jumper *jumper = nullptr, Hill *hill = nullptr);

    QString getJudgesText();
    QString getWindsText();
    int getPositionInRound();
    int getPositionInGroupForTeamCompetition();
    int getPositionInRoundForTeamCompetition();
    int getPositionInCompetitionForTeamCompetition();
private:
    int gate;
    double distance;
    double points;
    double gateCompensation;
    double windCompensation;
    double totalCompensation;
    double judgesPoints;

    Landing landing;
    QVector<double> judges;

    JumpSimulationData simulationData;

    QVector<Wind> winds;
    double averagedWind;

    bool DSQ;
    bool DNS;

    bool hasCoachGate;
    int coachGate;
    bool beats95HSPercents;

    Jumper * jumper;
    Hill * hill;
    JumpSimulator * simulator;
    CompetitionInfo * competition;
    CompetitionSingleResult * singleResult;

    double jumperForm;

    bool inSingleJumps;

public:
    void reset();
    void setupForNextJumper();

    static QJsonObject getJsonObject(JumpData jumpData);
    static JumpData getFromJson(QJsonObject obj, IdentifiableObjectsStorage *storage);

    double getDistance() const;
    void setDistance(double newDistance);
    double getPoints() const;
    void setPoints(double newPoints);
    double getGateCompensation() const;
    void setGateCompensation(double newGateCompensation);
    double getWindCompensation() const;
    void setWindCompensation(double newWindCompensation);
    double getTotalCompensation() const;
    void setTotalCompensation(double newTotalCompensation);
    Landing getLanding() const;
    void setLanding(const Landing &newLanding);
    QVector<double> getJudges() const;
    void setJudges(const QVector<double> &newJudges);
    double getJudgesPoints() const;
    void setJudgesPoints(double newJudgesPoints);
    Hill *getHill() const;
    void setHill(Hill *newHill);
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
    JumpSimulator *getSimulator() const;
    void setSimulator(JumpSimulator *newSimulator);
    int getGate() const;
    void setGate(int newGate);
    double getAveragedWind() const;
    void setAveragedWind(double newAveragedWind);
    JumpSimulationData &getSimulationDataReference();
    void setSimulationData(const JumpSimulationData &newSimulationData);
    QVector<Wind> getWinds() const;
    QVector<Wind>* getWindsPointer();
    void setWinds(const QVector<Wind> &newWinds);
    bool getDNS() const;
    void setDNS(bool newDNS);
    bool getHasCoachGate() const;
    void setHasCoachGate(bool newHasCoachGate);
    int getCoachGate() const;
    void setCoachGate(int newCoachGate);
    bool getBeats95HSPercents() const;
    void setBeats95HSPercents(bool newBeats95HSPercents);
    bool getDSQ() const;
    void setDSQ(bool newDSQ);
    double getJumperForm() const;
    void setJumperForm(double newJumperForm);
    CompetitionInfo *getCompetition() const;
    void setCompetition(CompetitionInfo *newCompetition);
    CompetitionSingleResult *getSingleResult() const;
    void setSingleResult(CompetitionSingleResult *newSingleResult);
    bool getInSingleJumps() const;
    void setInSingleJumps(bool newInSingleJumps);
};

QDebug operator<<(QDebug d, const JumpData & jumpData);

#endif // JUMPDATA_H
