#ifndef THANKSWINDOW_H
#define THANKSWINDOW_H

#include <QDialog>

namespace Ui {
class ThanksWindow;
}

class ThanksWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ThanksWindow(QWidget *parent = nullptr);
    ~ThanksWindow();

private:
    Ui::ThanksWindow *ui;
};

#endif // THANKSWINDOW_H
