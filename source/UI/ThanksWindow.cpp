#include "ThanksWindow.h"
#include "ui_ThanksWindow.h"

ThanksWindow::ThanksWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ThanksWindow)
{
    ui->setupUi(this);
}

ThanksWindow::~ThanksWindow()
{
    delete ui;
}
