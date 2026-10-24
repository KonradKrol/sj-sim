#ifndef NEWSIMULATIONSAVECONFIGURATIONWINDOW_H
#define NEWSIMULATIONSAVECONFIGURATIONWINDOW_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include "../../global/DataPack.h"
class QComboBox;

namespace Ui {
class NewSimulationSaveConfigurationWindow;
}

class NewSimulationSaveConfigurationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewSimulationSaveConfigurationWindow(QStringList otherNames, QWidget *parent = nullptr);
    ~NewSimulationSaveConfigurationWindow();

    QString getNameFromInput() const;
    int getSeasonNumberFromInput() const;
    bool getDatesEnabledFromInput() const;
    DataPack selectedPack(DataPack::Kind kind) const;

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::NewSimulationSaveConfigurationWindow *ui;

    QStringList otherNames;
    QComboBox *packChoices[3] = {nullptr,nullptr,nullptr};
    void updateSubmitState();
};

#endif // NEWSIMULATIONSAVECONFIGURATIONWINDOW_H
