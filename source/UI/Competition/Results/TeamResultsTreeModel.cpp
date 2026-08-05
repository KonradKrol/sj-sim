#include "TeamResultsTreeModel.h"
#include <QFont>
#include <QPixmap>
#include "../../../global/CountryFlagsManager.h"
#include "../../../global/Uuid.h"
#include "../../../global/GlobalDatabase.h"
#include "../../../utilities/functions.h"

extern Uuid globalIDGenerator;

TeamResultsTreeModel::TeamResultsTreeModel(TeamCompetitionManager * manager, QObject *parent)
    : QAbstractItemModel(parent), manager(manager), lastTeam(nullptr)
{
    rootItem = new TreeItem({tr("Miejsce"), tr("Drużyna"), tr("Zawodnik"), tr("Punkty")});
        if(manager != nullptr)
    {
        results = manager->getResults();
        teams = &manager->getRoundsTeamsReference()[0];
        teamsAdvanceStatuses = &manager->getTeamsAdvanceStatusesReference();
    }
}

QVariant TeamResultsTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            return rootItem->data(section);
        }
    }
    else if(role == Qt::FontRole){
        QFont font("Quicksand Medium", 11);
        return font;
    }
    else if(role == Qt::ForegroundRole){
        return QColor(qRgb(0, 0, 0));
    }
    return QVariant();
}

QModelIndex TeamResultsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->getChildItem(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TeamResultsTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->getParentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TeamResultsTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());


    if(parentItem == rootItem)
        return parentItem->childCount(); // tyle naglowkow druzyn
    else if(parentItem != rootItem && parentItem->getParentItem() == rootItem)
        return results->howManyJumpersJumpedInTeam(results->getResultsReference()[parentItem->row()].getTeam()) - 0; //ilość zawodników którzy NA RAZIE skoczyli w danej drużynie.
    return 0;
}

int TeamResultsTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    }
    return rootItem->columnCount();
}

QVariant TeamResultsTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(role == Qt::DisplayRole)
    {
        return item->data(index.column());
    }
    else if(role == Qt::DecorationRole)
    {
        if(index.column() == 1 && item->getParentItem() == rootItem){
            int teamIndex = 0;
            if(item->getParentItem() == rootItem)
                teamIndex = item->row();
            else teamIndex = item->getParentItem()->row();
            QPixmap flagPixmap = CountryFlagsManager::getFlagPixmap(results->getResultsReference()[index.row()].getTeam()->getCountryCode().toLower()).scaled(QSize(24, 14));
            if(item->getParentItem() == rootItem)
            {
                flagPixmap = flagPixmap.scaled(QSize(35, 21));
            }
            return flagPixmap;
        }
    }
    else if(role == Qt::FontRole){
        QFont font("Quicksand Medium");
        font.setPointSize(8);
        if(item->getParentItem() == rootItem){ //jeśli jest to nagłówek drużyny
            font.setPointSize(11);
            if(index.column() == 3)
                font.setBold(true);
        }
        return font;
    }
    else if(role == Qt::ForegroundRole){
        if(item->getParentItem() == rootItem)
            return QColor(qRgb(0, 0, 0));
        else
            return QColor(qRgb(30, 30, 30));
    }
    else if(role == Qt::BackgroundRole){
        if(lastTeam != nullptr)
        {
            if(results->getResultByIndex(index.row())->getTeam() == lastTeam)
                if(item->getParentItem() == rootItem)
                    return QColor(qRgb(232, 243, 255));
        }
        if(manager != nullptr){
            if(item->getParentItem() == rootItem){
                Team * team = results->getResultsReference()[index.row()].getTeam();
                if(manager->getActualRound() == manager->getCompetitionInfo()->getRulesPointer()->getRoundsReference().count() && manager->getActualGroup() == manager->getCompetitionInfo()->getRulesPointer()->getJumpersInTeamCount())
                {
                    if(results->getResultOfTeam(team)->getPosition() + StartListCompetitorStatus::remainingJumpers(manager->getStartListStatusesReference()) == 3)
                        return QColor(qRgb(255, 232, 215));
                    else if(results->getResultOfTeam(team)->getPosition() + StartListCompetitorStatus::remainingJumpers(manager->getStartListStatusesReference()) == 2)
                        return QColor(qRgb(232, 232, 232));
                    else if(results->getResultOfTeam(team)->getPosition() + StartListCompetitorStatus::remainingJumpers(manager->getStartListStatusesReference()) == 1)
                        return QColor(qRgb(255, 247, 205));
                }
                if(manager->getAdvanceStatusOfTeam(team) == StartListCompetitorStatus::SureDroppedOut){
                    return QColor(qRgb(252, 237, 237));
                }
                else if(results->getResultByIndex(index.row())->getJumpsReference().count() < manager->getActualRound() * manager->getActualGroup()) //Ilość skoków jest mniejsza niż round * group
                {
                    return QColor(qRgb(254, 254, 254));
                }
                else if(manager->getAdvanceStatusOfTeam(team) == StartListCompetitorStatus::Waiting){
                    return QColor(qRgb(248, 248, 244));
                }
                else if(manager->getAdvanceStatusOfTeam(team) == StartListCompetitorStatus::SureAdvanced){
                    return QColor(qRgb(242, 255, 246));
                }
            }
        }
        else
        {
            if(item->getParentItem() == rootItem){
                if(index.row() == 2)
                    return QColor(qRgb(255, 232, 215));
                else if(index.row() == 1)
                    return QColor(qRgb(232, 232, 232));
                else if(index.row() == 0)
                    return QColor(qRgb(255, 247, 205));
                return QColor(qRgb(253, 253, 249));
            }
            else
                return QColor(qRgb(253, 253, 249));
        }
    }

    return QVariant();
}

Team *TeamResultsTreeModel::getLastTeam() const
{
    return lastTeam;
}

void TeamResultsTreeModel::setLastTeam(Team *newLastTeam)
{
    lastTeam = newLastTeam;
}

CompetitionResults *TeamResultsTreeModel::getResults() const
{
    return results;
}

void TeamResultsTreeModel::setResults(CompetitionResults *newResults)
{
    results = newResults;
}

QVector<Team *> *TeamResultsTreeModel::getTeams() const
{
    return teams;
}

void TeamResultsTreeModel::setTeams(QVector<Team *> *newTeams)
{
    teams = newTeams;
}

TeamCompetitionManager *TeamResultsTreeModel::getManager() const
{
    return manager;
}

void TeamResultsTreeModel::setManager(TeamCompetitionManager *newManager)
{
    manager = newManager;
}

TreeItem *TeamResultsTreeModel::getRootItem() const
{
    return rootItem;
}

void TeamResultsTreeModel::setupTreeItems()
{
    TreeItem::deleteAllTreeItemsRecursively(rootItem);
    rootItem = new TreeItem({tr("Miejsce"), tr("Drużyna"), tr("Zawodnik"), tr("Punkty")});
               results->sortInDescendingOrder();
    for(auto & res : results->getResultsReference()){
        TreeItem * teamHeaderItem = new TreeItem({QString::number(res.getPosition()), GlobalDatabase::get()->getCountryByAlpha3(res.getTeam()->getCountryCode())->getName(), "", QString::number(res.getPointsSum())}, rootItem);
        for(auto & jumper : res.getTeam()->getJumpersReference()){
            //Czy w resultsach jest już ten zawodnik?
            bool resultsContainsJumper = false;
            for(auto & jump : res.getJumps())
                if(jump.getJumper() == jumper){
                    resultsContainsJumper = true;
                    break;
                }
            if(resultsContainsJumper == true){
                double jumperPointsSum = 0;
                for(auto & jump : res.getJumps()){
                    if(jump.getJumper() == jumper)
                        jumperPointsSum += jump.getPoints();
                }
                TreeItem * jumperItem = new TreeItem({"", "", jumper->getNameAndSurname(), QString::number(jumperPointsSum, 'f', 1)}, teamHeaderItem);
            }
        }
    }
}

QVector<QPair<Team *, int> > *TeamResultsTreeModel::getTeamsAdvanceStatuses() const
{
    return teamsAdvanceStatuses;
}

void TeamResultsTreeModel::setTeamsAdvanceStatuses(QVector<QPair<Team *, int> > *newTeamsAdvanceStatuses)
{
    teamsAdvanceStatuses = newTeamsAdvanceStatuses;
}
