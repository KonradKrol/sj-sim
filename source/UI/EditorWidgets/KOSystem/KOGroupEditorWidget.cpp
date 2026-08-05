#include "KOGroupEditorWidget.h"
#include "ui_KOGroupEditorWidget.h"

#include <QIcon>
#include "../../../global/CountryFlagsManager.h"
#include "../../../utilities/functions.h"

KOGroupEditorWidget::KOGroupEditorWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KOGroupEditorWidget)
{
    ui->setupUi(this);
}

KOGroupEditorWidget::~KOGroupEditorWidget()
{
    delete ui;
}

void KOGroupEditorWidget::updateGroupNumber()
{
    ui->label_groupNumber->setText(QString::number(group->getNumber()));
}

void KOGroupEditorWidget::updateComboBoxesLayout()
{
    for(auto & comboBox : comboBoxes)
    {
        ui->verticalLayout_jumpersComboBoxes->removeWidget(comboBox);
        MyFunctions::removeFromVector(comboBoxes, comboBox);
        delete comboBox;
    }
    for(auto & groupJumper : group->getJumpersReference())
    {
        QComboBox * comboBox = new QComboBox();
        for(auto & jumper : *jumpersList)
        {
            comboBox->addItem(QIcon(CountryFlagsManager::getFlagPixmap(jumper->getCountryCode().toLower())),
                              jumper->getNameAndSurname());
        }
        comboBoxes.push_back(comboBox);
        ui->verticalLayout_jumpersComboBoxes->addWidget(comboBox);
        comboBox->setCurrentIndex(jumpersList->indexOf(groupJumper));
    }
}

KOGroup KOGroupEditorWidget::getKOGroupFromInputs()
{
    KOGroup group;
    group.setNumber(this->group->getNumber());
    for(auto & comboBox : comboBoxes)
    {
        group.getJumpersReference().push_back(jumpersList->at(comboBox->currentIndex()));
    }

    return group;
}

QVector<Jumper *> KOGroupEditorWidget::getJumpersFromInputs()
{
    KOGroup group;
    for(auto & comboBox : comboBoxes)
    {
        group.getJumpersReference().push_back(jumpersList->at(comboBox->currentIndex()));
    }
    return group.getJumpersReference();
}

void KOGroupEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

QVector<Jumper *> *KOGroupEditorWidget::getJumpersList() const
{
    return jumpersList;
}

void KOGroupEditorWidget::setJumpersList(QVector<Jumper *> *newJumpersList)
{
    jumpersList = newJumpersList;
}

KOGroup *KOGroupEditorWidget::getGroup() const
{
    return group;
}

void KOGroupEditorWidget::setGroup(KOGroup *newGroup)
{
    group = newGroup;
}

