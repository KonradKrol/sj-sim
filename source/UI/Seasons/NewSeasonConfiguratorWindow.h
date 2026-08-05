#ifndef NEWSEASONCONFIGURATORWINDOW_H
#define NEWSEASONCONFIGURATORWINDOW_H

#include <QDialog>
#include <QAction>
#include <QToolBox>
#include "../DatabaseEditor/ListModels/JumpersListModel.h"
#include "CalendarEditor/CalendarEditorWidget.h"
#include "../EditorWidgets/ClassificationEditorWidget.h"
#include "../EditorWidgets/JumperEditorWidget.h"
#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"

class DatabaseItemsListView;

namespace Ui {
class NewSeasonConfiguratorDialog;
}

class NewSeasonConfiguratorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewSeasonConfiguratorWindow(bool nextSeason = false, QWidget *parent = nullptr);
    ~NewSeasonConfiguratorWindow();

    void showCalendarEditorHelp();

private slots:
    void on_pushButton_submit_clicked();

    void on_pushButton_loadCalendarPreset_clicked();

private:
    Ui::NewSeasonConfiguratorDialog *ui;

    QVector<Jumper *> jumpers;
    QVector<Hill *> hills;
    QVector<CompetitionRules> competitionsRules;
    QVector<SaveJumpersList> jumpersLists;

    DatabaseItemsListView * jumpersListView;
    DatabaseItemsListView * hillsListView;
    DatabaseItemsListView * rulesListView;

    JumperEditorWidget * jumperEditor;
    HillEditorWidget * hillEditor;
    CompetitionRulesEditorWidget * rulesEditor;

    int actualJumperIndex;
    int actualHillIndex;
    int actualRulesIndex;

    DatabaseItemsListView * classificationsListView;
    int classificationsListViewActualElement;
    ClassificationEditorWidget * classificationEditor;

    SeasonCalendar calendar;
    CalendarEditorWidget * calendarEditor;
    CalendarEditorTableModel * calendarTableModel;

    bool nextSeason;

public:
    QVector<Jumper *> getJumpers() const;
    QVector<Jumper *>& getJumpersReference();
    void setJumpers(const QVector<Jumper *> &newJumpers);
    CalendarEditorWidget *getCalendarEditor() const;
    void setCalendarEditor(CalendarEditorWidget *newCalendarEditor);
    SeasonCalendar getCalendar() const;
    void setCalendar(const SeasonCalendar &newCalendar);
    CalendarEditorTableModel *getCalendarTableModel() const;
    void setCalendarTableModel(CalendarEditorTableModel *newCalendarTableModel);
    ClassificationEditorWidget *getClassificationEditor() const;
    void setClassificationEditor(ClassificationEditorWidget *newClassificationEditor);
    QVector<Hill *> getHills() const;
    QVector<Hill *> getHillsReference();
    void setHills(const QVector<Hill *> &newHills);
    QVector<CompetitionRules> & getCompetitionsRulesReference();
    void setCompetitionsRules(const QVector<CompetitionRules> &newCompetitionsRules);
    QToolBox * getToolBox();
    bool getNextSeason() const;
    void setNextSeason(bool newNextSeason);
};

#endif // NEWSEASONCONFIGURATORWINDOW_H
