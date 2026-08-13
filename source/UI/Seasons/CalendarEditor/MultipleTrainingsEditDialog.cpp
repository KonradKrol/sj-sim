#include "MultipleTrainingsEditDialog.h"
#include "ui_MultipleTrainingsEditDialog.h"
#include "../../../global/GlobalDatabase.h"
#include <QComboBox>
#include <QLabel>

MultipleTrainingsEditDialog::MultipleTrainingsEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultipleTrainingsEditDialog)
{
    ui->setupUi(this);

    for(auto & preset : GlobalDatabase::get()->getJumpsImportancePresetsReference()){
        ui->comboBox_importancePreset->addItem(preset.getName() + " (" + QString::number(preset.getJumpsImportance(), 'f', 2) +")");
    }
}

MultipleTrainingsEditDialog::~MultipleTrainingsEditDialog()
{
    delete ui;
}

int MultipleTrainingsEditDialog::getTrainingsCount()
{
    return ui->spinBox_trainingsCount->value();
}

CompetitionRules MultipleTrainingsEditDialog::getTrainingsRules()
{
    return trainingsRules;
}

double MultipleTrainingsEditDialog::getJumpsImportance()
{
    return ui->doubleSpinBox_importance->value();
}

void MultipleTrainingsEditDialog::on_pushButton_trainingsRules_clicked()
{
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

    QWidget * widget = new QWidget(dialog);
    QHBoxLayout * layout = new QHBoxLayout(dialog);
    QLabel * label = new QLabel(dialog);
    label->setText("Zasady: ");
    QFont font("Ubuntu", 10);
    font.setBold(true);
    label->setFont(font);
    QComboBox * comboBox = new QComboBox(dialog);
    comboBox->addItem(tr("BRAK"));
    for(auto & rules : *rulesList){
        comboBox->addItem(rules.getName());
    }
    for(int rulesIndex = 0; rulesIndex < rulesList->count(); rulesIndex++){
        if(rulesList->at(rulesIndex).getID() == trainingsRules.getID()){
            comboBox->setCurrentIndex(rulesIndex + 1);
            break;
        }
    }
    layout->addWidget(label);
    layout->addWidget(comboBox);
    widget->setLayout(layout);
    connect(comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, editor, comboBox](){
        if(comboBox->currentIndex() > 0){
            CompetitionRules * rules = const_cast<CompetitionRules *>(&rulesList->at(comboBox->currentIndex() - 1));
            editor->setCompetitionRules(rules);
            editor->fillCompetitionRulesInputs();
        }
        else
            editor->resetInputs();
    });

    dialog->layout()->addWidget(widget);
    dialog->layout()->addWidget(editor);
    connect(editor, &CompetitionRulesEditorWidget::submitted, dialog, &QDialog::accept);
    if(dialog->exec() == QDialog::Accepted){
        trainingsRules = editor->getCompetitionRulesFromWidgetInputs();
    }
    delete label;
    delete comboBox;
    delete layout;
}

void MultipleTrainingsEditDialog::on_pushButton_submit_clicked()
{
    emit submitted();
}

void MultipleTrainingsEditDialog::setTrainingsRules(const CompetitionRules &newTrainingsRules)
{
    trainingsRules = newTrainingsRules;
}

void MultipleTrainingsEditDialog::setCount(int count)
{
    ui->spinBox_trainingsCount->setValue(count);
}

void MultipleTrainingsEditDialog::setImp(double imp)
{
    ui->doubleSpinBox_importance->setValue(imp);
}

QVector<CompetitionRules> *MultipleTrainingsEditDialog::getRulesList() const
{
    return rulesList;
}

void MultipleTrainingsEditDialog::setRulesList(QVector<CompetitionRules> *newRulesList)
{
    rulesList = newRulesList;
}


void MultipleTrainingsEditDialog::on_comboBox_importancePreset_currentIndexChanged(int index)
{
    if(index > 0)
    {
        index--;
        ui->doubleSpinBox_importance->setValue(GlobalDatabase::get()->getJumpsImportancePresetsReference()[index].getJumpsImportance());
    }
    else
    {
        ui->doubleSpinBox_importance->setValue(5);
    }
}
