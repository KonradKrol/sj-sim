#ifndef COMPETITIONINFOEDITORWIDGET_H
#define COMPETITIONINFOEDITORWIDGET_H

#include <QWidget>
#include <QMetaObject>
#include <QComboBox>
#include <QStringListModel>
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../../competitions/CompetitionInfo.h"
#include "../../global/CountryFlagsManager.h"

namespace Ui {
class CompetitionInfoEditorWidget;
}

class CompetitionInfoEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CompetitionInfoEditorWidget(CompetitionInfo * competitionInfo = nullptr, QVector<Hill *> * hillsList = nullptr, QVector<CompetitionRules> * rulesList = nullptr, QWidget *parent = nullptr);
    ~CompetitionInfoEditorWidget();

    void fillInputs();
    void resetInputs();

    CompetitionInfo getCompetitionInfoFromInputs();

    void setupHillsComboBox();
    void setupRulesComboBox();
    void setupClassificationsComboBox();
    void setupComboBoxClassifications();
    void setReadOnly(bool ok);

    void fixClassificationsVectors();

signals:
    void submitted();

private:
    Ui::CompetitionInfoEditorWidget *ui;

    CompetitionRulesEditorWidget * rulesEditor;
    CompetitionRules trialRoundRules;
    CompetitionRules trainingsRules;

    CompetitionInfo * competitionInfo;
    QVector<Hill *> * hillsList;
    Hill * defaultHill;
    QVector<CompetitionRules> * rulesList;

public:
    CompetitionRulesEditorWidget *getRulesEditor() const;
    void setRulesEditor(CompetitionRulesEditorWidget *newRulesEditor);
    CompetitionInfo *getCompetitionInfo() const;
    void setCompetitionInfo(CompetitionInfo *newCompetitionInfo);
    QVector<Hill *> *getHillsList() const;
    void setHillsList(QVector<Hill *> *newHillsList);
    QVector<CompetitionRules> *getRulesList() const;
    void setRulesList(QVector<CompetitionRules> *newRulesList);
    Hill *getDefaultHill() const;
    void setDefaultHill(Hill *newDefaultHill);
    CompetitionRules getTrialRoundRules() const;
    void setTrialRoundRules(const CompetitionRules &newTrialRoundRules);
    CompetitionRules getTrainingsRules() const;
    void setTrainingsRules(const CompetitionRules &newTrainingsRules);
public:
    bool getTrialRoundChecked();
    int getTrainingsCount();

private slots:
    void on_pushButton_submit_clicked();
    void on_pushButton_trialRoundRules_clicked();
    void on_pushButton_trainingsRules_clicked();
};

#endif // COMPETITIONINFOEDITORWIDGET_H
