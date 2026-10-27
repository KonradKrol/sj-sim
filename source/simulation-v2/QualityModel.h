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
    double negativeInfluence=.65;
    NoiseShape noise=NoiseShape::Normal;
    std::array<double,4> eventDeviation{{7,5.8,4.2,3.5}};
    std::array<double,4> specialistBias{{0,5,9,14}};
    std::array<double,4> trainingBonus{{0,1.5,3,5}};
    std::array<double,4> competitionPenalty{{0,.75,1.5,2.5}};
    std::array<double,4> conservedEffortPenalty{{0,3,5.5,8}};
    std::array<double,4> conservedEffortDeviation{{1,1.15,1.35,1.6}};
    std::array<double,4> regularNegative{{1,.8,.6,.4}};
    std::array<double,4> regularPositive{{1,.92,.84,.76}};
    std::array<double,4> unpredictableNegative{{1,1.2,1.45,1.75}};
    std::array<double,4> unpredictablePositive{{1,1.07,1.15,1.25}};
    double hillPreferenceStrength=1;
    void validate() const;
};

struct JumpQuality {
    double overallAndForm=0;
    double takeoffWeight=0;
    double hillEffect=0;
    double eventEffect=0;
    double randomEffect=0;
    double value() const { return overallAndForm+hillEffect+eventEffect+randomEffect; }
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
