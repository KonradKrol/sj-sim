#include "HelpWindow.h"
#include "ui_HelpWindow.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setOpenLinks(false);
    ui->textBrowser->setOpenExternalLinks(false);
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
