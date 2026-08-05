#include "JumpWindInfoWidget.h"
#include "ui_JumpWindInfoWidget.h"

#include "JumpWindInfoWidgetItem.h"

JumpWindInfoWidget::JumpWindInfoWidget(QVector<Wind> * winds, double KPoint, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JumpWindInfoWidget),
    winds(winds),
    KPoint(KPoint)
{
    ui->setupUi(this);
}

JumpWindInfoWidget::~JumpWindInfoWidget()
{
    delete ui;
}

void JumpWindInfoWidget::fillItemsLayout()
{
    int i=0;
    for(auto & wind : *winds){
        JumpWindInfoWidgetItem * item = new JumpWindInfoWidgetItem(&wind, i, KPoint, this);
        item->fillWidget();
        ui->verticalLayout_items->addWidget(item);
        i++;
    }
}

QVector<Wind> *JumpWindInfoWidget::getWinds() const
{
    return winds;
}

void JumpWindInfoWidget::setWinds(QVector<Wind> *newWinds)
{
    winds = newWinds;
}

double JumpWindInfoWidget::getKPoint() const
{
    return KPoint;
}

void JumpWindInfoWidget::setKPoint(double newKPoint)
{
    KPoint = newKPoint;
}
