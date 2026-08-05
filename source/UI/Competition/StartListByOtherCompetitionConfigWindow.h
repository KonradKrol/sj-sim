#ifndef STARTLISTBYOTHERCOMPETITIONCONFIGWINDOW_H
#define STARTLISTBYOTHERCOMPETITIONCONFIGWINDOW_H

#include <QDialog>
#include "../../competitions/CompetitionInfo.h"
#include "../../seasons/SimulationSave.h"

namespace Ui {
class StartListByOtherCompetitionConfigWindow;
}

class StartListByOtherCompetitionConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StartListByOtherCompetitionConfigWindow(QWidget *parent = nullptr);
    ~StartListByOtherCompetitionConfigWindow();

    void setupComboBox();
    CompetitionInfo * getCompetition();
    bool getCheckBoxState();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::StartListByOtherCompetitionConfigWindow *ui;
    SimulationSave * save;
    int competitionType;
public:
    SimulationSave *getSave() const;
    void setSave(SimulationSave *newSave);
    int getCompetitionType() const;
    void setCompetitionType(int newCompetitionType);
};

#endif // STARTLISTBYOTHERCOMPETITIONCONFIGWINDOW_H
