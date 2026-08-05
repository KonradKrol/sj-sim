#include "JumperCompetitionResultsWidget.h"
#include "ui_JumperCompetitionResultsWidget.h"

#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalAppSettings.h"
#include "../../competitions/CompetitionInfo.h"
#include <dpp/dpp.h>

JumperCompetitionResultsWidget::JumperCompetitionResultsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JumperCompetitionResultsWidget)
{
    ui->setupUi(this);
    for(int i=0; i<ui->tabWidget_jumps->count(); i++){
        delete ui->tabWidget_jumps->widget(0);
        ui->tabWidget_jumps->removeTab(0);
    }
}

JumperCompetitionResultsWidget::~JumperCompetitionResultsWidget()
{
    delete ui;
}

void JumperCompetitionResultsWidget::fillWidget()
{
    ui->label_nameAndSurname->setText(jumperResult->getJumper()->getNameAndSurname());
    ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(jumperResult->getJumper()->getCountryCode().toLower()).scaled(ui->label_flag->size()));
    ui->label_img->setPixmap(jumperResult->getJumper()->getImagePixmap().scaled(ui->label_img->size()));

    ui->label_pointsSum->setText(QString::number(jumperResult->getPointsSum(), 'f', 1) + tr(" punktów"));
        if(jumperResult->getCompetition() != nullptr){
        if(jumperResult->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
             ui->label_actualPosition->setText("(" + QString::number(jumperResult->getPosition()) + tr(" miejsce)"));
        else{
             int pos = jumperResult->getCompetition()->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&jumperResult->getCompetition()->getTeamsReference(), jumperResult->getJumper()->getCountryCode()))->getPosition();
             ui->label_actualPosition->setText("(" + QString::number(pos) + tr(" miejsce)"));
        }
        }
    if(GlobalAppSettings::get()->getJumpDataInfoChoiceReference().getPositionAfterJump() == true)
    {
        ui->label_actualPosition->show();
    }
    else
        ui->label_actualPosition->hide();

    ui->tabWidget_jumps->clear();
    int i = 0;
    for(auto & jump : jumperResult->getJumpsReference()){
        JumpDataDetailedInfoWindow * jumpInfo = new JumpDataDetailedInfoWindow(&jump, this);
        jumpInfo->fillJumpInformations();
        jumpInfo->removeJumperInfoTitle();
        jumpInfo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        ui->tabWidget_jumps->addTab(jumpInfo, tr("Seria ") + QString::number(i + 1));

        i++;
    }
    ui->tabWidget_jumps->setCurrentIndex(ui->tabWidget_jumps->count() - 1);

    if(jumperResult->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
        ui->pushButton_sendWebhook->setText(tr("Wyślij webhooka (występ zawodnika)"));
    else
        ui->pushButton_sendWebhook->setText(tr("Wyślij webhooka (występ drużyny)"));
}

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
    dpp::cluster bot("");
    dpp::webhook wh(GlobalAppSettings::get()->getCompetitionSingleResultWebhook().toStdString());
    dpp::message msg;
    if(jumperResult->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
        msg.add_embed(jumperResult->getEmbedForIndividualSingleCompResult());
    else
    {
        CompetitionSingleResult * teamResult = jumperResult->getCompetition()->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&jumperResult->getCompetition()->getTeamsReference(), jumperResult->getJumper()->getCountryCode()));
        msg.add_embed(teamResult->getEmbedForTeamSingleCompResult());
    }
    bot.execute_webhook(wh, msg);
}

