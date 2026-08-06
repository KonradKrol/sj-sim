#ifndef SINGLEJUMPSRESULTSWINDOW_H
#define SINGLEJUMPSRESULTSWINDOW_H

#include <QDialog>
#include <QtCharts/QChartGlobal>
#include "../../single-jumps/SingleJumpsManager.h"
#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "SingleJumpsResultsTableModel.h"

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QBarSeries;
QT_CHARTS_END_NAMESPACE
QT_CHARTS_USE_NAMESPACE
class SingleJumpMiniResultWidget;
class QResizeEvent;
class QSplitter;

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
    QSplitter *resultsSplitter;
    bool compactLayout;

    JumpDataDetailedInfoWindow * jumpInfoWidget;
    SingleJumpsResultsTableModel * model;

    SingleJumpsManager * manager;

    QSplineSeries * getSplineSeriesForDistancesChart();
    int maxNumberOfDistancesForChart;
    QSplineSeries * getSplineSeriesForPointsChart();
    int maxNumberOfPointsForChart;
    QSplineSeries * getSplineSeriesForJudgesChart();
    int maxNumberOfJudgesForChart;
    QBarSeries * getBarSeriesForLandingsChar();
    int maxNumberOfLandingsForChart;
    QSplineSeries * getSplineSeriesForWindsChart();
    int maxNumberOfWindsForChart;

    void askForIndexForJumpInformationShow();
    void showJumpDetails(int row);
    void updateResponsiveLayout(int windowWidth);

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // SINGLEJUMPSRESULTSWINDOW_H
