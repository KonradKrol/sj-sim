#include "NewJumpsImportancePresetDialog.h"
#include "ui_NewJumpsImportancePresetDialog.h"

NewJumpsImportancePresetDialog::NewJumpsImportancePresetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewJumpsImportancePresetDialog)
{
    ui->setupUi(this);
}

NewJumpsImportancePresetDialog::~NewJumpsImportancePresetDialog()
{
    delete ui;
}

QString NewJumpsImportancePresetDialog::name()
{
    return ui->lineEdit->text();
}

double NewJumpsImportancePresetDialog::importance()
{
    return ui->doubleSpinBox->value();
}

void NewJumpsImportancePresetDialog::on_buttonBox_accepted()
{
    accept();
}

