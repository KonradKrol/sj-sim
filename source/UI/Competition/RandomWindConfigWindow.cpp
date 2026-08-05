#include "RandomWindConfigWindow.h"
#include "ui_RandomWindConfigWindow.h"

RandomWindConfigWindow::RandomWindConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomWindConfigWindow)
{
    ui->setupUi(this);
}

RandomWindConfigWindow::~RandomWindConfigWindow()
{
    delete ui;
}

double RandomWindConfigWindow::getDirection()
{
    return ui->doubleSpinBox_direction->value();
}

double RandomWindConfigWindow::getLotability()
{
    return ui->doubleSpinBox_loterity->value();
}

void RandomWindConfigWindow::on_pushButton_submit_clicked()
{
    accept();
}

