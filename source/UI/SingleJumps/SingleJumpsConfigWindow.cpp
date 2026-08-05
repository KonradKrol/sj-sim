#include "SingleJumpsConfigWindow.h"
#include "ui_SingleJumpsConfigWindow.h"

#include "../EditorWidgets/JumperEditorWidget.h"
#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalSimulationSettings.h"

#include <QStringList>
#include <QStringListModel>
#include <QScrollArea>
#include <QCloseEvent>
#include <QMessageBox>

SingleJumpsConfigWindow::SingleJumpsConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingleJumpsConfigWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    ui->spinBox_dsqProbability->setValue(GlobalSimulationSettings::get()->getBaseDsqProbability());
    ui->pushButton_submit->setDefault(true);
    ui->pushButton_submit->setToolTip(tr("Rozpocznij symulację z aktualnymi ustawieniami (Enter)"));
    ui->spinBox_jumpsCount->setToolTip(tr("Liczba skoków do wykonania; minimum to 1"));

    jumperEditor = new JumperEditorWidget;
    jumperEditor->removeSubmitButton();

    hillEditor = new HillEditorWidget;
    hillEditor->removeSubmitButton();

    windsGeneratorSettingsEditor = new WindsGeneratorSettingsEditorWidget;
    windsGeneratorSettingsEditor->setRemovingSubmitButtons(true);
    windsGeneratorSettingsEditor->removeSubmitButton();

    ui->comboBox_existingJumper->addItem(tr("BRAK"));
    for(const auto & jumper : GlobalDatabase::get()->getGlobalJumpers())
    {
        ui->comboBox_existingJumper->addItem(QIcon(QPixmap(CountryFlagsManager::getFlagPixmap(jumper.getCountryCode().toLower()))) ,jumper.getNameAndSurname());
    }

    ui->comboBox_existingHill->addItem(tr("BRAK"));
    for(const auto & hill : GlobalDatabase::get()->getGlobalHills())
    {
        ui->comboBox_existingHill->addItem(QIcon(QPixmap(CountryFlagsManager::getFlagPixmap(hill.getCountryCode().toLower()))) ,hill.getName() + " HS" + QString::number(hill.getHSPoint()));
    }

    ui->verticalLayout_jumperContent->addWidget(jumperEditor);
    ui->verticalLayout_hillContent->addWidget(hillEditor);
    ui->verticalLayout_windsGeneratorSettings->addWidget(windsGeneratorSettingsEditor);
    connect(hillEditor, &HillEditorWidget::KPointInputChanged, this, [this](){
        windsGeneratorSettingsEditor->setKPoint(hillEditor->getKPointFromInput());
        windsGeneratorSettingsEditor->setWindGenerationSettings(new QVector<WindGenerationSettings>());
        windsGeneratorSettingsEditor->fillWindGenerationSettingsByDefault();
        windsGeneratorSettingsEditor->fillSettingsInputs();
    });

    ui->toolBox->setCurrentIndex(0);

    for(auto & preset : GlobalDatabase::get()->getJumpsImportancePresetsReference())
    {
        ui->comboBox_jumpsImportancePreset->addItem(preset.getName() + " (" + QString::number(preset.getJumpsImportance(), 'f', 2) + ")");
    }
    connect(ui->comboBox_jumpsImportancePreset, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index){
        if(index > 0)
            ui->doubleSpinBox_jumpsImportance->setValue(GlobalDatabase::get()->getJumpsImportancePresetsReference()[index - 1].getJumpsImportance());
        else
            ui->doubleSpinBox_jumpsImportance->setValue(5.0);
    });

    const auto updateWindCompensationInputs = [this](){
        const bool enabled = ui->checkBox_hasWindCompensation->isChecked();
        ui->comboBox_windCompensationDistanceEffect->setEnabled(enabled);
        ui->comboBox_windAverageCalculatingType->setEnabled(enabled);
    };
    connect(ui->checkBox_hasWindCompensation, &QCheckBox::toggled, this, updateWindCompensationInputs);
    updateWindCompensationInputs();
}

SingleJumpsConfigWindow::~SingleJumpsConfigWindow()
{
    delete jumperEditor;
    delete hillEditor;
    delete windsGeneratorSettingsEditor;
    delete ui;
}

void SingleJumpsConfigWindow::on_comboBox_existingJumper_currentIndexChanged(int index)
{
    if(index > 0){
        jumperEditor->setJumper(const_cast<Jumper *>(&GlobalDatabase::get()->getGlobalJumpers().at(index - 1)));
        jumperEditor->fillJumperInputs();
    }
    else jumperEditor->resetJumperInputs();
}

void SingleJumpsConfigWindow::on_comboBox_existingHill_currentIndexChanged(int index)
{
    if(index > 0){
        hillEditor->setHill(const_cast<Hill *>(&GlobalDatabase::get()->getGlobalHills().at(index - 1)));
        hillEditor->fillHillInputs();
        emit hillEditor->KPointInputChanged(const_cast<Hill *>(&GlobalDatabase::get()->getGlobalHills().at(index - 1))->getKPoint());
    }
    else hillEditor->resetHillInputs();
}

void SingleJumpsConfigWindow::on_pushButton_submit_clicked()
{
    const Jumper jumper = jumperEditor->getJumperFromWidgetInput();
    if(jumper.getNameAndSurname().trimmed().isEmpty())
    {
        QMessageBox::warning(this, tr("Brak zawodnika"),
                             tr("Wybierz istniejącego zawodnika albo wpisz jego imię i nazwisko."), QMessageBox::Ok);
        ui->toolBox->setCurrentIndex(0);
        ui->comboBox_existingJumper->setFocus();
        return;
    }

    const Hill hill = hillEditor->getHillFromWidgetInput();
    if(hill.getKPoint() <= 0 || hill.getHSPoint() <= 0 || hill.getHSPoint() < hill.getKPoint())
    {
        QMessageBox::warning(this, tr("Nieprawidłowa skocznia"),
                             tr("Wybierz lub skonfiguruj skocznię z dodatnim punktem K oraz punktem HS nie mniejszym od K."), QMessageBox::Ok);
        ui->toolBox->setCurrentIndex(1);
        ui->comboBox_existingHill->setFocus();
        return;
    }

    accept();
}

WindsGeneratorSettingsEditorWidget *SingleJumpsConfigWindow::getWindsGeneratorSettingsEditor() const
{
    return windsGeneratorSettingsEditor;
}

int SingleJumpsConfigWindow::getJumpsCountFromInput()
{
    return ui->spinBox_jumpsCount->value();
}

bool SingleJumpsConfigWindow::getChangeableWindFromInput()
{
    return ui->checkBox_changeableWind->isChecked();
}

short SingleJumpsConfigWindow::getWindAverageCalculatingType()
{
    return ui->comboBox_windAverageCalculatingType->currentIndex();
}

short SingleJumpsConfigWindow::getWindCompensationDistanceEffect()
{
    return ui->comboBox_windCompensationDistanceEffect->currentIndex();
}

int SingleJumpsConfigWindow::getGateFromInput()
{
    return ui->spinBox_gate->value();
}

int SingleJumpsConfigWindow::getDSQProbabilityFromInput()
{
    return ui->spinBox_dsqProbability->value();
}

bool SingleJumpsConfigWindow::getHasJudgesFromInput()
{
    return ui->checkBox_hasJudges->isChecked();
}

bool SingleJumpsConfigWindow::getHasWindCompensationFromInput()
{
    return ui->checkBox_hasWindCompensation->isChecked();
}

double SingleJumpsConfigWindow::importance()
{
    return ui->doubleSpinBox_jumpsImportance->value();
}

HillEditorWidget *SingleJumpsConfigWindow::getHillEditor() const
{
    return hillEditor;
}

JumperEditorWidget *SingleJumpsConfigWindow::getJumperEditor() const
{
    return jumperEditor;
}
