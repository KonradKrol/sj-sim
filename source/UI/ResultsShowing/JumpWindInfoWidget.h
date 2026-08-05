#ifndef JUMPWINDINFOWIDGET_H
#define JUMPWINDINFOWIDGET_H

#include <QWidget>
#include <QVector>
#include "../../simulator/Wind.h"

namespace Ui {
class JumpWindInfoWidget;
}

class JumpWindInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JumpWindInfoWidget(QVector<Wind> * winds = nullptr, double KPoint = 0, QWidget *parent = nullptr);
    ~JumpWindInfoWidget();

    void fillItemsLayout();

private:
    Ui::JumpWindInfoWidget *ui;
    QVector<Wind> * winds;
    double KPoint;

public:
    double getKPoint() const;
    void setKPoint(double newKPoint);
    QVector<Wind> *getWinds() const;
    void setWinds(QVector<Wind> *newWinds);
};

#endif // JUMPWINDINFOWIDGET_H
