#include "WeatherCalibration.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
namespace SimulationV2 {
double WeatherGeneratorParameters::get(const std::string &key,double fallback) const { auto i=values.find(key);return i==values.end()?fallback:i->second; }
const std::vector<GeneratorRange> &weatherGeneratorRanges() {
    static const std::vector<GeneratorRange> result{
        {"generator.preset.0.strength","strength",0,.6,2.3},
        {"generator.preset.0.angle","angle",0,-5,5},
        {"generator.preset.0.difference","difference",0,.01,.06},
        {"generator.preset.0.temporalAmplitude","temporalAmplitude",0,.02,.10},
        {"generator.preset.0.spatialAmplitude","spatialAmplitude",0,.01,.06},
        {"generator.preset.0.trendAmplitude","trendAmplitude",0,.04,.18},
        {"generator.preset.0.trendPeriodSeconds","trendPeriodSeconds",0,180,480},
        {"generator.preset.0.gustAmplitude","gustAmplitude",0,.05,.20},
        {"generator.preset.0.gustProbability","gustProbability",0,.01,.04},
        {"generator.preset.0.correlationSeconds","correlationSeconds",0,80,180},
        {"generator.preset.0.activityVariation","activityVariation",0,0,.15},
        {"generator.preset.1.strength","strength",1,2,4},
        {"generator.preset.1.angle","angle",1,-8,8},
        {"generator.preset.1.difference","difference",1,.03,.25},
        {"generator.preset.1.temporalAmplitude","temporalAmplitude",1,.04,.45},
        {"generator.preset.1.spatialAmplitude","spatialAmplitude",1,.03,.30},
        {"generator.preset.1.trendAmplitude","trendAmplitude",1,.12,.65},
        {"generator.preset.1.trendPeriodSeconds","trendPeriodSeconds",1,120,360},
        {"generator.preset.1.gustAmplitude","gustAmplitude",1,.2,1},
        {"generator.preset.1.gustProbability","gustProbability",1,.04,.12},
        {"generator.preset.1.correlationSeconds","correlationSeconds",1,45,150},
        {"generator.preset.1.activityVariation","activityVariation",1,.1,.35},
        {"generator.preset.2.strength","strength",2,.2,2.5},
        {"generator.preset.2.angle","angle",2,0,360},
        {"generator.preset.2.difference","difference",2,.7,1.5},
        {"generator.preset.2.temporalAmplitude","temporalAmplitude",2,.9,2.2},
        {"generator.preset.2.spatialAmplitude","spatialAmplitude",2,.7,1.5},
        {"generator.preset.2.trendAmplitude","trendAmplitude",2,2,3.7},
        {"generator.preset.2.trendPeriodSeconds","trendPeriodSeconds",2,35,105},
        {"generator.preset.2.gustAmplitude","gustAmplitude",2,2,4.8},
        {"generator.preset.2.gustProbability","gustProbability",2,.4,.8},
        {"generator.preset.2.correlationSeconds","correlationSeconds",2,12,35},
        {"generator.preset.2.activityVariation","activityVariation",2,.7,1},
        {"generator.preset.3.strength","strength",3,1,3},
        {"generator.preset.3.angle","angle",3,-20,20},
        {"generator.preset.3.difference","difference",3,.3,.9},
        {"generator.preset.3.temporalAmplitude","temporalAmplitude",3,.5,1.2},
        {"generator.preset.3.spatialAmplitude","spatialAmplitude",3,.4,.9},
        {"generator.preset.3.trendAmplitude","trendAmplitude",3,.4,1.4},
        {"generator.preset.3.trendPeriodSeconds","trendPeriodSeconds",3,60,180},
        {"generator.preset.3.gustAmplitude","gustAmplitude",3,1.2,3},
        {"generator.preset.3.gustProbability","gustProbability",3,.2,.5},
        {"generator.preset.3.correlationSeconds","correlationSeconds",3,20,65},
        {"generator.preset.3.activityVariation","activityVariation",3,.4,.8},
        {"generator.preset.4.strength","strength",4,.1,.3},
        {"generator.preset.4.strength2","strength2",4,.03,.5},
        {"generator.preset.4.angle","angle",4,0,360},
        {"generator.preset.4.difference","difference",4,.002,.015},
        {"generator.preset.4.temporalAmplitude","temporalAmplitude",4,.003,.025},
        {"generator.preset.4.spatialAmplitude","spatialAmplitude",4,.002,.015},
        {"generator.preset.4.trendAmplitude","trendAmplitude",4,.005,.025},
        {"generator.preset.4.trendPeriodSeconds","trendPeriodSeconds",4,180,600},
        {"generator.preset.4.gustAmplitude","gustAmplitude",4,.005,.04},
        {"generator.preset.4.gustProbability","gustProbability",4,.005,.02},
        {"generator.preset.4.correlationSeconds","correlationSeconds",4,100,240},
        {"generator.preset.4.activityVariation","activityVariation",4,0,.1},
        {"generator.preset.5.strength","strength",5,.4,1.7},
        {"generator.preset.5.angle","angle",5,-6,6},
        {"generator.preset.5.difference","difference",5,.01,.08},
        {"generator.preset.5.temporalAmplitude","temporalAmplitude",5,.02,.12},
        {"generator.preset.5.spatialAmplitude","spatialAmplitude",5,.01,.07},
        {"generator.preset.5.trendAmplitude","trendAmplitude",5,.04,.2},
        {"generator.preset.5.trendPeriodSeconds","trendPeriodSeconds",5,180,480},
        {"generator.preset.5.gustAmplitude","gustAmplitude",5,.05,.25},
        {"generator.preset.5.gustProbability","gustProbability",5,.01,.05},
        {"generator.preset.5.correlationSeconds","correlationSeconds",5,80,180},
        {"generator.preset.5.activityVariation","activityVariation",5,0,.15},
        {"generator.preset.6.strength","strength",6,2,4},
        {"generator.preset.6.angle","angle",6,-8,8},
        {"generator.preset.6.difference","difference",6,.03,.2},
        {"generator.preset.6.temporalAmplitude","temporalAmplitude",6,.04,.25},
        {"generator.preset.6.spatialAmplitude","spatialAmplitude",6,.03,.15},
        {"generator.preset.6.trendAmplitude","trendAmplitude",6,.1,.4},
        {"generator.preset.6.trendPeriodSeconds","trendPeriodSeconds",6,150,420},
        {"generator.preset.6.gustAmplitude","gustAmplitude",6,.15,.6},
        {"generator.preset.6.gustProbability","gustProbability",6,.02,.08},
        {"generator.preset.6.correlationSeconds","correlationSeconds",6,70,180},
        {"generator.preset.6.activityVariation","activityVariation",6,.05,.2},
        {"generator.preset.7.strength","strength",7,.5,2.5},
        {"generator.preset.7.angle","angle",7,-8,8},
        {"generator.preset.7.difference","difference",7,.02,.12},
        {"generator.preset.7.temporalAmplitude","temporalAmplitude",7,.02,.12},
        {"generator.preset.7.spatialAmplitude","spatialAmplitude",7,.01,.08},
        {"generator.preset.7.trendAmplitude","trendAmplitude",7,.04,.2},
        {"generator.preset.7.trendPeriodSeconds","trendPeriodSeconds",7,180,480},
        {"generator.preset.7.gustAmplitude","gustAmplitude",7,.05,.25},
        {"generator.preset.7.gustProbability","gustProbability",7,.01,.05},
        {"generator.preset.7.correlationSeconds","correlationSeconds",7,80,180},
        {"generator.preset.7.activityVariation","activityVariation",7,0,.15},
        {"generator.preset.8.strength","strength",8,.6,2.2},
        {"generator.preset.9.strength","strength",9,.6,2.2},
        {"generator.preset.8.angle","angle",8,-15,15},
        {"generator.preset.9.angle","angle",9,-15,15},
        {"generator.preset.8.difference","difference",8,.15,.6},
        {"generator.preset.9.difference","difference",9,.15,.6},
        {"generator.preset.8.temporalAmplitude","temporalAmplitude",8,.4,1},
        {"generator.preset.9.temporalAmplitude","temporalAmplitude",9,.4,1},
        {"generator.preset.8.spatialAmplitude","spatialAmplitude",8,.15,.5},
        {"generator.preset.9.spatialAmplitude","spatialAmplitude",9,.15,.5},
        {"generator.preset.8.trendAmplitude","trendAmplitude",8,.7,1.7},
        {"generator.preset.9.trendAmplitude","trendAmplitude",9,.7,1.7},
        {"generator.preset.8.trendPeriodSeconds","trendPeriodSeconds",8,60,180},
        {"generator.preset.9.trendPeriodSeconds","trendPeriodSeconds",9,60,180},
        {"generator.preset.8.gustAmplitude","gustAmplitude",8,.7,1.9},
        {"generator.preset.9.gustAmplitude","gustAmplitude",9,.7,1.9},
        {"generator.preset.8.gustProbability","gustProbability",8,.15,.35},
        {"generator.preset.9.gustProbability","gustProbability",9,.15,.35},
        {"generator.preset.8.correlationSeconds","correlationSeconds",8,25,70},
        {"generator.preset.9.correlationSeconds","correlationSeconds",9,25,70},
        {"generator.preset.8.activityVariation","activityVariation",8,.35,.7},
        {"generator.preset.9.activityVariation","activityVariation",9,.35,.7},
        {"generator.preset.10.strength","strength",10,.2,1},
        {"generator.preset.10.angle","angle",10,0,360},
        {"generator.preset.10.difference","difference",10,.8,1.6},
        {"generator.preset.10.temporalAmplitude","temporalAmplitude",10,.05,.3},
        {"generator.preset.10.spatialAmplitude","spatialAmplitude",10,.05,.3},
        {"generator.preset.10.trendAmplitude","trendAmplitude",10,.1,.5},
        {"generator.preset.10.trendPeriodSeconds","trendPeriodSeconds",10,150,420},
        {"generator.preset.10.gustAmplitude","gustAmplitude",10,.1,.6},
        {"generator.preset.10.gustProbability","gustProbability",10,.02,.1},
        {"generator.preset.10.correlationSeconds","correlationSeconds",10,70,180},
        {"generator.preset.10.activityVariation","activityVariation",10,.1,.25},
        {"generator.preset.0.mixedAmplitude","mixedAmplitude",0,1.2,2.5},
        {"generator.preset.1.mixedAmplitude","mixedAmplitude",1,1.2,2.5},
        {"generator.preset.2.mixedAmplitude","mixedAmplitude",2,1.2,2.5},
        {"generator.preset.3.mixedAmplitude","mixedAmplitude",3,1.2,2.5},
        {"generator.preset.4.mixedAmplitude","mixedAmplitude",4,1.2,2.5},
        {"generator.preset.5.mixedAmplitude","mixedAmplitude",5,1.2,2.5},
        {"generator.preset.6.mixedAmplitude","mixedAmplitude",6,1.2,2.5},
        {"generator.preset.7.mixedAmplitude","mixedAmplitude",7,1.2,2.5},
        {"generator.preset.8.mixedAmplitude","mixedAmplitude",8,1.2,2.5},
        {"generator.preset.9.mixedAmplitude","mixedAmplitude",9,1.2,2.5},
        {"generator.preset.10.mixedAmplitude","mixedAmplitude",10,1.2,2.5},
        {"generator.preset.0.transitionSeconds","transitionSeconds",0,4,18},
        {"generator.preset.1.transitionSeconds","transitionSeconds",1,4,18},
        {"generator.preset.2.transitionSeconds","transitionSeconds",2,4,18},
        {"generator.preset.3.transitionSeconds","transitionSeconds",3,4,18},
        {"generator.preset.4.transitionSeconds","transitionSeconds",4,4,18},
        {"generator.preset.5.transitionSeconds","transitionSeconds",5,4,18},
        {"generator.preset.6.transitionSeconds","transitionSeconds",6,4,18},
        {"generator.preset.7.transitionSeconds","transitionSeconds",7,4,18},
        {"generator.preset.8.transitionSeconds","transitionSeconds",8,4,18},
        {"generator.preset.9.transitionSeconds","transitionSeconds",9,4,18},
        {"generator.preset.10.transitionSeconds","transitionSeconds",10,4,18},
        {"generator.preset.0.correlationK","correlationK",0,.3,.9},
        {"generator.preset.1.correlationK","correlationK",1,.3,.9},
        {"generator.preset.2.correlationK","correlationK",2,.3,.9},
        {"generator.preset.3.correlationK","correlationK",3,.3,.9},
        {"generator.preset.4.correlationK","correlationK",4,.3,.9},
        {"generator.preset.5.correlationK","correlationK",5,.3,.9},
        {"generator.preset.6.correlationK","correlationK",6,.3,.9},
        {"generator.preset.7.correlationK","correlationK",7,.3,.9},
        {"generator.preset.8.correlationK","correlationK",8,.3,.9},
        {"generator.preset.9.correlationK","correlationK",9,.3,.9},
        {"generator.preset.10.correlationK","correlationK",10,.3,.9},
        {"generator.preset.0.gustWidthK","gustWidthK",0,.2,.5},
        {"generator.preset.1.gustWidthK","gustWidthK",1,.2,.5},
        {"generator.preset.2.gustWidthK","gustWidthK",2,.2,.5},
        {"generator.preset.3.gustWidthK","gustWidthK",3,.2,.5},
        {"generator.preset.4.gustWidthK","gustWidthK",4,.2,.5},
        {"generator.preset.5.gustWidthK","gustWidthK",5,.2,.5},
        {"generator.preset.6.gustWidthK","gustWidthK",6,.2,.5},
        {"generator.preset.7.gustWidthK","gustWidthK",7,.2,.5},
        {"generator.preset.8.gustWidthK","gustWidthK",8,.2,.5},
        {"generator.preset.9.gustWidthK","gustWidthK",9,.2,.5},
        {"generator.preset.10.gustWidthK","gustWidthK",10,.2,.5},
    };return result;
}
WeatherProposal proposeWeather(const WeatherProposalContext &context,RandomSource &rng,const WeatherGeneratorParameters &p)
{
    const auto get=[&](const std::string &key,double fallback){return p.get("generator.auto."+key,fallback);};
    if(!std::isfinite(context.k) || context.k<1 || context.k>400)throw std::invalid_argument("Proposal K out of range");
    if(context.variabilityPerk< -4 || context.variabilityPerk>4 || context.directionPerk< -4 || context.directionPerk>4
        || context.month<0 || context.month>12 || (context.surface!=Surface::Snow && context.surface!=Surface::Mat))
        throw std::invalid_argument("Invalid weather proposal context");
    const double headProbability[]{get("headProbability.0",.5),get("headProbability.1",.65),get("headProbability.2",.77),get("headProbability.3",.865),get("headProbability.4",.94)};
    double probability=headProbability[std::abs(context.directionPerk)];
    if(context.directionPerk<0) probability=1-probability;
    const bool head=rng.event(probability);
    const bool cross=rng.event(get("crossProbability",.15));
    const double magnitude=rng.uniform(get("strength.min",.2),get("strength.max",2.5));
    Wind base{(head?1:-1)*magnitude*(cross?.15:1),cross?(rng.event(.5)?1:-1)*magnitude:0};
    // Three broad weather characters, substantially biased by hill lottery.
    const double lottery=context.variabilityPerk/4.0;
    std::vector<double> weights;for(int i=0;i<3;++i){const double neutral[]{.45,.40,.15},stable[]{get("stable.0",.85),get("stable.1",.14),get("stable.2",.01)},unstable[]{get("unstable.0",.08),get("unstable.1",.42),get("unstable.2",.50)};double a=get("characterNeutral."+std::to_string(i),neutral[i]),b=get((lottery<0?"characterStable.":"characterLottery.")+std::to_string(i),lottery<0?stable[i]:unstable[i]);weights.push_back(a+(b-a)*std::abs(lottery));}const std::size_t character=rng.weightedIndex(weights);
    const double temporal[]{get("temporal.0",.15),get("temporal.1",.7),get("temporal.2",1.8)},spatial[]{get("spatial.0",.08),get("spatial.1",.55),get("spatial.2",1.4)},gusts[]{get("gusts.0",.3),get("gusts.1",1.8),get("gusts.2",4)};
    const double chance[]{get("chance.0",.03),get("chance.1",.25),get("chance.2",.65)},times[]{get("times.0",100),get("times.1",45),get("times.2",18)};
    WeatherProposal result;
    result.wind.temporalAmplitude=temporal[character];result.wind.spatialAmplitude=spatial[character];
    result.wind.gustAmplitude=gusts[character];result.wind.gustProbability=chance[character];
    result.wind.correlationSeconds=times[character];
    const double trendAmplitude[]{get("trendAmplitude.0",.15),get("trendAmplitude.1",1.2),get("trendAmplitude.2",2.8)},trendPeriod[]{get("trendPeriod.0",300),get("trendPeriod.1",150),get("trendPeriod.2",60)},transitions[]{get("transitions.0",30),get("transitions.1",15),get("transitions.2",5)},activity[]{get("activity.0",.15),get("activity.1",.5),get("activity.2",.95)};
    result.wind.trendAmplitude=trendAmplitude[character];result.wind.trendPeriodSeconds=trendPeriod[character];
    result.wind.trendTransitionSeconds=transitions[character];result.wind.activityVariation=activity[character];
    result.wind.correlationMetres=context.k*(character==0?get("correlationKStable",.8):get("correlationKVariable",.35));
    result.wind.gustWidthMetres=context.k*get("gustWidthK",.35);
    result.wind.references={{0,base},{context.k*.5,{base.head+rng.uniform(-spatial[character],spatial[character]),base.cross}},
        {context.k,{base.head+rng.uniform(-spatial[character],spatial[character]),base.cross+rng.uniform(-spatial[character],spatial[character])}}};
    // Broad editable seasonal suggestions, not a climate model or a forecast.
    int month=context.month;
    if(month && (context.country=="AUS" || context.country=="NZL" || context.country=="ARG" || context.country=="CHL"))
        month=(month+5)%12+1;
    const bool cold=month && (month>=11 || month<=3);
    const bool northern=context.country=="NOR" || context.country=="SWE" || context.country=="FIN" || context.country=="CAN";
    result.precipitation.kind=context.surface==Surface::Mat || (month && !cold && rng.event(get("rainProbability",.7))) ? PrecipitationKind::Rain : PrecipitationKind::Snow;
    const double wetChance=context.surface==Surface::Snow ? get("wetSnow",.25)+(cold?get("wetColdBonus",.25):0)+(northern?get("wetNorthBonus",.1):0) : get("wetMat",.30);
    if(rng.event(std::clamp(wetChance,0.0,1.0))) {
        const auto choice=rng.weightedIndex({get("precipitationPattern.0",.45),get("precipitationPattern.1",.25),get("precipitationPattern.2",.08),get("precipitationPattern.3",.22)});
        result.precipitation.scenario=static_cast<PrecipitationScenario>(choice+1);
        result.precipitation.intensity=choice==2?rng.uniform(get("heavyPrecipitation.min",8),get("heavyPrecipitation.max",10)):rng.uniform(get("precipitation.min",1),get("precipitation.max",5));
        result.precipitation.variation=choice==1?rng.uniform(get("precipitationVariation.min",2),get("precipitationVariation.max",5)):1;
    }
    const double strength=get("strengthScale",1),variation=get("variationScale",1),gust=get("gustScale",1),trend=get("trendScale",1),time=get("timeScale",1);
    for(auto &r:result.wind.references){r.value.head*=strength;r.value.cross*=strength;}
    result.wind.temporalAmplitude*=variation;result.wind.spatialAmplitude*=variation;result.wind.gustAmplitude*=gust;result.wind.trendAmplitude*=trend;
    result.wind.correlationSeconds*=time;result.wind.trendPeriodSeconds*=time;result.wind.trendTransitionSeconds*=time;
    result.wind.validate();result.precipitation.validate();
    return result;
}
const std::vector<GeneratorScalar> &weatherGeneratorScalars(){
static const std::vector<GeneratorScalar> result{
{"generator.auto.headProbability.0","headProbability",0,.5,0,1},
{"generator.auto.headProbability.1","headProbability",1,.65,0,1},
{"generator.auto.headProbability.2","headProbability",2,.77,0,1},
{"generator.auto.headProbability.3","headProbability",3,.865,0,1},
{"generator.auto.headProbability.4","headProbability",4,.94,0,1},
{"generator.auto.temporal.0","temporal",0,.15,0,100},
{"generator.auto.temporal.1","temporal",1,.7,0,100},
{"generator.auto.temporal.2","temporal",2,1.8,0,100},
{"generator.auto.chance.0","chance",0,.03,0,1},
{"generator.auto.chance.1","chance",1,.25,0,1},
{"generator.auto.chance.2","chance",2,.65,0,1},
{"generator.auto.trendAmplitude.0","trendAmplitude",0,.15,0,100},
{"generator.auto.trendAmplitude.1","trendAmplitude",1,1.2,0,100},
{"generator.auto.trendAmplitude.2","trendAmplitude",2,2.8,0,100},
{"generator.auto.crossProbability","crossProbability",-1,0.15,0,1},
{"generator.auto.strength.min","strength.min",-1,0.2,0,50},
{"generator.auto.strength.max","strength.max",-1,2.5,0,50},
{"generator.auto.correlationKStable","correlationKStable",-1,0.8,0.01,10},
{"generator.auto.correlationKVariable","correlationKVariable",-1,0.35,0.01,10},
{"generator.auto.gustWidthK","gustWidthK",-1,0.35,0.01,10},
{"generator.auto.rainProbability","rainProbability",-1,0.7,0,1},
{"generator.auto.wetSnow","wetSnow",-1,0.25,0,1},
{"generator.auto.wetColdBonus","wetColdBonus",-1,0.25,0,1},
{"generator.auto.wetNorthBonus","wetNorthBonus",-1,0.1,0,1},
{"generator.auto.wetMat","wetMat",-1,0.3,0,1},
{"generator.auto.heavyPrecipitation.min","heavyPrecipitation.min",-1,8,0,10},
{"generator.auto.heavyPrecipitation.max","heavyPrecipitation.max",-1,10,0,10},
{"generator.auto.precipitation.min","precipitation.min",-1,1,0,10},
{"generator.auto.precipitation.max","precipitation.max",-1,5,0,10},
{"generator.auto.precipitationVariation.min","precipitationVariation.min",-1,2,0,10},
{"generator.auto.precipitationVariation.max","precipitationVariation.max",-1,5,0,10},
{"generator.auto.characterNeutral.0","characterNeutral",0,0.45,0,1},
{"generator.auto.characterNeutral.1","characterNeutral",1,0.4,0,1},
{"generator.auto.characterNeutral.2","characterNeutral",2,0.15,0,1},
{"generator.auto.characterStable.0","characterStable",0,0.85,0,1},
{"generator.auto.characterStable.1","characterStable",1,0.14,0,1},
{"generator.auto.characterStable.2","characterStable",2,0.01,0,1},
{"generator.auto.characterLottery.0","characterLottery",0,0.08,0,1},
{"generator.auto.characterLottery.1","characterLottery",1,0.42,0,1},
{"generator.auto.characterLottery.2","characterLottery",2,0.5,0,1},
{"generator.auto.precipitationPattern.0","precipitationPattern",0,0.45,0,1},
{"generator.auto.precipitationPattern.1","precipitationPattern",1,0.25,0,1},
{"generator.auto.precipitationPattern.2","precipitationPattern",2,0.08,0,1},
{"generator.auto.precipitationPattern.3","precipitationPattern",3,0.22,0,1},
{"generator.auto.stable.0","stable",0,.85,0,1},
{"generator.auto.stable.1","stable",1,.14,0,1},
{"generator.auto.stable.2","stable",2,.01,0,1},
{"generator.auto.unstable.0","unstable",0,.08,0,1},
{"generator.auto.unstable.1","unstable",1,.42,0,1},
{"generator.auto.unstable.2","unstable",2,.50,0,1},
{"generator.auto.spatial.0","spatial",0,.08,0,100},
{"generator.auto.spatial.1","spatial",1,.55,0,100},
{"generator.auto.spatial.2","spatial",2,1.4,0,100},
{"generator.auto.gusts.0","gusts",0,.3,0,100},
{"generator.auto.gusts.1","gusts",1,1.8,0,100},
{"generator.auto.gusts.2","gusts",2,4,0,100},
{"generator.auto.times.0","times",0,100,0.1,100000},
{"generator.auto.times.1","times",1,45,0.1,100000},
{"generator.auto.times.2","times",2,18,0.1,100000},
{"generator.auto.trendPeriod.0","trendPeriod",0,300,0.1,100000},
{"generator.auto.trendPeriod.1","trendPeriod",1,150,0.1,100000},
{"generator.auto.trendPeriod.2","trendPeriod",2,60,0.1,100000},
{"generator.auto.transitions.0","transitions",0,30,0.1,100000},
{"generator.auto.transitions.1","transitions",1,15,0.1,100000},
{"generator.auto.transitions.2","transitions",2,5,0.1,100000},
{"generator.auto.activity.0","activity",0,.15,0,1},
{"generator.auto.activity.1","activity",1,.5,0,1},
{"generator.auto.activity.2","activity",2,.95,0,1},
};return result;
}
}
