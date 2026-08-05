#include "SimulationSavesWindow.h"
#include "ui_SimulationSavesWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../seasons/SimulationSave.h"
#include "../../seasons/Season.h"
#include "NewSimulationSaveConfigurationWindow.h"
#include "SimulationSaveManagerWindow.h"
#include <QModelIndex>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QShortcut>
#include <QSysInfo>

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
    NewSimulationSaveConfigurationWindow simulationSaveWindow(otherNames, this);
    if(simulationSaveWindow.exec() == QDialog::Accepted){
        SimulationSave * simulationSave = new SimulationSave();
        simulationSave->setName(simulationSaveWindow.getNameFromInput());

        QVector<Jumper *> jumpers;
        for(const Jumper &jumper : GlobalDatabase::get()->getEditableGlobalJumpers())
            jumpers.append(new Jumper(jumper));
        simulationSave->setJumpers(jumpers);

        QVector<Hill *> hills;
        for(const Hill &hill : GlobalDatabase::get()->getEditableGlobalHills())
            hills.append(new Hill(hill));
        simulationSave->setHills(hills);
        simulationSave->setCompetitionRules(GlobalDatabase::get()->getEditableCompetitionRules());

        Season season;
        season.setSeasonNumber(simulationSaveWindow.getSeasonNumberFromInput());
        season.setActualCalendar(nullptr);
        simulationSave->getSeasonsReference().push_back(season);
        simulationSave->setActualSeason(&simulationSave->getSeasonsReference().last());
        simulationSave->fixJumpersFormInstabilities();
        simulationSave->updateNextCompetitionIndex();

        int index = 0;
        if(ui->listView_simulationSaves->selectionModel()->selectedRows().size() > 0)
            index = ui->listView_simulationSaves->selectionModel()->selectedRows().first().row();

        if(listModel->insertSave(index, simulationSave))
        {
            const QString directory = simulationSavesDirectory().absolutePath() + QDir::separator();
            if(simulationSave->saveToFile(directory))
            {
                const QModelIndex newIndex = listModel->index(index, 0);
                ui->listView_simulationSaves->setCurrentIndex(newIndex);
                ui->listView_simulationSaves->scrollTo(newIndex);
                openSelectedSave(true);
            }
            else
                delete listModel->takeSave(index);
        }
        else
            delete simulationSave;
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
