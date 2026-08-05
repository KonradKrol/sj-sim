#include "SeasonJumpersListModel.h"

#include "../../../simulator/Jumper.h"
#include "../../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>
#include <QColor>

SeasonJumpersListModel::SeasonJumpersListModel(QVector<Jumper *> *seasonJumpers, QObject *parent)
    : QAbstractListModel(parent),
    seasonJumpers(seasonJumpers)
{
}

QVariant SeasonJumpersListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int SeasonJumpersListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return seasonJumpers->count();
}

QVariant SeasonJumpersListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if(role == Qt::DisplayRole){
        QString string;
        if(showItemsNumbers)
            string += QString::number(index.row() + 1) + ". ";
        string += seasonJumpers->at(index.row())->getNameAndSurname();
        return string;
    }
    else if(role == Qt::DecorationRole){
         return QIcon(CountryFlagsManager::getFlagPixmap(seasonJumpers->at(index.row())->getCountryCode().toLower()));
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

bool SeasonJumpersListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool SeasonJumpersListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool SeasonJumpersListModel::getShowItemsNumbers() const
{
    return showItemsNumbers;
}

void SeasonJumpersListModel::setShowItemsNumbers(bool newShowItemsNumbers)
{
    showItemsNumbers = newShowItemsNumbers;
}

QVector<Jumper *> *SeasonJumpersListModel::getSeasonJumpers() const
{
    return seasonJumpers;
}

void SeasonJumpersListModel::setSeasonJumpers(QVector<Jumper *> *newSeasonJumpers)
{
    seasonJumpers = newSeasonJumpers;
}
