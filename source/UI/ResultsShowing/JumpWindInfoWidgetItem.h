#ifndef JUMPWINDINFOWIDGETITEM_H
#define JUMPWINDINFOWIDGETITEM_H

#include <QWidget>
#include "../../simulator/Wind.h"

namespace Ui {
class JumpWindInfoWidgetItem;
}

class JumpWindInfoWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit JumpWindInfoWidgetItem(Wind * wind = nullptr, int sector = 0, double KPoint = 0, QWidget *parent = nullptr);
    ~JumpWindInfoWidgetItem();

    void fillWidget();

private:
    Ui::JumpWindInfoWidgetItem *ui;
    Wind * wind;
    int sector;
    double KPoint;

public:
    Wind *getWind() const;
    void setWind(Wind *newWind);
    int getSector() const;
    void setSector(int newSector);
    double getKPoint() const;
    void setKPoint(double newKPoint);
};

#endif // JUMPWINDINFOWIDGETITEM_H
