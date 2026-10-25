#include "KOGroupResultsTableModel.h"
#include "../../../competitions/CompetitionAdvancement.h"
#include <QIcon>
#include "../../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../../global/CountryFlagsManager.h"

KOGroupResultsTableModel::KOGroupResultsTableModel(KORoundManager * KOManager, KOGroup * group, QObject *parent)
    : QAbstractTableModel(parent),
    group(group),
    KOManager(KOManager)
{
}

QVariant KOGroupResultsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QStringList headers = {tr("Poz."), tr("Zawodnik"), tr("Punkty")};

    // Sprawdzamy poprawność parametrów
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section < 0 || section >= headers.size())
        return QVariant();

    // Zwracamy wartość z listy
    return headers.at(section);
}

int KOGroupResultsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || group == nullptr || KOManager == nullptr || KOManager->getResults() == nullptr)
        return 0;

    QVector<Jumper *> sortedGroupJumpers = group->getJumpersReference();
    KOManager->getResults()->sortJumpersByResults(sortedGroupJumpers);
    return sortedGroupJumpers.count();
}

int KOGroupResultsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant KOGroupResultsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || group == nullptr || KOManager == nullptr || KOManager->getResults() == nullptr
        || index.column() < 0 || index.column() >= columnCount())
        return QVariant();

    QVector<Jumper *> sortedGroupJumpers = group->getJumpersReference();
    KOManager->getResults()->sortJumpersByResults(sortedGroupJumpers);
    if(index.row() < 0 || index.row() >= sortedGroupJumpers.count())
        return QVariant();
    Jumper * jumper = sortedGroupJumpers.at(index.row());
    if(jumper == nullptr)
        return QVariant();

    if(role==CompetitionAdvancement::LatestJumpRole){auto *result=KOManager->getResults()->getResultOfIndividualJumper(jumper);return result && KOManager->getResults()->containsLatestJump(*result);}
    auto *manager=KOManager->getIndManager();
    const int groupState=KOManager->getStatuses().value(jumper,KORoundManager::Waiting);
    const int advancement=groupState==KORoundManager::Winner?StartListCompetitorStatus::SureAdvanced:groupState==KORoundManager::Loser?StartListCompetitorStatus::SureDroppedOut:StartListCompetitorStatus::Waiting;
    if(role==CompetitionAdvancement::LuckyLoserRole){auto *individual=dynamic_cast<IndividualCompetitionManager *>(manager);return individual && individual->isLuckyLoser(jumper);}
    if(role==CompetitionAdvancement::RoundStatusRole)return advancement;
    if(role==CompetitionAdvancement::QualificationStatusRole)return -1;
    if(role==CompetitionAdvancement::QualificationTargetRole)return QString();
    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0){
            QVector<int> positions = KOManager->getResults()->getJumpersPositions(&sortedGroupJumpers);
            return positions.value(sortedGroupJumpers.indexOf(jumper), 0);
        }
        else if(index.column() == 1)
            return jumper->getNameAndSurname();
        else if(index.column() == 2){
            if(KOManager->getResults()->getResultOfIndividualJumper(jumper) == nullptr)
                return 0;
            auto *result=KOManager->getResults()->getResultOfIndividualJumper(jumper);
            const int round=manager?manager->getActualRound():result->getJumpsReference().size();
            QString text=QString::number(result->getPointsSum(),'f',1);
            if(round>0 && round<=result->getJumpsReference().size()){const auto &jump=result->getJumpsReference()[round-1];if(jump.getDNS())text+=" · DNS";else if(jump.getDSQ())text+=" · DSQ";}
            return text;
        }
    }
    else if(role == Qt::ToolTipRole)
    {
        return data(index, Qt::DisplayRole);
    }
    else if(role == Qt::DecorationRole)
    {
        if(index.column() == 1)
        {
            return QIcon(CountryFlagsManager::getFlagPixmap(jumper->getCountryCode().toLower()).scaled(QSize(24, 14)));
        }
    }
    else if(role == Qt::BackgroundRole)
    {
        if(advancement==StartListCompetitorStatus::SureDroppedOut)return QColor(238,240,242);
        if(advancement==StartListCompetitorStatus::SureAdvanced)return QColor(233,255,229);
        return QColor(255,255,247);
    }
    else if(role == Qt::ForegroundRole)
    {
        return advancement==StartListCompetitorStatus::SureDroppedOut?QColor(108,116,128):QColor(0,0,0);
    }
    else if(role == Qt::FontRole)
    {
        QFont font("Quicksand Medium", 10);
        if(index.column() == 0)
            font.setBold(true);
        else if(index.column() == 1)
            font.setBold(false);
        else if(index.column() == 2)
            font.setBold(true);
        return font;
    }
    else if(role == Qt::TextAlignmentRole)
    {
        return Qt::AlignHCenter;
    }

    // FIXME: Implement me!
    return QVariant();
}

bool KOGroupResultsTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool KOGroupResultsTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool KOGroupResultsTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool KOGroupResultsTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

KORoundManager *KOGroupResultsTableModel::getKOManager() const
{
    return KOManager;
}

void KOGroupResultsTableModel::setKOManager(KORoundManager *newKOManager)
{
    KOManager = newKOManager;
}

KOGroup *KOGroupResultsTableModel::getGroup() const
{
    return group;
}

void KOGroupResultsTableModel::setGroup(KOGroup *newGroup)
{
    group = newGroup;
}
