#include "CompetitionRulesEditorWidget.h"
#include "ui_CompetitionRulesEditorWidget.h"

#include "RoundInfoEditorWidget.h"
#include "../../utilities/functions.h"

CompetitionRulesEditorWidget::CompetitionRulesEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompetitionRulesEditorWidget)
{
    ui->setupUi(this);
    competitionRules = nullptr;

    ui->toolBox_rounds->removeItem(0);
    ui->toolBox_rounds->removeItem(0);
    delete ui->page;
    delete ui->page_2;

    connect(ui->spinBox_roundsCount, &QSpinBox::editingFinished, this, [this](){
        fillRoundsInputs(ui->spinBox_roundsCount->value());
    });
    connect(ui->comboBox_competitionType, qOverload<int>(&QComboBox::currentIndexChanged), this, &CompetitionRulesEditorWidget::competitionTypeChanged);
    connect(ui->spinBox_jumpersInTeamCount, qOverload<int>(&QSpinBox::valueChanged), this, &CompetitionRulesEditorWidget::jumpersCountInTeamChanged);
    connect(this, &CompetitionRulesEditorWidget::competitionTypeChanged, this, [this](){
        if(ui->comboBox_competitionType->currentIndex() == CompetitionRules::Individual){
            ui->spinBox_jumpersInTeamCount->setHidden(true);
            ui->label_jumpersInTeam->setHidden(true);
            for(int i=0; i<ui->toolBox_rounds->count(); i++){
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->hideGroupsInfo();
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->showKOCheckBox();
            }
        }
        else{ //druż
            ui->spinBox_jumpersInTeamCount->setHidden(false);
            ui->label_jumpersInTeam->setHidden(false);
            for(int i=0; i<ui->toolBox_rounds->count(); i++){
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->showGroupsInfo();
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->setKO(false);
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->hideKOCheckBox();

            }
        }
    });
    emit competitionTypeChanged();

}

CompetitionRulesEditorWidget::~CompetitionRulesEditorWidget()
{
    delete ui;
}

void CompetitionRulesEditorWidget::resetInputs()
{
    ui->lineEdit_name->clear();
    ui->spinBox_roundsCount->setValue(0);
    ui->checkBox_windCompensation->setChecked(true);
    ui->checkBox_gateCompensation->setChecked(true);
    ui->checkBox_95HsRule->setChecked(true);
    ui->checkBox_judgesPoints->setChecked(true);
    ui->checkBox_dsq->setChecked(true);
    ui->spinBox_jumpersInTeamCount->setValue(0);
    ui->comboBox_competitionType->setCurrentIndex(CompetitionRules::Individual);
    ui->comboBox_windAverageCalculatingType->setCurrentIndex(2);
    ui->comboBox_windCompensationDistanceEffect->setCurrentIndex(1);
    ui->checkBox_hillRecordBreaking->setChecked(true);
}

void CompetitionRulesEditorWidget::fillCompetitionRulesInputs()
{
    ui->lineEdit_name->setText(competitionRules->getName());
    ui->spinBox_roundsCount->setValue(competitionRules->getRounds().count());
    ui->checkBox_windCompensation->setChecked(competitionRules->getHasWindCompensations());
    ui->checkBox_gateCompensation->setChecked(competitionRules->getHasGateCompensations());
    ui->checkBox_95HsRule->setChecked(competitionRules->getHas95HSRule());
    ui->checkBox_judgesPoints->setChecked(competitionRules->getHasJudgesPoints());
    ui->checkBox_dsq->setChecked(competitionRules->getHasDsq());
    if(competitionRules->getCompetitionType() == CompetitionRules::Team){
        ui->spinBox_jumpersInTeamCount->setEnabled(true);
        ui->spinBox_jumpersInTeamCount->setValue(competitionRules->getJumpersInTeamCount());
    }
    ui->comboBox_competitionType->setCurrentIndex(competitionRules->getCompetitionType());
    ui->comboBox_windAverageCalculatingType->setCurrentIndex(competitionRules->getWindAverageCalculatingType());
    ui->comboBox_windCompensationDistanceEffect->setCurrentIndex(competitionRules->getWindCompensationDistanceEffect());
    ui->checkBox_hillRecordBreaking->setChecked(competitionRules->getHillRecordBreaking());
    fillRoundsInputs(true);
}

void CompetitionRulesEditorWidget::fillRoundsInputs(bool setup)
{
    int roundsCount = ui->spinBox_roundsCount->value();
    while (ui->toolBox_rounds->count() > roundsCount)
    {
        delete ui->toolBox_rounds->widget(ui->toolBox_rounds->count() - 1);
    }

    int i=ui->toolBox_rounds->count() - 1; //toolBox_count: 2, roundsCount: 3
    while(ui->toolBox_rounds->count() < roundsCount)
    {
        int count = 1;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            count = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getCountFromInput();

        short sortGroups = 0;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            sortGroups = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getSortAfterGroupsFromInput();

        bool sortStartList = true;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            sortStartList = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getSortStartListFromInput();

        bool KO = false;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            sortStartList = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getKORoundFromInput();

        int KOGroupCount = 0;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            sortStartList = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getKOGroupCountFromInput();

        int KOAdvanceCount = 0;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            sortStartList = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getAdvancingFromKOGroup();

        int selection = 0;
        if(i - 1 >= 0) //Czyli jeżeli istnieje poprzednia runda
            sortStartList = dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getKOGroupsSelectionTypeFromInputs();

        ui->toolBox_rounds->addItem(new RoundInfoEditorWidget(getCompetitionTypeFromInput() == CompetitionRules::Individual, count, sortStartList, sortGroups, KO, KOGroupCount, KOAdvanceCount, selection, this), "");
    }
    for(int i=0; i<ui->toolBox_rounds->count(); i++)
    {
        ui->toolBox_rounds->setItemText(i, "Seria " + QString::number(i+1));
    }
    if(setup == true){
        disconnect(ui->spinBox_roundsCount, qOverload<int>(&QSpinBox::valueChanged), this, &CompetitionRulesEditorWidget::fillRoundsInputs);
        for(int i=0; i<ui->toolBox_rounds->count(); i++){
            if(competitionRules->getRounds().count() > i){
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->setRoundInfo(const_cast<RoundInfo *>(&competitionRules->getRounds().at(i)));
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->fillRoundInfoInput();
            }
            //else{
                //dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->resetRoundInfoInput();
            //}
            connect(ui->spinBox_roundsCount, qOverload<int>(&QSpinBox::valueChanged), this, [this](){
                fillRoundsInputs(false);
            });
        }
    }
}

void CompetitionRulesEditorWidget::removeSubmitButton()
{
    disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &CompetitionRulesEditorWidget::on_pushButton_submit_clicked);
    //ui->verticalLayout_characteristicsEditor->removeWidget(ui->pushButton_submit);
    delete ui->pushButton_submit;
}

QString CompetitionRulesEditorWidget::getNameFromInput()
{
    return ui->lineEdit_name->text();
}

QVector<RoundInfo> CompetitionRulesEditorWidget::getRoundsFromInput()
{
    QVector<RoundInfo> rounds;
    for(int i=0; i<ui->toolBox_rounds->count(); i++)
    {
        RoundInfo round;
        round.setCount(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getCountFromInput());
        round.setSortStartList(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getSortStartListFromInput());
        round.setSortAfterGroups(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getSortAfterGroupsFromInput());
        round.setKO(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getKORoundFromInput());
        round.setCountInKOGroup(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getKOGroupCountFromInput());
        round.setAdvancingFromKOGroup(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getAdvancingFromKOGroup());
        round.setKoGroupsSelectionType(dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->getKOGroupsSelectionTypeFromInputs());
        rounds.push_back(round);
    }
    return rounds;
}

bool CompetitionRulesEditorWidget::getHas95HSRuleFromInput()
{
    return ui->checkBox_95HsRule->isChecked();
}

bool CompetitionRulesEditorWidget::getHasWindCompensations()
{
    return ui->checkBox_windCompensation->isChecked();
}

bool CompetitionRulesEditorWidget::getHasGateCompensations()
{
    return ui->checkBox_gateCompensation->isChecked();
}

bool CompetitionRulesEditorWidget::getHasJudgesPoints()
{
    return ui->checkBox_judgesPoints->isChecked();
}

bool CompetitionRulesEditorWidget::getHasDsq()
{
    return ui->checkBox_dsq->isChecked();
}

int CompetitionRulesEditorWidget::getJumpersCountInTeam()
{
    return ui->spinBox_jumpersInTeamCount->value();
}

short CompetitionRulesEditorWidget::getCompetitionTypeFromInput()
{
    return ui->comboBox_competitionType->currentIndex();
}

short CompetitionRulesEditorWidget::getWindCompensationDistanceEffectFromInput()
{
    return ui->comboBox_windCompensationDistanceEffect->currentIndex();
}

short CompetitionRulesEditorWidget::getWindAverageCalculatingTypeFromInput()
{
    return ui->comboBox_windAverageCalculatingType->currentIndex();
}

CompetitionRules CompetitionRulesEditorWidget::getCompetitionRulesFromWidgetInputs()
{
    CompetitionRules rules;
    rules.setName(getNameFromInput());
    rules.setHas95HSRule(getHas95HSRuleFromInput());
    rules.setHasWindCompensations(getHasWindCompensations());
    rules.setHasGateCompensations(getHasGateCompensations());
    rules.setHasJudgesPoints(getHasJudgesPoints());
    rules.setHasDsq(getHasDsq());
    rules.setCompetitionType(getCompetitionTypeFromInput());
    rules.setRounds(getRoundsFromInput());
    rules.setWindAverageCalculatingType(getWindAverageCalculatingTypeFromInput());
    rules.setWindCompensationDistanceEffect(getWindCompensationDistanceEffectFromInput());
    rules.setJumpersInTeamCount(getJumpersCountInTeam());
    rules.setHillRecordBreaking(ui->checkBox_hillRecordBreaking->isChecked());
    return rules;
}

CompetitionRules *CompetitionRulesEditorWidget::getCompetitionRules() const
{
    return competitionRules;
}

void CompetitionRulesEditorWidget::setCompetitionRules(CompetitionRules *newCompetitionRules)
{
    competitionRules = newCompetitionRules;
}

void CompetitionRulesEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}
