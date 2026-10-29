#pragma once

#include "HillSkillCurve.h"
#include "RandomSource.h"
#include <array>

namespace SimulationV2 {

enum class EventKind { Training, Trial, Qualification, Competition };
enum class NoiseShape { Normal, Laplace };

struct Athlete {
    int overall=70, form=70, landing=2;
    // Training/conserving, flier/small hills, regular/unpredictable.
    std::array<int,3> perks{{0,0,0}};
    void validate() const;
};

struct QualityParameters {
    double overallWeight=.70;
    double formReferenceK=120;
    double formKScale=100;
    double formHillAmplitude=.04; // maximum change of form share, in either direction
    double negativeInfluence=.65;
    NoiseShape noise=NoiseShape::Normal;
    std::array<double,4> eventDeviation{{7,5.8,4.2,3.5}};
    std::array<double,5> specialistBias{{0,3.75,7,10.25,14}};
    std::array<double,5> trainingBonus{{0,1.125,2.25,3.5,5}};
    std::array<double,5> competitionPenalty{{0,0.5625,1.125,1.75,2.5}};
    std::array<double,5> conservedEffortPenalty{{0,2.25,4.25,6.125,8}};
    std::array<double,5> conservedEffortDeviation{{1,1.1125,1.25,1.4125,1.6}};
    std::array<double,5> regularNegative{{1,0.85,0.7,0.55,0.4}};
    std::array<double,5> regularPositive{{1,0.94,0.88,0.82,0.76}};
    std::array<double,5> unpredictableNegative{{1,1.15,1.325,1.525,1.75}};
    std::array<double,5> unpredictablePositive{{1,1.0525,1.11,1.175,1.25}};
    double hillPreferenceStrength=1;
    void validate() const;
};

struct JumpQuality {
    double overallAndForm=0;
    double takeoffWeight=0;
    double hillEffect=0;
    double eventEffect=0;
    double randomEffect=0;
    double manipulationEffect=0;
    double value() const { return overallAndForm+hillEffect+eventEffect+randomEffect+manipulationEffect; }
};

class QualityModel {
public:
    explicit QualityModel(QualityParameters parameters = {}, HillSkillCurve curve = HillSkillCurve());
    JumpQuality baseline(const Athlete &, double k, double hillTakeoffBias, EventKind) const;
    JumpQuality sample(const Athlete &, double k, double hillTakeoffBias, EventKind, RandomSource &) const;
    const QualityParameters &parameters() const { return config; }
private:
    QualityParameters config;
    HillSkillCurve curve;
};

}
