#include "Engine.h"

#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
namespace {
void validate(const JumpRequest &input)
{
    input.athlete.validate();input.hill.validate();input.scoring.validate();
    if(!std::isfinite(input.startSeconds) || input.startSeconds<0 || input.startSeconds>1e12-100000
        || !std::isfinite(input.dsqProbability) || input.dsqProbability<0 || input.dsqProbability>1
        || int(input.event)<0 || int(input.event)>3)throw std::invalid_argument("Invalid jump context");
}
double precipitation(const JumpRequest &input,const WeatherField &weather)
{
    const double value=weather.precipitation(input.startSeconds);
    if(!std::isfinite(value) || value<0 || value>10)throw std::invalid_argument("Invalid precipitation supplied by weather field");
    return value;
}
}

Engine::Engine(const EngineParameters &parameters)
    : qualityModel(parameters.quality,HillSkillCurve(parameters.skillAnchors,parameters.curveExtension)),
      flightModel(parameters.flight),landingModel(parameters.landing)
{}

JumpResult Engine::simulate(const JumpRequest &input,RandomSource &rng,const WeatherField &weather,const WindMeasurement &measurement) const
{
    validate(input);JumpResult result;
    if(input.skip) {result.status=JumpStatus::DidNotStart;return result;}
    if(rng.event(input.dsqProbability)) {result.status=JumpStatus::Disqualified;return result;}
    result.quality=qualityModel.sample(input.athlete,input.hill.k,input.hill.takeoffBias,input.event,rng);
    const double precip=precipitation(input,weather);
    const double reserve=flightModel.reserve(result.quality.value(),input.hill,input.scoring.juryGate-input.scoring.coachReduction,
        precip,weather.wind(0,input.startSeconds).head);
    result.flight=flightModel.fly(reserve,input.hill,weather,input.startSeconds,input.captureExposure);
    result.flight.precipitation=precip;
    result.measuredDistance=std::round(result.flight.distance*2)/2;
    result.landingProbabilities=landingModel.probabilities(result.flight.distance,input.hill.realHs,result.flight.meanAbsoluteCrosswind);
    result.landing=landingModel.sample(result.flight.distance,input.hill.realHs,result.flight.meanAbsoluteCrosswind,rng);
    result.judges=landingModel.judges(input.athlete.landing,result.landing,result.flight.distance,input.hill.realHs,rng);
    result.score=scoreJump(result.measuredDistance,result.judges,measurement.measure(weather,result.flight,input.startSeconds),input.hill,input.scoring);
    return result;
}

double Engine::previewDistance(const JumpRequest &input,const WeatherField &weather) const
{
    validate(input);
    const auto quality=qualityModel.baseline(input.athlete,input.hill.k,input.hill.takeoffBias,input.event);
    const double reserve=flightModel.reserve(quality.value(),input.hill,input.scoring.juryGate-input.scoring.coachReduction,
        precipitation(input,weather),weather.wind(0,input.startSeconds).head);
    return flightModel.fly(reserve,input.hill,weather,input.startSeconds,false).distance;
}

}
