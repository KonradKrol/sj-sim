#include "../../../global/AppPaths.h"
#include "ClassificationResultsTableView.h"
#include "ui_ClassificationResultsTableView.h"
#include "../../../global/CountryFlagsManager.h"
#include <QComboBox>
#include <QDir>
#include "../../../global/CountryCodes.h"
#include <QStandardItemModel>
#include <QSignalBlocker>
#include <algorithm>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QSet>
#include <QLocale>
#include <QPushButton>
#include "../../ResultsImage/ResultsImage.h"
#include "../../../global/DiscordWebhook.h"
#include "../../../global/GlobalAppSettings.h"
#include "../../../seasons/SimulationSave.h"

ClassificationResultsTableView::ClassificationResultsTableView(bool allowShowDetails, Classification * classification, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassificationResultsTableView),
    allowShowDetails(allowShowDetails),
    classification(classification)
{
    ui->setupUi(this);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);ui->tableView->setProperty("quickClassificationRanking",true);ui->tableView->setProperty("quickScoreColumn",2);ui->tableView->setProperty("quickGapColumn",2);
    save=nullptr;historyChoice=new QComboBox(this);historyChoice->setObjectName("comboBox_classificationHistory");historyChoice->setAccessibleName(tr("Stan klasyfikacji po konkursie"));historyChoice->hide();ui->verticalLayout->insertWidget(0,historyChoice);
    historyModel=new QStandardItemModel(this);connect(historyChoice,qOverload<int>(&QComboBox::currentIndexChanged),this,[this]{showHistory();});
    exportImage=new QPushButton(tr("Zapisz klasyfikację jako obraz…"),this);exportImage->setObjectName("pushButton_exportClassificationImage");
    exportImage->setToolTip(tr("Zapisuje aktualny stan klasyfikacji jako plik PNG i kopiuje go do schowka."));
    ui->verticalLayout->addWidget(exportImage);
    connect(exportImage,&QPushButton::clicked,this,[this]{ResultsImage::exportClassification(this,this->classification);});
    publishDiscord=new QPushButton(tr("Opublikuj klasyfikację na Discordzie…"),this);publishDiscord->setObjectName("pushButton_publishClassificationDiscord");
    publishDiscord->setToolTip(tr("Wysyła obraz z aktualnym stanem klasyfikacji na kanał Discord ustawiony w ustawieniach."));
    ui->verticalLayout->addWidget(publishDiscord);
    connect(publishDiscord,&QPushButton::clicked,this,[this]{ResultsImage::publishClassification(this,this->classification);});
    publishDiscord->setVisible(DiscordWebhook::isValidUrl(GlobalAppSettings::get()->getDiscordWebhookUrl()));

    tableModel = new ClassificationResultsTableModel(nullptr, this);

    connect(ui->tableView, &QTableView::doubleClicked, this, [this](const QModelIndex & index){
        if(this->classification && this->classification->getClassificationType()==Classification::Team && index.isValid())showContributions(index.sibling(index.row(),1).data(Qt::UserRole).toString());
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


void ClassificationResultsTableView::setHistoryCalendar(SeasonCalendar *calendar){historyCalendar=calendar;if(!calendar)historyChoice->hide();}
bool ClassificationResultsTableView::historical() const {return historyCalendar && historyChoice->currentIndex()>0;}
void ClassificationResultsTableView::showHistory(){
    // The image shows the current standings, not an archived state after an earlier event.
    exportImage->setEnabled(classification && !historical());
    // Shown only when a Discord webhook is set in the settings.
    publishDiscord->setVisible(DiscordWebhook::isValidUrl(GlobalAppSettings::get()->getDiscordWebhookUrl()));
    publishDiscord->setEnabled(classification && !historical());
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
        for(auto *item:items)item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);items[1]->setData(row.country,Qt::UserRole);items[1]->setIcon(QIcon(AppPaths::flagFile(row.country)));nextModel->appendRow(items);
    }
    auto *previousSelection=ui->tableView->selectionModel();historyModel=nextModel;ui->tableView->setModel(historyModel);if(previousSelection && previousSelection!=ui->tableView->selectionModel())previousSelection->deleteLater();previousModel->deleteLater();
}

void ClassificationResultsTableView::showContributions(const QString &country) {
    if(!classification || country.isEmpty())return;
    QVector<CompetitionInfo *> events;
    const auto append=[&](CompetitionInfo *event){if(event && event->getPlayed() && !events.contains(event))events.append(event);};
    if(historical()){for(int i=0;i<qMin(historyChoice->currentIndex(),int(historyEvents.size()));++i)append(historyEvents[i]);}
    else if(auto *result=classification->getResultOfTeam(country)){for(auto *source:result->getCompetitionsResults())if(source)append(source->getCompetition());for(auto *source:result->getSingleResults())if(source)append(source->getCompetition());}
    struct Row {QString name;double points=0;};QHash<QString,Row> athletes;double teamPoints=0,total=0;
    for(auto *event:events)for(const auto &contribution:classification->contributions(event))if(contribution.country==country){
        total+=contribution.points;
        if(contribution.jumper && contribution.points!=0){auto &row=athletes[contribution.jumper->getIDStr()];row.name=contribution.jumper->getNameAndSurname();row.points+=contribution.points;}
        else if(!contribution.jumper)teamPoints+=contribution.points;
    }
    auto rows=athletes.values();std::stable_sort(rows.begin(),rows.end(),[](const Row &a,const Row &b){return a.points==b.points?a.name<b.name:a.points>b.points;});
    QDialog dialog(this);dialog.setObjectName("classificationContributionDetails");dialog.setWindowTitle(tr("Wkład do klasyfikacji · %1").arg(country));dialog.setProperty("contributionCountry",country);dialog.setProperty("contributionName",classification->getName());dialog.setProperty("contributionTotal",total);dialog.resize(620,520);auto *layout=new QVBoxLayout(&dialog);
    auto *heading=new QLabel(tr("%1 · %2").arg(country,classification->getName()),&dialog);heading->setWordWrap(true);layout->addWidget(heading);
    auto *summary=new QLabel(tr("Razem: %1 pkt").arg(QLocale().toString(total,'g',12)),&dialog);summary->setObjectName("classificationContributionTotal");layout->addWidget(summary);
    auto *model=new QStandardItemModel(&dialog);model->setHorizontalHeaderLabels({tr("Lp."),tr("Zawodnik / źródło"),tr("Punkty")});
    const auto add=[&](const QString &name,double points){QList<QStandardItem *> items{new QStandardItem(QString::number(model->rowCount()+1)),new QStandardItem(name),new QStandardItem(QLocale().toString(points,'g',12))};for(auto *item:items)item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);items[2]->setData(points,Qt::UserRole);model->appendRow(items);};
    for(const auto &row:rows)add(row.name,row.points);if(teamPoints!=0)add(tr("Konkursy drużynowe"),teamPoints);
    if(rows.isEmpty() && teamPoints==0){auto *empty=new QLabel(tr("Brak wkładu punktowego w tym zakresie konkursów."),&dialog);empty->setWordWrap(true);layout->addWidget(empty);}
    auto *table=new QTableView(&dialog);table->setObjectName("classificationContributionTable");table->setProperty("quickPlainTable",true);table->setProperty("quickScoreColumn",2);table->setSelectionMode(QAbstractItemView::NoSelection);table->setModel(model);table->setEditTriggers(QAbstractItemView::NoEditTriggers);layout->addWidget(table,1);table->resizeColumnsToContents();

    // Place history in the same window: the team's points and place after every event of the
    // classification, within the same range of events as the contribution breakdown.
    SeasonCalendar *calendar=historyCalendar;
    if(!calendar && save)for(auto &season:save->getSeasonsReference())for(auto *candidate:season.getCalendarsReference())
        if(candidate && candidate->getClassificationsReference().contains(classification))calendar=candidate;
    if(calendar){
        const auto places=classification->constructTeamsArchiveResults(nullptr,calendar).value(country);
        auto *history=new QStandardItemModel(&dialog);history->setHorizontalHeaderLabels({tr("Konkurs"),tr("Punkty"),tr("Miejsce")});
        int number=0;
        for(auto *event:calendar->getCompetitionsReference()){
            if(!places.contains(event))continue;
            ++number;
            if(historical() && !historyEvents.mid(0,historyChoice->currentIndex()).contains(event))continue;
            double points=0;for(const auto &contribution:classification->contributions(event))if(contribution.country==country)points+=contribution.points;
            const QString name=tr("%1. %2").arg(number).arg(event->getHill()?event->getHill()->getName():event->getShortSerieTypeText());
            QList<QStandardItem *> items{new QStandardItem(name),new QStandardItem(QLocale().toString(points,'g',12)),new QStandardItem(QString::number(places.value(event)))};
            for(auto *item:items)item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            items[1]->setData(points,Qt::UserRole);items[2]->setData(places.value(event),Qt::UserRole);
            history->appendRow(items);
        }
        auto *historyHeading=new QLabel(tr("Miejsce po kolejnych konkursach"),&dialog);layout->addWidget(historyHeading);
        auto *historyTable=new QTableView(&dialog);historyTable->setObjectName("classificationPlaceHistoryTable");historyTable->setProperty("quickPlainTable",true);
        historyTable->setSelectionMode(QAbstractItemView::NoSelection);historyTable->setModel(history);historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        layout->addWidget(historyTable,1);historyTable->resizeColumnsToContents();
        dialog.setWindowTitle(tr("Wkład i historia miejsca · %1").arg(country));
    }
    auto *buttons=new QDialogButtonBox(QDialogButtonBox::Close,&dialog);connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);layout->addWidget(buttons);dialog.exec();
}
