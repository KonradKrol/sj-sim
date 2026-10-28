#include "FieldWindMeasurement.h"
#include "../simulator/WindsCalculator.h"
#include "../simulator/wind-generation/WindsGenerator.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace SimulationV2 {
namespace {
::Wind legacyWind(Wind value) {
    ::Wind result;result.setDirection(std::atan2(value.cross,value.head)*180/3.14159265358979323846);result.setStrength(value.strength());return result;
}
}
void FieldWindMeasurementParameters::validate() const {
    if(temporalSamples<1 || temporalSamples>257 || !std::isfinite(extentK) || extentK<.5 || extentK>2
        || !std::isfinite(denseSpacingMetres) || denseSpacingMetres<1 || denseSpacingMetres>50
        || !std::isfinite(weightPeak) || weightPeak<=0 || weightPeak>=1
        || !std::isfinite(weightStrength) || weightStrength<0 || weightStrength>10)
        throw std::invalid_argument("Invalid wind measurement parameters");
}
FieldWindMeasurement::FieldWindMeasurement(double k,const CompetitionRules &rules,FieldWindMeasurementParameters parameters):k(k),rules(rules),parameters(parameters) {
    if(!std::isfinite(k) || k<1 || k>400)throw std::invalid_argument("Invalid measurement hill K");
    parameters.validate();
}
QVector<::Wind> fieldWindSnapshot(const WeatherField &weather,double k,double seconds) {
    const int count=WindsGenerator::calculateWindsCountByKPoint(k);QVector<::Wind> winds;winds.reserve(count);
    const double segment=WindsGenerator::getWindSegmentDistance(k,count);
    for(int i=0;i<count;++i)winds.append(legacyWind(weather.wind((i+.5)*segment,seconds)));
    return winds;
}
double FieldWindMeasurement::measure(const WeatherField &weather,const FlightResult &flight,double seconds) const {
    if(!std::isfinite(seconds) || !std::isfinite(flight.duration) || !std::isfinite(flight.distance) || flight.distance<0 || flight.distance>1000
        || seconds<0 || seconds>1e12-100000 || flight.duration<0 || flight.duration>1000)
        throw std::invalid_argument("Invalid wind measurement time");
    const int mode=rules.getWindMeasurementType();
    if(mode!=CompetitionRules::LegacyWindMeasurement) {
        double measured;
        if(mode==CompetitionRules::FlightWindMeasurement)measured=flight.experiencedWind.head;
        else {
            const int count=mode==CompetitionRules::DenseWindMeasurement
                ?std::clamp(int(std::ceil(k*parameters.extentK/parameters.denseSpacingMetres)),2,128):k<100?5:k<170?7:10;
            std::vector<WindSensor> sensors;sensors.reserve(std::size_t(count));
            for(int i=0;i<count;++i) {
                const double position=(i+.5)/count;
                const double relative=position<=parameters.weightPeak?position/parameters.weightPeak:(1-position)/(1-parameters.weightPeak);
                const double weight=mode==CompetitionRules::WeightedWindMeasurement?1+parameters.weightStrength*relative:1;
                sensors.push_back({position*k*parameters.extentK,weight});
            }
            const SensorWindMeasurement network(std::move(sensors),mode==CompetitionRules::InstantWindMeasurement?1:parameters.temporalSamples);
            if(mode==CompetitionRules::InstantWindMeasurement) {auto snapshot=flight;snapshot.duration=0;measured=network.measure(weather,snapshot,seconds);}
            else measured=network.measure(weather,flight,seconds);
        }
        if(!std::isfinite(measured) || std::abs(measured)>100)throw std::invalid_argument("Invalid measured wind");
        const double rounded=std::round(measured*100)/100;return rounded==0?0:rounded;
    }
    const int count=WindsGenerator::calculateWindsCountByKPoint(k);QVector<::Wind> samples;samples.reserve(count);
    const double segment=WindsGenerator::getWindSegmentDistance(k,count);
    for(int i=0;i<count;++i) {
        Wind average;
        for(int sample=0;sample<9;++sample) {
            const auto value=weather.wind((i+.5)*segment,seconds+flight.duration*(sample+.5)/9);
            average.head+=value.head/9;average.cross+=value.cross/9;
        }
        samples.append(legacyWind(average));
    }
    return WindsCalculator::getAveragedWind(WindsCalculator::getScoringWinds(samples,flight.distance,k,rules.getWindCompensationDistanceEffect()),rules.getWindAverageCalculatingType()).getStrengthToAveragedWind();
}
}
