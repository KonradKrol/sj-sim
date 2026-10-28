#include "Weather.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace SimulationV2 {
namespace {
constexpr double pi=3.14159265358979323846;
void bounded(double value,double lo,double hi,const char *message)
{
    if (!std::isfinite(value) || value<lo || value>hi) throw std::invalid_argument(message);
}
double smooth(double x) { return x*x*(3-2*x); }
double mix(double a,double b,double t) { return a+(b-a)*t; }
double lattice(Seed seed,std::int64_t x,std::int64_t t)
{
    const Seed bits=deriveSeed(deriveSeed(seed,static_cast<Seed>(x)),static_cast<Seed>(t));
    return double(bits>>11)*(2.0/9007199254740992.0)-1;
}
double noise(Seed seed,double x,double t)
{
    const auto ix=std::int64_t(std::floor(x)), it=std::int64_t(std::floor(t));
    const double sx=smooth(x-double(ix)), st=smooth(t-double(it));
    return mix(mix(lattice(seed,ix,it),lattice(seed,ix+1,it),sx),
               mix(lattice(seed,ix,it+1),lattice(seed,ix+1,it+1),sx),st);
}
void positionAndTime(double distance,double seconds)
{
    bounded(distance,0,1000000,"Weather distance out of range");
    bounded(seconds,0,1e12,"Weather time out of range");
}
}

double Wind::strength() const { return std::hypot(head,cross); }

void WindParameters::validate() const
{
    if (references.empty() || references.size()>1000) throw std::invalid_argument("Weather needs 1..1000 reference points");
    double previous=-1;
    for (const auto &p:references) {
        bounded(p.distance,0,1000000,"Wind reference position out of range");
        bounded(p.value.head,-100,100,"Wind reference component out of range");
        bounded(p.value.cross,-100,100,"Wind reference component out of range");
        if (p.distance<=previous) throw std::invalid_argument("Wind reference positions must be strictly increasing");
        previous=p.distance;
    }
    bounded(temporalAmplitude,0,100,"Temporal wind amplitude out of range");
    bounded(spatialAmplitude,0,100,"Spatial wind amplitude out of range");
    bounded(correlationSeconds,.1,100000,"Wind time correlation out of range");
    bounded(correlationMetres,.1,100000,"Wind space correlation out of range");
    bounded(gustAmplitude,0,100,"Gust amplitude out of range");
    bounded(gustProbability,0,1,"Gust probability out of range");
    bounded(gustPeriodSeconds,.1,100000,"Gust period out of range");
    bounded(gustWidthMetres,.1,100000,"Gust width out of range");
    bounded(gustTravelMetresPerSecond,0,100,"Gust travel speed out of range");
    bounded(maximumStrength,.01,100,"Maximum wind strength out of range");
    bounded(trendAmplitude,0,100,"Wind tendency amplitude out of range");
    bounded(trendPeriodSeconds,1,100000,"Wind tendency duration out of range");
    bounded(trendTransitionSeconds,.1,trendPeriodSeconds*.3,"Wind tendency transition out of range");
    bounded(activityVariation,0,1,"Wind activity variation out of range");
    bounded(activityCorrelationSeconds,.1,100000,"Wind activity correlation out of range");
    bounded(fastCorrelationSeconds,.1,100000,"Fast wind correlation out of range");
}

void PrecipitationParameters::validate() const
{
    if(kind!=PrecipitationKind::Snow && kind!=PrecipitationKind::Rain)throw std::invalid_argument("Unknown precipitation kind");
    if(int(scenario)<0 || int(scenario)>4) throw std::invalid_argument("Unknown precipitation scenario");
    bounded(intensity,0,10,"Precipitation intensity out of range");
    bounded(variation,0,10,"Precipitation variation out of range");
    bounded(correlationSeconds,.1,100000,"Precipitation correlation out of range");
    bounded(increasePerMinute,0,10,"Precipitation trend out of range");
}

ContinuousWeather::ContinuousWeather(Seed key,WindParameters wind,PrecipitationParameters precipitation)
    : seed(key),config(std::move(wind)),precip(std::move(precipitation))
{
    config.validate();precip.validate();
}

Wind ContinuousWeather::wind(double distance,double seconds) const
{
    positionAndTime(distance,seconds);
    Wind result;
    const auto upper=std::upper_bound(config.references.begin(),config.references.end(),distance,
                                    [](double x,const WindReference &p){return x<p.distance;});
    if(upper==config.references.begin()) result=upper->value;
    else if(upper==config.references.end()) result=config.references.back().value;
    else {
        const auto &a=*(upper-1), &b=*upper;
        const double t=smooth((distance-a.distance)/(b.distance-a.distance));
        result={mix(a.value.head,b.value.head,t),mix(a.value.cross,b.value.cross,t)};
    }
    // A weather regime is shared across the hill. Boundaries jitter in time,
    // with a short continuous transition and a long plateau; no per-jump reroll.
    if(config.trendAmplitude>0) {
        const auto boundary=[&](std::int64_t n) {
            if(n<=0)return 0.0;
            return (double(n)+.25*lattice(deriveSeed(seed,7),n,0))*config.trendPeriodSeconds;
        };
        const auto target=[&](std::int64_t n) {
            if(n<=0)return Wind{};
            return Wind{config.trendAmplitude*lattice(deriveSeed(seed,8),n,0),
                        config.trendAmplitude*lattice(deriveSeed(seed,9),n,0)};
        };
        auto regime=std::int64_t(std::floor(seconds/config.trendPeriodSeconds));
        if(seconds<boundary(regime))--regime;
        else if(seconds>=boundary(regime+1))++regime;
        const auto from=target(regime-1),to=target(regime);
        const double blend=smooth(std::clamp((seconds-boundary(regime))/config.trendTransitionSeconds,0.0,1.0));
        result.head+=mix(from.head,to.head,blend);result.cross+=mix(from.cross,to.cross,blend);
    }
    const double x=distance/config.correlationMetres, t=seconds/config.correlationSeconds;
    const double activity=config.activityVariation>0 ? config.activityVariation*smooth(.5+.5*noise(deriveSeed(seed,10),0,seconds/config.activityCorrelationSeconds)) : 0;
    const auto fluctuation=[&](Seed stream) {
        const double slow=noise(deriveSeed(seed,stream),0,t);
        if(activity==0)return slow;
        return mix(slow,noise(deriveSeed(seed,stream+20),0,seconds/config.fastCorrelationSeconds),activity)*(1+2*activity);
    };
    result.head+=config.temporalAmplitude*fluctuation(1)
        +config.spatialAmplitude*noise(deriveSeed(seed,2),x,t*1.7)*(1+activity);
    result.cross+=config.temporalAmplitude*fluctuation(3)
        +config.spatialAmplitude*noise(deriveSeed(seed,4),x,t*1.7)*(1+activity);
    if(config.gustAmplitude>0 && config.gustProbability>0) {
        const auto slot=std::int64_t(std::floor(seconds/config.gustPeriodSeconds));
        // Pulses last at most 1.6 periods. Only these three slots can be active.
        for(int offset=0;offset<=2;++offset) {
            if(slot<offset) continue;
            SeededRandom gust(deriveSeed(deriveSeed(seed,5),Seed(slot-offset)));
            if(!gust.event(config.gustProbability)) continue;
            const double start=double(slot-offset)*config.gustPeriodSeconds+gust.uniform(0,.4)*config.gustPeriodSeconds;
            const double duration=gust.uniform(.2,1.6)*config.gustPeriodSeconds;
            const double phase=(seconds-start)/duration;
            if(phase<=0 || phase>=1) continue;
            const double centre=gust.uniform(0,config.references.back().distance+config.gustWidthMetres)
                +gust.uniform(-config.gustTravelMetresPerSecond,config.gustTravelMetresPerSecond)*(seconds-start);
            const double spatial=1-std::abs(distance-centre)/config.gustWidthMetres;
            if(spatial<=0) continue;
            const double angle=gust.uniform(0,2*pi);
            const double amplitude=config.gustAmplitude*gust.uniform(.4,1)*smooth(spatial)*std::pow(std::sin(pi*phase),2);
            result.head+=amplitude*std::cos(angle);result.cross+=amplitude*std::sin(angle);
        }
    }
    const double strength=result.strength();
    if(strength>config.maximumStrength) {
        const double scale=config.maximumStrength/strength;result.head*=scale;result.cross*=scale;
    }
    return result;
}

double ContinuousWeather::precipitation(double seconds) const
{
    positionAndTime(0,seconds);
    const double variation=noise(deriveSeed(seed,6),0,seconds/precip.correlationSeconds)*precip.variation;
    switch(precip.scenario) {
    case PrecipitationScenario::None:return 0;
    case PrecipitationScenario::Stable:return precip.intensity;
    case PrecipitationScenario::Variable:return std::clamp(precip.intensity+variation,0.0,10.0);
    case PrecipitationScenario::Blizzard:return std::clamp(std::max(8.0,precip.intensity)+variation*.35,0.0,10.0);
    case PrecipitationScenario::Increasing:return std::clamp(precip.intensity+seconds/60*precip.increasePerMinute+variation*.25,0.0,10.0);
    }
    throw std::invalid_argument("Unknown precipitation scenario");
}

WeatherProposal proposeWeather(const WeatherProposalContext &context,RandomSource &rng)
{
    bounded(context.k,1,400,"Proposal K out of range");
    if(context.variabilityPerk< -3 || context.variabilityPerk>3 || context.directionPerk< -3 || context.directionPerk>3
        || context.month<0 || context.month>12 || (context.surface!=Surface::Snow && context.surface!=Surface::Mat))
        throw std::invalid_argument("Invalid weather proposal context");
    static const double headProbability[]{.5,.70,.84,.94};
    double probability=headProbability[std::abs(context.directionPerk)];
    if(context.directionPerk<0) probability=1-probability;
    const bool head=rng.event(probability);
    const bool cross=rng.event(.15);
    const double magnitude=rng.uniform(.2,2.5);
    Wind base{(head?1:-1)*magnitude*(cross?.15:1),cross?(rng.event(.5)?1:-1)*magnitude:0};
    // Three broad weather characters, substantially biased by hill lottery.
    const double lottery=context.variabilityPerk/3.0;
    const std::size_t character=lottery>=0 ? rng.weightedIndex({.45-.37*lottery,.40+.02*lottery,.15+.35*lottery})
        : rng.weightedIndex({.45-.40*lottery,.40+.26*lottery,.15+.14*lottery});
    static const double temporal[]{.15,.7,1.8},spatial[]{.08,.55,1.4},gusts[]{.3,1.8,4};
    static const double chance[]{.03,.25,.65},times[]{100,45,18};
    WeatherProposal result;
    result.wind.temporalAmplitude=temporal[character];result.wind.spatialAmplitude=spatial[character];
    result.wind.gustAmplitude=gusts[character];result.wind.gustProbability=chance[character];
    result.wind.correlationSeconds=times[character];
    static const double trendAmplitude[]{.15,1.2,2.8},trendPeriod[]{300,150,60},transitions[]{30,15,5},activity[]{.15,.5,.95};
    result.wind.trendAmplitude=trendAmplitude[character];result.wind.trendPeriodSeconds=trendPeriod[character];
    result.wind.trendTransitionSeconds=transitions[character];result.wind.activityVariation=activity[character];
    result.wind.correlationMetres=context.k*(character==0?.8:.35);
    result.wind.gustWidthMetres=context.k*.35;
    result.wind.references={{0,base},{context.k*.5,{base.head+rng.uniform(-spatial[character],spatial[character]),base.cross}},
        {context.k,{base.head+rng.uniform(-spatial[character],spatial[character]),base.cross+rng.uniform(-spatial[character],spatial[character])}}};
    // Broad editable seasonal suggestions, not a climate model or a forecast.
    int month=context.month;
    if(month && (context.country=="AUS" || context.country=="NZL" || context.country=="ARG" || context.country=="CHL"))
        month=(month+5)%12+1;
    const bool cold=month && (month>=11 || month<=3);
    const bool northern=context.country=="NOR" || context.country=="SWE" || context.country=="FIN" || context.country=="CAN";
    result.precipitation.kind=context.surface==Surface::Mat || (month && !cold && rng.event(.7)) ? PrecipitationKind::Rain : PrecipitationKind::Snow;
    const double wetChance=context.surface==Surface::Snow ? .25+(cold?.25:0)+(northern?.1:0) : .30;
    if(rng.event(wetChance)) {
        const auto choice=rng.weightedIndex({.45,.25,.08,.22});
        result.precipitation.scenario=static_cast<PrecipitationScenario>(choice+1);
        result.precipitation.intensity=choice==2?rng.uniform(8,10):rng.uniform(1,5);
        result.precipitation.variation=choice==1?rng.uniform(2,5):1;
    }
    result.wind.validate();result.precipitation.validate();
    return result;
}

}
