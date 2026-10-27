#include "Scoring.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace SimulationV2 {
namespace {
double decimal(double v) { return std::round(v*10)/10; }
}

SensorWindMeasurement::SensorWindMeasurement(std::vector<WindSensor> stations,int samples,double beforeTakeoff,double afterLanding,bool onlyReached)
    : sensors(std::move(stations)),temporalSamples(samples),before(beforeTakeoff),after(afterLanding),reachedOnly(onlyReached)
{
    if(sensors.empty() || sensors.size()>1000 || samples<1 || samples>1000
        || !std::isfinite(before) || before<0 || before>1000 || !std::isfinite(after) || after<0 || after>1000)
        throw std::invalid_argument("Invalid wind measurement layout/window");
    double weights=0;
    for(const auto &sensor:sensors) {
        if(!std::isfinite(sensor.distance) || sensor.distance<0 || sensor.distance>1000000
            || !std::isfinite(sensor.weight) || sensor.weight<0 || sensor.weight>1000)
            throw std::invalid_argument("Invalid wind measurement sensor");
        weights+=sensor.weight;
    }
    if(weights<=0)throw std::invalid_argument("Wind measurement needs positive sensor weights");
}

double SensorWindMeasurement::measure(const WeatherField &weather,const FlightResult &flight,double startSeconds) const
{
    if(!std::isfinite(startSeconds) || startSeconds<0 || startSeconds>1e12-100000
        || !std::isfinite(flight.distance) || flight.distance<0 || flight.distance>1000
        || !std::isfinite(flight.duration) || flight.duration<0 || flight.duration>1000)
        throw std::invalid_argument("Invalid wind measurement interval");
    const double begin=std::max(0.0,startSeconds-before), end=startSeconds+flight.duration+after;
    double total=0,weights=0;
    for(const auto &sensor:sensors) {
        if(sensor.weight==0 || (reachedOnly && sensor.distance>flight.distance))continue;
        double timeAverage=0;
        for(int i=0;i<temporalSamples;++i) {
            const double time=begin+(end-begin)*(double(i)+.5)/temporalSamples;
            const double value=weather.wind(sensor.distance,time).head;
            if(!std::isfinite(value) || std::abs(value)>100)throw std::invalid_argument("Invalid wind sensor reading");
            timeAverage+=value/temporalSamples;
        }
        total+=timeAverage*sensor.weight;weights+=sensor.weight;
    }
    return weights>0 ? total/weights : 0;
}

void ScoringRules::validate() const
{
    if(juryGate<0 || juryGate>200 || referenceGate<0 || referenceGate>200 || coachReduction<0 || coachReduction>juryGate)
        throw std::invalid_argument("Invalid jury/reference/coach gate");
}

Score scoreJump(double distance,const std::array<double,5> &marks,double measuredWind,const Hill &hill,const ScoringRules &rules)
{
    hill.validate();rules.validate();
    if(!std::isfinite(distance) || distance<0 || distance>1000 || !std::isfinite(measuredWind) || std::abs(measuredWind)>100)
        throw std::invalid_argument("Invalid scored distance/wind");
    for(double mark:marks)if(!std::isfinite(mark) || mark<0 || mark>20)throw std::invalid_argument("Invalid judge mark");
    Score score;score.measuredWind=std::round(measuredWind*100)/100;
    score.coachThresholdReached=distance+1e-9>=.95*hill.hs;
    score.distance=decimal(hill.pointsAtK+(distance-hill.k)*hill.pointsPerMetre);
    if(rules.judges) {
        auto sorted=marks;std::sort(sorted.begin(),sorted.end());
        score.judges=sorted[1]+sorted[2]+sorted[3];
    }
    if(rules.windCompensation)score.wind=decimal(-score.measuredWind*(score.measuredWind>=0?hill.headwindPoints:hill.tailwindPoints));
    if(rules.gateCompensation) {
        const int coachCredit=(!rules.coach95PercentRule || score.coachThresholdReached)?rules.coachReduction:0;
        score.gate=decimal((rules.referenceGate-rules.juryGate+coachCredit)*hill.gatePoints);
    }
    score.total=std::max(0.0,decimal(score.distance+score.judges+score.wind+score.gate));
    return score;
}

}
