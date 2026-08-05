#ifndef CALENDAREDITORWIDGET_H
#define CALENDAREDITORWIDGET_H

#include <QWidget>
#include <QTableView>
#include "CalendarEditorTableModel.h"
#include "../CompetitionInfoEditorWidget.h"
#include "../../../seasons/SimulationSave.h"

namespace Ui {
class CalendarEditorWidget;
}

class CalendarEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarEditorWidget(CalendarEditorTableModel *model, QVector<Classification *> * classificationsList, QWidget *parent = nullptr);
    ~CalendarEditorWidget();

    void updateTable();

signals:
    void needToUpdateModel();
    void calendarPresetLoaded();
    void changed();

private:
    Ui::CalendarEditorWidget *ui;

    CalendarEditorTableModel * calendarModel;
    SeasonCalendar * calendar;
    QAction * action_add;
    QAction * action_remove;
    QAction * action_up;
    QAction * action_down;
    QAction * action_edit;
    QAction * action_duplicate;
    Hill * defaultHill;

    QVector<Classification *> * classificationsList;
    SimulationSave * save;

    CompetitionInfoEditorWidget * competitionInfoEditor;

    CompetitionInfo * actualCompetition;
    int actualCompetitionIndex;
    QString actualCompetitionID;

    void updateActualCompetitionByID();
    void debugCalendar();

private slots:
    void addActionTriggered();
    void removeActionTriggered();
    void upActionTriggered();
    void downActionTriggered();
    void editActionTriggered();
    void duplicateActionTriggered();

    void on_pushButton_saveCalendarPreset_clicked();

    void on_pushButton_loadFromPreset_clicked();

private:
    void execMultipleHillEditDialog(QVector<int> * rows, int column);
    void execMultipleTrainingsEditDialog(QVector<int> * rows, int column);
    void execMultipleCompetitionRulesEditDialog(QVector<int> * rows, int column);
    void execMultipleTrialRoundsEditDialog(QVector<int> * rows, int column);
    void execMultipleClassificationsEditDialog(QVector<int> * rows, int column);
    void multipleEditCompetitionTypes(QVector<int> * rows, int column);
    void multipleEditSerieTypes(QVector<int> * rows, int column);
    void execMultipleAdvancementCompetitionEditDialog(QVector<int> *rows, int column);
    void execMultipleAdvancementClassificationEditDialog(QVector<int> * rows, int column);
    void execJumpsImportanceEditDialog(QVector<int> * rows, int column);

public:
    CalendarEditorTableModel *getModel() const;
    void setModel(CalendarEditorTableModel *newModel);
    QTableView * getTableView();
    int getDontRemoveBefore() const;
    void setDontRemoveBefore(int newDontRemoveBefore);
    int getDontModifiyBefore() const;
    void setDontModifiyBefore(int newDontModifiyBefore);
    QVector<Classification *> *getClassificationsList() const;
    void setClassificationsList(QVector<Classification *> *newClassificationsList);
    SimulationSave *getSave() const;
    void setSave(SimulationSave *newSave);
};

#endif // CALENDAREDITORWIDGET_H
