#include "SerieTypesComboBoxesWidget.h"
#include "ui_SerieTypesComboBoxesWidget.h"
#include "../../../../competitions/CompetitionInfo.h"

SerieTypesComboBoxesWidget::SerieTypesComboBoxesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerieTypesComboBoxesWidget)
{
    ui->setupUi(this);
    connect(ui->checkBox_competitions, &QCheckBox::stateChanged, this, &SerieTypesComboBoxesWidget::changed);
    connect(ui->checkBox_qualifications, &QCheckBox::stateChanged, this, &SerieTypesComboBoxesWidget::changed);
    connect(ui->checkBox_trialRounds, &QCheckBox::stateChanged, this, &SerieTypesComboBoxesWidget::changed);
    connect(ui->checkBox_trainings, &QCheckBox::stateChanged, this, &SerieTypesComboBoxesWidget::changed);
}

SerieTypesComboBoxesWidget::~SerieTypesComboBoxesWidget()
{
    delete ui;
}

QSet<int> SerieTypesComboBoxesWidget::getSerieTypes()
{
    QSet<int> types;
    if(ui->checkBox_competitions->isChecked())
        types.insert(CompetitionInfo::Competition);
    if(ui->checkBox_qualifications->isChecked())
        types.insert(CompetitionInfo::Qualifications);
    if(ui->checkBox_trialRounds->isChecked())
        types.insert(CompetitionInfo::TrialRound);
    if(ui->checkBox_trainings->isChecked())
        types.insert(CompetitionInfo::Training);

    return types;
}
