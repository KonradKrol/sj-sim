#include "PointsForPlacesPresetEditorWidget.h"
#include "ui_PointsForPlacesPresetEditorWidget.h"
#include <QPushButton>
#include <QHBoxLayout>

PointsForPlacesPresetEditorWidget::PointsForPlacesPresetEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PointsForPlacesPresetEditorWidget)
{
    ui->setupUi(this);

    model = new PointsForPlacesTableModel(this);
    model->setPointsForPlaces(&pointsForPlaces);
    ui->tableView_pointsForPlaces->setModel(model);
    ui->tableView_pointsForPlaces->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->pushButton_submit->hide();
    auto *actions=new QHBoxLayout;
    const QStringList names{"classificationAddPlace","classificationRemovePlace","classificationClearPlaces"};
    const QStringList labels{tr("Dodaj miejsce"),tr("Usuń ostatnie miejsce"),tr("Wyczyść")};
    for(int i=0;i<3;++i){auto *button=new QPushButton(labels[i],this);button->setObjectName(names[i]);actions->addWidget(button);connect(button,&QPushButton::clicked,this,[this,i]{if(i==0)addActionTriggered();else if(i==1)removeActionTriggered();else model->removeRows(0,model->rowCount());});}
    ui->verticalLayout_2->insertLayout(1,actions);
    connect(model,&QAbstractItemModel::dataChanged,this,&PointsForPlacesPresetEditorWidget::configurationChanged);
    connect(model,&QAbstractItemModel::rowsInserted,this,&PointsForPlacesPresetEditorWidget::configurationChanged);
    connect(model,&QAbstractItemModel::rowsRemoved,this,&PointsForPlacesPresetEditorWidget::configurationChanged);
    connect(ui->lineEdit_name,&QLineEdit::editingFinished,this,&PointsForPlacesPresetEditorWidget::configurationChanged);

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

