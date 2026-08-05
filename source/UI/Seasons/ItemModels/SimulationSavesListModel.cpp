#include "SimulationSavesListModel.h"
#include <QFont>
#include <QColor>

SimulationSavesListModel::SimulationSavesListModel(QVector<SimulationSave *> * globalSimulationSavesVectorPointer, QObject *parent)
    : QAbstractListModel(parent),
      globalSimulationSavesVectorPointer(globalSimulationSavesVectorPointer)
{
}

int SimulationSavesListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return globalSimulationSavesVectorPointer->size();

    // FIXME: Implement me!
}

QVariant SimulationSavesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
        return globalSimulationSavesVectorPointer->at(index.row())->getName();
    else if(role == Qt::DecorationRole)
        return QString::number(index.row());
    else if(role == Qt::FontRole){
        return QFont("Quicksand", 13, 800);
    }
    else if(role == Qt::BackgroundRole){
        return QColor(qRgb(255, 255, 255));
    }
    else if(role == Qt::ForegroundRole){
        return QColor(qRgb(21, 21, 21));
    }

    // FIXME: Implement me!
    return QVariant();
}

bool SimulationSavesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

Qt::ItemFlags SimulationSavesListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index); // FIXME: Implement me!
}

bool SimulationSavesListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool SimulationSavesListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<SimulationSave *> *SimulationSavesListModel::getGlobalSimulationSavesVectorPointer() const
{
    return globalSimulationSavesVectorPointer;
}

void SimulationSavesListModel::setGlobalSimulationSavesVectorPointer(QVector<SimulationSave *> *newGlobalSimulationSavesVectorPointer)
{
    globalSimulationSavesVectorPointer = newGlobalSimulationSavesVectorPointer;
}
