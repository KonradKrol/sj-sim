#include "JumperStatsWindow.h"
#include "ui_JumperStatsWindow.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../utilities/functions.h"
#include "../../ResponsiveWindowUtils.h"
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QScrollArea>

JumperStatsWindow::JumperStatsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumperStatsWindow)
{
    ui->setupUi(this);
    setMinimumSize(640, 480);

    QLayoutItem *headerItem = ui->verticalLayout_2->takeAt(0);
    QLayout *headerLayout = headerItem->layout();
    QWidget *identityPanel = new QWidget(this);
    QHBoxLayout *identityLayout = new QHBoxLayout(identityPanel);
    identityLayout->setContentsMargins(0, 0, 0, 0);
    QScrollArea *filtersArea = new QScrollArea(this);
    filtersArea->setObjectName("scrollArea_filters");
    filtersArea->setFrameShape(QFrame::NoFrame);
    filtersArea->setWidgetResizable(true);
    filtersArea->setMinimumHeight(95);
    filtersArea->setMaximumHeight(150);
    filtersArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QWidget *filtersPanel = new QWidget(filtersArea);
    QGridLayout *filtersLayout = new QGridLayout(filtersPanel);
    filtersLayout->setContentsMargins(0, 0, 0, 0);

    const QList<QWidget *> headerWidgets = {
        ui->pushButton_csvExport, ui->label_img, ui->label_jumperNameAndSurname,
        ui->label_jumperFlag, ui->checkBox_mergeCalendars, ui->comboBox_calendar,
        ui->comboBox_hillFilter, ui->checkBox_withDSQ, ui->checkBox
    };
    for(QWidget *widget : headerWidgets)
        headerLayout->removeWidget(widget);
    const QList<QLayout *> filterGroups = {
        ui->verticalLayout_classificationsComboBoxes, ui->verticalLayout_rangeComboBoxes,
        ui->verticalLayout_hillTypesCheckBoxes, ui->verticalLayout_serieTypesCheckBoxes
    };
    for(QLayout *layout : filterGroups)
        headerLayout->removeItem(layout);
    while(QLayoutItem *item = headerLayout->takeAt(0))
        delete item;
    delete headerItem;

    identityLayout->addWidget(ui->pushButton_csvExport);
    identityLayout->addWidget(ui->label_img);
    identityLayout->addWidget(ui->label_jumperNameAndSurname, 1);
    identityLayout->addWidget(ui->label_jumperFlag);

    filtersLayout->addWidget(ui->checkBox_mergeCalendars, 0, 0);
    filtersLayout->addWidget(ui->comboBox_calendar, 0, 1);
    filtersLayout->addWidget(ui->comboBox_hillFilter, 0, 2);
    filtersLayout->addWidget(ui->checkBox_withDSQ, 0, 3);
    filtersLayout->addWidget(ui->checkBox, 0, 4);
    filtersLayout->addLayout(ui->verticalLayout_classificationsComboBoxes, 1, 0);
    filtersLayout->addLayout(ui->verticalLayout_rangeComboBoxes, 1, 1);
    filtersLayout->addLayout(ui->verticalLayout_hillTypesCheckBoxes, 1, 2);
    filtersLayout->addLayout(ui->verticalLayout_serieTypesCheckBoxes, 1, 3, 1, 2);
    filtersLayout->setColumnStretch(1, 1);
    filtersLayout->setColumnStretch(2, 1);
    filtersPanel->setMinimumWidth(720);
    ui->comboBox_calendar->setMinimumWidth(140);
    ui->comboBox_calendar->setMaximumWidth(240);
    ui->comboBox_hillFilter->setMinimumWidth(160);
    ui->comboBox_hillFilter->setMaximumWidth(300);

    filtersArea->setWidget(filtersPanel);
    ui->verticalLayout_2->insertWidget(0, filtersArea);
    ui->verticalLayout_2->insertWidget(0, identityPanel);
    ui->label_jumperNameAndSurname->setWordWrap(true);
    ui->label_jumperNameAndSurname->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    rangeComboBoxes = new CompetitionsRangeComboBoxesWidget(this);
    ui->verticalLayout_rangeComboBoxes->addWidget(rangeComboBoxes);

    serieTypesCheckBoxes = new SerieTypesComboBoxesWidget(this);
    ui->verticalLayout_serieTypesCheckBoxes->addWidget(serieTypesCheckBoxes);

    classificationsCheckBoxes = new ClassificationsCheckBoxesWidget(this);
    ui->verticalLayout_classificationsComboBoxes->addWidget(classificationsCheckBoxes);

    hillTypesCheckBoxes = new HillTypesCheckBoxesWidget(this);
    ui->verticalLayout_hillTypesCheckBoxes->addWidget(hillTypesCheckBoxes);

    /*layout_takeoffRatingChart = new QVBoxLayout();
    ui->verticalLayout_charts->addItem(layout_takeoffRatingChart);
    layout_flightRatingChart = new QVBoxLayout();
    ui->verticalLayout_charts->layout()->addItem(layout_flightRatingChart);*/

    ui->pushButton_csvExport->hide();
    setupCharts();
    emit ui->checkBox->stateChanged(0);
    ResponsiveWindowUtils::manageWindowGeometry(this, "JumperStatsWindow");
}

JumperStatsWindow::~JumperStatsWindow()
{
    delete ui;
}

void JumperStatsWindow::setupConnections()
{
    connect(rangeComboBoxes, &CompetitionsRangeComboBoxesWidget::changed, this, &JumperStatsWindow::fillWindow);
    connect(serieTypesCheckBoxes, &SerieTypesComboBoxesWidget::changed, this, &JumperStatsWindow::fillWindow);
    connect(classificationsCheckBoxes, &ClassificationsCheckBoxesWidget::changed, this, &JumperStatsWindow::fillWindow);
    connect(hillTypesCheckBoxes, &HillTypesCheckBoxesWidget::changed, this, &JumperStatsWindow::fillWindow);
}

void JumperStatsWindow::setupComboBox()
{
    disconnect(ui->comboBox_hillFilter, qOverload<int>(&QComboBox::currentIndexChanged), this, &JumperStatsWindow::fillWindow);
    ui->comboBox_hillFilter->clear();
    ui->comboBox_hillFilter->addItem(tr("BRAK"));
    for(auto & hill : save->getHillsReference())
    {
        ui->comboBox_hillFilter->addItem(QIcon(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower())), hill->getHillText());
    }
    connect(ui->comboBox_hillFilter, qOverload<int>(&QComboBox::currentIndexChanged), this, &JumperStatsWindow::fillWindow);

    disconnect(ui->comboBox_calendar, qOverload<int>(&QComboBox::currentIndexChanged), this, &JumperStatsWindow::fillWindow);
    disconnect(ui->comboBox_calendar, qOverload<int>(&QComboBox::currentIndexChanged), rangeComboBoxes, &CompetitionsRangeComboBoxesWidget::setupComboBoxes);
    for(auto & c : toDisconnect)
        disconnect(c);
    toDisconnect.clear();

    ui->comboBox_calendar->clear();
    for(auto & cal : save->getActualSeason()->getCalendarsReference())
    {
        ui->comboBox_calendar->addItem(cal->getName());
    }
    //ui->comboBox_calendar->setCurrentIndex(save->getActualSeason()->getCalendarsReference().indexOf(save->getActualSeason()->getActualCalendar()));
    auto c = connect(ui->comboBox_calendar, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){
        qDebug()<<"[ui->comboBox_calendar->currentIndex()]: "<<ui->comboBox_calendar->currentIndex();
        qDebug()<<"count: "<<ui->comboBox_calendar->count();
        SeasonCalendar * cal = save->getActualSeason()->getCalendarsReference()[ui->comboBox_calendar->currentIndex()];
        for(auto & s : save->getSeasonsReference())
        {
            for(auto & cl : s.getCalendarsReference())
                if(cl->getName() == cal->getName()){
                    if(cl->getCompetitionsReference().first()->getPlayed() == true){
                        rangeComboBoxes->setCalendarFilter(cl->getName());
                        rangeComboBoxes->setupComboBoxes();
                        return;
                    }
                }
        }
    });
    toDisconnect.push_back(c);

    connect(ui->comboBox_calendar, qOverload<int>(&QComboBox::currentIndexChanged), rangeComboBoxes, &CompetitionsRangeComboBoxesWidget::setupComboBoxes);
    connect(ui->comboBox_calendar, qOverload<int>(&QComboBox::currentIndexChanged), this, &JumperStatsWindow::fillWindow);
}

void JumperStatsWindow::fillWindow()
{
    ui->label_jumperNameAndSurname->setText(jumper->getNameAndSurname());
    ui->label_jumperNameAndSurname->setToolTip(jumper->getNameAndSurname());
    ui->label_jumperFlag->setPixmap(CountryFlagsManager::getFlagPixmap(jumper->getCountryCode().toLower()).scaled(ui->label_jumperFlag->size()));
    ui->label_img->setPixmap(jumper->getImagePixmap().scaled(ui->label_img->size()));

    Hill * specificHill = nullptr;
    if(ui->comboBox_hillFilter->currentIndex() > 0)
        specificHill = save->getHillsReference()[ui->comboBox_hillFilter->currentIndex() - 1];

    QVector<CompetitionInfo *> competitions = CompetitionInfo::getCompetitionsByStartAndEnd(CompetitionInfo::mergeSeasonsCompetitions(rangeComboBoxes->getSeasonsList(), rangeComboBoxes->getCalendarFilter(), ui->checkBox_mergeCalendars->isChecked()),
                                                                                            rangeComboBoxes->getCompetition(1), rangeComboBoxes->getCompetition(2));
    singleResults = CompetitionSingleResult::getFilteredSingleResults(competitions, jumper,
        serieTypesCheckBoxes->getSerieTypes(), hillTypesCheckBoxes->getHillTypesSet(), classificationsCheckBoxes->getClassifications(), classificationsCheckBoxes->allUnchecked(), specificHill);

    fillJumperApperancesChart();
    fillJudgesPointsChart();
    fillJumperFormChart();
    fillTakeoffRatingChart();
    fillFlightRatingChart();
}

void JumperStatsWindow::fillJumperApperancesChart()
{
    jumperApperancesLineSeries->clear();
    delete jumperApperancesLineSeries;
    jumperApperancesChart->removeAllSeries();
    jumperApperancesLineSeries = new QLineSeries(this);
    jumperApperancesLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(jumperApperancesLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForJumperApperances);

    int bestPosition = 100000000;
    int worstPosition = 1;
    double averagePosition = 0;
    QVector<int> positions;
    int i=1;
    for(auto & result : singleResults)
    {
        if(result->getJumpsReference().first().getDSQ() && ui->checkBox_withDSQ->isChecked() == false)
            continue;
        jumperApperancesLineSeries->append(i, result->getPosition());

        if(result->getPosition() < bestPosition)
            bestPosition = result->getPosition();
        if(result->getPosition() > worstPosition)
            worstPosition = result->getPosition();
        averagePosition += result->getPosition();
        positions.push_back(result->getPosition());
        i++;
    }
    averagePosition /= singleResults.count();
    double standardDev = MyFunctions::standardDeviation(positions, averagePosition);
    standardDev = roundDoubleToTwoPlaces(standardDev);
    averagePosition = roundDoubleToTwoPlaces(averagePosition);

    jumperApperancesChart->addSeries(jumperApperancesLineSeries);
    jumperApperancesChart->createDefaultAxes();
    jumperApperancesChart->axes(Qt::Vertical).first()->setReverse(true);
    jumperApperancesChart->axes(Qt::Vertical).first()->setRange(1, worstPosition);
    jumperApperancesChart->axes(Qt::Horizontal).first()->setLabelsVisible(false);
    jumperApperancesChart->zoom(1);
    jumperApperancesChart->setAnimationOptions(QChart::SeriesAnimations);
    jumperApperancesChartView->setChart(jumperApperancesChart);
    jumperApperancesChartView->setRenderHint(QPainter::Antialiasing);

    ui->label_bestPos->setText(QString::number(bestPosition));
    ui->label_worstPos->setText(QString::number(worstPosition));
    ui->label_averagePos->setText(QString::number(averagePosition));
    ui->label_standardDeviation->setText(QString::number(standardDev));
}

void JumperStatsWindow::fillJudgesPointsChart()
{
    judgesPointsLineSeries->clear();
    judgesPointsChart->removeAllSeries();
    judgesPointsLineSeries = new QLineSeries(this);
    judgesPointsLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(judgesPointsLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForJudges);

    QVector<double> judgesPoints;
    double worst = 60;
    int i = 1;
    for(auto & singleResult : singleResults)
    {
        for(auto & jump : singleResult->getJumpsReference())
        {
            if(jump.getDSQ() == false && singleResult->getCompetition()->getRulesPointer()->getHasJudgesPoints() == true){
                judgesPoints.push_back(jump.getJudgesPoints());
                judgesPointsLineSeries->append(i, jump.getJudgesPoints());
                i++;
                if(jump.getJudgesPoints() < worst)
                    worst = jump.getJudgesPoints();
            }
        }
    }  
    judgesPointsChart->addSeries(judgesPointsLineSeries);
    judgesPointsChart->createDefaultAxes();
    judgesPointsChart->axes(Qt::Vertical).first()->setRange(worst, 60);
    judgesPointsChart->axes(Qt::Horizontal).first()->setLabelsVisible(false);
    judgesPointsChart->zoom(1);
    judgesPointsChart->setAnimationOptions(QChart::SeriesAnimations);
    judgesPointsChartView->setChart(judgesPointsChart);
    judgesPointsChartView->setRenderHint(QPainter::Antialiasing);
}

void JumperStatsWindow::fillJumperFormChart()
{
    jumperFormLineSeries->clear();
    jumperFormChart->removeAllSeries();
    jumperFormLineSeries = new QLineSeries(this);
    jumperFormLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(jumperFormLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForForm);

    QVector<double> forms;
    double worst;
    int i = 1;
    for(auto & singleResult : singleResults)
    {
        if(singleResult->getJumpsReference().count() > 0){
            forms.push_back(singleResult->getJumpsReference().first().getJumperForm());
            jumperFormLineSeries->append(i, singleResult->getJumpsReference().first().getJumperForm());
            i++;
        }
    }
    jumperFormChart->addSeries(jumperFormLineSeries);
    jumperFormChart->createDefaultAxes();
    jumperFormChart->axes(Qt::Horizontal).first()->setLabelsVisible(false);
    jumperFormChart->zoom(1);
    jumperFormChart->setAnimationOptions(QChart::SeriesAnimations);
    jumperFormChartView->setChart(jumperFormChart);
    jumperFormChartView->setRenderHint(QPainter::Antialiasing);
}

void JumperStatsWindow::fillTakeoffRatingChart()
{
    takeoffRatingLineSeries->clear();
    takeoffRatingChart->removeAllSeries();
    takeoffRatingLineSeries = new QLineSeries(this);
    takeoffRatingLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(takeoffRatingLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForTakeoffRating);
    QVector<double> ratings;
    int i = 1;
    for(auto & singleResult : singleResults)
    {
        for(auto & jump : singleResult->getJumpsReference())
        {
            if(jump.getDSQ() == false)
            {
                ratings.push_back(jump.getSimulationDataReference().getTakeoffRating());
                takeoffRatingLineSeries->append(i, ratings.last());
                i++;
            }
        }
    }
    takeoffRatingChart->addSeries(takeoffRatingLineSeries);
    takeoffRatingChart->createDefaultAxes();
    takeoffRatingChart->axes(Qt::Horizontal).first()->setLabelsVisible(false);
    takeoffRatingChart->zoom(1);
    takeoffRatingChart->setAnimationOptions(QChart::SeriesAnimations);
    takeoffRatingChartView->setChart(takeoffRatingChart);
    takeoffRatingChartView->setRenderHint(QPainter::Antialiasing);
}

void JumperStatsWindow::fillFlightRatingChart()
{
    flightRatingLineSeries->clear();
    flightRatingChart->removeAllSeries();
    flightRatingLineSeries = new QLineSeries(this);
    flightRatingLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(flightRatingLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForFlightRating);

    QVector<double> ratings;
    int i = 1;
    for(auto & singleResult : singleResults)
    {
        for(auto & jump : singleResult->getJumpsReference())
        {
            if(jump.getDSQ() == false)
            {
                ratings.push_back(jump.getSimulationDataReference().getFlightRating());
                flightRatingLineSeries->append(i, ratings.last());
                i++;
            }
        }
    }
    flightRatingChart->addSeries(flightRatingLineSeries);
    flightRatingChart->createDefaultAxes();
    flightRatingChart->axes(Qt::Horizontal).first()->setLabelsVisible(false);
    flightRatingChart->zoom(1);
    flightRatingChart->setAnimationOptions(QChart::SeriesAnimations);
    flightRatingChartView->setChart(flightRatingChart);
    flightRatingChartView->setRenderHint(QPainter::Antialiasing);
}

QCheckBox *JumperStatsWindow::getShowFormCheckBox()
{
    return ui->checkBox;
}

QCheckBox *JumperStatsWindow::getMergeCalendarsCheckBox()
{
    return ui->checkBox_mergeCalendars;
}

QComboBox *JumperStatsWindow::getCalendarComboBox()
{
    return ui->comboBox_calendar;
}

void JumperStatsWindow::updateChartCompetitionBySingleResult(const QPointF &point, bool state, int type)
{
    if(state)
    {
        if((point.x() + 0.3 > int(point.x()) && point.x() - 0.3 < int(point.x())) && (point.y() + 0.3 > point.y() && point.y() - 0.3 < point.y()))
        {
            QVector<CompetitionSingleResult *> tempSingleResults = singleResults;
            tempSingleResults.detach();
            for(auto & res : tempSingleResults)
            {
                if(res->getJumpsReference().first().getDSQ() == true && ui->checkBox_withDSQ->isChecked() == false)
                    MyFunctions::removeFromVector(tempSingleResults, res);
            }
            CompetitionSingleResult * result = tempSingleResults[int(point.x() - 1)];
            Hill * hill = result->getCompetition()->getHill();

            Season * season = nullptr;
            SeasonCalendar * calendar = nullptr;
            for(auto & s : *rangeComboBoxes->getSeasonsList())
            {
                for(auto & c : s.getCalendarsReference())
                {
                    if(c->getCompetitionsReference().contains(result->getCompetition())){
                        season = &s;
                        calendar = c;
                    }
                }
            }

            QString string = QString::number(season->getSeasonNumber()) + "/" + QString::number(calendar->getCompetitionsReference().indexOf(result->getCompetition()) + 1)
                             + ": " + hill->getName() + " HS" + QString::number(hill->getHSPoint());
            switch(result->getCompetition()->getSerieType())
            {
            case CompetitionInfo::Competition:
                string += tr(" (Konkurs)");
                break;
            case CompetitionInfo::Qualifications:
                string += tr(" (Kwalifikacje)");
                break;
            case CompetitionInfo::TrialRound:
                string += tr(" (Seria próbna)");
                break;
            case CompetitionInfo::Training:
            {
                int trainingIndex = calendar->getTrainingNumber(result->getCompetition());
                if(trainingIndex < 0)
                    trainingIndex = 1;
                string += tr(" (Trening ") + QString::number(trainingIndex) + ")";
                break;
            }
            }
            ui->label_chartCompetition->setText(string);
            if(type == Positions)
                ui->label_chartStat->setText(QString::number(result->getPosition()) + tr(" miejsce"));
            else if(type == Form)
                ui->label_chartStat->setText(QString::number(result->getJumpsReference().first().getJumperForm()));

            QPixmap pixmap = CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()).scaled(ui->label_hillFlag->size());
            ui->label_hillFlag->setPixmap(pixmap);
        }
    }
}

void JumperStatsWindow::updateChartCompetitionByJumpData(const QPointF &point, bool state, int type)
{
    if(state)
    {
        if((point.x() + 0.2 > int(point.x()) && point.x() - 0.2 < int(point.x())) && (point.y() + 0.2 > point.y() && point.y() - 0.2 < point.y()))
        {
            int jumpDataIndex = point.x();
            int remaining = jumpDataIndex;
            JumpData * jumpData = nullptr;
            CompetitionSingleResult * result = nullptr;
            for(auto & res : singleResults)
            {
                for(auto & jump : res->getJumpsReference())
                {
                    bool ok = res->getCompetition()->getRulesPointer()->getHasJudgesPoints();
                    if(type != JudgesPoints)
                        ok = true;
                    if(jump.getDSQ() == false && ok){
                        remaining--;
                        if(remaining == 0)
                        {
                            jumpData = &jump;
                            result = res;
                            break;
                        }
                    }
                }
            }
            int whichJump = MyFunctions::getIndexOfItemInVector(result->getJumpsReference(), jumpData) + 1;

            Hill * hill = result->getCompetition()->getHill();

            Season * season = nullptr;
            SeasonCalendar * calendar = nullptr;
            for(auto & s : *rangeComboBoxes->getSeasonsList())
            {
                for(auto & c : s.getCalendarsReference())
                {
                    if(c->getCompetitionsReference().contains(result->getCompetition())){
                        season = &s;
                        calendar = c;
                    }
                }
            }

            QString string = QString::number(season->getSeasonNumber()) + "/" + QString::number(calendar->getCompetitionsReference().indexOf(result->getCompetition()) + 1)
                             + ": " + hill->getName() + " HS" + QString::number(hill->getHSPoint());
            switch(result->getCompetition()->getSerieType())
            {
            case CompetitionInfo::Competition:
                string += tr(" (Konkurs)");
                break;
            case CompetitionInfo::Qualifications:
                string += tr(" (Kwalifikacje)");
                break;
            case CompetitionInfo::TrialRound:
                string += tr(" (Seria próbna)");
                    break;
            case CompetitionInfo::Training:
            {
                int trainingIndex = calendar->getTrainingNumber(result->getCompetition());
                if(trainingIndex < 0)
                    trainingIndex = 1;
                string += tr(" (Trening ") + QString::number(trainingIndex) + ")";
                break;
            }
            }
            string += tr(" (Skok ") + QString::number(whichJump) + ")";
            ui->label_chartCompetition->setText(string);
            if(type == JudgesPoints)
                ui->label_chartStat->setText(QString::number(jumpData->getJudgesPoints()) + tr(" pkt"));
            else if(type == TakeoffRating)
                ui->label_chartStat->setText(QString::number(jumpData->getSimulationDataReference().getTakeoffRating()));
            else if(type == FlightRating)
                ui->label_chartStat->setText(QString::number(jumpData->getSimulationDataReference().getFlightRating()));

            QPixmap pixmap = CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()).scaled(ui->label_hillFlag->size());
            ui->label_hillFlag->setPixmap(pixmap);
        }
    }
}

void JumperStatsWindow::updateChartCompetitionByJumpDataForJumperApperances(const QPointF &point, bool state)
{
    updateChartCompetitionBySingleResult(point, state, Positions);
}

void JumperStatsWindow::updateChartCompetitionByJumpDataForJudges(const QPointF &point, bool state)
{
    updateChartCompetitionByJumpData(point, state, JudgesPoints);
}

void JumperStatsWindow::updateChartCompetitionByJumpDataForForm(const QPointF &point, bool state)
{
    updateChartCompetitionBySingleResult(point, state, Form);
}

void JumperStatsWindow::updateChartCompetitionByJumpDataForTakeoffRating(const QPointF &point, bool state)
{
    updateChartCompetitionByJumpData(point, state, TakeoffRating);
}

void JumperStatsWindow::updateChartCompetitionByJumpDataForFlightRating(const QPointF &point, bool state)
{
    updateChartCompetitionByJumpData(point, state, FlightRating);
}

SimulationSave *JumperStatsWindow::getSave() const
{
    return save;
}

void JumperStatsWindow::setSave(SimulationSave *newSave)
{
    save = newSave;
}

HillTypesCheckBoxesWidget *JumperStatsWindow::getHillTypesCheckBoxes() const
{
    return hillTypesCheckBoxes;
}

void JumperStatsWindow::setHillTypesCheckBoxes(HillTypesCheckBoxesWidget *newHillTypesCheckBoxes)
{
    hillTypesCheckBoxes = newHillTypesCheckBoxes;
}

ClassificationsCheckBoxesWidget *JumperStatsWindow::getClassificationsCheckBoxes() const
{
    return classificationsCheckBoxes;
}

void JumperStatsWindow::setClassificationsCheckBoxes(ClassificationsCheckBoxesWidget *newClassificationsCheckBoxes)
{
    classificationsCheckBoxes = newClassificationsCheckBoxes;
}

void JumperStatsWindow::setupCharts()
{
    jumperApperancesChart = new QChart();
    jumperApperancesChart->legend()->hide();
    jumperApperancesChart->setTitle(tr("Występy zawodnika"));
    jumperApperancesChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    jumperApperancesChartView = new QChartView(this);
    jumperApperancesChartView->setRenderHint(QPainter::Antialiasing);
    jumperApperancesChartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->verticalLayout_jumperApperancesChart->addWidget(jumperApperancesChartView);
    jumperApperancesLineSeries = new QLineSeries(this);
    jumperApperancesLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(jumperApperancesLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForJumperApperances);

    judgesPointsChart = new QChart();
    judgesPointsChart->legend()->hide();
    judgesPointsChart->setTitle(tr("Punkty od sędziów"));
    judgesPointsChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    judgesPointsChartView = new QChartView(this);
    judgesPointsChartView->setRenderHint(QPainter::Antialiasing);
    judgesPointsChartView->setFixedHeight(450);
    judgesPointsChartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->verticalLayout_charts->addWidget(judgesPointsChartView);
    judgesPointsLineSeries = new QLineSeries(this);
    judgesPointsLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(judgesPointsLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForJudges);

    jumperFormChart = new QChart();
    jumperFormChart->legend()->hide();
    jumperFormChart->setTitle(tr("Forma zawodnika"));
    jumperFormChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    jumperFormChartView = new QChartView(this);
    jumperFormChartView->setRenderHint(QPainter::Antialiasing);
    jumperFormChartView->setFixedHeight(450);
    jumperFormChartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->verticalLayout_charts->addWidget(jumperFormChartView);
    jumperFormLineSeries = new QLineSeries(this);
    jumperFormLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(jumperFormLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForForm);

    takeoffRatingChart = new QChart();
    takeoffRatingChart->legend()->hide();
    takeoffRatingChart->setTitle(tr("Ocena wybicia"));
    takeoffRatingChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    takeoffRatingChartView = new QChartView(this);
    takeoffRatingChartView->setRenderHint(QPainter::Antialiasing);
    takeoffRatingChartView->setFixedHeight(450);
    takeoffRatingChartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->verticalLayout_charts->addWidget(takeoffRatingChartView);
    takeoffRatingLineSeries = new QLineSeries(this);
    takeoffRatingLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(takeoffRatingLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForTakeoffRating);

    flightRatingChart = new QChart();
    flightRatingChart->legend()->hide();
    flightRatingChart->setTitle(tr("Ocena lotu"));
    flightRatingChart->setTitleFont(QFont("Quicksand Medium", 15, 1, false));
    flightRatingChartView = new QChartView(this);
    flightRatingChartView->setRenderHint(QPainter::Antialiasing);
    flightRatingChartView->setFixedHeight(450);
    flightRatingChartView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->verticalLayout_charts->addWidget(flightRatingChartView);
    flightRatingLineSeries = new QLineSeries(this);
    flightRatingLineSeries->setPen(QPen(QBrush(QColor(qRgb(77, 179, 230))), 3.5));
    connect(flightRatingLineSeries, &QLineSeries::hovered, this, &JumperStatsWindow::updateChartCompetitionByJumpDataForFlightRating);
}

Jumper *JumperStatsWindow::getJumper() const
{
    return jumper;
}

void JumperStatsWindow::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

CompetitionsRangeComboBoxesWidget *JumperStatsWindow::getRangeComboBoxes() const
{
    return rangeComboBoxes;
}

void JumperStatsWindow::setRangeComboBoxes(CompetitionsRangeComboBoxesWidget *newRangeComboBoxes)
{
    rangeComboBoxes = newRangeComboBoxes;
}

void JumperStatsWindow::on_horizontalScrollBar_jumperStatsChartScrollBar_valueChanged(int value)
{
    jumperApperancesChart->scroll(-value * 0.7, 0);
}

void JumperStatsWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Unchecked)
    {
        jumperFormChartView->hide();
        takeoffRatingChartView->hide();
        flightRatingChartView->hide();
    }
    else if(arg1 == Qt::Checked)
    {
        jumperFormChartView->show();
        takeoffRatingChartView->show();
        flightRatingChartView->show();
    }
}

void JumperStatsWindow::saveJumperChartCsv(QString fileName, short chartType)
{
    QFile file(fileName);
    file.open(QFile::WriteOnly | QFile::Text);
    file.resize(0);
    QTextStream stream(&file);
    stream << tr("Numer") << ";" << tr("Pozycja") << ";" << tr("Konkurs") << ";" << "\n";
    QLineSeries * series = nullptr;
    switch(chartType)
    {
    case Positions:
        series = jumperApperancesLineSeries;
        break;
    case JudgesPoints:
        series = judgesPointsLineSeries;
        break;
    case Form:
        series = jumperFormLineSeries;
        break;
    case TakeoffRating:
        series = takeoffRatingLineSeries;
        break;
    case FlightRating:
        series = flightRatingLineSeries;
        break;
    }
    if(chartType == Positions){
        for(auto & point : series->points())
        {

        }
    }
    else
    {
        for(auto & point : series->points())
        {

        }
    }
}

void JumperStatsWindow::on_pushButton_csvExport_clicked()
{
    bool ok;
    QString folderName = QInputDialog::getText(this, tr("Nazwa folderu"), tr("Podaj nazwę jaką ma mieć folder z zapisanymi statystykami skoczka"), QLineEdit::Normal, "", &ok);
    if(ok == true)
    {
        QDir d("stats/jumpers-stats/" + folderName + "/charts");
        if(!d.exists())
            d.mkpath(".");
        QString text = "stats/jumpers-stats/" + folderName + "/charts/";
        saveJumperChartCsv(text + "apperances.csv", Positions);
        saveJumperChartCsv(text + "judges.csv", JudgesPoints);
        saveJumperChartCsv(text + "form.csv", Form);
        saveJumperChartCsv(text + "takeoff-rating.csv", TakeoffRating);
        saveJumperChartCsv(text + "flight-rating.csv", FlightRating);
    }
}

void JumperStatsWindow::on_checkBox_mergeCalendars_stateChanged(int arg1)
{
    rangeComboBoxes->setMergeCalendars(arg1);
    rangeComboBoxes->setupComboBoxes();
    fillWindow();
}


void JumperStatsWindow::on_checkBox_withDSQ_stateChanged(int arg1)
{
    fillWindow();
}
