#include "FieldWindMeasurement.h"
#include "../simulator/WindsCalculator.h"
#include "../simulator/wind-generation/WindsGenerator.h"
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
namespace {
::Wind legacyWind(Wind value) {
    ::Wind result;result.setDirection(std::atan2(value.cross,value.head)*180/3.14159265358979323846);result.setStrength(value.strength());return result;
}
}
FieldWindMeasurement::FieldWindMeasurement(double k,const CompetitionRules &rules):k(k),rules(rules) {
    if(!std::isfinite(k) || k<1 || k>400)throw std::invalid_argument("Invalid measurement hill K");
}
QVector<::Wind> fieldWindSnapshot(const WeatherField &weather,double k,double seconds) {
    const int count=WindsGenerator::calculateWindsCountByKPoint(k);QVector<::Wind> winds;winds.reserve(count);
    const double segment=WindsGenerator::getWindSegmentDistance(k,count);
    for(int i=0;i<count;++i)winds.append(legacyWind(weather.wind((i+.5)*segment,seconds)));
    return winds;
}
double FieldWindMeasurement::measure(const WeatherField &weather,const FlightResult &flight,double seconds) const {
    if(!std::isfinite(seconds) || !std::isfinite(flight.duration) || seconds<0 || flight.duration<0 || flight.duration>1000)
        throw std::invalid_argument("Invalid wind measurement time");
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
