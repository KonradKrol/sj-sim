#ifndef TEAMRESULTSTREEMODEL_H
#define TEAMRESULTSTREEMODEL_H

#include <QAbstractItemModel>
#include <QPair>
#include "../../../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../../../competitions/CompetitionResults.h"
#include "../../../global/TreeItem.h"

class TeamResultsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TeamResultsTreeModel(TeamCompetitionManager * manager, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    CompetitionResults * results;
    QVector<Team *> * teams;
    TeamCompetitionManager * manager;
    QVector<QPair<Team *, int>> * teamsAdvanceStatuses;
    TreeItem * rootItem;
    Team * lastTeam;
public:
    void setupTreeItems();
public:
    QVector<QPair<Team *, int> > *getTeamsAdvanceStatuses() const;
    void setTeamsAdvanceStatuses(QVector<QPair<Team *, int> > *newTeamsAdvanceStatuses);
    TreeItem *getRootItem() const;
    TeamCompetitionManager *getManager() const;
    void setManager(TeamCompetitionManager *newManager);
    QVector<Team *> *getTeams() const;
    void setTeams(QVector<Team *> *newTeams);
    CompetitionResults *getResults() const;
    void setResults(CompetitionResults *newResults);
    Team *getLastTeam() const;
    void setLastTeam(Team *newLastTeam);
};

#endif // TEAMRESULTSTREEMODEL_H
