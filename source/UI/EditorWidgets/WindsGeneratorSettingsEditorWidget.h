#ifndef WINDSGENERATORSETTINGSEDITORWIDGET_H
#define WINDSGENERATORSETTINGSEDITORWIDGET_H

#include <QWidget>
#include <QVector>
#include "../../simulator/Wind.h"
#include "../../simulator/wind-generation/WindGenerationSettings.h"
#include "../../simulator/wind-generation/WindsGenerator.h"

namespace Ui {
class WindsGeneratorSettingsEditorWidget;
}

class WindsGeneratorSettingsEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WindsGeneratorSettingsEditorWidget(QWidget *parent = nullptr);
    ~WindsGeneratorSettingsEditorWidget();

    void fillWindGenerationSettingsByDefault();
    void fillSettingsInputs();
    void askForSettingsForFillAll();
    void removeSubmitButton();

    void fillSettingsInputsToExactWindsEditor();
    QVector<Wind> getExactWindsFromInputs();

    QVector<WindGenerationSettings> getWindsGenerationSettingsFromInputs();

signals:
    void KPointChanged();
    void submitted();

private:
    Ui::WindsGeneratorSettingsEditorWidget *ui;

    QVector<WindGenerationSettings> * windGenerationSettings;
    int settingsCount;
    bool removingSubmitButtons;

    double KPoint;

public:
    QVector<WindGenerationSettings> *getWindGenerationSettings() const;
    void setWindGenerationSettings(QVector<WindGenerationSettings> *newWindGenerationSettings);
    int getSettingsCount() const;
    void setSettingsCount(int newSettingsCount);
    bool getRemovingSubmitButtons() const;
    void setRemovingSubmitButtons(bool newRemovingSubmitButtons);\
    double getKPoint() const;
    void setKPoint(double newKPoint);
private slots:
    void on_pushButton_submit_clicked();
};

#endif // WINDSGENERATORSETTINGSEDITORWIDGET_H
