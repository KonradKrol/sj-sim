#include "InrunSnowGeneratorSettingsEditorWidget.h"
#include "ui_InrunSnowGeneratorSettingsEditorWidget.h"

InrunSnowGeneratorSettingsEditorWidget::InrunSnowGeneratorSettingsEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InrunSnowGeneratorSettingsEditorWidget)
{
    ui->setupUi(this);
}

InrunSnowGeneratorSettingsEditorWidget::~InrunSnowGeneratorSettingsEditorWidget()
{
    delete ui;
}

void InrunSnowGeneratorSettingsEditorWidget::removeSubmitButton()
{
     disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &InrunSnowGeneratorSettingsEditorWidget::on_pushButton_submit_clicked);
     delete ui->pushButton_submit;
}

void InrunSnowGeneratorSettingsEditorWidget::fillInputs()
{
    ui->doubleSpinBox_base->setValue(generator->getBaseInrunSnow());
    ui->doubleSpinBox_deviation->setValue(generator->getInrunSnowDeviation());
}

double InrunSnowGeneratorSettingsEditorWidget::getBase()
{
    return ui->doubleSpinBox_base->value();
}

double InrunSnowGeneratorSettingsEditorWidget::getDeviation()
{
    return ui->doubleSpinBox_deviation->value();
}

void InrunSnowGeneratorSettingsEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

InrunSnowGenerator *InrunSnowGeneratorSettingsEditorWidget::getGenerator() const
{
    return generator;
}

void InrunSnowGeneratorSettingsEditorWidget::setGenerator(InrunSnowGenerator *newGenerator)
{
    generator = newGenerator;
}

