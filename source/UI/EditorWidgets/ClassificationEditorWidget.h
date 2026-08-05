#ifndef CLASSIFICATIONEDITORWIDGET_H
#define CLASSIFICATIONEDITORWIDGET_H

#include <QWidget>
#include <QAction>
#include "../../seasons/Classification.h"
#include "TeamsEditing/Models/PointsForPlacesTableModel.h"

namespace Ui {
class ClassificationEditorWidget;
}

class ClassificationEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClassificationEditorWidget(Classification *classification = nullptr, QWidget *parent = nullptr);
    ~ClassificationEditorWidget();

    void resetInputs();
    void fillInputs();
    Classification * getClassificationFromInputs();

signals:
    void submitted();

private slots:
    void on_pushButton_submit_clicked();
    void addActionTriggered();
    void removeActionTriggered();

    void on_comboBox_altPreset_activated(int index);

private:
    Ui::ClassificationEditorWidget *ui;
    Classification * classification;
    QMap<int, double> pointsForPlaces;
    QMap<int, double> altPointsForPlaces;

    QAction * action_add;
    QAction * action_remove;

    PointsForPlacesTableModel * tableModel;
    PointsForPlacesTableModel * altTableModel;

public:
    Classification *getClassification() const;
    void setClassification(Classification *newClassification);
};

#endif // CLASSIFICATIONEDITORWIDGET_H
