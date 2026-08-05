#ifndef JUMPERSLISTSLISTMODEL_H
#define JUMPERSLISTSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../seasons/SaveJumpersList.h"

class JumpersListsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit JumpersListsListModel(QVector<SaveJumpersList> * jumpersLists, QObject *parent = nullptr);

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
    QVector<SaveJumpersList> * jumpersLists;
public:
    QVector<SaveJumpersList> *getJumpersLists() const;
    void setJumpersLists(QVector<SaveJumpersList> *newJumpersLists);
};

#endif // JUMPERSLISTSLISTMODEL_H
