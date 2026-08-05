#ifndef TEAMSSQUADSTREEVIEW_H
#define TEAMSSQUADSTREEVIEW_H

#include <QWidget>
#include <QAction>
#include <QTreeView>
#include "TeamsSquadsTreeModel.h"

namespace Ui {
class TeamsSquadsTreeView;
}

class TeamsSquadsTreeView : public QWidget
{
    Q_OBJECT

public:
    explicit TeamsSquadsTreeView(TeamsSquadsTreeModel * model, QWidget *parent = nullptr);
    ~TeamsSquadsTreeView();

signals:
    void treeViewDoubleClicked(const QModelIndex & index);
    void needToUpdateModel();

private:
    Ui::TeamsSquadsTreeView *ui;
    TeamsSquadsTreeModel * model;

    QAction * removeAction;
    QAction * upAction;
    QAction * downAction;

private slots:
    void onRemoveActionTriggered();
    void onUpActionTriggered();
    void onDownActionTriggered();

public:
    void setModel(TeamsSquadsTreeModel *newModel);
    QTreeView * getTreeView();
};

#endif // TEAMSSQUADSTREEVIEW_H
