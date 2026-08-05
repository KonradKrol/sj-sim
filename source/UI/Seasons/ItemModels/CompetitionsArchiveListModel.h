#ifndef COMPETITIONSARCHIVELISTMODEL_H
#define COMPETITIONSARCHIVELISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../competitions/CompetitionInfo.h"

class CompetitionsArchiveListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit CompetitionsArchiveListModel(QVector<CompetitionInfo *> * seasonCompetitions, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<CompetitionInfo *> * seasonCompetitions;
public:
    QVector<CompetitionInfo *> *getSeasonCompetitions() const;
    void setSeasonCompetitions(QVector<CompetitionInfo *> *newSeasonCompetitions);
};

#endif // COMPETITIONSARCHIVELISTMODEL_H
