#ifndef COMPETITIONSINGLERESULT_H
#define COMPETITIONSINGLERESULT_H

#include "../utilities/Identifiable.h"
#include "../simulator/JumpData.h"
#include "../simulator/Team.h"
#include <QVector>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSet>
#include <dpp/dpp.h>

class CompetitionInfo;
class Classification;

class CompetitionSingleResult : public Identifiable
{
public:
    CompetitionSingleResult(CompetitionInfo * competition = nullptr, Jumper * jumper = nullptr, int type = 0);
    CompetitionSingleResult(CompetitionInfo *competition, Team * team, int type = 0);
    inline bool operator<(const CompetitionSingleResult & second) {return pointsSum < second.getPointsSum();}
    //inline bool operator>(const CompetitionSingleResult & second) {return pointsSum > second.getPointsSum();}
    enum Type{
        IndividualResult,
        TeamResult
    };

    static CompetitionSingleResult getFromJson(QJsonObject obj, IdentifiableObjectsStorage *storage);
    static QJsonObject getJsonObject(CompetitionSingleResult result);
    static CompetitionSingleResult getFromJsonValue(QJsonValue val, IdentifiableObjectsStorage *storage);

    static QVector<CompetitionSingleResult *> getFilteredSingleResults(QVector<CompetitionInfo *> & competitions, Jumper * jumper, QSet<int> serieTypes, QSet<int> hillTypes,
                                                                       QVector<Classification *> classifications, bool skipClassifications, Hill *specificHill);

    static QHash<Jumper *, QVector<CompetitionSingleResult *>> getJumpersFilteredSingleResults(QVector<Jumper *> & jumpers, QVector<CompetitionInfo *> & competitions, QSet<int> serieTypes, QSet<int> hillTypes,
                                                                       QVector<Classification *> classifications, bool skipClassifications, Hill *specificHill);
    static QHash<QString, QVector<CompetitionSingleResult *>> getTeamsFilteredSingleResults(QVector<QString> & teams, QVector<CompetitionInfo *> & competitions, QSet<int> serieTypes, QSet<int> hillTypes,
                                                                                               QVector<Classification *> classifications, bool skipClassifications, Hill *specificHill);
    static QVector<CompetitionSingleResult *> getFilteredSingleResultsForTeam(QVector<CompetitionInfo *> & competitions, QString countryCode, QSet<int> serieTypes, QSet<int> hillTypes,
                                                                       QVector<Classification *> classifications, bool skipClassifications, Hill *specificHill);

    QString getCsvResultsObject();

    QString getIndividualJumpsSummaryText();
    QString getTeamJumpsSummaryText();

    dpp::embed getEmbedForIndividualSingleCompResult();
    dpp::embed getEmbedForTeamSingleCompResult();

protected:
    CompetitionInfo * competition;
    QVector<JumpData> jumps;
    double pointsSum;
    int position;

    int type;
    Jumper * jumper;
    Team * team;
    QVector<CompetitionSingleResult> teamJumpersResults;

public:
    void updatePointsSum();
    void updateTeamJumpersResults();
    CompetitionSingleResult * getTeamJumperResult(Jumper * jumper);

    QVector<JumpData> getJumps() const;
    QVector<JumpData> & getJumpsReference();
    void setJumps(const QVector<JumpData> &newJumps);
    double getPointsSum() const;
    void setPointsSum(double newPointsSum);
    int getType() const;
    void setType(int newType);
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
    Team *getTeam() const;
    void setTeam(Team *newTeam);
    int getPosition() const;
    void setPosition(int newPosition);
    QVector<CompetitionSingleResult> getTeamJumpersResults() const;
    QVector<CompetitionSingleResult> & getTeamJumpersResultsReference();
    void setTeamJumpersResults(const QVector<CompetitionSingleResult> &newTeamJumpersResults);
    CompetitionInfo *getCompetition() const;
    void setCompetition(CompetitionInfo *newCompetition);
};

inline bool operator>(const CompetitionSingleResult & l, const CompetitionSingleResult & r)
{
    if(l.getPointsSum() == r.getPointsSum())
    {
        return l.getID().str() > r.getID().str();
    }
    else return l.getPointsSum() > r.getPointsSum();
}

inline bool operator<(const CompetitionSingleResult & l, const CompetitionSingleResult & r)
{
    if(l.getPointsSum() == r.getPointsSum())
    {
        return l.getID() < r.getID();
    }
    else return l.getPointsSum() < r.getPointsSum();
}


#endif // COMPETITIONSINGLERESULT_H
