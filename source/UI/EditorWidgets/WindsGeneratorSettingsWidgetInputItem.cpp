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
    auto *input=findChild<QDoubleSpinBox *>("doubleSpinBox_windStrengthChange");return input?input->value():0;
}

short WindsGeneratorSettingsWidgetInputItem::getBaseWindDirectionFromInput()
{
    return ui->doubleSpinBox_baseWindDirection->value();
}

double WindsGeneratorSettingsWidgetInputItem::getWindDirectionChangeFromInput()
{
    auto *input=findChild<QDoubleSpinBox *>("doubleSpinBox_windDirectionChange");return input?input->value():0;
}

WindGenerationSettings WindsGeneratorSettingsWidgetInputItem::getWindGenerationSettingsFromInputs()
{
    WindGenerationSettings settings;
    settings.setBaseWindStrength(getBaseWindStrengthFromInput());
    settings.setWindStrengthInstability(getWindStrengthChangeFromInput());
    settings.setBaseDirection(getBaseWindDirectionFromInput());
    settings.setWindDirectionInstability(getWindDirectionChangeFromInput());
    if(characteristicsEditor)settings.setCharacteristics(characteristicsEditor->getCharacteristics());

    return settings;
}

void WindsGeneratorSettingsWidgetInputItem::fillInputs()
{
    if(settings)applyInputSettings(*settings);
}

void WindsGeneratorSettingsWidgetInputItem::applyInputSettings(const WindGenerationSettings &value)
{
    ui->doubleSpinBox_baseWindStrength->setValue(value.getBaseWindStrength());
    ui->doubleSpinBox_baseWindDirection->setValue(value.getBaseDirection());
    if(auto *input=findChild<QDoubleSpinBox *>("doubleSpinBox_windStrengthChange"))input->setValue(value.getWindStrengthInstability());
    if(auto *input=findChild<QDoubleSpinBox *>("doubleSpinBox_windDirectionChange"))input->setValue(value.getWindDirectionInstability());
    if(characteristicsEditor)characteristicsEditor->setCharacteristics(value.getCharacteristics());
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
    characteristicsEditor=nullptr;
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

