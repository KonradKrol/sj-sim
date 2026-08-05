#ifndef CLASSIFICATIONSARCHIVELISTMODEL_H
#define CLASSIFICATIONSARCHIVELISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../seasons/Classification.h"

class ClassificationsArchiveListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ClassificationsArchiveListModel(QVector<Classification *> * seasonClassifications, QObject *parent = nullptr);

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
    QVector<Classification *> * seasonClassifications;
public:
    QVector<Classification *> *getSeasonClassifications() const;
    void setSeasonClassifications(QVector<Classification *> *newSeasonClassifications);
};

#endif // CLASSIFICATIONSARCHIVELISTMODEL_H
