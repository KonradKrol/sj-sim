#ifndef SIMULATIONRATINGSWINDOW_H
#define SIMULATIONRATINGSWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QMetaObject>
#include "FilteringWidgets/ClassificationsCheckBoxesWidget.h"
#include "FilteringWidgets/CompetitionsRangeComboBoxesWidget.h"
#include "FilteringWidgets/SerieTypesComboBoxesWidget.h"
#include "FilteringWidgets/HillTypesCheckBoxesWidget.h"
#include "Models/SimulationRatingTableModel.h"
#include "Models/SimulationRecordsTableModel.h"
#include "Models/GeneralClassificationTableModel.h"
#include "../../../seasons/SimulationSave.h"

namespace Ui {
class SimulationRatingsWindow;
}

class SimulationRatingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationRatingsWindow(SimulationSave * save, QWidget *parent = nullptr);
    ~SimulationRatingsWindow();

    void fillWindow();
    void setupComboBox();
    void setupConnections();

    QCheckBox * getShowFormCheckBox();
    QCheckBox * getMergeCalendarsCheckBox();
    QComboBox * getCalendarComboBox();

private:
    Ui::SimulationRatingsWindow *ui;
    SimulationSave * save;

    ClassificationsCheckBoxesWidget * classificationsCheckBoxes;
    CompetitionsRangeComboBoxesWidget * rangeComboBoxes;
    SerieTypesComboBoxesWidget * serieTypesCheckBoxes;
    HillTypesCheckBoxesWidget * hillTypesCheckBoxes;
    QVector<Jumper *> filteredJumpers;

    QHash<Jumper *, QVector<CompetitionSingleResult *>> jumpersSingleResults;
    QHash<QString, QVector<CompetitionSingleResult *>> teamsSingleResults;

    //Averages models:
    SimulationRatingTableModel * averagePositionModel;
    SimulationRatingTableModel * averageJudgesModel;
    SimulationRatingTableModel * averageWindModel;
    SimulationRatingTableModel * averageTakeoffRatingModel;
    SimulationRatingTableModel * averageFlightRatingModel;
    SimulationRatingTableModel * averageDistanceModel;
    //Records models:
    SimulationRecordsTableModel * bestDistanceModel;
    SimulationRecordsTableModel * worstDistanceModel;
    SimulationRecordsTableModel * bestPointsModel;
    SimulationRecordsTableModel * worstPointsModel;
    SimulationRecordsTableModel * bestJudgesModel;
    SimulationRecordsTableModel * worstJudgesModel;
    SimulationRecordsTableModel * bestWindModel;
    SimulationRecordsTableModel * worstWindModel;
    SimulationRecordsTableModel * bestTakeoffModel;
    SimulationRecordsTableModel * worstTakeoffModel;
    SimulationRecordsTableModel * bestFlightModel;
    SimulationRecordsTableModel * worstFlightModel;
    //General classification models
    GeneralClassificationTableModel * generalClassificationModel;

    QVector<QMetaObject::Connection> toDisconnect;

public:
    ClassificationsCheckBoxesWidget *getClassificationsCheckBoxes() const;
    CompetitionsRangeComboBoxesWidget *getRangeComboBoxes() const;
    SerieTypesComboBoxesWidget *getSerieTypesCheckBoxes() const;
    HillTypesCheckBoxesWidget *getHillTypesCheckBoxes() const;
    GeneralClassificationTableModel *getGeneralClassificationModel() const;
    void setGeneralClassificationModel(GeneralClassificationTableModel *newGeneralClassificationModel);
    QVector<Jumper *> getFilteredJumpers() const;
    void setFilteredJumpers(const QVector<Jumper *> &newFilteredJumpers);
public:
    void saveRankingCsv(QString fileName, short rankingType);
    void saveRecordsCsv(QString fileName, short recordType, bool best);
    void saveGeneralClassificationCsv(QString fileName);
private:
    enum RankingType{
        AveragePosition,
        AverageJudges,
        AverageWind,
        AverageTakeoffRating,
        AverageTakeoffFlight,
        AverageDistance
    };
    enum RecordType{
        Distance,
        Points,
        Judges,
        Wind,
        TakeoffRating,
        FlightRating
    };

private slots:
    void on_pushButton_jumpersLists_clicked();
    void on_pushButton_csvExport_clicked();
    void on_checkBox_generalClassificationTeam_stateChanged(int arg1);
    void on_checkBox_generalClassificationCompPoints_stateChanged(int arg1);
    void on_checkBox_indResToTeam_stateChanged(int arg1);
    void on_checkBox_mergeCalendars_stateChanged(int arg1);
    void on_spinBox_minimalResults_editingFinished();
};

#endif // SIMULATIONRATINGSWINDOW_H
