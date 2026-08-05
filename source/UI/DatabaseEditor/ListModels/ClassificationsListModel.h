#ifndef CLASSIFICATIONSLISTMODEL_H
#define CLASSIFICATIONSLISTMODEL_H

#include <QAbstractListModel>
#include "../../../seasons/Classification.h"

class ClassificationsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ClassificationsListModel(QVector<Classification *> *classificationsVectorPointer, QObject *parent = nullptr);

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
    QVector<Classification *> * classificationsVectorPointer;
public:
    QVector<Classification *> *getClassificationsVectorPointer() const;
    void setClassificationsVectorPointer(QVector<Classification *> *newClassificationsVectorPointer);
};

#endif // CLASSIFICATIONSLISTMODEL_H
