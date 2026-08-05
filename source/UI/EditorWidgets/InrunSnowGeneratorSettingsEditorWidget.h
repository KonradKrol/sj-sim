#ifndef INRUNSNOWGENERATORSETTINGSEDITORWIDGET_H
#define INRUNSNOWGENERATORSETTINGSEDITORWIDGET_H

#include <QWidget>
#include "../../simulator/InrunSnowGenerator.h"

namespace Ui {
class InrunSnowGeneratorSettingsEditorWidget;
}

class InrunSnowGeneratorSettingsEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InrunSnowGeneratorSettingsEditorWidget(QWidget *parent = nullptr);
    ~InrunSnowGeneratorSettingsEditorWidget();

    void removeSubmitButton();
    void fillInputs();
    double getBase();
    double getDeviation();

signals:
    void submitted();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::InrunSnowGeneratorSettingsEditorWidget *ui;
    InrunSnowGenerator * generator;
public:
    InrunSnowGenerator *getGenerator() const;
    void setGenerator(InrunSnowGenerator *newGenerator);
};

#endif // INRUNSNOWGENERATORSETTINGSEDITORWIDGET_H
