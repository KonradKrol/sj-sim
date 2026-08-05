#include "SimulationSaveInfoWidget.h"
#include "ui_SimulationSaveInfoWidget.h"

SimulationSaveInfoWidget::SimulationSaveInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulationSaveInfoWidget)
{
    ui->setupUi(this);
}

SimulationSaveInfoWidget::~SimulationSaveInfoWidget()
{
    delete ui;
}

void SimulationSaveInfoWidget::resetInputs()
{
    ui->label_name->clear();
    ui->label_jumpersCount->clear();
    ui->label_currentSeason->clear();
    ui->label_playedCompetitions->clear();
    ui->label_classifications->clear();
    ui->label_actualCalendar->clear();
}

void SimulationSaveInfoWidget::fillInputs()
{
    ui->label_name->setText(simulationSave->getName());
    ui->label_jumpersCount->setText(QString::number(simulationSave->getJumpersReference().count()));
    ui->label_currentSeason->setText(QString::number(simulationSave->getActualSeason()->getSeasonNumber()));
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr)
    {
        ui->label_playedCompetitions->setText(QString::number(simulationSave->getNextCompetitionIndex()) + "/" + QString::number(simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference().count()));
        ui->label_classifications->setText(QString::number(simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference().count()));
    }
    else{
    ui->label_playedCompetitions->setText(QString::number(0) + "/" + QString::number(0));
    ui->label_classifications->setText(QString::number(0));
    }
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr)
        ui->label_actualCalendar->setText(simulationSave->getActualSeason()->getActualCalendar()->getName());
    else
        ui->label_actualCalendar->setText(tr("BRAK"));
}

SimulationSave *SimulationSaveInfoWidget::getSimulationSave() const
{
    return simulationSave;
}

void SimulationSaveInfoWidget::setSimulationSave(SimulationSave *newSimulationSave)
{
    simulationSave = newSimulationSave;
}
