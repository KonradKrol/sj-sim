#include "CompetitionInfoEditorWidget.h"
#include "ui_CompetitionInfoEditorWidget.h"
#include <QDialog>
#include "../../utilities/functions.h"

CompetitionInfoEditorWidget::CompetitionInfoEditorWidget(CompetitionInfo * competitionInfo, QVector<Hill *> *hillsList, QVector<CompetitionRules> *rulesList, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompetitionInfoEditorWidget),
    competitionInfo(competitionInfo),
    hillsList(hillsList),
    rulesList(rulesList)
{
    ui->setupUi(this);

    rulesEditor = new CompetitionRulesEditorWidget(this);
    rulesEditor->removeSubmitButton();
    rulesEditor->setStyleSheet("background: transparent;\ncolor: black;margin-left: 0px");
    rulesEditor->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    ui->verticalLayout_competitionRules->addWidget(rulesEditor);
    if(this->competitionInfo != nullptr){
        rulesEditor->setCompetitionRules(this->competitionInfo->getRulesPointer());
        if(this->competitionInfo->getTrialRound() != nullptr)
            trialRoundRules = this->competitionInfo->getTrialRound()->getRules();
        if(this->competitionInfo->getTrainingsReference().count() > 0)
            trainingsRules = this->competitionInfo->getTrainingsReference()[0]->getRules();
    }

    if(this->rulesList != nullptr)
        setupRulesComboBox();

    if(this->hillsList != nullptr)
        setupHillsComboBox();
}

void CompetitionInfoEditorWidget::setupRulesComboBox()
{
    ui->comboBox_rules->clear();
    ui->comboBox_rules->addItem(tr("BRAK"));
    for(auto & rules : *rulesList){
        ui->comboBox_rules->addItem(rules.getName());
    }
    connect(ui->comboBox_rules, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){
        int selected = ui->comboBox_rules->currentIndex();
        if(selected > 0){
            CompetitionRules * rules = const_cast<CompetitionRules *>(&rulesList->at(selected - 1));
            rulesEditor->setCompetitionRules(rules);
            rulesEditor->fillCompetitionRulesInputs();
        }
        else
            rulesEditor->resetInputs();
    });
}

CompetitionInfoEditorWidget::~CompetitionInfoEditorWidget()
{
    delete ui;
}

void CompetitionInfoEditorWidget::fillInputs()
{
    int i=0;
    ui->comboBox_hill->setCurrentIndex(0);
    for(auto & h : *hillsList){
        if(competitionInfo->getHill() == h){
            ui->comboBox_hill->setCurrentIndex(i+1);
            break;
        }
        i++;
    }
    ui->comboBox_seriesType->setCurrentIndex(competitionInfo->getSerieType());
    ui->comboBox_rules->setCurrentIndex(0);
    ui->spinBox_trainingsCount->setValue(competitionInfo->getTrainingsReference().count());
    ui->checkBox_trialRound->setChecked(competitionInfo->getTrialRound() != nullptr);

    rulesEditor->fillCompetitionRulesInputs();
    rulesEditor->fillRoundsInputs(true);
}

void CompetitionInfoEditorWidget::resetInputs()
{
    ui->comboBox_hill->setCurrentIndex(0);
    ui->comboBox_seriesType->setCurrentIndex(0);
    ui->spinBox_trainingsCount->setValue(0);
    ui->checkBox_trialRound->setChecked(false);
    rulesEditor->resetInputs();
    rulesEditor->fillRoundsInputs(true);
}

CompetitionInfo CompetitionInfoEditorWidget::getCompetitionInfoFromInputs()
{
    CompetitionInfo competition;
    if(ui->comboBox_hill->currentIndex() > 0)
        competition.setHill(const_cast<Hill*>(hillsList->at(ui->comboBox_hill->currentIndex() - 1)));
    else
        competition.setHill(defaultHill);
    competition.setSerieType(ui->comboBox_seriesType->currentIndex());
    competition.setRules(rulesEditor->getCompetitionRulesFromWidgetInputs());


    return competition;
}

void CompetitionInfoEditorWidget::setupHillsComboBox()
{
    ui->comboBox_hill->clear();
    ui->comboBox_hill->addItem(tr("BRAK"));
    for(const auto & hill : *hillsList){
        ui->comboBox_hill->addItem(QIcon(QPixmap(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()))), hill->getName() + " HS" + QString::number(hill->getHSPoint()));
    }
}

void CompetitionInfoEditorWidget::setReadOnly(bool ok)
{
    if(ok == true){
        ui->comboBox_hill->setDisabled(true);
        ui->comboBox_seriesType->setDisabled(true);
        rulesEditor->setDisabled(true);
    }
    else{
        ui->comboBox_hill->setEnabled(true);
        ui->comboBox_seriesType->setEnabled(true);
        rulesEditor->setEnabled(true);
    }
}

CompetitionRules CompetitionInfoEditorWidget::getTrainingsRules() const
{
    return trainingsRules;
}

void CompetitionInfoEditorWidget::setTrainingsRules(const CompetitionRules &newTrainingsRules)
{
    trainingsRules = newTrainingsRules;
}

bool CompetitionInfoEditorWidget::getTrialRoundChecked()
{
    return ui->checkBox_trialRound->isChecked();
}

int CompetitionInfoEditorWidget::getTrainingsCount()
{
    return ui->spinBox_trainingsCount->value();
}

CompetitionRules CompetitionInfoEditorWidget::getTrialRoundRules() const
{
    return trialRoundRules;
}

void CompetitionInfoEditorWidget::setTrialRoundRules(const CompetitionRules &newTrialRoundRules)
{
    trialRoundRules = newTrialRoundRules;
}

Hill *CompetitionInfoEditorWidget::getDefaultHill() const
{
    return defaultHill;
}

void CompetitionInfoEditorWidget::setDefaultHill(Hill *newDefaultHill)
{
    defaultHill = newDefaultHill;
}

QVector<CompetitionRules> *CompetitionInfoEditorWidget::getRulesList() const
{
    return rulesList;
}

void CompetitionInfoEditorWidget::setRulesList(QVector<CompetitionRules> *newRulesList)
{
    rulesList = newRulesList;
}

QVector<Hill *> *CompetitionInfoEditorWidget::getHillsList() const
{
    return hillsList;
}

void CompetitionInfoEditorWidget::setHillsList(QVector<Hill *> *newHillsList)
{
    hillsList = newHillsList;
}

CompetitionInfo *CompetitionInfoEditorWidget::getCompetitionInfo() const
{
    return competitionInfo;
}

void CompetitionInfoEditorWidget::setCompetitionInfo(CompetitionInfo *newCompetitionInfo)
{
    competitionInfo = newCompetitionInfo;
    rulesEditor->setCompetitionRules(competitionInfo->getRulesPointer());
}

void CompetitionInfoEditorWidget::setRulesEditor(CompetitionRulesEditorWidget *newRulesEditor)
{
    rulesEditor = newRulesEditor;
}

CompetitionRulesEditorWidget *CompetitionInfoEditorWidget::getRulesEditor() const
{
    return rulesEditor;
}

void CompetitionInfoEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}


void CompetitionInfoEditorWidget::on_pushButton_trialRoundRules_clicked()
{
    if(ui->checkBox_trialRound->isChecked()){
        QDialog * dialog = new QDialog(this);
        dialog->setWindowFlags(Qt::Window);
        dialog->setWindowTitle(tr("Zasady serii próbenj"));
        dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        dialog->setLayout(new QVBoxLayout(this));
        dialog->setStyleSheet("background-color: white; color: black;");

        CompetitionRulesEditorWidget * editor = new CompetitionRulesEditorWidget(this);
        editor->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        editor->setCompetitionRules(&trialRoundRules);
        editor->fillCompetitionRulesInputs();

        dialog->layout()->addWidget(editor);
        connect(editor, &CompetitionRulesEditorWidget::submitted, dialog, &QDialog::accept);
        if(dialog->exec() == QDialog::Accepted){
            trialRoundRules = editor->getCompetitionRulesFromWidgetInputs();
        }
    }
}

void CompetitionInfoEditorWidget::on_pushButton_trainingsRules_clicked()
{
    if(ui->spinBox_trainingsCount->value() > 0){
        QDialog * dialog = new QDialog(this);
        dialog->setWindowFlags(Qt::Window);
        dialog->setWindowTitle(tr("Zasady treningów"));
        dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        dialog->setLayout(new QVBoxLayout(this));
        dialog->setStyleSheet("background-color: white; color: black;");

        CompetitionRulesEditorWidget * editor = new CompetitionRulesEditorWidget(this);
        editor->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        editor->setCompetitionRules(&trainingsRules);
        editor->fillCompetitionRulesInputs();

        dialog->layout()->addWidget(editor);
        connect(editor, &CompetitionRulesEditorWidget::submitted, dialog, &QDialog::accept);
        if(dialog->exec() == QDialog::Accepted){
            trainingsRules = editor->getCompetitionRulesFromWidgetInputs();
        }
    }
}
