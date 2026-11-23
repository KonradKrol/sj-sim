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
    if(!precipitationEnabled)return 0;
    const double value=weather.precipitation(input.startSeconds);
    if(!std::isfinite(value) || value<0 || value>10)throw std::invalid_argument("Invalid precipitation supplied by weather field");
    return std::clamp(input.manipulation.precipitation.apply(value),0.0,10.0);
}

FlightResult manipulatedFlight(const JumpRequest &input,const FlightModel &model,const WeatherField &weather,double reserve,bool &limited,WindResponseSample response={})
{
    auto flight=model.fly(reserve,input.hill,weather,input.startSeconds,input.captureExposure,response,input.athlete.perks[1]);
    const double spread=std::max(0.,1+input.hill.distanceSpreadPerk*model.parameters().hillSpreadPerLevel*3.9);
    if(!input.manipulation.distance.active() && spread==1)return flight;
    const double natural=std::max(0.,input.hill.k+(flight.distance-input.hill.k)*spread);
    const double desired=input.manipulation.distance.apply(natural);
    // A requested result is realised by adjusting starting reserve and flying
    // again, including all newly reached wind. Never attach a false wind trace
    // to a distance simply overwritten after the natural flight calculation.
    const double limit=model.landingLimit(input.hill);
    const double target=std::clamp(desired,0.0,limit);
    limited=target!=desired;
    if(std::abs(target-flight.distance)<1e-8)return flight;
    // Find the starting reserve that lands on the target. Distance grows continuously with the
    // reserve, so a bracketed false position (Illinois variant) reaches the target in a few
    // flights instead of a 36-step bisection over [0, 1000000], at least as accurately.
    const auto distanceAt=[&](double value){return model.fly(value,input.hill,weather,input.startSeconds,false,response,input.athlete.perks[1]).distance;};
    constexpr double maximumReserve=1000000,tolerance=1e-7;
    double lo=0,dLo=0,hi=maximumReserve,dHi=0;bool haveHi=false;
    if(flight.distance<target){lo=reserve;dLo=flight.distance;}else{hi=reserve;dHi=flight.distance;haveHi=true;}
    // Start from the proportional guess and widen the bracket until it contains the target.
    double guess=std::clamp(reserve*target/std::max(flight.distance,1.0),0.0,maximumReserve);
    for(int i=0;i<60;++i){
        const double distance=distanceAt(guess);
        if(distance<target){if(guess>=lo){lo=guess;dLo=distance;}}
        else if(!haveHi || guess<=hi){hi=guess;dHi=distance;haveHi=true;}
        if(haveHi && lo<hi)break;
        if(!haveHi){if(guess>=maximumReserve)break;guess=std::min(maximumReserve,std::max(guess*1.5,guess+10));}
        else guess=std::max(0.0,guess*0.5);
    }
    double best=haveHi?hi:lo;
    if(haveHi){
        if(std::abs(dHi-target)<=tolerance)best=hi;
        else{
            int retained=0;
            for(int i=0;i<80 && hi-lo>1e-12;++i){
                double next=dHi!=dLo?lo+(target-dLo)*(hi-lo)/(dHi-dLo):(lo+hi)/2;
                if(!(next>lo && next<hi))next=(lo+hi)/2;
                const double distance=distanceAt(next);
                best=next;
                if(std::abs(distance-target)<=tolerance)break;
                if(distance<target){lo=next;dLo=distance;if(retained==-1)dHi=target+(dHi-target)/2;retained=-1;}
                else{hi=next;dHi=distance;if(retained==1)dLo=target+(dLo-target)/2;retained=1;}
                best=hi;
            }
        }
    }
    return model.fly(best,input.hill,weather,input.startSeconds,input.captureExposure,response,input.athlete.perks[1]);
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
    result.judges=landingModel.judges(input.athlete.landing,result.landing,result.flight.distance,input.hill.k,input.hill.realHs,rng,input.hill.hs);
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
    if(!preview.manipulation.distance.active()) {
        // Same natural flight and hill spread as manipulatedFlight(). A real jump then flies again
        // (a 36-step search) to land exactly on the spread distance; a preview only needs that
        // distance, so it skips the search on hills with a distance-spread perk.
        const auto flight=flightModel.fly(reserve,preview.hill,weather,preview.startSeconds,false,{},preview.athlete.perks[1]);
        const double spread=std::max(0.,1+preview.hill.distanceSpreadPerk*flightModel.parameters().hillSpreadPerLevel*3.9);
        if(spread==1)return flight.distance;
        const double natural=std::max(0.,preview.hill.k+(flight.distance-preview.hill.k)*spread);
        return std::clamp(natural,0.0,flightModel.landingLimit(preview.hill));
    }
    return manipulatedFlight(preview,flightModel,weather,reserve,limited).distance;
}

}
