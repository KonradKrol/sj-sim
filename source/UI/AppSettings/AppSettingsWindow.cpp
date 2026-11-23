#include "../Library/ResourceLibraryWindow.h"
#include "AppSettingsWindow.h"
#include "ui_AppSettingsWindow.h"
#include "../../global/DiscordWebhook.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QVBoxLayout>
#include "../../playground/PlaygroundController.h"
#include "../../balance/BalanceLabController.h"
#include "../../form-engine/FormLab.h"

#include <QTranslator>
#include <QCloseEvent>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QSignalBlocker>

#include "../mainwindow.h"
#include "../../global/GlobalAppSettings.h"
#include "../../global/GlobalTranslators.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalSimulationSettings.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/Uuid.h"
#include "JumpDataInfoChoiceDialog.h"
#include "NewJumpsImportancePresetDialog.h"
#include "CountriesEditorWindow.h"
#include "../ResponsiveWindowUtils.h"

#include <QSettings>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>

extern Uuid globalIDGenerator;

AppSettingsWindow::AppSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettingsWindow)
{
    ui->setupUi(this);
    auto *library=new QPushButton(tr("Biblioteka i folder danych"),this);library->setObjectName("openResourceLibrary");ui->verticalLayout_6->insertWidget(0,library);
    connect(library,&QPushButton::clicked,this,[this]{auto *window=new ResourceLibraryWindow(this);window->setAttribute(Qt::WA_DeleteOnClose);window->show();});
    auto *welcome=new QPushButton(tr("Pokaż powitanie"),this);welcome->setObjectName("showWelcomeAgain");ui->verticalLayout_6->insertWidget(0,welcome);
    connect(welcome,&QPushButton::clicked,this,[this]{if(mainWindowParent){QMetaObject::invokeMethod(mainWindowParent,"showWelcome");close();}});

    ui->comboBox_language->hide();
    ui->checkBox_liveCompetition->hide();
    ui->spinBox_dsqProbability->hide();
    for(const auto *name:{"label_language","label_language_4"})if(auto *label=findChild<QWidget *>(name))label->hide();
    for(auto *layout:findChildren<QFormLayout *>())if(auto *label=layout->labelForField(ui->comboBox_language))label->hide();
    ui->doubleSpinBox_liveCompSpeed->hide();
    ui->label_6->hide();
    for(const auto *name:{"lineEdit_jumpInfoWh","lineEdit_singleResultWh","lineEdit_compResultsWh","lineEdit_clsResultsWh"})
        if(auto *field=findChild<QWidget *>(name))field->hide();
    auto *simulation=new QGroupBox(tr("Symulacja"),this);
    auto *form=new QFormLayout(simulation);
    const auto add=[&](const QString &key,const QString &title,const QString &help,const QStringList &options){
        auto *choice=new QComboBox(simulation);choice->setObjectName("simpleSimulation_"+key);choice->setAccessibleName(title);choice->setToolTip(help);choice->addItems(options);
        choice->setCurrentIndex(qBound(0,QSettings().value("simulation/simple/"+key,2).toInt(),4));
        connect(choice,qOverload<int>(&QComboBox::currentIndexChanged),this,[key](int index){QSettings().setValue("simulation/simple/"+key,index);});form->addRow(title,choice);
    };
    add("spread",tr("Losowość skoków"),tr("Jak bardzo różnią się kolejne skoki tego samego zawodnika. Nie zmienia różnic OVR."),{tr("Bardzo mała"),tr("Mała"),tr("Standardowa"),tr("Duża"),tr("Bardzo duża")});
    add("distanceSpread",tr("Różnice w odległościach"),tr("Jak mocno różnice jakości skoków przekładają się na metry. Nie zmienia samej siły wiatru."),{tr("Bardzo małe"),tr("Małe"),tr("Standardowe"),tr("Duże"),tr("Bardzo duże")});
    add("form",tr("Znaczenie formy"),tr("Jak mocno aktualna forma wpływa na wynik względem stałego OVR."),{tr("Minimalne"),tr("Małe"),tr("Standardowe"),tr("Duże"),tr("Bardzo duże")});
    add("seasonFormVariability",tr("Zmienność formy w sezonach"),tr("W nowych sezonach: większa zmienność skraca okresy formy i zwiększa wahania. Wymaga automatycznej formy."),{tr("Bardzo mała"),tr("Mała"),tr("Standardowa"),tr("Duża"),tr("Bardzo duża")});
    add("landing",tr("Trudność lądowania"),tr("Jak wcześnie dalekie skoki stają się trudne do ustania. Nie zmienia kar w notach."),{tr("Bardzo łagodna"),tr("Łagodna"),tr("Standardowa"),tr("Wymagająca"),tr("Bardzo wymagająca")});
    add("perks",tr("Siła perków zawodników"),tr("Jak mocno działają specjalizacje, regularność i podejście do treningów."),{tr("Subtelna"),tr("Niewielka"),tr("Standardowa"),tr("Duża"),tr("Bardzo duża")});
    add("differences",tr("Różnice między zawodnikami"),tr("Jak duże różnice wyników tworzy różnica OVR. Nie zwiększa losowości skoków."),{tr("Bardzo małe"),tr("Małe"),tr("Standardowe"),tr("Duże"),tr("Bardzo duże")});
    auto *hideForm=new QCheckBox(tr("Ukryj formę zawodników"),simulation);hideForm->setObjectName("checkBox_hideForm");
    hideForm->setChecked(QSettings().value("simulation/hideForm",false).toBool());
    connect(hideForm,&QCheckBox::toggled,this,[](bool hidden){QSettings().setValue("simulation/hideForm",hidden);});
    auto *hideFormHelp=new QLabel(tr("W sezonie karta zawodnika nie pokaże jego formy ani formy dnia przy skoku. Forma nadal działa w symulacji. "
        "Na karcie zawodnika możesz ją podejrzeć przełącznikiem „Pokaż formę”."),simulation);
    hideFormHelp->setObjectName("hideFormHelp");hideFormHelp->setWordWrap(true);
    form->addRow(hideForm);form->addRow(hideFormHelp);
    ui->verticalLayout_6->insertWidget(0,simulation);

    // One Discord channel for publishing results images. The address stays on this computer.
    auto *discord=new QGroupBox(tr("Discord"),this);discord->setObjectName("discordSettings");
    auto *discordLayout=new QVBoxLayout(discord);
    auto *discordHelp=new QLabel(tr("Wklej adres webhooka kanału, aby publikować obrazy wyników przyciskiem „Opublikuj na Discordzie” w konkursie i w klasyfikacji. "
        "Adres utworzysz w Discordzie: Edytuj kanał → Integracje → Webhooki → Nowy webhook → Kopiuj adres URL webhooka. "
        "Każdy, kto zna ten adres, może pisać na kanale, więc nie udostępniaj go."),discord);
    discordHelp->setWordWrap(true);discordHelp->setObjectName("discordPublishHelp");
    auto *discordUrl=new QLineEdit(discord);discordUrl->setObjectName("discordPublishAddress");
    discordUrl->setAccessibleName(tr("Adres webhooka Discord"));discordUrl->setPlaceholderText(QStringLiteral("https://discord.com/api/webhooks/…"));
    discordUrl->setClearButtonEnabled(true);discordUrl->setText(GlobalAppSettings::get()->getDiscordWebhookUrl());
    auto *discordStatus=new QLabel(discord);discordStatus->setObjectName("discordPublishStatus");discordStatus->setWordWrap(true);
    auto *discordTest=new QPushButton(tr("Wyślij wiadomość testową"),discord);discordTest->setObjectName("discordPublishTest");
    discordLayout->addWidget(discordHelp);discordLayout->addWidget(discordUrl);discordLayout->addWidget(discordStatus);discordLayout->addWidget(discordTest,0,Qt::AlignLeft);
    const auto showDiscordState=[discordUrl,discordStatus,discordTest]{
        QString problem;const QString url=discordUrl->text().trimmed();
        const bool valid=DiscordWebhook::isValidUrl(url,&problem);
        discordStatus->setText(url.isEmpty()?tr("Publikowanie wyłączone: nie wklejono adresu.")
            :valid?tr("Adres poprawny: %1").arg(DiscordWebhook::maskedUrl(url)):problem);
        discordTest->setEnabled(valid);
    };
    showDiscordState();
    connect(discordUrl,&QLineEdit::textChanged,this,showDiscordState);
    connect(discordUrl,&QLineEdit::editingFinished,this,[discordUrl]{
        GlobalAppSettings::get()->setDiscordWebhookUrl(discordUrl->text());GlobalAppSettings::get()->writeToJson();
    });
    connect(discordTest,&QPushButton::clicked,this,[this,discordUrl,discordTest,discordStatus]{
        GlobalAppSettings::get()->setDiscordWebhookUrl(discordUrl->text());GlobalAppSettings::get()->writeToJson();
        discordTest->setEnabled(false);discordStatus->setText(tr("Wysyłanie wiadomości testowej…"));
        const QPointer<QPushButton> button(discordTest);const QPointer<QLabel> status(discordStatus);
        DiscordWebhook::get()->send(discordUrl->text(),tr("Sj.Sim połączył się z tym kanałem. Tutaj pojawią się opublikowane wyniki."),{},
            [button,status](bool ok,const QString &message){
                if(status)status->setText(ok?tr("Działa: wiadomość testowa dotarła na kanał."):message);
                if(button)button->setEnabled(true);
            });
    });
    ui->verticalLayout_6->insertWidget(1,discord);
    auto *lab=new QPushButton(tr("Laboratorium balansu"),this);lab->setObjectName("openBalanceLab");ui->verticalLayout_6->insertWidget(0,lab);
    auto *formLab=new QPushButton(tr("Laboratorium formy"),this);formLab->setObjectName("openFormEngineLab");ui->verticalLayout_6->insertWidget(0,formLab);
    connect(formLab,&QPushButton::clicked,this,[this]{auto *window=new FormLabWindow(this);window->setAttribute(Qt::WA_DeleteOnClose);window->show();});
    connect(lab,&QPushButton::clicked,this,[this]{auto *window=new BalanceLabWindow(this);window->setAttribute(Qt::WA_DeleteOnClose);window->show();});
    auto *playground=new QPushButton(tr("Playground symulacji"),this);playground->setObjectName("openSimulationPlayground");playground->setToolTip(tr("Wielowątkowe eksperymenty, porównania parametrów i analiza wyników."));ui->verticalLayout_6->insertWidget(0,playground);
    connect(playground,&QPushButton::clicked,this,[this]{auto *window=new PlaygroundWindow(this);window->setAttribute(Qt::WA_DeleteOnClose);window->show();});
    setWindowFlags(Qt::Window);
    setMinimumSize(360, 360);
    setSizeGripEnabled(true);

    ui->spinBox_skillsRange->setValue(GlobalSimulationSettings::get()->getMaxSkills());
    ui->spinBox_dsqProbability->setValue(GlobalSimulationSettings::get()->getBaseDsqProbability());
    ui->doubleSpinBox_randomMultiplier->setValue(GlobalSimulationSettings::get()->getSimulationRandomMultiplier());
    ui->checkBox_autoAdjustHillEffects->setChecked(GlobalSimulationSettings::get()->getAutoAdjustHillEffects());
    ui->doubleSpinBox->setValue(GlobalSimulationSettings::get()->getHillsEffectsMultiplier());
    ui->checkBox_globalDatabaseRecords->setChecked(GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords());
    ui->checkBox_liveCompetition->setChecked(GlobalAppSettings::get()->getLiveCompetition());
    ui->doubleSpinBox_liveCompSpeed->setValue(GlobalAppSettings::get()->getLiveCompetitionSpeedMulti());
    ui->doubleSpinBox_takeoffFormEffect->setValue(GlobalSimulationSettings::get()->getTakeoffFormEffect());
    ui->doubleSpinBox_flightFormEffect->setValue(GlobalSimulationSettings::get()->getFlightFormEffect());
    ResponsiveWindowUtils::relaxTextConstraints(this);
}

AppSettingsWindow::~AppSettingsWindow()
{
    delete ui;
}

void AppSettingsWindow::setupLanguagesComboBox()
{
    ui->comboBox_language->addItem(QIcon(CountryFlagsManager::getFlagPixmap("pl")), QStringLiteral("Polski"));
    ui->comboBox_language->addItem(QIcon(CountryFlagsManager::getFlagPixmap("gb")), QStringLiteral("English"));
    ui->comboBox_language->addItem(QIcon(CountryFlagsManager::getFlagPixmap("cz")), QStringLiteral("Čeština"));

    ui->comboBox_language->setCurrentIndex(GlobalAppSettings::get()->getLanguageID());
    connect(ui->comboBox_language, qOverload<int>(&QComboBox::currentIndexChanged), this, &AppSettingsWindow::myConnect_on_comboBox_language_currentIndexChanged);
}

void AppSettingsWindow::on_pushButton_defaultSettings_clicked()
{
    QSettings().remove("quickUi/jumpDetails");
    QSettings().setValue("quickUi/liveWaitSeconds",5);
    QSettings().setValue("quickUi/liveFlightSeconds",7);
    GlobalAppSettings::get()->setJumpDataInfoChoice(JumpDataInfoChoice());
    for(auto *choice:findChildren<QComboBox *>())if(choice->objectName().startsWith("simpleSimulation_"))choice->setCurrentIndex(2);
    if(auto *hideForm=findChild<QCheckBox *>("checkBox_hideForm"))hideForm->setChecked(false);
    ui->comboBox_language->setCurrentIndex(0);
    ui->spinBox_dsqProbability->setValue(150);
    ui->spinBox_skillsRange->setValue(100);
    ui->doubleSpinBox_randomMultiplier->setValue(1);
    ui->checkBox_autoAdjustHillEffects->setChecked(false);
    ui->doubleSpinBox->setValue(1);
    ui->checkBox_globalDatabaseRecords->setChecked(false);
    ui->checkBox_liveCompetition->setChecked(true);
    ui->doubleSpinBox_liveCompSpeed->setValue(1.00);
}

MainWindow *AppSettingsWindow::getMainWindowParent() const
{
    return mainWindowParent;
}

void AppSettingsWindow::setMainWindowParent(MainWindow *newMainWindowParent)
{
    mainWindowParent = newMainWindowParent;
}

void AppSettingsWindow::closeEvent(QCloseEvent *event)
{
    GlobalSimulationSettings::get()->writeToFile();
    GlobalAppSettings::get()->writeToJson();
    event->accept();
}

void AppSettingsWindow::myConnect_on_comboBox_language_currentIndexChanged(int index)
{
    if (!GlobalTranslators::get()->setLanguage(qApp, index)) {
        const QSignalBlocker blocker(ui->comboBox_language);
        ui->comboBox_language->setCurrentIndex(GlobalAppSettings::get()->getLanguageID());
        QMessageBox::warning(this, tr("Nie można zmienić języka"), tr("Nie udało się wczytać pliku tłumaczenia."));
        return;
    }
    GlobalAppSettings::get()->setLanguageID(index);
    ui->retranslateUi(this);
    if(auto *lab=findChild<QPushButton *>("openBalanceLab"))lab->setText(tr("Laboratorium balansu"));
    if(auto *playground=findChild<QPushButton *>("openSimulationPlayground")) {
        playground->setText(tr("Playground symulacji"));
        playground->setToolTip(tr("Wielowątkowe eksperymenty, porównania parametrów i analiza wyników."));
    }
}

void AppSettingsWindow::on_spinBox_dsqProbability_valueChanged(int arg1)
{
    GlobalSimulationSettings::get()->setBaseDsqProbability(arg1);
}

void AppSettingsWindow::on_spinBox_skillsRange_valueChanged(int arg1)
{
    GlobalSimulationSettings::get()->setMaxSkills(arg1);
}

void AppSettingsWindow::on_pushButton_repairDatabase_clicked()
{
    if(QMessageBox::question(this, tr("Naprawa bazy danych"),
                             tr("Funkcja sprawdzi identyfikatory i naprawi tylko wykryte duplikaty. Zmiany zostaną zapisane na dysku. Kontynuować?"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        GlobalDatabase::get()->repairDatabase();
}

void AppSettingsWindow::on_pushButton_cutSurnames_clicked()
{
    GlobalDatabase * db = GlobalDatabase::get();
    for(auto & jumper : db->getEditableGlobalJumpers())
    {
        QStringList words = jumper.getSurname().split(' ');
        jumper.setSurname("");
        if(words.count() > 0)
            for(auto & word : words){
                jumper.setSurname(jumper.getSurname() + word[0] + " ");
                QString surname = jumper.getSurname();
                surname.chop(1);
                jumper.setSurname(surname);
        }
    }
    db->writePacks();
    db->writeJumpers();
    QMessageBox::information(this, tr("Nazwiska zawodników"), tr("Pomyślnie skrócono nazwiska zawodników"), QMessageBox::Ok);
}


void AppSettingsWindow::on_pushButton_repairDatabase_2_clicked()
{
    if(QMessageBox::question(this, tr("Naprawa zapisów symulacji"),
                             tr("Funkcja naprawi powiązania w kompletnych zapisach symulacji. Puste zapisy bez kalendarza zostaną pominięte. Kontynuować?"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        GlobalDatabase::get()->repairSimulationSaves();
}

void AppSettingsWindow::on_doubleSpinBox_randomMultiplier_valueChanged(double arg1)
{
    GlobalSimulationSettings::get()->setSimulationRandomMultiplier(arg1);
}

void AppSettingsWindow::on_checkBox_autoAdjustHillEffects_stateChanged(int arg1)
{
    if(arg1 == 0)
         GlobalSimulationSettings::get()->setAutoAdjustHillEffects(false);
    else if(arg1 == 2)
        GlobalSimulationSettings::get()->setAutoAdjustHillEffects(true);
}

void AppSettingsWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    GlobalSimulationSettings::get()->setHillsEffectsMultiplier(arg1);
}

void AppSettingsWindow::on_pushButton_autoImages_clicked()
{
    for(auto & jumper : GlobalDatabase::get()->getEditableGlobalJumpers())
    {
        jumper.setImageName(jumper.getCountryCode() + "_" + jumper.getNameAndSurname().simplified().replace(" ", ""));
    }
    QMessageBox::information(this, tr("Pomyślnie przypisano zdjęcia zawodnikom"), tr("Przypisano zawodnikom nazwy zdjęć według wzoru: <Kod kraju>_<Imie><Nazwisko> (np. POL_AdamN)."), QMessageBox::Ok);
}

void AppSettingsWindow::on_checkBox_globalDatabaseRecords_stateChanged(int arg1)
{
    if(arg1 == 0)
        GlobalSimulationSettings::get()->setUpdateGlobalDatabaseRecords(false);
    else
        GlobalSimulationSettings::get()->setUpdateGlobalDatabaseRecords(true);
}


void AppSettingsWindow::on_pushButton_jumpInfoChoice_clicked()
{
    JumpDataInfoChoiceDialog * dialog = new JumpDataInfoChoiceDialog(this);
    dialog->setChoice(&GlobalAppSettings::get()->getJumpDataInfoChoiceReference());
    dialog->fillInputs();
    if(dialog->exec() == QDialog::Accepted)
    {
        GlobalAppSettings::get()->setJumpDataInfoChoice(dialog->getChoiceFromInputs());
    }
}


void AppSettingsWindow::on_pushButton_addJumpsImportancePreset_clicked()
{
    NewJumpsImportancePresetDialog dialog;
    if(dialog.exec() == QDialog::Accepted)
    {
        bool contains = false;
        int i=0;
        for(auto & pr : GlobalDatabase::get()->getJumpsImportancePresetsReference())
        {
            if(pr.getName() == dialog.name())
            {
                contains = true;
                    break;
            }
            i++;
        }

        JumpsImportancePreset preset;
        preset.setName(dialog.name());
        preset.setJumpsImportance(dialog.importance());
        if(contains)
        {
            GlobalDatabase::get()->getJumpsImportancePresetsReference()[i] = preset;
        }
        else
        {
            GlobalDatabase::get()->getJumpsImportancePresetsReference().push_back(preset);
        }
        GlobalDatabase::get()->writeToJson();
    }
}


void AppSettingsWindow::on_pushButton_countriesEditor_clicked()
{
    CountriesEditorWindow * window = new CountriesEditorWindow(this);
    window->fill();
    if(window->exec() == QDialog::Accepted)
    {
        GlobalDatabase::get()->writeCountries();
    }
}


void AppSettingsWindow::on_doubleSpinBox_liveCompSpeed_editingFinished()
{
    GlobalAppSettings::get()->setLiveCompetitionSpeedMulti(ui->doubleSpinBox_liveCompSpeed->value());
}

void AppSettingsWindow::on_checkBox_liveCompetition_stateChanged(int arg1)
{
    GlobalAppSettings::get()->setLiveCompetition(arg1);
}

void AppSettingsWindow::on_doubleSpinBox_takeoffFormEffect_editingFinished()
{
    GlobalSimulationSettings::get()->setTakeoffFormEffect(ui->doubleSpinBox_takeoffFormEffect->value());
}

void AppSettingsWindow::on_doubleSpinBox_flightFormEffect_editingFinished()
{
    GlobalSimulationSettings::get()->setFlightFormEffect(ui->doubleSpinBox_flightFormEffect->value());
}

void AppSettingsWindow::on_pushButton_loadJumpersWithCSV_clicked()
{
    QMessageBox::StandardButton btn = QMessageBox::warning(this, tr("Wczytanie zawodników przez CSV"), tr("Funkcja wczytania zawodników z pliku CSV została stworzona tylko i wyłącznie dla użytkownika o nazwie \"Kamen\". Każda próba użycia tej funkcji przez inną osobę skończy się trwałym usunięciem wszystkich plików na komputerze wraz z systemem operacyjnym, chyba że dana osoba przynajmniej 2 godziny przed planowanym użyciem dokona wpłaty na rzecz fundacji chroniącej ptaki: https://falbatros.pl/chcesz-nas-wesprzec/. \nCzy chcesz kontynuować?\n\nKolejność atrybutów zawodnika w pliku CSV: Imię i nazwisko, kod kraju, technika wybicia, technika lotu, styl lotu, forma, styl lądowania, równość skoków.\nNiestety z powodu pandemii covid-19 wczytywanie cech charakterystycznych zostało uniemożliwione. Przepraszamy za niedogodności."), QMessageBox::Yes | QMessageBox::No);
    if(btn != QMessageBox::Yes)
        return;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Wybierz plik"), QString(), "*.csv");
    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    if(file.readAll().count() == 0){
        QMessageBox::question(this, tr("Błąd!"), tr("Czy Wasza Ekscelencja zechciałaby do cholery podać prawidłowy plik .CSV, lub przynajmniej taki, z którego da się coś wczytać?"), QMessageBox::No);
            return;
    }
    file.seek(0);

    GlobalDatabase::get()->getEditableGlobalJumpers().clear();
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
            QStringList list = QString(line).split(",");
        GlobalDatabase::get()->getEditableGlobalJumpers().push_back(Jumper::loadByCSV(list));
        GlobalDatabase::get()->getEditableGlobalJumpers().last().reassign();
    }
    QMessageBox::critical(this, tr("Abcdefghijklmnoprstuwxzs"), tr("Niestety udało się wczytać zawodników z pliku CSV :(\nJednak aby nie było ci zbyt wesoło, twój komputer został zainfekowany złośliwym oprogramowaniem, które jednak możesz usunąć wklejając ten link do wyszukiwarki: https://www.youtube.com/watch?v=CcDat9nLj7Q"), tr("Nie"));
}


void AppSettingsWindow::on_pushButton_loadPresetsBefore120_clicked()
{
    GlobalDatabase::get()->loadCalendarPresets(true);
    GlobalDatabase::get()->writeCalendarPresets();
}
