#include "JumperCompetitionResultsWidget.h"
#include "ui_JumperCompetitionResultsWidget.h"

#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalAppSettings.h"
#include "../../competitions/CompetitionInfo.h"
#include <dpp/dpp.h>
#include <exception>
#include <cmath>
#include <QVariantMap>

JumperCompetitionResultsWidget::JumperCompetitionResultsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JumperCompetitionResultsWidget), jumperResult(nullptr)
{
    ui->setupUi(this);
    while(ui->tabWidget_jumps->count()){auto *page=ui->tabWidget_jumps->widget(0);ui->tabWidget_jumps->removeTab(0);delete page;}
}

JumperCompetitionResultsWidget::~JumperCompetitionResultsWidget()
{
    delete ui;
}

void JumperCompetitionResultsWidget::fillWidget()
{
    if(!jumperResult || !jumperResult->getJumper() || !jumperResult->getCompetition())return;
    QVariantMap rankSummary;
    if(auto *competition=jumperResult->getCompetition()) {
        const auto &results=competition->getResultsReference().getResultsReference();
        const CompetitionSingleResult *ranked=jumperResult;
        const bool team=competition->getRules().getCompetitionType()==CompetitionRules::Team;
        if(team) {
            ranked=nullptr;
            for(const auto &result:results)if(result.getTeam()){for(const auto &jump:result.getJumps())if(jump.getJumper()==jumperResult->getJumper()){ranked=&result;break;}if(ranked)break;}
        }
        if(ranked) {
            rankSummary["positionNumber"]=ranked->getPosition();
            double leading=ranked->getPointsSum();
            for(const auto &result:results)if(result.getRankingRound()==ranked->getRankingRound() && std::isfinite(result.getPointsSum()))leading=qMax(leading,result.getPointsSum());
            if(std::isfinite(leading)&&std::isfinite(ranked->getPointsSum()))rankSummary["leaderGap"]=qMax(0.,qRound64((leading-ranked->getPointsSum())*10)/10.);

        }
    }
    setProperty("quickRankSummary",rankSummary);
    const QString competitorName = jumperResult->getJumper()->getNameAndSurname();
    ui->label_nameAndSurname->setText(competitorName);
    ui->label_nameAndSurname->setToolTip(competitorName);
    ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(jumperResult->getJumper()->getCountryCode().toLower()).scaled(ui->label_flag->size()));
    ui->label_img->setPixmap(jumperResult->getJumper()->getImagePixmap().scaled(ui->label_img->size()));

    ui->label_pointsSum->setText(QString::number(jumperResult->getPointsSum(), 'f', 1) + tr(" punktów"));
        if(jumperResult->getCompetition() != nullptr){
        if(jumperResult->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
             ui->label_actualPosition->setText("(" + QString::number(jumperResult->getPosition()) + tr(" miejsce)"));
        else{
             int pos = rankSummary.value("positionNumber").toInt();
             ui->label_actualPosition->setText("(" + QString::number(pos) + tr(" miejsce)"));
        }
        }
    if(GlobalAppSettings::get()->getJumpDataInfoChoiceReference().getPositionAfterJump() == true)
    {
        ui->label_actualPosition->show();
    }
    else
        ui->label_actualPosition->hide();

    while(ui->tabWidget_jumps->count()){auto *page=ui->tabWidget_jumps->widget(0);ui->tabWidget_jumps->removeTab(0);delete page;}
    int i = 0;
    for(auto & jump : jumperResult->getJumpsReference()){
        JumpDataDetailedInfoWindow * jumpInfo = new JumpDataDetailedInfoWindow(&jump, this);
        jumpInfo->fillJumpInformations();
        jumpInfo->removeJumperInfoTitle();
        jumpInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->tabWidget_jumps->addTab(jumpInfo, tr("Seria ") + QString::number(i + 1));

        i++;
    }
    ui->tabWidget_jumps->setCurrentIndex(ui->tabWidget_jumps->count() - 1);

    if(jumperResult->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual) {
        ui->pushButton_sendWebhook->setText(tr("Wyślij webhook"));
        ui->pushButton_sendWebhook->setToolTip(tr("Wyślij webhooka z występem zawodnika"));
    }
    else {
        ui->pushButton_sendWebhook->setText(tr("Wyślij webhook"));
        ui->pushButton_sendWebhook->setToolTip(tr("Wyślij webhooka z występem drużyny"));
    }
}

int JumperCompetitionResultsWidget::selectedRound() const {return ui->tabWidget_jumps->currentIndex();}
int JumperCompetitionResultsWidget::roundsCount() const {return ui->tabWidget_jumps->count();}
void JumperCompetitionResultsWidget::selectRound(int index){if(index>=0 && index<roundsCount())ui->tabWidget_jumps->setCurrentIndex(index);}

Ui::JumperCompetitionResultsWidget *JumperCompetitionResultsWidget::getUi() const
{
return ui;
}

CompetitionSingleResult *JumperCompetitionResultsWidget::getJumperResult() const
{
    return jumperResult;
}

void JumperCompetitionResultsWidget::setJumperResult(CompetitionSingleResult *newJumperResult)
{
    jumperResult = newJumperResult;
}

void JumperCompetitionResultsWidget::on_pushButton_sendWebhook_clicked()
{
    if(jumperResult == nullptr || jumperResult->getJumper() == nullptr
        || jumperResult->getCompetition() == nullptr || jumperResult->getCompetition()->getRulesPointer() == nullptr)
        return;
    const QString webhookUrl = GlobalAppSettings::get()->getCompetitionSingleResultWebhook().trimmed();
    if(webhookUrl.isEmpty())
        return;
    try
    {
        dpp::cluster bot("");
        dpp::webhook wh(webhookUrl.toStdString());
        dpp::message msg;
        if(jumperResult->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            msg.add_embed(jumperResult->getEmbedForIndividualSingleCompResult());
        else
        {
            CompetitionSingleResult * teamResult = jumperResult->getCompetition()->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&jumperResult->getCompetition()->getTeamsReference(), jumperResult->getJumper()->getCountryCode()));
            if(teamResult == nullptr)
                return;
            msg.add_embed(teamResult->getEmbedForTeamSingleCompResult());
        }
        bot.execute_webhook(wh, msg);
    }
    catch(const std::exception &error)
    {
        qWarning() << "Single-result webhook failed:" << error.what();
    }
    catch(...)
    {
        qWarning() << "Single-result webhook failed with an unknown error";
    }
}
