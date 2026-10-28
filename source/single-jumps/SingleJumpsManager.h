#ifndef SINGLEJUMPSMANAGER_H
#define SINGLEJUMPSMANAGER_H

#include <QVector>
#include <QFuture>
#include <atomic>
#include <memory>
#include "../simulator/JumpData.h"
#include "../simulator/Hill.h"
#include "../simulator/WindsCalculator.h"
#include "../simulator/wind-generation/WindsGenerator.h"
#include "../simulator/wind-generation/WindGenerationSettings.h"
#include "../simulator/JumpSimulator.h"
#include "../competitions/CompetitionRules.h"

class SingleJumpsManager
{
public:
    SingleJumpsManager(int gate = 0, int jumpsCount = 0, const QString &resultsFileName = "", bool changeableWind = false, short resultsFormat = 0);

    void simulate();
    void setSimulationEvent(SimulationV2::EventKind value) {simulationEvent=value;}
    void setSimulationSeed(SimulationV2::Seed value) {simulationSeed=value;}
    void setSimulationWeather(std::shared_ptr<const SimulationV2::WeatherField> value) {simulationWeather=std::move(value);}
    struct BatchControl {
        std::atomic_bool canceled{false};
        std::atomic_int completed{0};
    };
    // Detached value snapshots; bind athlete/hill pointers on the owning thread before contextual use.
    QFuture<QVector<JumpData>> simulateAsync(int count,int threads=0,std::shared_ptr<BatchControl> control={}) const;

    enum ResultsFileFormat{
        Json,
        Csv
    };
    void saveResultsCsv(QString fileName);

private:
    Jumper jumper;
    Hill hill;
    WindsGenerator windsGenerator;
    QVector<WindGenerationSettings> windsGeneratorSettings;
    JumpSimulator jumpSimulator;

    int jumpsCount;
    bool changeableWind;
    short resultsFormat;
    QString resultsFileName;
    short windAverageCalculatingType;
    short windCompensationDistanceEffect;
    int gate;
    int DSQProbability;
    double importance;

    CompetitionRules rules;
    SimulationV2::EventKind simulationEvent=SimulationV2::EventKind::Competition;
    SimulationV2::Seed simulationSeed;
    std::shared_ptr<const SimulationV2::WeatherField> simulationWeather;

    QVector<JumpData> jumps;

public:
    Jumper getJumper() const;
    void setJumper(const Jumper &newJumper);
    Hill getHill() const;
    void setHill(const Hill &newHill);
    int getJumpsCount() const;
    void setJumpsCount(int newJumpsCount);
    QVector<JumpData> getJumps() const;
    QVector<JumpData> & getJumpsReference();
    void setJumps(const QVector<JumpData> &newJumps);
    QVector<WindGenerationSettings> getWindsGeneratorSettings() const;
    void setWindsGeneratorSettings(const QVector<WindGenerationSettings> &newWindsGeneratorSettings);
    bool getChangeableWind() const;
    void setChangeableWind(bool newChangeableWind);
    QString getResultsFileName() const;
    void setResultsFileName(const QString &newResultsFileName);
    int getGate() const;
    void setGate(int newGate);
    short getWindAverageCalculatingType() const;
    void setWindAverageCalculatingType(short newWindAverageCalculatingType);
    short getWindCompensationDistanceEffect() const;
    void setWindCompensationDistanceEffect(short newWindCompensationDistanceEffect);
    int getDSQProbability() const;
    void setDSQProbability(int newDSQProbability);
    CompetitionRules & getRules();
    void setRules(const CompetitionRules &newRules);
    double getImportance() const;
    void setImportance(double newImportance);
};

#endif // SINGLEJUMPSMANAGER_H
