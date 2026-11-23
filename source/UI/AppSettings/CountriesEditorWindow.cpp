#include "CountriesEditorWindow.h"
#include "ui_CountriesEditorWindow.h"
#include "CountriesController.h"

CountriesEditorWindow::CountriesEditorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountriesEditorWindow),
    m_controller(new CountriesController(this))
{
    ui->setupUi(this);
    setObjectName("countriesEditor");
    setWindowTitle(tr("Kraje"));
    ui->label_title->setText(tr("Kraje"));
    // The Quick interface shows CountriesEditor.qml for this window.
    setProperty("countriesController",QVariant::fromValue<QObject *>(m_controller));
    ui->tableView->hide();
    ui->pushButton_submit->setText(tr("Zamknij"));
    connect(ui->pushButton_submit,&QPushButton::clicked,this,&QDialog::reject);
    connect(m_controller,&CountriesController::finished,this,[this](bool saved){saved?accept():reject();});
}

CountriesEditorWindow::~CountriesEditorWindow()
{
    delete ui;
}
