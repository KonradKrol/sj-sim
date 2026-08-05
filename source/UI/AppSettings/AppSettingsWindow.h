#ifndef APPSETTINGSWINDOW_H
#define APPSETTINGSWINDOW_H

#include <QDialog>

class GlobalAppSettings;
class QCloseEvent;
class MainWindow;

namespace Ui {
class AppSettingsWindow;
}

class AppSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AppSettingsWindow(QWidget *parent = nullptr);
    ~AppSettingsWindow();

    void setupLanguagesComboBox();

    MainWindow *getMainWindowParent() const;
    void setMainWindowParent(MainWindow *newMainWindowParent);

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void on_pushButton_defaultSettings_clicked();

      void myConnect_on_comboBox_language_currentIndexChanged(int index);
    void on_spinBox_dsqProbability_valueChanged(int arg1);
    void on_spinBox_skillsRange_valueChanged(int arg1);

    void on_pushButton_repairDatabase_clicked();

    void on_pushButton_cutSurnames_clicked();

    void on_pushButton_repairDatabase_2_clicked();

    void on_doubleSpinBox_randomMultiplier_valueChanged(double arg1);

    void on_checkBox_autoAdjustHillEffects_stateChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_pushButton_autoImages_clicked();

    void on_checkBox_globalDatabaseRecords_stateChanged(int arg1);

    void on_pushButton_jumpInfoChoice_clicked();

    void on_pushButton_addJumpsImportancePreset_clicked();

    void on_pushButton_countriesEditor_clicked();

    void on_lineEdit_jumpInfoWh_editingFinished();

    void on_lineEdit_singleResultWh_editingFinished();

    void on_lineEdit_compResultsWh_editingFinished();

    void on_lineEdit_clsResultsWh_editingFinished();

    void on_doubleSpinBox_liveCompSpeed_editingFinished();

    void on_checkBox_liveCompetition_stateChanged(int arg1);

    void on_doubleSpinBox_takeoffFormEffect_editingFinished();

    void on_doubleSpinBox_flightFormEffect_editingFinished();

    void on_pushButton_loadJumpersWithCSV_clicked();

    void on_pushButton_loadPresetsBefore120_clicked();

private:
    Ui::AppSettingsWindow *ui;

    MainWindow * mainWindowParent;
};


#endif // APPSETTINGSWINDOW_H
