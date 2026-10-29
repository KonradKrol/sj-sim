#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../simulator/Hill.h"
#include "../simulator/wind-generation/WindGenerationSettings.h"

#include "../global/Uuid.h"
#include "../global/GlobalDatabase.h"
#include "../global/GlobalAppSettings.h"
#include "../global/GlobalSimulationSettings.h"
#include "../global/IdentifiableObjectsStorage.h"

#include "../single-jumps/SingleJumpsManager.h"

#include "AppSettings/AppSettingsWindow.h"
#include "DatabaseEditor/DatabaseEditorWindow.h"
#include "SingleJumps/SingleJumpsConfigWindow.h"
#include "SingleJumps/SingleJumpsResultsWindow.h"
#include "Competition/CompetitionConfigWindow.h"
#ifdef SJ_QUICK_UI
#include "Quick/QuickUi.h"
#endif
#ifdef SJ_COMPETITION_QUICK
#include "Competition/Quick/CompetitionQuickBridge.h"
#endif
#include "EditorWidgets/JumperEditorWidget.h"
#include "EditorWidgets/HillEditorWidget.h"
#include "EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "Seasons/SimulationSavesWindow.h"
#include "../seasons/SimulationSaveListOrder.h"
#include "Seasons/NewSimulationSaveConfigurationWindow.h"
#include "Seasons/SimulationSaveManagerWindow.h"
#include <QTimer>
#include <QDateTime>
#include <QLineEdit>
#include <QSettings>
#include <QSysInfo>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <exception>

#include "HelpWindow.h"
#include "ThanksWindow.h"
#include "AuthorWindow.h"
#include <QPushButton>

#include <QDebug>
#include <QCloseEvent>
#include <QVector>
#include <QDesktopServices>
#include <random>

#include <dpp/dpp.h>

extern Uuid globalIDGenerator;
const QString appVersion = "2.0.0-beta";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    dpp::cluster bot(""); // normally, you put your bot token in here. But to just run a webhook its not required

    //dpp::webhook wh("https://discord.com/api/webhooks/1164965874017509406/X4HjOnhaRY3uE7il7wdeH3ubgDAbcTOGoS380Lge0q-bzZk2lOwcpeGxRx0EIf7Of91q");

    //bot.execute_webhook(wh, dpp::message("Wiadomość wysyłana z poziomu Sj.Sim-a :smile:"));


    ui->setupUi(this);
    auto *author = new QPushButton(tr("O autorze"),this);
    author->setObjectName("pushButton_author");
    ui->verticalLayout_9->insertWidget(ui->verticalLayout_9->count()-1,author);
    connect(author,&QPushButton::clicked,this,&MainWindow::showAuthor);
    parentApplication = nullptr;
    ui->label_version->setText(appVersion);
    setFixedSize(size());

    GlobalDatabase::get()->loadFromJson();
    GlobalAppSettings::get()->loadFromJson();
    GlobalSimulationSettings::get()->loadFromFile();
}

MainWindow::~MainWindow()
{
    // Unload Quick scenes while the main window's child tree is still intact.
    // QWidget's later child teardown is too late for QuickUi's scene cleanup.
#ifdef SJ_QUICK_UI
    delete findChild<QuickUi *>(QString(), Qt::FindDirectChildrenOnly);
#endif
    delete ui;
}

void MainWindow::on_pushButton_singleJumps_clicked()
{
    SingleJumpsConfigWindow singleJumpsConfig(this);
    singleJumpsConfig.setModal(true);
    if(singleJumpsConfig.exec() == QDialog::Accepted)
    {
        SingleJumpsManager manager;
        manager.setJumpsCount(singleJumpsConfig.getJumpsCountFromInput());
        manager.setChangeableWind(singleJumpsConfig.getChangeableWindFromInput());
        manager.setGate(singleJumpsConfig.getGateFromInput());
        manager.setJumper(singleJumpsConfig.configuredJumper());
        manager.setHill(singleJumpsConfig.configuredHill());
        manager.setWindsGeneratorSettings(singleJumpsConfig.getWindsGeneratorSettingsEditor()->getWindsGenerationSettingsFromInputs());
        manager.setImportance(singleJumpsConfig.importance());
        manager.setSimulationEvent(singleJumpsConfig.simulationEvent());
        manager.setSimulationWeather(singleJumpsConfig.getWeatherSetup()->field());
        if(singleJumpsConfig.getDSQProbabilityFromInput() > (-1)){
            manager.getRules().setHasDsq(true);
            manager.setDSQProbability(singleJumpsConfig.getDSQProbabilityFromInput());
        }
        else manager.getRules().setHasDsq(false);

        manager.getRules().setHasJudgesPoints(singleJumpsConfig.getHasJudgesFromInput());
        manager.getRules().setHasWindCompensations(singleJumpsConfig.getHasWindCompensationFromInput());
        manager.getRules().setHasGateCompensations(false);
        manager.getRules().setWindAverageCalculatingType(singleJumpsConfig.getWindAverageCalculatingType());
        manager.getRules().setWindMeasurementType(singleJumpsConfig.getWindMeasurementType());
        manager.getRules().setWindCompensationDistanceEffect(singleJumpsConfig.getWindCompensationDistanceEffect());
        manager.simulate();

        SingleJumpsResultsWindow resultsWindow(&manager);
        resultsWindow.fillWindow();
        resultsWindow.installShortcuts();
        if(resultsWindow.exec() == QDialog::Rejected)
        {
            QDate date = QDate::currentDate();
            QTime time = QTime::currentTime();
            manager.saveResultsCsv(QString("%1-%2-%3 %4 (%5) (%6-%7)").arg(QString::number(date.day())).arg(QString::number(date.month())).arg(QString::number(date.year())).arg(manager.getJumper().getNameAndSurname()).arg(manager.getHill().getName() + " HS" + QString::number(manager.getHill().getHSPoint())).arg(QString::number(time.hour())).arg(QString::number(time.minute())) + ".csv");
        }
    }
}

void MainWindow::on_pushButton_singleCompetition_clicked()
{
#ifdef SJ_COMPETITION_QUICK
    if (!QApplication::arguments().contains("--widgets-competition")) {
        CompetitionQuickWindow window(this);
        window.exec();
        return;
    }
#endif
    CompetitionConfigWindow configWindow(CompetitionConfigWindow::Type::SingleCompetition, this);
    configWindow.setModal(true);
    if(configWindow.exec() == QDialog::Accepted)
    {

    }
}

void MainWindow::on_pushButton_databaseEdit_clicked()
{
    DatabaseEditorWindow databaseEditor;
    databaseEditor.setModal(true);
    if(databaseEditor.exec() == QDialog::Accepted)
    {
    }
}


void MainWindow::on_pushButton_settings_clicked()
{
    AppSettingsWindow settingsWindow;
    settingsWindow.setMainWindowParent(this);
    settingsWindow.setModal(true);
    settingsWindow.setupLanguagesComboBox();
    if(settingsWindow.exec() == QDialog::Accepted)
    {
        ui->retranslateUi(this);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);
}

QApplication *MainWindow::getParentApplication() const
{
    return parentApplication;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    GlobalAppSettings::get()->writeToJson();
}

void MainWindow::setParentApplication(QApplication *newParentApplication)
{
    parentApplication = newParentApplication;
}

void MainWindow::on_pushButton_reportIssue_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/SiekamCebule/sj-sim/issues/new"));
}

void MainWindow::startExampleSeason()
{
    if(property("exampleSeasonPreparing").toBool())return;
    auto *database=GlobalDatabase::get();
    if(!database->getLoadedSimulationSaves()){database->loadSimulationSaves(true);database->setLoadedSimulationSaves(true);}
    QDir root(QCoreApplication::applicationDirPath());if(!root.exists("userData") && QSysInfo::productType()=="windows")root.cdUp();
    const QString testRoot=qApp->property("exampleSeasonStorageRoot").toString();
    const QString directory=(testRoot.isEmpty()?root.absoluteFilePath("simulationSaves"):testRoot)+"/";QStringList names;
    for(auto *save:database->getEditableGlobalSimulationSaves())if(save)names.append(save->getName());
    for(const auto &file:QDir(directory).entryInfoList({"*.json"},QDir::Files))names.append(file.completeBaseName());
    auto *setup=new NewSimulationSaveConfigurationWindow(names,this);auto *poll=new QTimer(setup);poll->setInterval(40);
    setProperty("exampleSeasonPreparing",true);const auto deadline=QDateTime::currentMSecsSinceEpoch()+30000;
    connect(poll,&QTimer::timeout,this,[this,setup,poll,directory,deadline] {
        if(!setup->catalogReady() && QDateTime::currentMSecsSinceEpoch()<deadline)return;
        poll->stop();setProperty("exampleSeasonPreparing",false);
        bool stored=false;
        try {
            auto save=setup->createExampleSave();
            if(!save){setup->deleteLater();QMessageBox::information(this,tr("Przykładowy sezon"),tr("Nie udało się przygotować przykładu. Potrzebne są kompletne paczki skoczków, skoczni i zasad oraz zimowy preset kalendarza."));return;}
            const QString base=save->getName();for(int n=2;QFileInfo::exists(directory+save->getName()+".json");++n)save->setName(base+" "+QString::number(n));
            auto &saves=GlobalDatabase::get()->getEditableGlobalSimulationSaves();saves.reserve(saves.size()+1);
            if(!save->saveToFile(directory)){setup->deleteLater();return;}
            auto *ready=save.get();saves.append(ready);save.release();stored=true;SimulationSaveListOrder::remember(saves);
            setup->deleteLater();QSettings().setValue("onboarding/welcomeDismissed",true);
            auto *manager=new SimulationSaveManagerWindow(ready,this);manager->setAttribute(Qt::WA_DeleteOnClose);manager->show();
        }catch(const std::exception &error){setup->deleteLater();QMessageBox::critical(this,tr("Przykładowy sezon"),(stored?tr("Sezon został zapisany, ale nie udało się go otworzyć: %1"):tr("Nie udało się utworzyć przykładowego sezonu: %1")).arg(QString::fromUtf8(error.what())));}
    });
    poll->start();
}

void MainWindow::on_pushButton_season_clicked()
{
    if(GlobalDatabase::get()->getLoadedSimulationSaves() == false){
        GlobalDatabase::get()->loadSimulationSaves(true);
        GlobalDatabase::get()->setLoadedSimulationSaves(true);
    }

    SimulationSavesWindow savesWindow;
    savesWindow.setModal(true);
    if(savesWindow.exec() == QDialog::Accepted){

    }
}


void MainWindow::on_pushButton_help_clicked()
{
    HelpWindow * helpWindow = new HelpWindow();
    helpWindow->setAttribute(Qt::WA_DeleteOnClose);
    helpWindow->show();
}


void MainWindow::on_pushButton_thanks_clicked()
{
    ThanksWindow * thanksWindow = new ThanksWindow();
    thanksWindow->setAttribute(Qt::WA_DeleteOnClose);
    thanksWindow->show();
}

void MainWindow::showAuthor()
{
    auto *author = new AuthorWindow(this);
    author->setAttribute(Qt::WA_DeleteOnClose);
    author->show();
}
