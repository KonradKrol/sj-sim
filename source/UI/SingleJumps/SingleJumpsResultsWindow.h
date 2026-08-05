#ifndef SINGLEJUMPSRESULTSWINDOW_H
#define SINGLEJUMPSRESULTSWINDOW_H

#include <QDialog>
#include "../../single-jumps/SingleJumpsManager.h"
#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "SingleJumpsResultsTableModel.h"

namespace QtCharts {
class QSplineSeries;
class QBarSeries;
}
class SingleJumpMiniResultWidget;

namespace Ui {
class SingleJumpsResultsWindow;
}

class SingleJumpsResultsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SingleJumpsResultsWindow(SingleJumpsManager * manager, QWidget *parent = nullptr);
    ~SingleJumpsResultsWindow();

    void fillJumperInfo();
    void fillHillInfo();
    void fillMiniJumpsResultsLayout();
    void fillDistancesChart();
    void fillPointsChart();
    void fillJudgesChart();
    void fillLandingsChart();
    void fillWindsChart();

    void fillWindow();

    void installShortcuts();

    SingleJumpsManager *getManager() const;
    void setManager(SingleJumpsManager *newManager);
    int getMaxNumberOfDistancesForChart() const;
    int getMaxNumberOfJudgesForChart() const;
    int getMaxNumberOfLandingsForChart() const;
    int getSelectedItemIndex() const;
    void setSelectedItemIndex(int newSelectedItemIndex);
    QVector<SingleJumpMiniResultWidget *> getMiniResultItems() const;
    void setMiniResultItems(const QVector<SingleJumpMiniResultWidget *> &newMiniResultItems);
    int getMaxNumberOfWindsForChart() const;
    void setMaxNumberOfWindsForChart(int newMaxNumberOfWindsForChart);

    int getMaxNumberOfPointsForChart() const;

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_sortTable_clicked();

private:
    Ui::SingleJumpsResultsWindow *ui;

    JumpDataDetailedInfoWindow * jumpInfoWidget;
    SingleJumpsResultsTableModel * model;

    SingleJumpsManager * manager;

    QtCharts::QSplineSeries * getSplineSeriesForDistancesChart();
    int maxNumberOfDistancesForChart;
    QtCharts::QSplineSeries * getSplineSeriesForPointsChart();
    int maxNumberOfPointsForChart;
    QtCharts::QSplineSeries * getSplineSeriesForJudgesChart();
    int maxNumberOfJudgesForChart;
    QtCharts::QBarSeries * getBarSeriesForLandingsChar();
    int maxNumberOfLandingsForChart;
    QtCharts::QSplineSeries * getSplineSeriesForWindsChart();
    int maxNumberOfWindsForChart;

    void askForIndexForJumpInformationShow();
};

#endif // SINGLEJUMPSRESULTSWINDOW_H
