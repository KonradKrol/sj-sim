#ifndef JUMPMANIPULATORCONFIGWINDOW_H
#define JUMPMANIPULATORCONFIGWINDOW_H

#include <QDialog>
#include "../../simulator/JumpManipulator.h"
#include "../../simulator/wind-generation/WindGenerationSettings.h"
#include "../../hills/HillProfile.h"
class SimulationJumpDraft;

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
    void configureSimulationV2(const JumpManipulator &,const HillProfile &,const QVector<Wind> &,bool allowDsq);

signals:
    void submitted();

private slots:
    void on_checkBox_disableAveragedWindRange_stateChanged(int arg1);
    void on_pushButton_submit_2_clicked();
    void on_pushButton_editWinds_clicked();

private:
    Ui::JumpManipulatorConfigWindow *ui;

    QVector<WindGenerationSettings> * windGenerationSettings=nullptr;
    QVector<Wind> exactWinds;
    double KPoint=120;
    SimulationJumpDraft *draft=nullptr;

public:
    QVector<WindGenerationSettings> *getWindGenerationSettings() const;
    void setWindGenerationSettings(QVector<WindGenerationSettings> *newWindGenerationSettings);
    double getKPoint() const;
    void setKPoint(double newKPoint);
};

#endif // JUMPMANIPULATORCONFIGWINDOW_H
