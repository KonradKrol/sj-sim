#include "HillsListModel.h"

#include "../../../simulator/Hill.h"
#include "../../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>
#include <QColor>

HillsListModel::HillsListModel(QVector<Hill> *hillsVectorPointer, QObject *parent) : QAbstractListModel(parent),
    hillsVectorPointer(hillsVectorPointer)
{
}

QVariant HillsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

bool HillsListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int HillsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return hillsVectorPointer->count();
}


QVariant HillsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        return hillsVectorPointer->at(index.row()).getName() + " HS" + QString::number(hillsVectorPointer->at(index.row()).getHSPoint());
    }
    else if(role == Qt::DecorationRole){
        return QIcon(CountryFlagsManager::getFlagPixmap(hillsVectorPointer->at(index.row()).getCountryCode().toLower()));
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

bool HillsListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole){
        if (data(index, role) != value) {
            // FIXME: Implement me!
            emit dataChanged(index, index, {role});
            return true;
        }
    }
    return false;
}

Qt::ItemFlags HillsListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index); // FIXME: Implement me!
}

bool HillsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool HillsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<Hill> *HillsListModel::getHillsVectorPointer() const
{
    return hillsVectorPointer;
}

void HillsListModel::setHillsVectorPointer(QVector<Hill> *newHillsVectorPointer)
{
    hillsVectorPointer = newHillsVectorPointer;
}
