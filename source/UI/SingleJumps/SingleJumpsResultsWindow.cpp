#include "SingleJumpsResultsWindow.h"
#include "ui_SingleJumpsResultsWindow.h"

#include "SingleJumpsResultsTableSortDialog.h"
#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/Uuid.h"

#include <math.h>
#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QBarSeries>
#include <QBarSet>
QT_CHARTS_USE_NAMESPACE
#include <QEventLoop>
#include <QMap>
#include <QDebug>
#include <QAction>
#include <QInputDialog>

extern Uuid globalIDGenerator;

SingleJumpsResultsWindow::SingleJumpsResultsWindow(SingleJumpsManager *manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingleJumpsResultsWindow),
    maxNumberOfDistancesForChart(0),
    maxNumberOfJudgesForChart(0),
    maxNumberOfLandingsForChart(0),
    maxNumberOfWindsForChart(0),
    maxNumberOfPointsForChart(0),
    manager(manager)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    jumpInfoWidget = new JumpDataDetailedInfoWindow(nullptr, this);
    jumpInfoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout_jumpInfos->addWidget(jumpInfoWidget);
    jumpInfoWidget->hide();

    model = new SingleJumpsResultsTableModel(this->manager, this);
    ui->tableView->setModel(model);
}

SingleJumpsResultsWindow::~SingleJumpsResultsWindow()
{
    delete ui;
}

void SingleJumpsResultsWindow::fillJumperInfo()
{
    ui->label_jumperNameAndSurname->setText(manager->getJumper().getNameAndSurname());
    ui->label_jumperFlag->setPixmap(CountryFlagsManager::getFlagPixmap(manager->getJumper().getCountryCode().toLower()).scaled(ui->label_jumperFlag->size()));
}

void SingleJumpsResultsWindow::fillHillInfo()
{
    ui->label_hillName->setText(manager->getHill().getName() + " HS" + QString::number(manager->getHill().getHSPoint()));
    ui->label_hillFlag->setPixmap(CountryFlagsManager::getFlagPixmap(manager->getHill().getCountryCode().toLower()).scaled(ui->label_hillFlag->size()));
}

void SingleJumpsResultsWindow::fillDistancesChart()
{
    if(ui->verticalLayout_distanceStatistics->count() > 0){
        ui->verticalLayout_distanceStatistics->removeWidget(ui->verticalLayout_distanceStatistics->itemAt(0)->widget());
    }

    QChart * distancesChart = new QChart;
    distancesChart->legend()->hide();
    distancesChart->addSeries(getSplineSeriesForDistancesChart());
    distancesChart->setTitle("Rozkład odległości zawodnika");
    distancesChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    distancesChart->createDefaultAxes();
    distancesChart->axes(Qt::Vertical).first()->setRange(1, getMaxNumberOfDistancesForChart() * 1.15);

    QChartView * distancesChartView = new QChartView(distancesChart);
    distancesChartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout_distanceStatistics->addWidget(distancesChartView);
}

void SingleJumpsResultsWindow::fillPointsChart()
{
    if(ui->verticalLayout_pointsStatistics->count() > 0){
        ui->verticalLayout_pointsStatistics->removeWidget(ui->verticalLayout_pointsStatistics->itemAt(0)->widget());
    }

    QChart * pointsChart = new QChart;
    pointsChart->legend()->hide();
    pointsChart->addSeries(getSplineSeriesForPointsChart());
    pointsChart->setTitle(tr("Rozkład punktów zawodnika"));
    pointsChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    pointsChart->createDefaultAxes();
    pointsChart->axes(Qt::Vertical).first()->setRange(1, getMaxNumberOfPointsForChart() * 1.15);

    QChartView * pointsChartView = new QChartView(pointsChart);
    pointsChartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout_pointsStatistics->addWidget(pointsChartView);
}

void SingleJumpsResultsWindow::fillJudgesChart()
{
    if(ui->verticalLayout_judgesStatistics->count() > 0){
        ui->verticalLayout_judgesStatistics->removeWidget(ui->verticalLayout_judgesStatistics->itemAt(0)->widget());
    }

    QChart * judgesChart = new QChart();
    judgesChart->legend()->hide();
    judgesChart->addSeries(getSplineSeriesForJudgesChart());
    judgesChart->setTitle("Rozkład not sędziowskich zawodnika");
    judgesChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    judgesChart->createDefaultAxes();
    judgesChart->axes(Qt::Vertical).first()->setRange(1, maxNumberOfJudgesForChart * 1.15);

    QChartView * judgesChartView = new QChartView(judgesChart);
    judgesChartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout_judgesStatistics->addWidget(judgesChartView);
}

void SingleJumpsResultsWindow::fillLandingsChart()
{
    if(ui->verticalLayout_landingsStatistics->count() > 0){
        ui->verticalLayout_landingsStatistics->removeWidget(ui->verticalLayout_landingsStatistics->itemAt(0)->widget());
    }

    QChart * landingsChart = new QChart();
    landingsChart->addSeries(getBarSeriesForLandingsChar());
    landingsChart->setTitle("Rozkład rodzajów lądowania zawodnika");
    landingsChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    landingsChart->createDefaultAxes();
    landingsChart->axes(Qt::Vertical).first()->setRange(0, maxNumberOfLandingsForChart * 1.15);

    QChartView * landingsChartView = new QChartView(landingsChart);
    landingsChartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout_landingsStatistics->addWidget(landingsChartView);
}

void SingleJumpsResultsWindow::fillWindsChart()
{
    if(ui->verticalLayout_windStatistics->count() > 0){
        ui->verticalLayout_windStatistics->removeWidget(ui->verticalLayout_windStatistics->itemAt(0)->widget());
    }

    QChart * windsChart = new QChart();
    windsChart->addSeries(getSplineSeriesForWindsChart());
    windsChart->setTitle("Rozkład siły wiatru przy skokach");
    windsChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    windsChart->createDefaultAxes();
    windsChart->axes(Qt::Vertical).first()->setRange(0, maxNumberOfWindsForChart * 1.15);

    QChartView * windsChartView = new QChartView(windsChart);
    windsChartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout_windStatistics->addWidget(windsChartView);
}

void SingleJumpsResultsWindow::fillWindow()
{
    fillJumperInfo();
    fillHillInfo();
    fillDistancesChart();
    fillPointsChart();
    fillJudgesChart();
    fillLandingsChart();
    fillWindsChart();
}

SingleJumpsManager *SingleJumpsResultsWindow::getManager() const
{
    return manager;
}

void SingleJumpsResultsWindow::setManager(SingleJumpsManager *newManager)
{
    manager = newManager;
}

QSplineSeries *SingleJumpsResultsWindow::getSplineSeriesForDistancesChart()
{
    QSplineSeries * series = new QSplineSeries(this);
    QMap<int, int> distances;
    for(const auto & jump : manager->getJumps())
    {
        if(jump.getDSQ() == false){
            double distance = std::round(jump.getDistance());
            if(distances.contains(distance))
            {
                distances[distance] += 1;
            }
            else{
                distances.insert(distance, 1);
            }
        }
    }

    for(auto & key : distances.keys())
    {
        if(distances.value(key) > maxNumberOfDistancesForChart) maxNumberOfDistancesForChart = distances.value(key);
        series->append(key, distances.value(key));
    }

    return series;
}

QSplineSeries *SingleJumpsResultsWindow::getSplineSeriesForPointsChart()
{
    QSplineSeries * series = new QSplineSeries(this);
    QMap<int, int> pointsMap;
    for(const auto & jump : manager->getJumps())
    {
        if(jump.getDSQ() == false){
            double points = std::round(jump.getPoints());
            if(pointsMap.contains(points))
            {
                pointsMap[points] += 1;
            }
            else{
                pointsMap.insert(points, 1);
            }
        }
    }

    for(auto & key : pointsMap.keys())
    {
        if(pointsMap.value(key) > maxNumberOfPointsForChart) maxNumberOfPointsForChart = pointsMap.value(key);
        series->append(key, pointsMap.value(key));
    }

    return series;
}

int SingleJumpsResultsWindow::getMaxNumberOfPointsForChart() const
{
    return maxNumberOfPointsForChart;
}

QSplineSeries *SingleJumpsResultsWindow::getSplineSeriesForJudgesChart()
{
    QSplineSeries * series = new QSplineSeries(this);
    QMap<double, int> judges;
    for(const auto & jump : manager->getJumps())
    {
        if(jump.getDSQ() == false){
            for(int i=0; i<5; i++){
                double judge = jump.getJudges().at(i);//std::round(jump.getJudges().at(i));
                if(judges.contains(judge))
                {
                    judges[judge] += 1;
                }
                else{
                    judges[judge] = 1;
                }
            }
        }
    }
    for(auto & key : judges.keys())
    {
        if(judges.value(key) > maxNumberOfJudgesForChart) maxNumberOfJudgesForChart = judges.value(key);
        series->append(key, judges.value(key));
    }

    return series;
}

QBarSeries *SingleJumpsResultsWindow::getBarSeriesForLandingsChar()
{
    QBarSet * telemarkSet = new QBarSet(tr("Telemark"));
    QBarSet * bothLegsSet = new QBarSet(tr("Na dwie nogi"));
    QBarSet * supportSet = new QBarSet(tr("Podpórka"));
    QBarSet * fallSet = new QBarSet(tr("Upadek"));

    QMap<short, int> landings;
    for(const auto & jump : manager->getJumps())
    {
        if(jump.getDSQ() == false){
            short type = jump.getLanding().getType();
            if(landings.contains(type))
            {
                landings[type] += 1;
            }
            else{
                landings[type] = 1;
            }
        }
    }
    for(auto & key : landings.keys())
    {
        if(landings.value(key) > maxNumberOfLandingsForChart) maxNumberOfLandingsForChart = landings.value(key);
    }
    telemarkSet->append(landings.value(Landing::TelemarkLanding));
    bothLegsSet->append(landings.value(Landing::BothLegsLanding));
    supportSet->append(landings.value(Landing::SupportLanding));
    fallSet->append(landings.value(Landing::Fall));

    QBarSeries * series = new QBarSeries;
    series->append(telemarkSet);
    series->append(bothLegsSet);
    series->append(supportSet);
    series->append(fallSet);

    return series;
}

QSplineSeries *SingleJumpsResultsWindow::getSplineSeriesForWindsChart()
{
    QSplineSeries * series = new QSplineSeries(this);
    QMap<double, int> winds;
    for(const auto & jump : manager->getJumps())
    {
        if(jump.getDSQ() == false){
            double wind = jump.getAveragedWind();
            wind *= 20;
            wind = round(wind);
            wind *= 0.05;
            if(winds.contains(wind))
            {
                winds[wind] += 1;
            }
            else{
                winds.insert(wind, 1);
            }
        }
    }
    for(auto & key : winds.keys())
    {
        if(winds.value(key) > maxNumberOfWindsForChart) maxNumberOfWindsForChart = winds.value(key);
        series->append(key, winds.value(key));
    }

    return series;
}

void SingleJumpsResultsWindow::installShortcuts()
{
    QAction * action = new QAction(this);
    action->setShortcut(Qt::CTRL | Qt::Key_I);
    connect(action, &QAction::triggered, this, &SingleJumpsResultsWindow::askForIndexForJumpInformationShow);
    this->addAction(action);
}

void SingleJumpsResultsWindow::askForIndexForJumpInformationShow()
{
    if(manager->getJumpsCount() > 0){
        QInputDialog dialog;
        dialog.setInputMode(QInputDialog::IntInput);
        dialog.setModal(true);
        dialog.setIntMinimum(1);
        dialog.setIntMaximum(manager->getJumpsCount());
        dialog.setStyleSheet("QWidget{background-color: rgb(225, 225, 225); color: black;}");
        dialog.setWindowTitle(tr("Przejdź do wybranej informacji o skoku"));
        dialog.setLabelText(tr("Wpisz numer skoku do którego chcesz się przenieść"));
        if(dialog.exec() == QDialog::Accepted)
        {
            int index = dialog.intValue() - 1;
            jumpInfoWidget->setJumpData(const_cast<JumpData *>(&manager->getJumps().at(index)));
            jumpInfoWidget->fillJumpInformations();
            ui->tableView->selectRow(index);
            ui->tableView->scrollTo(model->index(index, 1));
            ui->toolBox->setCurrentIndex(0);
        }
    }
}

int SingleJumpsResultsWindow::getMaxNumberOfWindsForChart() const
{
    return maxNumberOfWindsForChart;
}

void SingleJumpsResultsWindow::setMaxNumberOfWindsForChart(int newMaxNumberOfWindsForChart)
{
    maxNumberOfWindsForChart = newMaxNumberOfWindsForChart;
}

int SingleJumpsResultsWindow::getMaxNumberOfLandingsForChart() const
{
    return maxNumberOfLandingsForChart;
}
int SingleJumpsResultsWindow::getMaxNumberOfJudgesForChart() const
{
    return maxNumberOfJudgesForChart;
}

int SingleJumpsResultsWindow::getMaxNumberOfDistancesForChart() const
{
    return maxNumberOfDistancesForChart;
}

void SingleJumpsResultsWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
     ui->toolBox->setCurrentIndex(0);
     jumpInfoWidget->setJumpData(const_cast<JumpData *>(&manager->getJumps().at(row)));
     if(jumpInfoWidget->isHidden())
         jumpInfoWidget->show();
     jumpInfoWidget->fillJumpInformations();
}


void SingleJumpsResultsWindow::on_pushButton_sortTable_clicked()
{
     SingleJumpsResultsTableSortDialog dialog(this);
     if(dialog.exec() == QDialog::Accepted)
     {
         switch(dialog.sortBy())
         {
         case SingleJumpsResultsTableSortDialog::Distance:
         {
            std::sort(manager->getJumpsReference().begin(), manager->getJumpsReference().end(), [this, &dialog](const JumpData & j1, const JumpData & j2){
                if(j1.getDistance() == j2.getDistance())
                    return j1.getIDStr() > j2.getIDStr();
                if(dialog.sortType() == SingleJumpsResultsTableSortDialog::Descending)
                    return j1.getDistance() > j2.getDistance();
                else
                    return j1.getDistance() < j2.getDistance();
            });
            break;
         }
         case SingleJumpsResultsTableSortDialog::Points:
         {
            std::sort(manager->getJumpsReference().begin(), manager->getJumpsReference().end(), [this, &dialog](const JumpData & j1, const JumpData & j2){
                if(j1.getDistance() == j2.getDistance())
                    return j1.getIDStr() > j2.getIDStr();
                if(dialog.sortType() == SingleJumpsResultsTableSortDialog::Descending)
                    return j1.getPoints() > j2.getPoints();
                else
                    return j1.getPoints() < j2.getPoints();
            });
            break;
         }
         case SingleJumpsResultsTableSortDialog::Wind:
         {
            std::sort(manager->getJumpsReference().begin(), manager->getJumpsReference().end(), [this, &dialog](const JumpData & j1, const JumpData & j2){
                if(j1.getDistance() == j2.getDistance())
                    return j1.getIDStr() > j2.getIDStr();
                if(dialog.sortType() == SingleJumpsResultsTableSortDialog::Descending)
                    return j1.getAveragedWind() > j2.getAveragedWind();
                else
                    return j1.getAveragedWind() < j2.getAveragedWind();
            });
            break;
         }
         case SingleJumpsResultsTableSortDialog::Judges:
         {
            std::sort(manager->getJumpsReference().begin(), manager->getJumpsReference().end(), [this, &dialog](const JumpData & j1, const JumpData & j2){
                if(j1.getDistance() == j2.getDistance())
                    return j1.getIDStr() > j2.getIDStr();
                if(dialog.sortType() == SingleJumpsResultsTableSortDialog::Descending)
                    return j1.getJudgesPoints() > j2.getJudgesPoints();
                else
                    return j1.getJudgesPoints() < j2.getJudgesPoints();
            });
            break;
         }
         }
         fillWindow();
         ui->tableView->setModel(nullptr);
         ui->tableView->setModel(model);
     }
}
