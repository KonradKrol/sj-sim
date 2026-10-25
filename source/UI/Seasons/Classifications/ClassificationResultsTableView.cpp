#include "ClassificationResultsTableView.h"
#include "ui_ClassificationResultsTableView.h"
#include "../../../global/CountryFlagsManager.h"
#include <QComboBox>
#include <QDir>
#include "../../../global/CountryCodes.h"
#include <QStandardItemModel>
#include <QSignalBlocker>
#include <algorithm>

ClassificationResultsTableView::ClassificationResultsTableView(bool allowShowDetails, Classification * classification, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassificationResultsTableView),
    allowShowDetails(allowShowDetails),
    classification(classification)
{
    ui->setupUi(this);
    save=nullptr;historyChoice=new QComboBox(this);historyChoice->setObjectName("comboBox_classificationHistory");historyChoice->setAccessibleName(tr("Stan klasyfikacji po konkursie"));historyChoice->hide();ui->verticalLayout->insertWidget(0,historyChoice);
    historyModel=new QStandardItemModel(this);connect(historyChoice,qOverload<int>(&QComboBox::currentIndexChanged),this,[this]{showHistory();});

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
    if(historyCalendar){
        const QSignalBlocker blocker(historyChoice);const QString selected=historyClassification==classification?historyChoice->currentData().toString():QString();historyClassification=classification;
        historyEvents.clear();historyChoice->clear();historyChoice->addItem(tr("Wszystkie konkursy"),QString());
        for(auto *event:historyCalendar->getCompetitionsReference())if(event && event->getPlayed() && event->getClassificationsReference().contains(classification)){
            historyEvents.append(event);historyChoice->addItem(tr("Po %1. konkursie · %2").arg(historyEvents.size()).arg(event->getHill()?event->getHill()->getName():QString()),event->getIDStr());
        }
        historyChoice->setCurrentIndex(qMax(0,historyChoice->findData(selected)));historyChoice->setVisible(classification!=nullptr);showHistory();
    }
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


void ClassificationResultsTableView::setHistoryCalendar(SeasonCalendar *calendar){historyCalendar=calendar;if(!calendar)historyChoice->hide();}
bool ClassificationResultsTableView::historical() const {return historyCalendar && historyChoice->currentIndex()>0;}
void ClassificationResultsTableView::showHistory(){
    ui->pushButton_webhook->setEnabled(classification && !historical());
    if(!historical() || !classification){ui->tableView->setModel(tableModel);return;}
    struct Row {Jumper *jumper=nullptr;QString country;double points=0;};QHash<QString,Row> totals;
    for(int i=0;i<qMin(historyChoice->currentIndex(),int(historyEvents.size()));++i)for(const auto &c:classification->contributions(historyEvents[i])){
        const bool individual=classification->getClassificationType()==Classification::Individual;
        const QString key=individual && c.jumper?c.jumper->getIDStr():c.country;
        auto &row=totals[key];row.jumper=c.jumper;row.country=c.country;row.points+=c.points;
    }
    auto rows=totals.values();std::stable_sort(rows.begin(),rows.end(),[](const Row &a,const Row &b){if(qAbs(a.points-b.points)>1e-9)return a.points>b.points;return (a.jumper?a.jumper->getIDStr():a.country)<(b.jumper?b.jumper->getIDStr():b.country);});
    auto *previousModel=historyModel;auto *nextModel=new QStandardItemModel(this);nextModel->setHorizontalHeaderLabels({tr("Poz."),classification->getClassificationType()==Classification::Individual?tr("Zawodnik"):tr("Drużyna"),tr("Punkty")});
    int place=0;double previous=0;for(int i=0;i<rows.size();++i){const auto &row=rows[i];if(i==0 || qAbs(row.points-previous)>1e-9)place=i+1;previous=row.points;
        QList<QStandardItem *> items{new QStandardItem(QString::number(place)),new QStandardItem(classification->getClassificationType()==Classification::Individual && row.jumper?row.jumper->getNameAndSurname():row.country),new QStandardItem(QString::number(row.points,'g',12))};
        for(auto *item:items)item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);items[1]->setIcon(QIcon(QDir::current().absoluteFilePath("flags/"+CountryCodes::toIso(row.country)+".svg")));nextModel->appendRow(items);
    }
    auto *previousSelection=ui->tableView->selectionModel();historyModel=nextModel;ui->tableView->setModel(historyModel);if(previousSelection && previousSelection!=ui->tableView->selectionModel())previousSelection->deleteLater();previousModel->deleteLater();
}
