#ifndef SIMULATIONSAVEINFOWIDGET_H
#define SIMULATIONSAVEINFOWIDGET_H

#include <QWidget>
#include "../../seasons/SimulationSave.h"

namespace Ui {
class SimulationSaveInfoWidget;
}

class SimulationSaveInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationSaveInfoWidget(QWidget *parent = nullptr);
    ~SimulationSaveInfoWidget();

    void resetInputs();
    void fillInputs();

private:
    Ui::SimulationSaveInfoWidget *ui;
    SimulationSave * simulationSave;
public:
    SimulationSave *getSimulationSave() const;
    void setSimulationSave(SimulationSave *newSimulationSave);
};

#endif // SIMULATIONSAVEINFOWIDGET_H
