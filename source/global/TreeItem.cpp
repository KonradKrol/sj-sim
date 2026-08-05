#include "TreeItem.h"
#include "../utilities/functions.h"
#include "Uuid.h"
#include <QModelIndex>

extern Uuid globalIDGenerator;

TreeItem::TreeItem(const QVector<QVariant> &dataVector, TreeItem *parentItem) :
    parentItem(parentItem), dataVector(dataVector),
    Identifiable()
{
    if(this->parentItem)
        this->parentItem->addChild(this);
}

void TreeItem::addChild(TreeItem *child)
{
    childItems.append(child);
}

QVariant TreeItem::data(int index) const
{
    if(index < dataVector.size())
        return dataVector[index];
    return QVariant();
}

TreeItem *TreeItem::getParentItem()
{
    return parentItem;
}

TreeItem *TreeItem::getChildItem(int index)
{
    return childItems[index];
}

int TreeItem::row() const
{
    if(parentItem){
        return parentItem->childItems.indexOf(const_cast<TreeItem *>(this));
    }
    return 0;
}

int TreeItem::columnCount() const
{
    return dataVector.count();
}

int TreeItem::childCount() const
{
    return childItems.count();
}

QVector<TreeItem *> TreeItem::getChildItemsReference()
{
    return childItems;
}

QVector<QVariant> &TreeItem::getDataVectorReference()
{
    return dataVector;
}

void TreeItem::deleteAllTreeItemsRecursively(TreeItem *rootItem)
{
    for(auto & item : rootItem->getChildItemsReference())
        deleteTreeItemRecursively(item);
    delete rootItem;
}

void TreeItem::deleteTreeItemRecursively(TreeItem *item)
{
    if(item->childCount() > 0)
    {
        for(auto & itm : item->getChildItemsReference())
        {
            deleteTreeItemRecursively(itm);
        }
    }
    item->getParentItem()->getChildItemsReference().remove(item->row());
    delete item;
}

bool TreeItem::recursivelyContains(TreeItem *root, TreeItem * searched)
{
    if(root == nullptr || searched == nullptr) return false;
    bool contains = false;
    for(auto & itm : root->getChildItemsReference())
    {
        if(recursivelyContains(itm, searched) == true){
            return true;
        }
    }
    if(MyFunctions::vectorContains(root->getChildItemsReference(), searched))
        return true;
    else
        return false;
}

int TreeItem::getIndexOfItemInVectorByTreeModelIndex(const QModelIndex &index, TreeItem *rootItem, bool includeParentRows)
{
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    int howMany = 0;
    void * ptr = nullptr;
    int parentRow = 0;
    int row = 0;
    while(ptr != index.internalPointer()){
        if(item->getParentItem() == rootItem)
            ptr = rootItem->getChildItem(parentRow);
        else
            ptr = rootItem->getChildItem(parentRow)->getChildItem(row);

        if(includeParentRows == true)
            howMany++;
        else if(row > 0)
            howMany++;

        row++;
        if(row == rootItem->getChildItem(parentRow)->childCount()){
            parentRow++;
            row = 0;
        }
    }
    return howMany - 1;
}
