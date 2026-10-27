#pragma once

#include "FlightModel.h"
#include "RandomSource.h"
#include <array>

namespace SimulationV2 {

enum class LandingKind { Telemark, TwoFeet, Support, Fall };

struct LandingParameters {
    double fallBase=.001, fallIncrease=.30, fallCentre=1.035, fallWidth=.025;
    double supportBase=.003, supportIncrease=.18, supportCentre=1.01, supportWidth=.035;
    double twoFeetBase=.035, twoFeetIncrease=.70, twoFeetCentre=.995, twoFeetWidth=.035;
    double crosswindFallPerMetrePerSecond=.003;
    double crosswindSupportPerMetrePerSecond=.008;
    double crosswindTwoFeetPerMetrePerSecond=.015;
    std::array<double,5> styleMean{{16.5,17.25,18,18.75,19.5}};
    std::array<double,4> landingPenalty{{0,2.5,5,7.5}};
    double sharedStyleDeviation=.35, judgeDeviation=.25;
    double difficultLandingStylePenalty=1.2;
    void validate() const;
};

struct LandingProbabilities {
    std::array<double,4> values{}; // telemark, two feet, support, fall; sums to 1
};

class LandingModel {
public:
    explicit LandingModel(LandingParameters = {});
    LandingProbabilities probabilities(double distance, double realHs, double crosswind) const;
    LandingKind sample(double distance, double realHs, double crosswind, RandomSource &) const;
    std::array<double,5> judges(int landingClass, LandingKind, double distance, double realHs, RandomSource &) const;
    const LandingParameters &parameters() const { return config; }
private:
    LandingParameters config;
};

}
