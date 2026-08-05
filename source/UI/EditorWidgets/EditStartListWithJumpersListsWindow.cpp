#include "EditStartListWithJumpersListsWindow.h"
#include "ui_EditStartListWithJumpersListsWindow.h"
#include "../../utilities/functions.h"

EditStartListWithJumpersListsWindow::EditStartListWithJumpersListsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditStartListWithJumpersListsWindow)
{
    ui->setupUi(this);
}

EditStartListWithJumpersListsWindow::~EditStartListWithJumpersListsWindow()
{
    delete ui;
}

QVector<QPair<SaveJumpersList *, int>> EditStartListWithJumpersListsWindow::constructJumpersListsStatuses()
{
    QVector<QPair<SaveJumpersList *, int>> statuses;
    for(auto & w : widgets)
    {
        QPair<SaveJumpersList *, int> status;
        status.first = w->getList();
        status.second = w->getStatus();
        statuses.push_back(status);
    }
    return statuses;
}

void EditStartListWithJumpersListsWindow::setupWidgets()
{
    for(auto & w : widgets)
    {
        ui->verticalLayout_items->removeWidget(w);
        MyFunctions::removeFromVector(widgets, w);
        delete w;
    }
    for(auto & list : *lists)
    {
        JumpersListsListItemWidget * w = new JumpersListsListItemWidget(&list, w);
        widgets.push_back(w);
        ui->verticalLayout_items->addWidget(w);
    }
}

QVector<JumpersListsListItemWidget *> EditStartListWithJumpersListsWindow::getWidgets() const
{
    return widgets;
}

void EditStartListWithJumpersListsWindow::setWidgets(const QVector<JumpersListsListItemWidget *> &newWidgets)
{
    widgets = newWidgets;
}

QVector<SaveJumpersList> *EditStartListWithJumpersListsWindow::getLists() const
{
    return lists;
}

void EditStartListWithJumpersListsWindow::setLists(QVector<SaveJumpersList> *newLists)
{
    lists = newLists;
}

void EditStartListWithJumpersListsWindow::on_pushButton_submit_clicked()
{
    accept();
}
