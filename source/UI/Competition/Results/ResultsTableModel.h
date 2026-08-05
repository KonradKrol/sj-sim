#ifndef RESULTSTABLEMODEL_H
#define RESULTSTABLEMODEL_H

#include <QAbstractTableModel>
#include "../../../competitions/CompetitionResults.h"
#include "../../../competitions/AbstractCompetitionManager.h"
#include "../../../competitions/CompetitionInfo.h"

class ResultsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ResultsTableModel(int type = 0, CompetitionResults * results = nullptr, AbstractCompetitionManager * manager = nullptr, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    int type; // 0 - Individual,  1 - Team
    CompetitionResults * results;
    AbstractCompetitionManager * manager;

    QVector<StartListCompetitorStatus> * startListStatuses;

    int qualifiersLimit;

    Jumper * lastJumper;

public:
    CompetitionResults *getResults() const;
    void setResults(CompetitionResults *newResults);
    int getType() const;
    void setType(int newType);
    AbstractCompetitionManager *getManager() const;
    void setManager(AbstractCompetitionManager *newManager);
    QVector<StartListCompetitorStatus> *getStartListStatuses() const;
    void setStartListStatuses(QVector<StartListCompetitorStatus> *newStartListStatuses);
    int getQualifiersLimit() const;
    void setQualifiersLimit(int newQualifiersLimit);
    Jumper *getLastJumper() const;
    void setLastJumper(Jumper *newLastJumper);
};

#endif // RESULTSTABLEMODEL_H
