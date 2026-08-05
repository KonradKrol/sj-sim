#include "TeamJumpersListModel.h"
#include "../../../../global/CountryFlagsManager.h"
#include <QFont>
#include <QColor>

TeamJumpersListModel::TeamJumpersListModel(QVector<Jumper *> *jumpers, int jumpersInTeamCount, QObject *parent) : QAbstractListModel(parent), jumpers(jumpers), jumpersInTeamCount(jumpersInTeamCount)
{
}

QVariant TeamJumpersListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int TeamJumpersListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return jumpers->count();

    // FIXME: Implement me!
}

QVariant TeamJumpersListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role){
    case Qt::DisplayRole:
        return jumpers->at(index.row())->getNameAndSurname();
    case Qt::DecorationRole:
        return CountryFlagsManager::getFlagPixmap(jumpers->at(index.row())->getCountryCode().toLower()).scaled(38, 22);
    case Qt::FontRole:
    {
        QFont font("Quicksand Medium", 13);
        if(index.row()+1 <= jumpersInTeamCount)
            font.setBold(true);
        return font;
    }
    case Qt::BackgroundRole:
        return QColor(qRgb(55, 123, 11));
        if(index.row()+1 <= jumpersInTeamCount){
            qDebug()<<"hrum";
            return QColor(qRgb(243, 255, 240));
        }
        else{
            return QColor(qRgb(252, 234, 227));
        }
        return QColor(qRgb(233, 245, 236));
    case Qt::ForegroundRole:
        return QColor(0, 0, 0);
    }

    // FIXME: Implement me!
    return QVariant();
}

QVector<Jumper *> *TeamJumpersListModel::getJumpers()
{
    return jumpers;
}

void TeamJumpersListModel::setJumpers(QVector<Jumper *> *newJumpers)
{
    jumpers = newJumpers;
}

int TeamJumpersListModel::getJumpersInTeamCount() const
{
    return jumpersInTeamCount;
}

void TeamJumpersListModel::setJumpersInTeamCount(int newJumpersInTeamCount)
{
    jumpersInTeamCount = newJumpersInTeamCount;
}
