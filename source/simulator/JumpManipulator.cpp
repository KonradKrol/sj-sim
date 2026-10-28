#include "JumpManipulator.h"
#include <cmath>
#include <algorithm>

JumpManipulator::JumpManipulator()
{
    distanceRange = inrunSnowRange = takeoffRatingRange = flightRatingRange = averagedWindRange = inrunSnowRange = QPair<double, double>(0, -1);
    distanceBonus = inrunSnowBonus = takeoffRatingBonus = flightRatingBonus = averagedWindBonus = 0;
    averagedWindRangeEnabled = false;
    exactLandingType = exactDSQProbability = -1;
    exactJudges.fill(-1, 5);
}

SimulationV2::JumpManipulation JumpManipulator::simulationInput(bool allowDsq) const
{
    if(simulationManipulation) {
        auto result=*simulationManipulation;if(!allowDsq)result.dsqProbability.reset();return result;
    }
    const auto range=[](QPair<double,double> value,double lo,double hi)->std::optional<SimulationV2::ValueRange> {
        if(value.first<=lo && value.second<0)return std::nullopt;
        return SimulationV2::ValueRange{std::max(lo,value.first),value.second<0?hi:value.second};
    };
    SimulationV2::JumpManipulation result;
    result.distance={distanceBonus,range(distanceRange,0,1000)};
    result.precipitation={inrunSnowBonus,range(inrunSnowRange,0,10)};
    result.measuredWind.bonus=averagedWindBonus;
    if(averagedWindRangeEnabled)result.measuredWind.range=SimulationV2::ValueRange{averagedWindRange.first,averagedWindRange.second};
    if(exactLandingType>=0)result.landing=static_cast<SimulationV2::LandingKind>(exactLandingType);
    for(int i=0;i<std::min(5,int(exactJudges.size()));++i)if(exactJudges[i]>=0)result.judges[std::size_t(i)]=exactJudges[i];
    if(exactDSQProbability>=0 && allowDsq)result.dsqProbability=exactDSQProbability==0?0:1.0/exactDSQProbability;
    return result;
}

double JumpManipulator::getInrunSnowBonus() const
{
    return inrunSnowBonus;
}

void JumpManipulator::setInrunSnowBonus(double newInrunSnowBonus)
{
    inrunSnowBonus = newInrunSnowBonus;
}

QPair<double, double> JumpManipulator::getInrunSnowRange() const
{
    return inrunSnowRange;
}

void JumpManipulator::setInrunSnowRange(const QPair<double, double> &newInrunSnowRange)
{
    inrunSnowRange = newInrunSnowRange;
}

bool JumpManipulator::getAveragedWindRangeEnabled() const
{
    return averagedWindRangeEnabled;
}

void JumpManipulator::setAveragedWindRangeEnabled(bool newAveragedWindRangeEnabled)
{
    averagedWindRangeEnabled = newAveragedWindRangeEnabled;
}

int JumpManipulator::getExactDSQProbability() const
{
    return exactDSQProbability;
}

void JumpManipulator::setExactDSQProbability(int newExactDSQProbability)
{
    exactDSQProbability = newExactDSQProbability;
}

QVector<Wind> JumpManipulator::getExactWinds() const
{
    return exactWinds;
}

void JumpManipulator::setExactWinds(const QVector<Wind> &newExactWinds)
{
    exactWinds = newExactWinds;
}

QVector<double> JumpManipulator::getExactJudges() const
{
    return exactJudges;
}

void JumpManipulator::setExactJudges(const QVector<double> &newExactJudges)
{
    exactJudges.fill(-1,5);
    for(int i=0;i<5 && i<newExactJudges.size();++i)if(std::isfinite(newExactJudges[i]) && newExactJudges[i]>=0)exactJudges[i]=qMin(20.,newExactJudges[i]);
}

int JumpManipulator::getExactLandingType() const
{
    return exactLandingType;
}

void JumpManipulator::setExactLandingType(int newExactLandingType)
{
    exactLandingType = newExactLandingType;
}

double JumpManipulator::getAveragedWindBonus() const
{
    return averagedWindBonus;
}

void JumpManipulator::setAveragedWindBonus(double newAveragedWindBonus)
{
    averagedWindBonus = newAveragedWindBonus;
}

double JumpManipulator::getFlightRatingBonus() const
{
    return flightRatingBonus;
}

void JumpManipulator::setFlightRatingBonus(double newFlightRatingBonus)
{
    flightRatingBonus = newFlightRatingBonus;
}

double JumpManipulator::getTakeoffRatingBonus() const
{
    return takeoffRatingBonus;
}

void JumpManipulator::setTakeoffRatingBonus(double newTakeoffRatingBonus)
{
    takeoffRatingBonus = newTakeoffRatingBonus;
}

double JumpManipulator::getDistanceBonus() const
{
    return distanceBonus;
}

void JumpManipulator::setDistanceBonus(double newDistanceBonus)
{
    distanceBonus = newDistanceBonus;
}

QPair<double, double> JumpManipulator::getAveragedWindRange() const
{
    return averagedWindRange;
}

void JumpManipulator::setAveragedWindRange(const QPair<double, double> &newAveragedWindRange)
{
    averagedWindRange = newAveragedWindRange;
}

QPair<double, double> JumpManipulator::getFlightRatingRange() const
{
    return flightRatingRange;
}

void JumpManipulator::setFlightRatingRange(const QPair<double, double> &newFlightRatingRange)
{
    flightRatingRange = newFlightRatingRange;
}

QPair<double, double> JumpManipulator::getTakeoffRatingRange() const
{
    return takeoffRatingRange;
}

void JumpManipulator::setTakeoffRatingRange(const QPair<double, double> &newTakeoffRatingRange)
{
    takeoffRatingRange = newTakeoffRatingRange;
}

QPair<double, double> JumpManipulator::getDistanceRange() const
{
    return distanceRange;
}

void JumpManipulator::setDistanceRange(const QPair<double, double> &newDistanceRange)
{
    distanceRange = newDistanceRange;
}
