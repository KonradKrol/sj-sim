#ifndef MULTIPLETRIALROUNDSEDITDIALOG_H
#define MULTIPLETRIALROUNDSEDITDIALOG_H

#include <QDialog>
#include "../../EditorWidgets/CompetitionRulesEditorWidget.h"


namespace Ui {
class MultipleTrialRoundsEditDialog;
}

class MultipleTrialRoundsEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultipleTrialRoundsEditDialog(QWidget *parent = nullptr);
    ~MultipleTrialRoundsEditDialog();

    bool getTrialRoundChecked();
    CompetitionRules getTrialRoundRules();
    double getJumpsImportance();

signals:
    void submitted();

private slots:
    void on_pushButton_trainingsRules_clicked();

    void on_pushButton_submit_clicked();

    void on_comboBox_importancePreset_currentIndexChanged(int index);

private:
    Ui::MultipleTrialRoundsEditDialog *ui;
    CompetitionRules trialRoundRules;
    QVector<CompetitionRules> * rulesList;
    double imp;
public:
    QVector<CompetitionRules> *getRulesList() const;
    void setRulesList(QVector<CompetitionRules> *newRulesList);
    void setTrialRoundRules(const CompetitionRules &newTrialRoundRules);
    void setTrialRound(bool ok);
    void setImp(double newImp);
};

#endif // MULTIPLETRIALROUNDSEDITDIALOG_H
