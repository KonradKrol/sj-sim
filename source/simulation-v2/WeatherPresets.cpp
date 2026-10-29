#include "WeatherCalibration.h"
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
WeatherProposal sampleWeatherPreset(WeatherPreset preset,double k,RandomSource &rng) { return sampleWeatherPreset(preset,k,rng,{}); }
WeatherProposal sampleWeatherPreset(WeatherPreset preset,double k,RandomSource &rng,const WeatherGeneratorParameters &parameters)
{
    const auto draw=[&](const std::string &field,double lo,double hi) {
        const auto key="generator.preset."+std::to_string(int(preset))+"."+field;
        const double low=parameters.get(key+".min",lo),high=parameters.get(key+".max",hi);
        if(!std::isfinite(low) || !std::isfinite(high) || low>high)throw std::invalid_argument("Invalid preset generator interval");
        const double u=rng.unit();return low*(1-u)+high*u;
    };
    if(!std::isfinite(k) || k<1 || k>400 || int(preset)<0 || int(preset)>=weatherPresetCount)
        throw std::invalid_argument("Invalid weather preset or hill");
    WeatherProposal result;auto &w=result.wind;
    double strength=0,angle=0,difference=0;
    switch(preset) {
    case WeatherPreset::StableTailwind:
        strength=draw("strength",.6,2.3);angle=180+draw("angle",-5,5);difference=draw("difference",.01,.06);
        w.temporalAmplitude=draw("temporalAmplitude",.02,.10);w.spatialAmplitude=draw("spatialAmplitude",.01,.06);
        w.trendAmplitude=draw("trendAmplitude",.04,.18);w.trendPeriodSeconds=draw("trendPeriodSeconds",180,480);
        w.gustAmplitude=draw("gustAmplitude",.05,.20);w.gustProbability=draw("gustProbability",.01,.04);
        w.correlationSeconds=draw("correlationSeconds",80,180);w.activityVariation=draw("activityVariation",0,.15);break;
    case WeatherPreset::StrongHeadwind:
        strength=draw("strength",2,4);angle=draw("angle",-8,8);difference=draw("difference",.03,.25);
        w.temporalAmplitude=draw("temporalAmplitude",.04,.45);w.spatialAmplitude=draw("spatialAmplitude",.03,.30);
        w.trendAmplitude=draw("trendAmplitude",.12,.65);w.trendPeriodSeconds=draw("trendPeriodSeconds",120,360);
        w.gustAmplitude=draw("gustAmplitude",.2,1);w.gustProbability=draw("gustProbability",.04,.12);
        w.correlationSeconds=draw("correlationSeconds",45,150);w.activityVariation=draw("activityVariation",.1,.35);break;
    case WeatherPreset::Lottery:
        strength=draw("strength",.2,2.5);angle=draw("angle",0,360);difference=draw("difference",.7,1.5);
        w.temporalAmplitude=draw("temporalAmplitude",.9,2.2);w.spatialAmplitude=draw("spatialAmplitude",.7,1.5);
        w.trendAmplitude=draw("trendAmplitude",2,3.7);w.trendPeriodSeconds=draw("trendPeriodSeconds",35,105);
        w.gustAmplitude=draw("gustAmplitude",2,4.8);w.gustProbability=draw("gustProbability",.4,.8);
        w.correlationSeconds=draw("correlationSeconds",12,35);w.activityVariation=draw("activityVariation",.7,1);break;
    case WeatherPreset::VariableCrosswind:
        strength=draw("strength",1,3);angle=(rng.event(.5)?90:270)+draw("angle",-20,20);difference=draw("difference",.3,.9);
        w.temporalAmplitude=draw("temporalAmplitude",.5,1.2);w.spatialAmplitude=draw("spatialAmplitude",.4,.9);
        w.trendAmplitude=draw("trendAmplitude",.4,1.4);w.trendPeriodSeconds=draw("trendPeriodSeconds",60,180);
        w.gustAmplitude=draw("gustAmplitude",1.2,3);w.gustProbability=draw("gustProbability",.2,.5);
        w.correlationSeconds=draw("correlationSeconds",20,65);w.activityVariation=draw("activityVariation",.4,.8);break;
    case WeatherPreset::Calm:
        strength=rng.event(.85)?draw("strength",.1,.3):draw("strength2",.03,.5);angle=draw("angle",0,360);difference=draw("difference",.002,.015);
        w.temporalAmplitude=draw("temporalAmplitude",.003,.025);w.spatialAmplitude=draw("spatialAmplitude",.002,.015);
        w.trendAmplitude=draw("trendAmplitude",.005,.025);w.trendPeriodSeconds=draw("trendPeriodSeconds",180,600);
        w.gustAmplitude=draw("gustAmplitude",.005,.04);w.gustProbability=draw("gustProbability",.005,.02);
        w.correlationSeconds=draw("correlationSeconds",100,240);w.activityVariation=draw("activityVariation",0,.1);break;
    case WeatherPreset::StableHeadwind:
        strength=draw("strength",.4,1.7);angle=draw("angle",-6,6);difference=draw("difference",.01,.08);
        w.temporalAmplitude=draw("temporalAmplitude",.02,.12);w.spatialAmplitude=draw("spatialAmplitude",.01,.07);
        w.trendAmplitude=draw("trendAmplitude",.04,.2);w.trendPeriodSeconds=draw("trendPeriodSeconds",180,480);
        w.gustAmplitude=draw("gustAmplitude",.05,.25);w.gustProbability=draw("gustProbability",.01,.05);
        w.correlationSeconds=draw("correlationSeconds",80,180);w.activityVariation=draw("activityVariation",0,.15);break;
    case WeatherPreset::StrongTailwind:
        strength=draw("strength",2,4);angle=180+draw("angle",-8,8);difference=draw("difference",.03,.2);
        w.temporalAmplitude=draw("temporalAmplitude",.04,.25);w.spatialAmplitude=draw("spatialAmplitude",.03,.15);
        w.trendAmplitude=draw("trendAmplitude",.1,.4);w.trendPeriodSeconds=draw("trendPeriodSeconds",150,420);
        w.gustAmplitude=draw("gustAmplitude",.15,.6);w.gustProbability=draw("gustProbability",.02,.08);
        w.correlationSeconds=draw("correlationSeconds",70,180);w.activityVariation=draw("activityVariation",.05,.2);break;
    case WeatherPreset::StableCrosswind:
        strength=draw("strength",.5,2.5);angle=(rng.event(.5)?90:270)+draw("angle",-8,8);difference=draw("difference",.02,.12);
        w.temporalAmplitude=draw("temporalAmplitude",.02,.12);w.spatialAmplitude=draw("spatialAmplitude",.01,.08);
        w.trendAmplitude=draw("trendAmplitude",.04,.2);w.trendPeriodSeconds=draw("trendPeriodSeconds",180,480);
        w.gustAmplitude=draw("gustAmplitude",.05,.25);w.gustProbability=draw("gustProbability",.01,.05);
        w.correlationSeconds=draw("correlationSeconds",80,180);w.activityVariation=draw("activityVariation",0,.15);break;
    case WeatherPreset::VariableHeadwind:
    case WeatherPreset::VariableTailwind:
        strength=draw("strength",.6,2.2);angle=(preset==WeatherPreset::VariableHeadwind?0:180)+draw("angle",-15,15);difference=draw("difference",.15,.6);
        w.temporalAmplitude=draw("temporalAmplitude",.4,1);w.spatialAmplitude=draw("spatialAmplitude",.15,.5);
        w.trendAmplitude=draw("trendAmplitude",.7,1.7);w.trendPeriodSeconds=draw("trendPeriodSeconds",60,180);
        w.gustAmplitude=draw("gustAmplitude",.7,1.9);w.gustProbability=draw("gustProbability",.15,.35);
        w.correlationSeconds=draw("correlationSeconds",25,70);w.activityVariation=draw("activityVariation",.35,.7);break;
    case WeatherPreset::MixedSlope:
        strength=draw("strength",.2,1);angle=draw("angle",0,360);difference=draw("difference",.8,1.6);
        w.temporalAmplitude=draw("temporalAmplitude",.05,.3);w.spatialAmplitude=draw("spatialAmplitude",.05,.3);
        w.trendAmplitude=draw("trendAmplitude",.1,.5);w.trendPeriodSeconds=draw("trendPeriodSeconds",150,420);
        w.gustAmplitude=draw("gustAmplitude",.1,.6);w.gustProbability=draw("gustProbability",.02,.1);
        w.correlationSeconds=draw("correlationSeconds",70,180);w.activityVariation=draw("activityVariation",.1,.25);break;
    }
    const double radians=angle*3.14159265358979323846/180;
    const Wind base{strength*std::cos(radians),strength*std::sin(radians)};
    w.references.clear();
    Wind offsetMean;
    for(int i=0;i<6;++i) {
        const Wind offset{rng.uniform(-difference,difference),rng.uniform(-difference,difference)};
        offsetMean.head+=offset.head/6;offsetMean.cross+=offset.cross/6;
        w.references.push_back({k*1.2*i/5,offset});
    }
    for(auto &point:w.references){point.value.head+=base.head-offsetMean.head;point.value.cross+=base.cross-offsetMean.cross;}
    if(preset==WeatherPreset::MixedSlope) {
        const double amplitude=draw("mixedAmplitude",1.2,2.5),orientation=rng.event(.5)?1:-1;
        for(std::size_t i=0;i<w.references.size();++i)
            w.references[i].value.head+=orientation*amplitude*(1-2*double(i)/(w.references.size()-1));
    }
    w.trendTransitionSeconds=std::min(w.trendPeriodSeconds*.2,draw("transitionSeconds",4,18));
    w.correlationMetres=k*draw("correlationK",.3,.9);w.gustWidthMetres=k*draw("gustWidthK",.2,.5);
    w.validate();return result;
}
}
