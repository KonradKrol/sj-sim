#ifndef JUMPSIMULATOR_H
#define JUMPSIMULATOR_H

#include <QVector>
#include <QPair>

#include "Jumper.h"
#include "JumpData.h"
#include "WindsCalculator.h"
#include "Hill.h"
#include "Landing.h"
#include "JumpManipulator.h"
#include "../simulation-v2/Engine.h"
#include <memory>

#include <QMap>

class CompetitionInfo;

class JumpSimulator
{
public:
    JumpSimulator(Jumper *jumper = nullptr, const QVector<Wind> & winds = QVector<Wind>(), Hill *hill = nullptr);
private:
    bool deterministicPreview=false;
    Jumper * jumper;
    JumperSkills * jumperSkills; //automatycznie od jumper
    QVector<Wind> winds;
    Hill * hill;
    int gate;
    int competitionStartGate;
    double DSQBaseProbability;
    JumpManipulator * manipulator;
    bool hasCoachGate;
    int coachGate;
    double inrunSnow;
    double jumpsImportance;
    SimulationV2::Engine simulationEngine;
    SimulationV2::EventKind simulationEvent=SimulationV2::EventKind::Competition;
    double simulationTime=0;
    std::optional<SimulationV2::Seed> simulationSeed;
    std::shared_ptr<const SimulationV2::WeatherField> simulationWeather;
    std::shared_ptr<const SimulationV2::WindMeasurement> simulationMeasurement;
    void simulateV2();
    double previewV2() const;

    CompetitionRules * competitionRules;

public:
    //skok
    void simulateJump();
    double previewDistance();
    void setSimulationEvent(SimulationV2::EventKind event) { simulationEvent=event; }
    void setSimulationSeed(SimulationV2::Seed seed) {simulationSeed=seed;}
    void setSimulationTime(double seconds) { simulationTime=seconds; }
    void setSimulationParameters(const SimulationV2::EngineParameters &parameters) { simulationEngine=SimulationV2::Engine(parameters); }
    void setSimulationWeather(std::shared_ptr<const SimulationV2::WeatherField>,std::shared_ptr<const SimulationV2::WindMeasurement>);

    void resetTemporaryParameters();
    void updateJumperSkills();
    void setupForNextJump();

private:
    enum RandomParameters{
        TakeoffRating,
        FlightRating,
        LandingInstability
    };
    static double getRandomForJumpSimulation(short parameter, Jumper * jumper, double jumpsImportance);
    static double getLandingChance(short landingType, double distance, Hill * hill, Jumper *jumper);

    bool generateDSQ();
    void generateTakeoffRating();
    void generateFlightRating();
    double getMultiplierForFlightStyleEffect();

    void generateDistance();
    void generateWindEffects();
    void generateInrunSnowEffect();
    void preventVeryLongJumps();
    void generateLanding();
    void generateJudges();
    void calculateCompensations();
    void calculatePoints();

    ///symulacja
    JumpData jumpData;
    void setupJumpData();

    JumpSimulationData * simulationData;

    double getWindSegmentDistance();

public:
    enum WindCompensationDistanceEffect{
        Disabled,
        Original,
        Full
    };
    static bool isJumperBeat95HSPercents(double distance, double HSPoint);
    JumpManipulator *getManipulator() const;
    void setManipulator(JumpManipulator *newManipulator);
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
    Hill *getHill() const;
    void setHill(Hill *newHill);
    double getTakeoffRating() const;
    void setTakeoffRating(double newTakeoffRating);
    double getFlightRating() const;
    void setFlightRating(double newFlightRating);
    JumperSkills *getJumperSkills() const;
    QVector<double> getJudges() const;
    void setJudges(const QVector<double> &newJudges);
    JumpData getJumpData();
    void setJumpData(const JumpData &newJumpData);
    double getDSQBaseProbability() const;
    void setDSQBaseProbability(double newDSQBaseProbability);
    CompetitionRules *getCompetitionRules() const;
    void setCompetitionRules(CompetitionRules *newCompetitionRules);
    QVector<Wind> getWinds() const;
    void setWinds(const QVector<Wind> &newWinds);
    int getCompetitionStartGate() const;
    void setCompetitionStartGate(int newCompetitionStartGate);
    int getGate() const;
    void setGate(int newGate);
    bool getHasCoachGate() const;
    void setHasCoachGate(bool newHasCoachGate);
    int getCoachGate() const;
    void setCoachGate(int newCoachGate);
    JumpSimulationData *getSimulationData();
    void setSimulationData(JumpSimulationData *newSimulationData);
    double getInrunSnow() const;
    void setInrunSnow(double newInrunSnow);
    double getJumpsImportance() const;
    void setJumpsImportance(double newJumpsImportance);
};

#endif // JUMPSIMULATOR_H
