#include "PointsForPlacesTableModel.h"
#include <QFont>
#include <QColor>

PointsForPlacesTableModel::PointsForPlacesTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant PointsForPlacesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!

    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole){
            switch(section){
            case 0:
                return "Miejsce";
            case 1:
                return "Punkty";
            }
        }
    }
    return QVariant();
}

bool PointsForPlacesTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int PointsForPlacesTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return pointsForPlaces->count();
}

int PointsForPlacesTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant PointsForPlacesTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if(role == Qt::DisplayRole){
        if(index.column() == 0)
            return index.row() + 1;
        else if(index.column() == 1){
            return pointsForPlaces->value(index.row() + 1);
        }
    }
    else if(role == Qt::FontRole){
        QFont font("Ubuntu", 10);
        if(index.column() == 0)
            font.setBold(true);
        return font;
    }
    else if(role == Qt::ForegroundRole)
        return QColor(qRgb(21, 21, 21));

    return QVariant();
}

bool PointsForPlacesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || index.column() != 1)
        return false;

    bool result;
    int val = value.toInt(&result);

    if (result){
        pointsForPlaces->insert(index.row() + 1, val);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    }

    return result;
}

Qt::ItemFlags PointsForPlacesTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if(index.column() == 0)
        return QAbstractItemModel::flags(index);
    else
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool PointsForPlacesTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool PointsForPlacesTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool PointsForPlacesTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool PointsForPlacesTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

QMap<int, double> *PointsForPlacesTableModel::getPointsForPlaces() const
{
    return pointsForPlaces;
}

void PointsForPlacesTableModel::setPointsForPlaces(QMap<int, double> *newPointsForPlaces)
{
    pointsForPlaces = newPointsForPlaces;
}
