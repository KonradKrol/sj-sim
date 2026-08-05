#include "DatabaseEditorWindow.h"
#include "ui_DatabaseEditorWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/CountryFlagsManager.h"
#include "../../simulator/Hill.h"
#include "../EditorWidgets/JumperEditorWidget.h"
#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../EditorWidgets/PointsForPlacesPresetEditorWidget.h"
#include "ListModels/JumpersListModel.h"
#include "ListModels/HillsListModel.h"
#include "ListModels/CompetitionRulesListModel.h"
#include "ListModels/PointsForPlacesPresetsListModel.h"
#include "DatabaseItemsListView.h"
#include <QFont>
#include <QScrollArea>
#include <QCloseEvent>
#include <QMessageBox>

DatabaseEditorWindow::DatabaseEditorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseEditorWindow),
    jumperEditor(jumperEditor),
    hillEditor(hillEditor),
    competitionRulesEditor(competitionRulesEditor),
    actualElementType(0),
    actualElementIndex(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tabWidget_main->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    tempGlobalJumpers = GlobalDatabase::get()->getGlobalJumpers();
    tempGlobalHills = GlobalDatabase::get()->getGlobalHills();
    tempGlobalCompetitionRules = GlobalDatabase::get()->getGlobalCompetitionsRules();
    tempGlobalPointsForPlacesPresets = GlobalDatabase::get()->getGlobalPointsForPlacesPresets();

    jumperEditor = new JumperEditorWidget;
    jumperEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    jumperEditor->hide();
    ui->verticalLayout_jumperEditor->addWidget(jumperEditor);

    hillEditor = new HillEditorWidget();
    hillEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hillEditor->hide();
    ui->verticalLayout_hillEditor->addWidget(hillEditor);

    competitionRulesEditor = new CompetitionRulesEditorWidget();
    competitionRulesEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    competitionRulesEditor->hide();
    ui->verticalLayout_rulesEditor->addWidget(competitionRulesEditor);

    pointsForPlacesPresetEditor = new PointsForPlacesPresetEditorWidget();
    pointsForPlacesPresetEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pointsForPlacesPresetEditor->hide();
    ui->verticalLayout_pointsForPlacesEditor->addWidget(pointsForPlacesPresetEditor);

    jumpersListView = new DatabaseItemsListView(DatabaseItemsListView::JumperItems, true, true, true, this);
    jumpersListView->setJumpers(&GlobalDatabase::get()->getEditableGlobalJumpers());
    jumpersListView->setupListModel();
    jumpersListView->selectOnlyFirstRow();
    ui->verticalLayout_jumpersList->addWidget(jumpersListView);

    hillsListView = new DatabaseItemsListView(DatabaseItemsListView::HillItems, true, true, true, this);
    hillsListView->setHills(&GlobalDatabase::get()->getEditableGlobalHills());
    hillsListView->setupListModel();
    hillsListView->selectOnlyFirstRow();
    ui->verticalLayout_hillsList->addWidget(hillsListView);

    competitionRulesListView = new DatabaseItemsListView(DatabaseItemsListView::CompetitionRulesItems, true, true, true, this);
    competitionRulesListView->setCompetitionRules(&GlobalDatabase::get()->getEditableCompetitionRules());
    competitionRulesListView->setupListModel();
    competitionRulesListView->selectOnlyFirstRow();
    ui->verticalLayout_competitionRulesList->addWidget(competitionRulesListView);

    pointsForPlacesPresetsListView = new DatabaseItemsListView(DatabaseItemsListView::PointsForPlacesPresetsItems, true, true, true, this);
    pointsForPlacesPresetsListView->setPointsForPlacesPresets(&GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets());
    pointsForPlacesPresetsListView->setupListModel();
    pointsForPlacesPresetsListView->selectOnlyFirstRow();
    ui->verticalLayout_pointsForPlacesPresetsList->addWidget(pointsForPlacesPresetsListView);

    connect(ui->tabWidget_main, &QTabWidget::currentChanged, this, [this](){
        setActualElementType(ui->tabWidget_main->currentIndex());
    });
    connect(this->jumperEditor, &JumperEditorWidget::submitted, this, &DatabaseEditorWindow::replaceJumperByJumperEditor);
    connect(this->hillEditor, &HillEditorWidget::submitted, this, &DatabaseEditorWindow::replaceHillByHillEditor);
    connect(this->competitionRulesEditor, &CompetitionRulesEditorWidget::submitted, this, &DatabaseEditorWindow::replaceCompetitionRuleByCompetitionRulesEditor);
    connect(this->pointsForPlacesPresetEditor, &PointsForPlacesPresetEditorWidget::submitted, this, &DatabaseEditorWindow::replacePointsForPlacesPresetByEditorObject);

    connect(this->jumpersListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onJumpersListViewDoubleClicked);
    connect(this->hillsListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onHillsListViewDoubleClicked);
    connect(this->competitionRulesListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onCompetitionRulesListViewDoubleClicked);
    connect(this->pointsForPlacesPresetsListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onPointsForPlacesPresetsListViewDoubleClicked);

    connect(this->jumpersListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->hillsListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->competitionRulesListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->pointsForPlacesPresetsListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->jumpersListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    connect(this->hillsListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    connect(this->competitionRulesListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    connect(this->pointsForPlacesPresetsListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
}

DatabaseEditorWindow::~DatabaseEditorWindow()
{
    delete ui;
}

void DatabaseEditorWindow::replaceJumperByJumperEditor()
{
    if(actualElementIndex < GlobalDatabase::get()->getEditableGlobalJumpers().count()){
        GlobalDatabase::get()->getEditableGlobalJumpers().replace(actualElementIndex, jumperEditor->getJumperFromWidgetInput());
        JumpersListModel * jumpersListModel = dynamic_cast<JumpersListModel *>(jumpersListView->getListModel());
        emit jumpersListModel->dataChanged(jumpersListModel->index(actualElementIndex), jumpersListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::replaceHillByHillEditor()
{
    if(actualElementIndex < GlobalDatabase::get()->getEditableGlobalHills().count()){
        GlobalDatabase::get()->getEditableGlobalHills().replace(actualElementIndex, hillEditor->getHillFromWidgetInput());
        HillsListModel * hillsListModel = dynamic_cast<HillsListModel *>(hillsListView->getListModel());
        emit hillsListModel->dataChanged(hillsListModel->index(actualElementIndex), hillsListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::replaceCompetitionRuleByCompetitionRulesEditor()
{
    if(actualElementIndex < GlobalDatabase::get()->getEditableCompetitionRules().count()){
        GlobalDatabase::get()->getEditableCompetitionRules().replace(actualElementIndex, competitionRulesEditor->getCompetitionRulesFromWidgetInputs());
        CompetitionRulesListModel * competitionRulesListModel = dynamic_cast<CompetitionRulesListModel *>(competitionRulesListView->getListModel());
        emit competitionRulesListModel->dataChanged(competitionRulesListModel->index(actualElementIndex), competitionRulesListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::replacePointsForPlacesPresetByEditorObject()
{
    if(actualElementIndex < GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets().count()){
        GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets().replace(actualElementIndex, pointsForPlacesPresetEditor->constructPresetFromInputs());
        PointsForPlacesPresetsListModel * pointsForPlacesPresetsListModel = dynamic_cast<PointsForPlacesPresetsListModel *>(pointsForPlacesPresetsListView->getListModel());
        emit pointsForPlacesPresetsListModel->dataChanged(pointsForPlacesPresetsListModel->index(actualElementIndex), pointsForPlacesPresetsListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox message;
    message.setStyleSheet("background-color: white; color: black;");
    QMessageBox::StandardButton button;
    button = message.question(this, tr("Wyjście z edytora bazy danych"), tr("Czy zapisać zmiany w bazie danych?"), QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::Yes);
    if(button == QMessageBox::Yes)
    {
        GlobalDatabase::get()->writeJumpers();
        GlobalDatabase::get()->writeHills();
        GlobalDatabase::get()->writeCompetitionsRules();
        GlobalDatabase::get()->writePointsForPlacesPresets();
        event->accept();
    }
    else if(button == QMessageBox::Cancel)
    {
        event->ignore();
    }
    else if(button == QMessageBox::No)
    {
        GlobalDatabase::get()->setGlobalJumpers(tempGlobalJumpers);
        GlobalDatabase::get()->setGlobalHills(tempGlobalHills);
        GlobalDatabase::get()->setGlobalCompetitionsRules(tempGlobalCompetitionRules);
        GlobalDatabase::get()->setGlobalPointsForPlacesPresets(tempGlobalPointsForPlacesPresets);
        event->accept();
    }
    else
        qDebug()<<"invalid button in database editor close event";
}

void DatabaseEditorWindow::onJumpersListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    if(jumperEditor->isHidden()){
        ui->tabWidget_main->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        jumperEditor->show();
    }
    jumperEditor->setJumper(const_cast<Jumper *>(&GlobalDatabase::get()->getEditableGlobalJumpers().at(row)));
    jumperEditor->fillJumperInputs();
}

void DatabaseEditorWindow::onHillsListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    if(hillEditor->isHidden()){
        ui->tabWidget_main->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        hillEditor->show();
    }
    hillEditor->setHill(const_cast<Hill *>(&GlobalDatabase::get()->getEditableGlobalHills().at(row)));
    hillEditor->fillHillInputs();
}

void DatabaseEditorWindow::onCompetitionRulesListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    if(competitionRulesEditor->isHidden()){
        ui->tabWidget_main->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        competitionRulesEditor->show();
    }
    competitionRulesEditor->setCompetitionRules(const_cast<CompetitionRules *>(&GlobalDatabase::get()->getEditableCompetitionRules().at(row)));
    competitionRulesEditor->fillCompetitionRulesInputs();
}

void DatabaseEditorWindow::onPointsForPlacesPresetsListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    if(pointsForPlacesPresetEditor->isHidden()){
        ui->tabWidget_main->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        pointsForPlacesPresetEditor->show();
    }
    pointsForPlacesPresetEditor->reset();
    pointsForPlacesPresetEditor->setPreset(const_cast<PointsForPlacesPreset *>(&GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets().at(row)));
    pointsForPlacesPresetEditor->fill();
}

short DatabaseEditorWindow::getActualElementType() const
{
    return actualElementType;
}

void DatabaseEditorWindow::setActualElementType(short newActualElementType)
{
    actualElementType = newActualElementType;
}

QVector<CompetitionRules> DatabaseEditorWindow::getTempGlobalCompetitionRules() const
{
    return tempGlobalCompetitionRules;
}

void DatabaseEditorWindow::setTempGlobalCompetitionRules(const QVector<CompetitionRules> &newTempGlobalCompetitionRules)
{
    tempGlobalCompetitionRules = newTempGlobalCompetitionRules;
}

QVector<Hill> DatabaseEditorWindow::getTempGlobalHills() const
{
    return tempGlobalHills;
}

void DatabaseEditorWindow::setTempGlobalHills(const QVector<Hill> &newTempGlobalHills)
{
    tempGlobalHills = newTempGlobalHills;
}

QVector<Jumper> DatabaseEditorWindow::getTempGlobalJumpers() const
{
    return tempGlobalJumpers;
}

void DatabaseEditorWindow::setTempGlobalJumpers(const QVector<Jumper> &newTempGlobalJumpers)
{
    tempGlobalJumpers = newTempGlobalJumpers;
}

HillsListModel *DatabaseEditorWindow::getHillsListModel() const
{
    return hillsListModel;
}

void DatabaseEditorWindow::setHillsListModel(HillsListModel *newHillsListModel)
{
    hillsListModel = newHillsListModel;
}

int DatabaseEditorWindow::getActualElementIndex() const
{
    return actualElementIndex;
}

void DatabaseEditorWindow::setActualElementIndex(int newActualElementIndex)
{
    actualElementIndex = newActualElementIndex;
}

JumpersListModel *DatabaseEditorWindow::getJumpersListModel() const
{
    return jumpersListModel;
}

void DatabaseEditorWindow::setJumpersListModel(JumpersListModel *newJumpersListModel)
{
    jumpersListModel = newJumpersListModel;
}

void DatabaseEditorWindow::on_pushButton_sortByCountries_clicked()
{
    if(ui->tabWidget_main->currentWidget() == ui->tab_jumpers)
    {
        QVector<Jumper> temp;
        QVector<QString> countries;
        for(auto & jumper : GlobalDatabase::get()->getEditableGlobalJumpers())
            if(countries.contains(jumper.getCountryCode()) == false)
                countries.push_back(jumper.getCountryCode());
        for(auto & country : countries)
        {
            for(auto & jumper : Team::getJumpersFilteredByCountryCode(GlobalDatabase::get()->getEditableGlobalJumpers(), country))
                temp.push_back(*jumper);
        }
        GlobalDatabase::get()->setGlobalJumpers(temp);
        jumpersListView->setJumpers(&GlobalDatabase::get()->getEditableGlobalJumpers());
        jumpersListView->setupListModel();
    }
    else if(ui->tabWidget_main->currentWidget() == ui->tab_hills)
    {
        QVector<Hill> temp;
        QVector<QString> countries;
        for(auto & hill : GlobalDatabase::get()->getEditableGlobalHills())
            if(countries.contains(hill.getCountryCode()) == false)
                countries.push_back(hill.getCountryCode());
        for(auto & country : countries)
        {
            for(auto & hill : Hill::getHillsListByCountryCode(GlobalDatabase::get()->getEditableGlobalHills(), country))
                temp.push_back(*hill);
        }
        GlobalDatabase::get()->setGlobalHills(temp);
        hillsListView->setHills(&GlobalDatabase::get()->getEditableGlobalHills());
        hillsListView->setupListModel();
    }
}

