#include "PointsForPlacesPresetsListModel.h"
#include "../../../global/PointsForPlacesPreset.h"
#include <QFont>
#include <QColor>

PointsForPlacesPresetsListModel::PointsForPlacesPresetsListModel(QVector<PointsForPlacesPreset> * presetsVectorPointer, QObject *parent)
    : QAbstractListModel(parent), presetsVectorPointer(presetsVectorPointer)
{
}

QVariant PointsForPlacesPresetsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

int PointsForPlacesPresetsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return presetsVectorPointer->count();
}

int PointsForPlacesPresetsListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant PointsForPlacesPresetsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        return presetsVectorPointer->at(index.row()).getName();
    }
    else if(role == Qt::FontRole){
        return QFont("Quicksand Medium", 11, 600);
    }
    else if(role == Qt::BackgroundRole){
        return QColor(qRgb(253, 253, 253));
    }
    else if(role == Qt::ForegroundRole){
        return QColor(qRgb(30, 30, 30));
    }


    // FIXME: Implement me!
    return QVariant();
}

bool PointsForPlacesPresetsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool PointsForPlacesPresetsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<PointsForPlacesPreset> *PointsForPlacesPresetsListModel::getPresetsVectorPointer() const
{
    return presetsVectorPointer;
}

void PointsForPlacesPresetsListModel::setPresetsVectorPointer(QVector<PointsForPlacesPreset> *newPresetsVectorPointer)
{
    presetsVectorPointer = newPresetsVectorPointer;
}
