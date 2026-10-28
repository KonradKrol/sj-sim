#ifndef SINGLEJUMPSCONFIGWINDOW_H
#define SINGLEJUMPSCONFIGWINDOW_H

#include <QDialog>
#include "../../simulation-v2/SimulationCatalog.h"
#include "../../simulation-v2/ProfileDraft.h"
#include "../../simulation-v2/WeatherSetup.h"
#include "../../simulation-v2/QualityModel.h"
#include "../../simulator/wind-generation/WindGenerationSettings.h"

class QComboBox;
class QLabel;

class JumperEditorWidget;
class HillEditorWidget;
class WindsGeneratorSettingsEditorWidget;
class QResizeEvent;
class QSplitter;

namespace Ui {
class SingleJumpsConfigWindow;
}

class SingleJumpsConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SingleJumpsConfigWindow(QWidget *parent = nullptr,
        const SimulationV2::CatalogPaths &paths = SimulationV2::CatalogPaths::defaults());
    bool catalogReady() const {return ready;}
    Jumper configuredJumper() const {return athleteDraft->configuredJumper();}
    Hill configuredHill() const {return hillDraft->configuredHill();}
    SimulationProfileDraft *getAthleteDraft() const {return athleteDraft;}
    SimulationProfileDraft *getHillDraft() const {return hillDraft;}
    SimulationV2::EventKind simulationEvent() const;
    SimulationWeatherSetup *getWeatherSetup() const {return weatherSetup;}
    ~SingleJumpsConfigWindow();

    JumperEditorWidget *getJumperEditor() const;
    HillEditorWidget *getHillEditor() const;
    WindsGeneratorSettingsEditorWidget *getWindsGeneratorSettingsEditor() const;

signals:
    void goToResultsWindow();

private slots:
    void on_comboBox_existingHill_currentIndexChanged(int index);
    void on_comboBox_existingJumper_currentIndexChanged(int index);
    void on_pushButton_submit_clicked();

private:
    Ui::SingleJumpsConfigWindow *ui;
    QSplitter *contentSplitter;
    bool compactLayout;

    JumperEditorWidget * jumperEditor;
    HillEditorWidget * hillEditor;
    WindsGeneratorSettingsEditorWidget * windsGeneratorSettingsEditor;

    SimulationV2::SimulationCatalog catalog;
    SimulationV2::CatalogSnapshot snapshot;
    SimulationProfileDraft *athleteDraft, *hillDraft;
    SimulationWeatherSetup *weatherSetup;
    QComboBox *athletePack, *hillPack, *eventKind;
    QComboBox *windMeasurementChoice;
    QLabel *catalogStatus;
    bool ready=false, filling=false;
    QVector<WindGenerationSettings> defaultWinds;
    Jumper widgetAthlete;
    Hill widgetHill;
    void loadCatalog();
    void fillPack(DataPack::Kind kind);
    void updateReadiness();
    void updateGateSuggestion();
    void updateResponsiveLayout(int windowWidth);

protected:
    void resizeEvent(QResizeEvent *event) override;

public:
    int getJumpsCountFromInput();
    bool getChangeableWindFromInput();
    short getResultsFormatFromInput();
    short getWindAverageCalculatingType();
    int getWindMeasurementType() const;
    short getWindCompensationDistanceEffect();
    int getGateFromInput();
    int suggestedGate();
    int getDSQProbabilityFromInput();
    bool getHasJudgesFromInput();
    bool getHasWindCompensationFromInput();
    double importance();
};

#endif // SINGLEJUMPSCONFIGWINDOW_H
