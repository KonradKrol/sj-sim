#include "FlightModel.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace SimulationV2 {
namespace {
void bound(double value,double lo,double hi,const char *message)
{
    if(!std::isfinite(value) || value<lo || value>hi) throw std::invalid_argument(message);
}
void checkWind(Wind wind)
{
    bound(wind.head,-100,100,"Non-finite or excessive headwind supplied by weather field");
    bound(wind.cross,-100,100,"Non-finite or excessive crosswind supplied by weather field");
}
}

void Hill::validate() const
{
    bound(k,1,400,"Hill K out of range");bound(hs,k,500,"Hill HS out of range");
    bound(realHs,.5*k,600,"Hill real HS out of range");bound(takeoffBias,-1,1,"Hill preference out of range");
    bound(pointsAtK,0,200,"Hill points at K out of range");bound(pointsPerMetre,0,20,"Hill metre value out of range");
    for(double value:{gatePoints,headwindPoints,tailwindPoints})bound(value,0,100,"Hill compensation factor out of range");
}

void FlightParameters::validate() const
{
    bound(referenceQuality,1,100,"Reference quality out of range");
    bound(referenceDistanceAtK,0,2,"Reference distance out of range");
    bound(qualityMetresAtK120,0,10,"Quality distance coefficient out of range");
    if(referenceGate<0 || referenceGate>200)throw std::invalid_argument("Reference gate out of range");
    bound(gateMetresAtK120,0,20,"Gate distance coefficient out of range");
    bound(gateTailwindInteraction,0,1,"Gate/wind interaction out of range");
    bound(precipitationLossPerLevel,0,.1,"Precipitation effect out of range");
    bound(rainLossPerLevel,0,.1,"Rain effect out of range");
    bound(rainLossThreshold,0,10,"Rain threshold out of range");
    bound(stepMetres,.05,5,"Integration step out of range");
    bound(secondsPerK,.1,30,"Flight time scale out of range");
    bound(windReservePerMetre,0,1,"Wind effect out of range");bound(crosswindDrag,0,1,"Crosswind drag out of range");
    bound(windResponseMetres,0,100,"Wind response scale out of range");
    bound(earlyWindSensitivity,0,10,"Early phase sensitivity out of range");bound(lateWindSensitivity,0,10,"Late phase sensitivity out of range");
    bound(maximumWindAssistance,0,.99,"Wind assistance must remain below consumption");
    bound(geometryStartRealHs,.5,1,"Geometry start out of range");
    bound(usableLandingBeyondRealHsAtK,.01,.5,"Landing extension out of range");
    bound(geometryResistance,.01,100,"Geometry resistance out of range");
    if(exposureSections<1 || exposureSections>32)throw std::invalid_argument("Exposure section count out of range");
}

FlightModel::FlightModel(FlightParameters parameters):config(parameters) { config.validate(); }

double FlightModel::landingLimit(const Hill &hill) const
{
    hill.validate();return hill.realHs+config.usableLandingBeyondRealHsAtK*hill.k;
}

double FlightModel::reserve(double quality,const Hill &hill,int gate,double precipitation,double initialHeadwind,PrecipitationKind kind) const
{
    hill.validate();bound(quality,-1e12,1e12,"Jump quality out of range");
    if(gate<0 || gate>200)throw std::invalid_argument("Starting gate out of range");
    bound(precipitation,0,10,"Precipitation out of range");bound(initialHeadwind,-100,100,"Initial wind out of range");
    if(kind!=PrecipitationKind::Snow && kind!=PrecipitationKind::Rain)throw std::invalid_argument("Unknown precipitation kind");
    const double distance=hill.k*config.referenceDistanceAtK+(quality-config.referenceQuality)*config.qualityMetresAtK120*hill.k/120;
    const double gateEffect=(gate-config.referenceGate)*config.gateMetresAtK120*hill.k/120
        *(1+config.gateTailwindInteraction*std::max(0.0,-initialHeadwind));
    // Approximate track condition, not a rain-induced loss of aerodynamic lift.
    // Light water is normal on prepared tracks; only excess rain receives a loss.
    const double loss=kind==PrecipitationKind::Snow ? config.precipitationLossPerLevel*precipitation
        : config.rainLossPerLevel*std::max(0.0,precipitation-config.rainLossThreshold);
    return std::clamp((distance+gateEffect)*(1-loss),0.0,1000000.0);
}

FlightResult FlightModel::fly(double initialReserve,const Hill &hill,const WeatherField &weather,double startSeconds,bool capture) const
{
    hill.validate();bound(initialReserve,0,1000000,"Flight reserve out of range");
    bound(startSeconds,0,1e12-100000,"Flight start time out of range");
    FlightResult result;result.reserve=initialReserve;
    const double limit=landingLimit(hill), geometryStart=hill.realHs*config.geometryStartRealHs;
    const double sectionWidth=hill.k/config.exposureSections;
    if(capture) {
        result.exposures.resize(std::size_t(config.exposureSections));
        for(int i=0;i<config.exposureSections;++i) {
            auto &section=result.exposures[std::size_t(i)];
            section.begin=i*sectionWidth;
            section.end=i+1==config.exposureSections ? limit : (i+1)*sectionWidth;
            // Extremely demanding custom hills can end before the last sections.
            section.begin=std::min(section.begin,limit);section.end=std::min(section.end,limit);
            section.before=weather.wind((section.begin+section.end)/2,startSeconds);checkWind(section.before);
        }
    }
    double x=0,remaining=initialReserve,response=0,headIntegral=0,crossIntegral=0,absoluteCrossIntegral=0;
    const int maxSteps=int(std::ceil(limit/config.stepMetres))+config.exposureSections+2;
    while(remaining>1e-9 && x<limit && result.integrationSteps<maxSteps) {
        int section=std::min(config.exposureSections-1,int(x/sectionWidth));
        double sectionEnd=section+1==config.exposureSections ? limit : (section+1)*sectionWidth;
        double step=std::min({config.stepMetres,limit-x,sectionEnd-x});
        // Roundoff exactly at a section boundary must not stall integration.
        if(step<1e-9) {
            ++section;
            if(section>=config.exposureSections)break;
            sectionEnd=section+1==config.exposureSections?limit:(section+1)*sectionWidth;
            step=std::min({config.stepMetres,limit-x,sectionEnd-x});
        }
        const Wind wind=weather.wind(x,startSeconds+x/hill.k*config.secondsPerK);checkWind(wind);
        const auto filtered=[&](double length) {
            if(config.windResponseMetres==0)return wind.head;
            const double a=length/config.windResponseMetres;
            return wind.head+(response-wind.head)*(-std::expm1(-a))/a;
        };
        const auto cost=[&](double length) {
            const double mid=x+length*.5;
            const double progress=std::clamp((mid-geometryStart)/(limit-geometryStart),0.0,1-1e-12);
            const double terrain=1+config.geometryResistance*progress*progress/(1-progress);
            const double phase=std::clamp(mid/hill.k,0.0,1.0);
            const double sensitivity=config.earlyWindSensitivity+(config.lateWindSensitivity-config.earlyWindSensitivity)*phase;
            const double assistance=std::min(config.maximumWindAssistance*terrain,
                config.windReservePerMetre*filtered(length)*sensitivity);
            return length*(terrain*(1+config.crosswindDrag*wind.cross*wind.cross)-assistance);
        };
        double spent=cost(step);
        if(spent>remaining) {
            double lo=0,hi=step;
            for(int i=0;i<30;++i) { const double mid=(lo+hi)/2;if(cost(mid)>remaining)hi=mid;else lo=mid; }
            step=(lo+hi)/2;spent=remaining;
        }
        if(capture) {
            auto &exposure=result.exposures[std::size_t(section)];
            if(exposure.traversed==0)exposure.minimumHead=exposure.maximumHead=wind.head;
            else {exposure.minimumHead=std::min(exposure.minimumHead,wind.head);exposure.maximumHead=std::max(exposure.maximumHead,wind.head);}
            exposure.experienced.head+=wind.head*step;exposure.experienced.cross+=wind.cross*step;
            exposure.traversed+=step;
        }
        if(config.windResponseMetres==0)response=wind.head;
        else response=wind.head+(response-wind.head)*std::exp(-step/config.windResponseMetres);
        headIntegral+=wind.head*step;crossIntegral+=wind.cross*step;absoluteCrossIntegral+=std::abs(wind.cross)*step;
        x+=step;remaining=std::max(0.0,remaining-spent);++result.integrationSteps;
    }
    result.distance=x;result.duration=x/hill.k*config.secondsPerK;
    if(x>0) {result.experiencedWind={headIntegral/x,crossIntegral/x};result.meanAbsoluteCrosswind=absoluteCrossIntegral/x;}
    if(capture)for(auto &exposure:result.exposures) {
        if(exposure.traversed>0) {exposure.experienced.head/=exposure.traversed;exposure.experienced.cross/=exposure.traversed;}
        exposure.after=weather.wind((exposure.begin+exposure.end)/2,startSeconds+result.duration);checkWind(exposure.after);
    }
    return result;
}

}
