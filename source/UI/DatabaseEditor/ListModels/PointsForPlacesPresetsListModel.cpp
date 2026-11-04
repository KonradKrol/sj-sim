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

    return presetsVectorPointer?presetsVectorPointer->count():0;
}

int PointsForPlacesPresetsListModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid()?0:1;
}

QVariant PointsForPlacesPresetsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row()<0 || index.row()>=rowCount())
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

bool PointsForPlacesPresetsListModel::insertRows(int row,int count,const QModelIndex &parent)
{
    if(parent.isValid() || !presetsVectorPointer || row<0 || count<=0 || row>rowCount())return false;
    beginInsertRows({},row,row+count-1);
    for(int i=0;i<count;++i)presetsVectorPointer->insert(row+i,PointsForPlacesPreset(tr("Nowa punktacja")));
    endInsertRows();return true;
}

bool PointsForPlacesPresetsListModel::removeRows(int row,int count,const QModelIndex &parent)
{
    if(parent.isValid() || !presetsVectorPointer || row<0 || count<=0 || row+count>rowCount())return false;
    beginRemoveRows({},row,row+count-1);
    presetsVectorPointer->remove(row,count);
    endRemoveRows();return true;
}

QVector<PointsForPlacesPreset> *PointsForPlacesPresetsListModel::getPresetsVectorPointer() const
{
    return presetsVectorPointer;
}

void PointsForPlacesPresetsListModel::setPresetsVectorPointer(QVector<PointsForPlacesPreset> *newPresetsVectorPointer)
{
    presetsVectorPointer = newPresetsVectorPointer;
}

bool PointsForPlacesPresetsListModel::moveRows(const QModelIndex &source,int row,int count,const QModelIndex &destination,int target) {
    if(source.isValid() || destination.isValid() || !presetsVectorPointer || count!=1 || row<0 || row>=rowCount() || target<0 || target>rowCount() || target==row || target==row+1)return false;
    if(!beginMoveRows({},row,row,{},target))return false;
    presetsVectorPointer->move(row,target>row?target-1:target);endMoveRows();return true;
}
