#ifndef GLOBALSIMULATIONSETTINGS_H
#define GLOBALSIMULATIONSETTINGS_H


class GlobalSimulationSettings
{
private:
    GlobalSimulationSettings();
    ~GlobalSimulationSettings();
    static GlobalSimulationSettings * m_globalSimulationSettings;
    static thread_local GlobalSimulationSettings *threadSettings;

private:
    int maxSkills;
    double simulationRandomMultiplier;
    double hillsEffectsMultiplier;
    bool autoAdjustHillEffects;
    bool updateGlobalDatabaseRecords;
    int baseDsqProbability;
    double takeoffFormEffect;
    double flightFormEffect;

public:
    struct Snapshot {
        int maxSkills,baseDsqProbability;
        double simulationRandomMultiplier,hillsEffectsMultiplier,takeoffFormEffect,flightFormEffect;
        bool autoAdjustHillEffects,updateGlobalDatabaseRecords;
    };
    class ScopedSnapshot;
    static Snapshot snapshot();
    GlobalSimulationSettings(GlobalSimulationSettings &) = delete;
    static GlobalSimulationSettings * get();

    bool writeToFile();
    bool loadFromFile();

    int getBaseDsqProbability() const;
    void setBaseDsqProbability(int newBaseDsqProbability);
    int getMaxSkills() const;
    void setMaxSkills(int newMaxSkills);
    double getSimulationRandomMultiplier() const;
    void setSimulationRandomMultiplier(double newSimulationRandomMultiplier);
    bool getAutoAdjustHillEffects() const;
    void setAutoAdjustHillEffects(bool newAutoAdjustHillEffects);
    double getHillsEffectsMultiplier() const;
    void setHillsEffectsMultiplier(double newHillsEffectsMultiplier);
    bool getUpdateGlobalDatabaseRecords() const;
    void setUpdateGlobalDatabaseRecords(bool newUpdateGlobalDatabaseRecords);
    double getTakeoffFormEffect() const;
    void setTakeoffFormEffect(double newTakeoffFormEffect);
    double getFlightFormEffect() const;
    void setFlightFormEffect(double newFlightFormEffect);
};

class GlobalSimulationSettings::ScopedSnapshot {
    GlobalSimulationSettings local;
    GlobalSimulationSettings *previous;
public:
    explicit ScopedSnapshot(const Snapshot &snapshot);
    ~ScopedSnapshot();
    ScopedSnapshot(const ScopedSnapshot &)=delete;
    ScopedSnapshot &operator=(const ScopedSnapshot &)=delete;
};

#endif // GLOBALSIMULATIONSETTINGS_H
