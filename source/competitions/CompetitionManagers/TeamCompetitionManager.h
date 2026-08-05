#ifndef TEAMCOMPETITIONMANAGER_H
#define TEAMCOMPETITIONMANAGER_H

#include "../AbstractCompetitionManager.h"
#include "../../simulator/Team.h"

class TeamCompetitionManager : public AbstractCompetitionManager
{
    Q_OBJECT
public:
    TeamCompetitionManager();
    void setActualTeamToActualJumperTeam();

    void setupNextRound();
    void setupStartListStatusesForActualRound(bool setupBeforeCompetition  = false); // zrobienie grup list startowych
    void updateCompetitorsAdvanceStatuses();

    bool checkCompetitionEnd() override;
    bool checkRoundEnd() override;
    bool checkGroupEnd();
    void setupNextGroup();
    void sortActualRoundJumpersForActualGroup();

    int getAdvanceStatusOfTeam(Team * team);
    QVector<Team *> getFilteredTeamsForNextRound();
    void updateTeamsVectorByQualifications(QVector<Team> & teamsVectorToModify);

    static QVector<Team> getFilteredTeamsAfterQualifications(CompetitionInfo *competition);
    static QVector<Team> getFilteredTeamsByClassification(CompetitionInfo *competition, Classification *classification, QVector<Team> & teams);

    static void setStartListOrderByClassification(QVector<Team> &teams, Classification * classification);
    static void setStartListOrderByCompetitionResults(QVector<Team>& teams, CompetitionInfo * competition);
    static void setStartListOrderRandomly(QVector<Team> &teams);

private:
    int actualGroup;
    Team * actualTeam;
    QVector<QVector<Team *>> roundsTeams;
    QVector<QPair<Team *, int>> teamsAdvanceStatuses;

public:
    Team *getActualTeam() const;
    void setActualTeam(Team *newActualTeam);
    QVector<QVector<Team *> > getRoundsTeams() const;
    QVector<QVector<Team *> > & getRoundsTeamsReference();
    void setRoundsTeams(const QVector<QVector<Team *> > &newRoundTeams);
    QVector<Team *>  & getActualRoundTeamsReference();
    QVector<QPair<Team *, int> > getTeamsAdvanceStatuses() const;
     QVector<QPair<Team *, int> > & getTeamsAdvanceStatusesReference();
    void setTeamsAdvanceStatuses(const QVector<QPair<Team *, int> > &newTeamsAdvanceStatuses);
    int getActualGroup() const;
    void setActualGroup(int newActualGroup);

signals:
    void actualTeamChanged();
    void actualGroupChanged();
};

#endif // TEAMCOMPETITIONMANAGER_H
