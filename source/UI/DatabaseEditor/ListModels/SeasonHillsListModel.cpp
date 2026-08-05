#include "SeasonHillsListModel.h"

#include "../../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>
#include <QColor>

SeasonHillsListModel::SeasonHillsListModel(QVector<Hill *> *seasonHills, QObject *parent)
    : QAbstractListModel(parent),
    seasonHills(seasonHills)
{
}

QVariant SeasonHillsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int SeasonHillsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return seasonHills->count();
}

QVariant SeasonHillsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        return seasonHills->at(index.row())->getName() + " HS" + QString::number(seasonHills->at(index.row())->getHSPoint());
    }
    else if(role == Qt::DecorationRole){
        return QIcon(CountryFlagsManager::getFlagPixmap(seasonHills->at(index.row())->getCountryCode().toLower()));
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

bool SeasonHillsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool SeasonHillsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<Hill *> *SeasonHillsListModel::getSeasonHills() const
{
    return seasonHills;
}

void SeasonHillsListModel::setSeasonHills(QVector<Hill *> *newSeasonHills)
{
    seasonHills = newSeasonHills;
}
