#include "StartListByOtherCompetitionConfigWindow.h"
#include "ui_StartListByOtherCompetitionConfigWindow.h"
#include "../../global/CountryFlagsManager.h"

StartListByOtherCompetitionConfigWindow::StartListByOtherCompetitionConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartListByOtherCompetitionConfigWindow)
{
    ui->setupUi(this);
}

StartListByOtherCompetitionConfigWindow::~StartListByOtherCompetitionConfigWindow()
{
    delete ui;
}

void StartListByOtherCompetitionConfigWindow::setupComboBox()
{
    ui->comboBox_comp->clear();
    for(auto & season : save->getSeasonsReference())
    {
        QString string = "--- Sezon " + QString::number(season.getSeasonNumber()) + " ---";
        ui->comboBox_comp->addItem(string);
        for(auto & cal : season.getCalendarsReference())
        {
            QString string = "--- Kalendarz " + cal->getName() + " ---";
            ui->comboBox_comp->addItem(string);
            for(auto & competition : CompetitionInfo::getSpecificTypeCompetitions(cal->getCompetitionsReference(), competitionType))
            {
                if(competition->getPlayed() == false)
                    break;
                QString string = QString::number(cal->getCompetitionsReference().indexOf(competition) + 1) + ". " + competition->getHill()->getHillText();
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
                ui->comboBox_comp->addItem(QIcon(pixmap), string);
            }
        }
    }
    ui->comboBox_comp->setCurrentIndex(ui->comboBox_comp->count() - 1);
}

CompetitionInfo *StartListByOtherCompetitionConfigWindow::getCompetition()
{
    int index = ui->comboBox_comp->currentIndex();
    if(index >= 0)
    {
        for(auto & season : save->getSeasonsReference())
        {
            index -= 1;
            for(auto & calendar : season.getCalendarsReference())
            {
                index -= 1;
                if(index > CompetitionInfo::getSpecificTypeCompetitions(calendar->getCompetitionsReference(), competitionType).count() && CompetitionInfo::getSpecificTypeCompetitions(calendar->getCompetitionsReference(), competitionType).count() > 0)
                {
                    index -= CompetitionInfo::howManyPlayedStatic(CompetitionInfo::getSpecificTypeCompetitions(calendar->getCompetitionsReference(), competitionType));
                }
                else
                {
                    //qDebug()<<CompetitionInfo::getSpecificTypeCompetitions(calendar->getCompetitionsReference(), competitionType)[index]->getHill()->getHillText();
                    return CompetitionInfo::getSpecificTypeCompetitions(calendar->getCompetitionsReference(), competitionType).at(index);
                }
            }
        }
    }
    return nullptr;
}

bool StartListByOtherCompetitionConfigWindow::getCheckBoxState()
{
    return ui->checkBox->isChecked();
}

void StartListByOtherCompetitionConfigWindow::on_pushButton_submit_clicked()
{
    accept();
}

int StartListByOtherCompetitionConfigWindow::getCompetitionType() const
{
    return competitionType;
}

void StartListByOtherCompetitionConfigWindow::setCompetitionType(int newCompetitionType)
{
    competitionType = newCompetitionType;
}

SimulationSave *StartListByOtherCompetitionConfigWindow::getSave() const
{
    return save;
}

void StartListByOtherCompetitionConfigWindow::setSave(SimulationSave *newSave)
{
    save = newSave;
}

