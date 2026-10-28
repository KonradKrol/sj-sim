#pragma once
#include "Weather.h"

namespace SimulationV2 {
enum class WeatherPreset { StableTailwind, StrongHeadwind, Lottery, VariableCrosswind, Calm,
    StableHeadwind, StrongTailwind, StableCrosswind, VariableHeadwind, VariableTailwind, MixedSlope };
inline constexpr int weatherPresetCount=11;
// Each named scenario draws a fresh, bounded parameter set. Hill traits belong
// to automatic proposals only; choosing a named scenario overrides that suggestion.
WeatherProposal sampleWeatherPreset(WeatherPreset,double k,RandomSource &);
}
