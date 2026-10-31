#ifndef COMPETITIONCONFIGWINDOW_H
#define COMPETITIONCONFIGWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QComboBox>
#include <QAction>
#include <QCheckBox>
#include <QTreeView>
#include "../../simulator/Jumper.h"
#include "../../simulator/Team.h"
#include "../../seasons/SimulationSave.h"
#include "../../competitions/CompetitionInfo.h"
#include "../DatabaseEditor/DatabaseItemsListView.h"
#include "TeamsSquadsTreeModel.h"
#include "TeamsSquadsTreeView.h"
#include "../EditorWidgets/InrunSnowGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/KOSystem/KOGroupsListView.h"
#include "../../simulation-v2/SimulationCatalog.h"
#include "../../simulation-v2/ProfileDraft.h"
#include "../../simulation-v2/WeatherSetup.h"
#include <memory>
#include <vector>

class HillEditorWidget;
class WindsGeneratorSettingsEditorWidget;
class CompetitionRulesEditorWidget;
class CompetitionStartListDisplayWidget;

namespace Ui {
class SingleCompetitionConfigWindow;
}

class CompetitionConfigWindow : public QDialog
{
    Q_OBJECT
    friend class CompetitionQuickBridge;
    friend class QuickRoster;

public:
    enum class DataSource { Canonical, LegacyCompatibility };
    explicit CompetitionConfigWindow(short type, QWidget *parent = nullptr, SimulationSave * save = nullptr,
        DataSource source = DataSource::Canonical,
        const SimulationV2::CatalogPaths &paths = SimulationV2::CatalogPaths::defaults());
    ~CompetitionConfigWindow();
    bool usesCanonicalCatalog() const {return type==SingleCompetition && source==DataSource::Canonical;}
    bool catalogReady() const {return !usesCanonicalCatalog() || ready;}
    SimulationWeatherSetup *getWeatherSetup() const {return weatherSetup;}
    SimulationProfileDraft *getHillDraft() const {return hillDraft;}
    Hill configuredHill() const;
    QVector<Jumper *> availableJumpers() const;

    enum Type{
        SingleCompetition,
        SeasonCompetition
    };

    int getJumpersCountInTeam() const;
    int getStartingGateFromInput() const;
    int getBaseDSQProbability() const;
    WindsGeneratorSettingsEditorWidget * getWindGeneratorSettingsWidget();
    InrunSnowGeneratorSettingsEditorWidget * getInrunSnowGeneratorSettingsWidget();

    void setClassificationsComboBoxIndex(int index);

signals:
    void submitted();

private:
    QStringList autoSquadSnapshot;
    QStringList squadSnapshot() const;
    bool automaticGateEnabled() const { return !simulationSave || simulationSave->getAutoGateEnabled(); }
    Ui::SingleCompetitionConfigWindow *ui;
    int type;
    HillEditorWidget * hillEditor;
    WindsGeneratorSettingsEditorWidget * windsGeneratorSettingsEditor;
    InrunSnowGeneratorSettingsEditorWidget * inrunSnowGeneratorSettingsEditor;

    void setupHillToolBoxItem();
    //
    CompetitionRulesEditorWidget * competitionRulesEditor;
    QWidget * competitionRulesParentWidget;
    QVBoxLayout * competitionRulesToolBoxItemLayout;
    QVBoxLayout * existingRulesLabelAndComboBoxLayout;
    QLabel * existingCompetitionRulesLabel;
    QComboBox * existingCompetitionRulesComboBox;
    void setupCompetitionRulesToolBoxItem();
    bool validateSimulationInputs();
    //

    DatabaseItemsListView * jumpersListView;
    TeamsSquadsTreeView * teamsTreeView;
    TeamsSquadsTreeModel * teamsSquadsModel;
    KOGroupsListView * KOGroupsList;

    QCheckBox * checkBox_singleCompetitionQualifications = nullptr;

    QVector<Team> competitionTeams;
    QVector<Jumper *> competitionJumpers;
    QVector<KOGroup> competitionGroups;

    CompetitionInfo * seasonCompetition = nullptr;
    SimulationSave * simulationSave;
    QVector<Jumper *> seasonCompetitionJumpers;
    QVector<KOGroup> seasonCompetitionGroups;

    QComboBox * comboBox_groupsSelectionType;

    QPushButton * push_button_randomWind;

    void updateResponsiveLayout(int windowWidth);
    bool compactLayout = false;
    DataSource source;
    SimulationV2::CatalogPaths catalogPaths;
    SimulationV2::SimulationCatalog catalog;
    SimulationV2::CatalogSnapshot snapshot;
    SimulationProfileDraft *hillDraft=nullptr;
    SimulationWeatherSetup *weatherSetup=nullptr;
    QComboBox *athletePack=nullptr,*hillPack=nullptr,*rulesPack=nullptr;
    QLabel *catalogStatus=nullptr;
    Hill widgetHill;
    bool ready=false,filling=false;
    QVector<Jumper *> sourceJumpers,importedJumpers;
    QString importNotice;
    std::vector<std::unique_ptr<Jumper>> importedStorage;
    void setupCanonicalCatalog();
    void setupWeather();
    void loadCanonicalCatalog();
    void selectCanonicalPack(DataPack::Kind);
    void selectCanonicalHill(int);
    void selectCanonicalRules(int);
    void updateCanonicalReadiness();
    void updateCanonicalGateSuggestion();
    void refreshCanonicalRoster();
    void persistCanonicalRecords(const Hill &);

public:
    short getType() const;
    void setType(short newType);
    WindsGeneratorSettingsEditorWidget *getWindGeneratorSettingsEditor();
    void setWindGeneratorSettingsEditor(WindsGeneratorSettingsEditorWidget *newWindsGeneratorSettingsEditor);
    CompetitionRulesEditorWidget *getCompetitionRulesEditor() const;
    void setCompetitionRulesEditor(CompetitionRulesEditorWidget *newCompetitionRulesEditor);
    HillEditorWidget *getHillEditor() const;
    void setHillEditor(HillEditorWidget *newHillEditor);
    CompetitionStartListDisplayWidget *getStartListDisplayWidget() const;
    void setStartListDisplayWidget(CompetitionStartListDisplayWidget *newStartListDisplayWidget);
    QVector<Jumper *> getCompetitionJumpers() const;
    QVector<Jumper *> & getCompetitionJumpersReference();
    QVector<Team> getCompetitionTeams() const;
    Q_INVOKABLE void autoCompleteTeams(bool ask=true);
    QVector<Team> & getCompetitionTeamsReference();
    void setCompetitionTeams(const QVector<Team> &newCompetitionTeams);
    void setCompetitionJumpers(const QVector<Jumper *> &newCompetitionJumpers);
    QAction *getMoveToTopShortcut() const;
    void setMoveToTopShortcut(QAction *newMoveToTopShortcut);
    QAction *getMoveToDownShortcut() const;
    void setMoveToDownShortcut(QAction *newMoveToDownShortcut);
    TeamsSquadsTreeModel *getTeamsSquadsModel() const;
    void setTeamsSquadsModel(TeamsSquadsTreeModel *newTeamsSquadsModel);
    QTreeView *getTeamsTreeView() const;
    void setTeamsTreeView(QTreeView *newTeamsTreeView);
    CompetitionInfo *getSeasonCompetition() const;
    void setSeasonCompetition(CompetitionInfo *newSeasonCompetition);
    int suggestedGate();
    SimulationSave *getSimulationSave() const;
    void setSimulationSave(SimulationSave *newSimulationSave);
    QVector<Jumper *> getSeasonCompetitionJumpers() const;
    void setSeasonCompetitionJumpers(const QVector<Jumper *> &newSeasonCompetitionJumpers);
    QVector<KOGroup> & getCompetitionGroupsReference();
    QVector<KOGroup> getCompetitionGroups() const;
    void setCompetitionGroups(const QVector<KOGroup> &newCompetitionGroups);
    QVector<KOGroup> getSeasonCompetitionGroups() const;
    QVector<KOGroup> & getSeasonCompetitionGroupsReference();
    void setSeasonCompetitionGroups(const QVector<KOGroup> &newSeasonCompetitionGroups);

private slots:
    void on_pushButton_submit_clicked();
    void on_pushButton_loadJumpers_clicked();
    void on_comboBox_classification_activated(int index);
    void on_pushButton_defaultStartListOrder_clicked();
    void on_comboBox_competition_activated(int index);
    void on_pushButton_jumpersLists_clicked();
    void on_pushButton_autoGate_clicked();
    void on_pushButton_sortStartListRandomly_clicked();
    void on_pushButton_sortStartListByCountries_clicked();
    void on_pushButton_orderByJumpersList_clicked();
    void on_pushButton_byOtherComp_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // COMPETITIONCONFIGWINDOW_H
