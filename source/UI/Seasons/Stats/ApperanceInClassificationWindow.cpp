#include "ApperanceInClassificationWindow.h"
#include "ui_ApperanceInClassificationWindow.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../global/GlobalDatabase.h"
#include "../../../simulator/Jumper.h"
#include "../../../simulator/Team.h"
#include "../../../competitions/CompetitionInfo.h"
#include "../../../utilities/functions.h"

ApperanceInClassificationWindow::ApperanceInClassificationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApperanceInClassificationWindow),
    jumper(nullptr)
{
    ui->setupUi(this);

    setupChart();
}

ApperanceInClassificationWindow::~ApperanceInClassificationWindow()
{
    delete ui;
}

void ApperanceInClassificationWindow::fillWindow()
{
    if(jumper != nullptr)
    {
        ui->label_jumperNameAndSurname->setText(jumper->getNameAndSurname());
        ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(jumper->getCountryCode().toLower())
                                      .scaled(ui->label_flag->size()));
        ui->label_img->show();
        ui->label_img->setPixmap(jumper->getImagePixmap().scaled(ui->label_img->size()));
    }
    else if(teamCode != "")
    {
        ui->label_jumperNameAndSurname->setText(GlobalDatabase::get()->getCountryByAlpha3(teamCode)->getName());
        ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(teamCode.toLower())
                                      .scaled(ui->label_flag->size()));
        ui->label_img->hide();
    }
    fillChart();
    ui->label_classification->setText(classification->getName());

}

void ApperanceInClassificationWindow::setupChart()
{
    chart = new QChart();
    chart->legend()->hide();
    chart->setTitle(tr("Miejsca w klasyfikacji"));
    chart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->verticalLayout_chart->addWidget(chartView);
    series = new QLineSeries(this);
    series->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(series, &QLineSeries::hovered, this, &ApperanceInClassificationWindow::updateChartCompetition);
}

void ApperanceInClassificationWindow::fillChart()
{
    series->clear();

    //Mamy w archiveResults pozycje Kamila Stocha w klasyfikacji po każdym konkursie
    //Chcemy na wykresie pokazać pozycje po koleji (od najstarszego konkursu)
    archiveResultsCompetitions = archiveResults.keys().toVector();
    positions.clear();

    SeasonCalendar * c = nullptr;
    for(auto & calendar : season->getCalendarsReference())
        if(calendar->getClassificationsReference().contains(classification))
            c = calendar;
    for(auto & comp : c->getCompetitionsReference())
    {
        if(archiveResultsCompetitions.contains(comp))
            positions.push_back(archiveResults.value(comp));
    }

    int i=1;
    for(auto & position : positions)
    {
        series->append(i, position);
        i++;
    }

    int bestPos = 100000000;
    int worstPos = 1;
    double avgPos = 0;
    for(auto & pos : positions)
    {
        if(pos < bestPos)
            bestPos = pos;
        if(pos > worstPos)
            worstPos = pos;
        avgPos += pos;
    }
    avgPos /= positions.count();
    avgPos = roundDoubleToTwoPlaces(avgPos);
    double standardDeviation = roundDoubleToOnePlace(MyFunctions::standardDeviation(positions, avgPos));

    ui->label_bestPos->setText(QString::number(bestPos));
    ui->label_worstPos->setText(QString::number(worstPos));
    ui->label_averagePos->setText(QString::number(avgPos));
    ui->label_standardDeviation->setText(QString::number(standardDeviation));

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setReverse(true);
    chart->axes(Qt::Vertical).first()->setRange(1, worstPos);
    chart->axes(Qt::Horizontal).first()->setLabelsVisible(false);
    chart->zoomReset();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void ApperanceInClassificationWindow::updateChartCompetition(const QPointF &point, bool state)
{
    if(state)
    {
        if((point.x() + 0.25 > int(point.x()) && point.x() - 0.25 < int(point.x())) && (point.y() + 0.25 > point.y() && point.y() - 0.25 < point.y()))
        {
            CompetitionInfo * competition = nullptr;
            SeasonCalendar * calendar = nullptr;
            int i=0;
            for(auto & cl : season->getCalendarsReference())
            {
                for(auto & comp : cl->getCompetitionsReference())
                {
                    if(archiveResultsCompetitions.contains(comp)){
                        if(i==int(point.x() - 1)){
                            competition = comp;
                            calendar = cl;
                            break;
                        }
                        i++;
                    }
                }
            }
            Hill * hill = competition->getHill();

            QString string = QString::number(season->getSeasonNumber()) + "/" + QString::number(calendar->getCompetitionsReference().indexOf(competition) + 1)
                             + ": " + hill->getName() + " HS" + QString::number(hill->getHSPoint());
            switch(competition->getSerieType())
            {
            case CompetitionInfo::Competition:
                string += tr(" (Konkurs)");
                break;
            case CompetitionInfo::Qualifications:
                string += tr(" (Kwalifikacje)");
                break;
            }
            switch(competition->getRulesPointer()->getCompetitionType())
            {
            case CompetitionRules::Individual:
                string += tr(" (Ind.)");
                break;
            case CompetitionRules::Team:
                string += tr(" (Druż.)");
                break;
            default:
                break;
            }

            ui->label_chartStat->setText(QString::number(positions[int(point.x()) - 1]) + tr(" miejsce"));
            ui->label_chartCompetition->setText(string);
            QPixmap pixmap = CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()).scaled(ui->label_hillFlag->size());
            ui->label_hillFlag->setPixmap(pixmap);
        }
    }
}

Season *ApperanceInClassificationWindow::getSeason() const
{
    return season;
}

void ApperanceInClassificationWindow::setSeason(Season *newSeason)
{
    season = newSeason;
}

QHash<CompetitionInfo *, int> ApperanceInClassificationWindow::getArchiveResults() const
{
    return archiveResults;
}

void ApperanceInClassificationWindow::setArchiveResults(const QHash<CompetitionInfo *, int> &newArchiveResults)
{
    archiveResults = newArchiveResults;
}

Classification *ApperanceInClassificationWindow::getClassification() const
{
    return classification;
}

void ApperanceInClassificationWindow::setClassification(Classification *newClassification)
{
    classification = newClassification;
}

QString ApperanceInClassificationWindow::getTeamCode() const
{
    return teamCode;
}

void ApperanceInClassificationWindow::setTeamCode(const QString &newTeamCode)
{
    teamCode = newTeamCode;
}

Jumper *ApperanceInClassificationWindow::getJumper() const
{
    return jumper;
}

void ApperanceInClassificationWindow::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}
