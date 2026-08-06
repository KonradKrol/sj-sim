QT       += core gui charts concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

win32: DEFINES += NOMINMAX

SOURCES += \
    source/UI/AppSettings/CountriesEditorTableModel.cpp \
    source/UI/AppSettings/CountriesEditorWindow.cpp \
    source/UI/AppSettings/JumpDataInfoChoiceDialog.cpp \
    source/UI/AppSettings/NewJumpsImportancePresetDialog.cpp \
    source/UI/Competition/CompetitionManagerWindow.cpp \
    source/UI/Competition/JumperCompetitionResultsWidget.cpp \
    source/UI/Competition/RandomWindConfigWindow.cpp \
    source/UI/Competition/Results/KOGroupResultsTableModel.cpp \
    source/UI/Competition/Results/ResultsTableModel.cpp \
    source/UI/Competition/Results/TeamResultsTreeModel.cpp \
    source/UI/Competition/StartList/StartListModel.cpp \
    source/UI/Competition/StartListByOtherCompetitionConfigWindow.cpp \
    source/UI/Competition/TeamsSquadsTreeModel.cpp \
    source/UI/Competition/TeamsSquadsTreeView.cpp \
    source/UI/DatabaseEditor/DatabaseItemsListView.cpp \
    source/UI/DatabaseEditor/ListModels/CalendarsListModel.cpp \
    source/UI/DatabaseEditor/ListModels/ClassificationsListModel.cpp \
    source/UI/DatabaseEditor/ListModels/CompetitionRulesListModel.cpp \
    source/UI/DatabaseEditor/ListModels/HillsListModel.cpp \
    source/UI/DatabaseEditor/ListModels/JumpersListModel.cpp \
    source/UI/DatabaseEditor/ListModels/JumpersListsListModel.cpp \
    source/UI/DatabaseEditor/ListModels/PointsForPlacesPresetsListModel.cpp \
    source/UI/DatabaseEditor/ListModels/SeasonHillsListModel.cpp \
    source/UI/DatabaseEditor/ListModels/SeasonJumpersListModel.cpp \
    source/UI/EditorWidgets/ClassificationEditorWidget.cpp \
    source/UI/EditorWidgets/CompetitionRulesEditorWidget.cpp \
    source/UI/EditorWidgets/EditStartListWithJumpersListsWindow.cpp \
    source/UI/EditorWidgets/InrunSnowGeneratorSettingsEditorWidget.cpp \
    source/UI/EditorWidgets/JumpersListEditorWindow.cpp \
    source/UI/EditorWidgets/JumpersListsListItemWidget.cpp \
    source/UI/EditorWidgets/KOSystem/KOGroupEditorWidget.cpp \
    source/UI/EditorWidgets/KOSystem/KOGroupInfoWidget.cpp \
    source/UI/EditorWidgets/KOSystem/KOGroupsListView.cpp \
    source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.cpp \
    source/UI/EditorWidgets/RoundInfoEditorWidget.cpp \
    source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.cpp \
    source/UI/EditorWidgets/TeamsEditing/Models/TeamJumpersListModel.cpp \
    source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.cpp \
    source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.cpp \
    source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.cpp \
    source/UI/FormGenerator/JumperFormGeneratorsSettingsTableModel.cpp \
    source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.cpp \
    source/UI/HelpWindow.cpp \
    source/UI/JumpManipulation/JumpManipulatorConfigWindow.cpp \
    source/UI/ResultsShowing/JumpDataDetailedInfoWindow.cpp \
    source/UI/Competition/CompetitionConfigWindow.cpp \
    source/UI/ResultsShowing/JumpWindInfoWidget.cpp \
    source/UI/ResultsShowing/JumpWindInfoWidgetItem.cpp \
    source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.cpp \
    source/UI/Seasons/CalendarEditor/CalendarEditorWidget.cpp \
    source/UI/Seasons/CalendarEditor/MultipleTrainingsEditDialog.cpp \
    source/UI/Seasons/CalendarEditor/MultipleTrialRoundsEditDialog.cpp \
    source/UI/Seasons/Classifications/ClassificationResultsTableModel.cpp \
    source/UI/Seasons/Classifications/ClassificationResultsTableView.cpp \
    source/UI/Seasons/CompetitionInfoEditorWidget.cpp \
    source/UI/Seasons/ItemModels/ClassificationsArchiveListModel.cpp \
    source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.cpp \
    source/UI/Seasons/ItemModels/SimulationSavesListModel.cpp \
    source/UI/Seasons/NewSeasonConfiguratorWindow.cpp \
    source/UI/Seasons/NewSimulationSaveConfigurationWindow.cpp \
    source/UI/Seasons/SimulationSaveInfoWidget.cpp \
    source/UI/Seasons/SimulationSaveManagerWindow.cpp \
    source/UI/Seasons/SimulationSavesWindow.cpp \
    source/UI/Seasons/Stats/ApperanceInClassificationWindow.cpp \
    source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.cpp \
    source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.cpp \
    source/UI/Seasons/Stats/FilteringWidgets/HillTypesCheckBoxesWidget.cpp \
    source/UI/Seasons/Stats/FilteringWidgets/SerieTypesComboBoxesWidget.cpp \
    source/UI/Seasons/Stats/JumperStatsWindow.cpp \
    source/UI/Seasons/Stats/Models/GeneralClassificationTableModel.cpp \
    source/UI/Seasons/Stats/Models/SimulationRatingTableModel.cpp \
    source/UI/Seasons/Stats/Models/SimulationRecordsTableModel.cpp \
    source/UI/Seasons/Stats/SimulationRatingsWindow.cpp \
    source/UI/SingleJumps/SingleJumpsConfigWindow.cpp \
    source/UI/AppSettings/AppSettingsWindow.cpp \
    source/UI/DatabaseEditor/DatabaseEditorWindow.cpp \
    source/UI/EditorWidgets/CharacteristicsEditor.cpp \
    source/UI/EditorWidgets/Dialogs/CharacteristicInputDialog.cpp \
    source/UI/EditorWidgets/HillEditorWidget.cpp \
    source/UI/EditorWidgets/JumperEditorWidget.cpp \
    source/UI/SingleJumps/SingleJumpsResultsTableModel.cpp \
    source/UI/SingleJumps/SingleJumpsResultsTableSortDialog.cpp \
    source/UI/SingleJumps/SingleJumpsResultsWindow.cpp \
    source/UI/ThanksWindow.cpp \
    source/competitions/AbstractCompetitionManager.cpp \
    source/competitions/CompetitionInfo.cpp \
    source/competitions/CompetitionManagers/KORoundManager.cpp \
    source/competitions/CompetitionManagers/TeamCompetitionManager.cpp \
    source/competitions/CompetitionResults.cpp \
    source/competitions/CompetitionRules.cpp \
    source/competitions/CompetitionSingleResult.cpp \
    source/competitions/CompetitionManagers/IndividualCompetitionManager.cpp \
    source/competitions/KOSystem/KOGroup.cpp \
    source/competitions/RoundInfo.cpp \
    source/competitions/StartListCompetitorStatus.cpp \
    source/form-generator/JumperFormGenerator.cpp \
    source/form-generator/JumperFormGeneratorSettings.cpp \
    source/form-generator/JumperFormGeneratorSettingsPreset.cpp \
    source/form-generator/JumperFormTendence.cpp \
    source/global/Country.cpp \
    source/global/CountryFlagsManager.cpp \
    source/global/GlobalAppSettings.cpp \
    source/global/GlobalDatabase.cpp \
    source/global/GlobalSimulationSettings.cpp \
    source/global/GlobalTranslators.cpp \
    source/global/IdentifiableObjectsStorage.cpp \
    source/global/JumpDataInfoChoice.cpp \
    source/global/JumpsImportancePreset.cpp \
    source/global/PointsForPlacesPreset.cpp \
    source/global/TreeItem.cpp \
    source/global/Uuid.cpp \
    source/main.cpp \
    source/UI/mainwindow.cpp \
    source/seasons/ClassificationSingleResult.cpp \
    source/seasons/SaveJumpersList.cpp \
    source/seasons/Season.cpp \
    source/seasons/SeasonCalendar.cpp \
    source/seasons/SeasonCalendarPreset.cpp \
    source/seasons/SimulationSave.cpp \
    source/seasons/Classification.cpp \
    source/simulator/Characteristic.cpp \
    source/simulator/Hill.cpp \
    source/simulator/InrunSnowGenerator.cpp \
    source/simulator/JumpData.cpp \
    source/simulator/JumpManipulator.cpp \
    source/simulator/JumpSimulationData.cpp \
    source/simulator/JumpSimulator.cpp \
    source/simulator/Jumper.cpp \
    source/simulator/JumperSkills.cpp \
    source/simulator/Landing.cpp \
    source/simulator/Team.cpp \
    source/simulator/Wind.cpp \
    source/simulator/WindsCalculator.cpp \
    source/simulator/wind-generation/WindGenerationSettings.cpp \
    source/simulator/wind-generation/WindsGenerator.cpp \
    source/single-jumps/SingleJumpsManager.cpp \
    source/utilities/ClassWithCharacteristics.cpp \
    source/global/MyRandom.cpp \
    source/utilities/Identifiable.cpp \
    source/utilities/functions.cpp

HEADERS += \
    source/UI/ResponsiveWindowUtils.h \
    source/UI/AppSettings/CountriesEditorTableModel.h \
    source/UI/AppSettings/CountriesEditorWindow.h \
    source/UI/AppSettings/JumpDataInfoChoiceDialog.h \
    source/UI/AppSettings/NewJumpsImportancePresetDialog.h \
    source/UI/Competition/CompetitionManagerWindow.h \
    source/UI/Competition/JumperCompetitionResultsWidget.h \
    source/UI/Competition/RandomWindConfigWindow.h \
    source/UI/Competition/Results/KOGroupResultsTableModel.h \
    source/UI/Competition/Results/ResultsTableModel.h \
    source/UI/Competition/Results/TeamResultsTreeModel.h \
    source/UI/Competition/StartList/StartListModel.h \
    source/UI/Competition/StartListByOtherCompetitionConfigWindow.h \
    source/UI/Competition/TeamsSquadsTreeModel.h \
    source/UI/Competition/TeamsSquadsTreeView.h \
    source/UI/DatabaseEditor/DatabaseItemsListView.h \
    source/UI/DatabaseEditor/ListModels/CalendarsListModel.h \
    source/UI/DatabaseEditor/ListModels/ClassificationsListModel.h \
    source/UI/DatabaseEditor/ListModels/CompetitionRulesListModel.h \
    source/UI/DatabaseEditor/ListModels/HillsListModel.h \
    source/UI/DatabaseEditor/ListModels/JumpersListModel.h \
    source/UI/DatabaseEditor/ListModels/JumpersListsListModel.h \
    source/UI/DatabaseEditor/ListModels/PointsForPlacesPresetsListModel.h \
    source/UI/DatabaseEditor/ListModels/SeasonHillsListModel.h \
    source/UI/DatabaseEditor/ListModels/SeasonJumpersListModel.h \
    source/UI/EditorWidgets/ClassificationEditorWidget.h \
    source/UI/EditorWidgets/CompetitionRulesEditorWidget.h \
    source/UI/EditorWidgets/EditStartListWithJumpersListsWindow.h \
    source/UI/EditorWidgets/InrunSnowGeneratorSettingsEditorWidget.h \
    source/UI/EditorWidgets/JumpersListEditorWindow.h \
    source/UI/EditorWidgets/JumpersListsListItemWidget.h \
    source/UI/EditorWidgets/KOSystem/KOGroupEditorWidget.h \
    source/UI/EditorWidgets/KOSystem/KOGroupInfoWidget.h \
    source/UI/EditorWidgets/KOSystem/KOGroupsListView.h \
    source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.h \
    source/UI/EditorWidgets/RoundInfoEditorWidget.h \
    source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.h \
    source/UI/EditorWidgets/TeamsEditing/Models/TeamJumpersListModel.h \
    source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.h \
    source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.h \
    source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h \
    source/UI/FormGenerator/JumperFormGeneratorsSettingsTableModel.h \
    source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.h \
    source/UI/HelpWindow.h \
    source/UI/JumpManipulation/JumpManipulatorConfigWindow.h \
    source/UI/ResultsShowing/JumpDataDetailedInfoWindow.h \
    source/UI/Competition/CompetitionConfigWindow.h \
    source/UI/ResultsShowing/JumpWindInfoWidget.h \
    source/UI/ResultsShowing/JumpWindInfoWidgetItem.h \
    source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.h \
    source/UI/Seasons/CalendarEditor/CalendarEditorWidget.h \
    source/UI/Seasons/CalendarEditor/MultipleTrainingsEditDialog.h \
    source/UI/Seasons/CalendarEditor/MultipleTrialRoundsEditDialog.h \
    source/UI/Seasons/Classifications/ClassificationResultsTableModel.h \
    source/UI/Seasons/Classifications/ClassificationResultsTableView.h \
    source/UI/Seasons/CompetitionInfoEditorWidget.h \
    source/UI/Seasons/ItemModels/ClassificationsArchiveListModel.h \
    source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.h \
    source/UI/Seasons/ItemModels/SimulationSavesListModel.h \
    source/UI/Seasons/NewSeasonConfiguratorWindow.h \
    source/UI/Seasons/NewSimulationSaveConfigurationWindow.h \
    source/UI/Seasons/SimulationSaveInfoWidget.h \
    source/UI/Seasons/SimulationSaveManagerWindow.h \
    source/UI/Seasons/SimulationSavesWindow.h \
    source/UI/Seasons/Stats/ApperanceInClassificationWindow.h \
    source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.h \
    source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.h \
    source/UI/Seasons/Stats/FilteringWidgets/HillTypesCheckBoxesWidget.h \
    source/UI/Seasons/Stats/FilteringWidgets/SerieTypesComboBoxesWidget.h \
    source/UI/Seasons/Stats/JumperStatsWindow.h \
    source/UI/Seasons/Stats/Models/GeneralClassificationTableModel.h \
    source/UI/Seasons/Stats/Models/SimulationRatingTableModel.h \
    source/UI/Seasons/Stats/Models/SimulationRecordsTableModel.h \
    source/UI/Seasons/Stats/SimulationRatingsWindow.h \
    source/UI/SingleJumps/SingleJumpsConfigWindow.h \
    source/UI/AppSettings/AppSettingsWindow.h \
    source/UI/DatabaseEditor/DatabaseEditorWindow.h \
    source/UI/EditorWidgets/CharacteristicsEditor.h \
    source/UI/EditorWidgets/Dialogs/CharacteristicInputDialog.h \
    source/UI/EditorWidgets/HillEditorWidget.h \
    source/UI/EditorWidgets/JumperEditorWidget.h \
    source/UI/SingleJumps/SingleJumpsResultsTableModel.h \
    source/UI/SingleJumps/SingleJumpsResultsTableSortDialog.h \
    source/UI/SingleJumps/SingleJumpsResultsWindow.h \
    source/UI/ThanksWindow.h \
    source/UI/mainwindow.h \
    source/competitions/AbstractCompetitionManager.h \
    source/competitions/CompetitionInfo.h \
    source/competitions/CompetitionManagers/KORoundManager.h \
    source/competitions/CompetitionManagers/TeamCompetitionManager.h \
    source/competitions/CompetitionResults.h \
    source/competitions/CompetitionRules.h \
    source/competitions/CompetitionSingleResult.h \
    source/competitions/CompetitionManagers/IndividualCompetitionManager.h \
    source/competitions/KOSystem/KOGroup.h \
    source/competitions/RoundInfo.h \
    source/competitions/StartListCompetitorStatus.h \
    source/form-generator/JumperFormGenerator.h \
    source/form-generator/JumperFormGeneratorSettings.h \
    source/form-generator/JumperFormGeneratorSettingsPreset.h \
    source/form-generator/JumperFormTendence.h \
    source/global/Country.h \
    source/global/CountryFlagsManager.h \
    source/global/GlobalAppSettings.h \
    source/global/GlobalDatabase.h \
    source/global/GlobalSimulationSettings.h \
    source/global/GlobalTranslators.h \
    source/global/IdentifiableObjectsStorage.h \
    source/global/JumpDataInfoChoice.h \
    source/global/JumpsImportancePreset.h \
    source/global/PointsForPlacesPreset.h \
    source/global/TreeItem.h \
    source/global/Uuid.h \
    source/seasons/ClassificationSingleResult.h \
    source/seasons/SaveJumpersList.h \
    source/seasons/Season.h \
    source/seasons/SeasonCalendar.h \
    source/seasons/SeasonCalendarPreset.h \
    source/seasons/SimulationSave.h \
    source/seasons/Classification.h \
    source/simulator/Characteristic.h \
    source/simulator/Hill.h \
    source/simulator/InrunSnowGenerator.h \
    source/simulator/JumpData.h \
    source/simulator/JumpManipulator.h \
    source/simulator/JumpSimulationData.h \
    source/simulator/JumpSimulator.h \
    source/simulator/Jumper.h \
    source/simulator/JumperSkills.h \
    source/simulator/Landing.h \
    source/simulator/Team.h \
    source/simulator/Wind.h \
    source/simulator/WindsCalculator.h \
    source/simulator/wind-generation/WindGenerationSettings.h \
    source/simulator/wind-generation/WindsGenerator.h \
    source/single-jumps/SingleJumpsManager.h \
    source/utilities/ClassWithCharacteristics.h \
    source/global/MyRandom.h \
    source/utilities/Identifiable.h \
    source/utilities/functions.h

FORMS += \
    source/UI/AppSettings/CountriesEditorWindow.ui \
    source/UI/AppSettings/JumpDataInfoChoiceDialog.ui \
    source/UI/AppSettings/NewJumpsImportancePresetDialog.ui \
    source/UI/Competition/CompetitionManagerWindow.ui \
    source/UI/Competition/JumperCompetitionResultsWidget.ui \
    source/UI/Competition/RandomWindConfigWindow.ui \
    source/UI/Competition/StartListByOtherCompetitionConfigWindow.ui \
    source/UI/Competition/TeamsSquadsTreeView.ui \
    source/UI/DatabaseEditor/DatabaseItemsListView.ui \
    source/UI/EditorWidgets/ClassificationEditorWidget.ui \
    source/UI/EditorWidgets/CompetitionRulesEditorWidget.ui \
    source/UI/EditorWidgets/EditStartListWithJumpersListsWindow.ui \
    source/UI/EditorWidgets/InrunSnowGeneratorSettingsEditorWidget.ui \
    source/UI/EditorWidgets/JumpersListEditorWindow.ui \
    source/UI/EditorWidgets/JumpersListsListItemWidget.ui \
    source/UI/EditorWidgets/KOSystem/KOGroupEditorWidget.ui \
    source/UI/EditorWidgets/KOSystem/KOGroupInfoWidgetui \
    source/UI/EditorWidgets/KOSystem/KOGroupsListView.ui \
    source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.ui \
    source/UI/EditorWidgets/RoundInfoEditorWidget.ui \
    source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.ui \
    source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.ui \
    source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.ui \
    source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.ui \
    source/UI/HelpWindow.ui \
    source/UI/JumpManipulation/JumpManipulatorConfigWindow.ui \
    source/UI/ResultsShowing/JumpDataDetailedInfoWindow.ui \
    source/UI/Competition/CompetitionConfigWindow.ui \
    source/UI/ResultsShowing/JumpWindInfoWidget.ui \
    source/UI/ResultsShowing/JumpWindInfoWidgetItem.ui \
    source/UI/Seasons/CalendarEditor/CalendarEditorWidget.ui \
    source/UI/Seasons/CalendarEditor/MultipleTrainingsEditDialog.ui \
    source/UI/Seasons/CalendarEditor/MultipleTrialRoundsEditDialog.ui \
    source/UI/Seasons/Classifications/ClassificationResultsTableView.ui \
    source/UI/Seasons/CompetitionInfoEditorWidget.ui \
    source/UI/Seasons/NewSeasonConfiguratorWindow.ui \
    source/UI/Seasons/NewSimulationSaveConfigurationWindow.ui \
    source/UI/Seasons/SimulationSaveInfoWidget.ui \
    source/UI/Seasons/SimulationSaveManagerWindow.ui \
    source/UI/Seasons/SimulationSavesWindow.ui \
    source/UI/Seasons/Stats/ApperanceInClassificationWindow.ui \
    source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.ui \
    source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.ui \
    source/UI/Seasons/Stats/FilteringWidgets/HillTypesCheckBoxesWidget.ui \
    source/UI/Seasons/Stats/FilteringWidgets/SerieTypesComboBoxesWidget.ui \
    source/UI/Seasons/Stats/JumperStatsWindow.ui \
    source/UI/Seasons/Stats/SimulationRatingsWindow.ui \
    source/UI/SingleJumps/SingleJumpsConfigWindow.ui \
    source/UI/AppSettings/AppSettingsWindow.ui \
    source/UI/DatabaseEditor/DatabaseEditorWindow.ui \
    source/UI/EditorWidgets/CharacteristicsEditor.ui \
    source/UI/EditorWidgets/Dialogs/CharacteristicInputDialog.ui \
    source/UI/EditorWidgets/HillEditorWidget.ui \
    source/UI/EditorWidgets/JumperEditorWidget.ui \
    source/UI/SingleJumps/SingleJumpsResultsTableSortDialog.ui \
    source/UI/SingleJumps/SingleJumpsResultsWindow.ui \
    source/UI/ThanksWindow.ui \
    source/UI/mainwindow.ui

TRANSLATIONS += \
    translations/translation_en.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    flaticon_authors \
    flags_author \
    userData/GlobalDatabase/globalCompetitionsRules.json \
    userData/GlobalDatabase/globalHills.json \
    userData/GlobalDatabase/globalJumpers.json \
    userData/Settings/appSettings.json \
    README.md

RESOURCES += \
assets.qrc

INCLUDEPATH += $$PWD/source/3rdparty/linux-dpp-stub

# Qt 5 defined QT_CHARTS_USE_NAMESPACE and friends in qchartglobal.h; Qt 6 removed them.
greaterThan(QT_MAJOR_VERSION, 5): DEFINES += QT_CHARTS_BEGIN_NAMESPACE= QT_CHARTS_END_NAMESPACE= QT_CHARTS_USE_NAMESPACE=
