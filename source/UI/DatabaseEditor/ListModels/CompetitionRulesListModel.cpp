#include "CompetitionRulesListModel.h"

#include "../../../competitions/CompetitionRules.h"
#include "../../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>
#include <QColor>

CompetitionRulesListModel::CompetitionRulesListModel(QVector<CompetitionRules> *competitonRulesVectorPointer, QObject *parent)
    : QAbstractListModel(parent),
      competitonRulesVectorPointer(competitonRulesVectorPointer)
{
}

QVariant CompetitionRulesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

bool CompetitionRulesListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int CompetitionRulesListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return competitonRulesVectorPointer->count();
}

QVariant CompetitionRulesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        return competitonRulesVectorPointer->at(index.row()).getName();
    }
    else if(role == Qt::DecorationRole){
        return QVariant();
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

bool CompetitionRulesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CompetitionRulesListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index); // FIXME: Implement me!
}

bool CompetitionRulesListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool CompetitionRulesListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<CompetitionRules> *CompetitionRulesListModel::getCompetitonRulesVectorPointer() const
{
    return competitonRulesVectorPointer;
}

void CompetitionRulesListModel::setCompetitonRulesVectorPointer(QVector<CompetitionRules> *newCompetitonRulesVectorPointer)
{
    competitonRulesVectorPointer = newCompetitonRulesVectorPointer;
}
