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
    bound(judgeOutlierChance,0,1,"Judge outlier chance out of range");
    bound(highMarkCompression,.01,1,"High mark compression out of range");
    bound(difficultLandingStylePenalty,0,20,"Difficult landing penalty out of range");
    bound(imperfectStyleBase,0,1,"Imperfect landing chance out of range");bound(imperfectStyleIncrease,0,1,"Imperfect landing increase out of range");bound(imperfectStylePenalty,0,10,"Imperfect landing penalty out of range");
    bound(distanceStyleOffset,-10,10,"Distance style offset out of range");
    bound(distanceStyleBelowK,0,50,"Short jump style slope out of range");bound(distanceStyleAboveK,0,50,"Long jump style slope out of range");
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

std::array<double,5> LandingModel::judges(int landingClass,LandingKind landing,double distance,double k,double realHs,RandomSource &rng) const
{
    validateDistance(distance,realHs);bound(k,1,400,"Hill K out of range");
    if(landingClass<0 || landingClass>4 || int(landing)<0 || int(landing)>3)throw std::invalid_argument("Unknown landing class/kind");
    const double difficult=logistic((distance/realHs-config.twoFeetCentre)/config.twoFeetWidth);
    const double relative=(distance-k)/k;
    const double distanceEffect=config.distanceStyleOffset+relative*(relative<0?config.distanceStyleBelowK:config.distanceStyleAboveK);
    const double imperfect=rng.event(std::clamp(config.imperfectStyleBase+config.imperfectStyleIncrease*difficult,0.0,1.0)) ? config.imperfectStylePenalty : 0;
    double penalty=config.landingPenalty[std::size_t(landing)];
    if(landing==LandingKind::TwoFeet) {
        // A bounded beta(1, 7/3) draw: 2..4, mean 2.6 at the default scale.
        penalty*= (2+2*(1-std::pow(1-rng.unit(),3.0/7)))/2.6;
    } else if(landing==LandingKind::Support || landing==LandingKind::Fall) {
        // Symmetric triangular draws concentrate around the requested mean.
        const double width=landing==LandingKind::Support?1.6:1.0;
        const double reference=landing==LandingKind::Support?5.6:8.0;
        penalty*=1+width/reference*(rng.unit()+rng.unit()-1);
    }
    const double mean=config.styleMean[std::size_t(landingClass)]+distanceEffect-imperfect-penalty
        -config.difficultLandingStylePenalty*difficult+rng.normal(0,config.sharedStyleDeviation);
    std::array<double,5> marks;
    const double disagreement[]{1,1.3,1.65,1.5};
    for(double &mark:marks) {
        double value=mean+rng.normal(0,config.judgeDeviation*disagreement[int(landing)]);
        if(rng.event(config.judgeOutlierChance))value+=rng.uniform(-3,2);
        // Compress only the upper tail; a perfect mark remains attainable.
        if(value>19.25)value=19.25+(value-19.25)*config.highMarkCompression;
        mark=std::clamp(std::round(value*2)/2,0.0,20.0);
    }
    return marks;
}

}
