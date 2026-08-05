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
    if(ui->spinBox_jumpsCount->value() > 0)
        accept();
    else QMessageBox::warning(this, tr("Ostrzeżenie"), tr("Ilość skoków musi być większa niż 0"), QMessageBox::Ok);
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
