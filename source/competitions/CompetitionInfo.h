#ifndef COMPETITIONINFO_H
#define COMPETITIONINFO_H

class Hill;
class SeasonCalendar;
class AbstractCompetitionManager;
class Season;
#include "CompetitionRules.h"
#include "CompetitionResults.h"
#include "CompetitionInfo.h"
#include "../utilities/Identifiable.h"
#include "../seasons/Classification.h"
#include "../seasons/SeasonCalendar.h"
#include "KOSystem/KOGroup.h"

#include <QDate>

class CompetitionInfo : public Identifiable
{
public:
    CompetitionInfo(Hill * hill = nullptr);
    ~CompetitionInfo();

    enum SerieType{
        Competition,
        Qualifications,
        TrialRound,
        Training
    };

    void updateQualifyingCompetitions(SeasonCalendar * calendar);
    bool saveToJsonFile(QString dir, QString name);
    bool saveToCsvFile(QString dir, QString name);

    static QVector<CompetitionInfo *> getSpecificTypeMainCompetitions(QVector<CompetitionInfo *> &competitions, int type);
    static QVector<CompetitionInfo *> getSpecificTypeCompetitions(QVector<CompetitionInfo *> &competitions, int type);
    static QVector<CompetitionInfo *> getCompetitionsByStartAndEnd(QVector<CompetitionInfo *> competitions, CompetitionInfo * start, CompetitionInfo * end);
    static QVector<CompetitionInfo *> mergeSeasonsCompetitions(QVector<Season> *seasons, QString filter, bool mergeCalendars);
    static int howManyPlayedStatic(QVector<CompetitionInfo *> comps);

    QString getShortSerieTypeText();
    QString getLongSerieTypeText();

    QString getSingleResultsTextForWebhook(AbstractCompetitionManager *manager);
    dpp::message getResultsWebhookMessage(AbstractCompetitionManager *manager);
    void sendResultsWebhook(AbstractCompetitionManager *manager);

private:
    Hill * hill;
    CompetitionRules rules;
    CompetitionResults results;
    short serieType;
    int exceptionalRoundsCount;
    double jumpsImportance;

    bool cancelled;
    bool played;

    CompetitionInfo * trialRound;
    QVector<CompetitionInfo *> trainings;
    QVector<Classification *> classifications;

    Classification * advancementClassification;
    CompetitionInfo * advancementCompetition; //Tylko jedno z tych 2 może być "aktywne"
    QVector<CompetitionInfo *> qualifyingCompetitions; //Konkursy dla których ten konkurs jest kwalifikacjami

    QVector<Team> teams;
    QVector<QVector<KOGroup>> roundsKOGroups;
    QVector<Jumper *> startList;

public:
    static QJsonObject getJsonObject(CompetitionInfo &competition);
    static CompetitionInfo getFromJson(const QJsonObject & json, IdentifiableObjectsStorage *storage, int *oldId = nullptr, QMap<ulong, Identifiable *> *before120Map = nullptr);

public:
    Hill *getHill() const;
    void setHill(Hill *newHill);
    CompetitionRules getRules() const;
    CompetitionRules * getRulesPointer() {return &rules;}
    void setRules(const CompetitionRules &newRules);
    short getSerieType() const;
    void setSerieType(short newSerieType);
    int getExceptionalRoundsCount() const;
    void setExceptionalRoundsCount(int newExceptionalRoundsCount);
    bool getCancelled() const;
    void setCancelled(bool newCancelled);
    CompetitionInfo *getTrialRound() const;
    void setTrialRound(CompetitionInfo *newTrialRound);
    QVector<CompetitionInfo *> getTrainings() const;
    QVector<CompetitionInfo *> & getTrainingsReference();
    void setTrainings(const QVector<CompetitionInfo *> &newTrainings);
    QVector<Classification *> getClassifications() const;
    QVector<Classification *> & getClassificationsReference();
    void setClassifications(const QVector<Classification *> &newClassifications);
    CompetitionResults getResults() const;
    void setResults(const CompetitionResults &newResults);
    CompetitionResults & getResultsReference();
    Classification *getAdvancementClassification() const;
    void setAdvancementClassification(Classification *newAdvancementClassification);
    CompetitionInfo *getAdvancementCompetition() const;
    void setAdvancementCompetition(CompetitionInfo *newAdvancementCompetition);
    QVector<CompetitionInfo *> getQualifyingCompetitions() const;
    QVector<CompetitionInfo *> & getQualifyingCompetitionsReference();
    void setQualifyingCompetitions(const QVector<CompetitionInfo *> &newQualifyingCompetitions);
    bool getPlayed() const;
    void setPlayed(bool newPlayed);
    QVector<Team> getTeams() const;
    QVector<Team> & getTeamsReference();
    void setTeams(const QVector<Team> &newTeams);
    QVector<QVector<KOGroup> > & getRoundsKOGroupsReference();
    QVector<QVector<KOGroup> > getRoundsKOGroups() const;
    void setRoundsKOGroups(const QVector<QVector<KOGroup> > &newRoundsKOGroups);
    double getJumpsImportance() const;
    void setJumpsImportance(double newJumpsImportance);
    QVector<Jumper *> getStartList() const;
    QVector<Jumper *> & getStartListReference();
    void setStartList(const QVector<Jumper *> &newStartList);
};

#endif // COMPETITIONINFO_H
