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
    double overallWeight=.77175;
    double overallSpread=1;
    double formDiminishingStrength=.35;
    double formReferenceK=120;
    double formKScale=100;
    double formHillAmplitude=.04; // maximum change of form share, in either direction
    double negativeInfluence=.65;
    NoiseShape noise=NoiseShape::Normal;
    std::array<double,4> eventDeviation{{6.644,5.828,4.2,3.5}};
    std::array<double,5> specialistBias{{0.0,3.0625,6.125,9.8,13.475}};
    std::array<double,5> trainingBonus{{0,1.125,2.25,3.5,5}};
    std::array<double,5> competitionPenalty{{0,0.5625,1.125,1.75,2.5}};
    std::array<double,5> conservedEffortPenalty{{0,2.25,4.25,6.125,8}};
    std::array<double,5> conservedEffortDeviation{{1,1.1125,1.25,1.4125,1.6}};
    std::array<double,5> regularNegative{{1,0.9055938917890852,0.7985049434675381,0.6241878293225928,0.3721457308704977}};
    std::array<double,5> regularPositive{{1,0.9641149466451203,0.9189868091677912,0.8781394599754577,0.8228755374243409}};
    std::array<double,5> unpredictableNegative{{1,1.133895578636703,1.27671470233168,1.432959307039079,1.660091091005461}};
    std::array<double,5> unpredictablePositive{{1,1.026746022811274,1.053271615795478,1.092023691355192,1.142328091885014}};
    std::array<double,5> regularNegativePower{{1,1.061037419630057,1.152157826965091,1.417739808817038,1.907457789176938}};
    std::array<double,5> unpredictableNegativePower{{1,0.9212056408552631,0.8842218368290771,0.8650246804035179,0.9119236838821024}};
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
