#include "AppSettingsWindow.h"
#include "ui_AppSettingsWindow.h"

#include <QTranslator>
#include <QCloseEvent>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>

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

extern Uuid globalIDGenerator;

AppSettingsWindow::AppSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettingsWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setMinimumSize(360, 360);
    setSizeGripEnabled(true);

    ui->spinBox_skillsRange->setValue(GlobalSimulationSettings::get()->getMaxSkills());
    ui->spinBox_dsqProbability->setValue(GlobalSimulationSettings::get()->getBaseDsqProbability());
    ui->doubleSpinBox_randomMultiplier->setValue(GlobalSimulationSettings::get()->getSimulationRandomMultiplier());
    ui->checkBox_autoAdjustHillEffects->setChecked(GlobalSimulationSettings::get()->getAutoAdjustHillEffects());
    ui->doubleSpinBox->setValue(GlobalSimulationSettings::get()->getHillsEffectsMultiplier());
    ui->checkBox_globalDatabaseRecords->setChecked(GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords());
    ui->lineEdit_jumpInfoWh->setText(GlobalAppSettings::get()->getJumpInfoWebhook());
    ui->lineEdit_singleResultWh->setText(GlobalAppSettings::get()->getCompetitionSingleResultWebhook());
    ui->lineEdit_compResultsWh->setText(GlobalAppSettings::get()->getCompetitionResultsWebhook());
    ui->lineEdit_clsResultsWh->setText(GlobalAppSettings::get()->getClassificationResultsWebhook());
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
    ui->comboBox_language->addItem(QIcon(CountryFlagsManager::getFlagPixmap("pl")), QObject::tr("Polski"));
    ui->comboBox_language->addItem(QIcon(CountryFlagsManager::getFlagPixmap("gb")), QObject::tr("Angielski"));

    ui->comboBox_language->setCurrentIndex(GlobalAppSettings::get()->getLanguageID());
    connect(ui->comboBox_language, qOverload<int>(&QComboBox::currentIndexChanged), this, &AppSettingsWindow::myConnect_on_comboBox_language_currentIndexChanged);
}

void AppSettingsWindow::on_pushButton_defaultSettings_clicked()
{
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
    switch(index)
    {
    case GlobalAppSettings::Polish:
        dynamic_cast<MainWindow *>(getMainWindowParent())->getParentApplication()->removeTranslator(const_cast<QTranslator *>(GlobalTranslators::get()->getGlobalTranslations().at(0)));
        ui->retranslateUi(this);
        GlobalAppSettings::get()->setLanguageID(GlobalAppSettings::Polish);
        break;
    case GlobalAppSettings::English:
        dynamic_cast<MainWindow *>(getMainWindowParent())->getParentApplication()->installTranslator(const_cast<QTranslator *>(GlobalTranslators::get()->getGlobalTranslations().at(0)));
        ui->retranslateUi(this);
        GlobalAppSettings::get()->setLanguageID(GlobalAppSettings::English);
        break;
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


void AppSettingsWindow::on_lineEdit_jumpInfoWh_editingFinished()
{
    GlobalAppSettings::get()->setJumpInfoWebhook(ui->lineEdit_jumpInfoWh->text());
}

void AppSettingsWindow::on_lineEdit_singleResultWh_editingFinished()
{
    GlobalAppSettings::get()->setCompetitionSingleResultWebhook(ui->lineEdit_singleResultWh->text());
}

void AppSettingsWindow::on_lineEdit_compResultsWh_editingFinished()
{
    GlobalAppSettings::get()->setCompetitionResultsWebhook(ui->lineEdit_compResultsWh->text());
}

void AppSettingsWindow::on_lineEdit_clsResultsWh_editingFinished()
{
    GlobalAppSettings::get()->setClassificationResultsWebhook(ui->lineEdit_clsResultsWh->text());
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
