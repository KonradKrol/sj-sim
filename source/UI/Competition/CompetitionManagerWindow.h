#ifndef COMPETITIONMANAGERWINDOW_H
#define COMPETITIONMANAGERWINDOW_H

#include <QDialog>
#include <QToolBar>
#include <QMenu>
#include <QTreeView>
#include "JumperCompetitionResultsWidget.h"
#include "StartList/StartListModel.h"
#include "Results/ResultsTableModel.h"
#include "Results/TeamResultsTreeModel.h"
#include "Results/KOGroupResultsTableModel.h"
#include "../../competitions/AbstractCompetitionManager.h"
#include "../../simulator/JumpManipulator.h"
#include "../../simulator/InrunSnowGenerator.h"
#include "../Seasons/Classifications/ClassificationResultsTableView.h"
#include <dpp/dpp.h>

namespace Ui {
class CompetitionManagerWindow;
}

class CompetitionManagerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CompetitionManagerWindow(AbstractCompetitionManager * manager = nullptr,  QWidget *parent = nullptr, bool singleCompetition = false);
    ~CompetitionManagerWindow();

    void updateToBeatDistanceLabel();
    void updateToAdvanceDistanceLabel();
    void updatePointsToTheLeaderLabel();
    void updateAvgWindLabel();
    void updateActualInrunSnow();
    void updateActualInrunSnowLevelLabel();

    void disableCompetitionManagementButtons();
    void enableCompetitionManagementButtons();

    void showMessageBoxForNextGroup();
    void setupGoToNextButtonForNextGroup();

    void showMessageBoxForNextRound();
    void setupGoToNextButtonForNextRound();

    void showMessageBoxForQualificationsEnd();
    void setupGoToNextButtonForQualificationsEnd();

    void showMessageBoxForCompetitionEnd();
    void setupGoToNextButtonForCompetitionEnd();

    void cancelCompetition();
    void cancelActualRound();
    void autoSimulateCompetition();
    void autoSimulateRound();
    void autoSimulateGroup(); // tylko druzynowe
    void autoSimulateJumps();

    void checkRecords(JumpData &jump, bool multipleRecords = false);
    void checkRecordsByResults(CompetitionResults * results);

    dpp::embed getEmbedForResults();
    dpp::message getMessageForResults();
    QString getTextForFullResultsEmbed();

    void executeLiveCompetitionEffects();

    void checkForEasterEggs();

signals:
    void nextRoundButtonClicked();

private:
    Ui::CompetitionManagerWindow *ui;
    AbstractCompetitionManager * manager;
    short type;
    bool singleCompetition;
    StartListModel * startListModel;
    ResultsTableModel * resultsTableModel;

    KORoundManager * KOManager;
    KOGroupResultsTableModel * KOGroupsResultsModel;

    TeamResultsTreeModel * teamResultsTreeModel;
    QTreeView * teamResultsTreeView;

    JumpManipulator currentInputJumpManipulator;

    QVector<Wind> actualWinds;
    WindsGenerator windsGenerator;

    InrunSnowGenerator inrunSnowGenerator;
    double actualInrunSnow;

    JumpSimulator simulator;
    void setupSimulator();

    JumperCompetitionResultsWidget * jumperResultsWidget;

    QToolBar * toolBar;

    QList<QAction *> cancelActionsList;
    QMenu * cancelActionsMenu;
    QAction * action_cancelCompetition;
    QAction * action_cancelRound;

    QList<QAction *> autoSimulationActionsList;
    QMenu * autoSimulationActionsMenu;
    QAction * action_autoSimulateCompetition;
    QAction * action_autoSimulateRound;
    QAction * action_autoSimulateGroup;
    QAction * action_autoSimulateJumps;

    QVector<KOGroup> getManualKOGroupsFromDialogInputs();
    void setupVirtualClassificationComboBox();
    QVector<Classification *> virtualClassifications;
    ClassificationResultsTableView * virtualClassificationTableView;

public:
    AbstractCompetitionManager *getManager() const;
    short getType() const;
    void setType(short newType);
    JumpManipulator getCurrentInputJumpManipulator() const;
    void setCurrentInputJumpManipulator(const JumpManipulator &newCurrentInputJumpManipulator);
    int getCurrentCoachGate() const;
    void setCurrentCoachGate(int newCurrentCoachGate);
    QVector<Wind> & getActualWindsReference();
    QVector<Wind> getActualWinds() const;
    void setActualWinds(const QVector<Wind> &newActualWinds);
    InrunSnowGenerator getInrunSnowGenerator() const;
    void setInrunSnowGenerator(const InrunSnowGenerator &newInrunSnowGenerator);

    KOGroupResultsTableModel *getKOGroupsResultsModel() const;
    void setKOGroupsResultsModel(KOGroupResultsTableModel *newKOGroupsResultsModel);

    KORoundManager *getKOManager() const;
    void setKOManager(KORoundManager *newKOManager);

    bool getSingleCompetition() const;
    void setSingleCompetition(bool newSingleCompetition);

private slots:
    void on_pushButton_jump_clicked();
    void on_tableView_results_doubleClicked(const QModelIndex &index);
    void on_pushButton_generateNewWinds_clicked();
    void on_pushButton_windsGeneratorSettings_clicked();
    void on_pushButton_manipulateJump_clicked();
    void on_pushButton_coachGate_clicked();
    void on_pushButton_inrunSnowGeneratorSettings_clicked();
    void on_comboBox_virtualClassification_currentIndexChanged(int index);
    void on_pushButton_sendResultsWebhook_clicked();
    void on_checkBox_liveCompetition_stateChanged(int arg1);
protected:
    void closeEvent(QCloseEvent * event);
};

#endif // COMPETITIONMANAGERWINDOW_H
