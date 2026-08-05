#include "WindsGeneratorSettingsEditorWidget.h"
#include "ui_WindsGeneratorSettingsEditorWidget.h"

#include "WindsGeneratorSettingsWidgetInputItem.h"
#include "../../simulator/wind-generation/WindGenerationSettings.h"

#include <QDebug>
#include <QAction>
#include <QDialog>
#include <QPushButton>
#include <QFont>

WindsGeneratorSettingsEditorWidget::WindsGeneratorSettingsEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindsGeneratorSettingsEditorWidget)
{
    KPoint = 0;
    settingsCount = removingSubmitButtons = 0;
    ui->setupUi(this);

    ui->toolBox->removeItem(0);
    ui->toolBox->removeItem(0);
    delete ui->widget;
    delete ui->page_2;

    QAction * action = new QAction(this);
    action->setShortcut(Qt::CTRL | Qt::Key_F);
    connect(action, &QAction::triggered, this, &WindsGeneratorSettingsEditorWidget::askForSettingsForFillAll);
    this->addAction(action);

    windGenerationSettings = nullptr;

    connect(this, &WindsGeneratorSettingsEditorWidget::KPointChanged, this, [this](){
        settingsCount = WindsGenerator::calculateWindsCountByKPoint(getKPoint());
    });
}

WindsGeneratorSettingsEditorWidget::~WindsGeneratorSettingsEditorWidget()
{
    delete ui;
}

void WindsGeneratorSettingsEditorWidget::fillWindGenerationSettingsByDefault()
{
    windGenerationSettings->fill(WindGenerationSettings(), getSettingsCount());
}

void WindsGeneratorSettingsEditorWidget::fillSettingsInputs()
{
    int count = ui->toolBox->count();
    for(int i=0; i<count; i++){
        delete ui->toolBox->widget(0);
    }

    int i=0;

    for(auto & settings : *windGenerationSettings)
    {
        QString sectorInfo = tr(" (Od ") + QString::number(WindsGenerator::getRangeOfWindSector(i + 1, KPoint).first) + tr(", do ");
        if(i + 1 == WindsGenerator::calculateWindsCountByKPoint(KPoint))
            sectorInfo += tr("końca");
        else sectorInfo += QString::number(WindsGenerator::getRangeOfWindSector(i + 1, KPoint).second) + "m";
        sectorInfo += ")";

        WindsGeneratorSettingsWidgetInputItem * item = new WindsGeneratorSettingsWidgetInputItem;
        item->setSettings(&settings);
        item->fillInputs();
        ui->toolBox->addItem(item, tr("Sektor wiatru nr ") + QString::number(i + 1) + sectorInfo);

        dynamic_cast<WindsGeneratorSettingsWidgetInputItem *>(ui->toolBox->widget(i))->removeSubmitButton();

        i++;
    }
}

void WindsGeneratorSettingsEditorWidget::fillSettingsInputsToExactWindsEditor()
{
    int count = ui->toolBox->count();
    for(int i=0; i<count; i++){
        delete ui->toolBox->widget(0);
    }

    int i=0;
    for(auto & settings : *windGenerationSettings)
    {
        QString sectorInfo = tr(" (Od ") + QString::number(WindsGenerator::getRangeOfWindSector(i + 1, KPoint).first) + tr(", do ");
        if(i + 1 == WindsGenerator::calculateWindsCountByKPoint(KPoint))
            sectorInfo += tr("końca");
        else sectorInfo += QString::number(WindsGenerator::getRangeOfWindSector(i + 1, KPoint).second) + "m";
        sectorInfo += ")";

        WindsGeneratorSettingsWidgetInputItem * item = new WindsGeneratorSettingsWidgetInputItem;
        item->setSettings(&settings);
        item->fillInputsToExactWindEditor();
        ui->toolBox->addItem(item, tr("Sektor wiatru nr ") + QString::number(i + 1) + sectorInfo);

        dynamic_cast<WindsGeneratorSettingsWidgetInputItem *>(ui->toolBox->widget(i))->removeSubmitButton();

        i++;
    }
}

void WindsGeneratorSettingsEditorWidget::askForSettingsForFillAll()
{
    if(getSettingsCount() > 0)
    {
        QDialog * dialog = new QDialog;
        dialog->setModal(true);
        dialog->setWindowFlags(Qt::Window);
        dialog->setWindowTitle(tr("Wypełnij podanym ustawieniem"));
        dialog->setStyleSheet("background-color: rgb(225, 225, 225);");
        dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        dialog->setFixedSize(dialog->size());
        dialog->setLayout(new QVBoxLayout(dialog));
        WindsGeneratorSettingsWidgetInputItem * item = new WindsGeneratorSettingsWidgetInputItem;
        dialog->layout()->addWidget(item);

        connect(item, &WindsGeneratorSettingsWidgetInputItem::submitted, dialog, &QDialog::accept);
        if(dialog->exec() == QDialog::Accepted)
        {
            int widgetsCount = ui->toolBox->count();
            for(int i=0; i<widgetsCount; i++){
                delete ui->toolBox->widget(0);
            }

            WindGenerationSettings settings = item->getWindGenerationSettingsFromInputs();
            for(auto & sett : *windGenerationSettings)
            {
                sett = settings;
            }
            for(int i=0; i<getSettingsCount(); i++)
            {
                QString sectorInfo = tr(" (Od ") + QString::number(WindsGenerator::getRangeOfWindSector(i + 1, KPoint).first) + tr(", do ");
                if(i + 1 == WindsGenerator::calculateWindsCountByKPoint(KPoint))
                    sectorInfo += tr("końca");
                else sectorInfo += QString::number(WindsGenerator::getRangeOfWindSector(i + 1, KPoint).second) + "m";
                sectorInfo += ")";

                WindsGeneratorSettingsWidgetInputItem * item = new WindsGeneratorSettingsWidgetInputItem;
                item->removeSubmitButton();
                ui->toolBox->addItem(item, tr("Sektor wiatru nr ") + QString::number(i + 1) + sectorInfo);
                item->setSettings(&settings);
                item->fillInputs();
            }
            delete item;
        }
    }
}

void WindsGeneratorSettingsEditorWidget::removeSubmitButton()
{
    disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &WindsGeneratorSettingsEditorWidget::on_pushButton_submit_clicked);
    delete ui->pushButton_submit;
}

QVector<WindGenerationSettings> WindsGeneratorSettingsEditorWidget::getWindsGenerationSettingsFromInputs()
{
    QVector<WindGenerationSettings> vector;

    for(int i=0; i < settingsCount; i++){
        WindsGeneratorSettingsWidgetInputItem * item = dynamic_cast<WindsGeneratorSettingsWidgetInputItem *>(ui->toolBox->widget(i));
        WindGenerationSettings settings;
        settings.setBaseWindStrength(item->getBaseWindStrengthFromInput());
        settings.setWindStrengthInstability(item->getWindStrengthChangeFromInput());
        settings.setBaseDirection(item->getBaseWindDirectionFromInput());
        settings.setWindDirectionInstability(item->getWindDirectionChangeFromInput());
        settings.setCharacteristics(item->getCharacteristicsEditor()->getCharacteristics());
        vector.push_back(settings);
    }
    return vector;
}

QVector<Wind> WindsGeneratorSettingsEditorWidget::getExactWindsFromInputs()
{
    QVector<Wind> vector;

    for(int i=0; i < settingsCount; i++){
        WindsGeneratorSettingsWidgetInputItem * item = dynamic_cast<WindsGeneratorSettingsWidgetInputItem *>(ui->toolBox->widget(i));
        Wind wind;
        wind.setDirection(item->getBaseWindDirectionFromInput());
        wind.setStrength(item->getBaseWindStrengthFromInput());
        vector.push_back(wind);
    }
    return vector;
}

double WindsGeneratorSettingsEditorWidget::getKPoint() const
{
    return KPoint;
}

void WindsGeneratorSettingsEditorWidget::setKPoint(double newKPoint)
{
    double old = KPoint;
    KPoint = newKPoint;

    if(old != newKPoint)
        emit KPointChanged();
}

QVector<WindGenerationSettings> *WindsGeneratorSettingsEditorWidget::getWindGenerationSettings() const
{
    return windGenerationSettings;
}

void WindsGeneratorSettingsEditorWidget::setWindGenerationSettings(QVector<WindGenerationSettings> *newWindGenerationSettings)
{
    windGenerationSettings = newWindGenerationSettings;
}

int WindsGeneratorSettingsEditorWidget::getSettingsCount() const
{
    return settingsCount;
}

void WindsGeneratorSettingsEditorWidget::setSettingsCount(int newSettingsCount)
{
    settingsCount = newSettingsCount;
}

bool WindsGeneratorSettingsEditorWidget::getRemovingSubmitButtons() const
{
    return removingSubmitButtons;
}

void WindsGeneratorSettingsEditorWidget::setRemovingSubmitButtons(bool newRemovingSubmitButtons)
{
    removingSubmitButtons = newRemovingSubmitButtons;
}

void WindsGeneratorSettingsEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

