#include "HelpWindow.h"
#include "ui_HelpWindow.h"
#include "ResponsiveWindowUtils.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setOpenLinks(false);
    ui->textBrowser->setOpenExternalLinks(false);
    ResponsiveWindowUtils::manageWindowGeometry(this, "HelpWindow");
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
