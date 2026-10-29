#include "QualityModel.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace SimulationV2 {

void Athlete::validate() const
{
    if (overall<1 || overall>100 || form<1 || form>100 || landing<0 || landing>4)
        throw std::invalid_argument("Athlete OVR/form must be 1..100 and landing 0..4");
    for (int perk:perks)
        if (perk< -3 || perk>3) throw std::invalid_argument("Athlete perks must be -3..3");
}

void QualityParameters::validate() const
{
    if (!std::isfinite(overallWeight) || overallWeight<0 || overallWeight>1
        || !std::isfinite(formReferenceK) || formReferenceK<1 || formReferenceK>400
        || !std::isfinite(formKScale) || formKScale<1 || formKScale>1000
        || !std::isfinite(formHillAmplitude) || formHillAmplitude<0 || formHillAmplitude>1
        || !std::isfinite(negativeInfluence) || negativeInfluence<0 || negativeInfluence>1
        || !std::isfinite(hillPreferenceStrength) || hillPreferenceStrength<0 || hillPreferenceStrength>10
        || (noise!=NoiseShape::Normal && noise!=NoiseShape::Laplace))
        throw std::invalid_argument("Invalid quality parameters");
    for (const auto *values:{&eventDeviation,&specialistBias,&trainingBonus,&competitionPenalty,
             &conservedEffortPenalty,&conservedEffortDeviation,&regularNegative,&regularPositive,
             &unpredictableNegative,&unpredictablePositive})
        for (double value:*values)
            if (!std::isfinite(value) || value<0 || value>100)
                throw std::invalid_argument("Quality coefficients must be finite and in [0,100]");
}

QualityModel::QualityModel(QualityParameters parameters, HillSkillCurve weights)
    : config(std::move(parameters)), curve(std::move(weights))
{
    config.validate();
}

JumpQuality QualityModel::baseline(const Athlete &athlete, double k, double hillTakeoffBias, EventKind event) const
{
    athlete.validate();
    if (!std::isfinite(hillTakeoffBias) || hillTakeoffBias< -1 || hillTakeoffBias>1
        || int(event)<0 || int(event)>3) throw std::invalid_argument("Invalid quality context");
    JumpQuality quality;
    const double formShare=std::clamp(1-config.overallWeight
        +config.formHillAmplitude*std::tanh((k-config.formReferenceK)/config.formKScale),0.0,1.0);
    quality.overallAndForm=(1-formShare)*athlete.overall+formShare*athlete.form;
    const double weight=curve.takeoffWeight(k);
    quality.takeoffWeight=std::clamp(weight+hillTakeoffBias*config.hillPreferenceStrength*std::min(weight,1-weight),0.0,1.0);
    // Equal underlying OVR/form for takeoff and flight. Only the specialist perk
    // separates them, so neutral athletes are unaffected by a hill's preference.
    const int specialist=athlete.perks[1];
    quality.hillEffect=specialist==0?0:(specialist>0?1:-1)*config.specialistBias[std::abs(specialist)]*(1-2*quality.takeoffWeight);
    const int attitude=athlete.perks[0];
    if (attitude>0) {
        quality.eventEffect=event==EventKind::Competition ? -config.competitionPenalty[attitude]
            : config.trainingBonus[attitude]*(event==EventKind::Qualification?.5:1);
    } else if (attitude<0 && (event==EventKind::Training || event==EventKind::Trial)) {
        quality.eventEffect=-config.conservedEffortPenalty[-attitude];
    }
    return quality;
}

JumpQuality QualityModel::sample(const Athlete &athlete, double k, double bias, EventKind event, RandomSource &rng) const
{
    auto quality=baseline(athlete,k,bias,event);
    double deviation=config.eventDeviation[std::size_t(event)];
    const int attitude=athlete.perks[0];
    if (attitude<0 && (event==EventKind::Training || event==EventKind::Trial))
        deviation*=config.conservedEffortDeviation[-attitude];
    double noise=config.noise==NoiseShape::Normal ? rng.normal(0,deviation) : rng.laplace(0,deviation/std::sqrt(2.0));
    const bool negative=noise<0;
    // Symmetric draws, asymmetric magnitudes: 65% means expected aggregate
    // negative influence, not a 65% probability of drawing a negative sample.
    noise*=negative?2*config.negativeInfluence:2*(1-config.negativeInfluence);
    const int regularity=athlete.perks[2];
    if (regularity>0) noise*=negative?config.regularNegative[regularity]:config.regularPositive[regularity];
    if (regularity<0) noise*=negative?config.unpredictableNegative[-regularity]:config.unpredictablePositive[-regularity];
    quality.randomEffect=noise;
    return quality;
}

}
