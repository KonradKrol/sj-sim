#ifndef HILLTYPESCHECKBOXESWIDGET_H
#define HILLTYPESCHECKBOXESWIDGET_H

#include <QWidget>
#include <QSet>
#include "../../../../simulator/Hill.h"

namespace Ui {
class HillTypesCheckBoxesWidget;
}

class HillTypesCheckBoxesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HillTypesCheckBoxesWidget(QWidget *parent = nullptr);
    ~HillTypesCheckBoxesWidget();

    QSet<int> getHillTypesSet();

signals:
    void changed();

private:
    Ui::HillTypesCheckBoxesWidget *ui;
};

#endif // HILLTYPESCHECKBOXESWIDGET_H
