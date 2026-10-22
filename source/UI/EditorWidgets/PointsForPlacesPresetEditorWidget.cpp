#include "PointsForPlacesPresetEditorWidget.h"
#include "ui_PointsForPlacesPresetEditorWidget.h"

PointsForPlacesPresetEditorWidget::PointsForPlacesPresetEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PointsForPlacesPresetEditorWidget)
{
    ui->setupUi(this);

    model = new PointsForPlacesTableModel(this);
    model->setPointsForPlaces(&pointsForPlaces);
    ui->tableView_pointsForPlaces->setModel(model);
    ui->tableView_pointsForPlaces->setSelectionMode(QAbstractItemView::SingleSelection);

    action_add = new QAction(this);
    action_add->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_A);
    this->addAction(action_add);
    connect(action_add, &QAction::triggered, this, &PointsForPlacesPresetEditorWidget::addActionTriggered);

    action_remove = new QAction(this);
    action_remove->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_D);
    this->addAction(action_remove);
    connect(action_remove, &QAction::triggered, this, &PointsForPlacesPresetEditorWidget::removeActionTriggered);

}

PointsForPlacesPresetEditorWidget::~PointsForPlacesPresetEditorWidget()
{
    delete ui;
}

QString PointsForPlacesPresetEditorWidget::getNameFromInput()
{
    return ui->lineEdit_name->text();
}

PointsForPlacesPreset PointsForPlacesPresetEditorWidget::constructPresetFromInputs()
{
    PointsForPlacesPreset p;
    p.setName(ui->lineEdit_name->text());
    p.setPointsForPlaces(pointsForPlaces);
    return p;
}

void PointsForPlacesPresetEditorWidget::fill()
{
    ui->lineEdit_name->setText(preset->getName());
    model->replacePoints(pointsForPlaces);
    emit model->dataChanged(model->index(0, 0), model->index(model->rowCount() - 1, model->columnCount() - 1));
}

void PointsForPlacesPresetEditorWidget::reset()
{
    ui->lineEdit_name->clear();
    model->removeRows(0, model->rowCount());
}

PointsForPlacesPreset *PointsForPlacesPresetEditorWidget::getPreset() const
{
    return preset;
}

void PointsForPlacesPresetEditorWidget::setPreset(PointsForPlacesPreset *newPreset)
{
    preset = newPreset;
    model->replacePoints(preset?preset->getPointsForPlaces():QMap<int,double>());
}

QMap<int, double> PointsForPlacesPresetEditorWidget::getPointsForPlaces() const
{
    return pointsForPlaces;
}

void PointsForPlacesPresetEditorWidget::setPointsForPlaces(const QMap<int, double> &newPointsForPlaces)
{
    model->replacePoints(newPointsForPlaces);
}

void PointsForPlacesPresetEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

void PointsForPlacesPresetEditorWidget::addActionTriggered()
{
    int count = pointsForPlaces.count();
    model->insertRow(count);
    emit model->dataChanged(model->index(count, 0), model->index(count, 1));
}

void PointsForPlacesPresetEditorWidget::removeActionTriggered()
{
    int count = pointsForPlaces.count();
    model->removeRow(count - 1);
}

