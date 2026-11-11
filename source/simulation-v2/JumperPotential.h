#pragma once
#include "ProfileBridge.h"
#include "../simulator/Jumper.h"
#include "../simulator/Hill.h"
#include "../form-engine/DailyForm.h"

namespace SimulationV2 {
inline double jumperPotential(const Jumper &jumper,const ::Hill &hill,const EngineParameters &parameters,
                              const WeatherField *weather,int gate,EventKind event,
                              std::optional<Seed> daySeed={},double startSeconds=0) {
    const auto profile=jumper.getSimulationProfile();
    if(!profile)return 0;
    auto actor=athleteInput(*profile);
    if(daySeed)actor.form=AutoForm::dailyForm(actor.form,*daySeed,profile->id);
    const auto venue=hill.getSimulationProfile();if(!venue)return actor.overall;
    const auto input=hillInput(*venue);
    const QualityModel quality(parameters.quality,HillSkillCurve(parameters.skillAnchors,parameters.curveExtension));
    const double value=quality.baseline(actor,input.k,input.takeoffBias,event).value();
    struct Calm final:WeatherField {Wind wind(double,double)const override{return {};}double precipitation(double)const override{return 0;}} calm;
    const auto &field=weather?*weather:static_cast<const WeatherField &>(calm);
    const FlightModel flight(parameters.flight);
    return flight.fly(flight.reserve(value,input,gate,0,field.wind(0,startSeconds).head),input,field,startSeconds,false,{},actor.perks[1]).distance;
}
}
