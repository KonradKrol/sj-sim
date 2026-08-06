#include "StartListByOtherCompetitionConfigWindow.h"
#include "ui_StartListByOtherCompetitionConfigWindow.h"
#include "../../global/CountryFlagsManager.h"
#include <QStandardItemModel>

StartListByOtherCompetitionConfigWindow::StartListByOtherCompetitionConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartListByOtherCompetitionConfigWindow),
    save(nullptr),
    competitionType(CompetitionRules::Individual)
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
    if(save == nullptr)
        return;

    auto addHeader = [this](const QString &text) {
        ui->comboBox_comp->addItem(text);
        if(auto *model = qobject_cast<QStandardItemModel *>(ui->comboBox_comp->model()))
            model->item(ui->comboBox_comp->count() - 1)->setEnabled(false);
    };

    int lastCompetitionIndex = -1;
    for(auto & season : save->getSeasonsReference())
    {
        addHeader("--- Sezon " + QString::number(season.getSeasonNumber()) + " ---");
        for(auto & cal : season.getCalendarsReference())
        {
            if(cal == nullptr)
                continue;
            addHeader("--- Kalendarz " + cal->getName() + " ---");
            for(auto & competition : CompetitionInfo::getSpecificTypeCompetitions(cal->getCompetitionsReference(), competitionType))
            {
                if(competition == nullptr || !competition->getPlayed())
                    continue;
                QString string = QString::number(cal->getCompetitionsReference().indexOf(competition) + 1) + ". ";
                string += competition->getHill() != nullptr ? competition->getHill()->getHillText() : tr("Brak skoczni");
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
                const QIcon icon = competition->getHill() != nullptr
                    ? QIcon(CountryFlagsManager::getFlagPixmap(competition->getHill()->getCountryCode().toLower()))
                    : QIcon();
                ui->comboBox_comp->addItem(icon, string,
                    QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(competition)));
                lastCompetitionIndex = ui->comboBox_comp->count() - 1;
            }
        }
    }
    ui->comboBox_comp->setCurrentIndex(lastCompetitionIndex);
    ui->pushButton_submit->setEnabled(lastCompetitionIndex >= 0);
}

CompetitionInfo *StartListByOtherCompetitionConfigWindow::getCompetition()
{
    const quintptr pointerValue = ui->comboBox_comp->currentData().value<quintptr>();
    return reinterpret_cast<CompetitionInfo *>(pointerValue);
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
