#include "PointsForPlacesTableModel.h"
#include <QFont>
#include <QColor>
#include <cmath>
#include <QLocale>

PointsForPlacesTableModel::PointsForPlacesTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant PointsForPlacesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!

    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole || role==Qt::EditRole){
            switch(section){
            case 0:
                return tr("Miejsce");
            case 1:
                return tr("Punkty");
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

    return pointsForPlaces?pointsForPlaces->count():0;
}

int PointsForPlacesTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant PointsForPlacesTableModel::data(const QModelIndex &index, int role) const
{
    if (!pointsForPlaces || !index.isValid() || index.row()<0 || index.row()>=rowCount() || index.column()<0 || index.column()>=2)
        return QVariant();


    if(role == Qt::DisplayRole || role==Qt::EditRole){
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
    if(role!=Qt::EditRole || !pointsForPlaces || !index.isValid() || index.row()<0 || index.row()>=rowCount() || index.column()!=1)return false;
    bool valid=false;double points=value.toDouble(&valid);if(!valid && value.type()==QVariant::String){points=QLocale().toDouble(value.toString(),&valid);if(!valid){auto text=value.toString();text.replace(',','.');points=text.toDouble(&valid);}}if(!valid||!std::isfinite(points))return false;
    pointsForPlaces->insert(index.row()+1,points);emit dataChanged(index,index,{Qt::DisplayRole,Qt::EditRole});return true;
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
    if(parent.isValid()||!pointsForPlaces||row<0||row>rowCount()||count<1)return false;
    beginInsertRows(parent,row,row+count-1);QMap<int,double> next;
    for(int i=0;i<rowCount();++i)next.insert(i+(i>=row?count:0)+1,pointsForPlaces->value(i+1));
    for(int i=0;i<count;++i)next.insert(row+i+1,0);*pointsForPlaces=next;endInsertRows();return true;
}

bool PointsForPlacesTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(column);Q_UNUSED(count);Q_UNUSED(parent);return false;
}

bool PointsForPlacesTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(parent.isValid()||!pointsForPlaces||row<0||count<1||row>rowCount()-count)return false;
    beginRemoveRows(parent,row,row+count-1);QMap<int,double> next;
    for(int i=0;i<rowCount();++i)if(i<row||i>=row+count)next.insert(next.size()+1,pointsForPlaces->value(i+1));
    *pointsForPlaces=next;endRemoveRows();return true;
}

bool PointsForPlacesTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(column);Q_UNUSED(count);Q_UNUSED(parent);return false;
}

QMap<int, double> *PointsForPlacesTableModel::getPointsForPlaces() const
{
    return pointsForPlaces;
}

void PointsForPlacesTableModel::setPointsForPlaces(QMap<int, double> *newPointsForPlaces)
{
    beginResetModel();pointsForPlaces = newPointsForPlaces;endResetModel();
}

void PointsForPlacesTableModel::replacePoints(const QMap<int,double> &points) {if(!pointsForPlaces)return;const auto copy=points;beginResetModel();*pointsForPlaces=copy;endResetModel();}
