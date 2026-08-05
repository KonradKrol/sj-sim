#ifndef WINDSGENERATORSETTINGSWIDGETINPUTITEM_H
#define WINDSGENERATORSETTINGSWIDGETINPUTITEM_H

#include <QWidget>

#include "CharacteristicsEditor.h"
#include "../../simulator/wind-generation/WindGenerationSettings.h"

namespace Ui {
class WindsGeneratorSettingsWidgetInputItem;
}

class WindsGeneratorSettingsWidgetInputItem : public QWidget
{
    Q_OBJECT

public:
    explicit WindsGeneratorSettingsWidgetInputItem(QWidget *parent = nullptr);
    ~WindsGeneratorSettingsWidgetInputItem();

    void removeSubmitButton();

    double getBaseWindStrengthFromInput();
    double getWindStrengthChangeFromInput();
    short getBaseWindDirectionFromInput();
    double getWindDirectionChangeFromInput();
    WindGenerationSettings getWindGenerationSettingsFromInputs();

    void fillInputs();
    void fillInputsToExactWindEditor();
signals:
    void submitted();

private slots:
    void on_pushButton_submit_clicked();

    void on_doubleSpinBox_baseWindDirection_valueChanged(double arg1);

private:
    Ui::WindsGeneratorSettingsWidgetInputItem *ui;

    CharacteristicsEditor * characteristicsEditor;

    WindGenerationSettings * settings;

public:
    WindGenerationSettings *getSettings() const;
    void setSettings(WindGenerationSettings *newSettings);
    CharacteristicsEditor *getCharacteristicsEditor() const;
};

#endif // WINDSGENERATORSETTINGSWIDGETINPUTITEM_H
