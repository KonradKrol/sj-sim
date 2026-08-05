#include "JumpersListsListItemWidget.h"
#include "ui_JumpersListsListItemWidget.h"

JumpersListsListItemWidget::JumpersListsListItemWidget(SaveJumpersList *list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JumpersListsListItemWidget),
    list(list)
{
    ui->setupUi(this);
    ui->label_listName->setText(list->getName());
    connect(this, &JumpersListsListItemWidget::selected, this, [this](){
        ui->pushButton_select->setStyleSheet("QPushButton\n{\nbackground-color: rgb(200, 241, 193);\ncolor: black;\nborder: 1px solid;\nborder-radius: 4px;\n}\nQPushButton:hover\n{\n	background-color: rgb(200, 241, 193);\n}");
        ui->pushButton_unselect->setStyleSheet("QPushButton\n{\nbackground-color: rgb(255, 255, 255);\ncolor: black;\nborder: 1px solid;\nborder-radius: 4px;\n}\nQPushButton:hover\n{\n	background-color: rgb(200, 241, 193);\n}");
        status = Select;
    });
    connect(this, &JumpersListsListItemWidget::unselected, this, [this](){
        ui->pushButton_select->setStyleSheet("QPushButton\n{\nbackground-color: rgb(255, 255, 255);\ncolor: black;\nborder: 1px solid;\nborder-radius: 4px;\n}\nQPushButton:hover\n{\n	background-color: rgb(255, 175, 175);\n}");
        ui->pushButton_unselect->setStyleSheet("QPushButton\n{\nbackground-color: rgb(255, 175, 175);\ncolor: black;\nborder: 1px solid;\nborder-radius: 4px;\n}\nQPushButton:hover\n{\n	background-color: rgb(255, 175, 175);\n}");
        status = Unselect;
    });

    status = Neutral;
}

JumpersListsListItemWidget::~JumpersListsListItemWidget()
{
    delete ui;
}

void JumpersListsListItemWidget::on_pushButton_select_clicked()
{
    emit selected();
}

void JumpersListsListItemWidget::on_pushButton_unselect_clicked()
{
    emit unselected();
}

int JumpersListsListItemWidget::getStatus() const
{
    return status;
}

void JumpersListsListItemWidget::setStatus(int newStatus)
{
    status = newStatus;
}

SaveJumpersList *JumpersListsListItemWidget::getList() const
{
    return list;
}

void JumpersListsListItemWidget::setList(SaveJumpersList *newList)
{
    list = newList;
}

