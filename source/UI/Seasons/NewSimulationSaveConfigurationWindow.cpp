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
    ui->lineEdit_name->setMaxLength(80);
    ui->lineEdit_name->setPlaceholderText(tr("Np. Puchar Świata 2026"));
    ui->lineEdit_name->setFocus();
    ui->pushButton_submit->setDefault(true);
    ui->pushButton_submit->setEnabled(false);
    connect(ui->lineEdit_name, &QLineEdit::textChanged, this, [this](const QString &text){
        ui->pushButton_submit->setEnabled(!text.trimmed().isEmpty());
    });
    setWindowFlags(Qt::Window);
}

NewSimulationSaveConfigurationWindow::~NewSimulationSaveConfigurationWindow()
{
    delete ui;
}

QString NewSimulationSaveConfigurationWindow::getNameFromInput() const
{
    return ui->lineEdit_name->text().trimmed();
}

int NewSimulationSaveConfigurationWindow::getSeasonNumberFromInput() const
{
    return ui->spinBox_seasonNumber->value();
}

void NewSimulationSaveConfigurationWindow::on_pushButton_submit_clicked()
{
    const QString name = getNameFromInput();
    if(name.isEmpty())
    {
        QMessageBox::warning(this, tr("Brak nazwy"), tr("Wpisz nazwę zapisu symulacji."), QMessageBox::Ok);
        ui->lineEdit_name->setFocus();
        return;
    }

    for(const QString &otherName : otherNames)
    {
        if(QString::compare(otherName.trimmed(), name, Qt::CaseInsensitive) == 0)
        {
            QMessageBox::warning(this, tr("Nazwa jest zajęta"), tr("Wybierz nazwę, która nie została jeszcze użyta na liście zapisów symulacji."), QMessageBox::Ok);
            ui->lineEdit_name->selectAll();
            ui->lineEdit_name->setFocus();
            return;
        }
    }

    const QString invalidCharacters = QStringLiteral("\\/:*?\"<>|");
    bool containsInvalidCharacter = false;
    for(const QChar character : name)
    {
        if(invalidCharacters.contains(character) || character.unicode() < 32)
        {
            containsInvalidCharacter = true;
            break;
        }
    }
    if(containsInvalidCharacter || name == "." || name == ".." || name.endsWith('.'))
    {
        QMessageBox::warning(this, tr("Nieprawidłowa nazwa"),
                             tr("Nazwa nie może zawierać znaków \\ / : * ? \" < > | ani kończyć się kropką."), QMessageBox::Ok);
        ui->lineEdit_name->selectAll();
        ui->lineEdit_name->setFocus();
        return;
    }

    const QString baseName = name.section('.', 0, 0).toUpper();
    const QStringList reservedNames = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
    };
    if(reservedNames.contains(baseName))
    {
        QMessageBox::warning(this, tr("Nieprawidłowa nazwa"),
                             tr("Ta nazwa jest zarezerwowana przez system. Wybierz inną."), QMessageBox::Ok);
        ui->lineEdit_name->selectAll();
        ui->lineEdit_name->setFocus();
        return;
    }

    if(name != ui->lineEdit_name->text())
        ui->lineEdit_name->setText(name);
    accept();
}
