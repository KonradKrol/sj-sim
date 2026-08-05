#include "ClassificationResultsTableView.h"
#include "ui_ClassificationResultsTableView.h"

ClassificationResultsTableView::ClassificationResultsTableView(bool allowShowDetails, Classification * classification, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassificationResultsTableView),
    allowShowDetails(allowShowDetails),
    classification(classification)
{
    ui->setupUi(this);

    tableModel = new ClassificationResultsTableModel(nullptr, this);

    connect(ui->tableView, &QTableView::doubleClicked, this, [this](const QModelIndex & index){
        if(this->allowShowDetails == true)
        {
           // Pokaż szczegóły wyniku w klasyfikacji po double click-u.
        }
    });
}

ClassificationResultsTableView::~ClassificationResultsTableView()
{
    delete ui;
}

void ClassificationResultsTableView::fillTable()
{
    tableModel->setClassification(classification);
    //tableModel->insertRows(0, classification->getResultsReference().count());
    ui->tableView->setModel(nullptr);
    ui->tableView->setModel(tableModel);
    ui->tableView->resizeColumnsToContents();
}

SimulationSave *ClassificationResultsTableView::getSave() const
{
    return save;
}

void ClassificationResultsTableView::setSave(SimulationSave *newSave)
{
    save = newSave;
}

bool ClassificationResultsTableView::getAllowShowDetails() const
{
    return allowShowDetails;
}

void ClassificationResultsTableView::setAllowShowDetails(bool newAllowShowDetails)
{
    allowShowDetails = newAllowShowDetails;
}

QTableView *ClassificationResultsTableView::getTableView()
{
    return ui->tableView;
}

Classification *ClassificationResultsTableView::getClassification() const
{
    return classification;
}

void ClassificationResultsTableView::setClassification(Classification *newClassification)
{
    classification = newClassification;
}

void ClassificationResultsTableView::on_pushButton_webhook_clicked()
{
    classification->sendResultsWebhook(save);
}

