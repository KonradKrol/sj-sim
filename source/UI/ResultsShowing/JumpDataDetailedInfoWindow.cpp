#include <cmath>
#include "JumpDataDetailedInfoWindow.h"
#include "ui_JumpDataDetailedInfoWindow.h"

#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalAppSettings.h"
#include "../../global/GlobalDatabase.h"
#include "../../utilities/functions.h"
#include <exception>

extern const QString appVersion;

JumpDataDetailedInfoWindow::JumpDataDetailedInfoWindow(JumpData *jumpData, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JumpDataDetailedInfoWindow),
    jumpData(jumpData),
    windInfoWidget(nullptr)
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
    if(!jumpData || !jumpData->getJumper() || !jumpData->getHill()) {
        ui->pushButton->setEnabled(false);
        ui->label_nameAndSurname->setText(tr("Brak danych skoku"));ui->label_flag->clear();
        for(const auto *name:{"label_distance","label_points","label_gate","label_gatePoints","label_averageWind","label_windPoints","label_totalCompensation","label_judges","label_judgesPoints","label_landingType","label_takeoffRating","label_flightRating","label_random","label_snowRain","label_positionInRound","label_positionInGroup","label_positionInCompetition"})
            if(auto *label=findChild<QLabel *>(name))label->setText(QString::fromUtf8("—"));
        ui->label_DSQ->hide();ui->label_beats95HSPercents->hide();windInfoWidget->hide();return;
    }
    windInfoWidget->show();
    if(jumpData != nullptr)
    {
        const bool hasCompetitionContext = jumpData->getInSingleJumps()
            || jumpData->getCompetition() != nullptr;
        ui->pushButton->setEnabled(hasCompetitionContext);
        if(!hasCompetitionContext)
        {
            ui->label_positionInRound->hide();
            ui->label_positionInRoundTitle->hide();
            ui->label_positionInGroup->hide();
            ui->label_positionInGroupTitle->hide();
            ui->label_positionInCompetition->hide();
            ui->label_positionInCompetitionTitle->hide();
            ui->pushButton->setToolTip(tr("Brak powiązania skoku z konkursem"));
        }

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
        QString judgesText;
        for(double judge : jumpData->getJudges())
            judgesText += "| " + QString::number(judge, 'f', 1) + " ";
        ui->label_judges->setText(judgesText.isEmpty() ? tr("Brak not") : judgesText + "|");
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
            ui->label_beats95HSPercents->setText(ui->label_beats95HSPercents->text() + " (" + QString::number((std::ceil(jumpData->getHill()->getHSPoint() * 0.95 * 2) / 2)) + "m)");
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
    if(auto *competition=jumpData->getCompetition()) {
        const auto &rules=competition->getRules();
        const auto hidePair=[this](bool allowed,QLabel *title,QLabel *value){if(!allowed){title->hide();value->hide();}};
        hidePair(rules.getHasJudgesPoints(),ui->label_judgesTItle,ui->label_judges);
        hidePair(rules.getHasJudgesPoints(),ui->label_judgesPointsTitle,ui->label_judgesPoints);
        hidePair(rules.getHasWindCompensations(),ui->label_windCompensationTitle,ui->label_windPoints);
        hidePair(rules.getHasGateCompensations(),ui->label_gatePointsTitle,ui->label_gatePoints);
        hidePair(rules.getHasWindCompensations() || rules.getHasGateCompensations(),ui->label_totalCompensationTitle,ui->label_totalCompensation);
        if(!rules.getHas95HSRule())ui->label_beats95HSPercents->hide();
    }
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

JumpWindInfoWidget *JumpDataDetailedInfoWindow::getWindInfoWidget() const
{
    return windInfoWidget;
}

Ui::JumpDataDetailedInfoWindow *JumpDataDetailedInfoWindow::getUi() const
{
    return ui;
}
