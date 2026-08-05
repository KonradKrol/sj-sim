#ifndef DATABASEEDITORWINDOW_H
#define DATABASEEDITORWINDOW_H

#include <QDialog>
#include <QVector>
#include <QEvent>
#include <QScrollArea>

class QCloseEvent;

class HillEditorWidget;
class JumperEditorWidget;
class CompetitionRulesEditorWidget;
class Jumper;
class Hill;
class DatabaseListItemWidget;
class CompetitionRules;
class JumpersListModel;
class HillsListModel;
class CompetitionRulesListModel;
class DatabaseItemsListView;
class PointsForPlacesPreset;
class PointsForPlacesPresetsListModel;
class PointsForPlacesPresetEditorWidget;

namespace Ui {
class DatabaseEditorWindow;
}

class DatabaseEditorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseEditorWindow(QWidget *parent = nullptr);
    ~DatabaseEditorWindow();

    void replaceJumperByJumperEditor();
    void replaceHillByHillEditor();
    void replaceCompetitionRuleByCompetitionRulesEditor();
    void replacePointsForPlacesPresetByEditorObject();

    enum ElementType{
        JumperElement,
        HillElement,
        CompetitionRulesElement,
        PointsForPlacesPresetElement
    };

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void onJumpersListViewDoubleClicked(const QModelIndex &index);
    void onHillsListViewDoubleClicked(const QModelIndex &index);
    void onCompetitionRulesListViewDoubleClicked(const QModelIndex &index);
    void onPointsForPlacesPresetsListViewDoubleClicked(const QModelIndex &index);

    void on_pushButton_sortByCountries_clicked();

private:
    QVector<Jumper> tempGlobalJumpers;
    QVector<Hill> tempGlobalHills;
    QVector<CompetitionRules> tempGlobalCompetitionRules;
    QVector<PointsForPlacesPreset> tempGlobalPointsForPlacesPresets;

    JumpersListModel * jumpersListModel;
    HillsListModel * hillsListModel;
    CompetitionRulesListModel * competitionRulesListModel;
    PointsForPlacesPresetsListModel * pointsForPlacesPresetsListModel;

    DatabaseItemsListView * jumpersListView;
    DatabaseItemsListView * hillsListView;
    DatabaseItemsListView * competitionRulesListView;
    DatabaseItemsListView * pointsForPlacesPresetsListView;

    Ui::DatabaseEditorWindow *ui;

    JumperEditorWidget * jumperEditor;
    HillEditorWidget * hillEditor;
    CompetitionRulesEditorWidget * competitionRulesEditor;
    PointsForPlacesPresetEditorWidget * pointsForPlacesPresetEditor;

    QScrollArea * jumperEditorScrollArea;
    QScrollArea * hillEditorScrollArea;
    QScrollArea * rulesEditorScrollArea;
    QScrollArea * pointsForPlacesEditorScrollArea;

    short actualElementType;
    int actualElementIndex;
public:
    short getActualElementType() const;
    void setActualElementType(short newActualElementType);
    int getSelectedItemIndex() const;
    void setSelectedItemIndex(int newSelectedItemIndex);
    QVector<Jumper> getTempGlobalJumpers() const;
    void setTempGlobalJumpers(const QVector<Jumper> &newTempGlobalJumpers);
    QVector<Hill> getTempGlobalHills() const;
    void setTempGlobalHills(const QVector<Hill> &newTempGlobalHills);
    QVector<CompetitionRules> getTempGlobalCompetitionRules() const;
    void setTempGlobalCompetitionRules(const QVector<CompetitionRules> &newTempGlobalCompetitionRules);
    JumpersListModel *getJumpersListModel() const;
    void setJumpersListModel(JumpersListModel *newJumpersListModel);
    int getActualElementIndex() const;
    void setActualElementIndex(int newActualElementIndex);
    HillsListModel *getHillsListModel() const;
    void setHillsListModel(HillsListModel *newHillsListModel);
    CompetitionRulesListModel *getCompetitionRulesEditor() const;
    void setCompetitionRulesEditor(CompetitionRulesListModel *newCompetitionRulesEditor);
    CompetitionRulesListModel *getCompetitionRulesListModel() const;
    void setCompetitionRulesListModel(CompetitionRulesListModel *newCompetitionRulesListModel);
};

#endif // DATABASEEDITORWINDOW_H
