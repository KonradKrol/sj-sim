#include "CompetitionManagerWindow.h"
#include "ui_CompetitionManagerWindow.h"
#include "../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/MyRandom.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/GlobalSimulationSettings.h"
#include "../../global/GlobalAppSettings.h"
#include "../../utilities/functions.h"
#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/InrunSnowGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/KOSystem/KOGroupsListView.h"
#include "../JumpManipulation/JumpManipulatorConfigWindow.h"
#include <QInputDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStringListModel>
#include <string>
#include <QPropertyAnimation>
#include <QGraphicsItem>
#include "ui_JumperCompetitionResultsWidget.h"
#include "ui_JumpDataDetailedInfoWindow.h"
#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"

extern const QString appVersion;

CompetitionManagerWindow::CompetitionManagerWindow(AbstractCompetitionManager *manager, QWidget *parent, bool singleCompetition) :
    QDialog(parent),
    ui(new Ui::CompetitionManagerWindow),
    manager(manager),
    KOManager(nullptr),
    singleCompetition(singleCompetition),
    teamResultsTreeView(nullptr)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    ui->checkBox_liveCompetition->setChecked(GlobalAppSettings::get()->getLiveCompetition());

    if(manager != nullptr){
        type = manager->getType();
        ui->spinBox_actualGate->setValue(manager->getActualGate());
    }

    ui->pushButton_goToNext->hide();

    startListModel = new StartListModel(this->manager, this);
    startListModel->setType(getType());
    qDebug()<<"hau ahu piesek";
    qDebug()<<this->manager->getStartListStatusesReference().count();
    startListModel->setStartListStatuses(&this->manager->getStartListStatusesReference());
    if(dynamic_cast<IndividualCompetitionManager *>(manager) != nullptr){
        if(dynamic_cast<IndividualCompetitionManager *>(manager)->getRoundsKOGroupsReference()[0]->count() > 0){
            startListModel->setKOGroups(dynamic_cast<IndividualCompetitionManager *>(manager)->getRoundsKOGroupsReference()[manager->getActualRound() - 1]);

            KOManager = new KORoundManager(startListModel->getKOGroups(), manager->getResults(), &manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1], dynamic_cast<IndividualCompetitionManager *>(manager));
            KOManager->updateJumpersSortedByResults();
            if(manager->getCompetitionRules()->getRoundsReference().count() > manager->getActualRound())
                KOManager->setLuckyLosersCount(manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getCount() - (KOManager->getGroups()->count() * KOManager->getRoundInfo()->getAdvancingFromKOGroup()));
            else
                KOManager->setLuckyLosersCount(0);
            dynamic_cast<IndividualCompetitionManager *>(manager)->setKOManager(KOManager);
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->setIndManager(dynamic_cast<IndividualCompetitionManager *>(manager));
            KOGroupsResultsModel = new KOGroupResultsTableModel(KOManager, &dynamic_cast<IndividualCompetitionManager *>(manager)->getRoundsKOGroupsReference()[0]->first(), this);
            ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
            ui->tableView_KOGroupResults->resizeColumnsToContents();
        }
        else
        {
            ui->tableView_KOGroupResults->hide();
        }
    }
    else
    {
        ui->tableView_KOGroupResults->hide();
    }


    ui->label_nameAndSurname->setText(this->manager->getActualJumper()->getNameAndSurname());
    ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(this->manager->getActualJumper()->getCountryCode().toLower()).scaled(ui->label_flag->size()));
    ui->label_nextJumperIMG->setPixmap(this->manager->getActualJumper()->getImagePixmap().scaled(ui->label_nextJumperIMG->size()));

    ui->label_personalBest->setText(QString::number(manager->getActualJumper()->getPersonalBest(), 'f', 1));

    if(this->manager->getCompetitionInfo()->getHill()->getHillType() == Hill::Flying){
        ui->label_personalBest->show();
        ui->label_personalBestContent->show();
    }
    else
    {
        ui->label_personalBest->hide();
        ui->label_personalBestContent->hide();
    }
    ui->label_record->setText(tr("Rekord: ") + QString::number(manager->getCompetitionInfo()->getHill()->getRecord(), 'f', 1));

    ui->label_lastRoundPosition->hide();
    connect(this->manager, &AbstractCompetitionManager::actualJumperChanged, this, [this](){
        if(this->manager->getActualStartListIndex() < this->manager->getStartListStatusesReference().count()){
            ui->label_nameAndSurname->setText(this->manager->getActualJumper()->getNameAndSurname());
            ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(this->manager->getActualJumper()->getCountryCode().toLower()).scaled(ui->label_flag->size()));
            ui->label_nextJumperIMG->setPixmap(this->manager->getActualJumper()->getImagePixmap().scaled(ui->label_nextJumperIMG->size()));
            ui->label_personalBest->setText(QString::number(this->manager->getActualJumper()->getPersonalBest(), 'f', 1));

            if(type == CompetitionRules::Individual && this->manager->getActualRound() > 1){
                int positionInLastRound = this->manager->getResults()->getHashWithJumpersPositions(&static_cast<IndividualCompetitionManager*>(this->manager)->getActualRoundJumpersReference(), this->manager->getActualRound() - 1).value(this->manager->getActualJumper());
                ui->label_lastRoundPosition->setText(QString::number(positionInLastRound));
                ui->label_lastRoundPosition->show();
                ui->label_lastRoundPositionContent->show();
                ui->label_lastRoundPositionContent->setText(QString(" po %1 serii").arg(this->manager->getActualRound() - 1));
            }
            else
            {
                ui->label_lastRoundPositionContent->hide();
                ui->label_lastRoundPosition->hide();
            }

        }
    });

    if(singleCompetition == false){
        virtualClassificationTableView = new ClassificationResultsTableView(false, nullptr, this);
        virtualClassificationTableView->getTableView()->setFixedSize(250, 172);
        setupVirtualClassificationComboBox();
        ui->verticalLayout_virtualClassificationListView->addWidget(virtualClassificationTableView);
        emit ui->comboBox_virtualClassification->currentIndexChanged(ui->comboBox_virtualClassification->currentIndex());
    }
    else
    {
        ui->comboBox_virtualClassification->hide();
    }

    ui->listView_startList->setModel(startListModel);

    if(type == CompetitionRules::Team){
        teamResultsTreeModel = new TeamResultsTreeModel(dynamic_cast<TeamCompetitionManager *>(manager));
        teamResultsTreeView = new QTreeView();
        teamResultsTreeView->setModel(teamResultsTreeModel);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->horizontalLayout_main->insertWidget(ui->horizontalLayout_main->count() - 2, teamResultsTreeView); //jako przedostatni

        connect(teamResultsTreeView, &QTreeView::doubleClicked, this, [this](const QModelIndex & index){
            TreeItem * item = static_cast<TreeItem *>(index.internalPointer());
            if(item->getParentItem() == teamResultsTreeModel->getRootItem())
                return;
            int teamIndex = item->getParentItem()->row();
            int jumperIndex = item->row();

            jumperResultsWidget->setJumperResult(&this->manager->getResults()->getResultsReference()[teamIndex].getTeamJumpersResultsReference()[jumperIndex]);
            jumperResultsWidget->fillWidget();
        });

        ui->horizontalLayout_main->removeWidget(ui->tableView_results);
        delete ui->tableView_results;
    }
    else{
        resultsTableModel = new ResultsTableModel(getType(), manager->getResults(), manager, this);
        resultsTableModel->setStartListStatuses(&manager->getStartListStatusesReference());
        ui->tableView_results->setModel(resultsTableModel);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }


    jumperResultsWidget = new JumperCompetitionResultsWidget(this);
    jumperResultsWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    ui->verticalLayout_jumperResult->addWidget(jumperResultsWidget);

    ui->label_toBeatDistance->setText("0m");
    ui->label_toAdvancementDistance->setText("-");
    windsGenerator.setGenerationSettings(manager->getWindGenerationSettingsReference());
    setActualWinds(windsGenerator.generateWinds());
    updateAvgWindLabel();
    updateActualInrunSnowLevelLabel();

    ui->label_pointsBehindLeader->hide();
    ui->label_pointsBehindLeaderContent->hide();
    ui->label_lastRoundPosition->hide();
    ui->label_lastRoundPositionContent->hide();
    ui->label_pointsBehindLeader->setText("");

    Hill * hill = manager->getCompetitionInfo()->getHill();

    ui->label_hillNameAndHSPoint->setText(hill->getName() + " HS" + QString::number(hill->getHSPoint()));
    ui->label_hillFlag->setPixmap(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()).scaled(ui->label_hillFlag->size()));

    jumperResultsWidget->hide();

    connect(ui->spinBox_actualGate, qOverload<int>(&QSpinBox::valueChanged), this, [this, manager](){
        if(manager->getActualStartListIndex() == 0){
            manager->getRoundsStartingGatesReference().pop_back();
            manager->getRoundsStartingGatesReference().push_back(ui->spinBox_actualGate->value());
            manager->setActualGate(ui->spinBox_actualGate->value());
        }
        else{
            manager->setActualGate(ui->spinBox_actualGate->value());
            manager->updateLast10Judges();
            manager->updateToBeatLineDistance();
            updateToAdvanceDistanceLabel();
            updateToBeatDistanceLabel();
        }
    });

    action_cancelCompetition = new QAction(tr("Odwołaj konkurs"), this);
    action_cancelRound = new QAction(tr("Odwołaj aktualną rundę"), this);
    cancelActionsList = {action_cancelCompetition, action_cancelRound};
    cancelActionsMenu = new QMenu(this);
    cancelActionsMenu->setStyleSheet("QMenu{background-color: rgb(230, 230, 230); color: rgb(70, 70, 70);}");
    cancelActionsMenu->addActions(cancelActionsList);
    ui->toolButton_cancelActions->setMenu(cancelActionsMenu);

    action_autoSimulateCompetition = new QAction(tr("Auto-symulacja konkursu"), this);
    action_autoSimulateRound = new QAction(tr("Auto-symulacja serii"), this);
    action_autoSimulateJumps = new QAction(tr("Auto-symulacja skoków"), this);
    autoSimulationActionsList = {action_autoSimulateCompetition, action_autoSimulateRound, action_autoSimulateJumps};
    if(getType() == CompetitionRules::Team){
        action_autoSimulateGroup = new QAction(tr("Auto-symulacja grupy"), this);
        autoSimulationActionsList.insert(autoSimulationActionsList.count() - 2, action_autoSimulateGroup); // jako przedostatnie
    }
    autoSimulationActionsMenu = new QMenu();
    autoSimulationActionsMenu->setStyleSheet("QMenu{background-color: rgb(230, 230, 230); color: rgb(70, 70, 70);}");
    autoSimulationActionsMenu->addActions(autoSimulationActionsList);
    ui->toolButton_autoSimulationActions->setMenu(autoSimulationActionsMenu);

    connect(action_cancelCompetition, &QAction::triggered, this, &CompetitionManagerWindow::cancelCompetition);
    connect(action_cancelRound, &QAction::triggered, this, &CompetitionManagerWindow::cancelActualRound);
    connect(action_autoSimulateCompetition, &QAction::triggered, this, &CompetitionManagerWindow::autoSimulateCompetition);
    connect(action_autoSimulateRound, &QAction::triggered, this, &CompetitionManagerWindow::autoSimulateRound);
    if(getType() == CompetitionRules::Team)
        connect(action_autoSimulateGroup, &QAction::triggered, this, &CompetitionManagerWindow::autoSimulateGroup);
    connect(action_autoSimulateJumps, &QAction::triggered, this, &CompetitionManagerWindow::autoSimulateJumps);
}

CompetitionManagerWindow::~CompetitionManagerWindow()
{
    delete ui;
    for(auto & virtualClassification : virtualClassifications)
    {
        delete virtualClassification;
    }
}

void CompetitionManagerWindow::updateToBeatDistanceLabel()
{
    manager->updateToBeatLineDistance();
    ui->label_toBeatDistance->setText(QString::number(manager->getToBeatLineDistance()) + "m");
}

void CompetitionManagerWindow::updateToAdvanceDistanceLabel()
{
    manager->updateToAdvanceLineDistance();
    if(manager->getToAdvanceLineDistance() == (-1))
        ui->label_toAdvancementDistance->setText("-");
    else ui->label_toAdvancementDistance->setText(QString::number(manager->getToAdvanceLineDistance()) + "m");
}

void CompetitionManagerWindow::updatePointsToTheLeaderLabel()
{
    manager->updateActualCompetitorPointsToTheLeader();
    if(manager->getActualCompetitorPointsToTheLeader() == (-1)){
        ui->label_pointsBehindLeader->hide();
        ui->label_pointsBehindLeaderContent->hide();
    }
    else{
        ui->label_pointsBehindLeader->show();
        ui->label_pointsBehindLeaderContent->show();
        ui->label_pointsBehindLeader->setText(" + " + QString::number(manager->getActualCompetitorPointsToTheLeader()) + "pkt");
    }

}

void CompetitionManagerWindow::updateAvgWindLabel()
{
    double wind = WindsCalculator::getAveragedWind(actualWinds, manager->getCompetitionRules()->getWindAverageCalculatingType()).getStrengthToAveragedWind();
    ui->label_actualAvgWind->setText(QString::number(wind) + " m/s");
    if(wind > 0)
        ui->label->setStyleSheet("color: rgb(68, 130, 26);");
    else if(wind < 0)
        ui->label->setStyleSheet("color: rgb(145, 35, 28);");
    else ui->label->setStyleSheet("color: rgb(50, 50, 50);");
}

void CompetitionManagerWindow::updateActualInrunSnow()
{
    actualInrunSnow += MyRandom::normalDistributionRandom(0, inrunSnowGenerator.getInrunSnowDeviation() / 10);
    qDebug()<<"actt";
    if(actualInrunSnow < 0)
        actualInrunSnow = 0;
    else if(actualInrunSnow > 10)
        actualInrunSnow = 10;
}

void CompetitionManagerWindow::updateActualInrunSnowLevelLabel()
{
    if(actualInrunSnow == 0)
    {
        ui->label_snowLevel->setText(tr("Brak"));
        ui->label_snowLevel->setStyleSheet("color: rgb(145, 145, 145);");
    }
    else if(actualInrunSnow <= 1)
    {
        ui->label_snowLevel->setText(tr("Bardzo niski"));
        ui->label_snowLevel->setStyleSheet("color: rgb(109, 184, 101);");
    }
    else if(actualInrunSnow <= 2.15)
    {
        ui->label_snowLevel->setText(tr("Niski"));
        ui->label_snowLevel->setStyleSheet("color: rgb(135, 179, 114);");
    }
    else if(actualInrunSnow <= 3.3)
    {
        ui->label_snowLevel->setText(tr("Dosyć niski"));
        ui->label_snowLevel->setStyleSheet("color: rgb(147, 173, 120);");
    }
    else if(actualInrunSnow <= 4.6)
    {
        ui->label_snowLevel->setText(tr("Poniżej średniej"));
        ui->label_snowLevel->setStyleSheet("color: rgb(168, 184, 132);");
    }
    else if(actualInrunSnow <= 5.4)
    {
        ui->label_snowLevel->setText(tr("Średni"));
        ui->label_snowLevel->setStyleSheet("color: rgb(212, 209, 133);");
    }
    else if(actualInrunSnow <= 6.4)
    {
        ui->label_snowLevel->setText(tr("Powyżej średniej"));
        ui->label_snowLevel->setStyleSheet("color: rgb(232, 217, 142);");
    }
    else if(actualInrunSnow <= 7.2)
    {
        ui->label_snowLevel->setText(tr("Dość wysoki"));
        ui->label_snowLevel->setStyleSheet("color: rgb(227, 185, 129);");
    }
    else if(actualInrunSnow <= 8.1)
    {
        ui->label_snowLevel->setText(tr("Wysoki"));
        ui->label_snowLevel->setStyleSheet("color: rgb(222, 160, 115);");
    }
    else if(actualInrunSnow <= 9.2)
    {
        ui->label_snowLevel->setText(tr("Bardzo wysoki"));
        ui->label_snowLevel->setStyleSheet("color: rgb(209, 113, 98);");
    }
    else if(actualInrunSnow <= 10)
    {
        ui->label_snowLevel->setText(tr("Ekstremalny"));
        ui->label_snowLevel->setStyleSheet("color: rgb(173, 55, 55);");
    }
}

void CompetitionManagerWindow::disableCompetitionManagementButtons()
{
    ui->pushButton_coachGate->setDisabled(true);
    ui->pushButton_generateNewWinds->setDisabled(true);
    ui->pushButton_jump->setDisabled(true);
    ui->pushButton_manipulateJump->setDisabled(true);
    ui->pushButton_windsGeneratorSettings->setDisabled(true);
}

void CompetitionManagerWindow::enableCompetitionManagementButtons()
{
    ui->pushButton_coachGate->setEnabled(true);
    ui->pushButton_generateNewWinds->setEnabled(true);
    ui->pushButton_jump->setEnabled(true);
    ui->pushButton_manipulateJump->setEnabled(true);
    ui->pushButton_windsGeneratorSettings->setEnabled(true);
}

void CompetitionManagerWindow::showMessageBoxForNextGroup()
{
    if(getType() == CompetitionRules::Team){
        QMessageBox * box = new QMessageBox(this);
        box->setStyleSheet("QMessageBox{color: black; background-color: white;}");
        box->setIcon(QMessageBox::Information);
        box->setWindowTitle(tr("Zakończenie ") + QString::number(dynamic_cast<TeamCompetitionManager *>(manager)->getActualGroup()) + tr(" grupy zawodników"));
        box->setText(tr("Aby przejść do następnej grupy zawodników, wciśnij przycisk na dole okna konkursu"));
        QPushButton *btnOk = box->addButton("OK", QMessageBox::AcceptRole);
        box->setModal(true);
        box->show();
    }
}

void CompetitionManagerWindow::setupGoToNextButtonForNextGroup()
{
    if(getType() == CompetitionRules::Team){
        TeamCompetitionManager * tmManager = dynamic_cast<TeamCompetitionManager *>(manager);
        ui->pushButton_goToNext->show();
        ui->pushButton_goToNext->setText(tr("Przejdź do ") + QString::number(tmManager->getActualGroup() + 1) + tr(" grupy"));
        disableCompetitionManagementButtons();

        QMetaObject::Connection * const connection = new QMetaObject::Connection;
        *connection = connect(ui->pushButton_goToNext, &QPushButton::clicked, this, [this, connection, tmManager](){
            tmManager->setupNextGroup();
            currentInputJumpManipulator = JumpManipulator();
            enableCompetitionManagementButtons();
            ui->pushButton_goToNext->hide();

            QObject::disconnect(*connection);
            delete connection;
        });
    }
}

void CompetitionManagerWindow::showMessageBoxForNextRound()
{
    QMessageBox * box = new QMessageBox(this);
    box->setStyleSheet("QMessageBox{color: black; background-color: white;}");
    box->setIcon(QMessageBox::Information);
    box->setWindowTitle(tr("Zakończenie ") + QString::number(manager->getActualRound()) + tr(" serii"));
    box->setText(tr("Aby przejść do następnej serii, wciśnij przycisk na dole okna konkursu"));
    QPushButton *btnOk = box->addButton("OK", QMessageBox::AcceptRole);
    box->setModal(true);
    box->show();
}

JumpManipulator CompetitionManagerWindow::getCurrentInputJumpManipulator() const
{
    return currentInputJumpManipulator;
}

void CompetitionManagerWindow::setCurrentInputJumpManipulator(const JumpManipulator &newCurrentInputJumpManipulator)
{
    currentInputJumpManipulator = newCurrentInputJumpManipulator;
}

short CompetitionManagerWindow::getType() const
{
    return type;
}

void CompetitionManagerWindow::setType(short newType)
{
    type = newType;
}

AbstractCompetitionManager *CompetitionManagerWindow::getManager() const
{
    return manager;
}

QVector<KOGroup> CompetitionManagerWindow::getManualKOGroupsFromDialogInputs()
{
    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
    QDialog * dialog = new QDialog(this);
    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    dialog->setLayout(mainLayout);
    dialog->setFixedSize(600, 820);

    QVBoxLayout * groupsListLayout = new QVBoxLayout(this);
    QComboBox * selectionComboBox = new QComboBox(this);
    selectionComboBox->setModel(new QStringListModel({tr("Dobór grup"), tr("Klasyczne"), tr("Dla dużych grup"), tr("Losowe (Z podziałem na koszyki)"), tr("Losowe")}, this));

    KOGroupsListView groupsListView = KOGroupsListView();
    QVector<Jumper *> jumpers = indManager->getFilteredJumpersForNextRound();
    QVector<KOGroup> groups = KOGroup::constructKOGroups(&manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()], &jumpers, KOGroup::Random, manager->getCompetitionInfo());
    groupsListView.setKOGroups(&groups);
    groupsListView.setJumpersList(&indManager->getActualRoundJumpersReference());
    groupsListView.fillListLayout();
    groupsListLayout->addWidget(selectionComboBox);
    groupsListLayout->addWidget(&groupsListView);

    connect(selectionComboBox, qOverload<int>(&QComboBox::activated), this, [this, & groupsListView, & jumpers, & groups](int index){
        if(index != KOGroup::Classic || (index == KOGroup::Classic || (manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getCountInKOGroup() == 2)))
        {
            if(index > 0){
                index--;
                RoundInfo * roundInfo = &manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()];
                groups = KOGroup::constructKOGroups(roundInfo, &jumpers, index, manager->getCompetitionInfo());
                groupsListView.setKOGroups(&groups);
                groupsListView.fillListLayout();
            }
        }
    });

    QVBoxLayout * submitButtonLayout = new QVBoxLayout(this);
    QPushButton * submitButton = new QPushButton("OK", this);
    submitButton->setFixedSize(111, 111);
    submitButton->setStyleSheet("QPushButton{\nborder: 1px solid rgb(0, 59, 23);\nborder-radius: 6px;\ncolor: rgb(0, 0, 0);\nbackground-color: rgb(123, 220, 144);\n}\nQPushButton:hover{\nbackground-color: rgb(153, 253, 174);\n}");
    submitButton->setFont(QFont("Ubuntu", 18));
    connect(submitButton, &QPushButton::clicked, dialog, &QDialog::accept);
    submitButtonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    submitButtonLayout->addWidget(submitButton);
    submitButtonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    mainLayout->addLayout(groupsListLayout);
    mainLayout->addSpacerItem(new QSpacerItem(50, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
    mainLayout->addLayout(submitButtonLayout);

    if(dialog->exec() == QDialog::Accepted)
    {

    }
    return groups;
}

void CompetitionManagerWindow::setupVirtualClassificationComboBox()
{
    ui->comboBox_virtualClassification->clear();
    ui->comboBox_virtualClassification->addItem(tr("Wybierz klasyfikację"));
    for(auto & cls : manager->getCompetitionInfo()->getClassificationsReference())
    {
        ui->comboBox_virtualClassification->addItem(cls->getName());
    }
}

void CompetitionManagerWindow::setupGoToNextButtonForNextRound()
{
    ui->pushButton_goToNext->show();
    ui->pushButton_goToNext->setText(tr("Przejdź do ") + QString::number(manager->getActualRound() + 1) + tr(" serii"));
    disableCompetitionManagementButtons();

    QMetaObject::Connection * const connection = new QMetaObject::Connection;
    *connection = connect(ui->pushButton_goToNext, &QPushButton::clicked, this, [this, connection](){
        //Tu będzie ten dialog z edycją grup
        int condition1 = manager->getCompetitionRules()->getRoundsReference().count() > manager->getActualRound();
        int condition2 = false;
        if(condition1)
            condition2 = manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getKO() == true && manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getKoGroupsSelectionType() == CompetitionRules::Manual;

        if(condition1 && condition2)
            dynamic_cast<IndividualCompetitionManager *>(manager)->setupNextRound(getManualKOGroupsFromDialogInputs());
        else if(dynamic_cast<IndividualCompetitionManager *>(manager) != nullptr)
            dynamic_cast<IndividualCompetitionManager *>(manager)->setupNextRound();
        else
            manager->setupNextRound();

        IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
        if(indManager != nullptr)
        {
            if(KOManager != nullptr)
            {
                delete KOManager;
                KOManager = nullptr;
                indManager->setKOManager(nullptr);
                delete KOGroupsResultsModel;
                ui->tableView_KOGroupResults->hide();
                startListModel->setKOGroups(nullptr);
            }
            if(indManager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1].getKO() == true)
            {
                KOManager = new KORoundManager(indManager->getRoundsKOGroupsReference()[manager->getActualRound() - 1], manager->getResults(), &indManager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1], dynamic_cast<IndividualCompetitionManager *>(manager));
                KOManager->updateJumpersSortedByResults();
                KOManager->setLuckyLosersCount(manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getCount() - (KOManager->getGroups()->count() * KOManager->getRoundInfo()->getAdvancingFromKOGroup()));
                KOManager->updateActualGroup(manager->getActualJumper());
                KOManager->setIndManager(dynamic_cast<IndividualCompetitionManager *>(manager));
                indManager->setKOManager(KOManager);
                KOGroupsResultsModel = new KOGroupResultsTableModel(KOManager, &dynamic_cast<IndividualCompetitionManager *>(manager)->getRoundsKOGroupsReference()[manager->getActualRound() - 1]->first(), this);
                ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
                ui->tableView_KOGroupResults->resizeColumnsToContents();
                ui->tableView_KOGroupResults->show();
                startListModel->setKOGroups(KOManager->getGroups());
            }
        }

        if(getType() == CompetitionRules::Team)
            teamResultsTreeModel->setTeams(&dynamic_cast<TeamCompetitionManager *>(manager)->getRoundsTeamsReference()[0]);
        currentInputJumpManipulator = JumpManipulator();
        enableCompetitionManagementButtons();
        ui->pushButton_goToNext->hide();

        QObject::disconnect(*connection);
        delete connection;
    });
}

void CompetitionManagerWindow::showMessageBoxForQualificationsEnd()
{
    QMessageBox * box = new QMessageBox(this);
    box->setStyleSheet("QMessageBox{color: black; background-color: white;}");
    box->setIcon(QMessageBox::Information);
    box->setWindowTitle(tr("Zakończenie kwalifikacji"));
    box->setText(tr("Aby przejść do konkursu, wciśnij przycisk na dole okna konkursu"));
    QPushButton *btnOk = box->addButton("OK", QMessageBox::AcceptRole);
    box->setModal(true);
    box->show();
    accept();
}

void CompetitionManagerWindow::setupGoToNextButtonForQualificationsEnd()
{
    ui->pushButton_goToNext->show();
    ui->pushButton_goToNext->setText(tr("Przejdź do konkursu"));
    disableCompetitionManagementButtons();

    switch(getType())
    {
    case CompetitionRules::Individual:{
        QMetaObject::Connection * const connection = new QMetaObject::Connection;
        *connection = connect(ui->pushButton_goToNext, &QPushButton::clicked, [this, connection](){
            accept();

            QObject::disconnect(*connection);
            delete connection;
        });
    }
    }
}

void CompetitionManagerWindow::showMessageBoxForCompetitionEnd()
{
    QMessageBox * box = new QMessageBox(this);
    box->setStyleSheet("QMessageBox{color: black; background-color: white;}");
    box->setIcon(QMessageBox::Information);
    switch(manager->getCompetitionInfo()->getSerieType()){
    case CompetitionInfo::Competition:{
        box->setWindowTitle(tr("Zakończenie konkursu"));
        box->setText(tr("Aby zakończyć konkurs, wciśnij przycisk na dole okna konkursu"));
        break;
    }
    case CompetitionInfo::Qualifications:{
        box->setWindowTitle(tr("Zakończenie kwalifikacji"));
        box->setText(tr("Aby zakończyć kwalifikacje, wciśnij przycisk na dole okna konkursu"));
        break;
    }
    case CompetitionInfo::TrialRound:{
        box->setWindowTitle(tr("Zakończenie serii próbnej"));
        box->setText(tr("Aby zakończyć serię próbną, wciśnij przycisk na dole okna konkursu"));
        break;
    }
    case CompetitionInfo::Training:{
        box->setWindowTitle(tr("Zakończenie treningu"));
        box->setText(tr("Aby zakończyć trening, wciśnij przycisk na dole okna konkursu"));
        break;
    }
    }
    QPushButton *btnOk = box->addButton("OK", QMessageBox::AcceptRole);
    box->setModal(true);
    box->show();
}

void CompetitionManagerWindow::setupGoToNextButtonForCompetitionEnd()
{
    ui->pushButton_goToNext->show();
    switch(manager->getCompetitionInfo()->getSerieType()){
    case CompetitionInfo::Competition:
        ui->pushButton_goToNext->setText(tr("Zakończ konkurs"));
        break;
    case CompetitionInfo::Qualifications:
        ui->pushButton_goToNext->setText(tr("Zakończ kwalifikacje"));
        break;
    case CompetitionInfo::TrialRound:
        ui->pushButton_goToNext->setText(tr("Zakończ serię próbną"));
        break;
    case CompetitionInfo::Training:
        ui->pushButton_goToNext->setText(tr("Zakończ trening"));
        break;
    }

    disableCompetitionManagementButtons();

    QMetaObject::Connection * const connection = new QMetaObject::Connection;
    *connection = connect(ui->pushButton_goToNext, &QPushButton::clicked, [this, connection](){
        accept();

        QObject::disconnect(*connection);
        delete connection;
    });
}

void CompetitionManagerWindow::autoSimulateRound()
{
    if(getType() == CompetitionRules::Individual)
        resultsTableModel->setLastJumper(nullptr);
    else
        teamResultsTreeModel->setLastTeam(nullptr);
    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
    TeamCompetitionManager * tmManager = dynamic_cast<TeamCompetitionManager *>(manager);
    while(manager->checkRoundEnd() != true && manager->checkCompetitionEnd() != true){
        setupSimulator();
        simulator.simulateJump();
        JumpData jump = simulator.getJumpData();
        jump.setCompetition(manager->getCompetitionInfo());
        if(type == CompetitionRules::Individual){
            indManager->getResults()->addJump(indManager->getActualJumper(), jump);
        }
        else if(type == CompetitionRules::Team){
            tmManager->getResults()->addJump(tmManager->getActualTeam(), jump);
            tmManager->getResults()->getResultOfTeam(tmManager->getActualTeam())->updateTeamJumpersResults();
        }

        int status = StartListCompetitorStatus::Finished;
        if(jump.getDSQ())
            status = StartListCompetitorStatus::Dsq;
        else if(jump.getDNS())
            status = StartListCompetitorStatus::Dns;
        StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setJumpStatus(status);

        bool ok = false;
        if(jump.getLanding().getType() == Landing::Fall && jump.getDistance() >= 0.95 * jump.getHill()->getHSPoint() && manager->getCompetitionRules()->getHas95HSRule()){
            ok = true;
        }
        StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setQualifiedBy95HSRule(ok);
        manager->updateLastQualifiedResult();
        manager->updateLeaderResult();
        manager->updateLast10Judges();
        manager->updateToAdvanceLineDistance();
        manager->updateToBeatLineDistance();
        if(KOManager != nullptr)
        {
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->updateJumpersSortedByResults();
            KOManager->updateStatuses();
        }
        manager->updateCompetitorsAdvanceStatuses();
        if(getType() == CompetitionRules::Team)
            teamResultsTreeModel->setTeams(&dynamic_cast<TeamCompetitionManager *>(manager)->getRoundsTeamsReference()[0]);

        setActualWinds(windsGenerator.generateWinds());
        updateActualInrunSnow();
        if(manager->isAllJumpsAreFinished() == false)
            manager->setActualJumperToNextUnfinished();
        if(type == CompetitionRules::Team && (tmManager->getActualGroup() != tmManager->getCompetitionRules()->getJumpersInTeamCount())){
            if(tmManager->checkGroupEnd()){
                tmManager->setupNextGroup();
            }
        }

    }
    jumperResultsWidget->show();

    emit startListModel->dataChanged(startListModel->index(0), startListModel->index(startListModel->rowCount() - 1));
    ui->listView_startList->scrollTo(startListModel->index(manager->getActualStartListIndex() - 1));
    if(getType() == CompetitionRules::Individual){
        ui->tableView_results->setModel(nullptr);
        ui->tableView_results->setModel(resultsTableModel);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        if(KOManager != nullptr)
        {
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->updateJumpersSortedByResults();
            KOManager->updateStatuses();
            KOGroupsResultsModel->setGroup(KOManager->getActualGroup());
            manager->updateLastQualifiedResult();
            updateToAdvanceDistanceLabel();
            ui->tableView_KOGroupResults->setModel(nullptr);
            ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        }
    }
    else if(getType() == CompetitionRules::Team){
        teamResultsTreeModel->setupTreeItems();
        teamResultsTreeView->setModel(nullptr);
        teamResultsTreeView->setModel(teamResultsTreeModel);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        teamResultsTreeView->expandToDepth(0);
    }

    manager->updateLast10Judges();
    updateToBeatDistanceLabel();
    updateToAdvanceDistanceLabel();
    updatePointsToTheLeaderLabel();
    ui->label_actualAvgWind->setText(QString::number(WindsCalculator::getAveragedWind(actualWinds, manager->getCompetitionRules()->getWindAverageCalculatingType()).getStrengthToAveragedWind()) + " m/s");
    updateActualInrunSnowLevelLabel();

    manager->updateCompetitorsAdvanceStatuses();
    checkRecordsByResults(manager->getResults());
    emit ui->comboBox_virtualClassification->currentIndexChanged(ui->comboBox_virtualClassification->currentIndex());

    if(manager->checkCompetitionEnd() == true){
        setupGoToNextButtonForCompetitionEnd();
        showMessageBoxForCompetitionEnd();
    }
    else if(manager->checkRoundEnd() == true){
        setupGoToNextButtonForNextRound();
        showMessageBoxForNextRound();
    }
}

void CompetitionManagerWindow::autoSimulateCompetition()
{
    if(getType() == CompetitionRules::Individual)
        resultsTableModel->setLastJumper(nullptr);
    else
        teamResultsTreeModel->setLastTeam(nullptr);
    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
    TeamCompetitionManager * tmManager = dynamic_cast<TeamCompetitionManager *>(manager);
    while(manager->checkCompetitionEnd() != true){
        setupSimulator();
        simulator.simulateJump();
        JumpData jump = simulator.getJumpData();
        jump.setCompetition(manager->getCompetitionInfo());
        if(type == CompetitionRules::Individual){
            indManager->getResults()->addJump(indManager->getActualJumper(), jump);
        }
        else if(type == CompetitionRules::Team){
            tmManager->getResults()->addJump(tmManager->getActualTeam(), jump);
            tmManager->getResults()->getResultOfTeam(tmManager->getActualTeam())->updateTeamJumpersResults();
        }

        int status = StartListCompetitorStatus::Finished;
        if(jump.getDSQ())
            status = StartListCompetitorStatus::Dsq;
        else if(jump.getDNS())
            status = StartListCompetitorStatus::Dns;
        StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setJumpStatus(status);

        bool ok = false;
        if(jump.getLanding().getType() == Landing::Fall && jump.getDistance() >= 0.95 * jump.getHill()->getHSPoint() && manager->getCompetitionRules()->getHas95HSRule()){
            ok = true;
        }
        StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setQualifiedBy95HSRule(ok);
        manager->updateLastQualifiedResult();
        manager->updateLeaderResult();
        manager->updateLast10Judges();
        manager->updateToAdvanceLineDistance();
        manager->updateToBeatLineDistance();
        if(KOManager != nullptr)
        {
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->updateJumpersSortedByResults();
            KOManager->updateStatuses();
        }
        manager->updateCompetitorsAdvanceStatuses();

        setActualWinds(windsGenerator.generateWinds());
        updateActualInrunSnow();

        if(manager->isAllJumpsAreFinished() == false)
            manager->setActualJumperToNextUnfinished();
        if(manager->checkRoundEnd() && (manager->getActualRound() != manager->getCompetitionRules()->getRounds().count() && manager->getActualRound() != manager->getCompetitionInfo()->getExceptionalRoundsCount())){
            if(type == CompetitionRules::Individual)
            {
                indManager->setupNextRound();
                IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
                if(indManager != nullptr)
                {
                    if(KOManager != nullptr)
                    {
                        delete KOManager;
                        KOManager = nullptr;
                        indManager->setKOManager(nullptr);
                        delete KOGroupsResultsModel;
                        ui->tableView_KOGroupResults->hide();
                        startListModel->setKOGroups(nullptr);
                    }
                    if(indManager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1].getKO() == true)
                    {
                        KOManager = new KORoundManager(indManager->getRoundsKOGroupsReference()[manager->getActualRound() - 1], manager->getResults(), &indManager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1], dynamic_cast<IndividualCompetitionManager *>(manager));
                        KOManager->updateJumpersSortedByResults();
                        KOManager->setLuckyLosersCount(manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getCount() - (KOManager->getGroups()->count() * KOManager->getRoundInfo()->getAdvancingFromKOGroup()));
                        KOManager->updateActualGroup(manager->getActualJumper());
                        KOManager->setIndManager(dynamic_cast<IndividualCompetitionManager *>(manager));
                        indManager->setKOManager(KOManager);
                        KOGroupsResultsModel = new KOGroupResultsTableModel(KOManager, &dynamic_cast<IndividualCompetitionManager *>(manager)->getRoundsKOGroupsReference()[0]->first(), this);
                        ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
                        ui->tableView_KOGroupResults->resizeColumnsToContents();
                        ui->tableView_KOGroupResults->show();
                        startListModel->setKOGroups(KOManager->getGroups());
                    }
                }
            }
            else if(type == CompetitionRules::Team){
                tmManager->setupNextRound();
                teamResultsTreeModel->setTeams(&dynamic_cast<TeamCompetitionManager *>(manager)->getRoundsTeamsReference()[0]);
            }
        }
        else if(type == CompetitionRules::Team && (tmManager->getActualGroup() != tmManager->getCompetitionRules()->getJumpersInTeamCount())){
            if(tmManager->checkGroupEnd()){
                tmManager->setupNextGroup();
            }
        }
    }
    jumperResultsWidget->show();

    emit startListModel->dataChanged(startListModel->index(0), startListModel->index(startListModel->rowCount() - 1));
    ui->listView_startList->scrollTo(startListModel->index(manager->getActualStartListIndex() - 1));
    if(getType() == CompetitionRules::Individual){
        ui->tableView_results->setModel(nullptr);
        ui->tableView_results->setModel(resultsTableModel);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        if(KOManager != nullptr)
        {
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->updateJumpersSortedByResults();
            KOManager->updateStatuses();
            KOGroupsResultsModel->setGroup(KOManager->getActualGroup());
            manager->updateLastQualifiedResult();
            updateToAdvanceDistanceLabel();
            ui->tableView_KOGroupResults->setModel(nullptr);
            ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        }
    }
    else if(getType() == CompetitionRules::Team){
        teamResultsTreeModel->setupTreeItems();
        teamResultsTreeView->setModel(nullptr);
        teamResultsTreeView->setModel(teamResultsTreeModel);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        teamResultsTreeView->expandToDepth(0);
    }//teams sie psuje kiedy jest ostatnia seria

    manager->updateLast10Judges();
    updateToBeatDistanceLabel();
    updateToAdvanceDistanceLabel();
    updatePointsToTheLeaderLabel();
    ui->label_actualAvgWind->setText(QString::number(WindsCalculator::getAveragedWind(actualWinds, manager->getCompetitionRules()->getWindAverageCalculatingType()).getStrengthToAveragedWind()) + " m/s");
    updateActualInrunSnowLevelLabel();
    emit ui->comboBox_virtualClassification->currentIndexChanged(ui->comboBox_virtualClassification->currentIndex());

    manager->updateCompetitorsAdvanceStatuses();
    checkRecordsByResults(manager->getResults());

    if(manager->checkCompetitionEnd() == true){
        setupGoToNextButtonForCompetitionEnd();
        showMessageBoxForCompetitionEnd();
    }
}

void CompetitionManagerWindow::autoSimulateGroup()
{
        teamResultsTreeModel->setLastTeam(nullptr);
    if(getType() == CompetitionRules::Team){
        TeamCompetitionManager * tmManager = dynamic_cast<TeamCompetitionManager *>(manager);
        while(tmManager->checkGroupEnd() != true){
            setupSimulator();
            simulator.simulateJump();
            JumpData jump = simulator.getJumpData();
            jump.setCompetition(manager->getCompetitionInfo());
            tmManager->getResults()->addJump(tmManager->getActualTeam(), jump);
            tmManager->getResults()->getResultOfTeam(tmManager->getActualTeam())->updateTeamJumpersResults();

            int status = StartListCompetitorStatus::Finished;
            if(jump.getDSQ())
                status = StartListCompetitorStatus::Dsq;
            else if(jump.getDNS())
                status = StartListCompetitorStatus::Dns;
            StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setJumpStatus(status);

            bool ok = false;
            if(jump.getLanding().getType() == Landing::Fall && jump.getDistance() >= 0.95 * jump.getHill()->getHSPoint() && manager->getCompetitionRules()->getHas95HSRule()){
                ok = true;
            }
            StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setQualifiedBy95HSRule(ok);
            manager->updateLastQualifiedResult();
            manager->updateLeaderResult();
            manager->updateLast10Judges();
            manager->updateToAdvanceLineDistance();
            manager->updateToBeatLineDistance();
            manager->updateCompetitorsAdvanceStatuses();

            setActualWinds(windsGenerator.generateWinds());
            updateActualInrunSnow();
            if(manager->isAllJumpsAreFinished() == false)
                manager->setActualJumperToNextUnfinished();
        }
        jumperResultsWidget->show();

        emit startListModel->dataChanged(startListModel->index(0), startListModel->index(startListModel->rowCount() - 1));
        ui->listView_startList->scrollTo(startListModel->index(manager->getActualStartListIndex() - 1));
        teamResultsTreeModel->setupTreeItems();
        teamResultsTreeView->setModel(nullptr);
        teamResultsTreeView->setModel(teamResultsTreeModel);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        teamResultsTreeView->expandToDepth(0);

        manager->updateLast10Judges();
        updateToBeatDistanceLabel();
        updateToAdvanceDistanceLabel();
        updatePointsToTheLeaderLabel();
        ui->label_actualAvgWind->setText(QString::number(WindsCalculator::getAveragedWind(actualWinds, manager->getCompetitionRules()->getWindAverageCalculatingType()).getStrengthToAveragedWind()) + " m/s");
        updateActualInrunSnowLevelLabel();

        manager->updateCompetitorsAdvanceStatuses();
        checkRecordsByResults(manager->getResults());
        emit ui->comboBox_virtualClassification->currentIndexChanged(ui->comboBox_virtualClassification->currentIndex());

        if(manager->checkCompetitionEnd() == true){
            setupGoToNextButtonForCompetitionEnd();
            showMessageBoxForCompetitionEnd();
        }
        else if(manager->checkRoundEnd() == true){
            setupGoToNextButtonForNextRound();
            showMessageBoxForNextRound();
        }
        else if(tmManager->checkGroupEnd() == true){
            setupGoToNextButtonForNextGroup();
            showMessageBoxForNextGroup();
        }
    }
}

void CompetitionManagerWindow::autoSimulateJumps()
{
    bool ok;
    int jumpsCount = QInputDialog::getInt(this, tr("Symulacja kilku skoków"), tr("Ilość skoków"), 5, 0, 2147483647, 1, &ok);
    if(ok == true){
        for(int i = 0; i < jumpsCount; i++){
            IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
            TeamCompetitionManager * tmManager = dynamic_cast<TeamCompetitionManager *>(manager);
            setupSimulator();
            simulator.simulateJump();
            JumpData jump = simulator.getJumpData();
            jump.setCompetition(manager->getCompetitionInfo());
            if(type == CompetitionRules::Individual){
                indManager->getResults()->addJump(indManager->getActualJumper(), jump);
            }
            else if(type == CompetitionRules::Team){
                tmManager->getResults()->addJump(tmManager->getActualTeam(), jump);
                tmManager->getResults()->getResultOfTeam(tmManager->getActualTeam())->updateTeamJumpersResults();
            }

            checkRecords(jump);

            int status = StartListCompetitorStatus::Finished;
            if(jump.getDSQ())
                status = StartListCompetitorStatus::Dsq;
            else if(jump.getDNS())
                status = StartListCompetitorStatus::Dns;
            StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setJumpStatus(status);

            bool ok = false;
            if(jump.getLanding().getType() == Landing::Fall && jump.getDistance() >= 0.95 * jump.getHill()->getHSPoint() && manager->getCompetitionRules()->getHas95HSRule()){
                ok = true;
            }
            StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setQualifiedBy95HSRule(ok);
            manager->updateLastQualifiedResult();
            manager->updateLeaderResult();
            manager->updateLast10Judges();
           // manager->updateToAdvanceLineDistance();
            manager->updateToBeatLineDistance();
            if(KOManager != nullptr)
            {
                KOManager->updateActualGroup(manager->getActualJumper());
                KOManager->updateJumpersSortedByResults();
                KOManager->updateStatuses();
            }
            manager->updateCompetitorsAdvanceStatuses();

            setActualWinds(windsGenerator.generateWinds());
            updateActualInrunSnow();
            if(type == CompetitionRules::Individual)
                resultsTableModel->setLastJumper(manager->getActualJumper());
            else
                teamResultsTreeModel->setLastTeam(static_cast<TeamCompetitionManager*>(manager)->getActualTeam());


            if(manager->isAllJumpsAreFinished() == false)
                manager->setActualJumperToNextUnfinished();

            if(manager->checkCompetitionEnd() == true)
                break;
            if(manager->checkRoundEnd() && (manager->getActualRound() != manager->getCompetitionRules()->getRounds().count() && manager->getActualRound() != manager->getCompetitionInfo()->getExceptionalRoundsCount()) && i+1 < jumpsCount){
                if(type == CompetitionRules::Individual)
                {
                    if(getType() == CompetitionRules::Individual)
                        resultsTableModel->setLastJumper(nullptr);
                    indManager->setupNextRound();
                    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
                    if(indManager != nullptr)
                    {
                        if(KOManager != nullptr)
                        {
                            delete KOManager;
                            KOManager = nullptr;
                            indManager->setKOManager(nullptr);
                            delete KOGroupsResultsModel;
                            ui->tableView_KOGroupResults->hide();
                            startListModel->setKOGroups(nullptr);
                        }
                        if(indManager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1].getKO() == true)
                        {
                            KOManager = new KORoundManager(indManager->getRoundsKOGroupsReference()[manager->getActualRound() - 1], manager->getResults(), &indManager->getCompetitionRules()->getRoundsReference()[manager->getActualRound() - 1], dynamic_cast<IndividualCompetitionManager *>(manager));
                            KOManager->updateJumpersSortedByResults();
                            KOManager->setLuckyLosersCount(manager->getCompetitionRules()->getRoundsReference()[manager->getActualRound()].getCount() - (KOManager->getGroups()->count() * KOManager->getRoundInfo()->getAdvancingFromKOGroup()));
                            KOManager->updateActualGroup(manager->getActualJumper());
                            KOManager->setIndManager(dynamic_cast<IndividualCompetitionManager *>(manager));
                            indManager->setKOManager(KOManager);
                            KOGroupsResultsModel = new KOGroupResultsTableModel(KOManager, &dynamic_cast<IndividualCompetitionManager *>(manager)->getRoundsKOGroupsReference()[0]->first(), this);
                            ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
                            ui->tableView_KOGroupResults->resizeColumnsToContents();
                            ui->tableView_KOGroupResults->show();
                            startListModel->setKOGroups(KOManager->getGroups());
                        }
                    }
                }
                else if(type == CompetitionRules::Team){
                    teamResultsTreeModel->setLastTeam(nullptr);
                    tmManager->setupNextRound();
                    teamResultsTreeModel->setTeams(&dynamic_cast<TeamCompetitionManager *>(manager)->getRoundsTeamsReference()[0]);
                }
            }
            else if(type == CompetitionRules::Team && (tmManager->getActualGroup() != tmManager->getCompetitionRules()->getJumpersInTeamCount())){
                if(tmManager->checkGroupEnd()){
                    tmManager->setupNextGroup();
                }
            }
        }
        jumperResultsWidget->show();

        emit startListModel->dataChanged(startListModel->index(0), startListModel->index(startListModel->rowCount() - 1));
        ui->listView_startList->scrollTo(startListModel->index(manager->getActualStartListIndex() - 1));
        if(getType() == CompetitionRules::Individual){
            ui->tableView_results->setModel(nullptr);
            ui->tableView_results->setModel(resultsTableModel);
            ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            if(KOManager != nullptr)
            {
                KOManager->updateActualGroup(manager->getActualJumper());
                KOManager->updateJumpersSortedByResults();
                KOManager->updateStatuses();
                KOGroupsResultsModel->setGroup(KOManager->getActualGroup());
                manager->updateLastQualifiedResult();
                updateToAdvanceDistanceLabel();
                ui->tableView_KOGroupResults->setModel(nullptr);
                ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
                ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            }
        }
        else if(getType() == CompetitionRules::Team){
            teamResultsTreeModel->setupTreeItems();
            teamResultsTreeView->setModel(nullptr);
            teamResultsTreeView->setModel(teamResultsTreeModel);
            teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
            teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
            teamResultsTreeView->expandToDepth(0);
        }

        manager->updateLast10Judges();
        updateToBeatDistanceLabel();
        updateToAdvanceDistanceLabel();
        updatePointsToTheLeaderLabel();
        ui->label_actualAvgWind->setText(QString::number(WindsCalculator::getAveragedWind(actualWinds, manager->getCompetitionRules()->getWindAverageCalculatingType()).getStrengthToAveragedWind()) + " m/s");
        updateActualInrunSnowLevelLabel();
        emit ui->comboBox_virtualClassification->currentIndexChanged(ui->comboBox_virtualClassification->currentIndex());

        manager->updateCompetitorsAdvanceStatuses();

        if(manager->checkCompetitionEnd() == true){
            if(getType() == CompetitionRules::Individual)
                resultsTableModel->setLastJumper(nullptr);
            else
                teamResultsTreeModel->setLastTeam(nullptr);
            setupGoToNextButtonForCompetitionEnd();
            showMessageBoxForCompetitionEnd();
        }
    }
}

void CompetitionManagerWindow::checkRecords(JumpData &jump, bool multipleRecords)
{
    if(jump.getCompetition()->getRulesPointer()->getHillRecordBreaking() == true && (jump.getLanding().getType() == Landing::TelemarkLanding || jump.getLanding().getType() == Landing::BothLegsLanding) && (singleCompetition == false || GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords() == true)){
        if(roundDoubleToHalf(jump.getDistance()) == roundDoubleToHalf(jump.getHill()->getRecord()) && multipleRecords == false)
        {
            QMessageBox::information(this, "Wyrównanie rekordu skoczni", tr("%1 wyrównał rekord skoczni %2, wynoszący %3m").arg(jump.getJumper()->getTextInfo()).arg(jump.getHill()->getHillText()).arg(QString::number(jump.getHill()->getRecord(), 'f', 1)), QMessageBox::Ok);
        }
        else if(roundDoubleToHalf(jump.getDistance()) > roundDoubleToHalf(jump.getHill()->getRecord()))
        {
            QMessageBox::information(this, "Nowy rekord skoczni", tr("Po skoku na odległość %1m, %2 ustanowił nowy rekord skoczni %3 (wcześniej %4m).").arg(QString::number(jump.getDistance(), 'f', 1)).arg(jump.getJumper()->getTextInfo()).arg(jump.getHill()->getHillText()).arg(QString::number(jump.getHill()->getRecord(), 'f', 1)), QMessageBox::Ok);

            if(singleCompetition){
                if(GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords() == true)
                    jump.getHill()->setRecord(jump.getDistance());
            }
            else{
                    jump.getHill()->setRecord(jump.getDistance());
            }
            ui->label_record->setText(tr("Rekord: ") + QString::number(jump.getHill()->getRecord(), 'f', 1));
    }
    }

    if(jump.getHill()->getHillType() == Hill::Flying && (jump.getLanding().getType() == Landing::TelemarkLanding || jump.getLanding().getType() == Landing::BothLegsLanding)  && (singleCompetition == false || GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords() == true)){
    if(roundDoubleToHalf(jump.getDistance()) == roundDoubleToHalf(jump.getJumper()->getPersonalBest()) && multipleRecords == false)
    {
        QMessageBox::information(this, "Wyrównanie rekordu życiowego", tr("%1 wyrównał swój własny rekord życiowy, skacząc %2m").arg(jump.getJumper()->getTextInfo()).arg(QString::number(jump.getDistance(), 'f', 1)), QMessageBox::Ok);
    }
    else if(roundDoubleToHalf(jump.getDistance()) > roundDoubleToHalf(jump.getJumper()->getPersonalBest()))
    {
        QMessageBox::information(this, "Pobicie rekordu życiowego", tr("%1 pobił swój własny rekord życiowy, skacząc %2m (poprzedni rekord: %3m)").arg(jump.getJumper()->getTextInfo()).arg(QString::number(jump.getDistance(), 'f', 1)).arg(QString::number(jump.getJumper()->getPersonalBest(), 'f', 1)), QMessageBox::Ok);

       qDebug()<<"singleCompetition: "<<singleCompetition;
       qDebug()<<"GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords(): "<<GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords();
       if(singleCompetition){
            if(GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords() == true)
                    jump.getJumper()->setPersonalBest(jump.getDistance());
       }
       else
            jump.getJumper()->setPersonalBest(jump.getDistance());
    }
    }
}

void CompetitionManagerWindow::checkRecordsByResults(CompetitionResults *results)
{
    Hill * hill = manager->getCompetitionInfo()->getHill();
    for(auto & result : results->getResultsReference())
    {
    for(auto & jump : result.getJumpsReference())
    {
       checkRecords(jump, true);
    }
    for(auto & res : result.getTeamJumpersResultsReference())
    {
       for(auto & jump : res.getJumpsReference())
       {
         checkRecords(jump, true);
       }
    }
    }
}

void CompetitionManagerWindow::executeLiveCompetitionEffects()
{
    disableCompetitionManagementButtons();
    jumperResultsWidget->getUi()->label_actualPosition->hide();
    jumperResultsWidget->getUi()->label_pointsSum->hide();

    qDebug()<<"sh ; .bc "<<jumperResultsWidget->getUi()->tabWidget_jumps->count();
    qDebug()<<jumperResultsWidget->getUi()->tabWidget_jumps->widget(jumperResultsWidget->getUi()->tabWidget_jumps->count() - 1);

    JumpDataDetailedInfoWindow * info = static_cast<JumpDataDetailedInfoWindow*>(jumperResultsWidget->getUi()->tabWidget_jumps->widget(jumperResultsWidget->getUi()->tabWidget_jumps->count() - 1));
    if(info->isHidden())
        info->show();
    info->getUi()->label_distance->hide();
    info->getUi()->label_judges->hide();
    info->getUi()->label_judgesPoints->hide();
    info->getUi()->label_points->hide();
    info->getUi()->label_landingType->hide();
    info->getUi()->label_takeoffRating->hide();
    info->getUi()->label_flightRating->hide();
    info->getUi()->label_random->hide();
    info->getUi()->label_snowRain->hide();
    info->getUi()->label_positionInRound->hide();

    JumpDataInfoChoice * c = &GlobalAppSettings::get()->getJumpDataInfoChoiceReference();

    QEventLoop loop;
    QTimer::singleShot(5000 * (manager->getCompetitionInfo()->getHill()->getKPoint() / double(130)) * GlobalAppSettings::get()->getLiveCompetitionSpeedMulti(), &loop, SLOT(quit()));
    loop.exec();
    if(c->getDistance())
        info->getUi()->label_distance->show();

    QTimer::singleShot(1000 * GlobalAppSettings::get()->getLiveCompetitionSpeedMulti(), &loop, SLOT(quit()));
    loop.exec();
    if(c->getLandingType())
        info->getUi()->label_landingType->show();

    QTimer::singleShot(3500 * GlobalAppSettings::get()->getLiveCompetitionSpeedMulti(), &loop, SLOT(quit()));
    loop.exec();
    if(c->getJudges())
        info->getUi()->label_judges->show();
    if(c->getJudgesPoints())
        info->getUi()->label_judgesPoints->show();

    QTimer::singleShot(2500 * GlobalAppSettings::get()->getLiveCompetitionSpeedMulti(), &loop, SLOT(quit()));
    loop.exec();
    jumperResultsWidget->getUi()->label_actualPosition->show();
    jumperResultsWidget->getUi()->label_pointsSum->show();
    if(c->getPoints())
        info->getUi()->label_points->show();
    if(c->getJumpPositionInRound())
        info->getUi()->label_positionInRound->show();
    if(c->getTakeoffRating())
        info->getUi()->label_takeoffRating->show();
    if(c->getFlightRating())
        info->getUi()->label_flightRating->show();
    if(c->getDistanceRandom())
        info->getUi()->label_random->show();
    if(c->getInrunSnow())
        info->getUi()->label_snowRain->show();

    if(manager->getCompetitionInfo()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual){
        ui->tableView_results->setUpdatesEnabled(true);
        if(KOManager != nullptr)
            ui->tableView_KOGroupResults->setUpdatesEnabled(true);
    }
    if(teamResultsTreeView != nullptr)
        teamResultsTreeView->setUpdatesEnabled(true);
    else
        ui->tableView_results->show();
    enableCompetitionManagementButtons();

    updateToBeatDistanceLabel();
    updateToAdvanceDistanceLabel();
    updatePointsToTheLeaderLabel();
}

void CompetitionManagerWindow::checkForEasterEggs()
{
    //6000
    //50000000
    CompetitionInfo * c = manager->getCompetitionInfo();
    if(c->getHill()->getName() == "Innsbruck" && c->getHill()->getKPoint() == 120)
    {
        if(MyRandom::randomInt(1, 6000) == 10)
        {
            QString jumperText = manager->getActualJumper()->getNameAndSurname();
            QString secondText;
            if(MyRandom::randomInt(1, 2000) == 2)
         secondText = tr("Niestety fatalne wieści dopłynęły do nas z austriackiego Innsbrucka. Nie udało się odratować %1 który zmarł na miejscu.").arg(jumperText);
            else if(MyRandom::randomInt(1, 2000) < 200)
         secondText = tr("Zawodnik został poważnie poturbowany ale nic nie zagraża jego życiu.");
            else
         secondText = tr("Zawodnik na szczęście wyszedł bez szwanku.");
         QMessageBox::warning(this, tr("Niebezpieczna sytuacja na skoczni"), tr("Zawodnik %1 nie zdążył wyhamować i w niebezpieczny sposób przeleciał przez bandę.\n%2").arg(jumperText).arg(secondText), tr("Jestem wzruszony"), tr("Dobrze tak temu łajdakowi"), tr("Mam nadzieje że wzrost populacji krokodyli nastąpi maksymalnie do 2028 roku"));
        }
    }
    if(MyRandom::randomInt(1, 50000000) == 2)
    {
        QMessageBox::critical(this, tr("Koniec konkursu!!!"), tr("Niestety konkurs musi zostać odwołany w trybie NATYCHMIASTOWYM. Pod skocznią wykryto bombę która w każdej chwili może wybuchnąć. Przepraszamy za niedogodności."), tr("XD"), tr("Ez super mega ok"), tr("Też tak myślę"));
        close();
    }
}

bool CompetitionManagerWindow::getSingleCompetition() const
{
    return singleCompetition;
}

void CompetitionManagerWindow::setSingleCompetition(bool newSingleCompetition)
{
    singleCompetition = newSingleCompetition;
}

KORoundManager *CompetitionManagerWindow::getKOManager() const
{
    return KOManager;
}

void CompetitionManagerWindow::setKOManager(KORoundManager *newKOManager)
{
    KOManager = newKOManager;
}

KOGroupResultsTableModel *CompetitionManagerWindow::getKOGroupsResultsModel() const
{
    return KOGroupsResultsModel;
}

void CompetitionManagerWindow::setKOGroupsResultsModel(KOGroupResultsTableModel *newKOGroupsResultsModel)
{
    KOGroupsResultsModel = newKOGroupsResultsModel;
}

InrunSnowGenerator CompetitionManagerWindow::getInrunSnowGenerator() const
{
    return inrunSnowGenerator;
}

void CompetitionManagerWindow::setInrunSnowGenerator(const InrunSnowGenerator &newInrunSnowGenerator)
{
    inrunSnowGenerator = newInrunSnowGenerator;
    actualInrunSnow = inrunSnowGenerator.getBaseInrunSnow();
    updateActualInrunSnowLevelLabel();
}

void CompetitionManagerWindow::cancelCompetition()
{
    manager->getResults()->getResultsReference().clear();
    manager->getCompetitionInfo()->setCancelled(true);

    emit startListModel->dataChanged(startListModel->index(manager->getActualStartListIndex() - 1), startListModel->index(manager->getActualStartListIndex() - 1));
    ui->listView_startList->scrollTo(startListModel->index(manager->getActualStartListIndex()));
    if(getType() == CompetitionRules::Individual){
        ui->tableView_results->setModel(nullptr);
        ui->tableView_results->setModel(resultsTableModel);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        if(KOManager != nullptr)
        {
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->updateJumpersSortedByResults();
            KOManager->updateStatuses();
            KOGroupsResultsModel->setGroup(KOManager->getActualGroup());
            manager->updateLastQualifiedResult();
            updateToAdvanceDistanceLabel();
            ui->tableView_KOGroupResults->setModel(nullptr);
            ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        }
    }
    else if(getType() == CompetitionRules::Team){
        teamResultsTreeView->setModel(nullptr);
        teamResultsTreeView->setModel(teamResultsTreeModel);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        teamResultsTreeView->expandToDepth(0);
    }

    setupGoToNextButtonForCompetitionEnd();
    showMessageBoxForCompetitionEnd();
}

void CompetitionManagerWindow::cancelActualRound()
{
    for(auto & res : manager->getResults()->getResultsReference()){
        if(res.getJumpsReference().count() == manager->getActualRound()){
            res.getJumpsReference().removeLast();
            res.updatePointsSum();
        }
        manager->getResults()->sortInDescendingOrder();

        emit startListModel->dataChanged(startListModel->index(manager->getActualStartListIndex() - 1), startListModel->index(manager->getActualStartListIndex() - 1));
        if(getType() == CompetitionRules::Individual){
            ui->tableView_results->setModel(nullptr);
            ui->tableView_results->setModel(resultsTableModel);
            ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            if(KOManager != nullptr)
            {
                KOManager->updateActualGroup(manager->getActualJumper());
                KOManager->updateJumpersSortedByResults();
                KOManager->updateStatuses();
                KOGroupsResultsModel->setGroup(KOManager->getActualGroup());
                manager->updateLastQualifiedResult();
                updateToAdvanceDistanceLabel();
                ui->tableView_KOGroupResults->setModel(nullptr);
                ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
                ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            }
        }
        else if(getType() == CompetitionRules::Team){
            teamResultsTreeView->setModel(nullptr);
            teamResultsTreeView->setModel(teamResultsTreeModel);
            teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
            teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
            teamResultsTreeView->expandToDepth(0);
        }
    }

    setupGoToNextButtonForCompetitionEnd();
    showMessageBoxForCompetitionEnd();
}
//dlaczego jak dodaje trzecii vector z drużynami, to pierwszy się zmienia?
// W roundsTeams jest ta sama wartość ale we wskaźniku do niego jest 0 elementów, czyli być może istnieje dalej w pamięci i ma 0 elementów, ale w roundsTeams pojawił się nowy który ma te 12 ale inny adres.
// Teorytycznie nowe przypisanie wskaźnika do model.teams powinno zadziałać, ale jakie jest rozwiązanie tego problemu w sposób "czysty?"

void CompetitionManagerWindow::setupSimulator()
{
    simulator.setGate(manager->getActualGate());
    simulator.setHasCoachGate(manager->getCoachGateForNextJumper());
    simulator.setCoachGate(manager->getActualCoachGate());
    simulator.setCompetitionStartGate(manager->getRoundsStartingGatesReference()[manager->getActualRound() - 1]);
    simulator.setJumper(manager->getActualJumper());
    simulator.setHill(manager->getCompetitionInfo()->getHill());
    simulator.setManipulator(&this->currentInputJumpManipulator);
    simulator.setCompetitionRules(manager->getCompetitionRules());
    simulator.setDSQBaseProbability(manager->getBaseDSQProbability());
    simulator.setWinds(actualWinds);
    updateActualInrunSnow();
    simulator.setInrunSnow(actualInrunSnow);
    qDebug()<<"importance: "<<manager->getCompetitionInfo()->getJumpsImportance();
    simulator.setJumpsImportance(manager->getCompetitionInfo()->getJumpsImportance());
}

void CompetitionManagerWindow::on_pushButton_jump_clicked()
{
    if(GlobalAppSettings::get()->getLiveCompetition()){
        if(manager->getCompetitionInfo()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual){
            ui->tableView_results->setUpdatesEnabled(false);
            if(KOManager != nullptr)
                ui->tableView_KOGroupResults->setUpdatesEnabled(false);
        }
        if(teamResultsTreeView != nullptr)
            teamResultsTreeView->setUpdatesEnabled(false);
        //ui->tableView_results->show();

    }
    IndividualCompetitionManager * indManager = dynamic_cast<IndividualCompetitionManager *>(manager);
    TeamCompetitionManager * tmManager = dynamic_cast<TeamCompetitionManager *>(manager);
    setupSimulator();
    simulator.simulateJump();
    JumpData jump = simulator.getJumpData();
    jump.setCompetition(manager->getCompetitionInfo());
    if(type == CompetitionRules::Individual){
        indManager->getResults()->addJump(indManager->getActualJumper(), jump);
    }
    else if(type == CompetitionRules::Team){
        tmManager->getResults()->addJump(tmManager->getActualTeam(), jump);
        tmManager->getResults()->getResultOfTeam(tmManager->getActualTeam())->updateTeamJumpersResults();
    }

    int status = StartListCompetitorStatus::Finished;
    if(jump.getDSQ())
        status = StartListCompetitorStatus::Dsq;
    else if(jump.getDNS())
        status = StartListCompetitorStatus::Dns;

    StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setJumpStatus(status);

    bool ok = false;
    if(jump.getLanding().getType() == Landing::Fall && jump.getDistance() >= 0.95 * jump.getHill()->getHSPoint() && manager->getCompetitionRules()->getHas95HSRule())
        ok = true;
    StartListCompetitorStatus::getStatusOfJumper(manager->getActualJumper(), manager->getStartListStatusesReference())->setQualifiedBy95HSRule(ok);
    manager->updateLastQualifiedResult();
    manager->updateLeaderResult();
    manager->updateLast10Judges();
    /*updateToAdvanceDistanceLabel();
    updateToBeatDistanceLabel();*/
    if(KOManager != nullptr)
    {
        KOManager->updateActualGroup(manager->getActualJumper());
        KOManager->updateJumpersSortedByResults();
        KOManager->updateStatuses();
    }
    manager->updateCompetitorsAdvanceStatuses();

    if(jumperResultsWidget->isHidden()) jumperResultsWidget->show();
    if(type == CompetitionRules::Individual){
        jumperResultsWidget->setJumperResult(manager->getResults()->getResultOfIndividualJumper(manager->getActualJumper()));
    }
    else if(type == CompetitionRules::Team){
        jumperResultsWidget->setJumperResult(manager->getResults()->getResultOfTeam(tmManager->getActualTeam())->getTeamJumperResult(manager->getActualJumper()));
    }
    jumperResultsWidget->fillWidget();
    if(GlobalAppSettings::get()->getLiveCompetition())
        executeLiveCompetitionEffects();
    checkForEasterEggs();

    emit startListModel->dataChanged(startListModel->index(manager->getActualStartListIndex() - 1), startListModel->index(manager->getActualStartListIndex() - 1));
    ui->listView_startList->scrollTo(startListModel->index(manager->getActualStartListIndex() - 1));
    if(getType() == CompetitionRules::Individual){
        resultsTableModel->setLastJumper(manager->getActualJumper());
        ui->tableView_results->setModel(nullptr);
        ui->tableView_results->setModel(resultsTableModel);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_results->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        if(KOManager != nullptr)
        {
            KOManager->updateActualGroup(manager->getActualJumper());
            KOManager->updateJumpersSortedByResults();
            KOManager->updateStatuses();
            KOGroupsResultsModel->setGroup(KOManager->getActualGroup());
            manager->updateLastQualifiedResult();
            updateToAdvanceDistanceLabel();
            ui->tableView_KOGroupResults->setModel(nullptr);
            ui->tableView_KOGroupResults->setModel(KOGroupsResultsModel);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView_KOGroupResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        }
    }
    else if(getType() == CompetitionRules::Team){
        teamResultsTreeModel->setLastTeam(static_cast<TeamCompetitionManager*>(manager)->getActualTeam());
        teamResultsTreeModel->setupTreeItems();
        teamResultsTreeView->setModel(nullptr);
        teamResultsTreeView->setModel(teamResultsTreeModel);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);
        teamResultsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        teamResultsTreeView->expandToDepth(0);
    }

    manager->setCoachGateForNextJumper(false);
    manager->setActualCoachGate(0);
    ui->spinBox_actualGate->setValue(manager->getActualGate());
    if(manager->isAllJumpsAreFinished() == false)
        manager->setActualJumperToNextUnfinished();
    setActualWinds(windsGenerator.generateWinds());
    updateActualInrunSnow();
    currentInputJumpManipulator = JumpManipulator();

    updateAvgWindLabel();
    updateActualInrunSnowLevelLabel();
    manager->updateLastQualifiedResult();
    manager->updateLeaderResult();
    manager->updateLast10Judges();
    if(GlobalAppSettings::get()->getLiveCompetition() == false){
    updateToBeatDistanceLabel();
    updateToAdvanceDistanceLabel();
    updatePointsToTheLeaderLabel();
    }

    checkRecords(jump);
    emit ui->comboBox_virtualClassification->currentIndexChanged(ui->comboBox_virtualClassification->currentIndex());

    if(manager->checkCompetitionEnd()){
        if(getType() == CompetitionRules::Individual)
            resultsTableModel->setLastJumper(nullptr);
        else
            teamResultsTreeModel->setLastTeam(nullptr);
        setupGoToNextButtonForCompetitionEnd();
        showMessageBoxForCompetitionEnd();
    }
    else if(manager->checkRoundEnd()){
        if(getType() == CompetitionRules::Individual)
            resultsTableModel->setLastJumper(nullptr);
        else
            teamResultsTreeModel->setLastTeam(nullptr);
        setupGoToNextButtonForNextRound();
        showMessageBoxForNextRound();
    }
    else if(getType() == CompetitionRules::Team)
    {
        if(tmManager->checkGroupEnd()){
            teamResultsTreeModel->setLastTeam(nullptr);
            setupGoToNextButtonForNextGroup();
            showMessageBoxForNextGroup();
        }
    }
}

void CompetitionManagerWindow::on_tableView_results_doubleClicked(const QModelIndex &index)
{
    if(index.column() == 1){
        jumperResultsWidget->setJumperResult(manager->getResults()->getResultByIndex(index.row()));
        jumperResultsWidget->fillWidget();
    }
}


void CompetitionManagerWindow::on_pushButton_generateNewWinds_clicked()
{
    setActualWinds(windsGenerator.generateWinds());
    if(manager->getActualStartListIndex() > 0){
        manager->updateLeaderResult();
        manager->updateLast10Judges();
        updateToBeatDistanceLabel();
        updateToAdvanceDistanceLabel();
    }
    updateAvgWindLabel();
}


void CompetitionManagerWindow::on_pushButton_windsGeneratorSettings_clicked()
{
    QDialog * dialog = new QDialog;
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle("Edytuj ustawienia generatora wiatru");
    dialog->setStyleSheet("background-color: rgb(225, 225, 225);");
    dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog->setFixedSize(dialog->size());
    dialog->setLayout(new QVBoxLayout(dialog));

    WindsGeneratorSettingsEditorWidget * editor = new WindsGeneratorSettingsEditorWidget;
    editor->setWindGenerationSettings(&manager->getWindGenerationSettingsReference());
    editor->setRemovingSubmitButtons(true);
    editor->setKPoint(manager->getCompetitionInfo()->getHill()->getKPoint());
    editor->setSettingsCount(WindsGenerator::calculateWindsCountByKPoint(editor->getKPoint()));
    editor->fillSettingsInputs();

    dialog->layout()->addWidget(editor);
    connect(editor, &WindsGeneratorSettingsEditorWidget::submitted, dialog, &QDialog::accept);

    if(dialog->exec() == QDialog::Accepted){
        manager->setWindGenerationSettings(editor->getWindsGenerationSettingsFromInputs());
        windsGenerator.setGenerationSettings(manager->getWindGenerationSettingsReference());
        delete editor;
        delete dialog;
    }

}

void CompetitionManagerWindow::on_pushButton_inrunSnowGeneratorSettings_clicked()
{
    QDialog * dialog = new QDialog;
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle("Edytuj ustawienia generatora wiatru");
    dialog->setStyleSheet("background-color: rgb(225, 225, 225);");
    dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog->setFixedSize(dialog->size());
    dialog->setLayout(new QVBoxLayout(dialog));

    InrunSnowGeneratorSettingsEditorWidget * editor = new InrunSnowGeneratorSettingsEditorWidget;
    editor->setGenerator(&inrunSnowGenerator);
    editor->fillInputs();

    dialog->layout()->addWidget(editor);
    connect(editor, &InrunSnowGeneratorSettingsEditorWidget::submitted, dialog, &QDialog::accept);

    if(dialog->exec() == QDialog::Accepted){
        inrunSnowGenerator.setBaseInrunSnow(editor->getBase());
        inrunSnowGenerator.setInrunSnowDeviation(editor->getDeviation());
        actualInrunSnow = editor->getBase();

        delete editor;
        delete dialog;
    }
}

void CompetitionManagerWindow::on_pushButton_manipulateJump_clicked()
{
    JumpManipulatorConfigWindow * window = new JumpManipulatorConfigWindow;
    window->setWindGenerationSettings(&manager->getWindGenerationSettingsReference());
    window->setKPoint(manager->getCompetitionInfo()->getHill()->getKPoint());
    connect(window, &JumpManipulatorConfigWindow::submitted, window, &JumpManipulatorConfigWindow::accept);
    if(window->exec() == QDialog::Accepted){
        JumpManipulator manipulator = window->getJumpManipulatorFromInputs();
        currentInputJumpManipulator = (manipulator);
        if(manipulator.getExactWinds().size() > 0)
            setActualWinds(manipulator.getExactWinds());      
        updateAvgWindLabel();
        updateActualInrunSnowLevelLabel();
    }
}


void CompetitionManagerWindow::on_pushButton_coachGate_clicked()
{
    bool ok{};
    int howMany = QInputDialog::getInt(this, "Obniżenie belki na życzenie trenera", "O ile stopni obniżyć belkę?\nAby uzyskać dodatkową rekompensatę, zawodnik musi osiągnąć 95% punktu HS (" + QString::number(roundDoubleToHalf(manager->getCompetitionInfo()->getHill()->getHSPoint() * 0.95)) + " m)", 1, 0, 1000000, 1, &ok);
    if(howMany > 0 && ok){
        manager->setCoachGateForNextJumper(true);
        manager->setActualCoachGate(manager->getActualGate() - howMany);
        manager->updateLast10Judges();
        updateToAdvanceDistanceLabel();
        updateToBeatDistanceLabel();

        ui->spinBox_actualGate->setValue(manager->getActualCoachGate());
        manager->setActualGate(manager->getActualGate() + howMany);
    }
    else if(howMany == 0 && ok)
    {
        manager->setCoachGateForNextJumper(false);
        manager->updateLast10Judges();
        updateToAdvanceDistanceLabel();
        updateToBeatDistanceLabel();

        ui->spinBox_actualGate->setValue(manager->getActualGate());
    }
}

QVector<Wind> CompetitionManagerWindow::getActualWinds() const
{
    return actualWinds;
}

QVector<Wind> & CompetitionManagerWindow::getActualWindsReference()
{
    return actualWinds;
}

void CompetitionManagerWindow::setActualWinds(const QVector<Wind> &newActualWinds)
{
    actualWinds = newActualWinds;
    manager->setActualWinds(&actualWinds);
}

void CompetitionManagerWindow::on_comboBox_virtualClassification_currentIndexChanged(int index)
{
    if(singleCompetition == false){
    if(index > 0 && singleCompetition == false)
    {
        index--;
        Classification * classification = new Classification(*manager->getCompetitionInfo()->getClassificationsReference()[index]);
        classification->getResultsReference().detach();
        for(auto & singleRes : classification->getResultsReference())
        {
            if(singleRes->getCompetitionsResultsReference().contains(&this->manager->getCompetitionInfo()->getResultsReference()) == false)
                singleRes->getCompetitionsResultsReference().push_back(&this->manager->getCompetitionInfo()->getResultsReference());
            singleRes->updateSingleResults();
            singleRes->updatePointsSum();
        }
        classification->sortInDescendingOrder();
        virtualClassificationTableView->setClassification(classification);
        virtualClassificationTableView->fillTable();

        virtualClassifications.push_back(classification);
    }
    else
    {
        Classification * classification = new Classification("Ok");
        virtualClassificationTableView->setClassification(classification);
        virtualClassificationTableView->fillTable();
        virtualClassifications.push_back(classification);
    }
    }
}


void CompetitionManagerWindow::on_pushButton_sendResultsWebhook_clicked()
{
    manager->getCompetitionInfo()->sendResultsWebhook(manager);
}

void CompetitionManagerWindow::on_checkBox_liveCompetition_stateChanged(int arg1)
{
    GlobalAppSettings::get()->setLiveCompetition(arg1);
}

void CompetitionManagerWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Wyjście z konkursu"),
                                                               tr("Na pewno chcesz wyjsć z tego okna? Spowoduje to odwołanie aktualnie rozgrywanego konkursu."),
                                                               QMessageBox::Cancel | QMessageBox::Yes, QMessageBox::Cancel);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        manager->getCompetitionInfo()->setCancelled(true);
        manager->getResults()->getResultsReference().clear();
        accept();
        event->accept();
    }
}
