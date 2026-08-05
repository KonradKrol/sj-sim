#include "CountriesEditorWindow.h"
#include "ui_CountriesEditorWindow.h"
#include "../../global/GlobalDatabase.h"

CountriesEditorWindow::CountriesEditorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountriesEditorWindow),
    model(nullptr)
{
    ui->setupUi(this);

    action_insert = new QAction(this);
    action_insert->setShortcut(Qt::CTRL | Qt::Key_A);
    this->addAction(action_insert);
    connect(action_insert, &QAction::triggered, this, [this](){
        int insertIndex = 0;
        if(ui->tableView->selectionModel()->selectedIndexes().count() > 0)
            insertIndex = ui->tableView->selectionModel()->selectedIndexes().first().row();
        model->getCountries()->insert(insertIndex, Country("COUNTRY", "XX", "XXX"));
        model->insertRow(insertIndex);
        emit model->dataChanged(model->index(insertIndex, 0), model->index(insertIndex, 1));
    });

    action_remove = new QAction(this);
    action_remove->setShortcut(Qt::CTRL | Qt::Key_D);
    this->addAction(action_remove);
    connect(action_remove, &QAction::triggered, this, [this](){
        if(ui->tableView->selectionModel()->selectedIndexes().count() > 0)
        {
            int removeIndex = ui->tableView->selectionModel()->selectedIndexes().first().row();
            model->getCountries()->remove(removeIndex);
            model->removeRow(removeIndex);
            //emit model->dataChanged(model->index(removeRow, 0), model->index(removeRow, 1));
        }
    });
}

CountriesEditorWindow::~CountriesEditorWindow()
{
    delete ui;
}

void CountriesEditorWindow::fill()
{
    if(model == nullptr){
        model = new CountriesEditorTableModel(this);
        connect(model, &CountriesEditorTableModel::edited, this, [this](){
            //sort();
        });
    }
    ui->tableView->setModel(nullptr);
    ui->tableView->setModel(model);
}

bool cmp(const Country & p1, Country & p2)
{
    return p1.getAlpha3() < p2.getAlpha3();
}

void CountriesEditorWindow::sort()
{
    std::sort(model->getCountries()->begin(), model->getCountries()->end(), &cmp);
    fill();
}

void CountriesEditorWindow::on_pushButton_submit_clicked()
{
    accept();
}

CountriesEditorTableModel *CountriesEditorWindow::getModel() const
{
    return model;
}

void CountriesEditorWindow::setModel(CountriesEditorTableModel *newModel)
{
    model = newModel;
}

