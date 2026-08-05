#include "ClassificationsListModel.h"

#include <QIcon>
#include <QFont>
#include <QColor>

ClassificationsListModel::ClassificationsListModel(QVector<Classification *> *classificationsVectorPointer, QObject *parent) : QAbstractListModel(parent),
    classificationsVectorPointer(classificationsVectorPointer)
{
}

QVariant ClassificationsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int ClassificationsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return classificationsVectorPointer->count();
}


QVariant ClassificationsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        return classificationsVectorPointer->at(index.row())->getName();
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

bool ClassificationsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool ClassificationsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<Classification *> *ClassificationsListModel::getClassificationsVectorPointer() const
{
    return classificationsVectorPointer;
}

void ClassificationsListModel::setClassificationsVectorPointer(QVector<Classification *> *newClassificationsVectorPointer)
{
    classificationsVectorPointer = newClassificationsVectorPointer;
}
