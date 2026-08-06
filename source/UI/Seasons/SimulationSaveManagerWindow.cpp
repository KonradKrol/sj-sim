#include "SimulationSaveManagerWindow.h"
#include "ui_SimulationSaveManagerWindow.h"
#include "../../global/GlobalAppSettings.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalDatabase.h"
#include "../../utilities/functions.h"
#include "../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../Competition/CompetitionConfigWindow.h"
#include "../Competition/CompetitionManagerWindow.h"
#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/InrunSnowGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/JumpersListEditorWindow.h"
#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../Competition/JumperCompetitionResultsWidget.h"
#include "../Competition/Results/ResultsTableModel.h"
#include "../Competition/Results/TeamResultsTreeModel.h"
#include "../FormGenerator/JumpersFormGeneratorConfigWindow.h"
#include "Stats/JumperStatsWindow.h"
#include "Stats/ApperanceInClassificationWindow.h"
#include "Stats/SimulationRatingsWindow.h"
#include "NewSeasonConfiguratorWindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QProgressDialog>
#include <QPushButton>
#include <QSignalBlocker>
#include <QInputDialog>
#include <QDesktopServices>
#include <QTableView>
#include <QFileDialog>
#include <QTreeView>
#include <QLabel>
#include <QResizeEvent>

SimulationSaveManagerWindow::SimulationSaveManagerWindow(SimulationSave *save, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationSaveManagerWindow),
    simulationSave(save)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setMinimumSize(640, 480);
    ui->splitter_manager->setStretchFactor(0, 4);
    ui->splitter_manager->setStretchFactor(1, 1);
    ui->splitter_manager->setSizes({980, 300});

    // Calendar data loaded from older saves may contain a missing or stale hill
    // reference. Repair it before any widget or diagnostic code dereferences it.
    if(simulationSave->getActualSeason() != nullptr
        && simulationSave->getActualSeason()->getActualCalendar() != nullptr
        && !simulationSave->getHillsReference().isEmpty())
    {
        simulationSave->getActualSeason()->getActualCalendar()->fixCompetitionsHills(
            &simulationSave->getHillsReference(), simulationSave->getHillsReference().first());
    }

    ui->toolBox->setCurrentIndex(0);
    ui->toolBox_2->setCurrentIndex(0);
    ui->label_saveName->setText(simulationSave->getName());
    ui->label_seasonNumber->setText(QString::number(simulationSave->getActualSeason()->getSeasonNumber()));
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr)
        ui->label_calendarName->setText(simulationSave->getActualSeason()->getActualCalendar()->getName());
    else
        ui->label_calendarName->setText(tr("BRAK"));
    ui->comboBox_archiveSeason->setCurrentIndex(0);

    connect(ui->toolBox, &QToolBox::currentChanged, this, [this](){
        if(ui->toolBox->currentIndex() == 0 || ui->toolBox->currentIndex() == 1)
        {
            QTimer::singleShot(200, this, [this](){
                showJumperAndHillsEditingHelp();
            });
        }
    });
    emit ui->toolBox->currentChanged(0);

    //-----//

    jumpersListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonJumpersItems, true, false, true, this);
    jumpersListView->setSeasonJumpers(&simulationSave->getJumpersReference());
    jumpersListView->setupListModel();
    ui->verticalLayout_jumpersList->addWidget(jumpersListView);

    jumperEditor = new JumperEditorWidget();
    jumperEditor->hide();
    ui->verticalLayout_jumperEditor->addWidget(jumperEditor);
    connect(jumpersListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex & index){
        jumperEditor->show();
        jumperEditor->setJumper(simulationSave->getJumpersReference()[index.row()]);
        if(simulationSave->getShowInstability() == true)
            jumperEditor->setFormInstability(simulationSave->getJumperFormInstability(jumperEditor->getJumper()));
        else jumperEditor->setFormInstability(nullptr);
        jumperEditor->fillJumperInputs();
        jumperEditor->setShowForm(simulationSave->getShowForm());
    });
    connect(jumperEditor, &JumperEditorWidget::submitted, this, [this](){
        int idx = jumpersListView->getLastDoubleClickedIndex();
        *simulationSave->getJumpersReference()[idx] = jumperEditor->getJumperFromWidgetInput();
        simulationSave->getJumpersFormInstabilitiesReference()[jumperEditor->getJumper()] = jumperEditor->getFormInstabilityFromUI();
        emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(idx), jumpersListView->getListModel()->index(idx));
    });

    //-----//

    hillsListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonHillsItems, true, false, true, this);
    hillsListView->setSeasonHills(&simulationSave->getHillsReference());
    hillsListView->setupListModel();
    ui->verticalLayout_hillsList->addWidget(hillsListView);

    hillEditor = new HillEditorWidget();
    hillEditor->hide();
    ui->verticalLayout_hillEditor->addWidget(hillEditor);
    connect(hillsListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex & index){
        hillEditor->show();
        hillEditor->setHill(simulationSave->getHillsReference()[index.row()]);
        hillEditor->fillHillInputs();
    });
    connect(hillEditor, &HillEditorWidget::submitted, this, [this](){
        int idx = hillsListView->getLastDoubleClickedIndex();
        *simulationSave->getHillsReference()[idx] = hillEditor->getHillFromWidgetInput();
        emit hillsListView->getListModel()->dataChanged(hillsListView->getListModel()->index(idx), hillsListView->getListModel()->index(idx));
    });
    connect(hillsListView, &DatabaseItemsListView::insert, this, [this](){
        SeasonCalendar *actualCalendar = simulationSave->getActualSeason()->getActualCalendar();
        if(actualCalendar != nullptr && !simulationSave->getHillsReference().isEmpty())
            actualCalendar->fixCompetitionsHills(&simulationSave->getHillsReference(), simulationSave->getHillsReference().first());
    });

    //-----//

    rulesListView = new DatabaseItemsListView(DatabaseItemsListView::CompetitionRulesItems, true, true, true, this);
    rulesListView->setCompetitionRules(&simulationSave->getCompetitionRulesReference());
    rulesListView->setupListModel();
    ui->verticalLayout_rulesList->addWidget(rulesListView);

    rulesEditor = new CompetitionRulesEditorWidget();
    rulesEditor->hide();
    ui->verticalLayout_rulesEditor->addWidget(rulesEditor);
    connect(rulesListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex & index){
        rulesEditor->show();
        rulesEditor->setCompetitionRules(&simulationSave->getCompetitionRulesReference()[index.row()]);
        rulesEditor->fillCompetitionRulesInputs();
    });
    connect(rulesEditor, &CompetitionRulesEditorWidget::submitted, this, [this](){
        int idx = rulesListView->getLastDoubleClickedIndex();
        simulationSave->getCompetitionRulesReference()[idx] = rulesEditor->getCompetitionRulesFromWidgetInputs();
        emit rulesListView->getListModel()->dataChanged(rulesListView->getListModel()->index(idx), rulesListView->getListModel()->index(idx));
    });

    //-----//

    SeasonCalendar *actualCalendar = simulationSave->getActualSeason()->getActualCalendar();
    if(actualCalendar != nullptr && !simulationSave->getHillsReference().isEmpty())
        actualCalendar->fixCompetitionsHills(&simulationSave->getHillsReference(), simulationSave->getHillsReference().first());
    calendarTableModel = new CalendarEditorTableModel(actualCalendar, &simulationSave->getHillsReference(), &simulationSave->getCompetitionRulesReference(), simulationSave->getNextCompetitionIndex(), this);
    QVector<Classification *> *activeClassifications = actualCalendar != nullptr
        ? &actualCalendar->getClassificationsReference()
        : &emptyClassifications;
    calendarEmptyStateLabel = new QLabel(tr("Utwórz kalendarz, a następnie kliknij go dwukrotnie, aby rozpocząć edycję."), this);
    calendarEmptyStateLabel->setAlignment(Qt::AlignCenter);
    calendarEmptyStateLabel->setWordWrap(true);
    calendarEmptyStateLabel->setVisible(actualCalendar == nullptr);
    ui->verticalLayout_calendar->addWidget(calendarEmptyStateLabel);
    calendarEditor = new CalendarEditorWidget(calendarTableModel, activeClassifications, this);
    calendarEditor->setSave(simulationSave);
    calendarEditor->setEnabled(actualCalendar != nullptr);
    ui->verticalLayout_calendar->addWidget(calendarEditor);
    ui->pushButton_repairDatabase->setEnabled(actualCalendar != nullptr && !simulationSave->getHillsReference().isEmpty());
    ui->pushButton_repairDatabase->setToolTip(actualCalendar != nullptr
        ? tr("Sprawdź i napraw powiązania w aktualnym zapisie")
        : tr("Najpierw utwórz i wybierz kalendarz"));
    calendarsListView = new DatabaseItemsListView(DatabaseItemsListView::CalendarItems, true, true, true, this);
    calendarsListView->setAddKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A);
    calendarsListView->setUpKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Up);
    calendarsListView->setDownKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Down);
    calendarsListView->setRemoveKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_D);
    calendarsListView->hideInstructions();
    calendarsListView->setCalendars(&simulationSave->getActualSeason()->getCalendarsReference());
    calendarsListView->setupListModel();
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr)
        calendarsListView->getListView()->selectionModel()->select(calendarsListView->getListModel()->index(simulationSave->getActualSeason()->getCalendarsReference().indexOf(simulationSave->getActualSeason()->getActualCalendar())), QItemSelectionModel::Select);
    ui->verticalLayout_calendarsListView->insertWidget(0, calendarsListView);

    //-----//

    classificationEditor = new ClassificationEditorWidget();
    classificationEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    classificationEditor->setParent(this);
    ui->verticalLayout_classificationEditor->addWidget(classificationEditor);

    classificationsListView = new DatabaseItemsListView(DatabaseItemsListView::ClassificationItems, true, true, true, this);
    classificationsListView->setClassifications(activeClassifications);
    classificationsListView->setupListModel();
    classificationsListView->setEnabled(actualCalendar != nullptr);
    classificationEditor->setEnabled(actualCalendar != nullptr);
    ui->lineEdit_calendarName->setEnabled(actualCalendar != nullptr);
    classificationsListView->selectOnlyFirstRow();
    ui->verticalLayout_classificationsList->addWidget(classificationsListView);
    connect(classificationsListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex &index){
        Classification * classification = simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference()[index.row()];
        if(classificationEditor->getClassification() != nullptr)
            classificationEditor->resetInputs();
        classificationEditor->setClassification(classification);
        classificationEditor->fillInputs();
    });
    connect(classificationEditor, &ClassificationEditorWidget::submitted, this, [this](){
        bool classificationWasUsed = false;
        for(auto & comp : simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference())
        {
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                if(comp->getPlayed() == false)
                    break;
                if(MyFunctions::vectorContainsByID(comp->getClassificationsReference(), classificationEditor->getClassification()))
                {
                    classificationWasUsed = true;
                    break;
                }
            }
        }
        if(classificationWasUsed == false){
            if(classificationsListView->getListView()->selectionModel()->selectedRows().count() > 0)
            {
                int row = classificationsListView->getLastDoubleClickedIndex();//->getListView()->selectionModel()->selectedRows().at(0).row();
                simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference()[row] = classificationEditor->getClassificationFromInputs();
            }
        }
        else
        {
            QMessageBox::information(this, tr("Edycja klasyfikacji"), tr("Nie możesz edytować tej klasyfikacji, ponieważ wcześniej został rozegrany konkurs z jej użyciem.\nMożesz ją tylko całkowicie usunąć"), QMessageBox::Ok);
        }
    });
    connect(classificationsListView, &DatabaseItemsListView::insert, this, [this](){
        simulationSave->getActualSeason()->getActualCalendar()->fixCompetitionsClassifications();
        setupClassificationsComboBox();
        emit ui->comboBox_classifications->currentIndexChanged(0);
        emit calendarTableModel->dataChanged(calendarTableModel->index(0, 0), calendarTableModel->index(calendarTableModel->rowCount() - 1, 6));
    });
    connect(classificationsListView, &DatabaseItemsListView::remove, this, [this](){
        simulationSave->getActualSeason()->getActualCalendar()->fixCompetitionsClassifications();
        setupClassificationsComboBox();
        if(ui->comboBox_classifications->count() > 0)
            emit ui->comboBox_classifications->currentIndexChanged(0);
        emit calendarTableModel->dataChanged(calendarTableModel->index(0, 0), calendarTableModel->index(calendarTableModel->rowCount() - 1, 6));
    });
    connect(calendarEditor, &CalendarEditorWidget::changed, this, [this](){
        simulationSave->updateNextCompetitionIndex();
        fillNextCompetitionInformations();
        simulationSave->getActualSeason()->getActualCalendar()->updateCompetitionsQualifyingCompetitions();
        updateCompetitionConfigButton();
    });

    classificationResultsTableView = new ClassificationResultsTableView(false, nullptr, this);
    classificationResultsTableView->setSave(simulationSave);
    classificationResultsTableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    classificationResultsTableView->setFixedHeight(500);
    ui->verticalLayout_classificationResults->addWidget(classificationResultsTableView);

    setupClassificationsComboBox();
    if(ui->comboBox_classifications->count() > 0)
    {
        emit ui->comboBox_classifications->currentIndexChanged(0);
    }

    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr)
    {
        competitionsArchiveModel = new CompetitionsArchiveListModel(&simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference(), this);
        classificationsArchiveModel = new ClassificationsArchiveListModel(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference(), this);
    }
    else
    {
        competitionsArchiveModel = new CompetitionsArchiveListModel(&emptyArchiveCompetitions, this);
        classificationsArchiveModel = new ClassificationsArchiveListModel(&emptyArchiveClassifications, this);
    }

    ui->comboBox_archiveSeason->clear();
    for(auto & season : simulationSave->getSeasonsReference())
    {
        ui->comboBox_archiveSeason->insertItem(0, QString::number(season.getSeasonNumber()));
    }
    ui->comboBox_archiveCalendar->setCurrentIndex(0);
    if(simulationSave->getActualSeason()->getActualCalendar())
        ui->comboBox_archiveCalendar->setCurrentIndex(simulationSave->getActualSeason()->getCalendarsReference().indexOf(simulationSave->getActualSeason()->getActualCalendar()));

    ui->listView_competitionsArchive->setModel(competitionsArchiveModel);
    ui->listView_classificationsArchive->setModel(classificationsArchiveModel);

    archiveClassificationResults = new ClassificationResultsTableView(false, nullptr, this);
    archiveClassificationResults->setSave(simulationSave);
    archiveClassificationResults->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    archiveClassificationResults->setFixedHeight(500);
    ui->verticalLayout_archiveClassificationResults->addWidget(archiveClassificationResults);

    connect(classificationResultsTableView->getTableView(), &QTableView::doubleClicked, this, [this](const QModelIndex & index){
        showClassificationApperanceWindowAfterListClick(index, classificationResultsTableView->getClassification());
    });
    connect(archiveClassificationResults->getTableView(), &QTableView::doubleClicked, this, [this](const QModelIndex & index){
        showClassificationApperanceWindowAfterListClick(index, archiveClassificationResults->getClassification());
    });

    jumpersListsListView = new DatabaseItemsListView(DatabaseItemsListView::JumpersListsItems, true, true, true, this);
    jumpersListsListView->setMinimumWidth(220);
    jumpersListsListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    jumpersListsListView->setJumpersLists(&simulationSave->getJumpersListsReference());
    jumpersListsListView->setupListModel();
    jumpersListsListView->selectOnlyFirstRow();
    ui->verticalLayout_jumpersListsListView->addWidget(jumpersListsListView);
    connect(jumpersListsListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex & index){
        SaveJumpersList * list = &simulationSave->getJumpersListsReference()[index.row()];
        JumpersListEditorWindow * editor = new JumpersListEditorWindow(this, list->getName(), list->getIsDefault());
        editor->setAllJumpers(simulationSave->getJumpersReference());
        editor->setSelectedJumpers(list->getJumpersReference());
        editor->updateUnselectedJumpers();
        editor->getSelectedJumpersListView()->setSeasonJumpers(&editor->getSelectedJumpersReference());
        editor->getSelectedJumpersListView()->setupListModel();
        editor->getSelectedJumpersListView()->hideInstructions();
        editor->getUnselectedJumpersListView()->setSeasonJumpers(&editor->getUnselectedJumpersReference());
        editor->getUnselectedJumpersListView()->setupListModel();
        editor->getUnselectedJumpersListView()->hideInstructions();

        if(editor->exec() == QDialog::Accepted)
        {
            list->setJumpers(editor->getSelectedJumpers());
            list->setName(editor->getNameFromLineEdit());
            list->setIsDefault(editor->getIsDefaultFromCheckBox());
            jumpersListsListView->setupListModel();
            emit jumpersListsListView->getListModel()->dataChanged(jumpersListsListView->getListModel()->index(0), jumpersListsListView->getListModel()->index(jumpersListsListView->getListModel()->rowCount() - 1));
        }
    });

    {
        QSignalBlocker compactSaveFileBlocker(ui->checkBox_compactSaveFile);
        QSignalBlocker showFormBlocker(ui->checkBox_showForm);
        QSignalBlocker showInstabilityBlocker(ui->checkBox_showInstability);
        ui->checkBox_compactSaveFile->setChecked(simulationSave->getSaveFileSizeReduce());
        ui->checkBox_showForm->setChecked(simulationSave->getShowForm());
        ui->checkBox_showInstability->setChecked(simulationSave->getShowInstability());
    }

    connect(this, &SimulationSaveManagerWindow::actualCalendarChanged, this, [this](){
        SeasonCalendar *actualCalendar = simulationSave->getActualSeason()->getActualCalendar();
        if(actualCalendar == nullptr)
            return;
        classificationsListView->setClassifications(&actualCalendar->getClassificationsReference());
        classificationsListView->setupListModel();
        competitionsArchiveModel->setSeasonCompetitions(&actualCalendar->getCompetitionsReference());
        classificationsArchiveModel->setSeasonClassifications(&actualCalendar->getClassificationsReference());
    });

    connect(calendarsListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex & index){
        simulationSave->getActualSeason()->setActualCalendar(simulationSave->getActualSeason()->getCalendarsReference()[index.row()]);
        emit actualCalendarChanged();      
    });
    connect(this, &SimulationSaveManagerWindow::actualCalendarChanged, this, [this](){
        SeasonCalendar *actualCalendar = simulationSave->getActualSeason()->getActualCalendar();
        if(actualCalendar == nullptr)
            return;
        calendarEmptyStateLabel->hide();
        calendarEditor->setEnabled(true);
        classificationsListView->setEnabled(true);
        classificationEditor->setEnabled(true);
        ui->lineEdit_calendarName->setEnabled(true);
        calendarTableModel->setCalendar(actualCalendar);
        calendarEditor->setSave(simulationSave);
        calendarEditor->setClassificationsList(&actualCalendar->getClassificationsReference());
        calendarEditor->updateTable();
        ui->pushButton_repairDatabase->setEnabled(!simulationSave->getHillsReference().isEmpty());
        ui->pushButton_repairDatabase->setToolTip(tr("Sprawdź i napraw powiązania w aktualnym zapisie"));
        ui->lineEdit_calendarName->setText(actualCalendar->getName());
        updateCompetitionConfigButton();
        fillNextCompetitionInformations();
        ui->label_calendarName->setText(simulationSave->getActualSeason()->getActualCalendar()->getName());
        setupClassificationsComboBox();

        classificationsListView->setClassifications(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
        classificationsListView->setupListModel();
        if(ui->comboBox_classifications->count() > 0)
            emit ui->comboBox_classifications->currentIndexChanged(0);

        if(simulationSave->getActualSeason()->getActualCalendar())
            ui->comboBox_archiveCalendar->setCurrentIndex(simulationSave->getActualSeason()->getCalendarsReference().indexOf(simulationSave->getActualSeason()->getActualCalendar()));
    });
    connect(calendarEditor, &CalendarEditorWidget::calendarPresetLoaded, this, [this](){
        classificationsListView->setClassifications(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
        classificationsListView->getListModel()->insertRows(0, simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference().count());
        classificationsListView->getListView()->reset();
        simulationSave->updateNextCompetitionIndex();
        updateCompetitionConfigButton();
    });

    updateResponsiveLayout(width());
}

void SimulationSaveManagerWindow::updateResponsiveLayout(int windowWidth)
{
    const bool useCompactLayout = windowWidth < 1100;
    if(useCompactLayout == compactLayout)
        return;

    compactLayout = useCompactLayout;
    ui->splitter_manager->setOrientation(compactLayout ? Qt::Vertical : Qt::Horizontal);
    if(compactLayout)
        ui->splitter_manager->setSizes({qMax(260, height() * 2 / 3), 180});
    else
        ui->splitter_manager->setSizes({qMax(700, windowWidth - 300), 300});
}

void SimulationSaveManagerWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateResponsiveLayout(event->size().width());
}

void SimulationSaveManagerWindow::showClassificationApperanceWindowAfterListClick(const QModelIndex &index, Classification * classification)
{
    if(index.column() == 1)
    {
         ApperanceInClassificationWindow * window = new ApperanceInClassificationWindow(this);
         if(classification->getClassificationType() == Classification::Individual){
             Jumper * jumper = classification->getResultByIndex(index.row())->getJumper();
             window->setJumper(jumper);
         }
         else{
             QString teamCode = classification->getResultByIndex(index.row())->getTeamCode();
             window->setTeamCode(teamCode);
         }
         window->setClassification(classification);

         Season * classificationSeason = nullptr;
         for(auto & season : simulationSave->getSeasonsReference())
         {
             for(auto & cal : season.getCalendarsReference())
             {
                 for(auto & classification : cal->getClassificationsReference())
                     if(classification == window->getClassification())
                         classificationSeason = &season;
             }
         }
         window->setSeason(classificationSeason);

         if(classification->getClassificationType() == Classification::Individual){
             QHash<Jumper *, QHash<CompetitionInfo *, int>> archiveResults; //Tu są pozycje danego zawodnika z danych etapów klasyfikacji. 
             archiveResults = classification->constructJumpersArchiveResults(classificationSeason, classificationSeason->getCalendarsReference()[ui->comboBox_archiveCalendar->currentIndex()]);
             window->setArchiveResults(archiveResults.value(window->getJumper())); //Daliśmy do windowa informacje o pozycjach Kamila Stocha w różnych etapach klasyfikacji
         }
         else{
             QHash<QString, QHash<CompetitionInfo *, int>> archiveResults;
             archiveResults = classification->constructTeamsArchiveResults(classificationSeason, classificationSeason->getCalendarsReference()[ui->comboBox_archiveCalendar->currentIndex()]);
             window->setArchiveResults(archiveResults.value(window->getTeamCode()));
        }
         window->fillWindow();

         if(window->exec() == QDialog::Accepted){

         }
    }
}

DatabaseItemsListView *SimulationSaveManagerWindow::getCalendarsListView() const
{
    return calendarsListView;
}

void SimulationSaveManagerWindow::focusCalendarTab()
{
    ui->toolBox->setCurrentIndex(3);
    calendarsListView->getListView()->setFocus();
}

void SimulationSaveManagerWindow::setCalendarsListView(DatabaseItemsListView *newCalendarsListView)
{
    calendarsListView = newCalendarsListView;
}

SimulationSaveManagerWindow::~SimulationSaveManagerWindow()
{
    delete ui;
}

void SimulationSaveManagerWindow::showJumperAndHillsEditingHelp()
{
    if(GlobalAppSettings::get()->getShowSeasonJumpersAndHillsHelp() == true){
    QMessageBox box;
    box.setWindowTitle(tr("Edycja zawodników i skoczni"));
    box.setText(tr("BARDZO WAŻNE: Zawodników i skoczni które dodano, nie można już później usunąć. Bądź ostrożny podczas dodawania!"));
    QAbstractButton *pButtonDontShow = box.addButton(tr("Nie pokazuj ponownie"), QMessageBox::ActionRole);
    box.setStandardButtons(QMessageBox::Ok);
    box.setDefaultButton(QMessageBox::Ok);
    box.exec();
    if(box.clickedButton() == pButtonDontShow)
    {
        GlobalAppSettings::get()->setShowSeasonJumpersAndHillsHelp(false);
        GlobalAppSettings::get()->writeToJson();
    }
    }
}

void SimulationSaveManagerWindow::fillNextCompetitionInformations()
{
    simulationSave->updateNextCompetitionIndex();
    SeasonCalendar * calendar = simulationSave->getActualSeason()->getActualCalendar();
    CompetitionInfo * next = simulationSave->getNextCompetition();
    if(calendar != nullptr && next != nullptr && next->getHill() != nullptr){
        int competitionIndex = calendar->getCompetitionsReference().indexOf(next);
        int indexInCalendar = SeasonCalendar::getCompetitionMainIndex(calendar->getCompetitionsReference(), next);
        QString text = QString::number(competitionIndex + 1) + ". " + next->getHill()->getName() + " HS" + QString::number(next->getHill()->getHSPoint());
        ui->label_nextCompetitionIndexAndHill->setText(text);
        ui->label_hillFlag->setPixmap(CountryFlagsManager::getFlagPixmap(next->getHill()->getCountryCode().toLower()).scaled(ui->label_hillFlag->size()));
        switch(next->getSerieType())
        {
        case CompetitionInfo::Competition:
            ui->label_serieTypeAndTrainingIndex->setText(tr("Konkurs"));
            break;
        case CompetitionInfo::Qualifications:
            ui->label_serieTypeAndTrainingIndex->setText(tr("Kwalifikacje"));
            break;
        case CompetitionInfo::TrialRound:
            ui->label_serieTypeAndTrainingIndex->setText(tr("Seria próbna"));
            break;
        case CompetitionInfo::Training:
            CompetitionInfo * mainCompetition = SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), indexInCalendar);
            qDebug()<<mainCompetition->getShortSerieTypeText()<<" MAIN! "<<calendar->getCompetitionsReference().indexOf(mainCompetition);
            int trainingIndex = mainCompetition->getTrainingsReference().indexOf(next);
            qDebug()<<"competitionIndex: "<<competitionIndex;
            qDebug()<<"indexInCalendar; "<<indexInCalendar;
            qDebug()<<"trngs "<<mainCompetition->getTrainingsReference();
            qDebug()<<"next "<<next;
            qDebug()<<"trainingIndex: "<<trainingIndex;
            ui->label_serieTypeAndTrainingIndex->setText(tr("Trening ") + QString::number(trainingIndex + 1));
            break;
        }
        calendarTableModel->setDontModifyBefore(simulationSave->getNextCompetitionIndex());
        emit calendarTableModel->dataChanged(calendarTableModel->index(0, 0), calendarTableModel->index(calendarTableModel->rowCount() - 1, calendarTableModel->columnCount() - 1));
    }
    else{
        ui->label_nextCompetitionIndexAndHill->setText("BRAK");
        ui->label_hillFlag->setText("");
        ui->label_serieTypeAndTrainingIndex->setText("");
    }
}

void SimulationSaveManagerWindow::setupClassificationsComboBox()
{
    disconnectComboBox();
    ui->comboBox_classifications->clear();
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr){
    for(auto & classification : simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference())
    {
        QString text = classification->getName();
        ui->comboBox_classifications->addItem(text);
    }
    }
    connectComboBox();
}

void SimulationSaveManagerWindow::connectComboBox()
{
    connect(ui->comboBox_classifications, qOverload<int>(&QComboBox::currentIndexChanged), this, &SimulationSaveManagerWindow::whenClassificationsComboBoxIndexChanged);
}

void SimulationSaveManagerWindow::disconnectComboBox()
{
    disconnect(ui->comboBox_classifications, qOverload<int>(&QComboBox::currentIndexChanged), this, &SimulationSaveManagerWindow::whenClassificationsComboBoxIndexChanged);
}

void SimulationSaveManagerWindow::setupNextSeasonConfigButton()
{
    ui->pushButton_competitionConfig->setText(tr("Konfiguruj nowy sezon"));
    ui->pushButton_competitionConfig->setToolTip(QString());
    ui->pushButton_competitionConfig->setEnabled(true);
}

void SimulationSaveManagerWindow::updateCompetitionConfigButton()
{
    SeasonCalendar *calendar = simulationSave->getActualSeason()->getActualCalendar();
    if(calendar == nullptr)
    {
        ui->pushButton_competitionConfig->setText(tr("Utwórz kalendarz"));
        ui->pushButton_competitionConfig->setToolTip(tr("Przejdź do karty Kalendarz"));
        ui->pushButton_competitionConfig->setEnabled(true);
    }
    else if(calendar->getCompetitionsReference().isEmpty())
    {
        ui->pushButton_competitionConfig->setText(tr("Dodaj konkurs w kalendarzu"));
        ui->pushButton_competitionConfig->setToolTip(tr("Dodaj konkurs w aktualnym kalendarzu"));
        ui->pushButton_competitionConfig->setEnabled(false);
    }
    else if(simulationSave->getNextCompetition() != nullptr)
    {
        ui->pushButton_competitionConfig->setText(tr("Konfiguruj konkurs"));
        ui->pushButton_competitionConfig->setToolTip(QString());
        ui->pushButton_competitionConfig->setEnabled(true);
    }
    else if(checkSeasonEnd())
    {
        setupNextSeasonConfigButton();
    }
    else
    {
        ui->pushButton_competitionConfig->setText(tr("Wybierz kalendarz z konkursem"));
        ui->pushButton_competitionConfig->setToolTip(tr("Wybierz kalendarz zawierający nierozgrany konkurs"));
        ui->pushButton_competitionConfig->setEnabled(false);
    }
}

void SimulationSaveManagerWindow::configNextSeason()
{
    NewSeasonConfiguratorWindow * newSeasonConfig = new NewSeasonConfiguratorWindow(true, this);
    newSeasonConfig->setJumpers(simulationSave->getJumpersReference());
    newSeasonConfig->setHills(simulationSave->getHillsReference());
    newSeasonConfig->setCompetitionsRules(simulationSave->getCompetitionRulesReference());
    if(newSeasonConfig->exec() == QDialog::Accepted)
    {
        Season season;
        season.setSeasonNumber(simulationSave->getSeasonsReference().last().getSeasonNumber() + 1);

        simulationSave->getSeasonsReference().push_back(season);
        simulationSave->setActualSeason(&simulationSave->getSeasonsReference().last());
        simulationSave->getActualSeason()->setActualCalendar(nullptr);
        simulationSave->updateNextCompetitionIndex();
        simulationSave->saveToFile("simulationSaves/");

        ui->pushButton_competitionConfig->setText(tr("Konfiguruj konkurs"));
        ui->label_seasonNumber->setText(QString::number(simulationSave->getActualSeason()->getSeasonNumber()));

        delete calendarTableModel;
        calendarTableModel = new CalendarEditorTableModel(simulationSave->getActualSeason()->getActualCalendar(), &simulationSave->getHillsReference(), &simulationSave->getCompetitionRulesReference(), simulationSave->getNextCompetitionIndex(), this);
        /*calendarTableModel->setCalendar(&simulationSave->getActualSeason()->getActualCalendar());
        calendarTableModel->setHillsList(&simulationSave->getHillsReference());
        calendarTableModel->setRulesList(&simulationSave->getCompetitionRulesReference());
        calendarTableModel->setDontModifyBefore(simulationSave->getNextCompetitionIndex());
        */
        delete calendarEditor;
        calendarEditor = new CalendarEditorWidget(calendarTableModel, &simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference(), this);
calendarEditor->setClassificationsList(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
    ui->verticalLayout_calendar->addWidget(calendarEditor);

        classificationsListView->setClassifications(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
        classificationsListView->setupListModel();

        jumpersListView->getListView()->reset();
        hillsListView->getListView()->reset();
        rulesListView->getListView()->reset();

        fillNextCompetitionInformations();

        competitionsArchiveModel->setSeasonCompetitions(&simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference());
        classificationsArchiveModel->setSeasonClassifications(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
        ui->comboBox_archiveSeason->clear();
        for(auto & season : simulationSave->getSeasonsReference())
        {
            ui->comboBox_archiveSeason->insertItem(0, QString::number(season.getSeasonNumber()));
        }
        ui->comboBox_archiveSeason->setCurrentIndex(0);

        ui->listView_competitionsArchive->setModel(competitionsArchiveModel);
        ui->listView_classificationsArchive->setModel(classificationsArchiveModel);

        setupClassificationsComboBox();
        ui->comboBox_archiveSeason->setCurrentIndex(0);
        if(ui->comboBox_classifications->count() > 0)
            emit ui->comboBox_classifications->currentIndexChanged(0);
    }
}

bool SimulationSaveManagerWindow::checkSeasonEnd()
{
    SeasonCalendar * startCalendar = simulationSave->getActualSeason()->getActualCalendar();
    for(auto & cal : simulationSave->getActualSeason()->getCalendarsReference())
    {
        simulationSave->getActualSeason()->setActualCalendar(cal);
        simulationSave->updateNextCompetitionIndex();
        if(simulationSave->getNextCompetition() != nullptr){
            simulationSave->getActualSeason()->setActualCalendar(startCalendar);
            simulationSave->updateNextCompetitionIndex();
            return false;
        }
    }
    simulationSave->getActualSeason()->setActualCalendar(startCalendar);
    simulationSave->updateNextCompetitionIndex();
    return true;
}

void SimulationSaveManagerWindow::whenClassificationsComboBoxIndexChanged(int index)
{
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr
        && index >= 0
        && index < simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference().count()){
        classificationResultsTableView->setClassification(simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference()[index]);
        classificationResultsTableView->fillTable();
    }
}

SimulationSave *SimulationSaveManagerWindow::getSimulationSave() const
{
    return simulationSave;
}

void SimulationSaveManagerWindow::on_pushButton_competitionConfig_clicked()
{
    if(simulationSave->getActualSeason()->getActualCalendar() == nullptr)
    {
        focusCalendarTab();
        return;
    }

    if(checkSeasonEnd() == true)
    {
        configNextSeason();
    }
    else if(simulationSave->getNextCompetition() != nullptr){
        CompetitionConfigWindow * configWindow = new CompetitionConfigWindow(CompetitionConfigWindow::SeasonCompetition, this, simulationSave);
        configWindow->setModal(true);

        if(Classification::getSpecificTypeClassifications(simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference(), simulationSave->getNextCompetition()->getRulesPointer()->getCompetitionType()).count() > 0)
            configWindow->setClassificationsComboBoxIndex(1);

        if(configWindow->exec() == QDialog::Accepted)
        {
            CompetitionInfo * competition = simulationSave->getNextCompetition();
            int type = competition->getRulesPointer()->getCompetitionType();

            for (auto & team : configWindow->getCompetitionTeamsReference())
                Team::cutTeamJumpers(&team, competition->getRulesPointer()->getJumpersInTeamCount());

            AbstractCompetitionManager * competitionManager = nullptr;
            if(type == CompetitionRules::Individual)
                competitionManager = new IndividualCompetitionManager();
            else if(type == CompetitionRules::Team)
                competitionManager = new TeamCompetitionManager();

            if(competition->getQualifyingCompetitionsReference().count() > 0)
                competitionManager->setAltQualifiersLimit(competition->getQualifyingCompetitionsReference()[0]->getRulesPointer()->getRoundsReference()[0].getCount());

            competitionManager->setCompetitionInfo(competition);
            competitionManager->setCompetitionRules(competition->getRulesPointer());
            competitionManager->setResults(&competition->getResultsReference());
            competitionManager->setActualGate(configWindow->getStartingGateFromInput());
            competitionManager->setActualRound(1);
            if(type == CompetitionRules::Team)
                static_cast<TeamCompetitionManager *>(competitionManager)->setActualGroup(1);
            competitionManager->setBaseDSQProbability(configWindow->getBaseDSQProbability());
            competitionManager->setWindGenerationSettings(configWindow->getWindGeneratorSettingsEditor()->getWindsGenerationSettingsFromInputs());

            if(type == CompetitionRules::Individual){
                if(competition->getRulesPointer()->getRoundsReference()[0].getKO())
                {
                    competition->getRoundsKOGroupsReference().push_back(configWindow->getSeasonCompetitionGroups());
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&competition->getRoundsKOGroupsReference().last());
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(KOGroup::getJumpersFromGroups(&competition->getRoundsKOGroupsReference().first()));
                }
                else{
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(configWindow->getSeasonCompetitionJumpers());
                    competition->getRoundsKOGroupsReference().push_back(QVector<KOGroup>());
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&competition->getRoundsKOGroupsReference().last());
                }
                competition->setStartList(static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().first());

            }
            else if(type == CompetitionRules::Team){
                competition->setTeams(configWindow->getCompetitionTeamsReference());
                static_cast<TeamCompetitionManager *>(competitionManager)->getRoundsTeamsReference().push_back(MyFunctions::convertToVectorOfPointers(&competition->getTeamsReference()));
            }

                if(type == CompetitionRules::Team)
                dynamic_cast<TeamCompetitionManager *>(competitionManager)->setupStartListStatusesForActualRound(true);
            else
                competitionManager->setupStartListStatusesForActualRound();
            competitionManager->setActualStartListIndex(0);
            competitionManager->getRoundsStartingGatesReference().push_back(competitionManager->getActualGate());

            CompetitionManagerWindow * managerWindow = new CompetitionManagerWindow(competitionManager, this);
            managerWindow->setInrunSnowGenerator(InrunSnowGenerator(configWindow->getInrunSnowGeneratorSettingsWidget()->getBase(), configWindow->getInrunSnowGeneratorSettingsWidget()->getDeviation()));
            if(managerWindow->exec() == QDialog::Accepted)
            {
                competition->setPlayed(true);
                for(auto & classification : competition->getClassificationsReference())
                {
                    for(auto & competitionSingleResult : competition->getResultsReference().getResultsReference())
                    {
                        if(classification->getClassificationType() == Classification::Individual)
                        {
                            int count = 1;
                            if(competitionSingleResult.getTeam() != nullptr)
                                count = competitionSingleResult.getTeam()->getJumpersCount();

                            for(int i=0; i<count; i++)
                            {
                                Jumper * jumper = competitionSingleResult.getJumper();
                                if(competitionSingleResult.getTeam() != nullptr)
                                    jumper = competitionSingleResult.getTeam()->getJumpersReference()[i];

                                ClassificationSingleResult * classificationResult = classification->getResultOfIndividualJumper(jumper);
                                if(classificationResult == nullptr){
                                    classification->getResultsReference().push_back(new ClassificationSingleResult(classification, jumper));
                                    classificationResult = classification->getResultOfIndividualJumper(jumper);
                                }
                                if(MyFunctions::vectorContains(classificationResult->getCompetitionsResultsReference(), &competition->getResultsReference()) == false){
                                    classificationResult->getCompetitionsResultsReference().push_back(&competition->getResultsReference());
                                    classificationResult->updateSingleResults();
                                    classificationResult->updatePointsSum();
                                }
                            }
                        }
                        else if(classification->getClassificationType() == Classification::Team)
                        {
                            QString countryCode;
                            if(competitionSingleResult.getJumper() != nullptr)
                                countryCode = competitionSingleResult.getJumper()->getCountryCode();
                            else if(competitionSingleResult.getTeam() != nullptr)
                                countryCode = competitionSingleResult.getTeam()->getCountryCode();

                            ClassificationSingleResult * classificationResult = classification->getResultOfTeam(countryCode);
                            if(classificationResult == nullptr){
                                classification->getResultsReference().push_back(new ClassificationSingleResult(classification, countryCode));
                                classificationResult = classification->getResultOfTeam(countryCode);
                            }
                            if(MyFunctions::vectorContains(classificationResult->getCompetitionsResultsReference(), &competition->getResultsReference()) == false){
                                classificationResult->getCompetitionsResultsReference().push_back(&competition->getResultsReference());
                                classificationResult->updateSingleResults();
                                classificationResult->updatePointsSum();
                            }
                        }
                    }
                    classification->sortInDescendingOrder();
                }
                if(ui->comboBox_classifications->count() > 0)
                {
                    emit ui->comboBox_classifications->currentIndexChanged(ui->comboBox_classifications->currentIndex());
                }

                for(auto & season : simulationSave->getSeasonsReference())
                    for(auto & comp : season.getActualCalendar()->getCompetitionsReference())
                    {
                        comp->getResultsReference().fixSingleResultsJumps();
                    }

                simulationSave->updateNextCompetitionIndex();
                fillNextCompetitionInformations();
                ui->listView_competitionsArchive->reset();
                simulationSave->saveToFile("simulationSaves/");

                competition->saveToCsvFile("results/season-competitions/csv/", simulationSave->getName() + QString(" (%1)" ).arg(simulationSave->getActualSeason()->getActualCalendar()->getName()) + QString::number(simulationSave->getActualSeason()->getSeasonNumber()) + "-" + QString::number(simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference().indexOf(competition)) + " " + competition->getHill()->getHillText() + " (" + competition->getShortSerieTypeText() + ")" + ".csv");

                if(checkSeasonEnd())
                {
                    QMessageBox::information(this, tr("Koniec sezonu"), tr("Sezon dobiegł końca! Aby skonfigurować kolejny sezon, wciśnij odpowiedni przycisk w oknie."), QMessageBox::Ok);
                    setupNextSeasonConfigButton();
                }
                else if(simulationSave->getNextCompetition() == nullptr)
                    QMessageBox::information(this, tr("Koniec kalendarza"), tr("Nie ma więcej zaplanowanych konkursów w tym kalendarzu. Czas na rozegranie konkursów z innych kalendarzy!"), QMessageBox::Ok);
            }
        }
    }
    else
    {
        QMessageBox::information(this, tr("Koniec kalendarza"), tr("Nie ma więcej zaplanowanych konkursów w tym kalendarzu. Czas na rozegranie konkursów z innych kalendarzy!"), QMessageBox::Ok);
    }
}

void SimulationSaveManagerWindow::on_pushButton_saveToFile_clicked()
{
    if(simulationSave->saveToFile("simulationSaves/"))
        QMessageBox::information(this, tr("Zapis do pliku"), tr("Pomyślnie zapisano aktualny zapis symulacji do pliku"), QMessageBox::Ok);
}

void SimulationSaveManagerWindow::on_pushButton_saveAsCopy_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Otwórz folder"));
    if(dir.isEmpty())
        return;
    bool ok;
    QString name = QInputDialog::getText(this, tr("Nazwa kopii zapisu symulacji"), tr("Za pomocą jakiej etykiety znamienity pan/pani zamierza oznaczyć sporządzony w bliskiej przyszłości dokument przechowujący kopię bieżącego zapisu symulacji dyscypliny olimpijskiej jaką są skoki narciarskie w pamięci aktualnie używanego komputera?"), QLineEdit::Normal, simulationSave->getName() + "copy", &ok);
    name = name.trimmed();
    if(!ok)
        return;
    if(name.isEmpty() || name.contains('/') || name.contains('\\'))
    {
        QMessageBox::warning(this, tr("Nieprawidłowa nazwa kopii"),
                             tr("Wpisz nazwę pliku, która nie zawiera ukośników."), QMessageBox::Ok);
        return;
    }
    if(!simulationSave->saveToFile(dir + "/", name))
        return;
    qDebug()<<"dir: "<<dir;
    qDebug()<<"name "<<name;
    QMessageBox::information(this, tr("Kopia zapisu symulacji"), tr("Pomyślnie zapisano kopię aktualnego zapisu symulacji do pliku"), QMessageBox::Ok);
}

void SimulationSaveManagerWindow::on_pushButton_repairDatabase_clicked()
{
    SeasonCalendar * calendar = simulationSave->getActualSeason()->getActualCalendar();
    if(calendar == nullptr || simulationSave->getHillsReference().isEmpty())
    {
        QMessageBox::warning(this, tr("Naprawa bazy danych"),
                             tr("Najpierw utwórz i wybierz kalendarz oraz upewnij się, że zapis zawiera co najmniej jedną skocznię."), QMessageBox::Ok);
        return;
    }

    if(QMessageBox::question(this, tr("Naprawa bazy danych"),
                             tr("Funkcja poprawi powiązania kalendarza, a następnie zapisze symulację. Kontynuować?"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;

    QProgressDialog dialog;
    dialog.setStyleSheet("QProgressDialog{background-color: white; color: black;}");
    dialog.setMinimum(0);
    dialog.setMaximum(11);
    dialog.setValue(0);
    dialog.setMinimumDuration(0);
    dialog.setWindowTitle(QObject::tr("Naprawia zapisów symulacji "));
    //dialog.setLabelText(QString(QObject::tr("Postęp naprawy zapisu symulacji: %1 z %2")).arg(QString::number(dialog.value())).arg(QString::number(dialog.maximum())));
    dialog.setWindowModality(Qt::WindowModal);
    dialog.show();

    calendar->fixCompetitionsClassifications();
    dialog.setValue(dialog.value() + 1);
    QCoreApplication::processEvents();
    calendar->fixAdvancementClassifications();
    dialog.setValue(dialog.value() + 1);
    QCoreApplication::processEvents();
    calendar->fixAdvancementCompetitions();
    dialog.setValue(dialog.value() + 1);
    QCoreApplication::processEvents();
    calendar->fixCompetitionsHills(&simulationSave->getHillsReference(), simulationSave->getHillsReference().first());
    dialog.setValue(dialog.value() + 1);
    QCoreApplication::processEvents();
    simulationSave->repairDatabase();
    dialog.setValue(dialog.value() + 1);
    QCoreApplication::processEvents();
    const bool saved = simulationSave->saveToFile("simulationSaves/");
    dialog.setValue(dialog.value() + 1);
    QCoreApplication::processEvents();

    if(saved)
        QMessageBox::information(this, tr("Naprawa bazy danych"), tr("Naprawiono bazę danych tego zapisu symulacji i zapisano do pliku."), QMessageBox::Ok);
}


void SimulationSaveManagerWindow::on_comboBox_archiveSeason_currentIndexChanged(int index)
{
    if(index >= 0){
        Season * season = &simulationSave->getSeasonsReference()[simulationSave->getSeasonsReference().count() - 1 - index];

        ui->comboBox_archiveCalendar->clear();
        for(auto & c : season->getCalendarsReference())
        {
            ui->comboBox_archiveCalendar->addItem(c->getName());
        }
        if(simulationSave->getActualSeason()->getActualCalendar())
            ui->comboBox_archiveCalendar->setCurrentIndex(simulationSave->getActualSeason()->getCalendarsReference().indexOf(simulationSave->getActualSeason()->getActualCalendar()));

        updateArchive();
    }
}

void SimulationSaveManagerWindow::on_comboBox_archiveCalendar_currentIndexChanged(int index)
{
    updateArchive();
}

void SimulationSaveManagerWindow::updateArchive()
{
    Season * season = &simulationSave->getSeasonsReference()[simulationSave->getSeasonsReference().count() - 1 - ui->comboBox_archiveSeason->currentIndex()];
    if(season->getCalendarsReference().count() > ui->comboBox_archiveCalendar->currentIndex() && ui->comboBox_archiveCalendar->currentIndex() > (-1)){
    SeasonCalendar * calendar = season->getCalendarsReference()[ui->comboBox_archiveCalendar->currentIndex()];

    competitionsArchiveModel->setSeasonCompetitions(&calendar->getCompetitionsReference());
    ui->listView_competitionsArchive->reset();
    classificationsArchiveModel->setSeasonClassifications(&calendar->getClassificationsReference());
    ui->listView_classificationsArchive->reset();
    }
    else
    {
        emptyArchiveCompetitions.clear();
        emptyArchiveClassifications.clear();
        competitionsArchiveModel->setSeasonCompetitions(&emptyArchiveCompetitions);
        ui->listView_competitionsArchive->reset();
        classificationsArchiveModel->setSeasonClassifications(&emptyArchiveClassifications);
        ui->listView_classificationsArchive->reset();
    }
}

void SimulationSaveManagerWindow::on_listView_competitionsArchive_doubleClicked(const QModelIndex &index)
{
    CompetitionInfo * competition = competitionsArchiveModel->getSeasonCompetitions()->at(index.row());
    int compType = competition->getRulesPointer()->getCompetitionType();

    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Wyniki konkursu"));
    dialog->resize(1400, 800);
    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    dialog->setLayout(mainLayout);

    QTableView * resultsTableView = new QTableView(this);
    ResultsTableModel * resultsModel = new ResultsTableModel(competition->getRulesPointer()->getCompetitionType(), &competition->getResultsReference(), nullptr, this);
    resultsTableView->setModel(resultsModel);
    resultsTableView->resizeColumnsToContents();
    if(compType == CompetitionRules::Individual)
        mainLayout->addWidget(resultsTableView);

    QTreeView * teamResultsTreeView = new QTreeView(this);
    TeamResultsTreeModel * teamResultsModel = new TeamResultsTreeModel(nullptr, this);
    if(compType == CompetitionRules::Team){
        QVector<Team *> teams = MyFunctions::convertToVectorOfPointers(&competition->getTeamsReference());
        teamResultsModel->setTeams(&teams);
        teamResultsModel->setResults(&competition->getResultsReference());
        teamResultsModel->setupTreeItems();
        teamResultsTreeView->setModel(teamResultsModel);
        teamResultsTreeView->expandToDepth(0);
        mainLayout->addWidget(teamResultsTreeView);
    }

    JumperCompetitionResultsWidget * jumperResultWidget = new JumperCompetitionResultsWidget(this);
    mainLayout->addWidget(jumperResultWidget);
    mainLayout->setStretch(0, 1);

    QPushButton * button = new QPushButton(tr("Wyślij webhooka"), this);
    button->setFixedWidth(150);
    button->setFont(QFont("Segoe UI", 12));
    button->setStyleSheet("color: black");
    mainLayout->addWidget(button);
    connect(button, &QPushButton::clicked, this, [competition](){
        competition->sendResultsWebhook(nullptr);
    });

    connect(resultsTableView, &QListView::doubleClicked, this, [jumperResultWidget, competition](const QModelIndex index){
        jumperResultWidget->setJumperResult(competition->getResultsReference().getResultByIndex(index.row()));
        jumperResultWidget->getJumperResult()->setCompetition(competition);
        jumperResultWidget->fillWidget();
    });
    connect(teamResultsTreeView, &QTreeView::doubleClicked, this, [jumperResultWidget, competition, teamResultsModel](const QModelIndex index){
        TreeItem * item = static_cast<TreeItem *>(index.internalPointer());
        if(item->getParentItem() == teamResultsModel->getRootItem())
            return;
        int teamIndex = item->getParentItem()->row();
        int jumperIndex = item->row();
        qDebug()<<"teamIndex "<<teamIndex;
        qDebug()<<"jumperIndex "<<jumperIndex;
        //qDebug()<<competition<<" "<<jumperResultWidget->getJumperResult()->getCompetition();
        jumperResultWidget->setJumperResult(&competition->getResultsReference().getResultsReference()[teamIndex].getTeamJumpersResultsReference()[jumperIndex]);
        jumperResultWidget->getJumperResult()->setCompetition(competition);
        for(auto & jmp : jumperResultWidget->getJumperResult()->getJumpsReference())
            jmp.setCompetition(competition);
        jumperResultWidget->fillWidget();
    });

    dialog->show();
}


void SimulationSaveManagerWindow::on_listView_classificationsArchive_doubleClicked(const QModelIndex &index)
{
    Classification * classification = classificationsArchiveModel->getSeasonClassifications()->at(index.row());
    archiveClassificationResults->setClassification(classification);
    archiveClassificationResults->fillTable();
}

void SimulationSaveManagerWindow::on_pushButton_jumperStats_clicked()
{
    bool condition = false;
    for(auto & cal : simulationSave->getSeasonsReference().first().getCalendarsReference())
    {
        for(auto & comp : cal->getCompetitionsReference())
            if(comp->getPlayed()){
                condition = true;
                break;
            }
    }
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr && condition)
    {
    if(jumpersListView->getListView()->selectionModel()->selectedRows().count() == 0)
    {
        QMessageBox::warning(this, tr("Statystyki skoczka"), tr("Najpierw zaznacz jakiegoś zawodnika!"), QMessageBox::Ok);
    }
    else
    {
        int index = jumpersListView->getListView()->selectionModel()->selectedRows().first().row();
        Jumper * jumper = simulationSave->getJumpersReference()[index];
        JumperStatsWindow statsWindow;
        statsWindow.setSave(simulationSave);
        statsWindow.setupComboBox();
        statsWindow.setJumper(jumper);
        statsWindow.getRangeComboBoxes()->setSeasonsList(&simulationSave->getSeasonsReference());
        statsWindow.getRangeComboBoxes()->setMergeCalendars(statsWindow.getMergeCalendarsCheckBox()->isChecked());
        statsWindow.getRangeComboBoxes()->setupComboBoxes();
        statsWindow.getRangeComboBoxes()->setupConnections();
        statsWindow.getClassificationsCheckBoxes()->setClassificationsList(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
        statsWindow.getClassificationsCheckBoxes()->setupCheckBoxes();
        statsWindow.setupConnections();
        statsWindow.getShowFormCheckBox()->setChecked(simulationSave->getShowForm());
        if(statsWindow.getCalendarComboBox()->count() > 0){
            statsWindow.getCalendarComboBox()->setCurrentIndex(simulationSave->getActualSeason()->getCalendarsReference().indexOf(simulationSave->getActualSeason()->getActualCalendar()));
            emit statsWindow.getCalendarComboBox()->currentIndexChanged(statsWindow.getCalendarComboBox()->currentIndex());
        }
        else
            statsWindow.fillWindow();
        if(statsWindow.exec() == QDialog::Accepted)
        {

        }
    }
    }
    else
    {
    QMessageBox * msgBox = new QMessageBox(this);
    msgBox->setText(tr("Statystyki zawodnika"));
    msgBox->setText(tr("Tępy kretynie! Jakim cudem chcesz zobaczyć statystyki, jeśli nie rozegrałeś jeszcze żadnego konkursu?"));
    QPushButton *sorryButton = msgBox->addButton(tr("Przepraszam, to się więcej nie powtórzy"), QMessageBox::RejectRole);
    QPushButton *showButton = msgBox->addButton(tr("Pokaż statystyki mimo to"), QMessageBox::AcceptRole);
    msgBox->show();

    connect(msgBox, &QMessageBox::buttonClicked, this, [sorryButton, showButton, msgBox](QAbstractButton * btn){
        if(btn == showButton)
        {
            QDesktopServices::openUrl(QUrl("https://pl.wikipedia.org/wiki/Jaynagar_Majilpur"));
        }
        msgBox->accept();
    });
    }
}

void SimulationSaveManagerWindow::on_pushButton_clicked()
{
    bool condition = false;
    for(auto & cal : simulationSave->getSeasonsReference().first().getCalendarsReference())
    {
        for(auto & comp : cal->getCompetitionsReference())
            if(comp->getPlayed()){
                condition = true;
                break;
            }
    }
    if(simulationSave->getActualSeason()->getActualCalendar() != nullptr && condition)
    {
    SimulationRatingsWindow statsWindow(simulationSave, this);
    statsWindow.getClassificationsCheckBoxes()->setClassificationsList(&simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference());
    statsWindow.getClassificationsCheckBoxes()->setupCheckBoxes();
    statsWindow.setFilteredJumpers(simulationSave->getJumpersReference());
    statsWindow.setupComboBox();
    statsWindow.setupConnections();
    statsWindow.getRangeComboBoxes()->setSeasonsList(&simulationSave->getSeasonsReference());
    statsWindow.getRangeComboBoxes()->setMergeCalendars(statsWindow.getMergeCalendarsCheckBox()->isChecked());
    statsWindow.getRangeComboBoxes()->setupComboBoxes();
    statsWindow.getRangeComboBoxes()->setupConnections();
    statsWindow.getShowFormCheckBox()->setChecked(simulationSave->getShowForm());  
    if(statsWindow.getCalendarComboBox()->count() > 0){
        statsWindow.getCalendarComboBox()->setCurrentIndex(simulationSave->getActualSeason()->getCalendarsReference().indexOf(simulationSave->getActualSeason()->getActualCalendar()));
        emit statsWindow.getCalendarComboBox()->currentIndexChanged(statsWindow.getCalendarComboBox()->currentIndex());
    }
    else
        statsWindow.fillWindow();
    if(statsWindow.exec() == QDialog::Accepted)
    {

    }
    }
    else
    {
    QMessageBox * msgBox = new QMessageBox(this);
    msgBox->setText(tr("Statystyki zapisu"));
    msgBox->setText(tr("Tępy kretynie! Jakim cudem chcesz zobaczyć statystyki, jeśli nie rozegrałeś jeszcze żadnego konkursu?"));
    QPushButton *sorryButton = msgBox->addButton(tr("Przepraszam, to się więcej nie powtórzy"), QMessageBox::RejectRole);
    QPushButton *showButton = msgBox->addButton(tr("Pokaż statystyki mimo to"), QMessageBox::AcceptRole);
    msgBox->show();

    connect(msgBox, &QMessageBox::buttonClicked, this, [sorryButton, showButton, msgBox](QAbstractButton * btn){
        if(btn == showButton)
        {
            QDesktopServices::openUrl(QUrl("https://pl.wikipedia.org/wiki/Jaynagar_Majilpur"));
        }
        msgBox->accept();
    });
    }
}

void SimulationSaveManagerWindow::on_pushButton_formGenerator_clicked()
{
    JumpersFormGeneratorConfigWindow window(this);
    window.getTableModel()->setGeneratorsSettings(JumperFormGeneratorSettings::constructJumperFormGeneratorsSettingsVector(simulationSave->getJumpersReference()));
    window.setSave(simulationSave);
    window.updateTable();
    window.on_doubleSpinBox_formBonus_editingFinished();
    window.on_doubleSpinBox_minForm_editingFinished();
    window.on_doubleSpinBox_maxForm_editingFinished();
    if(window.exec() == QDialog::Accepted)
    {

    }
}


void SimulationSaveManagerWindow::on_checkBox_showForm_stateChanged(int arg1)
{
    const bool showForm = arg1 == Qt::Checked;
    simulationSave->setShowForm(showForm);
    jumperEditor->setShowForm(showForm);
    saveSimulationSettings();
}


void SimulationSaveManagerWindow::on_checkBox_compactSaveFile_stateChanged(int arg1)
{
    simulationSave->setSaveFileSizeReduce(arg1 == Qt::Checked);
    saveSimulationSettings();
}

void SimulationSaveManagerWindow::on_lineEdit_calendarName_editingFinished()
{
    if(calendarsListView->getListView()->selectionModel()->selectedRows(0).count() > 0)
    {
        SeasonCalendar * c = simulationSave->getActualSeason()->getCalendarsReference()[calendarsListView->getListView()->selectionModel()->selectedRows(0).first().row()];
        c->setName(ui->lineEdit_calendarName->text());
        calendarsListView->setupListModel();
    }
}


void SimulationSaveManagerWindow::on_checkBox_showInstability_stateChanged(int arg1)
{
    simulationSave->setShowInstability(arg1 == Qt::Checked);
    saveSimulationSettings();
}

void SimulationSaveManagerWindow::saveSimulationSettings()
{
    simulationSave->saveToFile("simulationSaves/");
}
