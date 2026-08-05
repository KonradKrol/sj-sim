#ifndef POINTSFORPLACESPRESETEDITORWIDGET_H
#define POINTSFORPLACESPRESETEDITORWIDGET_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QAction>
#include "TeamsEditing/Models/PointsForPlacesTableModel.h"
#include "../../global/PointsForPlacesPreset.h"

namespace Ui {
class PointsForPlacesPresetEditorWidget;
}

class PointsForPlacesPresetEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointsForPlacesPresetEditorWidget(QWidget *parent = nullptr);
    ~PointsForPlacesPresetEditorWidget();

    QString getNameFromInput();
    PointsForPlacesPreset constructPresetFromInputs();

    void fill();
    void reset();

signals:
    void submitted();

private:
    Ui::PointsForPlacesPresetEditorWidget *ui;
    QMap<int, double> pointsForPlaces;
    PointsForPlacesTableModel * model;
    QAction * action_add;
    QAction * action_remove;
    PointsForPlacesPreset * preset;

public:
    QMap<int, double> getPointsForPlaces() const;
    void setPointsForPlaces(const QMap<int, double> &newPointsForPlaces);
    PointsForPlacesPreset *getPreset() const;
    void setPreset(PointsForPlacesPreset *newPreset);

private slots:
    void on_pushButton_submit_clicked();
    void addActionTriggered();
    void removeActionTriggered();
};

#endif // POINTSFORPLACESPRESETEDITORWIDGET_H
