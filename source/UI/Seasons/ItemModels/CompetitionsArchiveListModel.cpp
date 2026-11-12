#include "CompetitionsArchiveListModel.h"
#include "../../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>

CompetitionsArchiveListModel::CompetitionsArchiveListModel(QVector<CompetitionInfo *> *seasonCompetitions, QObject *parent)
    : QAbstractListModel(parent), seasonCompetitions(seasonCompetitions)
{
    rebuildPlayed();
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

    return playedCompetitions.size();
}

QVariant CompetitionsArchiveListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row()<0 || index.row()>=rowCount())
        return QVariant();

    if(role == Qt::DisplayRole) {
        if(labelLanguage!=tr("Konkurs"))rebuildLabels();
        return displayLabels.value(index.row());
    }
    else if(role == Qt::DecorationRole)
    {
        CompetitionInfo * competition = competitionAt(index.row());
        return QIcon(CountryFlagsManager::getFlagPixmap(competition->getHill()->getCountryCode().toLower()));
    }
    else if(role == Qt::FontRole)
    {
        CompetitionInfo * competition = competitionAt(index.row());
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
        CompetitionInfo * competition = competitionAt(index.row());
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
    beginResetModel();
    seasonCompetitions = newSeasonCompetitions;
    rebuildPlayed();
    endResetModel();
}

void CompetitionsArchiveListModel::rebuildPlayed() {
    playedCompetitions.clear();
    if(seasonCompetitions)for(auto *event:*seasonCompetitions)if(event && event->getPlayed())playedCompetitions.append(event);
    rebuildLabels();
}
void CompetitionsArchiveListModel::rebuildLabels() const {
    labelLanguage=tr("Konkurs");
    displayLabels.clear();
    for(int row=0;row<playedCompetitions.size();++row) {
        auto *competition=playedCompetitions[row];
        QString string = QString::number(row + 1) + ". " + (competition->getHill()?competition->getHill()->getName():tr("Brak skoczni")) + " HS" + QString::number(competition->getHill()?competition->getHill()->getHSPoint():0) + " (";
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
        displayLabels.append(string);
    }
}
CompetitionInfo *CompetitionsArchiveListModel::competitionAt(int row) const {
    return playedCompetitions.value(row,nullptr);
}
