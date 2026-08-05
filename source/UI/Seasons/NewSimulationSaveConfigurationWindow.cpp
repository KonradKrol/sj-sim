#include "NewSimulationSaveConfigurationWindow.h"
#include "ui_NewSimulationSaveConfigurationWindow.h"
#include <QDate>
#include <QMessageBox>

NewSimulationSaveConfigurationWindow::NewSimulationSaveConfigurationWindow(QStringList otherNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSimulationSaveConfigurationWindow),
    otherNames(otherNames)
{
    ui->setupUi(this);
    ui->spinBox_seasonNumber->setValue(QDate::currentDate().year());
    setWindowFlags(Qt::Window);
}

NewSimulationSaveConfigurationWindow::~NewSimulationSaveConfigurationWindow()
{
    delete ui;
}

QString NewSimulationSaveConfigurationWindow::getNameFromInput() const
{
    return ui->lineEdit_name->text();
}

int NewSimulationSaveConfigurationWindow::getSeasonNumberFromInput() const
{
    return ui->spinBox_seasonNumber->value();
}

void NewSimulationSaveConfigurationWindow::on_pushButton_submit_clicked()
{
    if(otherNames.contains(ui->lineEdit_name->text())){
        QMessageBox::warning(this, tr("Nazwa jest zajęta"), tr("Wybierz nazwę, która nie została jeszcze wybrana na liście zapisów symulacji"), "OK");
        return;
    }
    accept();
}

