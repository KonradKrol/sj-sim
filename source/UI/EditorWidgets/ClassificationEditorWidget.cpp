#include "ClassificationEditorWidget.h"
#include "ui_ClassificationEditorWidget.h"

#include "../../competitions/CompetitionRules.h"
#include "../../global/GlobalDatabase.h"

ClassificationEditorWidget::ClassificationEditorWidget(Classification * classification, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassificationEditorWidget)
{
    ui->setupUi(this);

    if(classification != nullptr){
        pointsForPlaces = this->classification->getPointsForPlacesReference();
        altPointsForPlaces = this->classification->getAltPointsForPlacesReference();
    }

    tableModel = new PointsForPlacesTableModel(this);
    tableModel->setPointsForPlaces(&pointsForPlaces);
    ui->tableView_pointsForPlaces->setModel(tableModel);
    ui->tableView_pointsForPlaces->setSelectionMode(QAbstractItemView::SingleSelection);

    altTableModel = new PointsForPlacesTableModel(this);
    altTableModel->setPointsForPlaces(&altPointsForPlaces);
    ui->tableView_altPointsForPlaces->setModel(altTableModel);
    ui->tableView_altPointsForPlaces->setSelectionMode(QAbstractItemView::SingleSelection);

    action_add = new QAction(this);
    action_add->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_A);
    this->addAction(action_add);
    connect(action_add, &QAction::triggered, this, &ClassificationEditorWidget::addActionTriggered);

    action_remove = new QAction(this);
    action_remove->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_D);
    this->addAction(action_remove);
    connect(action_remove, &QAction::triggered, this, &ClassificationEditorWidget::removeActionTriggered);

    connect(ui->comboBox_points, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){
        if(ui->comboBox_points->currentIndex() == Classification::PointsForPlaces)
        {
            ui->label_shortcuts->show();

            ui->tableView_pointsForPlaces->show();
            ui->label_preset->show();
            ui->comboBox_presets->show();

            ui->tableView_altPointsForPlaces->show();
            ui->label_altPreset->show();
            ui->comboBox_altPreset->show();
        }
        else if(ui->comboBox_points->currentIndex() == Classification::CompetitionPoints)
        {
            ui->label_shortcuts->hide();

            ui->tableView_pointsForPlaces->hide();
            ui->label_preset->hide();
            ui->comboBox_presets->hide();

            ui->tableView_altPointsForPlaces->hide();
            ui->label_altPreset->hide();
            ui->comboBox_altPreset->hide();
        }
    });

    int i=1;
    ui->comboBox_presets->addItem(tr("BRAK"));
    ui->comboBox_altPreset->addItem(tr("BRAK"));
    for(auto & preset : GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets()){
        ui->comboBox_presets->addItem(QString::number(i) + ". " + preset.getName());
        ui->comboBox_altPreset->addItem(QString::number(i) + ". " + preset.getName());
        i++;
    }
    connect(ui->comboBox_presets, qOverload<int>(&QComboBox::activated), this, [this](){
        if(ui->comboBox_presets->currentIndex() == 0)
        {
            tableModel->removeRows(0, tableModel->rowCount());
            pointsForPlaces.clear();
        }
        else
        {
            tableModel->removeRows(0, tableModel->rowCount());
            pointsForPlaces = GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets()[ui->comboBox_presets->currentIndex() - 1].getPointsForPlacesReference();
            tableModel->insertRows(0, pointsForPlaces.count());
            emit tableModel->dataChanged(tableModel->index(0, 0), tableModel->index(tableModel->rowCount() - 1, tableModel->columnCount() - 1));
        }
    });
}

ClassificationEditorWidget::~ClassificationEditorWidget()
{
    delete ui;
}

void ClassificationEditorWidget::resetInputs()
{
    ui->lineEdit_name->clear();
    ui->comboBox_type->setCurrentIndex(0);
    ui->comboBox_points->setCurrentIndex(0);
    tableModel->removeRows(0, tableModel->rowCount());
    altTableModel->removeRows(0, altTableModel->rowCount());
    pointsForPlaces.clear();
    altPointsForPlaces.clear();
}

void ClassificationEditorWidget::fillInputs()
{
    ui->lineEdit_name->setText(classification->getName());
    ui->comboBox_type->setCurrentIndex(classification->getClassificationType());
    ui->comboBox_points->setCurrentIndex(classification->getPunctationType());

    pointsForPlaces = classification->getPointsForPlaces();
    altPointsForPlaces = classification->getAltPointsForPlaces();
    tableModel->insertRows(0, pointsForPlaces.count());
    altTableModel->insertRows(0, altPointsForPlaces.count());
    emit tableModel->dataChanged(tableModel->index(0, 0), tableModel->index(tableModel->rowCount() - 1, tableModel->columnCount() - 1));
    emit altTableModel->dataChanged(altTableModel->index(0, 0), altTableModel->index(altTableModel->rowCount() - 1, altTableModel->columnCount() - 1));
}

Classification * ClassificationEditorWidget::getClassificationFromInputs()
{
    Classification * classification = new Classification();
    classification->setName(ui->lineEdit_name->text());
    classification->setClassificationType(ui->comboBox_type->currentIndex());
    classification->setPunctationType(ui->comboBox_points->currentIndex());
    classification->setPointsForPlaces(pointsForPlaces);
    classification->setAltPointsForPlaces(altPointsForPlaces);
    return classification;
}

void ClassificationEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

void ClassificationEditorWidget::addActionTriggered()
{
    int count = pointsForPlaces.count();
    pointsForPlaces.insert(count + 1, 0);
    tableModel->insertRow(count);
    emit tableModel->dataChanged(tableModel->index(count, 0), tableModel->index(count, 1));
}

void ClassificationEditorWidget::removeActionTriggered()
{
    int count = pointsForPlaces.count();
    pointsForPlaces.remove(count);
    tableModel->removeRow(count - 1);
}

Classification *ClassificationEditorWidget::getClassification() const
{
    return classification;
}

void ClassificationEditorWidget::setClassification(Classification *newClassification)
{
    classification = newClassification;
    pointsForPlaces = classification->getPointsForPlacesReference();
}

void ClassificationEditorWidget::on_comboBox_altPreset_activated(int index)
{
    if(ui->comboBox_altPreset->currentIndex() == 0)
    {
        altTableModel->removeRows(0, altTableModel->rowCount());
        altPointsForPlaces.clear();
    }
    else
    {
        altTableModel->removeRows(0, altTableModel->rowCount());
        altPointsForPlaces = GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets()[ui->comboBox_altPreset->currentIndex() - 1].getPointsForPlacesReference();
        altTableModel->insertRows(0, altPointsForPlaces.count());
        emit altTableModel->dataChanged(altTableModel->index(0, 0), altTableModel->index(altTableModel->rowCount() - 1, altTableModel->columnCount() - 1));
    }
}
