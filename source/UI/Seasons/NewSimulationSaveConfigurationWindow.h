#ifndef NEWSIMULATIONSAVECONFIGURATIONWINDOW_H
#define NEWSIMULATIONSAVECONFIGURATIONWINDOW_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include "../../global/DataPack.h"
#include "../../simulation-v2/SimulationCatalog.h"
#include <memory>
class QComboBox;
class QLabel;
class SimulationSave;

namespace Ui {
class NewSimulationSaveConfigurationWindow;
}

class NewSimulationSaveConfigurationWindow : public QDialog
{
    Q_OBJECT

public:
    enum class DataSource { Canonical, LegacyCompatibility };
    explicit NewSimulationSaveConfigurationWindow(QStringList otherNames, QWidget *parent = nullptr,
        DataSource source=DataSource::Canonical,
        const SimulationV2::CatalogPaths &paths=SimulationV2::CatalogPaths::defaults());
    ~NewSimulationSaveConfigurationWindow();

    QString getNameFromInput() const;
    int getSeasonNumberFromInput() const;
    bool getDatesEnabledFromInput() const;
    DataPack selectedPack(DataPack::Kind kind) const;
    bool catalogReady() const { return ready; }
    int engineVersion() const { return source==DataSource::Canonical?2:1; }
    std::unique_ptr<SimulationSave> createSave() const;

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::NewSimulationSaveConfigurationWindow *ui;

    QStringList otherNames;
    DataSource source;
    SimulationV2::SimulationCatalog catalog;
    SimulationV2::CatalogSnapshot snapshot;
    bool ready=false;
    QComboBox *packChoices[3] = {nullptr,nullptr,nullptr};
    QLabel *catalogStatus=nullptr;
    void loadCatalog();
    void fillPackChoices();
    void updateCatalogStatus();
    void updateSubmitState();
};

#endif // NEWSIMULATIONSAVECONFIGURATIONWINDOW_H
