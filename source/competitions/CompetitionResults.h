#ifndef COMPETITIONRESULTS_H
#define COMPETITIONRESULTS_H
#include "CompetitionSingleResult.h"
#include "../utilities/Identifiable.h"
#include <QVector>
#include <QObject>
#include <QDebug>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>
#include "../global/IdentifiableObjectsStorage.h"

class CompetitionInfo;

class CompetitionResults : public Identifiable
{
public:
    CompetitionResults();

private:
    CompetitionInfo * competition;
    QVector<CompetitionSingleResult> results;
    QVector<QVector<JumpData *>> constructRoundsJumps(QVector<RoundInfo> * rounds);

public:
    CompetitionResults constructRoundsResults(QVector<RoundInfo> *roundsInfos, QVector<int> rounds);
    void updateRoundsJumps();

    static CompetitionResults getFromJson(QJsonObject obj, IdentifiableObjectsStorage *storage);
    static QJsonObject getJsonObject(CompetitionResults & results);

    QVector<int> getJumpersPositions(const QVector<Jumper *> *jumpers) const;
    QHash<Jumper *, int> getHashWithJumpersPositions(const QVector<Jumper *> *jumpers, int round = -1) const;
    void sortJumpersByResults(QVector<Jumper *> & jumpers);
    static QHash<Jumper *, int> getResultsWithJumpersPositionsForClassificationArchiveResults(QHash<Jumper *, double> results);
    static QHash<QString, int> getResultsWithTeamsPositionsForClassificationArchiveResults(QHash<QString, double> results);
    static QVector<QString> getTeamsCodesByIndividualResults(CompetitionResults * results);
    static QVector<Jumper *> getJumpersByTeamResults(CompetitionResults * results);

    QHash<Jumper *, int> getPositionsByRound(int round);

    void fixSingleResultsJumps();

public:
    QVector<CompetitionSingleResult> getResults() const;
    QVector<CompetitionSingleResult> & getResultsReference();
    void setResults(const QVector<CompetitionSingleResult> &newResults);

    CompetitionSingleResult * getResultOfIndividualJumper(Jumper * jumper);
    CompetitionSingleResult * getResultOfTeam(Team * team);
    CompetitionSingleResult * getResultByIndex(int index);

    void addJump(Jumper * jumper, JumpData & jump, int jumpNumber = -1);
    void addJump(Team * team, JumpData & jump, int jumpNumber = -1);
    void addJump(CompetitionSingleResult * result, JumpData & jump, int jumpNumber = -1);

    int howManyJumpersJumpedInTeam(Team * team);
    int getIndexOfJumperResult(Jumper * jumper);

    void updatePositions();
    void sortInDescendingOrder();
    void sortInAscendingOrder();
    CompetitionInfo *getCompetition() const;
    void setCompetition(CompetitionInfo *newCompetition);
};

#endif // COMPETITIONRESULTS_H
