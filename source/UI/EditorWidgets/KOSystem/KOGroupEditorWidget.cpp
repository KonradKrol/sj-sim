#include "KOGroupEditorWidget.h"
#include "ui_KOGroupEditorWidget.h"

#include <QIcon>
#include "../../../global/CountryFlagsManager.h"
#include "../../../utilities/functions.h"

KOGroupEditorWidget::KOGroupEditorWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KOGroupEditorWidget),
    group(nullptr),
    jumpersList(nullptr)
{
    ui->setupUi(this);
}

KOGroupEditorWidget::~KOGroupEditorWidget()
{
    delete ui;
}

void KOGroupEditorWidget::updateGroupNumber()
{
    ui->label_groupNumber->setText(group != nullptr ? QString::number(group->getNumber()) : QStringLiteral("-"));
}

void KOGroupEditorWidget::updateComboBoxesLayout()
{
    qDeleteAll(comboBoxes);
    comboBoxes.clear();
    if(group == nullptr || jumpersList == nullptr)
        return;
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
    if(this->group == nullptr || jumpersList == nullptr)
        return group;
    group.setNumber(this->group->getNumber());
    for(auto & comboBox : comboBoxes)
    {
        const int index = comboBox->currentIndex();
        if(index >= 0 && index < jumpersList->count())
            group.getJumpersReference().push_back(jumpersList->at(index));
    }

    return group;
}

QVector<Jumper *> KOGroupEditorWidget::getJumpersFromInputs()
{
    KOGroup group;
    if(jumpersList == nullptr)
        return group.getJumpersReference();
    for(auto & comboBox : comboBoxes)
    {
        const int index = comboBox->currentIndex();
        if(index >= 0 && index < jumpersList->count())
            group.getJumpersReference().push_back(jumpersList->at(index));
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
