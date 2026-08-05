#ifndef CLASSIFICATIONSCHECKBOXESWIDGET_H
#define CLASSIFICATIONSCHECKBOXESWIDGET_H

#include <QWidget>
#include <QVector>
#include "../../../../seasons/Classification.h"

namespace Ui {
class ClassificationsCheckBoxesWidget;
}

class ClassificationsCheckBoxesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClassificationsCheckBoxesWidget(QWidget *parent = nullptr);
    ~ClassificationsCheckBoxesWidget();

    void setupCheckBoxes();
    QVector<Classification *> getClassifications();
    bool allUnchecked();

    QVector<Classification *> *getClassificationsList() const;
    void setClassificationsList(QVector<Classification *> *newClassificationsList);

signals:
    void changed();

private:
    Ui::ClassificationsCheckBoxesWidget *ui;
    QVector<Classification *> * classificationsList;
    QVector<QCheckBox *> checkBoxes;
};

#endif // CLASSIFICATIONSCHECKBOXESWIDGET_H
