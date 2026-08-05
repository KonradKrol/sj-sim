#include "JumpManipulatorConfigWindow.h"
#include "ui_JumpManipulatorConfigWindow.h"

#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"

#include <QPair>

JumpManipulatorConfigWindow::JumpManipulatorConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumpManipulatorConfigWindow)
{
    ui->setupUi(this);
}

JumpManipulatorConfigWindow::~JumpManipulatorConfigWindow()
{
    delete ui;
}

JumpManipulator JumpManipulatorConfigWindow::getJumpManipulatorFromInputs() const
{
    JumpManipulator m;
    m.setDistanceRange(QPair<double, double>(ui->doubleSpinBox_minDistance->value(), ui->doubleSpinBox_maxDistance->value()));
    m.setInrunSnowRange(QPair<double, double>(ui->doubleSpinBox_minInrunSnow->value(), ui->doubleSpinBox_maxInrunSnow->value()));
    m.setTakeoffRatingRange(QPair<double, double>(ui->doubleSpinBox_minTakeoffRating->value(), ui->doubleSpinBox_maxTakeoffRating->value()));
    m.setFlightRatingRange(QPair<double, double>(ui->doubleSpinBox_minFlightRating->value(), ui->doubleSpinBox_maxFlightRating->value()));
    if(ui->checkBox_disableAveragedWindRange->isChecked() == false){
        m.setAveragedWindRange(QPair<double, double>(ui->doubleSpinBox_minAveragedWind->value(), ui->doubleSpinBox_maxAveragedWind->value()));
        m.setAveragedWindRangeEnabled(true);
    }
    else m.setAveragedWindRangeEnabled(false);

    m.setDistanceBonus(ui->doubleSpinBox_distanceBonus->value());
    m.setInrunSnowBonus(ui->doubleSpinBox_inrunSnowBonus->value());
    m.setTakeoffRatingBonus(ui->doubleSpinBox_takeoffRatingBonus->value());
    m.setFlightRatingBonus(ui->doubleSpinBox_flightRatingBonus->value());
    m.setAveragedWindBonus(ui->doubleSpinBox_averagedWindBonus->value());

    m.setExactLandingType(ui->comboBox_landingType->currentIndex() - 1);
    QVector<double> judges;
    judges.resize(5);
    judges[0] = ui->doubleSpinBox_judge1->value();
    judges[1] = ui->doubleSpinBox_judge2->value();
    judges[2] = ui->doubleSpinBox_judge3->value();
    judges[3] = ui->doubleSpinBox_judge4->value();
    judges[4] = ui->doubleSpinBox_judge5->value();
    m.setExactJudges(judges);

    m.setExactDSQProbability(ui->spinBox_dsqProbability->value());
    m.setExactWinds(this->exactWinds);

    return m;
}

void JumpManipulatorConfigWindow::on_checkBox_disableAveragedWindRange_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->doubleSpinBox_minAveragedWind->setEnabled(false);
        ui->doubleSpinBox_maxAveragedWind->setEnabled(false);
        ui->doubleSpinBox_minAveragedWind->setStyleSheet(ui->doubleSpinBox_minAveragedWind->styleSheet());
        ui->doubleSpinBox_maxAveragedWind->setStyleSheet(ui->doubleSpinBox_maxAveragedWind->styleSheet());
    }
    else{
        ui->doubleSpinBox_minAveragedWind->setEnabled(true);
        ui->doubleSpinBox_maxAveragedWind->setEnabled(true);
        ui->doubleSpinBox_minAveragedWind->setStyleSheet(ui->doubleSpinBox_minAveragedWind->styleSheet());
        ui->doubleSpinBox_maxAveragedWind->setStyleSheet(ui->doubleSpinBox_maxAveragedWind->styleSheet());
    }
}


void JumpManipulatorConfigWindow::on_pushButton_submit_2_clicked()
{
    emit submitted();
}


void JumpManipulatorConfigWindow::on_pushButton_editWinds_clicked()
{
    QDialog * dialog = new QDialog;
    dialog->setModal(true);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Edytuj wiatr"));
    dialog->setStyleSheet("background-color: rgb(225, 225, 225);");
    dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setFixedSize(dialog->size());
    dialog->setLayout(new QVBoxLayout(dialog));

    WindsGeneratorSettingsEditorWidget * windsEditor = new WindsGeneratorSettingsEditorWidget;
    windsEditor->setKPoint(this->getKPoint());
    windsEditor->setWindGenerationSettings(this->windGenerationSettings);
    windsEditor->fillSettingsInputsToExactWindsEditor();
    dialog->layout()->addWidget(windsEditor);
    dialog->raise();

    connect(windsEditor, &WindsGeneratorSettingsEditorWidget::submitted, dialog, &QDialog::accept);
    if(dialog->exec() == QDialog::Accepted){
        this->exactWinds = windsEditor->getExactWindsFromInputs();
        delete windsEditor;
        delete dialog;
    }
}

double JumpManipulatorConfigWindow::getKPoint() const
{
    return KPoint;
}

void JumpManipulatorConfigWindow::setKPoint(double newKPoint)
{
    KPoint = newKPoint;
}

QVector<WindGenerationSettings> *JumpManipulatorConfigWindow::getWindGenerationSettings() const
{
    return windGenerationSettings;
}

void JumpManipulatorConfigWindow::setWindGenerationSettings(QVector<WindGenerationSettings> *newWindGenerationSettings)
{
    windGenerationSettings = newWindGenerationSettings;
}

