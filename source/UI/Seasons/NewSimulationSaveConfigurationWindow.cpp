#include "NewSimulationSaveConfigurationWindow.h"
#include "ui_NewSimulationSaveConfigurationWindow.h"
#include <QDate>
#include <QCheckBox>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include "../../global/GlobalDatabase.h"

NewSimulationSaveConfigurationWindow::NewSimulationSaveConfigurationWindow(QStringList otherNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSimulationSaveConfigurationWindow),
    otherNames(otherNames)
{
    ui->setupUi(this);
    auto *dates=new QCheckBox(tr("Używaj dat zawodów"),this);dates->setObjectName("newSeasonDates");
    dates->setToolTip(tr("Opcjonalny kalendarz z datami. Możesz włączyć go później, przed rozpoczęciem sezonu."));ui->formLayout->addRow(dates);
    auto *database=GlobalDatabase::get();database->ensurePacks();database->syncActivePacks();
    const QStringList labels{tr("Paczka skoczków"),tr("Paczka skoczni"),tr("Paczka zasad")};
    QSettings preferences;
    auto *remember=new QCheckBox(tr("Zapamiętaj wybór paczek"),this);remember->setObjectName("rememberSimulationPacks");
    remember->setChecked(preferences.value("newSimulation/rememberPacks",true).toBool());
    for(int kind=0;kind<3;++kind) {
        auto *choice=new QComboBox(this);packChoices[kind]=choice;choice->setObjectName(QString("simulationPack%1").arg(kind));
        for(const auto &pack:database->getPacks())if(int(pack.kind)==kind)choice->addItem(pack.name,pack.id);
        const auto preferred=remember->isChecked()?preferences.value(QString("newSimulation/pack%1").arg(kind),database->activePackId(DataPack::Kind(kind))).toString():database->activePackId(DataPack::Kind(kind));
        const int index=choice->findData(preferred);choice->setCurrentIndex(index>=0?index:choice->count()?0:-1);
        ui->formLayout->addRow(labels[kind],choice);
        auto *count=new QLabel(this);count->setObjectName(QString("simulationPackCount%1").arg(kind));ui->formLayout->addRow(QString(),count);
        const auto refreshCount=[this,kind,count] {const auto pack=selectedPack(DataPack::Kind(kind));const int size=kind==0?pack.jumpers.size():kind==1?pack.hills.size():pack.rules.size();count->setText(kind==0?tr("Skoczkowie: %1").arg(size):kind==1?tr("Skocznie: %1").arg(size):tr("Zestawy zasad: %1").arg(size));updateSubmitState();};
        connect(choice,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[refreshCount](int){refreshCount();});refreshCount();
    }
    ui->formLayout->addRow(remember);
    ui->pushButton_submit->setText(tr("Utwórz symulację"));
    ui->spinBox_seasonNumber->setValue(QDate::currentDate().year());
    ui->lineEdit_name->setMaxLength(80);
    ui->lineEdit_name->setPlaceholderText(tr("Np. Puchar Świata 2026"));
    ui->lineEdit_name->setFocus();
    ui->pushButton_submit->setDefault(true);
    ui->pushButton_submit->setEnabled(false);
    connect(ui->lineEdit_name, &QLineEdit::textChanged, this, [this](const QString &text){
        Q_UNUSED(text)
        updateSubmitState();
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
    for(int kind=0;kind<3;++kind) {
        const auto pack=selectedPack(DataPack::Kind(kind));
        if((kind==0 && pack.jumpers.isEmpty()) || (kind==1 && pack.hills.isEmpty()) || (kind==2 && pack.rules.isEmpty())) {
            QMessageBox::warning(this,tr("Pusta paczka"),tr("Wybierz paczki zawierające skoczków, skocznie i zasady konkursów."));return;
        }
    }
    QSettings preferences;const bool remember=findChild<QCheckBox *>("rememberSimulationPacks")->isChecked();
    preferences.setValue("newSimulation/rememberPacks",remember);
    for(int kind=0;kind<3;++kind) {
        const auto key=QString("newSimulation/pack%1").arg(kind);
        if(remember)preferences.setValue(key,packChoices[kind]->currentData());else preferences.remove(key);
    }
    accept();
}

bool NewSimulationSaveConfigurationWindow::getDatesEnabledFromInput() const { return findChild<QCheckBox *>("newSeasonDates")->isChecked(); }

DataPack NewSimulationSaveConfigurationWindow::selectedPack(DataPack::Kind kind) const {
    const auto id=packChoices[int(kind)]->currentData().toString();
    for(const auto &pack:GlobalDatabase::get()->getPacks())if(pack.kind==kind && pack.id==id)return pack;
    DataPack empty;empty.kind=kind;return empty;
}

void NewSimulationSaveConfigurationWindow::updateSubmitState() {
    bool ready=!getNameFromInput().isEmpty();
    for(int kind=0;kind<3;++kind) {
        if(!packChoices[kind]){ready=false;continue;}
        const auto pack=selectedPack(DataPack::Kind(kind));
        if((kind==0 && pack.jumpers.isEmpty()) || (kind==1 && pack.hills.isEmpty()) || (kind==2 && pack.rules.isEmpty()))ready=false;
    }
    ui->pushButton_submit->setEnabled(ready);
}
