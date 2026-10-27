#include <QObject>
#include "CompetitionRulesEditorWidget.h"
#include "ui_CompetitionRulesEditorWidget.h"

#include "RoundInfoEditorWidget.h"
#include <QEvent>
#include <QTimer>
#include <QScopedValueRollback>
#include <QDoubleSpinBox>
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

    connect(ui->spinBox_roundsCount, qOverload<int>(&QSpinBox::valueChanged), this, [this](){
        fillRoundsInputs(false);
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
    watchInputs();

}

CompetitionRulesEditorWidget::~CompetitionRulesEditorWidget()
{
    delete ui;
}

void CompetitionRulesEditorWidget::resetInputs()
{
    QScopedValueRollback<bool> loading(loadingInputs,true);
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
    QScopedValueRollback<bool> loading(loadingInputs,true);
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
    QScopedValueRollback<bool> loading(loadingInputs,true);
    int roundsCount = ui->spinBox_roundsCount->value();
    while (ui->toolBox_rounds->count() > roundsCount)
    {
        delete ui->toolBox_rounds->widget(ui->toolBox_rounds->count() - 1);
    }

    while(ui->toolBox_rounds->count() < roundsCount)
    {
        auto *previous = qobject_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(ui->toolBox_rounds->count()-1));
        const int count = previous ? previous->getCountFromInput() : 1;
        const short sortGroups = previous ? previous->getSortAfterGroupsFromInput() : 0;
        ui->toolBox_rounds->addItem(new RoundInfoEditorWidget(getCompetitionTypeFromInput() == CompetitionRules::Individual,
            count, true, sortGroups, false, 0, 0, 0, this), "");
    }
    for(int i=0; i<ui->toolBox_rounds->count(); i++)
    {
        ui->toolBox_rounds->setItemText(i, QObject::tr("Seria ") + QString::number(i+1));
        static_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->setRoundIndex(i);
    }
    if(setup == true){
        for(int i=0; i<ui->toolBox_rounds->count(); i++){
            if(competitionRules->getRounds().count() > i){
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->setRoundInfo(&competitionRules->getRoundsReference()[i]);
                dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->fillRoundInfoInput();
            }
            //else{
                //dynamic_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i))->resetRoundInfoInput();
            //}
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
        auto *editor=static_cast<RoundInfoEditorWidget *>(ui->toolBox_rounds->widget(i));
        round.setResetPoints(editor->getResetPointsFromInput());
        round.setPreviousRoundsWeights(editor->getPreviousRoundsWeightsFromInput());
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

void CompetitionRulesEditorWidget::watchInputs() {
    installEventFilter(this);
    for(auto *object:findChildren<QObject *>()) {
        object->installEventFilter(this);
        if(object->property("rulesInputWatched").toBool())continue;
        object->setProperty("rulesInputWatched",true);
        const auto changed=[this]{if(!loadingInputs)emit configurationChanged();};
        if(auto *input=qobject_cast<QSpinBox *>(object))connect(input,qOverload<int>(&QSpinBox::valueChanged),this,changed);
        else if(auto *input=qobject_cast<QDoubleSpinBox *>(object))connect(input,qOverload<double>(&QDoubleSpinBox::valueChanged),this,changed);
        else if(auto *input=qobject_cast<QCheckBox *>(object))connect(input,&QCheckBox::toggled,this,changed);
        else if(auto *input=qobject_cast<QComboBox *>(object))connect(input,qOverload<int>(&QComboBox::activated),this,changed);
        else if(auto *input=qobject_cast<QLineEdit *>(object);input && !qobject_cast<QAbstractSpinBox *>(input->parentWidget()))connect(input,&QLineEdit::editingFinished,this,changed);
    }
}
bool CompetitionRulesEditorWidget::eventFilter(QObject *object,QEvent *event) {
    if(event->type()==QEvent::ChildAdded && !watchQueued){watchQueued=true;QTimer::singleShot(0,this,[this]{watchQueued=false;watchInputs();});}
    return QWidget::eventFilter(object,event);
}
