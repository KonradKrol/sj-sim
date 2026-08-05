#include "JumpDataInfoChoiceDialog.h"
#include "ui_JumpDataInfoChoiceDialog.h"

JumpDataInfoChoiceDialog::JumpDataInfoChoiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumpDataInfoChoiceDialog)
{
    ui->setupUi(this);
}

JumpDataInfoChoiceDialog::~JumpDataInfoChoiceDialog()
{
    delete ui;
}

JumpDataInfoChoice JumpDataInfoChoiceDialog::getChoiceFromInputs()
{
    JumpDataInfoChoice ch;
    ch.setDistance(ui->checkBox_distance->isChecked());
    ch.setPoints(ui->checkBox_points->isChecked());
    ch.setGate(ui->checkBox_gate->isChecked());
    ch.setAveragedWind(ui->checkBox_averagedWind->isChecked());
    ch.setGateCompensation(ui->checkBox_gateCompensation->isChecked());
    ch.setWindCompensation(ui->checkBox_windCompensation->isChecked());
    ch.setTotalCompensation(ui->checkBox_totalCompensation->isChecked());
    ch.setJudges(ui->checkBox_judges->isChecked());
    ch.setJudgesPoints(ui->checkBox_judgesPoints->isChecked());
    ch.setLandingType(ui->checkBox_landingType->isChecked());
    ch.setPositionAfterJump(ui->checkBox_positionAfterJump->isChecked());
    ch.setJumpPositionInRound(ui->checkBox_jumpPositionInRound->isChecked());
    ch.setSpecificWind(ui->checkBox_specificWind->isChecked());
    ch.setTakeoffRating(ui->checkBox_takeoffRating->isChecked());
    ch.setFlightRating(ui->checkBox_flightRating->isChecked());
    ch.setDistanceRandom(ui->checkBox_distanceRandom->isChecked());
    ch.setInrunSnow(ui->checkBox_snowRain->isChecked());
    return ch;
}

void JumpDataInfoChoiceDialog::fillInputs()
{
    ui->checkBox_distance->setChecked(choice->getDistance());
    ui->checkBox_points->setChecked(choice->getPoints());
    ui->checkBox_gate->setChecked(choice->getGate());
    ui->checkBox_averagedWind->setChecked(choice->getAveragedWind());
    ui->checkBox_gateCompensation->setChecked(choice->getGateCompensation());
    ui->checkBox_windCompensation->setChecked(choice->getWindCompensation());
    ui->checkBox_totalCompensation->setChecked(choice->getTotalCompensation());
    ui->checkBox_judges->setChecked(choice->getJudges());
    ui->checkBox_judgesPoints->setChecked(choice->getJudgesPoints());
    ui->checkBox_landingType->setChecked(choice->getLandingType());
    ui->checkBox_positionAfterJump->setChecked(choice->getPositionAfterJump());
    ui->checkBox_jumpPositionInRound->setChecked(choice->getJumpPositionInRound());
    ui->checkBox_specificWind->setChecked(choice->getSpecificWind());
    ui->checkBox_takeoffRating->setChecked(choice->getTakeoffRating());
    ui->checkBox_flightRating->setChecked(choice->getFlightRating());
    ui->checkBox_distanceRandom->setChecked(choice->getDistanceRandom());
    ui->checkBox_snowRain->setChecked(choice->getInrunSnow());
}

void JumpDataInfoChoiceDialog::on_pushButton_submit_clicked()
{
    accept();
}

JumpDataInfoChoice *JumpDataInfoChoiceDialog::getChoice() const
{
    return choice;
}

void JumpDataInfoChoiceDialog::setChoice(JumpDataInfoChoice *newChoice)
{
    choice = newChoice;
}

