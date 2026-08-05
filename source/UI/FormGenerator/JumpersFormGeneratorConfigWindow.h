#ifndef JUMPERSFORMGENERATORCONFIGWINDOW_H
#define JUMPERSFORMGENERATORCONFIGWINDOW_H

#include <QDialog>
#include <QVector>
#include <QAction>
#include "JumperFormGeneratorsSettingsTableModel.h"
#include "../../form-generator/JumperFormGeneratorSettingsPreset.h"
#include "../../form-generator/JumperFormTendence.h"
#include "../../seasons/SimulationSave.h"

namespace Ui {
class JumpersFormGeneratorConfigWindow;
}

class JumpersFormGeneratorConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit JumpersFormGeneratorConfigWindow(QWidget *parent = nullptr);
    ~JumpersFormGeneratorConfigWindow();

    void updateTable();
    JumperFormGeneratorSettings getSettingsFromInputs();

private:
    Ui::JumpersFormGeneratorConfigWindow *ui;

    QAction * removeFromTableAction;
    QAction * alternativeRemoveFromTableAction;
    JumperFormGeneratorsSettingsTableModel * tableModel;
    SimulationSave * save;
public:
    JumperFormGeneratorsSettingsTableModel *getTableModel() const;
    QVector<JumperFormTendence> *getJumpersTendences() const;
    void setJumpersTendences(QVector<JumperFormTendence> *newJumpersTendences);

    SimulationSave *getSave() const;
    void setSave(SimulationSave *newSave);

    QAction *getRemoveFromTableAction() const;
    void setRemoveFromTableAction(QAction *newRemoveFromTableAction);

public slots:
    void on_doubleSpinBox_tendVariability_editingFinished();
    void on_doubleSpinBox_formBonus_editingFinished();
    void on_doubleSpinBox_minForm_editingFinished();
    void on_doubleSpinBox_maxForm_editingFinished();
private slots:
    void on_listWidget_questions_doubleClicked(const QModelIndex &index);
    void on_pushButton_saveAsPreset_clicked();
    void on_pushButton_loadPreset_clicked();
    void on_pushButton_generate_clicked();
    void onRemoveFromTable(bool alternative);
    void on_doubleSpinBox_formVariability_editingFinished();
    void on_pushButton_jumpersLists_clicked();
};

#endif // JUMPERSFORMGENERATORCONFIGWINDOW_H
