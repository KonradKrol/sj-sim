#ifndef RANDOMWINDCONFIGWINDOW_H
#define RANDOMWINDCONFIGWINDOW_H

#include <QDialog>

namespace Ui {
class RandomWindConfigWindow;
}

class RandomWindConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RandomWindConfigWindow(QWidget *parent = nullptr);
    ~RandomWindConfigWindow();

    double getDirection();
    double getLotability();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::RandomWindConfigWindow *ui;
};

#endif // RANDOMWINDCONFIGWINDOW_H
