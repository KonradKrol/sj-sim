#include "LandingModel.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
namespace {
void bound(double v,double lo,double hi,const char *message)
{
    if(!std::isfinite(v) || v<lo || v>hi)throw std::invalid_argument(message);
}
double logistic(double value) { return .5+.5*std::tanh(value*.5); }
void validateDistance(double distance,double realHs)
{
    bound(distance,0,1000,"Landing distance out of range");bound(realHs,.5,600,"Landing real HS out of range");
}
}

void LandingParameters::validate() const
{
    for(double v:{fallBase,fallIncrease,supportBase,supportIncrease,twoFeetBase,twoFeetIncrease})bound(v,0,1,"Landing probability coefficient out of range");
    for(double v:{fallCentre,supportCentre,twoFeetCentre})bound(v,.5,2,"Landing risk centre out of range");
    for(double v:{fallWidth,supportWidth,twoFeetWidth})bound(v,.001,1,"Landing risk width out of range");
    for(double v:{crosswindFallPerMetrePerSecond,crosswindSupportPerMetrePerSecond,crosswindTwoFeetPerMetrePerSecond})bound(v,0,1,"Crosswind landing risk out of range");
    for(double v:styleMean)bound(v,0,20,"Style mean out of range");
    for(double v:landingPenalty)bound(v,0,20,"Landing style penalty out of range");
    bound(sharedStyleDeviation,0,10,"Shared style deviation out of range");bound(judgeDeviation,0,10,"Judge deviation out of range");
    bound(difficultLandingStylePenalty,0,20,"Difficult landing penalty out of range");
}

LandingModel::LandingModel(LandingParameters parameters):config(parameters) { config.validate(); }

LandingProbabilities LandingModel::probabilities(double distance,double realHs,double crosswind) const
{
    validateDistance(distance,realHs);bound(crosswind,0,100,"Landing crosswind out of range");
    const double ratio=distance/realHs;
    const double fall=std::clamp(config.fallBase+config.fallIncrease*logistic((ratio-config.fallCentre)/config.fallWidth)
        +crosswind*config.crosswindFallPerMetrePerSecond,0.0,1.0);
    const double support=(1-fall)*std::clamp(config.supportBase+config.supportIncrease*logistic((ratio-config.supportCentre)/config.supportWidth)
        +crosswind*config.crosswindSupportPerMetrePerSecond,0.0,1.0);
    const double twoFeet=(1-fall-support)*std::clamp(config.twoFeetBase+config.twoFeetIncrease*logistic((ratio-config.twoFeetCentre)/config.twoFeetWidth)
        +crosswind*config.crosswindTwoFeetPerMetrePerSecond,0.0,1.0);
    return {{{std::max(0.0,1-fall-support-twoFeet),twoFeet,support,fall}}};
}

LandingKind LandingModel::sample(double distance,double realHs,double crosswind,RandomSource &rng) const
{
    const auto p=probabilities(distance,realHs,crosswind);
    const double draw=rng.unit();double cumulative=0;
    for(std::size_t i=0;i<p.values.size();++i) {cumulative+=p.values[i];if(draw<cumulative)return static_cast<LandingKind>(i);}
    return LandingKind::Fall;
}

std::array<double,5> LandingModel::judges(int landingClass,LandingKind landing,double distance,double realHs,RandomSource &rng) const
{
    validateDistance(distance,realHs);
    if(landingClass<0 || landingClass>4 || int(landing)<0 || int(landing)>3)throw std::invalid_argument("Unknown landing class/kind");
    const double difficult=logistic((distance/realHs-config.twoFeetCentre)/config.twoFeetWidth);
    const double mean=config.styleMean[std::size_t(landingClass)]-config.landingPenalty[std::size_t(landing)]
        -config.difficultLandingStylePenalty*difficult+rng.normal(0,config.sharedStyleDeviation);
    std::array<double,5> marks;
    for(double &mark:marks)mark=std::clamp(std::round((mean+rng.normal(0,config.judgeDeviation))*2)/2,0.0,20.0);
    return marks;
}

}
