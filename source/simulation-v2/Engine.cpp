#include "Engine.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
namespace {
void validate(const JumpRequest &input)
{
    input.athlete.validate();input.hill.validate();input.scoring.validate();input.manipulation.validate();
    if(!std::isfinite(input.startSeconds) || input.startSeconds<0 || input.startSeconds>1e12-100000
        || !std::isfinite(input.dsqProbability) || input.dsqProbability<0 || input.dsqProbability>1
        || int(input.event)<0 || int(input.event)>3)throw std::invalid_argument("Invalid jump context");
}
double precipitation(const JumpRequest &input,const WeatherField &weather)
{
    const double value=weather.precipitation(input.startSeconds);
    if(!std::isfinite(value) || value<0 || value>10)throw std::invalid_argument("Invalid precipitation supplied by weather field");
    return std::clamp(input.manipulation.precipitation.apply(value),0.0,10.0);
}

FlightResult manipulatedFlight(const JumpRequest &input,const FlightModel &model,const WeatherField &weather,double reserve,bool &limited,WindResponseSample response={})
{
    auto flight=model.fly(reserve,input.hill,weather,input.startSeconds,input.captureExposure,response);
    if(!input.manipulation.distance.active())return flight;
    const double desired=input.manipulation.distance.apply(flight.distance);
    // A requested result is realised by adjusting starting reserve and flying
    // again, including all newly reached wind. Never attach a false wind trace
    // to a distance simply overwritten after the natural flight calculation.
    const double limit=model.landingLimit(input.hill);
    const double target=std::clamp(desired,0.0,limit);
    limited=target!=desired;
    if(std::abs(target-flight.distance)<1e-8)return flight;
    double lo=0,hi=1000000;
    for(int i=0;i<36;++i) {
        const double mid=(lo+hi)/2;
        if(model.fly(mid,input.hill,weather,input.startSeconds,false,response).distance<target)lo=mid;else hi=mid;
    }
    return model.fly((lo+hi)/2,input.hill,weather,input.startSeconds,input.captureExposure,response);
}
}

Engine::Engine(const EngineParameters &parameters)
    : qualityModel(parameters.quality,HillSkillCurve(parameters.skillAnchors,parameters.curveExtension)),
      flightModel(parameters.flight),landingModel(parameters.landing)
{}

JumpResult Engine::simulate(const JumpRequest &input,RandomSource &rng,const WeatherField &weather,const WindMeasurement &measurement) const
{
    validate(input);JumpResult result;result.manipulated=input.manipulation.active();
    if(input.skip) {result.status=JumpStatus::DidNotStart;return result;}
    if(rng.event(input.manipulation.dsqProbability.value_or(input.dsqProbability))) {result.status=JumpStatus::Disqualified;return result;}
    // Reserve this seed before sampling quality: changing its distribution or
    // setting its deviation to zero must not change the wind-response draw.
    const Seed responseSeed=rng.next();
    result.quality=qualityModel.sample(input.athlete,input.hill.k,input.hill.takeoffBias,input.event,rng);
    result.quality.manipulationEffect=input.manipulation.quality.apply(result.quality.value())-result.quality.value();
    const double precip=precipitation(input,weather);
    const double reserve=flightModel.reserve(result.quality.value(),input.hill,input.scoring.juryGate-input.scoring.coachReduction,
        precip,weather.wind(0,input.startSeconds).head,weather.precipitationKind());
    // Fixed separate stream: identical input seeds couple weather/no-weather trials,
    // while landing draws do not depend on the number of integration steps.
    SeededRandom responseRng(responseSeed);
    const WindResponseSample response{std::tanh(responseRng.normal(0,1)),responseRng.unit(),responseRng.uniform(.5,1)};
    result.flight=manipulatedFlight(input,flightModel,weather,reserve,result.distanceLimitedByGeometry,response);
    result.flight.precipitation=precip;
    result.flight.precipitationKind=weather.precipitationKind();
    result.measuredDistance=std::round(result.flight.distance*2)/2;
    result.landingProbabilities=landingModel.probabilities(result.flight.distance,input.hill.realHs,result.flight.meanAbsoluteCrosswind);
    result.landing=landingModel.sample(result.flight.distance,input.hill.realHs,result.flight.meanAbsoluteCrosswind,rng);
    if(input.manipulation.landing)result.landing=*input.manipulation.landing;
    result.judges=landingModel.judges(input.athlete.landing,result.landing,result.flight.distance,input.hill.realHs,rng);
    for(std::size_t i=0;i<result.judges.size();++i)if(input.manipulation.judges[i])result.judges[i]=*input.manipulation.judges[i];
    const double measuredWind=measurement.measure(weather,result.flight,input.startSeconds);
    if(!std::isfinite(measuredWind) || std::abs(measuredWind)>100)throw std::invalid_argument("Invalid measured wind");
    result.score=scoreJump(result.measuredDistance,result.judges,std::clamp(input.manipulation.measuredWind.apply(measuredWind),-100.0,100.0),input.hill,input.scoring);
    return result;
}

double Engine::previewDistance(const JumpRequest &input,const WeatherField &weather) const
{
    validate(input);
    const auto quality=qualityModel.baseline(input.athlete,input.hill.k,input.hill.takeoffBias,input.event);
    const double reserve=flightModel.reserve(input.manipulation.quality.apply(quality.value()),input.hill,input.scoring.juryGate-input.scoring.coachReduction,
        precipitation(input,weather),weather.wind(0,input.startSeconds).head,weather.precipitationKind());
    auto preview=input;preview.captureExposure=false;bool limited=false;
    return manipulatedFlight(preview,flightModel,weather,reserve,limited).distance;
}

}
