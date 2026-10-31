#ifndef SIMULATIONSAVE_H
#define SIMULATIONSAVE_H

#include <QVector>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "Season.h"
#include "../utilities/Identifiable.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../form-generator/JumperFormTendence.h"
#include "../seasons/SaveJumpersList.h"

class SimulationSave : public Identifiable, public IdentifiableObjectsStorage
{
public:
    SimulationSave();
    ~SimulationSave();

    static SimulationSave *getFromJson(QJsonObject obj, IdentifiableObjectsStorage *storage);
    static QJsonObject getJsonObject(SimulationSave & save);

    bool saveToFile(QString dir = "", QString fileName = "!default");
    static SimulationSave *loadFromFile(QString filePath);
    void updateNextCompetitionIndex();
    void repairDatabase();
    void fixJumpersFormInstabilities();
    double * getJumperFormInstability(Jumper *j);
    int getEngineVersion() const {return engineVersion;}
    bool setEngineVersion(int value) {if(value!=1 && value!=2)return false;engineVersion=value;return true;}
    bool getAutoSquadsEnabled() const { return autoSquadsEnabled; }
    void setAutoSquadsEnabled(bool value) { autoSquadsEnabled=value; }
    bool getAutoGateEnabled() const { return autoGateEnabled; }
    void setAutoGateEnabled(bool value) { autoGateEnabled=value; }
    bool getAutosaveEnabled() const { return autosaveEnabled; }
    void setAutosaveEnabled(bool enabled) { autosaveEnabled = enabled; }
    // Shared by the season overview, calendar assignment and contest preparation.
    static constexpr bool activeRosterFeatureEnabled() { return true; }
    QString activeJumpersListId() const;
    const SaveJumpersList *activeJumpersList() const;
    QVector<Jumper *> activeJumpers() const;
    bool setActiveJumpersListId(const QString &id);

private:
    int engineVersion=1;
    bool autosaveEnabled=true;
    bool autoSquadsEnabled=true,autoGateEnabled=true;
    QString activeListId;
    QString name;
    QVector<Season> seasons;
    Season * actualSeason;
    QVector<Jumper *> jumpers;
    QVector<Hill *> hills;
    QVector<CompetitionRules> competitionRules;
    QVector<SaveJumpersList> jumpersLists;
    QHash<Jumper *, double> jumpersFormInstabilities;

    CompetitionInfo * nextCompetition;
    int nextCompetitionIndex;

    //SETTINGS
    bool showForm;
    bool showInstability;
    bool saveFileSizeReduce;

public:
    QString getName() const;
    void setName(const QString &newName);
    QVector<Season> getSeasons() const;
    QVector<Season>& getSeasonsReference();
    void setSeasons(const QVector<Season> &newSeasons);
    QVector<Jumper *> getJumpers() const;
    QVector<Jumper *> & getJumpersReference();
    void setJumpers(const QVector<Jumper *> &newJumpers);
    QVector<Hill *> & getHillsReference();
    void setHills(const QVector<Hill *> &newHills);
    QVector<Hill *> getHills() const;
    QVector<CompetitionRules> & getCompetitionRulesReference();
    void setCompetitionRules(const QVector<CompetitionRules> &newCompetitionRules);
    Season *getActualSeason() const;
    void setActualSeason(Season *newActualSeason);
    int getNextCompetitionIndex() const;
    void setNextCompetitionIndex(int newNextCompetitionIndex);
    CompetitionInfo *getNextCompetition() const;
    void setNextCompetition(CompetitionInfo *newNextCompetition);
    bool getShowForm() const;
    void setShowForm(bool newShowForm);
    bool getSaveFileSizeReduce() const;
    void setSaveFileSizeReduce(bool newSaveFileSizeReduce);
    QVector<SaveJumpersList> getJumpersLists() const;
    QVector<SaveJumpersList> & getJumpersListsReference();
    void setJumpersLists(const QVector<SaveJumpersList> &newJumpersLists);
    QHash<Jumper *, double> getJumpersFormInstabilities() const;
    QHash<Jumper *, double> & getJumpersFormInstabilitiesReference();
    QVector<QPair<Jumper *, double> > getJumpersFormInstabilitiesToVector();
    void setJumpersFormInstabilities(const QHash<Jumper *, double> &newJumpersFormInstabilities);
    bool getShowInstability() const;
    void setShowInstability(bool newShowInstability);
};

#endif // SIMULATIONSAVE_H

//Co muszę zrobić?
//Znaleźć sposób na wczytanie pewnych rzeczy np. CompetitionInfo tak żeby następne wczytane CompetitionInfo mogły mieć ustawione treningi które zostały wczytane tuż przed aktualnym CompetitionInfo
//Rozwiązanie:
//1. Zrobić w CompetitionInfo funkcję zwracającą sam obiekt a nie vector
//2. Zrobić w klasie Calendar funkcję loadCalendar na podstawie QJsonObject-a która po każdym wczytanym competition daje go do seasonObjectsManagera
