#include "SimulationSavesWindow.h"
#include "ui_SimulationSavesWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../seasons/SimulationSave.h"
#include "../../seasons/Season.h"
#include "NewSeasonConfiguratorWindow.h"
#include "NewSimulationSaveConfigurationWindow.h"
#include "SimulationSaveManagerWindow.h"
#include <QTimer>
#include <QModelIndex>
#include <QFile>

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

    connect(ui->listView_simulationSaves, &QListView::doubleClicked, this, [this](const QModelIndex & index){
        saveInfoWidget->show();
        saveInfoWidget->setSimulationSave(GlobalDatabase::get()->getEditableGlobalSimulationSaves()[index.row()]);
        saveInfoWidget->fillInputs();
    });
}

SimulationSavesWindow::~SimulationSavesWindow()
{
    delete ui;
}

void SimulationSavesWindow::on_listView_simulationSaves_doubleClicked(const QModelIndex &index)
{
}

void SimulationSavesWindow::on_pushButton_add_clicked()
{
    QStringList otherNames;
    for(auto & save : GlobalDatabase::get()->getEditableGlobalSimulationSaves()){
        otherNames.push_back(save->getName());
    }
    NewSimulationSaveConfigurationWindow * simulationSaveWindow = new NewSimulationSaveConfigurationWindow(otherNames, this);
    if(simulationSaveWindow->exec() == QDialog::Accepted){
        NewSeasonConfiguratorWindow * seasonWindow = new NewSeasonConfiguratorWindow(false, this);
        connect(seasonWindow->getToolBox(), &QToolBox::currentChanged, this, [this, seasonWindow](){
            if(seasonWindow->getToolBox()->currentIndex() == 3){ //edytor kalendarzy
                QTimer::singleShot(200, [this, seasonWindow](){
                    seasonWindow->showCalendarEditorHelp();
                });
            }
        });
        if(seasonWindow->exec() == QDialog::Accepted){
            SimulationSave * simulationSave = new SimulationSave();
            simulationSave->setName(simulationSaveWindow->getNameFromInput());
            simulationSave->setJumpers(seasonWindow->getJumpersReference());
            simulationSave->setHills(seasonWindow->getHillsReference());
            simulationSave->setCompetitionRules(seasonWindow->getCompetitionsRulesReference());

            Season season;
            season.setSeasonNumber(simulationSaveWindow->getSeasonNumberFromInput());

            simulationSave->getSeasonsReference().push_back(season);
            simulationSave->setActualSeason(&simulationSave->getSeasonsReference().last());
            simulationSave->getActualSeason()->setActualCalendar(nullptr);

            simulationSave->updateNextCompetitionIndex();

            int index = 0;
            if(ui->listView_simulationSaves->selectionModel()->selectedRows().size() > 0)
                index = ui->listView_simulationSaves->selectionModel()->selectedRows().first().row();

            GlobalDatabase::get()->getEditableGlobalSimulationSaves().insert(index, simulationSave);

            emit listModel->dataChanged(listModel->index(index), listModel->index(listModel->rowCount() - 1));

            simulationSave->saveToFile("simulationSaves/");
        }
    }
}


void SimulationSavesWindow::on_pushButton_OK_clicked()
{
    if(ui->listView_simulationSaves->selectionModel()->selectedRows().count() > 0){
        SimulationSaveManagerWindow * manager = new SimulationSaveManagerWindow(GlobalDatabase::get()->getEditableGlobalSimulationSaves()[ui->listView_simulationSaves->selectionModel()->selectedRows().first().row()], this);
        manager->getSimulationSave()->fixJumpersFormInstabilities();
        manager->getSimulationSave()->updateNextCompetitionIndex();
        if(manager->getSimulationSave()->getNextCompetition() == nullptr)
            manager->setupNextSeasonConfigButton();
        manager->fillNextCompetitionInformations();
        if(manager->exec() == QDialog::Accepted)
        {

        }
    }
}


void SimulationSavesWindow::on_pushButton_remove_clicked()
{
    if(ui->listView_simulationSaves->selectionModel()->selectedRows().count() > 0){
        auto button = QMessageBox::question(this, tr("Usunięcie zapisu symulacji"), tr("Czy na pewno chcesz usunąć ten zapis symulacji?"),
 QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
        if(button == QMessageBox::Yes)
        {
            int row = ui->listView_simulationSaves->selectionModel()->selectedRows().first().row();
            QFile file("simulationSaves/" + GlobalDatabase::get()->getEditableGlobalSimulationSaves()[row]->getName() + ".json");
            file.remove();
            GlobalDatabase::get()->getEditableGlobalSimulationSaves().remove(row);
            listModel->removeRow(row);
        }
    }
}

