#ifndef MULTIPLETRAININGSEDITDIALOG_H
#define MULTIPLETRAININGSEDITDIALOG_H

#include <QDialog>
#include "../../EditorWidgets/CompetitionRulesEditorWidget.h"

namespace Ui {
class MultipleTrainingsEditDialog;
}

class MultipleTrainingsEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultipleTrainingsEditDialog(QWidget *parent = nullptr);
    ~MultipleTrainingsEditDialog();

    int getTrainingsCount();
    CompetitionRules getTrainingsRules();
    double getJumpsImportance();

signals:
    void submitted();

private slots:
    void on_pushButton_trainingsRules_clicked();
    void on_pushButton_submit_clicked();

    void on_comboBox_importancePreset_currentIndexChanged(int index);

private:
    Ui::MultipleTrainingsEditDialog *ui;
    CompetitionRules trainingsRules;
    QVector<CompetitionRules> * rulesList;

public:
    QVector<CompetitionRules> *getRulesList() const;
    void setRulesList(QVector<CompetitionRules> *newRulesList);
    void setTrainingsRules(const CompetitionRules &newTrainingsRules);
    void setCount(int count);
    void setImp(double imp);
};

#endif // MULTIPLETRAININGSEDITDIALOG_H
