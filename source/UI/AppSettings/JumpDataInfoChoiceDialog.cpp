#include "JumpDataInfoChoiceDialog.h"
#include "ui_JumpDataInfoChoiceDialog.h"
#include <QSettings>

JumpDataInfoChoiceDialog::JumpDataInfoChoiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumpDataInfoChoiceDialog)
{
    ui->setupUi(this);
    auto *daily=new QCheckBox(tr("Forma dnia (+/−)"),this);daily->setObjectName("checkBox_dailyFormBonus");
    daily->setChecked(QSettings().value("quickUi/jumpDetails/dailyFormBonus",false).toBool());
    ui->layout_details->addWidget(daily,ui->layout_details->rowCount(),0,1,2);
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
    ch.setPositionAfterJump(ui->checkBox_positionAfterJump->isChecked());
    ch.setJumpPositionInRound(ui->checkBox_jumpPositionInRound->isChecked());
    ch.setJudges(ui->checkBox_judges->isChecked());
    ch.setJudgesPoints(ui->checkBox_judgesPoints->isChecked());
    ch.setAveragedWind(ui->checkBox_averagedWind->isChecked());
    ch.setGate(ui->checkBox_gate->isChecked());
    ch.setLandingType(ui->checkBox_landingType->isChecked());
    ch.setWindCompensation(ui->checkBox_windCompensation->isChecked());
    ch.setGateCompensation(ui->checkBox_gateCompensation->isChecked());
    ch.setTotalCompensation(ui->checkBox_totalCompensation->isChecked());

    return ch;
}

void JumpDataInfoChoiceDialog::fillInputs()
{
    ui->checkBox_distance->setChecked(choice->getDistance());
    ui->checkBox_points->setChecked(choice->getPoints());
    ui->checkBox_positionAfterJump->setChecked(choice->getPositionAfterJump());
    ui->checkBox_jumpPositionInRound->setChecked(choice->getJumpPositionInRound());
    ui->checkBox_judges->setChecked(choice->getJudges());
    ui->checkBox_judgesPoints->setChecked(choice->getJudgesPoints());
    ui->checkBox_averagedWind->setChecked(choice->getAveragedWind());
    ui->checkBox_gate->setChecked(choice->getGate());
    ui->checkBox_landingType->setChecked(choice->getLandingType());
    ui->checkBox_windCompensation->setChecked(choice->getWindCompensation());
    ui->checkBox_gateCompensation->setChecked(choice->getGateCompensation());
    ui->checkBox_totalCompensation->setChecked(choice->getTotalCompensation());
    ui->checkBox_totalPoints->setChecked(QSettings().value("quickUi/jumpDetails/totalPoints",true).toBool());
    ui->checkBox_leaderGap->setChecked(QSettings().value("quickUi/jumpDetails/leaderGap",true).toBool());
    ui->checkBox_weights->setChecked(QSettings().value("quickUi/jumpDetails/weights",true).toBool());
    ui->checkBox_quality->setChecked(QSettings().value("quickUi/jumpDetails/quality",false).toBool());
    findChild<QCheckBox *>("checkBox_dailyFormBonus")->setChecked(QSettings().value("quickUi/jumpDetails/dailyFormBonus",false).toBool());
    ui->checkBox_precipitation->setChecked(QSettings().value("quickUi/jumpDetails/precipitation",false).toBool());

}

void JumpDataInfoChoiceDialog::on_pushButton_submit_clicked()
{
    QSettings().setValue("quickUi/jumpDetails/totalPoints",ui->checkBox_totalPoints->isChecked());
    QSettings().setValue("quickUi/jumpDetails/leaderGap",ui->checkBox_leaderGap->isChecked());
    QSettings().setValue("quickUi/jumpDetails/weights",ui->checkBox_weights->isChecked());
    QSettings().setValue("quickUi/jumpDetails/quality",ui->checkBox_quality->isChecked());
    QSettings().setValue("quickUi/jumpDetails/precipitation",ui->checkBox_precipitation->isChecked());
    QSettings().setValue("quickUi/jumpDetails/dailyFormBonus",findChild<QCheckBox *>("checkBox_dailyFormBonus")->isChecked());
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

