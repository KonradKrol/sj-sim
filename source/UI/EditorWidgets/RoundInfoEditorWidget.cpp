#include "RoundInfoEditorWidget.h"
#include "ui_RoundInfoEditorWidget.h"

#include "CompetitionRulesEditorWidget.h"
#include <QDebug>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

RoundInfoEditorWidget::RoundInfoEditorWidget(bool hideGroupsInfo, int competitiorsCount, bool sortStartList, short sortGroups, bool KO, int KOCount, int KOAdvance, int selection,  QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoundInfoEditorWidget)
{
    roundInfo = nullptr;
    ui->setupUi(this);
    resetPoints=new QCheckBox(tr("Resetuj punkty przed tą serią"),this);
    resetPoints->setObjectName("checkBox_resetPoints");
    resetPoints->setToolTip(tr("Wcześniejsze skoki pozostają w historii, ale nie są dodawane do łącznej noty."));
    customWeights=new QCheckBox(tr("Zaawansowane: własne wagi wcześniejszych serii"),this);
    customWeights->setObjectName("checkBox_customRoundWeights");
    weightsPanel=new QWidget(this);weightsPanel->setObjectName("previousRoundWeights");
    auto *weightsContent=new QVBoxLayout(weightsPanel);weightsContent->setContentsMargins(0,0,0,0);
    auto *hint=new QLabel(tr("0 = pomiń serię, 1 = pełne punkty. Wagi dotyczą oryginalnych not z podanych serii. Łączna nota jest zaokrąglana do 0,1 pkt."),weightsPanel);
    hint->setWordWrap(true);weightsContent->addWidget(hint);
    auto *fields=new QWidget(weightsPanel);weightsLayout=new QFormLayout(fields);weightsLayout->setContentsMargins(0,0,0,0);weightsContent->addWidget(fields);
    ui->verticalLayout->addWidget(resetPoints);ui->verticalLayout->addWidget(customWeights);ui->verticalLayout->addWidget(weightsPanel);
    connect(customWeights,&QCheckBox::toggled,weightsPanel,&QWidget::setVisible);
    connect(resetPoints,&QCheckBox::toggled,this,[this](bool reset){customWeights->setEnabled(!reset);weightsPanel->setEnabled(!reset);});
    weightsPanel->hide();setRoundIndex(0);

    if(hideGroupsInfo == true){
        ui->comboBox_sortingAfterGroups->hide();
        ui->label_sortingGroups->hide();
    }
    ui->spinBox_count->setValue(competitiorsCount);
    ui->checkBox_sortStartList->setChecked(sortStartList);
    ui->comboBox_sortingAfterGroups->setCurrentIndex(sortGroups);

    const auto updateKoFields=[this](){
        if(ui->checkBox_KORound->isChecked() == true)
        {
            ui->spinBox_KOGroupCount->show();
            ui->spinBox_AdvancingFromKOGroup->show();
            ui->label_KOCount->show();
            ui->label_KOAdvanceCount->show();
            ui->label_KOGroupsSelectionType->show();
            ui->comboBox_KOGroupsSelectionType->show();
        }
        else
        {
            ui->spinBox_KOGroupCount->hide();
            ui->spinBox_AdvancingFromKOGroup->hide();
            ui->label_KOCount->hide();
            ui->label_KOAdvanceCount->hide();
            ui->label_KOGroupsSelectionType->hide();
            ui->comboBox_KOGroupsSelectionType->hide();
            ui->label_warningIcon->hide();
        }
    };
    connect(ui->checkBox_KORound, &QCheckBox::stateChanged, this, updateKoFields);
    ui->label_warningIcon->setPixmap(QPixmap("://img/warning.png").scaled(ui->label_warningIcon->size()));

    ui->checkBox_KORound->setChecked(KO);
    updateKoFields();
    ui->spinBox_KOGroupCount->setValue(KOCount);
    ui->spinBox_AdvancingFromKOGroup->setValue(KOAdvance);
    ui->spinBox_AdvancingFromKOGroup->setMaximum(ui->spinBox_KOGroupCount->value());
    ui->comboBox_KOGroupsSelectionType->setCurrentIndex(selection);

    connect(ui->spinBox_KOGroupCount, qOverload<int>(&QSpinBox::valueChanged), this, [this](int groupSize){
        ui->spinBox_AdvancingFromKOGroup->setMaximum(groupSize);
    });

    connect(ui->checkBox_KORound, &QCheckBox::stateChanged, static_cast<CompetitionRulesEditorWidget *>(parent), &CompetitionRulesEditorWidget::KORoundChanged);
}

RoundInfoEditorWidget::~RoundInfoEditorWidget()
{
    delete ui;
}

void RoundInfoEditorWidget::fillRoundInfoInput()
{
    if(roundInfo == nullptr)
        qDebug()<<"roundInfo is nullptr!";
    else{
        ui->spinBox_count->setValue(roundInfo->getCount());
        ui->checkBox_sortStartList->setChecked(roundInfo->getSortStartList());
        ui->comboBox_sortingAfterGroups->setCurrentIndex(roundInfo->getSortAfterGroups());

        ui->checkBox_KORound->setChecked(roundInfo->getKO());
        ui->spinBox_KOGroupCount->setValue(roundInfo->getCountInKOGroup());
        ui->spinBox_AdvancingFromKOGroup->setValue(roundInfo->getAdvancingFromKOGroup());
        ui->comboBox_KOGroupsSelectionType->setCurrentIndex(roundInfo->getKoGroupsSelectionType());
        resetPoints->setChecked(roundInfo->getResetPoints());
        const auto weights=roundInfo->getPreviousRoundsWeights();
        customWeights->setChecked(!weights.isEmpty());
        for(int index=0;index<weightInputs.size();++index)weightInputs[index]->setValue(weights.value(index,1.));
    }
}

void RoundInfoEditorWidget::resetRoundInfoInput()
{
    ui->spinBox_count->setValue(0);
    ui->checkBox_sortStartList->setChecked(false);
    ui->comboBox_sortingAfterGroups->setCurrentIndex(3);

    ui->checkBox_KORound->setChecked(false);
    ui->spinBox_KOGroupCount->setValue(2);
    ui->spinBox_AdvancingFromKOGroup->setValue(1);
    ui->comboBox_KOGroupsSelectionType->setCurrentIndex(0);
    resetPoints->setChecked(false);customWeights->setChecked(false);
    for(auto *input:weightInputs)input->setValue(1.);
}

void RoundInfoEditorWidget::setRoundIndex(int index) {
    resetPoints->setVisible(index>0);customWeights->setVisible(index>0);
    while(weightInputs.size()>index) {auto *input=weightInputs.takeLast();weightsLayout->removeRow(input);}
    while(weightInputs.size()<index) {
        auto *input=new QDoubleSpinBox(weightsLayout->parentWidget());
        input->setObjectName(QString("doubleSpinBox_roundWeight_%1").arg(weightInputs.size()+1));
        input->setRange(0,1000);input->setDecimals(4);input->setSingleStep(.1);input->setValue(1.);
        const auto label=tr("Waga punktów z serii %1").arg(weightInputs.size()+1);
        input->setAccessibleName(label);weightsLayout->addRow(label,input);weightInputs.append(input);
    }
    weightsPanel->setVisible(index>0 && customWeights->isChecked());
}

bool RoundInfoEditorWidget::getResetPointsFromInput() const { return !weightInputs.isEmpty() && resetPoints->isChecked(); }
QVector<double> RoundInfoEditorWidget::getPreviousRoundsWeightsFromInput() const {
    QVector<double> weights;
    if(customWeights->isChecked())for(auto *input:weightInputs)weights.append(input->value());
    return weights;
}

int RoundInfoEditorWidget::getCountFromInput()
{
    return ui->spinBox_count->value();
}

bool RoundInfoEditorWidget::getSortStartListFromInput()
{
    return ui->checkBox_sortStartList->isChecked();
}

int RoundInfoEditorWidget::getSortAfterGroupsFromInput()
{
    return ui->comboBox_sortingAfterGroups->currentIndex();
}

bool RoundInfoEditorWidget::getKORoundFromInput()
{
    return ui->checkBox_KORound->isChecked();
}

int RoundInfoEditorWidget::getKOGroupCountFromInput()
{
    return ui->spinBox_KOGroupCount->value();
}

int RoundInfoEditorWidget::getAdvancingFromKOGroup()
{
    return ui->spinBox_AdvancingFromKOGroup->value();
}

int RoundInfoEditorWidget::getKOGroupsSelectionTypeFromInputs()
{
    return ui->comboBox_KOGroupsSelectionType->currentIndex();
}

void RoundInfoEditorWidget::hideGroupsInfo()
{
    ui->comboBox_sortingAfterGroups->hide();
    ui->label_sortingGroups->hide();
}

void RoundInfoEditorWidget::showGroupsInfo()
{
    ui->comboBox_sortingAfterGroups->show();
    ui->label_sortingGroups->show();
}

void RoundInfoEditorWidget::hideKOCheckBox()
{
    ui->checkBox_KORound->hide();
}

void RoundInfoEditorWidget::showKOCheckBox()
{
    ui->checkBox_KORound->show();
}

void RoundInfoEditorWidget::setKO(bool ok)
{
    ui->checkBox_KORound->setChecked(ok);
}

RoundInfo *RoundInfoEditorWidget::getRoundInfo() const
{
    return roundInfo;
}

void RoundInfoEditorWidget::setRoundInfo(RoundInfo *newRoundInfo)
{
    roundInfo = newRoundInfo;
}

void RoundInfoEditorWidget::on_comboBox_KOGroupsSelectionType_activated(int index)
{
    if(index != 0 || getKORoundFromInput() == false)
        ui->label_warningIcon->hide();
    else
        ui->label_warningIcon->show();
}
