#include "JumpSimulator.h"
#include "../global/GlobalSimulationSettings.h"
#include "../global/MyRandom.h"
#include "../utilities/functions.h"

#include <QDebug>
#include <QRandomGenerator>
#include <math.h>

JumpSimulator::JumpSimulator(Jumper *jumper, const QVector<Wind> &winds, Hill *hill)
    : winds(winds)
    , hill(hill)
{
    this->jumper = jumper;
    simulationData = nullptr;
    if (jumper != nullptr)
        jumperSkills = this->jumper->getJumperSkillsPointer();
    manipulator = nullptr;
    setHasCoachGate(false);
    setCoachGate(0);
    inrunSnow = 0;

    resetTemporaryParameters();
}

double JumpSimulator::getJumpsImportance() const
{
    return jumpsImportance;
}

void JumpSimulator::setJumpsImportance(double newJumpsImportance)
{
    jumpsImportance = newJumpsImportance;
}

double JumpSimulator::getInrunSnow() const
{
    return inrunSnow;
}

void JumpSimulator::setInrunSnow(double newInrunSnow)
{
    inrunSnow = newInrunSnow;
}

int JumpSimulator::getCoachGate() const
{
    return coachGate;
}

void JumpSimulator::setCoachGate(int newCoachGate)
{
    coachGate = newCoachGate;
}

bool JumpSimulator::getHasCoachGate() const
{
    return hasCoachGate;
}

void JumpSimulator::setHasCoachGate(bool newHasCoachGate)
{
    hasCoachGate = newHasCoachGate;
}

bool JumpSimulator::generateDSQ()
{
    if (competitionRules->getHasDsq() == true) {
        int probability = 0;
        if (manipulator->getExactDSQProbability() > (-1))
            probability = manipulator->getExactDSQProbability();
        else {
            probability = getDSQBaseProbability()
                          / (1 + (jumperSkills->getLevelOfCharacteristic("dsq-probability") / 7));
            if (getDSQBaseProbability() == 0)
                return false;
        }

        if(probability == 0)
            probability = 1;
        int random = MyRandom::randomInt(0, probability - 1);
        if (random == probability - 1)
            return true;
    }
    return false;
}

void JumpSimulator::simulateJump()
{
    jumpData = JumpData();
    jumpData.setSimulator(this);
    simulationData = &jumpData.simulationData;

    if (manipulator->getExactWinds().size() > 0) {
        this->setWinds(manipulator->getExactWinds());
        qDebug() << "set WINDS";
    }

    resetTemporaryParameters();
    updateJumperSkills();
    hill->setRealHSByCharacteristic();
    setupJumpData();
    if (generateDSQ() == true) {
        jumpData.setDSQ(true);
        return;
    }
    jumpData.setDSQ(false);

    generateTakeoffRating();
    generateInrunSnowEffect();
    generateFlightRating();
    generateDistance();
    generateWindEffects();

    preventVeryLongJumps();
    jumpData.distance += manipulator->getDistanceBonus();
    if (jumpData.distance < manipulator->getDistanceRange().first)
        jumpData.distance = manipulator->getDistanceRange().first;
    else if (jumpData.distance > manipulator->getDistanceRange().second
             && manipulator->getDistanceRange().second > (-1))
        jumpData.distance = manipulator->getDistanceRange().second;
    if (jumpData.getDistance() < 0)
        jumpData.distance = 0;
    generateLanding();
    generateJudges();
    jumpData.setDistance(roundDoubleToHalf(jumpData.getDistance()));

    calculateCompensations();
    calculatePoints();
}

void JumpSimulator::generateTakeoffRating()
{
    double multiplier = GlobalSimulationSettings::get()->getMaxSkills() / 100;

    double ratingMultiplier = (1 - GlobalSimulationSettings::get()->getTakeoffFormEffect())
                              + 0.1 * hill->getLevelOfCharacteristic("takeoff-technique-effect");
    simulationData->takeoffRating = jumperSkills->getTakeoffTechnique() * ratingMultiplier;

    simulationData->takeoffRating
        += ((jumperSkills->getLevelOfCharacteristic("takeoff-power") * 2 * multiplier)
            * (1 + 0.1 * hill->getLevelOfCharacteristic("takeoff-power-effect")));

    ratingMultiplier = GlobalSimulationSettings::get()->getTakeoffFormEffect() + 0.1 * hill->getLevelOfCharacteristic("takeoff-form-effect");
    simulationData->takeoffRating += jumperSkills->getForm() * ratingMultiplier;

    simulationData->takeoffRating -= std::abs(Hill::calculateBestTakeoffHeightLevel(hill)
                                              - jumper->getJumperSkills().getLevelOfCharacteristic(
                                                  "takeoff-height"))
                                     * 1.3 * multiplier;
    qDebug() << "takeoff level: " << Hill::calculateBestTakeoffHeightLevel(hill);
    qDebug() << "flight level: " << Hill::calculateBestFlightHeightLevel(hill);

    double random = JumpSimulator::getRandomForJumpSimulation(JumpSimulator::TakeoffRating, jumper, jumpsImportance);
    random *= 1 + (0.1 * hill->getLevelOfCharacteristic("takeoff-randomness-effect"));
    simulationData->takeoffRating += random;
    simulationData->metersSumRandom += random * hill->calculateTakeoffEffect();
    simulationData->metersSumRandom = roundDoubleToOnePlace(simulationData->metersSumRandom);

    simulationData->takeoffRating += manipulator->getTakeoffRatingBonus();
    if (simulationData->takeoffRating < manipulator->getTakeoffRatingRange().first)
        simulationData->takeoffRating = manipulator->getTakeoffRatingRange().first;
    else if (simulationData->takeoffRating > manipulator->getTakeoffRatingRange().second
             && manipulator->getTakeoffRatingRange().second > (-1))
        simulationData->takeoffRating = manipulator->getTakeoffRatingRange().second;
}

void JumpSimulator::generateFlightRating()
{
    double multiplier = GlobalSimulationSettings::get()->getMaxSkills() / 100;

    double ratingMultiplier = (1 - GlobalSimulationSettings::get()->getFlightFormEffect())
                              + 0.11 * hill->getLevelOfCharacteristic("flight-technique-effect");
    simulationData->flightRating = jumperSkills->getFlightTechnique() * ratingMultiplier;

    ratingMultiplier = GlobalSimulationSettings::get()->getFlightFormEffect() + 0.1 * hill->getLevelOfCharacteristic("flight-form-effect");
    simulationData->flightRating += jumperSkills->getForm() * ratingMultiplier;

    simulationData->flightRating -= std::abs(
        Hill::calculateBestFlightHeightLevel(hill)
        - jumper->getJumperSkills().getLevelOfCharacteristic("flight-height") * 1.6 * multiplier);

    simulationData->flightRating *= getMultiplierForFlightStyleEffect();
    double random = JumpSimulator::getRandomForJumpSimulation(JumpSimulator::FlightRating, jumper, jumpsImportance);
    random *= 1 + (0.1 * hill->getLevelOfCharacteristic("flight-randomness-effect"));
    simulationData->flightRating += random;
    simulationData->metersSumRandom += random * hill->calculateFlightEffect();
    simulationData->metersSumRandom = roundDoubleToOnePlace(simulationData->metersSumRandom);

    simulationData->flightRating += manipulator->getFlightRatingBonus();
    if (simulationData->flightRating < manipulator->getFlightRatingRange().first)
        simulationData->flightRating = manipulator->getFlightRatingRange().first;
    else if (simulationData->flightRating > manipulator->getFlightRatingRange().second
             && manipulator->getFlightRatingRange().second > (-1))
        simulationData->flightRating = manipulator->getFlightRatingRange().second;
}

double JumpSimulator::getMultiplierForFlightStyleEffect()
{
    switch (jumperSkills->getFlightStyle()) {
    case JumperSkills::FlightStyle::VStyle:
        return 0.972;
    case JumperSkills::FlightStyle::ModernVStyle:
        return 1.0;
    case JumperSkills::FlightStyle::WideVStyle:
        return 1.028;
    case JumperSkills::FlightStyle::HStyle:
        return 1.056;
    }
    return 1.00;
}

void JumpSimulator::generateDistance()
{
    int tempGate = gate;
    if (hasCoachGate)
        tempGate = coachGate;

    qDebug()<<"takeoff effect: "<<hill->calculateTakeoffEffect();
    qDebug()<<"flight effect: "<<hill->calculateFlightEffect();

    jumpData.distance += simulationData->takeoffRating * hill->calculateTakeoffEffect();
    jumpData.distance += simulationData->flightRating * hill->calculateFlightEffect();
    jumpData.distance += tempGate * (hill->getPointsForGate() / hill->getPointsForMeter());
    jumpData.distance = roundDoubleToHalf(jumpData.getDistance());
}

void JumpSimulator::generateWindEffects()
{
    double multiplier = GlobalSimulationSettings::get()->getMaxSkills() / 100;

    double change = 0;
    int i = 0;
    for (const auto &wind : getWinds()) {
        if (wind.getDirection() <= 90 || wind.getDirection() >= 270) //Wiatr przedni
        {
            double windDirectionDeviation = Wind::getAbsForFrontWindDistance(wind.getDirection());
            change = (wind.getStrength() * (1 - (windDirectionDeviation / 90)))
                     * (getWindSegmentDistance() / 22.5);
            change *= 1.00 + (((simulationData->getFlightRating() - 75 * multiplier)) / 350);
            switch (jumperSkills->getFlightStyle()) {
            case JumperSkills::VStyle:
                change *= 0.9;
                break;
            case JumperSkills::ModernVStyle:
                change *= 0.95;
                break;
            case JumperSkills::WideVStyle:
                change *= 1.00;
                break;
            case JumperSkills::HStyle:
                change *= 1.05;
                break;
            }
            change /= 1 + (jumperSkills->getLevelOfCharacteristic("flight-height") / 30);
            change *= 1 + (jumperSkills->getLevelOfCharacteristic("front-wind-use") / 16);
        } else //Wiatr tylny
        {
            double changeAdditionFromFrontWide = abs(
                (wind.getStrength()
                 * (1 - (Wind::getAbsForFrontWindDistance(wind.getDirection()) / 90)))
                * (getWindSegmentDistance() / 22.5));
            double windDirectionDeviation = abs(180 - wind.getDirection());
            change = changeAdditionFromFrontWide
                     + ((wind.getStrength() * (1 - (windDirectionDeviation / 90)))
                        * (getWindSegmentDistance() / 13.5));
            change /= 1.00 + (((simulationData->getFlightRating() - 75 * multiplier)) / 450);
            switch (jumperSkills->getFlightStyle()) {
            case JumperSkills::VStyle:
                change *= 0.96;
                break;
            case JumperSkills::ModernVStyle:
                change *= 0.99;
                break;
            case JumperSkills::WideVStyle:
                change *= 1.02;
                break;
            case JumperSkills::HStyle:
                change *= 1.05;
                break;
            }
            change /= 1 + (jumperSkills->getLevelOfCharacteristic("flight-height") / 45);
            change /= 1 + (jumperSkills->getLevelOfCharacteristic("back-wind-protection") / 16);
        }
        if (i != getWinds().count()) {
            if (jumpData.getDistance() < (i + 1) * getWindSegmentDistance()) {
                double percent = 1;
                if (i > 0)
                    percent = ((jumpData.getDistance())  - (double(getWindSegmentDistance() * (i + 0)))) / getWindSegmentDistance();
                if (percent > 1)
                    percent = 1;
                else if (percent < 0)
                    percent = 0;
                qDebug()<<QString("percent %1 at distance %2").arg(QString::number(percent)).arg(QString::number(jumpData.getDistance()));
                change *= percent;
            }
        }
        if (wind.getDirection() <= 90 || wind.getDirection() >= 270) {
            jumpData.distance += change;
        } else {
            jumpData.distance -= change;
        }
        qDebug()<<"distance after wind change: "<<jumpData.distance;
        i++;
    }
}

void JumpSimulator::generateInrunSnowEffect()
{
    simulationData->inrunSnow += manipulator->getInrunSnowBonus();
    if (simulationData->inrunSnow < manipulator->getInrunSnowRange().first)
        simulationData->inrunSnow = manipulator->getInrunSnowRange().first;
    if (simulationData->inrunSnow > manipulator->getInrunSnowRange().second
        && manipulator->getInrunSnowRange().second > (-1))
        simulationData->inrunSnow = manipulator->getInrunSnowRange().second;

    if (simulationData->inrunSnow > 10)
        simulationData->inrunSnow = 10;
    else if (simulationData->inrunSnow < 0)
        simulationData->inrunSnow = 0;
    qDebug() << "inrunSnow -------> " << inrunSnow;
    qDebug() << " simulationData->takeoffRating: " << simulationData->takeoffRating;
    simulationData->takeoffRating /= (1 + (simulationData->inrunSnow / 18));
    qDebug() << " simulationData->takeoffRating: " << simulationData->takeoffRating;
}

void JumpSimulator::preventVeryLongJumps()
{
    if (jumpData.getDistance() > hill->getRealHS()) {
        double distanceAfterRealHS = jumpData.getDistance() - hill->getRealHS();
        double substraction = MyRandom::lognormalDistributionRandom(distanceAfterRealHS / 19,
                                                                    distanceAfterRealHS / 41);
        jumpData.distance -= substraction;
        if (jumpData.distance < hill->getRealHS() * 0.992)
            jumpData.distance = hill->getRealHS() * 0.992;
    }
}

void JumpSimulator::generateLanding()
{
    QVector<double> probabilities;
    probabilities.fill(0, 4);
    probabilities[0] = getLandingChance(Landing::TelemarkLanding,
                                        jumpData.getDistance(),
                                        hill,
                                        jumper);
    probabilities[1] = getLandingChance(Landing::BothLegsLanding,
                                        jumpData.getDistance(),
                                        hill,
                                        jumper);
    probabilities[2] = getLandingChance(Landing::SupportLanding,
                                        jumpData.getDistance(),
                                        hill,
                                        jumper);
    probabilities[3] = getLandingChance(Landing::Fall, jumpData.getDistance(), hill, jumper);

    qDebug() << "Szansa na telemark: " << probabilities[0];
    qDebug() << "Szansa na lądowanie na dwie nogi: " << probabilities[1];
    qDebug() << "Szansa na podpórkę: " << probabilities[2];
    qDebug() << "Szansa na upadek: " << probabilities[3];

    double drawSum = 0;
    for (const auto &probability : probabilities)
        drawSum += probability;

    double drawRandom = MyRandom::randomDouble(0, drawSum);
    double actualSum = 0;

    int i = 0;
    for (const auto &probability : probabilities) {
        actualSum += probability;
        if (drawRandom < actualSum) {
            jumpData.landing.setType(i);
            break;
        }
        i++;
    }

    if (manipulator->getExactLandingType() > (-1))
        jumpData.landing.setType(manipulator->getExactLandingType());

    double landingRating = MyRandom::normalDistributionRandomHalf(1.2 - ((jumperSkills->getLandingStyle() - 10) / 7) + hill->getLandingImbalanceChangeByHillProfile(jumpData.distance) / 3, 0.76 + hill->getLandingImbalanceChangeByHillProfile(jumpData.distance) / 1.7, MyRandom::Positive);

    if (landingRating < 0)
        landingRating = 0;
    else if (landingRating > 5)
        landingRating = 5;
}

void JumpSimulator::generateJudges()
{
    if (competitionRules->getHasJudgesPoints() == true) {
        double judgesRating = 17.15;
        qDebug()<<"JudgesRating at start: "<<judgesRating;
        judgesRating += ((jumperSkills->getLandingStyle() - 12) / 5);
        qDebug()<<"JudgesRating after landingStyle: "<<judgesRating;
        judgesRating *= 1 + ((jumpData.distance / hill->getKPoint() - 1) / 1.9);
        qDebug()<<"JudgesRating after distance: "<<judgesRating;
        double base = 0 + (((jumpData.distance / hill->getHSPoint()) - 1) * 15);
        double dev = 0 + (((jumpData.distance / hill->getHSPoint()) - 1) * 12);
        qDebug()<<"Base "<<base<<", dev "<<dev;
        if(dev > 0 && base >= 0)
            judgesRating -= MyRandom::normalDistributionRandomHalf(base, dev, MyRandom::Positive);
        qDebug()<<"JudgesRating after long distance random: "<<judgesRating;
        switch(jumpData.landing.getType())
        {
        case Landing::TelemarkLanding:
            judgesRating += MyRandom::normalDistributionRandom(0, 0.4);
            break;
        case Landing::BothLegsLanding:
            judgesRating += MyRandom::normalDistributionRandom(-2.5, 0.5);
            break;
        case Landing::SupportLanding:
            judgesRating += MyRandom::normalDistributionRandom(-7, 0.7);
            break;
        case Landing::Fall:
            judgesRating += MyRandom::normalDistributionRandom(-9, 0.7);
            break;
        default:
            break;
        }
        qDebug()<<"JudgesRating after random: "<<judgesRating;

        if (judgesRating > 20)
            judgesRating = 20;
        else if (judgesRating < 1)
            judgesRating = 1;

        if (jumpData.judges.size() != 5)
            jumpData.judges.fill(0, 5);

        for(auto & jg : jumpData.judges)
        {
            jg = judgesRating;
            switch(jumpData.landing.getType())
            {
            case Landing::TelemarkLanding:
                jg += MyRandom::normalDistributionRandom(0, 0.24);
                break;
            case Landing::BothLegsLanding:
                jg += MyRandom::normalDistributionRandom(0, 0.4);
                break;
            case Landing::SupportLanding:
                jg += MyRandom::normalDistributionRandom(0, 0.8);
                break;
            case Landing::Fall:
                jg += MyRandom::normalDistributionRandom(0, 0.7);
                break;
            default:
                break;
            }
            jg = roundDoubleToHalf(jg);
            if(jg > 20)
                jg = 20;
            else if(jg < 0)
                jg = 0;
        }
    } else {
        jumpData.judges.fill(0, 5);
    }
    int i = 0;
    for (auto &jg : jumpData.judges) {
        if (manipulator->getExactJudges().at(i) > (-1)) {
            jg = manipulator->getExactJudges().at(i);
        }
        i++;
    }
}

void JumpSimulator::calculateCompensations()
{
    jumpData.setWindCompensation(0);
    if (competitionRules->getHasWindCompensations() == true) {
        QVector<Wind> tempWinds = getWinds();
        switch (competitionRules->getWindCompensationDistanceEffect()) //
        {
        case WindCompensationDistanceEffect::Original:
            if (jumpData.getDistance() <= (0.75 * hill->getKPoint())) {
                int howManyToRemove = winds.count()
                                      - std::round((0.75 * hill->getKPoint())
                                                   / getWindSegmentDistance());
                for (int i = 0; i < howManyToRemove; i++)
                    winds.remove(winds.count() - 1);
            }
            break;
        case WindCompensationDistanceEffect::Full:
            for (int i = 0; i < tempWinds.count(); i++) {
                if (jumpData.getDistance() < (i + 1) * getWindSegmentDistance()) {
                    double percent = double(jumpData.getDistance() - (getWindSegmentDistance() * i))
                                     / ((i + 1) * getWindSegmentDistance());
                    if (percent > 1)
                        percent = 1;
                    else if (percent < 0)
                        percent = 0;
                    winds[i] = Wind(winds[i].getDirection(), winds[i].getStrength() * percent);
                }
            }
        }
        tempWinds = winds;

        Wind avgWind
            = WindsCalculator::getAveragedWind(tempWinds,
                                               competitionRules->getWindAverageCalculatingType());

        double avgConvertedStrength = avgWind.getStrengthToAveragedWind();
        avgConvertedStrength += manipulator->getAveragedWindBonus();
        if (manipulator->getAveragedWindRangeEnabled() == true) {
            if (avgConvertedStrength < manipulator->getAveragedWindRange().first)
                avgConvertedStrength = (manipulator->getAveragedWindRange().first);
            else if (avgConvertedStrength > manipulator->getAveragedWindRange().second)
                avgConvertedStrength = (manipulator->getAveragedWindRange().second);
        }

        jumpData.setAveragedWind(avgConvertedStrength);

        avgWind.setStrength(abs(avgConvertedStrength));
        if (avgConvertedStrength > 0)
            avgWind.setDirection(0);
        else if (avgConvertedStrength < 0)
            avgWind.setDirection(180);
        double windCompensation = WindsCalculator::getWindCompensation(avgWind, hill);
        jumpData.setWindCompensation(windCompensation);
    }

    jumpData.setGateCompensation(0);
    jumpData.setBeats95HSPercents(false);
    if (competitionRules->getHasGateCompensations() == true) {
        jumpData.setGateCompensation(
            WindsCalculator::getGateCompensation(competitionStartGate, gate, hill));
        double coachGateCompensation = 0;
        if (hasCoachGate == true) {
            if (JumpSimulator::isJumperBeat95HSPercents(jumpData.distance, hill->getHSPoint())) {
                jumpData.setBeats95HSPercents(true);
                coachGateCompensation = WindsCalculator::getGateCompensation(gate, coachGate, hill);
            }
        }
        jumpData.setGateCompensation(jumpData.getGateCompensation() + coachGateCompensation);
        jumpData.setGateCompensation(roundDoubleToOnePlace(jumpData.getGateCompensation()));
    }

    jumpData.setTotalCompensation(jumpData.getWindCompensation() + jumpData.getGateCompensation());
    jumpData.setTotalCompensation(roundDoubleToOnePlace(jumpData.getTotalCompensation()));
}

void JumpSimulator::calculatePoints()
{
    jumpData.points = hill->getPointsForKPoint();
    jumpData.points += (roundDoubleToHalf(jumpData.getDistance()) - hill->getKPoint())
                       * hill->getPointsForMeter();
    jumpData.points += jumpData.getGateCompensation();
    jumpData.points += jumpData.getWindCompensation();

    jumpData.judgesPoints = 0;
    if (competitionRules->getHasJudgesPoints()) {
        double min = 20, max = 0;
        for (const auto jg : jumpData.getJudges()) {
            if (jg > max)
                max = jg;
            else if (jg < min)
                min = jg;
            jumpData.judgesPoints += jg;
        }
        jumpData.judgesPoints -= min;
        jumpData.judgesPoints -= max;
    }

    jumpData.points += jumpData.getJudgesPoints();

    jumpData.points = roundDoubleToOnePlace(jumpData.getPoints());
    if (jumpData.points < 0)
        jumpData.points = 0;
}

void JumpSimulator::setupJumpData()
{
    jumpData.reset();
    jumpData.jumper = getJumper();
    jumpData.hill = getHill();
    jumpData.simulator = this;
    jumpData.winds = winds;
    //sredni wiatr jest zapisywany w jumpDacie w funkcji calculateCompensations().
    jumpData.setGate(this->gate);
    competitionRules = getCompetitionRules();
    jumpData.hasCoachGate = this->hasCoachGate;
    jumpData.coachGate = this->coachGate;
    if (hasCoachGate == true)
        jumpData.setGate(this->coachGate);
    simulationData->inrunSnow = inrunSnow;

    jumpData.jumperForm = jumper->getJumperSkills().getForm();
}

JumpSimulationData *JumpSimulator::getSimulationData()
{
    return simulationData;
}

void JumpSimulator::setSimulationData(JumpSimulationData *newSimulationData)
{
    simulationData = newSimulationData;
}

double JumpSimulator::getWindSegmentDistance()
{
    return (hill->getKPoint() + (hill->getKPoint() / getWinds().count())) / getWinds().count();
}

void JumpSimulator::setJumpData(const JumpData &newJumpData)
{
    jumpData = newJumpData;
}

void JumpSimulator::resetTemporaryParameters()
{
    if (simulationData != nullptr) {
        simulationData->reset();
    }
    jumpData.reset();
}

JumpData JumpSimulator::getJumpData()
{
    return jumpData;
}

Hill *JumpSimulator::getHill() const
{
    return hill;
}

void JumpSimulator::setHill(Hill *newHill)
{
    hill = newHill;
}

Jumper *JumpSimulator::getJumper() const
{
    return jumper;
}

void JumpSimulator::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
    updateJumperSkills();
}

int JumpSimulator::getGate() const
{
    return gate;
}

void JumpSimulator::setGate(int newGate)
{
    gate = newGate;
}

JumperSkills *JumpSimulator::getJumperSkills() const
{
    return jumperSkills;
}

void JumpSimulator::updateJumperSkills()
{
    if (jumper != nullptr)
        jumperSkills = jumper->getJumperSkillsPointer();
}

void JumpSimulator::setupForNextJump()
{
    jumpData.setupForNextJumper();
}

double JumpSimulator::getDSQBaseProbability() const
{
    return DSQBaseProbability;
}

void JumpSimulator::setDSQBaseProbability(double newDSQBaseProbability)
{
    DSQBaseProbability = newDSQBaseProbability;
}

double JumpSimulator::getRandomForJumpSimulation(short parameter, Jumper *jumper, double jumpsImportance)
{
    GlobalSimulationSettings *s = GlobalSimulationSettings::get();
    JumperSkills *skills = jumper->getJumperSkillsPointer();
    double randomMultiplier = s->getSimulationRandomMultiplier();
    double maxSkillsMultiplier = GlobalSimulationSettings::get()->getMaxSkills() / 100;
    switch (parameter) {
    case JumpSimulator::TakeoffRating: {
        double base = 0;
        double dev = 0;
        double random = 0;
        dev = 4;
        double newDev = dev * 0.6;
        newDev /= 1 + ((skills->getForm() - 50) / 30 * 0.13);
        newDev /= 1 + (0.13 * double(skills->getJumpsEquality()) / 2.4);
        newDev /= 1 + (0.05 * skills->getLevelOfCharacteristic("takeoff-height"));
        newDev *= 1 + (0.915 * (10 - jumpsImportance)) * (1 - (skills->getLevelOfCharacteristic("less-important-jumps") / 5));


        double addition = ((dev / newDev) - 1);
        qDebug() << "addition: " << addition;
        if (MyRandom::randomDouble(0, 2 + addition) <= 1) {
            random = MyRandom::normalDistributionRandomHalf(base, dev, MyRandom::Positive);
        } else {
            dev = newDev;
            qDebug()<<"devt: "<<dev;
            random = MyRandom::normalDistributionRandomHalf(base, dev, MyRandom::Negative);
        }

        random *= randomMultiplier;
        random *= maxSkillsMultiplier;
        qDebug() << "takeoffRandom: " << random;
        return random;
    }
    case JumpSimulator::FlightRating: {
        double base = 0;
        double dev = 0;
        double random = 0;
        dev = 4;
        switch (skills->getFlightStyle()) {
        case JumperSkills::VStyle:
            dev -= 0.22;
            break;
        case JumperSkills::ModernVStyle:
            dev -= 0.11;
            break;
        case JumperSkills::WideVStyle:
            dev += 0.11;
            break;
        case JumperSkills::HStyle:
            dev += 0.22;
            break;
        }
        double newDev = dev * 0.6;

        qDebug()<<"dev: "<<newDev;
        newDev /= 1 + ((skills->getForm() - 50) / 30 * 0.13);
        qDebug()<<"dev after equality: "<<newDev;
        newDev /= 1 + (0.13 * double(skills->getJumpsEquality()) / 2.4);
        qDebug()<<"dev after flight height: "<<newDev;
        newDev /= 1 + (0.05 * skills->getLevelOfCharacteristic("flight-height"));
        qDebug()<<"dev after form"<<newDev;
        newDev *= 1 + (0.915 * (10 - jumpsImportance)) * (1 - (skills->getLevelOfCharacteristic("less-important-jumps") / 4));
        qDebug()<<"dev after jumps jumps importance"<<newDev;

        double addition = ((dev / newDev) - 1);
        if (MyRandom::randomDouble(0, 2 + addition) <= 1) {
            random = MyRandom::normalDistributionRandomHalf(base, dev, MyRandom::Positive);
        } else {
            dev = newDev;
            qDebug()<<"devf: "<<dev;
            random = MyRandom::normalDistributionRandomHalf(base, dev, MyRandom::Negative);
        }

        random *= randomMultiplier;
        random *= maxSkillsMultiplier;
        qDebug() << "flightRandom: " << random;
        return random;
    }
    }

    return 0;
}

double JumpSimulator::getLandingChance(short landingType,
                                       double distance,
                                       Hill *hill,
                                       Jumper *jumper)
{
    int realHS = hill->getRealHS();
    double chance = 0;
    switch (landingType) {
    case Landing::TelemarkLanding:
        if (distance < realHS * 0.96) //128.5 dla HS134
            chance = 99.8;
        else if (distance < realHS * 0.965) //129 dla HS134
            chance = 99.65;
        else if (distance < realHS * 0.97) //130 dla HS134
            chance = 99.1;
        else if (distance < realHS * 0.975) //130.5 dla HS134
            chance = 98.65;
        else if (distance < realHS * 0.98) //131.5 dla HS134
            chance = 98.35;
        else if (distance < realHS * 0.985) //132 dla HS134
            chance = 98.2;
        else if (distance < realHS * 0.99) //132.5 dla HS134
            chance = 97.6;
        else if (distance < realHS * 0.995) //132.5 dla HS134
            chance = 97.2;
        else if (distance < realHS * 1.00) //134 dla HS134
            chance = 96.15;
        else if (distance < realHS * 1.005) //134 dla HS134
            chance = 94.65;
        else if (distance < realHS * 1.01) //135.5 dla HS134
            chance = 91;
        else if (distance < realHS * 1.015) //135.5 dla HS134
            chance = 87;
        else if (distance < realHS * 1.02) //136.5 dla HS134
            chance = 81;
        else if (distance < realHS * 1.025) //136.5 dla HS134
            chance = 76;
        else if (distance < realHS * 1.03) //138 dla HS134
            chance = 70;
        else if (distance < realHS * 1.035) //138 dla HS134
            chance = 62.5;
        else if (distance < realHS * 1.04) //139.5 dla HS134
            chance = 52;
        else if (distance < realHS * 1.045) //139.5 dla HS134
            chance = 40;
        else if (distance < realHS * 1.05) //141 dla HS134
            chance = 25;
        else if (distance < realHS * 1.055) //141 dla HS134
            chance = 14;
        else if (distance < realHS * 1.06) //142 dla HS134
            chance = 8;
        else if (distance < realHS * 1.065) //142 dla HS134
            chance = 6;
        else if (distance < realHS * 1.07) //143.5 dla HS134
            chance = 3;
        else if (distance < realHS * 1.075) //143.5 dla HS134
            chance = 2;
        else if (distance < realHS * 1.08) //145 dla HS134
            chance = 1;
        else if (distance < realHS * 1.085) //145 dla HS134
            chance = 0.5;
        else if (distance < realHS * 1.09) //146 dla HS134
            chance = 0.25;
        else //147.5 i więcej dla HS134
            chance = 0.025;
        break;
    case Landing::BothLegsLanding:
        if (distance < realHS * 0.96) //128.5 dla HS134
            chance = 0.1;
        else if (distance < realHS * 0.965) //129 dla HS134
            chance = 0.16;
        else if (distance < realHS * 0.97) //130 dla HS134
            chance = 0.3;
        else if (distance < realHS * 0.975) //130.5 dla HS134
            chance = 0.5;
        else if (distance < realHS * 0.98) //131.5 dla HS134
            chance = 0.7;
        else if (distance < realHS * 0.985) //132 dla HS134
            chance = 1.1;
        else if (distance < realHS * 0.99) //132.5 dla HS134
            chance = 1.5;
        else if (distance < realHS * 0.995) //132.5 dla HS134
            chance = 2.1;
        else if (distance < realHS * 1.00) //134 dla HS134
            chance = 3;
        else if (distance < realHS * 1.005) //134 dla HS134
            chance = 5;
        else if (distance < realHS * 1.01) //135.5 dla HS134
            chance = 8;
        else if (distance < realHS * 1.015) //135.5 dla HS134
            chance = 12;
        else if (distance < realHS * 1.02) //136.5 dla HS134
            chance = 16;
        else if (distance < realHS * 1.025) //136.5 dla HS134
            chance = 21;
        else if (distance < realHS * 1.03) //138 dla HS134
            chance = 26.5;
        else if (distance < realHS * 1.035) //138 dla HS134
            chance = 34.5;
        else if (distance < realHS * 1.04) //139.5 dla HS134
            chance = 45;
        else if (distance < realHS * 1.045) //139.5 dla HS134
            chance = 57;
        else if (distance < realHS * 1.05) //141 dla HS134
            chance = 71;
        else if (distance < realHS * 1.055) //141 dla HS134
            chance = 85.5;
        else if (distance < realHS * 1.06) //142 dla HS134
            chance = 104;
        else if (distance < realHS * 1.065) //142 dla HS134
            chance = 130;
        else if (distance < realHS * 1.07) //143.5 dla HS134
            chance = 165;
        else if (distance < realHS * 1.075) //143.5 dla HS134
            chance = 190;
        else if (distance < realHS * 1.08) //145 dla HS134
            chance = 249;
        else if (distance < realHS * 1.085) //145 dla HS134
            chance = 295;
        else if (distance < realHS * 1.09) //146 dla HS134
            chance = 360;
        else //147.5 i więcej dla HS134
            chance = 500;
        break;
    case Landing::Fall:
        if (distance < realHS * 0.96) //128.5 dla HS134
            chance = 0.013;
        else if (distance < realHS * 0.965) //129 dla HS134
            chance = 0.019;
        else if (distance < realHS * 0.97) //130 dla HS134
            chance = 0.03;
        else if (distance < realHS * 0.975) //130.5 dla HS134
            chance = 0.04;
        else if (distance < realHS * 0.98) //131.5 dla HS134
            chance = 0.05;
        else if (distance < realHS * 0.985) //132 dla HS134
            chance = 0.065;
        else if (distance < realHS * 0.99) //132.5 dla HS134
            chance = 0.085;
        else if (distance < realHS * 0.995) //132.5 dla HS134
            chance = 0.11;
        else if (distance < realHS * 1.00) //134 dla HS134
            chance = 0.13;
        else if (distance < realHS * 1.005) //134 dla HS134
            chance = 0.16;
        else if (distance < realHS * 1.01) //135.5 dla HS134
            chance = 0.20;
        else if (distance < realHS * 1.015) //135.5 dla HS134
            chance = 0.3;
        else if (distance < realHS * 1.02) //136.5 dla HS134
            chance = 0.45;
        else if (distance < realHS * 1.025) //136.5 dla HS134
            chance = 0.68;
        else if (distance < realHS * 1.03) //138 dla HS134
            chance = 1.25;
        else if (distance < realHS * 1.035) //138 dla HS134
            chance = 2;
        else if (distance < realHS * 1.04) //139.5 dla HS134
            chance = 3.5;
        else if (distance < realHS * 1.045) //139.5 dla HS134
            chance = 5.65;
        else if (distance < realHS * 1.05) //141 dla HS134
            chance = 8.5;
        else if (distance < realHS * 1.055) //141 dla HS134
            chance = 15;
        else if (distance < realHS * 1.06) //142 dla HS134
            chance = 25;
        else if (distance < realHS * 1.065) //142 dla HS134
            chance = 48;
        else if (distance < realHS * 1.07) //143.5 dla HS134
            chance = 80;
        else if (distance < realHS * 1.075) //143.5 dla HS134
            chance = 140;
        else if (distance < realHS * 1.08) //145 dla HS134
            chance = 230;
        else if (distance < realHS * 1.085) //145 dla HS134
            chance = 360;
        else if (distance < realHS * 1.09) //146 dla HS134
            chance = 490;
        else if (distance < realHS * 1.095) //146 dla HS134
            chance = 600;
        else if (distance < realHS * 1.10) //146 dla HS134
            chance = 850;
        else if (distance < realHS * 1.105) //146 dla HS134
            chance = 1200;
        else if (distance < realHS * 1.11) //146 dla HS134
            chance = 1730;
        else if (distance < realHS * 1.12) //146 dla HS134
            chance = 2300;
        else if (distance < realHS * 1.125) //146 dla HS134
            chance = 4500;
        else if (distance < realHS * 1.13) //146 dla HS134
            chance = 8000;
        else //147.5 i więcej dla HS134
            chance = 16000;
        break;
    case Landing::SupportLanding:
        if (distance < realHS * 0.96) //128.5 dla HS134
            chance = 0.003;
        else if (distance < realHS * 0.965) //129 dla HS134
            chance = 0.006;
        else if (distance < realHS * 0.97) //130 dla HS134
            chance = 0.01;
        else if (distance < realHS * 0.975) //130.5 dla HS134
            chance = 0.015;
        else if (distance < realHS * 0.98) //131.5 dla HS134
            chance = 0.0215;
        else if (distance < realHS * 0.985) //132 dla HS134
            chance = 0.035;
        else if (distance < realHS * 0.99) //132.5 dla HS134
            chance = 0.048;
        else if (distance < realHS * 0.995) //132.5 dla HS134
            chance = 0.08;
        else if (distance < realHS * 1.00) //134 dla HS134
            chance = 0.15;
        else if (distance < realHS * 1.005) //134 dla HS134
            chance = 0.185;
        else if (distance < realHS * 1.01) //135.5 dla HS134
            chance = 0.25;
        else if (distance < realHS * 1.015) //135.5 dla HS134
            chance = 0.35;
        else if (distance < realHS * 1.02) //136.5 dla HS134
            chance = 0.51;
        else if (distance < realHS * 1.025) //136.5 dla HS134
            chance = 0.8;
        else if (distance < realHS * 1.03) //138 dla HS134
            chance = 1.3;
        else if (distance < realHS * 1.035) //138 dla HS134
            chance = 2.8;
        else if (distance < realHS * 1.04) //139.5 dla HS134
            chance = 4.9;
        else if (distance < realHS * 1.045) //139.5 dla HS134
            chance = 6.7;
        else if (distance < realHS * 1.05) //141 dla HS134
            chance = 8.25;
        else if (distance < realHS * 1.055) //141 dla HS134
            chance = 12;
        else if (distance < realHS * 1.06) //142 dla HS134
            chance = 17;
        else if (distance < realHS * 1.065) //142 dla HS134
            chance = 25;
        else if (distance < realHS * 1.07) //143.5 dla HS134
            chance = 31.5;
        else if (distance < realHS * 1.075) //143.5 dla HS134
            chance = 40;
        else if (distance < realHS * 1.08) //145 dla HS134
            chance = 50;
        else if (distance < realHS * 1.085) //145 dla HS134
            chance = 60;
        else if (distance < realHS * 1.09) //146 dla HS134
            chance = 70;
        /*else if(distance < realHS * 1.095) //146 dla HS134
            chance = 600;
        else if(distance < realHS * 1.10) //146 dla HS134
            chance = 850;
        else if(distance < realHS * 1.105) //146 dla HS134
            chance = 1200;
        else if(distance < realHS * 1.11) //146 dla HS134
            chance = 1730;
        else if(distance < realHS * 1.12) //146 dla HS134
            chance = 2300;
        else if(distance < realHS * 1.125) //146 dla HS134
            chance = 4500;
        else if(distance < realHS * 1.13) //146 dla HS134
            chance = 8000;*/
        else //147.5 i więcej dla HS134
            chance = 100;
        break;
    }

    //tendencja do lądowania na dwie nogi (np. Ammann)
    if (landingType == Landing::BothLegsLanding) {
        chance *= (1
                   + (jumper->getJumperSkills().getLevelOfCharacteristic(
                       "both-legs-landing-tendence")));
    }

    //Umiejętność lądowania
    if (landingType == Landing::Fall) {
        chance /= (1 + (jumper->getJumperSkills().getLevelOfCharacteristic("landing-skill") / 4.5));
    } else if (landingType == Landing::SupportLanding) {
        chance /= (1 + (jumper->getJumperSkills().getLevelOfCharacteristic("landing-skill") / 6));
    }

    return chance;
}

bool JumpSimulator::isJumperBeat95HSPercents(double distance, double HSPoint)
{
    if (distance >= 0.95 * HSPoint)
        return true;
    else
        return false;
}

CompetitionRules *JumpSimulator::getCompetitionRules() const
{
    return competitionRules;
}

void JumpSimulator::setCompetitionRules(CompetitionRules *newCompetitionRules)
{
    competitionRules = newCompetitionRules;
}

JumpManipulator *JumpSimulator::getManipulator() const
{
    return manipulator;
}

void JumpSimulator::setManipulator(JumpManipulator *newManipulator)
{
    manipulator = newManipulator;
}
QVector<Wind> JumpSimulator::getWinds() const
{
    return winds;
}

void JumpSimulator::setWinds(const QVector<Wind> &newWinds)
{
    winds = newWinds;
}

int JumpSimulator::getCompetitionStartGate() const
{
    return competitionStartGate;
}

void JumpSimulator::setCompetitionStartGate(int newCompetitionStartGate)
{
    competitionStartGate = newCompetitionStartGate;
}
