#ifndef SERIETYPESCOMBOBOXESWIDGET_H
#define SERIETYPESCOMBOBOXESWIDGET_H

#include <QWidget>
#include <QSet>

namespace Ui {
class SerieTypesComboBoxesWidget;
}

class SerieTypesComboBoxesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerieTypesComboBoxesWidget(QWidget *parent = nullptr);
    ~SerieTypesComboBoxesWidget();

    QSet<int> getSerieTypes();

signals:
    void changed();

private:
    Ui::SerieTypesComboBoxesWidget *ui;
};

#endif // SERIETYPESCOMBOBOXESWIDGET_H
