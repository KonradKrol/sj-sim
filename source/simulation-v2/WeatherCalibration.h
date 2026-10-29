#pragma once
#include "WeatherPresets.h"
#include <map>
#include <string>
#include <vector>

namespace SimulationV2 {
struct GeneratorRange {
    std::string key, field;
    int preset;
    double low, high;
};
struct GeneratorScalar {std::string key,field;int level;double value,minimum,maximum;};
struct WeatherGeneratorParameters {
    std::map<std::string,double> values;
    double get(const std::string &key,double fallback) const;
};
const std::vector<GeneratorRange> &weatherGeneratorRanges();
const std::vector<GeneratorScalar> &weatherGeneratorScalars();
WeatherProposal sampleWeatherPreset(WeatherPreset,double,RandomSource &,const WeatherGeneratorParameters &);
WeatherProposal proposeWeather(const WeatherProposalContext &,RandomSource &,const WeatherGeneratorParameters &);
}
