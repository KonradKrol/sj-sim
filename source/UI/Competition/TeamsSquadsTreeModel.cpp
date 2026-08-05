#include "TeamsSquadsTreeModel.h"
#include <QFont>
#include <QPixmap>
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalDatabase.h"

TeamsSquadsTreeModel::TeamsSquadsTreeModel(QVector<Team> *teams, int jumpersInTeam, QObject *parent)
    : QAbstractItemModel(parent), teams(teams), jumpersInTeam(jumpersInTeam)
{
    rootItem = new TreeItem({tr("Drużyna"), tr("Zawodnik")});
}

TeamsSquadsTreeModel::~TeamsSquadsTreeModel()
{
    TreeItem::deleteAllTreeItemsRecursively(rootItem);
}

QVariant TeamsSquadsTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QModelIndex TeamsSquadsTreeModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex TeamsSquadsTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->getParentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TeamsSquadsTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    if(parentItem != rootItem)
        return jumpersInTeam; //zawodnicy
    return parentItem->childCount(); //naglowki druzyn (parent == rootItem)
}

int TeamsSquadsTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TeamsSquadsTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data(index.column());
    }
    else if(role == Qt::DecorationRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if(((index.column() == 0 && item->getParentItem() == rootItem) || (index.column() == 1 && item->getParentItem() != rootItem))){
            int teamIndex = 0;
            if(item->getParentItem() == rootItem)
                teamIndex = item->row();
            else
                teamIndex = item->getParentItem()->row();

            QPixmap flagPixmap = CountryFlagsManager::getFlagPixmap(teams->at(teamIndex).getCountryCode().toLower()).scaled(QSize(24, 14));
            if(item->getParentItem() == rootItem)
            {
                flagPixmap = flagPixmap.scaled(QSize(35, 21));
            }
            return flagPixmap;
        }
    }
    else if(role == Qt::FontRole){
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        QFont font("Quicksand Medium");
        font.setPointSizeF(8);
        if(item->getParentItem() == rootItem){ //jeśli jest to nagłówek drużyny
            font.setPointSize(9);
            font.setBold(true);
        }
        return font;
    }
    else if(role == Qt::ForegroundRole){
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if(item->getParentItem() == rootItem)
            return QColor(qRgb(0, 0, 0));
        else
            return QColor(qRgb(30, 30, 30));
    }

    return QVariant();
}

TreeItem *TeamsSquadsTreeModel::getRootItem() const
{
    return rootItem;
}

void TeamsSquadsTreeModel::setRootItem(TreeItem *newRootItem)
{
    rootItem = newRootItem;
}

QVector<Team> *TeamsSquadsTreeModel::getTeams() const
{
    return teams;
}

void TeamsSquadsTreeModel::setTeams(QVector<Team> *newTeams)
{
    teams = newTeams;
}

void TeamsSquadsTreeModel::setupTreeItems()
{
    TreeItem::deleteAllTreeItemsRecursively(rootItem);
    rootItem = new TreeItem({"Drużyna", "Zawodnik"});
    for(auto & team : *teams){
        TreeItem * teamItem = new TreeItem({GlobalDatabase::get()->getCountryByAlpha3(team.getCountryCode())->getName(), ""}, rootItem);
        int i=0;
        for(auto & jumper : team.getJumpersReference()){
            if(i == jumpersInTeam) break;
            TreeItem * jumperItem = new TreeItem({"", jumper->getNameAndSurname()}, teamItem);
            i++;
        }
    }
}

int TeamsSquadsTreeModel::getJumpersInTeam() const
{
    return jumpersInTeam;
}

void TeamsSquadsTreeModel::setJumpersInTeam(int newJumpersInTeam)
{
    jumpersInTeam = newJumpersInTeam;
}
