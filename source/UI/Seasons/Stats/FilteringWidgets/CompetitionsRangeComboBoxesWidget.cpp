#include "CompetitionsRangeComboBoxesWidget.h"
#include "ui_CompetitionsRangeComboBoxesWidget.h"
#include <QIcon>
#include <QStandardItemModel>
#include "../../../../global/CountryFlagsManager.h"

CompetitionsRangeComboBoxesWidget::CompetitionsRangeComboBoxesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompetitionsRangeComboBoxesWidget)
{
    ui->setupUi(this);
}

CompetitionsRangeComboBoxesWidget::~CompetitionsRangeComboBoxesWidget()
{
    delete ui;
}

void CompetitionsRangeComboBoxesWidget::setupConnections()
{
    connect(ui->comboBox_firstCompetition, qOverload<int>(&QComboBox::currentIndexChanged), this, &CompetitionsRangeComboBoxesWidget::changed);
    connect(ui->comboBox_secondCompetition, qOverload<int>(&QComboBox::currentIndexChanged), this, &CompetitionsRangeComboBoxesWidget::changed);
}

void CompetitionsRangeComboBoxesWidget::setupComboBoxes()
{
    qDebug()<<QString("setup............. (%1, %2)").arg(calendarFilter).arg(mergeCalendars);
    disconnect(ui->comboBox_firstCompetition, qOverload<int>(&QComboBox::currentIndexChanged), this, &CompetitionsRangeComboBoxesWidget::changed);
    disconnect(ui->comboBox_secondCompetition, qOverload<int>(&QComboBox::currentIndexChanged), this, &CompetitionsRangeComboBoxesWidget::changed);
    ui->comboBox_firstCompetition->clear();
    ui->comboBox_secondCompetition->clear();
    entries.clear();
    int seasonIndex=1;
    if(seasonsList)
    for(auto & season : *seasonsList)
    {
        int competitionIndex = 1;
        QString string = "--- Sezon " + QString::number(season.getSeasonNumber()) + " ---";
        ui->comboBox_firstCompetition->addItem(string);
        ui->comboBox_secondCompetition->addItem(string);
        entries.append(nullptr);
        for(auto *combo:{ui->comboBox_firstCompetition,ui->comboBox_secondCompetition})
            if(auto *model=qobject_cast<QStandardItemModel *>(combo->model()))model->item(combo->count()-1)->setEnabled(false);
        for(auto & cal : season.getCalendarsReference())
        {
            if(cal->getName() == calendarFilter || calendarFilter == "" || mergeCalendars == true)
            {
                for(auto & competition : cal->getCompetitionsReference())
                {
                    if(competition->getPlayed() == false)
                        break;
                    QString string = "S" + QString::number(seasonIndex) + "C" + QString::number(competitionIndex);
                    string += " - " + competition->getHill()->getName() + " HS" + QString::number(competition->getHill()->getHSPoint());
                    if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
                        string += tr(" (D)");
                    switch(competition->getSerieType())
                    {
                    case CompetitionInfo::Competition:
                        string += tr(" (Konk.)");
                        break;
                    case CompetitionInfo::Qualifications:
                        string += tr(" (Kwal.)");
                        break;
                    case CompetitionInfo::TrialRound:
                        string += tr(" (Prób.)");
                            break;
                    case CompetitionInfo::Training:
                        string += tr(" (Tren.)");
                        break;
                    }

                    QPixmap pixmap = CountryFlagsManager::getFlagPixmap(competition->getHill()->getCountryCode().toLower());
                    ui->comboBox_firstCompetition->addItem(QIcon(pixmap), string);
                    ui->comboBox_secondCompetition->addItem(QIcon(pixmap), string);
                    entries.append(competition);
                    competitionIndex++;
                }
                qDebug()<<"kal "<<cal->getName()<<", "<<ui->comboBox_firstCompetition->count();
                qDebug()<<"count: "<<ui->comboBox_firstCompetition->count();
            }
        }
        seasonIndex++;
    }
    int first=-1,last=-1;
    for(int i=0;i<entries.size();++i)if(entries[i]){if(first<0)first=i;last=i;}
    ui->comboBox_firstCompetition->setCurrentIndex(first);
    ui->comboBox_secondCompetition->setCurrentIndex(last);
    setupConnections();
}

bool CompetitionsRangeComboBoxesWidget::getMergeCalendars() const
{
    return mergeCalendars;
}

void CompetitionsRangeComboBoxesWidget::setMergeCalendars(bool newMergeCalendars)
{
    mergeCalendars = newMergeCalendars;
}

/*
 * Sezon 2023 (0)
 * K1 (1)
 * K2 (2)
 * Sezon 2024 (3)
 * K1 (4)
 * K2 (5)
 * Sezon 2025 (6)
 * K1 (7)
 * K2 (8)
*/
CompetitionInfo *CompetitionsRangeComboBoxesWidget::getCompetition(int which)
{
    if(which!=1 && which!=2)return nullptr;
    const int index=which==1?ui->comboBox_firstCompetition->currentIndex():ui->comboBox_secondCompetition->currentIndex();
    return entries.value(index,nullptr);
}

QVector<Season> *CompetitionsRangeComboBoxesWidget::getSeasonsList() const
{
    return seasonsList;
}

void CompetitionsRangeComboBoxesWidget::setSeasonsList(QVector<Season> *newSeasonsList)
{
    seasonsList = newSeasonsList;
}

QString CompetitionsRangeComboBoxesWidget::getCalendarFilter() const
{
    return calendarFilter;
}

void CompetitionsRangeComboBoxesWidget::setCalendarFilter(const QString &newCalendarFilter)
{
    calendarFilter = newCalendarFilter;
}
