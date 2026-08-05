#ifndef GLOBALDATABASE_H
#define GLOBALDATABASE_H

#include "../simulator/Jumper.h"
#include "../simulator/Hill.h"
#include "../competitions/CompetitionRules.h"
#include "../seasons/SimulationSave.h"
#include "../seasons/SeasonCalendarPreset.h"
#include "../form-generator/JumperFormGeneratorSettingsPreset.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "JumpsImportancePreset.h"
#include "PointsForPlacesPreset.h"
#include "Country.h"

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QtConcurrent>

class GlobalDatabase : public IdentifiableObjectsStorage
{
private:
    GlobalDatabase(); 
    ~GlobalDatabase();
    static GlobalDatabase * m_globalDatabase;

    QVector<Jumper> globalJumpers;
    QVector<Hill> globalHills;
    QVector<CompetitionRules> globalCompetitionsRules;
    QVector<SimulationSave *> globalSimulationSaves;
    QVector<PointsForPlacesPreset> globalPointsForPlacesPresets;
    QVector<SeasonCalendarPreset> globalCalendarPresets;
    QVector<JumperFormGeneratorSettingsPreset> formGeneratorPresets;
    QVector<JumpsImportancePreset> jumpsImportancePresets;
    QVector<Country> countries;

    bool loadedSimulationSaves;

public:
    GlobalDatabase(GlobalDatabase &) = delete;
    static GlobalDatabase * get();

    void repairDatabase();
    void repairSimulationSaves();

    void removeJumper(int index);

    bool loadFromJson();
    bool writeToJson();

    bool loadJumpers();
    bool loadHills();
    bool loadCompetitionsRules();
    bool loadSimulationSaves(bool progressDialog = false);
    bool loadPointsForPlacesPresets();
    bool loadCalendarPresets(bool before1_2_0 = false);
    bool loadFormGeneratorPresets();
    bool loadJumpsImportancePresets();
    bool loadCountries();

    bool writeJumpers();
    bool writeHills();
    bool writeCompetitionsRules();
    bool writeSimulationSaves();
    bool writePointsForPlacesPresets();
    bool writeCalendarPresets();
    bool writeFormGeneratorPresets();
    bool writeJumpsImportancePresets();
    bool writeCountries();

    void setupHillsFlags();

    Country *getCountryByAlpha2(QString alpha2);
    Country *getCountryByAlpha3(QString alpha3);

    QVector<Jumper> getGlobalJumpers() const;
    void setGlobalJumpers(const QVector<Jumper> &newGlobalJumpers);
    QVector<Jumper> & getEditableGlobalJumpers();
    QVector<Hill> getGlobalHills() const;
    void setGlobalHills(const QVector<Hill> &newGlobalHills);
    QVector<Hill> & getEditableGlobalHills();
    QVector<CompetitionRules> getGlobalCompetitionsRules() const;
    void setGlobalCompetitionsRules(const QVector<CompetitionRules> &newGlobalCompetitionsRules);
    QVector<CompetitionRules> & getEditableCompetitionRules();
    QVector<SimulationSave *> getGlobalSimulationSaves() const;
    QVector<SimulationSave *> & getEditableGlobalSimulationSaves();
    void setGlobalSimulationSaves(const QVector<SimulationSave *> &newGlobalSimulationSaves);
    QVector<PointsForPlacesPreset> getGlobalPointsForPlacesPresets() const;
    QVector<PointsForPlacesPreset> & getEditableGlobalPointsForPlacesPresets();
    void setGlobalPointsForPlacesPresets(const QVector<PointsForPlacesPreset> &newGlobalPointsForPlacesPresets);
    QVector<SeasonCalendarPreset> getGlobalCalendarPresets() const;
    QVector<SeasonCalendarPreset> & getEditableGlobalCalendarPresets();
    void setGlobalCalendarPresets(const QVector<SeasonCalendarPreset> &newGlobalCalendarPresets);
    bool getLoadedSimulationSaves() const;
    void setLoadedSimulationSaves(bool newLoadedSimulationSaves);
    QVector<JumperFormGeneratorSettingsPreset> getFormGeneratorPresets() const;
    QVector<JumperFormGeneratorSettingsPreset> & getEditableFormGeneratorPresets();
    void setFormGeneratorPresets(const QVector<JumperFormGeneratorSettingsPreset> &newFormGeneratorPresets);
    QVector<JumpsImportancePreset> getJumpsImportancePresets() const;
    QVector<JumpsImportancePreset> & getJumpsImportancePresetsReference();
    void setJumpsImportancePresets(const QVector<JumpsImportancePreset> &newJumpsImportancePresets);
    QVector<Country> getCountries() const;
    QVector<Country> & getCountriesReference();
    void setCountries(const QVector<Country> &newCountries);
};

#endif // GLOBALDATABASE_H
