#include "WeatherSetup.h"
#include "ProfileBridge.h"
#include <QRandomGenerator>
#include <QDir>
#include <QUrl>
#include "../global/CountryCodes.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double pi=3.14159265358979323846;
SimulationV2::Wind mean(const SimulationV2::WindParameters &wind) {
    SimulationV2::Wind result;
    for(const auto &point:wind.references){result.head+=point.value.head/wind.references.size();result.cross+=point.value.cross/wind.references.size();}
    return result;
}
double angle(SimulationV2::Wind value) {return std::fmod(std::atan2(value.cross,value.head)*180/pi+360,360);}
SimulationV2::Wind polar(double strength,double degrees) {return {strength*std::cos(degrees*pi/180),strength*std::sin(degrees*pi/180)};}
bool bounded(double v,double lo,double hi) {return std::isfinite(v) && v>=lo && v<=hi;}
}
SimulationWeatherSetup::SimulationWeatherSetup(QObject *parent):QObject(parent) {
    seed=QRandomGenerator::global()->generate64();rebuild();
}
void SimulationWeatherSetup::setHill(const HillProfile &profile) {
    HillProfile checked;QString validationError;
    if(!HillProfile::fromJson(profile.toJson(),checked,validationError)){error=validationError;emit changed();return;}
    const bool contextChanged=hill.toJson()!=profile.toJson();
    const bool newHill=!hasHill || hill.id!=profile.id;
    const double previousK=hill.k;hill=profile;hasHill=true;
    if(newHill){suggest();return;}
    if(previousK!=hill.k) {
        for(auto &point:wind.references)point.distance*=hill.k/previousK;
        wind.correlationMetres*=hill.k/previousK;wind.gustWidthMetres*=hill.k/previousK;rebuild();
    } else if(contextChanged) {error.clear();emit changed();}
}
void SimulationWeatherSetup::clearHill() {if(!hasHill)return;hasHill=false;error.clear();emit changed();emit previewChanged();}
void SimulationWeatherSetup::inferControls() {
    variability=std::clamp(std::max({wind.temporalAmplitude/2.2,wind.gustAmplitude/4.8,wind.trendAmplitude/3.7})*100,0.0,100.0);
    const auto average=mean(wind);if(average.strength()>1e-9)requestedDirection=angle(average);double spread=0;
    for(const auto &point:wind.references)spread+=std::hypot(point.value.head-average.head,point.value.cross-average.cross)/wind.references.size();
    differences=std::clamp(std::max(wind.spatialAmplitude/1.5,spread/1.5)*100,0.0,100.0);
}
void SimulationWeatherSetup::rebuild() {
    weather=std::make_shared<SimulationV2::ContinuousWeather>(seed,wind,precipitation);
    error.clear();emit changed();emit previewChanged();
}
void SimulationWeatherSetup::regenerate(bool includePrecipitation) {
    seed=QRandomGenerator::global()->generate64();SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(seed,0));
    const auto proposed=SimulationV2::proposeWeather(SimulationV2::weatherProposalContext(hill,surface,eventDate.month()),rng);
    wind=proposed.wind;
    if(includePrecipitation)precipitation=proposed.precipitation;
    preset=-1;modified=false;inferControls();rebuild();
}
void SimulationWeatherSetup::suggest() {if(!hasHill)return;precipitationEdited=false;regenerate(true);}
void SimulationWeatherSetup::applyPreset(int index) {
    if(!hasHill || index<0 || index>4)return;
    seed=QRandomGenerator::global()->generate64();SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(seed,0));
    wind=SimulationV2::sampleWeatherPreset(static_cast<SimulationV2::WeatherPreset>(index),hill.k,rng).wind;
    preset=index;modified=false;inferControls();rebuild();
}
void SimulationWeatherSetup::reroll() {
    if(preset>=0 && !modified){applyPreset(preset);return;}
    if(preset<0 && !modified){regenerate(!precipitationEdited);return;}
    seed=QRandomGenerator::global()->generate64();rebuild();
}
QVariantMap SimulationWeatherSetup::values() const {
    const auto average=mean(wind);
    QVariantMap result{{"ready",hasHill},{"preset",preset},{"modified",modified},{"seed",QString::number(seed)},
        {"strength",average.strength()},{"direction",requestedDirection},{"variability",variability},{"differences",differences},
        {"surface",int(surface)},{"precipitationScenario",int(precipitation.scenario)},{"precipitationIntensity",precipitation.intensity},
        {"precipitationVariation",precipitation.variation},{"precipitationIncrease",precipitation.increasePerMinute},
        {"hillName",hill.name},{"hs",hill.hs},{"flag",QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(hill.country)+".svg")).toString()},{"k",hill.k}};
    result["staticWind"]=wind.temporalAmplitude==0 && wind.spatialAmplitude==0 && (wind.gustAmplitude==0 || wind.gustProbability==0) && wind.trendAmplitude==0;
    result["temporalAmplitude"]=wind.temporalAmplitude;result["correlationSeconds"]=wind.correlationSeconds;
    result["spatialAmplitude"]=wind.spatialAmplitude;result["correlationMetres"]=wind.correlationMetres;
    result["trendAmplitude"]=wind.trendAmplitude;result["trendPeriodSeconds"]=wind.trendPeriodSeconds;
    result["trendTransitionSeconds"]=wind.trendTransitionSeconds;result["activityVariation"]=wind.activityVariation;
    result["activityCorrelationSeconds"]=wind.activityCorrelationSeconds;result["fastCorrelationSeconds"]=wind.fastCorrelationSeconds;
    result["gustAmplitude"]=wind.gustAmplitude;result["gustProbability"]=wind.gustProbability;
    result["gustPeriodSeconds"]=wind.gustPeriodSeconds;result["gustWidthMetres"]=wind.gustWidthMetres;
    result["gustTravelMetresPerSecond"]=wind.gustTravelMetresPerSecond;result["maximumStrength"]=wind.maximumStrength;
    return result;
}
QVariantList SimulationWeatherSetup::references() const {
    QVariantList rows;
    for(const auto &point:wind.references)rows.append(QVariantMap{{"distance",point.distance},{"strength",point.value.strength()},{"direction",angle(point.value)}});
    return rows;
}
QVariantList SimulationWeatherSetup::preview() const {
    QVariantList rows;
    for(int i=0;i<6;++i) {
        const double distance=hill.k*1.2*(i+.5)/6;const auto value=weather->wind(distance,origin+previewTime);
        rows.append(QVariantMap{{"distance",distance},{"strength",value.strength()},{"direction",angle(value)},{"head",value.head},{"cross",value.cross}});
    }
    return rows;
}
void SimulationWeatherSetup::setPreviewOrigin(double value) {if(!bounded(value,0,1e12-100000) || origin==value)return;origin=value;emit previewChanged();}
void SimulationWeatherSetup::setPreviewSeconds(double value) {if(!bounded(value,0,3600) || previewTime==value)return;previewTime=value;emit previewChanged();}
void SimulationWeatherSetup::setValue(const QString &key,double value) {
    if(!hasHill || !std::isfinite(value))return;
    auto next=wind;auto nextPrecipitation=precipitation;
    double nextVariability=variability,nextDifferences=differences;
    bool windEdit=true;
    if(key=="strength" || key=="direction") {
        if(!bounded(value,0,key=="strength"?10:360))return;
        const auto average=mean(next);const double rotation=(value-requestedDirection)*pi/180;
        if(key=="strength") {
            const auto target=polar(value,requestedDirection);
            for(auto &point:next.references){point.value.head+=target.head-average.head;point.value.cross+=target.cross-average.cross;}
        } else for(auto &point:next.references) {
            const auto old=point.value;point.value={old.head*std::cos(rotation)-old.cross*std::sin(rotation),old.head*std::sin(rotation)+old.cross*std::cos(rotation)};
        }
    } else if(key=="variability") {
        if(!bounded(value,0,100))return;
        nextVariability=value;const double v=value/100;
        // Absolute amplitudes: stronger baseline wind never implies higher lottery.
        next.temporalAmplitude=2.2*v*v;next.trendAmplitude=3.7*v*v;next.activityVariation=v;
        next.correlationSeconds=180-165*v;next.trendPeriodSeconds=300-240*v;
        next.trendTransitionSeconds=std::min(next.trendPeriodSeconds*.3,20-17*v);
        next.gustAmplitude=4.8*v*v;next.gustProbability=.8*v;next.gustPeriodSeconds=25-15*v;
        next.fastCorrelationSeconds=12-8*v;next.spatialAmplitude=1.5*differences/100*v;
    } else if(key=="differences") {
        if(!bounded(value,0,100))return;
        nextDifferences=value;const auto average=mean(next);
        SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(seed,99));next.references.clear();
        SimulationV2::Wind deviationMean;
        for(int i=0;i<6;++i){const SimulationV2::Wind offset{rng.uniform(-1,1),rng.uniform(-1,1)};deviationMean.head+=offset.head/6;deviationMean.cross+=offset.cross/6;next.references.push_back({hill.k*1.2*i/5,offset});}
        for(auto &point:next.references){point.value.head=average.head+(point.value.head-deviationMean.head)*value*.025;point.value.cross=average.cross+(point.value.cross-deviationMean.cross)*value*.025;}
        next.spatialAmplitude=1.5*value/100*variability/100;
    } else if(key=="surface") {
        if(value!=0 && value!=1)return;
        surface=static_cast<SimulationV2::Surface>(int(value));emit changed();return;
    } else if(key.startsWith("precipitation")) {
        windEdit=false;
        if(key=="precipitationScenario") {if(value!=std::floor(value) || !bounded(value,0,4))return;nextPrecipitation.scenario=static_cast<SimulationV2::PrecipitationScenario>(int(value));if(value>0 && nextPrecipitation.intensity==0)nextPrecipitation.intensity=value==3?9:3;if(value==3)nextPrecipitation.intensity=std::max(8.0,nextPrecipitation.intensity);}
        else if(key=="precipitationIntensity")nextPrecipitation.intensity=nextPrecipitation.scenario==SimulationV2::PrecipitationScenario::Blizzard && bounded(value,0,10)?std::max(8.0,value):value;
        else if(key=="precipitationVariation")nextPrecipitation.variation=value;
        else if(key=="precipitationIncrease")nextPrecipitation.increasePerMinute=value;
        else return;
    } else {
        double *target=nullptr;
        if(key=="temporalAmplitude")target=&next.temporalAmplitude;else if(key=="correlationSeconds")target=&next.correlationSeconds;
        else if(key=="spatialAmplitude")target=&next.spatialAmplitude;else if(key=="correlationMetres")target=&next.correlationMetres;
        else if(key=="trendAmplitude")target=&next.trendAmplitude;else if(key=="trendPeriodSeconds")target=&next.trendPeriodSeconds;
        else if(key=="trendTransitionSeconds")target=&next.trendTransitionSeconds;else if(key=="activityVariation")target=&next.activityVariation;
        else if(key=="activityCorrelationSeconds")target=&next.activityCorrelationSeconds;else if(key=="fastCorrelationSeconds")target=&next.fastCorrelationSeconds;
        else if(key=="gustAmplitude")target=&next.gustAmplitude;else if(key=="gustProbability")target=&next.gustProbability;
        else if(key=="gustPeriodSeconds")target=&next.gustPeriodSeconds;else if(key=="gustWidthMetres")target=&next.gustWidthMetres;
        else if(key=="gustTravelMetresPerSecond")target=&next.gustTravelMetresPerSecond;else if(key=="maximumStrength")target=&next.maximumStrength;
        if(!target)return;
        *target=value;
        if(key=="trendPeriodSeconds")next.trendTransitionSeconds=std::min(next.trendTransitionSeconds,value*.3);
    }
    try {next.validate();nextPrecipitation.validate();}
    catch(const std::invalid_argument &){error=tr("Wartość poza zakresem. Zachowano poprzednie ustawienia.");emit changed();return;}
    wind=std::move(next);precipitation=nextPrecipitation;
    if(key=="direction")requestedDirection=std::fmod(value,360);
    if(windEdit){modified=true;inferControls();if(key=="variability")variability=nextVariability;if(key=="differences")differences=nextDifferences;}
    else precipitationEdited=true;
    rebuild();
}
void SimulationWeatherSetup::setReference(int index,const QString &key,double value) {
    if(!hasHill || index<0 || index>=int(wind.references.size()) || !std::isfinite(value))return;
    auto next=wind;auto &point=next.references[std::size_t(index)];
    if(key=="distance" && bounded(value,0,hill.k*1.5))point.distance=value;
    else if(key=="strength" && bounded(value,0,15))point.value=polar(value,angle(point.value));
    else if(key=="direction" && bounded(value,0,360))point.value=polar(point.value.strength(),value);
    else return;
    try {next.validate();}catch(const std::invalid_argument &){error=tr("Punkty muszą być ustawione w rosnącej kolejności odległości.");emit changed();return;}
    wind=std::move(next);modified=true;inferControls();rebuild();
}
