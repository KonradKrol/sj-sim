#include "ClassificationsArchiveListModel.h"

ClassificationsArchiveListModel::ClassificationsArchiveListModel(QVector<Classification *> * seasonClassifications, QObject *parent)
    : QAbstractListModel(parent), seasonClassifications(seasonClassifications)
{
}

QVariant ClassificationsArchiveListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

int ClassificationsArchiveListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return seasonClassifications->count();
}

QVariant ClassificationsArchiveListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if(role == Qt::DisplayRole)
    {
        Classification * classification = seasonClassifications->at(index.row());
        return QString::number(index.row() + 1) + ". " + classification->getName();
    }
    else if(role == Qt::FontRole)
    {
        return QFont("Ubuntu", 10);
    }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    return QVariant();
}

bool ClassificationsArchiveListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool ClassificationsArchiveListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<Classification *> *ClassificationsArchiveListModel::getSeasonClassifications() const
{
    return seasonClassifications;
}

void ClassificationsArchiveListModel::setSeasonClassifications(QVector<Classification *> *newSeasonClassifications)
{
    seasonClassifications = newSeasonClassifications;
}
