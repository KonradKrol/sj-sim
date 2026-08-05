#include "ClassificationsCheckBoxesWidget.h"
#include "ui_ClassificationsCheckBoxesWidget.h"
#include <QCheckBox>
#include"../../../../utilities/functions.h"

ClassificationsCheckBoxesWidget::ClassificationsCheckBoxesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassificationsCheckBoxesWidget)
{
    ui->setupUi(this);
}

ClassificationsCheckBoxesWidget::~ClassificationsCheckBoxesWidget()
{
    delete ui;
}

void ClassificationsCheckBoxesWidget::setupCheckBoxes()
{
    for(auto & checkBox : checkBoxes)
    {
        disconnect(checkBox, &QCheckBox::stateChanged, this, &ClassificationsCheckBoxesWidget::changed);
        ui->gridLayout->removeWidget(checkBox);
        MyFunctions::removeFromVector(checkBoxes, checkBox);
        delete checkBox;
    }
    int row = 0;
    int column = 0;
    for(auto & classification : *classificationsList)
    {
        if(classification->getClassificationType() == Classification::Individual){
            QCheckBox * checkBox = new QCheckBox(classification->getName(), this);
            checkBox->setFont(QFont("Ubuntu", 6));
            connect(checkBox, &QCheckBox::stateChanged, this, &ClassificationsCheckBoxesWidget::changed);
            ui->gridLayout->addWidget(checkBox, row, column);
            checkBoxes.push_back(checkBox);
            row++;
            if(row > 1){
                row = 0;
                column++;
            }
        }
    }
}

QVector<Classification *> ClassificationsCheckBoxesWidget::getClassifications()
{
    QVector<Classification *> classifications;
    QVector<Classification *> copied = Classification::getSpecificTypeClassifications(*classificationsList, Classification::Individual);
    for(int i=0; i<checkBoxes.count(); i++)
    {
        if(checkBoxes[i]->isChecked() == true){
            classifications.push_back(copied[i]);
        }
    }
    return classifications;
}

bool ClassificationsCheckBoxesWidget::allUnchecked()
{
    for(auto & checkBox : checkBoxes)
        if(checkBox->isChecked() == true)
            return false;
    return true;
}

QVector<Classification *> *ClassificationsCheckBoxesWidget::getClassificationsList() const
{
    return classificationsList;
}

void ClassificationsCheckBoxesWidget::setClassificationsList(QVector<Classification *> *newClassificationsList)
{
    classificationsList = newClassificationsList;
}
