#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>
#include "../utilities/Identifiable.h"

class TreeItem : public Identifiable
{
public:
    TreeItem(const QVector<QVariant> & dataVector, TreeItem * parentItem = nullptr);
private:
    TreeItem * parentItem;
    QVector<TreeItem*> childItems;
    QVector<QVariant> dataVector;
public:
    void addChild(TreeItem * child);
    QVariant data(int index) const;
    TreeItem * getParentItem();
    TreeItem * getChildItem(int index);
    int row() const;
    int columnCount() const;
    int childCount() const;
    QVector<TreeItem *> getChildItemsReference();
    QVector<QVariant> & getDataVectorReference();

    virtual void abc() {}

    static void deleteAllTreeItemsRecursively(TreeItem * rootItem);
    static void deleteTreeItemRecursively(TreeItem * item);
    static int getIndexOfItemInVectorByTreeModelIndex(const QModelIndex & index, TreeItem * rootItem, bool includeParentRows = false);
    static bool recursivelyContains(TreeItem * root, TreeItem *searched);
};

#endif // TREEITEM_H
