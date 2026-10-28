#include "NewSimulationSaveConfigurationWindow.h"
#include "ui_NewSimulationSaveConfigurationWindow.h"
#include <QDate>
#include <QCheckBox>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include "../../global/GlobalDatabase.h"
#include "../../global/GlobalSimulationSettings.h"
#include "../../seasons/SimulationSave.h"
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QTimer>

NewSimulationSaveConfigurationWindow::NewSimulationSaveConfigurationWindow(QStringList otherNames, QWidget *parent,
    DataSource source,const SimulationV2::CatalogPaths &paths) :
    QDialog(parent),
    ui(new Ui::NewSimulationSaveConfigurationWindow),
    otherNames(otherNames),source(source),catalog(paths)
{
    ui->setupUi(this);
    auto *dates=new QCheckBox(tr("Używaj dat zawodów"),this);dates->setObjectName("newSeasonDates");
    dates->setToolTip(tr("Opcjonalny kalendarz z datami. Możesz włączyć go później, przed rozpoczęciem sezonu."));ui->formLayout->addRow(dates);
    catalogStatus=new QLabel(tr("Wczytywanie paczek…"),this);catalogStatus->setObjectName("simulationCatalogStatus");
    catalogStatus->setWordWrap(true);ui->formLayout->addRow(catalogStatus);
    auto *details=new QPushButton(tr("Szczegóły wczytywania"),this);details->setObjectName("simulationCatalogDetails");details->hide();ui->formLayout->addRow(details);
    connect(details,&QPushButton::clicked,this,[this]{QMessageBox box(QMessageBox::Information,tr("Wczytywanie paczek"),tr("Uwagi: %1").arg(snapshot.issues.size()),QMessageBox::Ok,this);box.setDetailedText(snapshot.issues.join('\n'));box.exec();});
    const QStringList labels{tr("Paczka skoczków"),tr("Paczka skoczni"),tr("Paczka zasad")};
    QSettings preferences;
    auto *remember=new QCheckBox(tr("Zapamiętaj wybór paczek"),this);remember->setObjectName("rememberSimulationPacks");
    remember->setChecked(preferences.value("newSimulation/rememberPacks",true).toBool());
    for(int kind=0;kind<3;++kind) {
        auto *choice=new QComboBox(this);packChoices[kind]=choice;choice->setObjectName(QString("simulationPack%1").arg(kind));
        choice->setEnabled(false);
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
    if(source==DataSource::LegacyCompatibility)loadCatalog();
    else QTimer::singleShot(0,this,&NewSimulationSaveConfigurationWindow::loadCatalog);
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
    if(!ready)return;
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
    if(int(kind)<0 || int(kind)>2 || !packChoices[int(kind)])return {};
    const auto id=packChoices[int(kind)]->currentData().toString();
    if(const auto *pack=snapshot.find(kind,id))return pack->data;
    DataPack empty;empty.kind=kind;return empty;
}

std::unique_ptr<SimulationSave> NewSimulationSaveConfigurationWindow::createSave() const
{
    if(!ready || getNameFromInput().isEmpty())return {};
    const auto athletes=selectedPack(DataPack::Jumpers),hills=selectedPack(DataPack::Hills),rules=selectedPack(DataPack::Rules);
    if(athletes.jumpers.isEmpty() || hills.hills.isEmpty() || rules.rules.isEmpty())return {};
    auto save=std::make_unique<SimulationSave>();save->setEngineVersion(engineVersion());save->setName(getNameFromInput());
    for(const auto &jumper:athletes.jumpers) {
        auto actor=std::make_unique<Jumper>(jumper);save->getJumpersReference().append(actor.get());actor.release();
    }
    for(const auto &hill:hills.hills) {
        auto venue=std::make_unique<Hill>(hill);save->getHillsReference().append(venue.get());venue.release();
    }
    save->setCompetitionRules(rules.rules);
    Season season;season.setSeasonNumber(getSeasonNumberFromInput());season.setDatesEnabled(getDatesEnabledFromInput());season.setActualCalendar(nullptr);
    save->getSeasonsReference().append(season);save->setActualSeason(&save->getSeasonsReference().last());
    save->fixJumpersFormInstabilities();save->updateNextCompetitionIndex();save->repairDatabase();
    return save;
}

void NewSimulationSaveConfigurationWindow::updateSubmitState() {
    bool canSubmit=ready && !getNameFromInput().isEmpty();
    for(int kind=0;kind<3;++kind) {
        if(!packChoices[kind]){canSubmit=false;continue;}
        const auto pack=selectedPack(DataPack::Kind(kind));
        if((kind==0 && pack.jumpers.isEmpty()) || (kind==1 && pack.hills.isEmpty()) || (kind==2 && pack.rules.isEmpty()))canSubmit=false;
    }
    ui->pushButton_submit->setEnabled(canSubmit);
    if(ready)updateCatalogStatus();
}

void NewSimulationSaveConfigurationWindow::loadCatalog()
{
    const auto *database=GlobalDatabase::get();const auto packs=database->getPacks();
    const QStringList active{database->activePackId(DataPack::Jumpers),database->activePackId(DataPack::Hills),database->activePackId(DataPack::Rules)};
    if(source==DataSource::LegacyCompatibility) {
        for(const auto &pack:packs)snapshot.packs.append({pack,{}, {}});
        snapshot.active=active;ready=true;fillPackChoices();return;
    }
    const auto service=catalog;const double maximum=GlobalSimulationSettings::get()->getMaxSkills();
    auto *watcher=new QFutureWatcher<SimulationV2::CatalogSnapshot>(this);
    connect(watcher,&QFutureWatcher<SimulationV2::CatalogSnapshot>::finished,this,[this,watcher]{
        snapshot=watcher->result();watcher->deleteLater();ready=true;fillPackChoices();
    });
    watcher->setFuture(QtConcurrent::run([=]{return service.load(packs,maximum,active);}));
}

void NewSimulationSaveConfigurationWindow::fillPackChoices()
{
    QSettings preferences;const bool remember=findChild<QCheckBox *>("rememberSimulationPacks")->isChecked();
    for(int kind=0;kind<3;++kind) {
        auto *choice=packChoices[kind];
        for(const auto &pack:snapshot.packs)if(int(pack.data.kind)==kind)choice->addItem(pack.data.name,pack.data.id);
        const QString preferred=remember?preferences.value(QString("newSimulation/pack%1").arg(kind),snapshot.active.value(kind)).toString():snapshot.active.value(kind);
        int index=choice->findData(preferred);
        if(index<0)for(const auto &pack:snapshot.packs)
            if(int(pack.data.kind)==kind && pack.data.approximateMigrationSource==preferred){index=choice->findData(pack.data.id);break;}
        if(index<0)index=choice->findData(snapshot.active.value(kind));
        choice->setCurrentIndex(index);choice->setEnabled(true);
    }
    updateSubmitState();
}

void NewSimulationSaveConfigurationWindow::updateCatalogStatus()
{
    QStringList messages;
    for(int kind=0;kind<3;++kind)if(packChoices[kind]) {
        const auto *pack=snapshot.find(DataPack::Kind(kind),packChoices[kind]->currentData().toString());
        if(pack && !pack->migrationNotice.isEmpty())messages.append(pack->migrationNotice);
    }
    messages.removeDuplicates();
    if(!snapshot.issues.isEmpty())messages.append(tr("Uwagi przy wczytywaniu paczek: %1.").arg(snapshot.issues.size()));
    catalogStatus->setText(messages.join('\n'));catalogStatus->setVisible(!messages.isEmpty());
    findChild<QPushButton *>("simulationCatalogDetails")->setVisible(!snapshot.issues.isEmpty());
}
