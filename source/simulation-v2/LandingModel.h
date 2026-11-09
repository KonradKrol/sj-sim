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
    std::array<double,5> styleMean{{16.7556743,17.5556743,17.8356743,18.3956743,18.6756743}};
    std::array<double,4> landingPenalty{{0,2.6,5.6,8}};
    double sharedStyleDeviation=.35, judgeDeviation=.28;
    double judgeOutlierChance=.002;
    double highMarkCompression=.75;
    double difficultLandingStylePenalty=.25;
    double distanceStyleOffset=-.7, distanceStyleBelowK=10.5, distanceStyleAboveK=11;
    double imperfectStyleBase=.06, imperfectStyleIncrease=.15, imperfectStylePenalty=.65;
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
    std::array<double,5> judges(int landingClass, LandingKind, double distance, double k, double realHs, RandomSource &) const;
    const LandingParameters &parameters() const { return config; }
private:
    LandingParameters config;
};

}
