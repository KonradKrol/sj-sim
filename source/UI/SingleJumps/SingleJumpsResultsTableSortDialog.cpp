#include "SingleJumpsResultsTableSortDialog.h"
#include "ui_SingleJumpsResultsTableSortDialog.h"

SingleJumpsResultsTableSortDialog::SingleJumpsResultsTableSortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingleJumpsResultsTableSortDialog)
{
    ui->setupUi(this);
}

SingleJumpsResultsTableSortDialog::~SingleJumpsResultsTableSortDialog()
{
    delete ui;
}

short SingleJumpsResultsTableSortDialog::sortBy()
{
    return ui->comboBox_sortBy->currentIndex();
}

short SingleJumpsResultsTableSortDialog::sortType()
{
    return ui->comboBox_sortType->currentIndex();
}

void SingleJumpsResultsTableSortDialog::on_pushButton_submit_clicked()
{
    accept();
}

