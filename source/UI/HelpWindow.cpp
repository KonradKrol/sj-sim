#include "HelpWindow.h"
#include "ui_HelpWindow.h"
#include "ResponsiveWindowUtils.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);
    ui->label_title->setWordWrap(true);
    ui->label_title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    for(int index = 0; index < ui->tabWidget->count(); ++index)
        ui->tabWidget->setTabToolTip(index, ui->tabWidget->tabText(index));
    ui->textBrowser->setOpenLinks(false);
    ui->textBrowser->setOpenExternalLinks(false);
    ResponsiveWindowUtils::manageWindowGeometry(this, "HelpWindow");
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
