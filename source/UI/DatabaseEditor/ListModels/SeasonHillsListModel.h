#ifndef SEASONHILLSLISTMODEL_H
#define SEASONHILLSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../simulator/Hill.h"

class SeasonHillsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SeasonHillsListModel(QVector<Hill *> * seasonHills, QObject *parent = nullptr);

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
    QVector<Hill *> * seasonHills;
public:
    QVector<Hill *> *getSeasonHills() const;
    void setSeasonHills(QVector<Hill *> *newSeasonHills);
};

#endif // SEASONHILLSLISTMODEL_H
