#include "WeatherPresets.h"
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
WeatherProposal sampleWeatherPreset(WeatherPreset preset,double k,RandomSource &rng)
{
    if(!std::isfinite(k) || k<1 || k>400 || int(preset)<0 || int(preset)>=weatherPresetCount)
        throw std::invalid_argument("Invalid weather preset or hill");
    WeatherProposal result;auto &w=result.wind;
    double strength=0,angle=0,difference=0;
    switch(preset) {
    case WeatherPreset::StableTailwind:
        strength=rng.uniform(.6,2.3);angle=180+rng.uniform(-5,5);difference=rng.uniform(.01,.06);
        w.temporalAmplitude=rng.uniform(.02,.10);w.spatialAmplitude=rng.uniform(.01,.06);
        w.trendAmplitude=rng.uniform(.04,.18);w.trendPeriodSeconds=rng.uniform(180,480);
        w.gustAmplitude=rng.uniform(.05,.20);w.gustProbability=rng.uniform(.01,.04);
        w.correlationSeconds=rng.uniform(80,180);w.activityVariation=rng.uniform(0,.15);break;
    case WeatherPreset::StrongHeadwind:
        strength=rng.uniform(2,4);angle=rng.uniform(-8,8);difference=rng.uniform(.03,.25);
        w.temporalAmplitude=rng.uniform(.04,.45);w.spatialAmplitude=rng.uniform(.03,.30);
        w.trendAmplitude=rng.uniform(.12,.65);w.trendPeriodSeconds=rng.uniform(120,360);
        w.gustAmplitude=rng.uniform(.2,1);w.gustProbability=rng.uniform(.04,.12);
        w.correlationSeconds=rng.uniform(45,150);w.activityVariation=rng.uniform(.1,.35);break;
    case WeatherPreset::Lottery:
        strength=rng.uniform(.2,2.5);angle=rng.uniform(0,360);difference=rng.uniform(.7,1.5);
        w.temporalAmplitude=rng.uniform(.9,2.2);w.spatialAmplitude=rng.uniform(.7,1.5);
        w.trendAmplitude=rng.uniform(2,3.7);w.trendPeriodSeconds=rng.uniform(35,105);
        w.gustAmplitude=rng.uniform(2,4.8);w.gustProbability=rng.uniform(.4,.8);
        w.correlationSeconds=rng.uniform(12,35);w.activityVariation=rng.uniform(.7,1);break;
    case WeatherPreset::VariableCrosswind:
        strength=rng.uniform(1,3);angle=(rng.event(.5)?90:270)+rng.uniform(-20,20);difference=rng.uniform(.3,.9);
        w.temporalAmplitude=rng.uniform(.5,1.2);w.spatialAmplitude=rng.uniform(.4,.9);
        w.trendAmplitude=rng.uniform(.4,1.4);w.trendPeriodSeconds=rng.uniform(60,180);
        w.gustAmplitude=rng.uniform(1.2,3);w.gustProbability=rng.uniform(.2,.5);
        w.correlationSeconds=rng.uniform(20,65);w.activityVariation=rng.uniform(.4,.8);break;
    case WeatherPreset::Calm:
        strength=rng.event(.85)?rng.uniform(.1,.3):rng.uniform(.03,.5);angle=rng.uniform(0,360);difference=rng.uniform(.002,.015);
        w.temporalAmplitude=rng.uniform(.003,.025);w.spatialAmplitude=rng.uniform(.002,.015);
        w.trendAmplitude=rng.uniform(.005,.025);w.trendPeriodSeconds=rng.uniform(180,600);
        w.gustAmplitude=rng.uniform(.005,.04);w.gustProbability=rng.uniform(.005,.02);
        w.correlationSeconds=rng.uniform(100,240);w.activityVariation=rng.uniform(0,.1);break;
    case WeatherPreset::StableHeadwind:
        strength=rng.uniform(.4,1.7);angle=rng.uniform(-6,6);difference=rng.uniform(.01,.08);
        w.temporalAmplitude=rng.uniform(.02,.12);w.spatialAmplitude=rng.uniform(.01,.07);
        w.trendAmplitude=rng.uniform(.04,.2);w.trendPeriodSeconds=rng.uniform(180,480);
        w.gustAmplitude=rng.uniform(.05,.25);w.gustProbability=rng.uniform(.01,.05);
        w.correlationSeconds=rng.uniform(80,180);w.activityVariation=rng.uniform(0,.15);break;
    case WeatherPreset::StrongTailwind:
        strength=rng.uniform(2,4);angle=180+rng.uniform(-8,8);difference=rng.uniform(.03,.2);
        w.temporalAmplitude=rng.uniform(.04,.25);w.spatialAmplitude=rng.uniform(.03,.15);
        w.trendAmplitude=rng.uniform(.1,.4);w.trendPeriodSeconds=rng.uniform(150,420);
        w.gustAmplitude=rng.uniform(.15,.6);w.gustProbability=rng.uniform(.02,.08);
        w.correlationSeconds=rng.uniform(70,180);w.activityVariation=rng.uniform(.05,.2);break;
    case WeatherPreset::StableCrosswind:
        strength=rng.uniform(.5,2.5);angle=(rng.event(.5)?90:270)+rng.uniform(-8,8);difference=rng.uniform(.02,.12);
        w.temporalAmplitude=rng.uniform(.02,.12);w.spatialAmplitude=rng.uniform(.01,.08);
        w.trendAmplitude=rng.uniform(.04,.2);w.trendPeriodSeconds=rng.uniform(180,480);
        w.gustAmplitude=rng.uniform(.05,.25);w.gustProbability=rng.uniform(.01,.05);
        w.correlationSeconds=rng.uniform(80,180);w.activityVariation=rng.uniform(0,.15);break;
    case WeatherPreset::VariableHeadwind:
    case WeatherPreset::VariableTailwind:
        strength=rng.uniform(.6,2.2);angle=(preset==WeatherPreset::VariableHeadwind?0:180)+rng.uniform(-15,15);difference=rng.uniform(.15,.6);
        w.temporalAmplitude=rng.uniform(.4,1);w.spatialAmplitude=rng.uniform(.15,.5);
        w.trendAmplitude=rng.uniform(.7,1.7);w.trendPeriodSeconds=rng.uniform(60,180);
        w.gustAmplitude=rng.uniform(.7,1.9);w.gustProbability=rng.uniform(.15,.35);
        w.correlationSeconds=rng.uniform(25,70);w.activityVariation=rng.uniform(.35,.7);break;
    case WeatherPreset::MixedSlope:
        strength=rng.uniform(.2,1);angle=rng.uniform(0,360);difference=rng.uniform(.8,1.6);
        w.temporalAmplitude=rng.uniform(.05,.3);w.spatialAmplitude=rng.uniform(.05,.3);
        w.trendAmplitude=rng.uniform(.1,.5);w.trendPeriodSeconds=rng.uniform(150,420);
        w.gustAmplitude=rng.uniform(.1,.6);w.gustProbability=rng.uniform(.02,.1);
        w.correlationSeconds=rng.uniform(70,180);w.activityVariation=rng.uniform(.1,.25);break;
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
        const double amplitude=rng.uniform(1.2,2.5),orientation=rng.event(.5)?1:-1;
        for(std::size_t i=0;i<w.references.size();++i)
            w.references[i].value.head+=orientation*amplitude*(1-2*double(i)/(w.references.size()-1));
    }
    w.trendTransitionSeconds=std::min(w.trendPeriodSeconds*.2,rng.uniform(4,18));
    w.correlationMetres=k*rng.uniform(.3,.9);w.gustWidthMetres=k*rng.uniform(.2,.5);
    w.validate();return result;
}
}
