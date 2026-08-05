#include "JumpWindInfoWidgetItem.h"
#include "ui_JumpWindInfoWidgetItem.h"

#include "../../simulator/wind-generation/WindsGenerator.h"
#include "../../utilities/functions.h"
#include <QPixmap>

JumpWindInfoWidgetItem::JumpWindInfoWidgetItem(Wind *wind, int sector, double KPoint, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JumpWindInfoWidgetItem),
    wind(wind),
    sector(sector),
    KPoint(KPoint)
{
    ui->setupUi(this);
}

JumpWindInfoWidgetItem::~JumpWindInfoWidgetItem()
{
    delete ui;
}

void JumpWindInfoWidgetItem::fillWidget()
{
    if(wind != nullptr){
        ui->label_windSector->setText(tr("Sektor nr ") + QString::number(getSector() + 1));
        QString text = "(" + QString::number(WindsGenerator::getRangeOfWindSector(getSector() + 1, getKPoint()).first);
        if(getSector() + 1 != WindsGenerator::calculateWindsCountByKPoint(getKPoint()))
            text += "-" + QString::number(WindsGenerator::getRangeOfWindSector(getSector() + 1, getKPoint()).second) + "m)";
        else text += " do końca)";
        ui->label_windRange->setText(text);
        ui->label_degrees->setText(QString::number(round(wind->getDirection())) + "°");
        double speed = wind->getStrength();
        ui->label_windSpeed->setText(QString().setNum(speed, 'g', 2) + " m/s");

        {
            if(speed < 0.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(13, 117, 27);");
            else if(speed < 1)
                ui->label_windSpeed->setStyleSheet("color: rgb(24, 161, 42);");
            else if(speed < 1.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(88, 194, 62);");
            else if(speed < 2)
                ui->label_windSpeed->setStyleSheet("color: rgb(142, 209, 102);");
            else if(speed < 2.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(162, 194, 87);");
            else if(speed < 3)
                ui->label_windSpeed->setStyleSheet("color: rgb(206, 224, 114);");
            else if(speed < 3.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(219, 206, 116);");
            else if(speed < 4)
                ui->label_windSpeed->setStyleSheet("color: rgb(227, 178, 109);");
            else if(speed < 4.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(232, 159, 95);");
            else if(speed < 5)
                ui->label_windSpeed->setStyleSheet("color: rgb(240, 137, 89);");
            else if(speed < 5.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(247, 112, 49);");
            else if(speed < 6)
                ui->label_windSpeed->setStyleSheet("color: rgb(245, 98, 69);");
            else if(speed < 6.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(250, 79, 45);");
            else if(speed < 7)
                ui->label_windSpeed->setStyleSheet("color: rgb(240, 55, 31);");
            else if(speed < 7.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(247, 37, 10);");
            else if(speed < 8)
                ui->label_windSpeed->setStyleSheet("color: rgb(186, 2, 2);");
            else if(speed < 8.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(150, 5, 5);");
            else if(speed < 9)
                ui->label_windSpeed->setStyleSheet("color: rgb(102, 6, 6);");
            else if(speed < 9.5)
                ui->label_windSpeed->setStyleSheet("color: rgb(56, 2, 2);");
            else if(speed < 10)
                ui->label_windSpeed->setStyleSheet("color: rgb(33, 2, 2);");
            else
                ui->label_windSpeed->setStyleSheet("color: rgb(0, 0, 0);");
        }


        QPixmap pixmap("://img/arrow-up1.png");
        QTransform transform;
        transform.rotate(wind->getDirection());
        pixmap = pixmap.transformed(transform).scaled(ui->label_windDirectionIcon->size());
        ui->label_windDirectionIcon->setPixmap(pixmap);
        /*switch(wind->getDirection())
        {
        case 0: //Null
            break;
        case 1:
            ui->label_windDirectionIcon->setPixmap(QPixmap("://img/arrow-down.png").scaled(ui->label_windDirectionIcon->size()));
            break;
        case 2:
            ui->label_windDirectionIcon->setPixmap(QPixmap("://img/back-side.png").scaled(ui->label_windDirectionIcon->size()));
            break;
        case 3:
            ui->label_windDirectionIcon->setPixmap(QPixmap("://img/horizontal.png").scaled(ui->label_windDirectionIcon->size()));
            break;
        case 4:
            ui->label_windDirectionIcon->setPixmap(QPixmap("://img/front-side.png").scaled(ui->label_windDirectionIcon->size()));
            break;
        case 5:
            ui->label_windDirectionIcon->setPixmap(QPixmap("://img/arrow-up1.png").scaled(ui->label_windDirectionIcon->size()));
            break;
        }*/
    }
    else{
        qDebug()<<"Wind is nullptr!";
    }
}

double JumpWindInfoWidgetItem::getKPoint() const
{
    return KPoint;
}

void JumpWindInfoWidgetItem::setKPoint(double newKPoint)
{
    KPoint = newKPoint;
}

int JumpWindInfoWidgetItem::getSector() const
{
    return sector;
}

void JumpWindInfoWidgetItem::setSector(int newSector)
{
    sector = newSector;
}

Wind *JumpWindInfoWidgetItem::getWind() const
{
    return wind;
}

void JumpWindInfoWidgetItem::setWind(Wind *newWind)
{
    wind = newWind;
}
