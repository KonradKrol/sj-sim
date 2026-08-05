#include "TeamsSquadsTreeView.h"
#include "ui_TeamsSquadsTreeView.h"

TeamsSquadsTreeView::TeamsSquadsTreeView(TeamsSquadsTreeModel *model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeamsSquadsTreeView),
    model(model)
{
    ui->setupUi(this);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setModel(model);

    removeAction = new QAction(this);
    removeAction->setShortcut(Qt::CTRL | Qt::Key_D);
    this->addAction(removeAction);
    connect(removeAction, &QAction::triggered, this, &TeamsSquadsTreeView::onRemoveActionTriggered);

    upAction = new QAction(this);
    upAction->setShortcut(Qt::CTRL | Qt::Key_Up);
    this->addAction(upAction);
    connect(upAction, &QAction::triggered, this, &TeamsSquadsTreeView::onUpActionTriggered);

    downAction = new QAction(this);
    downAction->setShortcut(Qt::CTRL | Qt::Key_Down);
    this->addAction(downAction);
    connect(downAction, &QAction::triggered, this, &TeamsSquadsTreeView::onDownActionTriggered);

    connect(ui->treeView, &QTreeView::doubleClicked, this, &TeamsSquadsTreeView::treeViewDoubleClicked);
}

TeamsSquadsTreeView::~TeamsSquadsTreeView()
{
    delete ui;
}

void TeamsSquadsTreeView::onRemoveActionTriggered()
{
    if(ui->treeView->selectionModel()->selectedRows().count() > 0){
        QModelIndex index = ui->treeView->selectionModel()->selectedRows().at(0);
        TreeItem * item = static_cast<TreeItem*>(ui->treeView->selectionModel()->selectedRows().at(0).internalPointer());
        if(item->getParentItem() == model->getRootItem()){
            int rowToRemove = item->row();
            model->getTeams()->remove(rowToRemove, 1);
            emit needToUpdateModel();

            if(item->row() < model->rowCount() - 1){
                QItemSelectionModel * selectionModel = ui->treeView->selectionModel();
                selectionModel->clearSelection();
                selectionModel->select(model->index(index.row(), index.column(), index.parent()), QItemSelectionModel::Select);
                selectionModel->select(model->index(index.row(), index.column() + 1, index.parent()), QItemSelectionModel::Select);
                selectionModel->select(model->index(index.row(), index.column() + 2, index.parent()), QItemSelectionModel::Select);
                ui->treeView->setSelectionModel(selectionModel);
            }
        }
    }
}

void TeamsSquadsTreeView::onUpActionTriggered()
{
    if(ui->treeView->selectionModel()->selectedRows().count() > 0){
        QModelIndex index = ui->treeView->selectionModel()->selectedRows().at(0);
        TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
        if(item->getParentItem() == model->getRootItem() && item->row() > 0){
            model->getTeams()->swapItemsAt(item->row(), item->row() - 1);
            emit needToUpdateModel();

            QItemSelectionModel * selectionModel = ui->treeView->selectionModel();
            selectionModel->clearSelection();
            selectionModel->select(model->index(index.row() - 1, index.column(), index.parent()), QItemSelectionModel::Select);
            selectionModel->select(model->index(index.row() - 1, index.column() + 1, index.parent()), QItemSelectionModel::Select);
            selectionModel->select(model->index(index.row() - 1, index.column() + 2, index.parent()), QItemSelectionModel::Select);
            ui->treeView->setSelectionModel(selectionModel);
        }
    }
}

void TeamsSquadsTreeView::onDownActionTriggered()
{
    if(ui->treeView->selectionModel()->selectedRows().count() > 0){
        QModelIndex index = ui->treeView->selectionModel()->selectedRows().at(0);
        TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
        if(item->getParentItem() == model->getRootItem() && item->row() < model->rowCount() - 1){
            model->getTeams()->swapItemsAt(item->row(), item->row() + 1);
            emit needToUpdateModel();

            QItemSelectionModel * selectionModel = ui->treeView->selectionModel();
            selectionModel->clearSelection();
            selectionModel->select(model->index(index.row() + 1, index.column(), index.parent()), QItemSelectionModel::Select);
            selectionModel->select(model->index(index.row() + 1, index.column() + 1, index.parent()), QItemSelectionModel::Select);
            selectionModel->select(model->index(index.row() + 1, index.column() + 2, index.parent()), QItemSelectionModel::Select);
            ui->treeView->setSelectionModel(selectionModel);
        }
    }
}

void TeamsSquadsTreeView::setModel(TeamsSquadsTreeModel *newModel)
{
    model = newModel;
    ui->treeView->setModel(model);
    ui->treeView->expandToDepth(0);
}

QTreeView *TeamsSquadsTreeView::getTreeView()
{
    return ui->treeView;
}
