#include "GeneralClassificationTableModel.h"
#include "../../../../global/CountryFlagsManager.h"
#include <QFont>
#include <QIcon>

GeneralClassificationTableModel::GeneralClassificationTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant GeneralClassificationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Miejsce");
        case 1:
        {
            if(type == Ind)
                return tr("Zawodnik");
            else
                return tr("Drużyna");
        }
        case 2:
            return tr("Punkty");
        }
    }
    return QVariant();
}

int GeneralClassificationTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if(type == Ind)
        return indResults.count();
    else
        return teamResults.count();
}

int GeneralClassificationTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant GeneralClassificationTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0:
            return index.row() + 1;
        case 1:
        {
            if(type == Ind)
                return indResults[index.row()].first->getNameAndSurname();
            else
                return teamResults[index.row()].first;
        }
        case 2:
            if(type == Ind)
                return indResults[index.row()].second;
            else
                return teamResults[index.row()].second;
        }
    }
    else if(role == Qt::DecorationRole)
    {
        if(index.column() == 1)
        {
            if(type == Ind)
                return QIcon(CountryFlagsManager::getFlagPixmap(indResults[index.row()].first->getCountryCode().toLower()));
            else
                return QIcon(CountryFlagsManager::getFlagPixmap(teamResults[index.row()].first.toLower()));
        }
    }
    else if(role == Qt::FontRole)
    {
        QFont font("Quicksand Medium", 11);
        if(index.column() == 0)
            font.setBold(true);
        else if(index.column() == 1)
            font.setBold(false);
        else if(index.column() == 2)
            font.setBold(true);
    }
    else if(role == Qt::BackgroundRole)
    {
        if(index.row() == 0)
            return QColor(qRgb(250, 239, 205));
        else if(index.row() == 1)
            return QColor(qRgb(222, 222, 222));
        else if(index.row() == 2)
            return QColor(qRgb(255, 227, 199));
        else
            return QColor(qRgb(253, 253, 253));
    }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignHCenter;

    // FIXME: Implement me!
    return QVariant();
}

QVector<QPair<QString, double> > GeneralClassificationTableModel::getTeamResults() const
{
    return teamResults;
}

QVector<QPair<QString, double> > &GeneralClassificationTableModel::getTeamResultsReference()
{
    return teamResults;
}

void GeneralClassificationTableModel::setTeamResults(const QVector<QPair<QString, double> > &newTeamResults)
{
    teamResults = newTeamResults;
}

int GeneralClassificationTableModel::getType() const
{
    return type;
}

void GeneralClassificationTableModel::setType(int newType)
{
    type = newType;
}

QVector<QPair<Jumper *, double> > GeneralClassificationTableModel::getIndResults() const
{
    return indResults;
}

QVector<QPair<Jumper *, double> > &GeneralClassificationTableModel::getIndResultsReference()
{
    return indResults;
}

void GeneralClassificationTableModel::setIndResults(const QVector<QPair<Jumper *, double> > &newResults)
{
    indResults = newResults;
}
