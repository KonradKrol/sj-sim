#include "WindsGeneratorSettingsWidgetInputItem.h"
#include "ui_WindsGeneratorSettingsWidgetInputItem.h"
#include <QTransform>

WindsGeneratorSettingsWidgetInputItem::WindsGeneratorSettingsWidgetInputItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindsGeneratorSettingsWidgetInputItem)
{
    settings = nullptr;
    ui->setupUi(this);

    characteristicsEditor = new CharacteristicsEditor(Characteristic::ParentType::WindsGeneratorSettings);
    ui->verticalLayout_characteristicsEditor->insertWidget(0, characteristicsEditor);

    /*ui->comboBox_baseWindDirection->setItemIcon(0, QIcon("://img/arrow-down.png"));
    ui->comboBox_baseWindDirection->setItemIcon(1, QIcon("://img/back-side.png"));
    ui->comboBox_baseWindDirection->setItemIcon(2, QIcon("://img/horizontal.png"));
    ui->comboBox_baseWindDirection->setItemIcon(3, QIcon("://img/front-side.png"));
    ui->comboBox_baseWindDirection->setItemIcon(4, QIcon("://img/arrow-up1.png"));*/

    emit ui->doubleSpinBox_baseWindDirection->valueChanged(0);

    disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &WindsGeneratorSettingsWidgetInputItem::on_pushButton_submit_clicked);
}

WindsGeneratorSettingsWidgetInputItem::~WindsGeneratorSettingsWidgetInputItem()
{
    delete ui;
}

void WindsGeneratorSettingsWidgetInputItem::removeSubmitButton()
{
    disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &WindsGeneratorSettingsWidgetInputItem::on_pushButton_submit_clicked);
    ui->verticalLayout_characteristicsEditor->removeWidget(ui->pushButton_submit);
    delete ui->pushButton_submit;
}

double WindsGeneratorSettingsWidgetInputItem::getBaseWindStrengthFromInput()
{
    return ui->doubleSpinBox_baseWindStrength->value();
}

double WindsGeneratorSettingsWidgetInputItem::getWindStrengthChangeFromInput()
{
    return ui->doubleSpinBox_windStrengthChange->value();
}

short WindsGeneratorSettingsWidgetInputItem::getBaseWindDirectionFromInput()
{
    return ui->doubleSpinBox_baseWindDirection->value();
}

double WindsGeneratorSettingsWidgetInputItem::getWindDirectionChangeFromInput()
{
    return ui->doubleSpinBox_windDirectionChange->value();
}

WindGenerationSettings WindsGeneratorSettingsWidgetInputItem::getWindGenerationSettingsFromInputs()
{
    WindGenerationSettings settings;
    settings.setBaseWindStrength(getBaseWindStrengthFromInput());
    settings.setWindStrengthInstability(getWindStrengthChangeFromInput());
    settings.setBaseDirection(getBaseWindDirectionFromInput());
    settings.setWindDirectionInstability(getWindDirectionChangeFromInput());
    settings.setCharacteristics(characteristicsEditor->getCharacteristics());

    return settings;
}

void WindsGeneratorSettingsWidgetInputItem::fillInputs()
{
    ui->doubleSpinBox_baseWindStrength->setValue(settings->getBaseWindStrength());
    ui->doubleSpinBox_windStrengthChange->setValue(settings->getWindStrengthInstability());
    ui->doubleSpinBox_baseWindDirection->setValue(settings->getBaseDirection());
    ui->doubleSpinBox_windDirectionChange->setValue(settings->getWindDirectionInstability());
    characteristicsEditor->setCharacteristics(settings->getCharacteristics());
}

void WindsGeneratorSettingsWidgetInputItem::fillInputsToExactWindEditor()
{
    ui->formLayout_main->removeWidget(ui->label_strengthChange);
    delete ui->label_strengthChange;
    ui->formLayout_main->removeWidget(ui->doubleSpinBox_windStrengthChange);
    delete ui->doubleSpinBox_windStrengthChange;
    ui->formLayout_main->removeWidget(ui->label_directionChange);
    delete ui->label_directionChange;
    ui->formLayout_main->removeWidget(ui->doubleSpinBox_windDirectionChange);
    delete ui->doubleSpinBox_windDirectionChange;
    ui->formLayout_main->removeWidget(ui->label_strengthChangeMS);
    delete ui->label_strengthChangeMS;
    ui->verticalLayout_characteristicsEditor->removeWidget(characteristicsEditor);
    delete characteristicsEditor;
    ui->formLayout_main->removeWidget(ui->label_stopnie1);
    delete ui->label_stopnie1;
    ui->formLayout_main->removeWidget(ui->label_stopnie2);
    delete ui->label_stopnie2;


    ui->doubleSpinBox_baseWindStrength->setValue(settings->getBaseWindStrength());
    ui->doubleSpinBox_baseWindDirection->setValue(settings->getBaseDirection());
}

void WindsGeneratorSettingsWidgetInputItem::on_pushButton_submit_clicked()
{
    emit submitted();
}

CharacteristicsEditor *WindsGeneratorSettingsWidgetInputItem::getCharacteristicsEditor() const
{
    return characteristicsEditor;
}

WindGenerationSettings *WindsGeneratorSettingsWidgetInputItem::getSettings() const
{
    return settings;
}

void WindsGeneratorSettingsWidgetInputItem::setSettings(WindGenerationSettings *newSettings)
{
    settings = newSettings;
}


void WindsGeneratorSettingsWidgetInputItem::on_doubleSpinBox_baseWindDirection_valueChanged(double arg1)
{
    QPixmap pixmap("://img/arrow-up1.png");
    QTransform transform;
    transform.rotate(arg1);
    pixmap = pixmap.transformed(transform).scaled(ui->label_windDirectionImg->size());
    ui->label_windDirectionImg->setPixmap(pixmap);
}

