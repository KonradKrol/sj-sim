#include "SimulationRatingTableModel.h"
#include <QFont>
#include <QColor>
#include <QIcon>
#include "../../../../global/CountryFlagsManager.h"

SimulationRatingTableModel::SimulationRatingTableModel(QObject *parent)
    : QAbstractTableModel(parent), greenRedColors(false)
{
}

QVariant SimulationRatingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if(section == 0)
            return tr("Miejsce");
        else if(section == 1)
            return tr("Zawodnik");
        else if(section == 2)
            return tr("Wartość");
    }
    return QVariant();
}

int SimulationRatingTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return ranking.count();
}

int SimulationRatingTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant SimulationRatingTableModel::data(const QModelIndex &index, int role) const
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
            return ranking[index.row()].first->getNameAndSurname();
        case 2:
            return ranking[index.row()].second;
        }
    }
    else if(role == Qt::FontRole)
    {
        QFont font("Quicksand", 10);
        if(index.column() == 0 && index.column() == 2)
            font.setBold(true);
        return font;
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
    else if(role == Qt::DecorationRole)
    {
        if(index.column() == 1)
        {
            return QIcon(CountryFlagsManager::getFlagPixmap(ranking[index.row()].first->getCountryCode()));
        }
    }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignHCenter;
    else if(role == Qt::ForegroundRole)
    {
        if(greenRedColors)
        {
            if(ranking[index.row()].second > 0)
                return QColor(qRgb(68, 130, 26));
            else if(ranking[index.row()].second < 0)
                return QColor(qRgb(145, 35, 286));
            else
                return QColor(qRgb(50, 50, 50));
        }
        return QColor(qRgb(15, 15, 15));
    }

    return QVariant();
}

bool SimulationRatingTableModel::getGreenRedColors() const
{
    return greenRedColors;
}

void SimulationRatingTableModel::setGreenRedColors(bool newGreenRedColors)
{
    greenRedColors = newGreenRedColors;
}

QVector<QPair<Jumper *, double> > SimulationRatingTableModel::getRanking() const
{
    return ranking;
}

QVector<QPair<Jumper *, double> > &SimulationRatingTableModel::getRankingReference()
{
    return ranking;
}

void SimulationRatingTableModel::setRanking(const QVector<QPair<Jumper *, double> > &newRanking)
{
    ranking = newRanking;
}
