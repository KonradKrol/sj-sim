#ifndef JUMPMANIPULATORCONFIGWINDOW_H
#define JUMPMANIPULATORCONFIGWINDOW_H

#include <QDialog>
#include "../../simulator/JumpManipulator.h"
#include "../../simulator/wind-generation/WindGenerationSettings.h"

namespace Ui {
class JumpManipulatorConfigWindow;
}

class JumpManipulatorConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit JumpManipulatorConfigWindow(QWidget *parent = nullptr);
    ~JumpManipulatorConfigWindow();

    JumpManipulator getJumpManipulatorFromInputs() const;

signals:
    void submitted();

private slots:
    void on_checkBox_disableAveragedWindRange_stateChanged(int arg1);
    void on_pushButton_submit_2_clicked();
    void on_pushButton_editWinds_clicked();

private:
    Ui::JumpManipulatorConfigWindow *ui;

    QVector<WindGenerationSettings> * windGenerationSettings;
    QVector<Wind> exactWinds;
    double KPoint;

public:
    QVector<WindGenerationSettings> *getWindGenerationSettings() const;
    void setWindGenerationSettings(QVector<WindGenerationSettings> *newWindGenerationSettings);
    double getKPoint() const;
    void setKPoint(double newKPoint);
};

#endif // JUMPMANIPULATORCONFIGWINDOW_H
