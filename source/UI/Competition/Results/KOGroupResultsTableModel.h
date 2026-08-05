#ifndef KOGROUPRESULTSTABLEMODEL_H
#define KOGROUPRESULTSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "../../../competitions/KOSystem/KOGroup.h"
#include "../../../competitions/CompetitionManagers/KORoundManager.h"

class KOGroupResultsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit KOGroupResultsTableModel(KORoundManager * KOManager, KOGroup * group = nullptr, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    KOGroup * group;
    KORoundManager * KOManager;
public:
    KOGroup *getGroup() const;
    void setGroup(KOGroup *newGroup);
    CompetitionResults *getResults() const;
    void setResults(CompetitionResults *newResults);
    KORoundManager *getKOManager() const;
    void setKOManager(KORoundManager *newKOManager);
};

#endif // KOGROUPRESULTSTABLEMODEL_H
