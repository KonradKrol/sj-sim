#include "CompetitionsArchiveListModel.h"
#include "../../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>

CompetitionsArchiveListModel::CompetitionsArchiveListModel(QVector<CompetitionInfo *> *seasonCompetitions, QObject *parent)
    : QAbstractListModel(parent), seasonCompetitions(seasonCompetitions)
{
}

QVariant CompetitionsArchiveListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

int CompetitionsArchiveListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    int count = 0;
    if(seasonCompetitions != nullptr){
    for(auto & competition : *seasonCompetitions)
    {
        if(competition->getPlayed() == true)
            count++;
        else
            break;
    }
    }
    return count;
}

QVariant CompetitionsArchiveListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        CompetitionInfo * competition = seasonCompetitions->at(index.row());
        if(competition->getPlayed() == true){
            QString string = QString::number(index.row() + 1) + ". " + competition->getHill()->getName() + " HS" + QString::number(competition->getHill()->getHSPoint()) + " (";
            switch(competition->getSerieType())
            {
            case CompetitionInfo::Competition:
                string += tr("Konkurs");
                break;
            case CompetitionInfo::Qualifications:
                string += tr("Kwalifikacje");
                break;
            case CompetitionInfo::TrialRound:
                string += tr("Seria próbna");
                    break;
            case CompetitionInfo::Training:
                string += tr("Trening");
                break;
            }
            string += ")";
            /*if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
                string += tr(" --> (Konkurs drużynowy)");*/
            return string;
        }
    }
    else if(role == Qt::DecorationRole)
    {
        CompetitionInfo * competition = seasonCompetitions->at(index.row());
        return QIcon(CountryFlagsManager::getFlagPixmap(competition->getHill()->getCountryCode().toLower()));
    }
    else if(role == Qt::FontRole)
    {
        CompetitionInfo * competition = seasonCompetitions->at(index.row());
        QFont font("Ubuntu", 9);
        if(competition->getSerieType() == CompetitionInfo::Competition || competition->getSerieType() == CompetitionInfo::Qualifications)
            font.setBold(true);
        if(competition->getSerieType() == CompetitionInfo::Qualifications)
            font.setItalic(true);
        return font;
    }
    else if(role == Qt::ForegroundRole)
        return QColor("black");
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;
    else if(role == Qt::BackgroundRole)
    {
        CompetitionInfo * competition = seasonCompetitions->at(index.row());
        if(competition->getCancelled() == true)
            return QColor(qRgb(255, 238, 238));
        if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
            return QColor(qRgb(230, 238, 255));
        else// if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            return QColor(qRgb(249, 255, 252));
        /*switch(competition->getSerieType())
        {
        case CompetitionInfo::Competition:
            return QColor(qRgb(197, 224, 252));
        case CompetitionInfo::Qualifications:
            return QColor(qRgb(197, 224, 252));
        case CompetitionInfo::TrialRound:
            return QColor(qRgb(197, 224, 252));
        case CompetitionInfo::Training:
            return QColor(qRgb(197, 224, 252));
        }*/
    }

    return QVariant();
}

bool CompetitionsArchiveListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool CompetitionsArchiveListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

QVector<CompetitionInfo *> *CompetitionsArchiveListModel::getSeasonCompetitions() const
{
    return seasonCompetitions;
}

void CompetitionsArchiveListModel::setSeasonCompetitions(QVector<CompetitionInfo *> *newSeasonCompetitions)
{
    seasonCompetitions = newSeasonCompetitions;
}
