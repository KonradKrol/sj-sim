#include "SimulationSavesWindow.h"
#include "ui_SimulationSavesWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../seasons/SimulationSave.h"
#include "../../seasons/Season.h"
#include "NewSimulationSaveConfigurationWindow.h"
#include "SimulationSaveManagerWindow.h"
#include <QModelIndex>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QShortcut>
#include <QSysInfo>
#include <exception>
#include <memory>
#include <new>
#include <stdexcept>

namespace
{
QDir simulationSavesDirectory()
{
    QDir directory(QCoreApplication::applicationDirPath());
    if(!directory.exists("userData") && QSysInfo::productType() == "windows")
        directory.cdUp();
    directory.mkpath("simulationSaves");
    return QDir(directory.absoluteFilePath("simulationSaves"));
}
}

SimulationSavesWindow::SimulationSavesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationSavesWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    listModel = new SimulationSavesListModel(&GlobalDatabase::get()->getEditableGlobalSimulationSaves(), this);
    ui->listView_simulationSaves->setModel(listModel);
    ui->listView_simulationSaves->setSelectionMode(QAbstractItemView::SingleSelection);

    saveInfoWidget = new SimulationSaveInfoWidget(this);
    saveInfoWidget->hide();
    ui->verticalLayout_saveInfos->addWidget(saveInfoWidget);

    ui->pushButton_add->setToolTip(tr("Utwórz nowy zapis symulacji"));
    ui->pushButton_OK->setToolTip(tr("Otwórz zaznaczony zapis (Enter)"));
    ui->pushButton_remove->setToolTip(tr("Usuń zaznaczony zapis (Delete)"));
    ui->listView_simulationSaves->setToolTip(tr("Kliknij zapis, aby zobaczyć szczegóły. Enter otwiera, Delete usuwa."));

    connect(ui->listView_simulationSaves->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this](){
        updateSelectionState();
    });

    QShortcut *openShortcut = new QShortcut(QKeySequence(Qt::Key_Return), ui->listView_simulationSaves);
    connect(openShortcut, &QShortcut::activated, this, [this](){ openSelectedSave(); });
    QShortcut *openKeypadShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), ui->listView_simulationSaves);
    connect(openKeypadShortcut, &QShortcut::activated, this, [this](){ openSelectedSave(); });
    QShortcut *removeShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listView_simulationSaves);
    connect(removeShortcut, &QShortcut::activated, this, &SimulationSavesWindow::on_pushButton_remove_clicked);

    if(listModel->rowCount() > 0)
        ui->listView_simulationSaves->setCurrentIndex(listModel->index(0, 0));
    updateSelectionState();
}

SimulationSavesWindow::~SimulationSavesWindow()
{
    delete ui;
}

void SimulationSavesWindow::on_listView_simulationSaves_doubleClicked(const QModelIndex &index)
{
    if(index.isValid())
        openSelectedSave();
}

void SimulationSavesWindow::on_pushButton_add_clicked()
{
    QStringList otherNames;
    for(auto & save : GlobalDatabase::get()->getEditableGlobalSimulationSaves()){
        otherNames.push_back(save->getName());
    }
    const QStringList existingSaveFiles = simulationSavesDirectory().entryList(
        QStringList() << QStringLiteral("*.json"), QDir::Files);
    for(const QString &fileName : existingSaveFiles)
        otherNames.push_back(QFileInfo(fileName).completeBaseName());

    NewSimulationSaveConfigurationWindow simulationSaveWindow(otherNames, this);
    if(simulationSaveWindow.exec() != QDialog::Accepted)
        return;

    bool savedToDisk = false;
    try
    {
        std::unique_ptr<SimulationSave> simulationSave = std::make_unique<SimulationSave>();
        simulationSave->setName(simulationSaveWindow.getNameFromInput());

        for(const Jumper &jumper : GlobalDatabase::get()->getEditableGlobalJumpers())
        {
            std::unique_ptr<Jumper> copy = std::make_unique<Jumper>(jumper);
            simulationSave->getJumpersReference().append(copy.get());
            copy.release();
        }

        for(const Hill &hill : GlobalDatabase::get()->getEditableGlobalHills())
        {
            std::unique_ptr<Hill> copy = std::make_unique<Hill>(hill);
            simulationSave->getHillsReference().append(copy.get());
            copy.release();
        }
        simulationSave->setCompetitionRules(GlobalDatabase::get()->getEditableCompetitionRules());

        Season season;
        season.setSeasonNumber(simulationSaveWindow.getSeasonNumberFromInput());
        season.setActualCalendar(nullptr);
        simulationSave->getSeasonsReference().push_back(season);
        simulationSave->setActualSeason(&simulationSave->getSeasonsReference().last());
        simulationSave->fixJumpersFormInstabilities();
        simulationSave->updateNextCompetitionIndex();

        GlobalDatabase::get()->getEditableGlobalSimulationSaves().reserve(
            GlobalDatabase::get()->getEditableGlobalSimulationSaves().size() + 1);
        int index = 0;
        if(ui->listView_simulationSaves->selectionModel()->selectedRows().size() > 0)
            index = ui->listView_simulationSaves->selectionModel()->selectedRows().first().row();

        const QString directory = simulationSavesDirectory().absolutePath() + QDir::separator();
        if(!simulationSave->saveToFile(directory))
            return;
        savedToDisk = true;

        if(!listModel->insertSave(index, simulationSave.get()))
            throw std::runtime_error("Could not add the new simulation to the saves list");
        simulationSave.release();

        const QModelIndex newIndex = listModel->index(index, 0);
        ui->listView_simulationSaves->setCurrentIndex(newIndex);
        ui->listView_simulationSaves->scrollTo(newIndex);
        openSelectedSave(true);
    }
    catch(const std::bad_alloc &)
    {
        QMessageBox::critical(this, tr("Brak pamięci"),
                              savedToDisk
                                  ? tr("Zapis został utworzony na dysku, ale zabrakło pamięci, aby go otworzyć. Uruchom aplikację ponownie.")
                                  : tr("Nie udało się utworzyć symulacji, ponieważ zabrakło pamięci. Zamknij inne programy i spróbuj ponownie."));
    }
    catch(const std::exception &exception)
    {
        QMessageBox::critical(this, tr("Nie udało się utworzyć symulacji"),
                              tr("Wystąpił błąd podczas tworzenia symulacji: %1").arg(QString::fromUtf8(exception.what())));
    }
    catch(...)
    {
        QMessageBox::critical(this, tr("Nie udało się utworzyć symulacji"),
                              tr("Wystąpił nieoczekiwany błąd podczas tworzenia symulacji."));
    }
}


void SimulationSavesWindow::on_pushButton_OK_clicked()
{
    openSelectedSave();
}

void SimulationSavesWindow::openSelectedSave(bool focusCalendar)
{
    const int row = selectedSaveRow();
    if(row >= 0){
        SimulationSaveManagerWindow * manager = new SimulationSaveManagerWindow(GlobalDatabase::get()->getEditableGlobalSimulationSaves().at(row), this);
        manager->getSimulationSave()->fixJumpersFormInstabilities();
        manager->getSimulationSave()->updateNextCompetitionIndex();
        manager->updateCompetitionConfigButton();
        manager->fillNextCompetitionInformations();
        if(focusCalendar)
            manager->focusCalendarTab();
        manager->exec();
        delete manager;
        updateSelectionState();
    }
}


void SimulationSavesWindow::on_pushButton_remove_clicked()
{
    const int row = selectedSaveRow();
    if(row >= 0){
        SimulationSave *save = GlobalDatabase::get()->getEditableGlobalSimulationSaves().at(row);
        auto button = QMessageBox::question(this, tr("Usunięcie zapisu symulacji"),
                                            tr("Czy na pewno chcesz usunąć zapis „%1”? Tej operacji nie można cofnąć.").arg(save->getName()),
 QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
        if(button == QMessageBox::Yes)
        {
            const QString filePath = simulationSavesDirectory().absoluteFilePath(save->getName() + ".json");
            if(QFile::exists(filePath) && !QFile::remove(filePath))
            {
                QMessageBox::critical(this, tr("Nie udało się usunąć zapisu"),
                                      tr("Plik zapisu nie został usunięty. Sprawdź uprawnienia do pliku i spróbuj ponownie."));
                return;
            }
            delete listModel->takeSave(row);

            if(listModel->rowCount() > 0)
                ui->listView_simulationSaves->setCurrentIndex(listModel->index(qMin(row, listModel->rowCount() - 1), 0));
            updateSelectionState();
        }
    }
}

int SimulationSavesWindow::selectedSaveRow() const
{
    const QModelIndex index = ui->listView_simulationSaves->currentIndex();
    if(!index.isValid() || index.row() < 0 || index.row() >= listModel->rowCount())
        return -1;
    return index.row();
}

void SimulationSavesWindow::updateSelectionState()
{
    const int row = selectedSaveRow();
    const bool hasSelection = row >= 0;
    ui->pushButton_OK->setEnabled(hasSelection);
    ui->pushButton_remove->setEnabled(hasSelection);

    if(hasSelection)
    {
        saveInfoWidget->setSimulationSave(GlobalDatabase::get()->getEditableGlobalSimulationSaves().at(row));
        saveInfoWidget->fillInputs();
        saveInfoWidget->show();
    }
    else
    {
        saveInfoWidget->setSimulationSave(nullptr);
        saveInfoWidget->resetInputs();
        saveInfoWidget->hide();
    }
}
