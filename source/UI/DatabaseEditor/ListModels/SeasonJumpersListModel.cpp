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

    return seasonJumpers ? seasonJumpers->count() : 0;
}

QVariant SeasonJumpersListModel::data(const QModelIndex &index, int role) const
{
    if (!seasonJumpers || !index.isValid() || index.row()<0 || index.row()>=seasonJumpers->size() || !seasonJumpers->at(index.row()))
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

bool SeasonJumpersListModel::insertRows(int, int, const QModelIndex &)
{
    // Insertion requires an owned season object; use insertItem().
    return false;
}

bool SeasonJumpersListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!seasonJumpers || parent.isValid() || row<0 || count<=0 || row>seasonJumpers->size() || count>seasonJumpers->size()-row) return false;
    beginRemoveRows(parent, row, row + count - 1);
    seasonJumpers->remove(row,count);
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
    beginResetModel();
    seasonJumpers = newSeasonJumpers;
    endResetModel();
}

bool SeasonJumpersListModel::insertItem(int row, Jumper *value)
{
    if(!seasonJumpers || !value || row<0 || row>seasonJumpers->size())return false;
    beginInsertRows(QModelIndex(),row,row);
    seasonJumpers->insert(row,value);
    endInsertRows();
    return true;
}
