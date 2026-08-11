#include "CompetitionConfigWindow.h"
#include "ui_CompetitionConfigWindow.h"

#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../EditorWidgets/TeamsEditing/TeamEditorWidget.h"
#include "../EditorWidgets/EditStartListWithJumpersListsWindow.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/GlobalSimulationSettings.h"
#include "../../global/CountryFlagsManager.h"
#include "../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../../competitions/CompetitionResults.h"
#include "../../utilities/functions.h"
#include "CompetitionManagerWindow.h"
#include "RandomWindConfigWindow.h"
#include "StartListByOtherCompetitionConfigWindow.h"
#include "../ResponsiveWindowUtils.h"

#include <QSizePolicy>
#include <QBoxLayout>
#include <QStringListModel>
#include <QStringList>
#include <QCompleter>
#include <QFileDialog>
#include <QFile>
#include <QKeySequence>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QResizeEvent>
#include <limits>

CompetitionConfigWindow::CompetitionConfigWindow(short type, QWidget *parent, SimulationSave *save) :
    QDialog(parent),
    ui(new Ui::SingleCompetitionConfigWindow),
    type(type),
    simulationSave(save)
{
    ui->setupUi(this);
    setMinimumSize(640, 480);
    ui->splitter_configuration->setStretchFactor(0, 2);
    ui->splitter_configuration->setStretchFactor(1, 5);
    ui->splitter_configuration->setStretchFactor(2, 2);
    ui->splitter_configuration->setSizes({260, 640, 280});
    ui->verticalLayout_4->setAlignment(Qt::AlignTop);
    ui->verticalLayout_hillEditorWidget->setAlignment(Qt::AlignTop);
    ui->verticalLayout_2->setAlignment(Qt::AlignTop);
    ui->verticalLayout->addWidget(ui->pushButton_submit, 0, Qt::AlignHCenter);
    const int submitHeight = qBound(50, ui->pushButton_submit->fontMetrics().height() + 18, 64);
    ui->pushButton_submit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->pushButton_submit->setFixedHeight(submitHeight);
    ui->pushButton_submit->setMinimumWidth(250);
    ui->pushButton_submit->setMaximumWidth(360);
    ui->pushButton_submit->setDefault(true);
    ui->pushButton_submit->setToolTip(tr("Sprawdź ustawienia i rozpocznij symulację (Enter)"));
    ui->pushButton_autoGate->setToolTip(tr("Dobierz bezpieczną belkę na podstawie najmocniejszego zawodnika"));
    ui->pushButton_loadJumpers->setToolTip(tr("Wczytaj listę zawodników z pliku JSON"));
    ui->pushButton_jumpersLists->hide();
    if(simulationSave != nullptr)
        seasonCompetition = simulationSave->getNextCompetition();
    ui->spinBox_dsqProbability->setValue(GlobalSimulationSettings::get()->getBaseDsqProbability());
    delete ui->page_2;
    ui->toolBox->removeItem(1);
    setWindowFlags(Qt::Window);
        setupHillToolBoxItem();

    windsGeneratorSettingsEditor = new WindsGeneratorSettingsEditorWidget(this);
    windsGeneratorSettingsEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    windsGeneratorSettingsEditor->setRemovingSubmitButtons(true);
    windsGeneratorSettingsEditor->removeSubmitButton();
    inrunSnowGeneratorSettingsEditor = new InrunSnowGeneratorSettingsEditorWidget(this);
    inrunSnowGeneratorSettingsEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    inrunSnowGeneratorSettingsEditor->removeSubmitButton();
    competitionRulesEditor = new CompetitionRulesEditorWidget(this);
    competitionRulesEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    competitionRulesEditor->removeSubmitButton();
    competitionRulesEditor->setCompetitionRules(new CompetitionRules(tr("Zasady")));
    competitionRulesEditor->setParent(this);

    ui->toolBox->raise();
    //ui->label_title->raise();
    ui->toolBox->addItem(windsGeneratorSettingsEditor, tr("Ustawienia generatora wiatru"));
    push_button_randomWind = new QPushButton(tr("Losowe ustawienia generatora wiatru"));
    push_button_randomWind->setParent(windsGeneratorSettingsEditor);
    push_button_randomWind->setStyleSheet("QPushButton{\nborder: 2px solid rgb(0, 59, 23);\nborder-radius: 6px;\ncolor: rgb(0, 0, 0);\nbackground-color: rgb(160, 182, 217);\npadding: 2px;\n}\nQPushButton:hover{\nbackground-color: rgb(138, 157, 189);\n}");
    if(QBoxLayout *windEditorLayout = qobject_cast<QBoxLayout *>(windsGeneratorSettingsEditor->layout()))
        windEditorLayout->insertWidget(0, push_button_randomWind, 0, Qt::AlignTop);
    connect(push_button_randomWind, &QPushButton::clicked, this, [this](){
        RandomWindConfigWindow window;
        if(window.exec() == QDialog::Accepted){
            WindGenerationSettings::randomizeSettings(windsGeneratorSettingsEditor->getWindGenerationSettings(), window.getLotability(), window.getDirection());
            windsGeneratorSettingsEditor->fillSettingsInputs();
       }
    });

    ui->toolBox->addItem(inrunSnowGeneratorSettingsEditor, tr("Ustawienia generatora śniegu na najeździe"));
    setupCompetitionRulesToolBoxItem();

    if(getType() == SingleCompetition)
    {
        ui->comboBox_classification->hide();
        ui->comboBox_competition->hide();
        ui->pushButton_defaultStartListOrder->hide();
        ui->pushButton_orderByJumpersList->hide();
        ui->pushButton_byOtherComp->hide();
        ui->label_3->hide();

        for(auto & preset : GlobalDatabase::get()->getJumpsImportancePresetsReference())
        {
            ui->comboBox_jumpsImportancePreset->addItem(preset.getName() + " (" + QString::number(preset.getJumpsImportance(), 'f', 2) + ")");
        }

        connect(ui->comboBox_jumpsImportancePreset, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index){
            if(index > 0)
                ui->doubleSpinBox_jumpsImportance->setValue(GlobalDatabase::get()->getJumpsImportancePresetsReference()[index - 1].getJumpsImportance());
            else
                ui->doubleSpinBox_jumpsImportance->setValue(5);
        });
    }
    else
    {
        ui->label_jumpsImportance->hide();
        ui->doubleSpinBox_jumpsImportance->hide();
        ui->comboBox_jumpsImportancePreset->hide();

        int compType = seasonCompetition->getRulesPointer()->getCompetitionType();
        for(auto & classification : Classification::getSpecificTypeClassifications(simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference(), compType))
            ui->comboBox_classification->addItem(classification->getName());

        QVector<CompetitionInfo *> competitions = CompetitionInfo::getSpecificTypeMainCompetitions(simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference(), compType);
        for(auto & competition : competitions)
        {
            int competitionIndex = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionMainIndex(simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference(), competition); //Index konkursu na głównej liście (bez treningów i serii próbnych)
            if(competitionIndex < SeasonCalendar::getCompetitionMainIndex(simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference(), simulationSave->getNextCompetition())){
                Hill * hill = competition->getHill();
                ui->comboBox_competition->addItem(QIcon(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower())),
                                              QString::number(competitionIndex + 1) + ". " + hill->getName() + " HS" + QString::number(hill->getHSPoint()));
            }
        }

        if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
            ui->pushButton_defaultStartListOrder->hide();
    }

    if(getType() == SingleCompetition)
    {
        competitionJumpers = MyFunctions::convertToVectorOfPointers(
            &GlobalDatabase::get()->getEditableGlobalJumpers());
        competitionTeams = Team::constructTeamsVectorByJumpersList(
            competitionJumpers, competitionRulesEditor->getJumpersCountInTeam());
    }

    jumpersListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonJumpersItems, false, true, true, this);
    jumpersListView->getListView()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    jumpersListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    if(getType() == SingleCompetition)
        jumpersListView->setSeasonJumpers(&competitionJumpers);
    else{
        jumpersListView->setType(DatabaseItemsListView::SeasonJumpersItems);
        bool condition = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference().count() > simulationSave->getNextCompetitionIndex() + 1;
        bool secondCondition = false;
        if(condition)
            secondCondition = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference()[simulationSave->getNextCompetitionIndex() + 1]->getTrialRound() == seasonCompetition && seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual;
        CompetitionInfo * comp;
        if(secondCondition)
            comp = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference()[simulationSave->getNextCompetitionIndex() + 1];
        else
            comp = seasonCompetition;
        //Czy seria próbna następnego konkursu po aktualnym równa się aktualnemu konkursowi

                //Jeżeli tak, przefiltruj na podstawie następnego po aktualnym
        if((comp->getAdvancementCompetition() != nullptr) && seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual){
            if(comp->getAdvancementCompetition()->getCancelled() == false)
                seasonCompetitionJumpers = IndividualCompetitionManager::getFilteredJumpersAfterQualifications(comp, simulationSave->getJumpersReference());
            else{
                for(auto & jp : simulationSave->getJumpersReference())
                {
                    seasonCompetitionJumpers.push_back(jp);
                }
                for(auto & list : simulationSave->getJumpersListsReference())
                {
                    if(list.getIsDefault() == false)
                    {
                        for(auto & jp : list.getJumpersReference()){
                            MyFunctions::removeFromVector(seasonCompetitionJumpers, jp);
                        }
                    }
                    else
                    {
                        for(auto & jp : list.getJumpersReference()){
                            if(seasonCompetitionJumpers.contains(jp) == false)
                                seasonCompetitionJumpers.push_back(jp);
                        }
                    }
                }
            }
        }
        else if(seasonCompetition->getAdvancementClassification() != nullptr && seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            seasonCompetitionJumpers = IndividualCompetitionManager::getFilteredJumpersByClassification(comp, comp->getAdvancementClassification(), simulationSave->getJumpersReference());
        else{
            for(auto & jp : simulationSave->getJumpersReference())
            {
                seasonCompetitionJumpers.push_back(jp);
            }
            for(auto & list : simulationSave->getJumpersListsReference())
            {
                if(list.getIsDefault() == false)
                {
                    for(auto & jp : list.getJumpersReference()){
                        MyFunctions::removeFromVector(seasonCompetitionJumpers, jp);
                    }
                }
                else
                {
                    for(auto & jp : list.getJumpersReference()){
                        if(seasonCompetitionJumpers.contains(jp) == false)
                            seasonCompetitionJumpers.push_back(jp);
                    }
                }
            }
        }
        jumpersListView->setSeasonJumpers(&seasonCompetitionJumpers);
    }
    jumpersListView->setupListModel();
    ui->verticalLayout_startList->addWidget(jumpersListView);

    teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams);
    if(getType() == SingleCompetition)
        teamsSquadsModel->setJumpersInTeam(competitionRulesEditor->getJumpersCountInTeam());
    else teamsSquadsModel->setJumpersInTeam(seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
    teamsSquadsModel->setupTreeItems();
    teamsTreeView = new TeamsSquadsTreeView(teamsSquadsModel, this);
    teamsTreeView->setMinimumWidth(220);
    teamsTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    jumpersListView->setMinimumWidth(220);
    jumpersListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout_startList->addWidget(teamsTreeView);

    if(getType() == SeasonCompetition)
    {
        if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual){
            jumpersListView->setHidden(false);
            teamsTreeView->setHidden(true);
            ui->pushButton_jumpersLists->show();
        }
        else if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team){
            jumpersListView->setHidden(true);
            teamsTreeView->setHidden(false);
            ui->pushButton_jumpersLists->hide();
        }

        ui->pushButton_loadJumpers->hide();
        setWindowTitle(tr("Konfiguracja konkursu"));
        ui->toolBox->widget(0)->hide();
        ui->toolBox->removeItem(0);
        ui->toolBox->widget(2)->hide();
        ui->toolBox->removeItem(2);

        windsGeneratorSettingsEditor->setKPoint(seasonCompetition->getHill()->getKPoint());
        windsGeneratorSettingsEditor->setWindGenerationSettings(new QVector<WindGenerationSettings>());
        windsGeneratorSettingsEditor->fillWindGenerationSettingsByDefault();
        windsGeneratorSettingsEditor->fillSettingsInputs();

        if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
        {
            bool condition = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference().count() > simulationSave->getNextCompetitionIndex() + 1;
            bool secondCondition = false;
            if(condition)
                secondCondition = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference()[simulationSave->getNextCompetitionIndex() + 1]->getTrialRound() == seasonCompetition;
            CompetitionInfo * comp;
            if(secondCondition)
                comp = simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference()[simulationSave->getNextCompetitionIndex() + 1];
            else
                comp = seasonCompetition;
            if(comp->getAdvancementCompetition() != nullptr)
            {
                competitionTeams = TeamCompetitionManager::getFilteredTeamsAfterQualifications(comp);
            }
            else
            {
                competitionTeams = Team::constructTeamsVectorByJumpersList(seasonCompetitionJumpers, seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
                if(seasonCompetition->getAdvancementClassification() != nullptr && seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
                    competitionTeams = TeamCompetitionManager::getFilteredTeamsByClassification(seasonCompetition, seasonCompetition->getAdvancementClassification(), competitionTeams);
            }
            if(seasonCompetition->getTrialRound() != nullptr && seasonCompetition->getTrialRound()->getCancelled() == false)
            {
                competitionTeams = seasonCompetition->getTrialRound()->getTeamsReference();
            }

            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            //teamsTreeView->getTreeView()->expandToDepth(0);
            connect(jumpersListView, &DatabaseItemsListView::remove, this, [this](){
                competitionTeams = Team::constructTeamsVectorByJumpersList(seasonCompetitionJumpers, seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
                teamsSquadsModel->setupTreeItems();
                teamsTreeView->setModel(teamsSquadsModel);
                teamsTreeView->getTreeView()->expandToDepth(0);
            });
        }
    }
    else if(getType() == SingleCompetition)
    {
        checkBox_singleCompetitionQualifications = new QCheckBox(tr("Przeprowadzenie kwalifikacji"), this);
        checkBox_singleCompetitionQualifications->setStyleSheet("color: rgb(30, 30, 30);\nQCheckBox::indicator{\nwidth: 40px;\nheight: 30px;\n}");
        checkBox_singleCompetitionQualifications->setFont(QFont("Quicksand Medium", 12, 650));
        ui->verticalLayout_competitionOptions->insertWidget(0, checkBox_singleCompetitionQualifications);

        setWindowTitle("Konfiguracja pojedynczego konkursu");
    }
    connect(competitionRulesEditor, &CompetitionRulesEditorWidget::jumpersCountInTeamChanged, this, [this](){
        competitionTeams = Team::constructTeamsVectorByJumpersList(competitionJumpers, competitionRulesEditor->getJumpersCountInTeam());
        delete teamsSquadsModel;
        teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, competitionRulesEditor->getJumpersCountInTeam());
        teamsSquadsModel->setupTreeItems();
        teamsTreeView->setModel(teamsSquadsModel);
        teamsTreeView->getTreeView()->expandToDepth(0);
        //emit teamsSquadsModel->dataChanged(teamsSquadsModel->index(0, 0, QModelIndex()), teamsSquadsModel->createIndex());
    });

    connect(hillEditor, &HillEditorWidget::KPointInputChanged, this, [this](){
        windsGeneratorSettingsEditor->setKPoint(hillEditor->getKPointFromInput());
        windsGeneratorSettingsEditor->setWindGenerationSettings(new QVector<WindGenerationSettings>());
        windsGeneratorSettingsEditor->fillWindGenerationSettingsByDefault();
        windsGeneratorSettingsEditor->fillSettingsInputs();
    });
    connect(competitionRulesEditor, &CompetitionRulesEditorWidget::competitionTypeChanged, this, [this](){
        if(competitionRulesEditor->getCompetitionTypeFromInput() == CompetitionRules::Individual){
            jumpersListView->setHidden(false);
            teamsTreeView->setHidden(true);
            if(seasonCompetition != nullptr)
                ui->pushButton_jumpersLists->show();
            else
                ui->pushButton_jumpersLists->hide();
        }
        else if(competitionRulesEditor->getCompetitionTypeFromInput() == CompetitionRules::Team){
            jumpersListView->setHidden(true);
            teamsTreeView->setHidden(false);
            ui->pushButton_jumpersLists->hide();
        }
    });

    connect(teamsTreeView, &TeamsSquadsTreeView::needToUpdateModel, this, [this]{
        int count = competitionRulesEditor->getJumpersCountInTeam();
        if(seasonCompetition != nullptr)
            count = seasonCompetition->getRulesPointer()->getJumpersInTeamCount();
        teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, count );
        teamsSquadsModel->setupTreeItems();
        teamsTreeView->setModel(teamsSquadsModel);
    });


    connect(teamsTreeView, &TeamsSquadsTreeView::treeViewDoubleClicked, this, [this](const QModelIndex & index){
        if(getType() == SeasonCompetition)
            if(seasonCompetition->getAdvancementCompetition() != nullptr || seasonCompetition->getAdvancementClassification() != nullptr)
                return;
        TreeItem * item = static_cast<TreeItem *>(index.internalPointer());
        int teamIndex = 0;
        if(item->getParentItem() == teamsSquadsModel->getRootItem()){
            teamIndex = item->row();
        }
        else{
            teamIndex = item->getParentItem()->row();
        }

        TeamEditorWidget * editor = new TeamEditorWidget(&competitionTeams[teamIndex]);
        if(getType() == SingleCompetition)
            editor->setJumpersCount(competitionRulesEditor->getJumpersCountInTeam());
        else editor->setJumpersCount(seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
        editor->getModel()->setJumpersInTeamCount(editor->getJumpersCount());

        editor->setAttribute(Qt::WA_DeleteOnClose);
        editor->fillWidgetInputs();
        editor->show();
        QDialog dialog(this);
        dialog.setLayout(new QHBoxLayout());
        dialog.layout()->addWidget(editor);
        dialog.setModal(true);
        connect(editor, &TeamEditorWidget::submitted, &dialog, &QDialog::accept);
        if(dialog.exec() == QDialog::Accepted){
            competitionTeams[teamIndex] = editor->constructTeamFromWidgetInput();
            delete teamsSquadsModel;
            teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams);
            if(getType() == SingleCompetition)
                teamsSquadsModel->setJumpersInTeam(competitionRulesEditor->getJumpersCountInTeam());
            else teamsSquadsModel->setJumpersInTeam(seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            teamsTreeView->getTreeView()->expandToDepth(0);
        }
    });

    ui->verticalLayout_startList->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum));

    if(getType() == SingleCompetition)
        emit competitionRulesEditor->competitionTypeChanged();

    KOGroupsList = new KOGroupsListView(this);
    if(getType() == SingleCompetition)
    {
        KOGroupsList->setJumpersList(&competitionJumpers);
    }
    else{
        KOGroupsList->setJumpersList(&seasonCompetitionJumpers);
    }
    KOGroupsList->hide();
    comboBox_groupsSelectionType = new QComboBox(this);
    comboBox_groupsSelectionType->addItem(tr("Dobierz grupy"));
    comboBox_groupsSelectionType->addItem(tr("Klasycznie"));
    comboBox_groupsSelectionType->addItem(tr("Klasycznie (Dla dużych grup)"));
    comboBox_groupsSelectionType->addItem(tr("Losowo (Z podziałem na koszyki)"));
    comboBox_groupsSelectionType->addItem(tr("Losowo"));
    ui->verticalLayout_startList->addWidget(comboBox_groupsSelectionType);
    comboBox_groupsSelectionType->hide();
    ui->verticalLayout_startList->addWidget(KOGroupsList);

    if(getType() == SingleCompetition){
        /*RoundInfo roundInfo;//= competitionRulesEditor->getRoundsFromInput().at(0);
        roundInfo.setAdvancingFromKOGroup(1);
        roundInfo.setCountInKOGroup(2);
        roundInfo.setKO(true);
        if(competitionRulesEditor->getCompetitionTypeFromInput() == CompetitionRules::Individual){
            QVector<Jumper *> jms = MyFunctions::convertToVectorOfPointers(&competitionJumpers);
            competitionGroups = KOGroup::constructDefaultKOGroups(&roundInfo, &jms, KOGroup::Random, nullptr);
        }*/
        KOGroupsList->setKOGroups(&competitionGroups);
    }
    else{
        KOGroupsList->setKOGroups(&seasonCompetitionGroups);
    }

    if(getType() == SingleCompetition){
        if(competitionRulesEditor->getRoundsFromInput().count() > 0){
            if(competitionRulesEditor->getRoundsFromInput().at(0).getKO() == true)
            {
                jumpersListView->hide();
                teamsTreeView->hide();
                KOGroupsList->show();
                comboBox_groupsSelectionType->show();
                KOGroupsList->fillListLayout();
            }
            else{
                KOGroupsList->hide();
                comboBox_groupsSelectionType->hide();
                emit competitionRulesEditor->competitionTypeChanged();
            }
        }
    }
    else{
        if(seasonCompetition != nullptr){
            if(seasonCompetition->getRulesPointer()->getRoundsReference().isEmpty() == false && seasonCompetition->getRulesPointer()->getRoundsReference()[0].getKO() == true)
            {
                jumpersListView->hide();
                teamsTreeView->hide();
                KOGroupsList->show();
                comboBox_groupsSelectionType->show();

                seasonCompetitionGroups = KOGroup::constructKOGroups(&seasonCompetition->getRulesPointer()->getRoundsReference()[0], &seasonCompetitionJumpers, seasonCompetition->getRulesPointer()->getRoundsReference()[0].getKoGroupsSelectionType(), seasonCompetition);
                KOGroupsList->fillListLayout();
            }
            else{
                KOGroupsList->hide();
                comboBox_groupsSelectionType->hide();
            }
        }
    }
    connect(competitionRulesEditor, &CompetitionRulesEditorWidget::KORoundChanged, this, [this](){
        if(competitionRulesEditor->getRoundsFromInput().count() > 0){
            if (competitionRulesEditor->getRoundsFromInput().at(0).getKO() == true
                && checkBox_singleCompetitionQualifications->isChecked() == false) {
                jumpersListView->hide();
                teamsTreeView->hide();
                KOGroupsList->show();
                comboBox_groupsSelectionType->show();
                KOGroupsList->fillListLayout();
            } else {
                KOGroupsList->hide();
                comboBox_groupsSelectionType->hide();
                emit competitionRulesEditor->competitionTypeChanged();
            }
        }
    });
    if(type == SingleCompetition){
    connect(checkBox_singleCompetitionQualifications, &QCheckBox::stateChanged, this, [this, type]() {     
        if (competitionRulesEditor->getRoundsFromInput().count() > 0) {
            if (competitionRulesEditor->getRoundsFromInput().at(0).getKO() == true
                && checkBox_singleCompetitionQualifications->isChecked() == false) {
                jumpersListView->hide();
                teamsTreeView->hide();
                KOGroupsList->show();
                comboBox_groupsSelectionType->show();
                KOGroupsList->fillListLayout();
            } else {
                KOGroupsList->hide();
                comboBox_groupsSelectionType->hide();
                emit competitionRulesEditor->competitionTypeChanged();
            }
        }
        });
    }
    if(getType() == SingleCompetition)
        emit competitionRulesEditor->KORoundChanged();

    connect(comboBox_groupsSelectionType, qOverload<int>(&QComboBox::activated), this, [this](int index){
        if(index > 0)
        {
            index--;
            if(getType() == SingleCompetition)
            {
                if(index != KOGroup::Classic || (index == KOGroup::Classic || (competitionRulesEditor->getRoundsFromInput().at(0).getCountInKOGroup() == 2)))
                {
                    RoundInfo roundInfo = competitionRulesEditor->getRoundsFromInput().at(0);
                    competitionGroups = KOGroup::constructKOGroups(&roundInfo, &competitionJumpers, index, nullptr);
                }
            }
            else if(index != KOGroup::Classic || (index == KOGroup::Classic || (seasonCompetition->getRulesPointer()->getRoundsReference().isEmpty() == false && seasonCompetition->getRulesPointer()->getRoundsReference()[0].getCountInKOGroup() == 2)))
            {
                seasonCompetitionGroups = KOGroup::constructKOGroups(&seasonCompetition->getRulesPointer()->getRoundsReference()[0], &seasonCompetitionJumpers, index, seasonCompetition);
            }
            KOGroupsList->fillListLayout();
        }
    });

    updateResponsiveLayout(width());
    ResponsiveWindowUtils::manageWindowGeometry(this, "CompetitionConfigWindow");
}

CompetitionConfigWindow::~CompetitionConfigWindow()
{
    delete ui;
}

int CompetitionConfigWindow::getJumpersCountInTeam() const
{
    return competitionRulesEditor->getJumpersCountInTeam();
}

int CompetitionConfigWindow::getStartingGateFromInput() const
{
    return ui->spinBox_startGate->value();
}

int CompetitionConfigWindow::getBaseDSQProbability() const
{
    return ui->spinBox_dsqProbability->value();
}

WindsGeneratorSettingsEditorWidget *CompetitionConfigWindow::getWindGeneratorSettingsWidget()
{
    return windsGeneratorSettingsEditor;
}

InrunSnowGeneratorSettingsEditorWidget *CompetitionConfigWindow::getInrunSnowGeneratorSettingsWidget()
{
    return inrunSnowGeneratorSettingsEditor;
}

void CompetitionConfigWindow::setClassificationsComboBoxIndex(int index)
{
    ui->comboBox_classification->setCurrentIndex(index);
    emit ui->comboBox_classification->activated(index);
}

void CompetitionConfigWindow::setupHillToolBoxItem()
{
    ui->comboBox_existingHill->clear();
    ui->comboBox_existingHill->addItem(tr("BRAK"));
    for(const auto & hill : GlobalDatabase::get()->getEditableGlobalHills())
    {
        ui->comboBox_existingHill->addItem(QIcon(QPixmap(CountryFlagsManager::getFlagPixmap(hill.getCountryCode().toLower()))) ,hill.getName() + " HS" + QString::number(hill.getHSPoint()));
    }

    hillEditor = new HillEditorWidget(this);
    hillEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    hillEditor->removeSubmitButton();
    ui->verticalLayout_hillEditorWidget->addWidget(hillEditor);

    connect(ui->comboBox_existingHill, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){
        int selected = ui->comboBox_existingHill->currentIndex();
        if(selected > 0){
            hillEditor->setHill(&GlobalDatabase::get()->getEditableGlobalHills()[selected - 1]);
            hillEditor->fillHillInputs();
            emit hillEditor->KPointInputChanged(hillEditor->getHill()->getKPoint());
        }
        else hillEditor->resetHillInputs();
    });
}

void CompetitionConfigWindow::setupCompetitionRulesToolBoxItem()
{
    existingCompetitionRulesComboBox = new QComboBox();
    QStringList rulesStringList;
    rulesStringList.push_back(tr("BRAK"));
    for(const auto & rule : GlobalDatabase::get()->getEditableCompetitionRules())
    {
        rulesStringList.push_back(rule.getName());
    }
    QStringListModel * existingCompetitionRulesModel = new QStringListModel(rulesStringList, this);
    existingCompetitionRulesComboBox->setModel(existingCompetitionRulesModel);

    competitionRulesParentWidget = new QWidget(this);
    competitionRulesToolBoxItemLayout = new QVBoxLayout(competitionRulesParentWidget);
    competitionRulesToolBoxItemLayout->setAlignment(Qt::AlignTop);
    existingRulesLabelAndComboBoxLayout = new QVBoxLayout();
    competitionRulesToolBoxItemLayout->addLayout(existingRulesLabelAndComboBoxLayout);
    //competitionRulesToolBoxItemLayout->addSpacerItem(new QSpacerItem(35, 35, QSizePolicy::Maximum, QSizePolicy::Maximum));
    competitionRulesToolBoxItemLayout->addWidget(competitionRulesEditor);
    //competitionRulesToolBoxItemLayout->addSpacerItem(new QSpacerItem(35, 35, QSizePolicy::Expanding, QSizePolicy::Expanding));

    existingCompetitionRulesLabel = new QLabel(tr("Istniejące zasady konkursu"), competitionRulesParentWidget);
    existingCompetitionRulesLabel->setStyleSheet("color: rgb(20, 20, 20);");
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("Quicksand [PfEd]");
    existingCompetitionRulesLabel->setFont(font);
    existingRulesLabelAndComboBoxLayout->addWidget(existingCompetitionRulesLabel);
    existingRulesLabelAndComboBoxLayout->addWidget(existingCompetitionRulesComboBox);

    ui->toolBox->addItem(competitionRulesParentWidget, tr("Zasady konkursu"));

    connect(existingCompetitionRulesComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){
        int selected = existingCompetitionRulesComboBox->currentIndex();
        if(selected > 0){
            CompetitionRules * rules = &GlobalDatabase::get()->getEditableCompetitionRules()[selected - 1];
            competitionRulesEditor->setCompetitionRules(rules);
            competitionRulesEditor->fillCompetitionRulesInputs();
        }
        else competitionRulesEditor->resetInputs();
    });
}

QVector<KOGroup> CompetitionConfigWindow::getSeasonCompetitionGroups() const
{
    return seasonCompetitionGroups;
}

QVector<KOGroup> &CompetitionConfigWindow::getSeasonCompetitionGroupsReference()
{
    return seasonCompetitionGroups;
}

void CompetitionConfigWindow::setSeasonCompetitionGroups(const QVector<KOGroup> &newSeasonCompetitionGroups)
{
    seasonCompetitionGroups = newSeasonCompetitionGroups;
}

QVector<KOGroup> CompetitionConfigWindow::getCompetitionGroups() const
{
    return competitionGroups;
}

void CompetitionConfigWindow::setCompetitionGroups(const QVector<KOGroup> &newCompetitionGroups)
{
    competitionGroups = newCompetitionGroups;
}

QVector<Jumper *> CompetitionConfigWindow::getSeasonCompetitionJumpers() const
{
    return seasonCompetitionJumpers;
}

void CompetitionConfigWindow::setSeasonCompetitionJumpers(const QVector<Jumper *> &newSeasonCompetitionJumpers)
{
    seasonCompetitionJumpers = newSeasonCompetitionJumpers;
}

QVector<KOGroup> &CompetitionConfigWindow::getCompetitionGroupsReference()
{
    return competitionGroups;
}

SimulationSave *CompetitionConfigWindow::getSimulationSave() const
{
    return simulationSave;
}

void CompetitionConfigWindow::setSimulationSave(SimulationSave *newSimulationSave)
{
    simulationSave = newSimulationSave;
}

CompetitionInfo *CompetitionConfigWindow::getSeasonCompetition() const
{
    return seasonCompetition;
}

void CompetitionConfigWindow::setSeasonCompetition(CompetitionInfo *newSeasonCompetition)
{
    seasonCompetition = newSeasonCompetition;
}

TeamsSquadsTreeModel *CompetitionConfigWindow::getTeamsSquadsModel() const
{
    return teamsSquadsModel;
}

void CompetitionConfigWindow::setTeamsSquadsModel(TeamsSquadsTreeModel *newTeamsSquadsModel)
{
    teamsSquadsModel = newTeamsSquadsModel;
}

void CompetitionConfigWindow::setCompetitionJumpers(const QVector<Jumper *> &newCompetitionJumpers)
{
    competitionJumpers = newCompetitionJumpers;
}

QVector<Team> CompetitionConfigWindow::getCompetitionTeams() const
{
    return competitionTeams;
}

QVector<Team> &CompetitionConfigWindow::getCompetitionTeamsReference()
{
    return competitionTeams;
}

void CompetitionConfigWindow::setCompetitionTeams(const QVector<Team> &newCompetitionTeams)
{
    competitionTeams = newCompetitionTeams;
}

QVector<Jumper *> CompetitionConfigWindow::getCompetitionJumpers() const
{
    return competitionJumpers;
}

QVector<Jumper *> &CompetitionConfigWindow::getCompetitionJumpersReference()
{
    return competitionJumpers;
}

HillEditorWidget *CompetitionConfigWindow::getHillEditor() const
{
    return hillEditor;
}

void CompetitionConfigWindow::setHillEditor(HillEditorWidget *newHillEditor)
{
    hillEditor = newHillEditor;
}

CompetitionRulesEditorWidget *CompetitionConfigWindow::getCompetitionRulesEditor() const
{
    return competitionRulesEditor;
}

void CompetitionConfigWindow::setCompetitionRulesEditor(CompetitionRulesEditorWidget *newCompetitionRulesEditor)
{
    competitionRulesEditor = newCompetitionRulesEditor;
}

WindsGeneratorSettingsEditorWidget *CompetitionConfigWindow::getWindGeneratorSettingsEditor()
{
    return windsGeneratorSettingsEditor;
}

void CompetitionConfigWindow::setWindGeneratorSettingsEditor(WindsGeneratorSettingsEditorWidget *newWindsGeneratorSettingsEditor)
{
    windsGeneratorSettingsEditor = newWindsGeneratorSettingsEditor;
}

short CompetitionConfigWindow::getType() const
{
    return type;
}

void CompetitionConfigWindow::setType(short newType)
{
    type = newType;
}

bool CompetitionConfigWindow::validateSimulationInputs()
{
    CompetitionRules rules = getType() == SeasonCompetition
        ? *seasonCompetition->getRulesPointer()
        : competitionRulesEditor->getCompetitionRulesFromWidgetInputs();

    if(rules.getRoundsReference().isEmpty())
    {
        QMessageBox::warning(this, tr("Brak serii"),
                             tr("Konkurs musi zawierać co najmniej jedną serię."), QMessageBox::Ok);
        return false;
    }

    if(getType() == SingleCompetition)
    {
        const Hill hill = hillEditor->getHillFromWidgetInput();
        if(hill.getKPoint() <= 0 || hill.getHSPoint() <= 0 || hill.getHSPoint() < hill.getKPoint())
        {
            QMessageBox::warning(this, tr("Nieprawidłowa skocznia"),
                                 tr("Wybierz lub skonfiguruj skocznię z dodatnim punktem K oraz punktem HS nie mniejszym od K."), QMessageBox::Ok);
            return false;
        }
    }

    if(rules.getCompetitionType() == CompetitionRules::Individual)
    {
        const QVector<Jumper *> &jumpers = getType() == SeasonCompetition
            ? seasonCompetitionJumpers
            : competitionJumpers;
        bool validStartList = !jumpers.isEmpty();
        for(Jumper *jumper : jumpers)
            validStartList = validStartList && jumper != nullptr && !jumper->getNameAndSurname().trimmed().isEmpty();
        if(!validStartList)
        {
            QMessageBox::warning(this, tr("Pusta lista startowa"),
                                 tr("Dodaj co najmniej jednego prawidłowego zawodnika przed rozpoczęciem konkursu."), QMessageBox::Ok);
            return false;
        }

        const RoundInfo &firstRound = rules.getRoundsReference().first();
        const bool groupsWillBeGeneratedAfterQualifications =
            getType() == SingleCompetition
            && checkBox_singleCompetitionQualifications != nullptr
            && checkBox_singleCompetitionQualifications->isChecked();
        if(firstRound.getKO() && !groupsWillBeGeneratedAfterQualifications)
        {
            QVector<KOGroup> &groups = getType() == SeasonCompetition
                ? seasonCompetitionGroups
                : competitionGroups;
            const QVector<Jumper *> groupedJumpers = KOGroup::getJumpersFromGroups(&groups);
            bool completeGroups = !groups.isEmpty() && groupedJumpers.count() == jumpers.count();
            for(Jumper *jumper : jumpers)
                completeGroups = completeGroups && groupedJumpers.count(jumper) == 1;
            if(!completeGroups)
            {
                QMessageBox::warning(this, tr("Niekompletne grupy KO"),
                                     tr("Wybierz sposób tworzenia grup i upewnij się, że każdy zawodnik znajduje się w grupie KO."), QMessageBox::Ok);
                return false;
            }
        }
    }
    else
    {
        if(competitionTeams.isEmpty())
        {
            QMessageBox::warning(this, tr("Brak drużyn"),
                                 tr("Dodaj co najmniej jedną drużynę przed rozpoczęciem konkursu."), QMessageBox::Ok);
            return false;
        }

        const int requiredJumpers = rules.getJumpersInTeamCount();
        if(requiredJumpers <= 0)
        {
            QMessageBox::warning(this, tr("Nieprawidłowy skład drużyny"),
                                 tr("Liczba zawodników w drużynie musi być większa od zera."), QMessageBox::Ok);
            return false;
        }
        for(Team &team : competitionTeams)
        {
            bool validTeam = team.getJumpersReference().count() >= requiredJumpers;
            for(Jumper *jumper : team.getJumpersReference())
                validTeam = validTeam && jumper != nullptr && !jumper->getNameAndSurname().trimmed().isEmpty();
            if(!validTeam)
            {
                QMessageBox::warning(this, tr("Niekompletna drużyna"),
                                     tr("Każda drużyna musi mieć co najmniej %1 zawodników.").arg(requiredJumpers), QMessageBox::Ok);
                return false;
            }
        }
    }

    return true;
}

void CompetitionConfigWindow::on_pushButton_submit_clicked()
{
    if(!validateSimulationInputs())
        return;

    emit submitted();
    if(getType() == SeasonCompetition)
        accept();
    switch(getType())
    {
    case SingleCompetition:{
        CompetitionInfo qualsInfo;
        qualsInfo.setHill(new Hill(hillEditor->getHillFromWidgetInput()));
        qualsInfo.setRules(competitionRulesEditor->getCompetitionRulesFromWidgetInputs());
        qualsInfo.setSerieType(CompetitionInfo::Qualifications);
        qualsInfo.setExceptionalRoundsCount(1);
        qualsInfo.setJumpsImportance(ui->doubleSpinBox_jumpsImportance->value() / 1.373015873015873);
        int type = competitionRulesEditor->getCompetitionTypeFromInput();

        for(auto & team : competitionTeams)
            Team::cutTeamJumpers(&team, competitionRulesEditor->getJumpersCountInTeam());

        AbstractCompetitionManager * competitionManager = nullptr;
        if(type == CompetitionRules::Individual)
            competitionManager = new IndividualCompetitionManager();
        else if(type == CompetitionRules::Team)
            competitionManager = new TeamCompetitionManager();

        AbstractCompetitionManager * qualificationsManager = nullptr;
        if(checkBox_singleCompetitionQualifications->isChecked() == true){
            if(type == CompetitionRules::Individual)
                qualificationsManager = new IndividualCompetitionManager();
            else if(type == CompetitionRules::Team)
                qualificationsManager = new TeamCompetitionManager();

            qualificationsManager->getRoundsStartingGatesReference().push_back(ui->spinBox_startGate->value());

            if(type == CompetitionRules::Individual){
                static_cast<IndividualCompetitionManager *>(qualificationsManager)->getRoundsJumpersReference().push_back(competitionJumpers);
                qualsInfo.getRoundsKOGroupsReference().push_back(QVector<KOGroup>());
                static_cast<IndividualCompetitionManager *>(qualificationsManager)->getRoundsKOGroupsReference().push_back(&qualsInfo.getRoundsKOGroupsReference().last());
                //if(competitionRulesEditor->getRoundsFromInput().at(0).getKO() == true)
                  //  static_cast<IndividualCompetitionManager *>(qualificationsManager)->getRoundsKOGroupsReference().push_back(competitionGroups);
            }
            else if(type == CompetitionRules::Team){
                qualsInfo.setTeams(competitionTeams);
                qualsInfo.getTeamsReference().detach();
                QVector<Team *> teams;
                for(auto & t : qualsInfo.getTeamsReference())
                    teams.push_back(&t);
                static_cast<TeamCompetitionManager *>(qualificationsManager)->getRoundsTeamsReference().push_back(teams);
            }

            qualificationsManager->setCompetitionInfo(&qualsInfo);
            qualificationsManager->getCompetitionInfo()->setSerieType(CompetitionInfo::Qualifications);
            qualificationsManager->setCompetitionRules(qualsInfo.getRulesPointer());
            qualificationsManager->setResults(&qualsInfo.getResultsReference());
            qualificationsManager->setActualGate(ui->spinBox_startGate->value());
            qualificationsManager->getRoundsStartingGatesReference().push_back(qualificationsManager->getActualGate());
            qualificationsManager->setActualRound(1);
            if(type == CompetitionRules::Team)
                static_cast<TeamCompetitionManager *>(qualificationsManager)->setActualGroup(1);

            if(type == CompetitionRules::Team)
                dynamic_cast<TeamCompetitionManager *>(qualificationsManager)->setupStartListStatusesForActualRound(true);
            else
                qualificationsManager->setupStartListStatusesForActualRound();
            qualificationsManager->setActualStartListIndex(0);
            qualificationsManager->setBaseDSQProbability(ui->spinBox_dsqProbability->value());
            qualificationsManager->setWindGenerationSettings(windsGeneratorSettingsEditor->getWindsGenerationSettingsFromInputs());
            QVector<RoundInfo> qualsRounds = {
                RoundInfo(0, false),
                RoundInfo(qualificationsManager->getCompetitionRules()->getRounds().at(0).getCount(), qualificationsManager->getCompetitionRules()->getRounds().at(0).getSortStartList())
            };
            qualsRounds.last().setKO(false);
            qualificationsManager->getCompetitionRules()->setRounds(qualsRounds);

            CompetitionManagerWindow * qualsWindow = new CompetitionManagerWindow(qualificationsManager, this, true);
            qualsWindow->setInrunSnowGenerator(InrunSnowGenerator(inrunSnowGeneratorSettingsEditor->getBase(), inrunSnowGeneratorSettingsEditor->getDeviation()));
            connect(qualsWindow->getManager(), &AbstractCompetitionManager::competitionEnd, qualsWindow, [qualsWindow](){
                qualsWindow->showMessageBoxForQualificationsEnd();
            });
            if(qualsWindow->exec() == QDialog::Accepted)
            {
            }
        }

        CompetitionInfo info;
        info.setHill(new Hill(hillEditor->getHillFromWidgetInput()));
        info.setRules(competitionRulesEditor->getCompetitionRulesFromWidgetInputs());
        info.setSerieType(CompetitionInfo::Competition);
        info.setJumpsImportance(ui->doubleSpinBox_jumpsImportance->value());

        //competitionManager->setStartingJumpers(startListDisplayWidget->getIndividualCompetitionJumpers());
        //Wypełnić

        competitionManager->setCompetitionInfo(&info);
        competitionManager->setCompetitionRules(info.getRulesPointer());
        competitionManager->setResults(&info.getResultsReference());
        competitionManager->setActualGate(ui->spinBox_startGate->value());
        competitionManager->setActualRound(1);
        if(type == CompetitionRules::Team)
            static_cast<TeamCompetitionManager *>(competitionManager)->setActualGroup(1);
        competitionManager->setBaseDSQProbability(ui->spinBox_dsqProbability->value());
        competitionManager->setWindGenerationSettings(windsGeneratorSettingsEditor->getWindsGenerationSettingsFromInputs());

        if(qualsInfo.getCancelled() == true)
            return;
        if(qualsInfo.getResultsReference().getResultsReference().count() > 0 && qualsInfo.getCancelled() == false){
            if(type == CompetitionRules::Individual){
                if (competitionRulesEditor->getRoundsFromInput().at(0).getKO() == true) {
                    RoundInfo rinf = competitionRulesEditor->getRoundsFromInput().at(0);

                    bool sortStartList = qualificationsManager->getCompetitionRules()
                                             ->getRoundsReference()[1]
                                             .getSortStartList();
                    qualificationsManager->getCompetitionRules()->getRoundsReference()[1].setSortStartList(true);
                    qualificationsManager->getCompetitionRules()->getRoundsReference()[0].setSortStartList(true);
                    QVector<Jumper *> jumpers = static_cast<IndividualCompetitionManager *>(qualificationsManager)->getFilteredJumpersForNextRound();
                    qualificationsManager->getCompetitionRules()->getRoundsReference()[1].setSortStartList(sortStartList);
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(jumpers);

                    info.getRoundsKOGroupsReference().push_back(KOGroup::constructKOGroups(&rinf, &static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().last(), rinf.getKoGroupsSelectionType(), competitionManager->getCompetitionInfo()));
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().last() = KOGroup::getJumpersFromGroups(&info.getRoundsKOGroupsReference().last());
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&info.getRoundsKOGroupsReference().last());
                }
                else
                {
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(static_cast<IndividualCompetitionManager *>(qualificationsManager)->getFilteredJumpersForNextRound());
                    info.getRoundsKOGroupsReference().push_back(QVector<KOGroup>());
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&info.getRoundsKOGroupsReference().last());
                }



                /*static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(static_cast<IndividualCompetitionManager *>(qualificationsManager)->getFilteredJumpersForNextRound());
                info.getRoundsKOGroupsReference().push_back(QVector<KOGroup>());
                static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&info.getRoundsKOGroupsReference().last());*/
            }
            else if(type == CompetitionRules::Team){
                info.setTeams(competitionTeams);
                static_cast<TeamCompetitionManager *>(qualificationsManager)->updateTeamsVectorByQualifications(info.getTeamsReference());

                static_cast<TeamCompetitionManager *>(competitionManager)->getRoundsTeamsReference().push_back(MyFunctions::convertToVectorOfPointers(&info.getTeamsReference()));
            }
            competitionManager->setActualGate(qualificationsManager->getActualGate());
            competitionManager->setWindGenerationSettings(qualificationsManager->getWindGenerationSettingsReference());
        }
        else{
            if(type == CompetitionRules::Individual){
                if(competitionRulesEditor-> getRoundsFromInput().at(0).getKO() == true){
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(KOGroup::getJumpersFromGroups(&competitionGroups));
                    info.getRoundsKOGroupsReference().push_back(competitionGroups);
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&info.getRoundsKOGroupsReference().last());
                }
                else
                {
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsJumpersReference().push_back(competitionJumpers);
                    info.getRoundsKOGroupsReference().push_back(QVector<KOGroup>());
                    static_cast<IndividualCompetitionManager *>(competitionManager)->getRoundsKOGroupsReference().push_back(&info.getRoundsKOGroupsReference().last());
                }
            }
            else if(type == CompetitionRules::Team){
                info.setTeams(competitionTeams);
                info.getTeamsReference().detach();
                QVector<Team *> teams;
                for(auto & t : info.getTeamsReference())
                    teams.push_back(&t);
                static_cast<TeamCompetitionManager *>(competitionManager)->getRoundsTeamsReference().push_back(teams);
            }
        }
        if(type == CompetitionRules::Team)
            dynamic_cast<TeamCompetitionManager *>(competitionManager)->setupStartListStatusesForActualRound(true);
        else
            competitionManager->setupStartListStatusesForActualRound();
        competitionManager->setActualStartListIndex(0);
        competitionManager->getRoundsStartingGatesReference().push_back(competitionManager->getActualGate());

        CompetitionManagerWindow * window = new CompetitionManagerWindow(competitionManager, this, true);
        window->setInrunSnowGenerator(InrunSnowGenerator(inrunSnowGeneratorSettingsEditor->getBase(), inrunSnowGeneratorSettingsEditor->getDeviation()));
        if(window->exec() == QDialog::Accepted)
        {
            this->setModal(true);
        }
        competitionTeams = Team::constructTeamsVectorByJumpersList(competitionJumpers, competitionRulesEditor->getJumpersCountInTeam());

        QDate date = QDate::currentDate();
        QTime time = QTime::currentTime();
        QString name = QString("%1-%2-%3 %4 (%5-%6)").arg(QString::number(date.day())).arg(QString::number(date.month())).arg(QString::number(date.year())).arg(info.getHill()->getName() + " HS" + QString::number(info.getHill()->getHSPoint())).arg(QString::number(time.hour())).arg(QString::number(time.minute()));
        info.saveToCsvFile("results/single-competitions/csv/", name + ".csv");
        if(qualificationsManager != nullptr){
            qualsInfo.saveToCsvFile("results/single-competitions/csv/", name + " (Q)"+".csv");
        }

        Hill * hill = nullptr;
        if(ui->comboBox_existingHill->currentIndex() > 0)
            hill = &GlobalDatabase::get()->getEditableGlobalHills()[ui->comboBox_existingHill->currentIndex() - 1];
        if(hill != nullptr && GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords() == true){
            if(qualsInfo.getHill()->getRecord() > hill->getRecord())
            hill->setRecord(qualsInfo.getHill()->getRecord());
            if(info.getHill()->getRecord() > hill->getRecord())
                hill->setRecord(info.getHill()->getRecord());
        }
        if(GlobalSimulationSettings::get()->getUpdateGlobalDatabaseRecords() == true)
        {
            for(auto & jumper : competitionJumpers)
            {
                Jumper * globalJumper = MyFunctions::getVectorItemByID(GlobalDatabase::get()->getEditableGlobalJumpers(), jumper->getID());
                if(jumper->getPersonalBest() > globalJumper->getPersonalBest())
                    globalJumper->setPersonalBest(jumper->getPersonalBest());
            }
        }

        delete info.getHill();
        delete competitionManager;
        if(qualificationsManager != nullptr)
            delete qualificationsManager;
    }
        break;
    }

}

void CompetitionConfigWindow::updateResponsiveLayout(int windowWidth)
{
    const bool useCompactLayout = windowWidth < 1100;
    if(useCompactLayout == compactLayout)
        return;

    compactLayout = useCompactLayout;
    ui->splitter_configuration->setOrientation(compactLayout ? Qt::Vertical : Qt::Horizontal);
    if(compactLayout)
        ui->splitter_configuration->setSizes({190, qMax(180, height() / 2), 150});
    else
        ui->splitter_configuration->setSizes({260, qMax(440, windowWidth - 540), 280});
}

void CompetitionConfigWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateResponsiveLayout(event->size().width());
}

void CompetitionConfigWindow::on_pushButton_loadJumpers_clicked()
{
    if(getType() == SingleCompetition){
        QUrl fileUrl = QFileDialog::getOpenFileUrl(this, tr("Wybierz plik z zawodnikami"), QUrl(), "JSON (*.json)");
        if(fileUrl.isEmpty())
            return;

        QFile file(fileUrl.toLocalFile());
        if(!(file.open(QIODevice::ReadOnly | QIODevice::Text)))
        {
            QMessageBox message(QMessageBox::Icon::Critical, tr("Nie można otworzyć pliku z zawodnikami"), tr("Nie udało się otworzyć wybranego pliku\nUpewnij się, że istnieje tam taki plik lub ma on odpowiednie uprawnienia"),  QMessageBox::StandardButton::Ok);
            message.setModal(true);
            message.exec();
            return;
        }
        const QByteArray contents = file.readAll();
        file.close();
        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(contents, &parseError);
        if(parseError.error != QJsonParseError::NoError || !document.isObject() || !document.object().value("jumpers").isArray())
        {
            QMessageBox::warning(this, tr("Nieprawidłowy plik zawodników"),
                                 tr("Wybrany plik nie zawiera prawidłowej listy zawodników JSON.\n%1")
                                     .arg(parseError.error == QJsonParseError::NoError ? QString() : parseError.errorString()),
                                 QMessageBox::Ok);
            return;
        }
        QVector<Jumper> jumpers = Jumper::getVectorFromJson(contents);
        if(jumpers.isEmpty() == false){
            competitionJumpers.clear();
            for(auto & j : jumpers)
                competitionJumpers.push_back(new Jumper(j));
            jumpersListView->setSeasonJumpers(&competitionJumpers);
            delete jumpersListView->getListModel();
            jumpersListView->setupListModel();

            competitionTeams = Team::constructTeamsVectorByJumpersList(competitionJumpers, competitionRulesEditor->getJumpersCountInTeam());
            delete teamsSquadsModel;
            teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, competitionRulesEditor->getJumpersCountInTeam());
            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            teamsTreeView->getTreeView()->expandToDepth(0);
        }
        else
            QMessageBox::warning(this, tr("Pusta lista zawodników"),
                                 tr("Wybrany plik nie zawiera żadnych prawidłowych zawodników."), QMessageBox::Ok);
    }
}

void CompetitionConfigWindow::on_comboBox_competition_activated(int index)
{
    if(index > 0)
    {
        index--; //Nie liczymy pierwszego item'a - "Według konkursu"
        int type = seasonCompetition->getRulesPointer()->getCompetitionType();
        QVector<CompetitionInfo *> competitions = CompetitionInfo::getSpecificTypeMainCompetitions(simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference(), type);
        CompetitionInfo * competition = SeasonCalendar::getMainCompetitionByIndex(competitions, index);
        if(type == CompetitionRules::Individual){
            IndividualCompetitionManager::setStartListOrderByCompetitionResults(seasonCompetitionJumpers, competition);
            if(seasonCompetition->getRulesPointer()->getRoundsReference().isEmpty() == false && seasonCompetition->getRulesPointer()->getRoundsReference()[0].getKO() == true)
                if(comboBox_groupsSelectionType->currentIndex() > 0){
                    seasonCompetitionGroups = KOGroup::constructKOGroups(&seasonCompetition->getRulesPointer()->getRoundsReference()[0], &seasonCompetitionJumpers, comboBox_groupsSelectionType->currentIndex() - 1, seasonCompetition);
                    KOGroupsList->setKOGroups(&seasonCompetitionGroups);
                    KOGroupsList->fillListLayout();
                }
            emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(0, 0), jumpersListView->getListModel()->index(jumpersListView->getListModel()->rowCount() - 1));
        }
        else{
            TeamCompetitionManager::setStartListOrderByCompetitionResults(competitionTeams, competition);
            delete teamsSquadsModel;
            teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            teamsTreeView->getTreeView()->expandToDepth(0);
        }
    }
}

void CompetitionConfigWindow::on_comboBox_classification_activated(int index)
{
    if(index > 0)
    {
        index--; //Nie liczymy pierwszego item'a - "Według klasyfikacji"
        int type = seasonCompetition->getRulesPointer()->getCompetitionType();
        Classification * classification = Classification::getSpecificTypeClassifications(simulationSave->getActualSeason()->getActualCalendar()->getClassificationsReference(), type).at(index);
        if(type == CompetitionRules::Individual)
        {
            IndividualCompetitionManager::setStartListOrderByClassification(seasonCompetitionJumpers, classification);
            if(seasonCompetition->getRulesPointer()->getRoundsReference().isEmpty() == false && seasonCompetition->getRulesPointer()->getRoundsReference()[0].getKO() == true)
                if(comboBox_groupsSelectionType->currentIndex() > 0){
                    seasonCompetitionGroups = KOGroup::constructKOGroups(&seasonCompetition->getRulesPointer()->getRoundsReference()[0], &seasonCompetitionJumpers, comboBox_groupsSelectionType->currentIndex() - 1, seasonCompetition);
                    KOGroupsList->setKOGroups(&seasonCompetitionGroups);
                    KOGroupsList->fillListLayout();
                }
            emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(0, 0), jumpersListView->getListModel()->index(jumpersListView->getListModel()->rowCount() - 1));
        }
        else{
            TeamCompetitionManager::setStartListOrderByClassification(competitionTeams, classification);
            delete teamsSquadsModel;
            teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            teamsTreeView->getTreeView()->expandToDepth(0);
        }
    }
}

void CompetitionConfigWindow::on_pushButton_sortStartListRandomly_clicked()
{
    int compType = 0;
    if(type == CompetitionConfigWindow::SeasonCompetition)
        compType = seasonCompetition->getRulesPointer()->getCompetitionType();
    else
        compType = competitionRulesEditor->getCompetitionRulesFromWidgetInputs().getCompetitionType();
    if(compType == CompetitionRules::Individual){
        if(type == CompetitionConfigWindow::SeasonCompetition)
            IndividualCompetitionManager::setStartListOrderRandomly(seasonCompetitionJumpers);
        else
            IndividualCompetitionManager::setStartListOrderRandomly(competitionJumpers);
    }
    else
    {
        TeamCompetitionManager::setStartListOrderRandomly(competitionTeams);
    }
    emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(0, 0), jumpersListView->getListModel()->index(jumpersListView->getListModel()->rowCount() - 1));

    delete teamsSquadsModel;
    int count = 0;
    if(type == CompetitionConfigWindow::SeasonCompetition)
        count = seasonCompetition->getRulesPointer()->getJumpersInTeamCount();
    else count = competitionRulesEditor->getJumpersCountInTeam();
    teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, count);
    teamsSquadsModel->setupTreeItems();
    teamsTreeView->setModel(teamsSquadsModel);
    teamsTreeView->getTreeView()->expandToDepth(0);
}

void CompetitionConfigWindow::on_pushButton_sortStartListByCountries_clicked()
{
    QVector<QString> countries;
    int compType = 0;
    if(type == CompetitionConfigWindow::SeasonCompetition)
        compType = seasonCompetition->getRulesPointer()->getCompetitionType();
    else
        compType = competitionRulesEditor->getCompetitionRulesFromWidgetInputs().getCompetitionType();
    if(compType == CompetitionRules::Individual)
    {
        if(type == CompetitionConfigWindow::SeasonCompetition){
            for(auto & jp : seasonCompetitionJumpers)
                if(countries.contains(jp->getCountryCode()) == false)
                    countries.push_back(jp->getCountryCode());

            IndividualCompetitionManager::setStartListOrderByCountries(countries, seasonCompetitionJumpers);
            if(seasonCompetition->getRulesPointer()->getRoundsReference().isEmpty() == false && seasonCompetition->getRulesPointer()->getRoundsReference()[0].getKO() == true){
                seasonCompetitionGroups = KOGroup::constructKOGroups(&seasonCompetition->getRulesPointer()->getRoundsReference()[0], &seasonCompetitionJumpers, comboBox_groupsSelectionType->currentIndex() - 1, seasonCompetition);
                KOGroupsList->setKOGroups(&seasonCompetitionGroups);
                KOGroupsList->fillListLayout();
            }
        }
        else
        {
            for(auto & jp : competitionJumpers)
                if(countries.contains(jp->getCountryCode()) == false)
                    countries.push_back(jp->getCountryCode());

            IndividualCompetitionManager::setStartListOrderByCountries(countries, competitionJumpers);
            if(competitionRulesEditor->getRoundsFromInput().count() > 0){
            RoundInfo ri = competitionRulesEditor->getRoundsFromInput().at(0);
            if(ri.getKO() == true){
                competitionGroups = KOGroup::constructKOGroups(&ri, &competitionJumpers, comboBox_groupsSelectionType->currentIndex() - 1, nullptr);
                KOGroupsList->setKOGroups(&competitionGroups);
                KOGroupsList->fillListLayout();
            }
            }
        }

    emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(0, 0), jumpersListView->getListModel()->index(jumpersListView->getListModel()->rowCount() - 1));

    delete teamsSquadsModel;
    int count = 0;
    if(type == CompetitionConfigWindow::SeasonCompetition)
        count = seasonCompetition->getRulesPointer()->getJumpersInTeamCount();
    else count = competitionRulesEditor->getJumpersCountInTeam();
    teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, count);
    teamsSquadsModel->setupTreeItems();
    teamsTreeView->setModel(teamsSquadsModel);
    teamsTreeView->getTreeView()->expandToDepth(0);
    }
}

void CompetitionConfigWindow::on_pushButton_defaultStartListOrder_clicked()
{
    int type = seasonCompetition->getRulesPointer()->getCompetitionType();
    if(type == CompetitionRules::Individual)
    {
        IndividualCompetitionManager::setStartListOrderByDefault(&simulationSave->getJumpersReference(), seasonCompetitionJumpers);
        if(seasonCompetition->getRulesPointer()->getRoundsReference().isEmpty() == false && seasonCompetition->getRulesPointer()->getRoundsReference()[0].getKO() == true){
                seasonCompetitionGroups = KOGroup::constructKOGroups(&seasonCompetition->getRulesPointer()->getRoundsReference()[0], &seasonCompetitionJumpers, comboBox_groupsSelectionType->currentIndex() - 1, seasonCompetition);
                KOGroupsList->setKOGroups(&seasonCompetitionGroups);
                KOGroupsList->fillListLayout();
            }
        emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(0, 0), jumpersListView->getListModel()->index(jumpersListView->getListModel()->rowCount() - 1));
    }
}

void CompetitionConfigWindow::on_pushButton_orderByJumpersList_clicked()
{
    if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
    {
        QInputDialog dialog;
        dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
        QStringList items;
        for(auto & list : simulationSave->getJumpersListsReference())
                items.push_back(list.getName());
        dialog.setComboBoxItems(items);
        dialog.setWindowTitle(tr("Wybierz listę zawodników"));
        if(dialog.exec() == QDialog::Accepted)
        {
            IndividualCompetitionManager::setStartListOrderByJumpersList(&simulationSave->getJumpersListsReference()[items.indexOf(dialog.textValue())], seasonCompetitionJumpers);
        }
    }
}

void CompetitionConfigWindow::on_pushButton_jumpersLists_clicked()
{
    EditStartListWithJumpersListsWindow * window = new EditStartListWithJumpersListsWindow(this);
    window->setLists(&simulationSave->getJumpersListsReference());
    window->setupWidgets();
    if(window->exec() == QDialog::Accepted)
    {
        QSet<Jumper *> addedJumpers;
        int i = 0;
        for(auto & status : window->constructJumpersListsStatuses())
        {
            if(status.second == JumpersListsListItemWidget::Select && seasonCompetition->getAdvancementCompetition() == nullptr && seasonCompetition->getAdvancementClassification() == nullptr)
            {
                for(auto & jumper : status.first->getJumpersReference())
                {
                    if(seasonCompetitionJumpers.contains(jumper) == false){
                        seasonCompetitionJumpers.insert(i, jumper);
                        i++;
                        addedJumpers.insert(jumper);
                      }
                }
            }
            else if(status.second == JumpersListsListItemWidget::Unselect)
            {
                for(auto & jumper : status.first->getJumpersReference())
                {
                    if(seasonCompetitionJumpers.contains(jumper) == true && addedJumpers.contains(jumper) == false)
                        MyFunctions::removeFromVector(seasonCompetitionJumpers, jumper);
                }
            }
        }
        if((seasonCompetition->getAdvancementCompetition() != nullptr && seasonCompetition->getAdvancementCompetition()->getCancelled() == false) || seasonCompetition->getAdvancementClassification() != nullptr)
            seasonCompetitionJumpers = IndividualCompetitionManager::getFilteredJumpersAfterQualifications(seasonCompetition, seasonCompetitionJumpers);
        jumpersListView->setupListModel();
    }
}


void CompetitionConfigWindow::on_pushButton_autoGate_clicked()
{
    Jumper * bestJumper = nullptr;
    double best = -std::numeric_limits<double>::infinity();
    QVector<Jumper *> jumpers;
    if(type == CompetitionConfigWindow::SeasonCompetition)
        jumpers = seasonCompetitionJumpers;
    else
        jumpers = competitionJumpers;

    if(jumpers.isEmpty())
    {
        QMessageBox::warning(this, tr("Automatyczna belka"),
                             tr("Dodaj co najmniej jednego zawodnika przed automatycznym doborem belki."), QMessageBox::Ok);
        return;
    }

    for(auto & jumper : jumpers)
    {
        double val = jumper->getJumperSkillsPointer()->getTakeoffTechnique() * 0.8225 + jumper->getJumperSkillsPointer()->getFlightTechnique() * 0.81 + jumper->getJumperSkillsPointer()->getForm() * 0.3675;
        if(val > best){
            bestJumper = jumper;
            best = val;
        }
    }
    if(bestJumper == nullptr)
    {
        QMessageBox::warning(this, tr("Automatyczna belka"),
                             tr("Nie udało się wybrać zawodnika do próby. Sprawdź dane listy startowej."), QMessageBox::Ok);
        return;
    }
    int gate = -10;
    JumpSimulator simulator;
    simulator.setJumper(bestJumper);
    simulator.setManipulator(new JumpManipulator());
    simulator.setDSQBaseProbability(ui->spinBox_dsqProbability->value());
    if(type == CompetitionConfigWindow::SeasonCompetition){
        simulator.setCompetitionRules(seasonCompetition->getRulesPointer());
        simulator.setHill(seasonCompetition->getHill());
    }
    else{
        simulator.setCompetitionRules(new CompetitionRules(competitionRulesEditor->getCompetitionRulesFromWidgetInputs()));
        simulator.setHill(new Hill(hillEditor->getHillFromWidgetInput()));
    }
    if(type == CompetitionConfigWindow::SeasonCompetition)
        simulator.setJumpsImportance(seasonCompetition->getJumpsImportance());
    else
        simulator.setJumpsImportance(ui->doubleSpinBox_jumpsImportance->value());
    WindsGenerator windsGenerator;
    windsGenerator.setGenerationSettings(windsGeneratorSettingsEditor->getWindsGenerationSettingsFromInputs());
    while(true)
    {
        simulator.setGate(gate);
        simulator.setCompetitionStartGate(gate);
        simulator.setInrunSnow(inrunSnowGeneratorSettingsEditor->getBase() - abs(inrunSnowGeneratorSettingsEditor->getDeviation()));
        int HSPointJumps = 0;
        for(int i=0; i<10; i++)
        {
            simulator.setWinds(windsGenerator.generateWinds());
            simulator.simulateJump();
            double dist = simulator.getJumpData().getDistance();
            if(dist >= simulator.getHill()->getHSPoint())
                HSPointJumps++;
            simulator.setupForNextJump();
        }
        if(HSPointJumps <= 1)
        {
            gate ++;
        }
        else break;
    }
    ui->spinBox_startGate->setValue(gate);
 }


void CompetitionConfigWindow::on_pushButton_byOtherComp_clicked()
{
    StartListByOtherCompetitionConfigWindow * window = new StartListByOtherCompetitionConfigWindow(this);
    window->setSave(simulationSave);
    window->setCompetitionType(seasonCompetition->getRulesPointer()->getCompetitionType());
    window->setupComboBox();
    if(window->exec() == QDialog::Accepted)
    {
        if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual){
        if(window->getCheckBoxState() == false){
            seasonCompetitionJumpers = window->getCompetition()->getStartList();
                qDebug()<<"seasonCompetitionJumpers count: "<<seasonCompetitionJumpers.count();
            jumpersListView->setupListModel();
        }
        else
        {
            QVector<Jumper *> temp = seasonCompetitionJumpers;
            temp.detach();
            seasonCompetitionJumpers.clear();
            for(auto & j : window->getCompetition()->getStartListReference())
            {
                if(temp.contains(j))
                    seasonCompetitionJumpers.push_back(j);
            }
            qDebug()<<"temop1 : "<<temp.count();
            for(auto & seasonJumper : seasonCompetitionJumpers)
                MyFunctions::removeFromVector(temp, seasonJumper);
            qDebug()<<"temp: "<<temp.count();

            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(temp.begin(), temp.end(), g);
            for(auto & j : temp)
                seasonCompetitionJumpers.push_front(j);
            jumpersListView->setupListModel();
        }
        }
        else if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
        {
        if(window->getCheckBoxState() == false){
            seasonCompetition->getTeamsReference().clear();
            for(auto & team : window->getCompetition()->getTeamsReference())
            {
                Team t;
                t.reassign();
                t.setJumpers(team.getJumpersReference());
                t.setCountryCode(team.getCountryCode());
                seasonCompetition->getTeamsReference().push_back(t);
            }
            delete teamsSquadsModel;
            teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            teamsTreeView->getTreeView()->expandToDepth(0);
        }
        else
        {
            QVector<Team> temp = competitionTeams;
            temp.detach();
            competitionTeams.clear();
            for(auto & t : window->getCompetition()->getTeamsReference())
            {
                if(Team::containsTeamByCode(temp, t.getCountryCode()))
                    competitionTeams.push_back(Team::getTeamObjectByCountryCode(&temp, t.getCountryCode()));
            }
            delete teamsSquadsModel;
            teamsSquadsModel = new TeamsSquadsTreeModel(&competitionTeams, seasonCompetition->getRulesPointer()->getJumpersInTeamCount());
            teamsSquadsModel->setupTreeItems();
            teamsTreeView->setModel(teamsSquadsModel);
            teamsTreeView->getTreeView()->expandToDepth(0);
        }
        }
    }
}
