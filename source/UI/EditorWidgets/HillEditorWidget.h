#ifndef HILLEDITORWIDGET_H
#define HILLEDITORWIDGET_H

#include <QWidget>

class CharacteristicsEditor;
class Hill;

namespace Ui {
class HillEditorWidget;
}

class HillEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HillEditorWidget(QWidget *parent = nullptr);
    ~HillEditorWidget();

    void resetHillInputs();
    void fillHillInputs();

    void removeSubmitButton();

    Hill getHillFromWidgetInput() const;

    int getKPointFromInput();

signals:
    void submitted();
    void KPointInputChanged(double KPoint);

private slots:
    void on_checkBox_autoPointsForKPoint_stateChanged(int arg1);
    void on_checkBox_autoPointsForMeter_stateChanged(int arg1);
    void on_checkBox_autoBackWindPoints_stateChanged(int arg1);

    void on_lineEdit_countryCode_textChanged(const QString &arg1);

    void on_pushButton_submit_clicked();

private:
    Ui::HillEditorWidget *ui;

    CharacteristicsEditor * characteristicsEditor;
    Hill * hill; 

public:
    Hill *getHill() const;
    void setHill(Hill *newHill);
    CharacteristicsEditor *getCharacteristicsEditor() const;
    void setCharacteristicsEditor(CharacteristicsEditor *newCharacteristicsEditor);
};

#endif // HILLEDITORWIDGET_H
