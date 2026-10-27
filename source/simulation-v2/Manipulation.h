#pragma once

#include "LandingModel.h"
#include <array>
#include <optional>

namespace SimulationV2 {

struct ValueRange {
    double minimum=0, maximum=0;
};

struct ValueAdjustment {
    double bonus=0;
    std::optional<ValueRange> range;
    double apply(double) const;
    bool active() const { return bonus!=0 || range.has_value(); }
    void validate(double bound) const;
};

// Explicit experimental overrides; none changes the standard engine when unset.
struct JumpManipulation {
    ValueAdjustment quality, distance, precipitation, measuredWind;
    std::optional<LandingKind> landing;
    std::array<std::optional<double>,5> judges{};
    std::optional<double> dsqProbability;
    bool active() const;
    void validate() const;
};

}
