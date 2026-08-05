#include "JumpDataDetailedInfoWindow.h"
#include "ui_JumpDataDetailedInfoWindow.h"

#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalAppSettings.h"
#include "../../global/GlobalDatabase.h"
#include "../../utilities/functions.h"
#include <dpp/dpp.h>

extern const QString appVersion;

JumpDataDetailedInfoWindow::JumpDataDetailedInfoWindow(JumpData *jumpData, QWidget *parent) :
    jumpData(jumpData),
    QWidget(parent),
    ui(new Ui::JumpDataDetailedInfoWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    windInfoWidget = new JumpWindInfoWidget;
    windInfoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout_windsInfo->addWidget(windInfoWidget);
}

JumpDataDetailedInfoWindow::~JumpDataDetailedInfoWindow()
{
    delete ui;
}

void JumpDataDetailedInfoWindow::fillJumpInformations()
{
    if(jumpData != nullptr)
    {
        Jumper * jumper;
        jumper = jumpData->getJumper();

        ui->label_nameAndSurname->setText(jumper->getNameAndSurname());
        ui->label_flag->setPixmap(CountryFlagsManager::getFlagPixmap(jumper->getCountryCode().toLower()).scaled(ui->label_flag->size()));
        ui->label_distance->setText(QString::number(jumpData->getDistance(), 'f', 1) + "m");
        ui->label_points->setText(QString::number(jumpData->getPoints(), 'f', 1) + tr("pkt"));
        ui->label_gate->setText(QString::number(jumpData->getGate()));
        ui->label_gatePoints->setText(QString::number(jumpData->getGateCompensation(), 'f', 1) + tr("pkt"));
        ui->label_gatePoints->setStyleSheet(Wind::getStyleSheetForAveragedWind(jumpData->getGateCompensation()));
        ui->label_averageWind->setText(QString::number(jumpData->getAveragedWind(), 'f', 2) + tr("m/s"));
        ui->label_averageWind->setStyleSheet(Wind::getStyleSheetForAveragedWind(jumpData->getAveragedWind()));
        ui->label_windPoints->setText(QString::number(jumpData->getWindCompensation(), 'f', 1) + tr("pkt"));
        ui->label_windPoints->setStyleSheet(Wind::getStyleSheetForAveragedWind(jumpData->getWindCompensation()));
        ui->label_totalCompensation->setText(QString::number(jumpData->getTotalCompensation(), 'f', 1) + tr("pkt"));
        ui->label_totalCompensation->setStyleSheet(Wind::getStyleSheetForAveragedWind(jumpData->getTotalCompensation()));
        ui->label_judges->setText("| " + QString::number(jumpData->getJudges().at(0)) + " | ");
        for(int i=0; i < 4; i++) //jedno zostalo juz wykonane
        {
            ui->label_judges->setText(ui->label_judges->text() + QString::number(jumpData->getJudges().at(i+1), 'f', 1) + " | ");
        }
        ui->label_judgesPoints->setText(QString::number(jumpData->getJudgesPoints(), 'f', 1) + tr("pkt"));
        ui->label_landingType->setText(jumpData->getLanding().getTextLandingType());
        ui->label_landingType->setStyleSheet(Landing::getStyleSheetForLandingTypeTextInfo(jumpData->getLanding().getType()));

        ui->label_takeoffRating->setText(QString::number(jumpData->getSimulationDataReference().getTakeoffRating()));
        ui->label_flightRating->setText(QString::number(jumpData->getSimulationDataReference().getFlightRating()));
        ui->label_random->setText(QString::number(jumpData->getSimulationDataReference().getMetersSumRandom()) + "m");
        ui->label_snowRain->setText(QString::number(jumpData->getSimulationDataReference().getInrunSnow()));

        if(jumpData->getDSQ())
            ui->label_DSQ->show();
        else
            ui->label_DSQ->hide();

        if(jumpData->getHasCoachGate()){
            ui->label_beats95HSPercents->show();
            if(jumpData->getBeats95HSPercents()){
                ui->label_beats95HSPercents->setText(tr("Zawodnik osiągnął 95% punktu HS"));
                ui->label_beats95HSPercents->setStyleSheet("color: rgb(49, 76, 44);");
            }
            else{
                ui->label_beats95HSPercents->setText(tr("Zawodnik nie osiągnął 95% punktu HS"));
                ui->label_beats95HSPercents->setStyleSheet("color: rgb(112, 37, 31);");
            }
            ui->label_beats95HSPercents->setText(ui->label_beats95HSPercents->text() + " (" + QString::number(roundDoubleToHalf(jumpData->getHill()->getHSPoint() * 0.95)) + "m)");
        }
        else{
            ui->label_beats95HSPercents->hide();
        }

        windInfoWidget->setWinds(jumpData->getWindsPointer());
        windInfoWidget->setKPoint(jumpData->getHill()->getKPoint());
        windInfoWidget->fillItemsLayout();

        showOrHideByAppSettings();

        if(jumpData->getCompetition() != nullptr){
            if(jumpData->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            {
                qDebug()<<"INDIVIDUAALLL";
                ui->label_positionInRound->setText(QString::number(jumpData->getPositionInRound()));
                ui->label_positionInGroup->hide();
                ui->label_positionInGroupTitle->hide();
                ui->label_positionInCompetition->hide();
                ui->label_positionInCompetitionTitle->hide();
            }
            else
            {
                qDebug()<<"TEAMMM";
                ui->label_positionInGroup->setText(QString::number(jumpData->getPositionInGroupForTeamCompetition()));
                ui->label_positionInRound->setText(QString::number(jumpData->getPositionInRoundForTeamCompetition()));
                ui->label_positionInCompetition->setText(QString::number(jumpData->getPositionInCompetitionForTeamCompetition()));
            }
        }
    }
    else qDebug()<<"jumpData is nullptr!";
}

void JumpDataDetailedInfoWindow::showOrHideByAppSettings()
{
    JumpDataInfoChoice * c = &GlobalAppSettings::get()->getJumpDataInfoChoiceReference();
    ui->label_distanceTitle->setHidden(!c->getDistance());
    ui->label_distance->setHidden(!c->getDistance());
    ui->label_pointsTitle->setHidden(!c->getPoints());
    ui->label_points->setHidden(!c->getPoints());
    ui->label_gateTitle->setHidden(!c->getGate());
    ui->label_gate->setHidden(!c->getGate());
    ui->label_averageWindTitle->setHidden(!c->getAveragedWind());
    ui->label_averageWind->setHidden(!c->getAveragedWind());
    ui->label_gatePointsTitle->setHidden(!c->getGateCompensation());
    ui->label_gatePoints->setHidden(!c->getGateCompensation());
    ui->label_windCompensationTitle->setHidden(!c->getWindCompensation());
    ui->label_windPoints->setHidden(!c->getWindCompensation());
    ui->label_totalCompensationTitle->setHidden(!c->getTotalCompensation());
    ui->label_totalCompensation->setHidden(!c->getTotalCompensation());
    ui->label_judgesTItle->setHidden(!c->getJudges());
    ui->label_judges->setHidden(!c->getJudges());
    ui->label_judgesPointsTitle->setHidden(!c->getJudgesPoints());
    ui->label_judgesPoints->setHidden(!c->getJudgesPoints());
    ui->label_landingTypeTitle->setHidden(!c->getLandingType());
    ui->label_landingType->setHidden(!c->getLandingType());
    windInfoWidget->setHidden(!c->getSpecificWind());
    ui->label_takeoffRatingTitle->setHidden(!c->getTakeoffRating());
    ui->label_takeoffRating->setHidden(!c->getTakeoffRating());
    ui->label_flightRatingTitle->setHidden(!c->getFlightRating());
    ui->label_flightRating->setHidden(!c->getFlightRating());
    ui->label_randomTitle->setHidden(!c->getDistanceRandom());
    ui->label_random->setHidden(!c->getDistanceRandom());
    ui->label_snowRainTitle->setHidden(!c->getInrunSnow());
    ui->label_snowRain->setHidden(!c->getInrunSnow());
    ui->label_positionInRoundTitle->setHidden(!c->getJumpPositionInRound());
    ui->label_positionInRound->setHidden(!c->getJumpPositionInRound());
    ui->label_positionInGroupTitle->setHidden(!c->getJumpPositionInRound());
    ui->label_positionInGroup->setHidden(!c->getJumpPositionInRound());
    ui->label_positionInCompetitionTitle->setHidden(!c->getJumpPositionInRound());
    ui->label_positionInCompetition->setHidden(!c->getJumpPositionInRound());
}

void JumpDataDetailedInfoWindow::removeJumperInfoTitle()
{
    if(ui->horizontalLayout_nameSurnameAndFlag != nullptr){
        QLayoutItem * item;
        while((item = ui->horizontalLayout_nameSurnameAndFlag->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
    delete ui->horizontalLayout_nameSurnameAndFlag;
    if(ui->horizontalLayout_jumperInfoTitle != nullptr){
        QLayoutItem * item;
        while((item = ui->horizontalLayout_jumperInfoTitle->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
    delete ui->horizontalLayout_jumperInfoTitle;
}

void JumpDataDetailedInfoWindow::removeWindsInfoLayout()
{
    if (ui->verticalLayout_windsInfo){
        while(ui->verticalLayout_windsInfo->count() > 0){
            QLayoutItem *item = ui->verticalLayout_windsInfo->takeAt(0);
            delete item->widget();
            delete item;
        }
    }
    delete ui->verticalLayout_windsInfo;
}



JumpData *JumpDataDetailedInfoWindow::getJumpData() const
{
    return jumpData;
}

void JumpDataDetailedInfoWindow::setJumpData(JumpData *newJumpData)
{
    jumpData = newJumpData;
}

void JumpDataDetailedInfoWindow::on_pushButton_clicked()
{
    dpp::cluster bot("");
    dpp::webhook wh(GlobalAppSettings::get()->getJumpInfoWebhook().toStdString());
    dpp::message msg;
    msg.add_embed(getEmbedForJumpInfo());
    bot.execute_webhook(wh, msg);
}

dpp::embed JumpDataDetailedInfoWindow::getEmbedForJumpInfo()
{
    Jumper * jumper = jumpData->getJumper();
    CompetitionInfo * comp = jumpData->getCompetition();

    int round = 0;
    if(jumpData->getInSingleJumps() == false){
    if(comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
        round = MyFunctions::getIndexOfItemInVector(comp->getResultsReference().getResultOfIndividualJumper(jumper)->getJumpsReference(), jumpData) + 1;
    else
        round = MyFunctions::getIndexOfItemInVector(comp->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&comp->getTeamsReference(), jumper->getCountryCode()))->getTeamJumperResult(jumper)->getJumpsReference(), jumpData) + 1;
    }

    dpp::embed embed;
    embed.set_color(dpp::colors::summer_sky);
    QString title = QString("**%1 %2**").arg(jumper->getNameAndSurname()).arg(QString(":flag_%1:").arg(GlobalDatabase::get()->getCountryByAlpha3(jumper->getCountryCode())->getAlpha2().toLower()));
    QString description;
    QString typeText;
    if(jumpData->getInSingleJumps() == false){
    if(comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
        typeText = tr("indywidualny");
    else
        typeText = tr("drużynowy");
    }
    if(jumpData->getInSingleJumps() == false)
        description = comp->getHill()->getHillTextForDiscord() + QString(" (%1)").arg(comp->getLongSerieTypeText()) + " " + typeText + tr(" - Skok %1").arg(QString::number(round));
    else
        description = jumpData->getHill()->getHillTextForDiscord() + tr(" (tryb pojedynczych skoków)");
    embed.set_title(title.toStdString());
    embed.set_description(description.toStdString());

    bool gateComp = true;
    bool windComp = true;
    bool judges = true;
    if(comp != nullptr)
    {
        gateComp = comp->getRulesPointer()->getHasGateCompensations();
        windComp = comp->getRulesPointer()->getHasWindCompensations();
        judges = comp->getRulesPointer()->getHasJudgesPoints();
    }

    JumpDataInfoChoice * c = &GlobalAppSettings::get()->getJumpDataInfoChoiceReference();
    if(c->getDistance())
        embed.add_field(tr("Odległość").toStdString(), QString::number(jumpData->getDistance(), 'f', 1).toStdString(), true);
    if(c->getPoints())
        embed.add_field(tr("Punkty").toStdString(), QString::number(jumpData->getPoints(), 'f', 1).toStdString(), true);
    if(c->getGate())
        embed.add_field(tr("Belka").toStdString(), QString::number(jumpData->getGate()).toStdString(), true);
    if(c->getAveragedWind())
        embed.add_field(tr("Uśredniony wiatr").toStdString(), QString::number(jumpData->getAveragedWind(), 'f', 1).toStdString(), true);
    if(c->getGateCompensation() && gateComp)
        embed.add_field(tr("Rekompensata za belkę").toStdString(), QString::number(jumpData->getGateCompensation(), 'f', 1).toStdString(), false);
    if(c->getWindCompensation() && windComp)
        embed.add_field(tr("Rekompensata za wiatr").toStdString(), QString::number(jumpData->getWindCompensation(), 'f', 1).toStdString(), true);
    if(c->getTotalCompensation() && (gateComp || windComp))
        embed.add_field(tr("Łączna rekompensata").toStdString(), QString::number(jumpData->getTotalCompensation(), 'f', 1).toStdString(), true);
    if(c->getJudges() && judges)
        embed.add_field(tr("Noty sędziowskie").toStdString(), jumpData->getJudgesText().toStdString(), false);
    if(c->getJudgesPoints() && judges)
        embed.add_field(tr("Punkty od sędziów").toStdString(), QString::number(jumpData->getJudgesPoints(), 'f', 1).toStdString(), true);
    if(c->getLandingType())
        embed.add_field(tr("Rodzaj lądowania").toStdString(), jumpData->getLanding().getTextLandingType().toStdString(), true);
    if(c->getSpecificWind())
        embed.add_field(tr("Wiatr przy skoku").toStdString(), jumpData->getWindsText().toStdString(), false);
    if(c->getPositionAfterJump() && jumpData->getInSingleJumps() == false){
        if(comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            embed.add_field(tr("Miejsce po skoku").toStdString(), QString::number(comp->getResultsReference().getResultOfIndividualJumper(jumper)->getPosition()).toStdString(), true);
        else
            embed.add_field(tr("Miejsce po skoku").toStdString(), QString::number(comp->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&comp->getTeamsReference(), jumper->getCountryCode()))->getPosition()).toStdString(), true);
    }
    if(c->getJumpPositionInRound() && jumpData->getInSingleJumps() == false && comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
        embed.add_field(tr("Pozycja noty w serii").toStdString(), QString::number(jumpData->getPositionInRound()).toStdString(), true);
    if(c->getTakeoffRating())
        embed.add_field(tr("Ocena wybicia").toStdString(), QString::number(jumpData->getSimulationDataReference().getTakeoffRating()).toStdString(), false);
    if(c->getFlightRating())
        embed.add_field(tr("Ocena lotu").toStdString(), QString::number(jumpData->getSimulationDataReference().getFlightRating()).toStdString(), true);
    if(c->getDistanceRandom())
        embed.add_field(tr("Losowość").toStdString(), QString::number(jumpData->getSimulationDataReference().getMetersSumRandom(), 'f', 1).toStdString() + "m", true);
    if(c->getInrunSnow())
        embed.add_field(tr("Śnieg/deszcz przy skoku").toStdString(), QString::number(jumpData->getSimulationDataReference().getInrunSnow()).toStdString(), true);


    embed.set_footer(
        dpp::embed_footer()
            .set_text(tr("Wiadomość wysłana z poziomu Sj.Sim ").toStdString() + appVersion.toStdString() + "\n" + "https://github.com/SiekamCebule/sj-sim")
        );
    return embed;
}

JumpWindInfoWidget *JumpDataDetailedInfoWindow::getWindInfoWidget() const
{
    return windInfoWidget;
}

Ui::JumpDataDetailedInfoWindow *JumpDataDetailedInfoWindow::getUi() const
{
    return ui;
}
