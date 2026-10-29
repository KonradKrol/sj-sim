#pragma once

#include "RandomSource.h"
#include <string>
#include <vector>

namespace SimulationV2 {

struct Wind {
    double head=0, cross=0; // m/s; positive head is favourable
    double strength() const;
};

enum class PrecipitationKind { Snow, Rain };

class WeatherField {
public:
    virtual ~WeatherField() = default;
    virtual Wind wind(double distance, double seconds) const = 0;
    virtual double precipitation(double seconds) const = 0;
    virtual PrecipitationKind precipitationKind() const { return PrecipitationKind::Snow; }
};

struct WindReference {
    double distance=0;
    Wind value;
};

struct WindParameters {
    std::vector<WindReference> references{{0,{}},{120,{}}};
    double temporalAmplitude=.25;
    double spatialAmplitude=.15;
    double correlationSeconds=60;
    double correlationMetres=50;
    double gustAmplitude=.8;
    double gustProbability=.12;
    double gustPeriodSeconds=16;
    double gustWidthMetres=45;
    double gustTravelMetresPerSecond=3;
    double maximumStrength=15;
    // Changes in the prevailing tendency, held across several jumps. Zero disables.
    double trendAmplitude=0;
    double trendPeriodSeconds=180;
    double trendTransitionSeconds=15;
    // Slowly varying episodes blend calm fluctuations and fast, chaotic ones.
    double activityVariation=0;
    double activityCorrelationSeconds=180;
    double fastCorrelationSeconds=8;
    void validate() const;
};

enum class PrecipitationScenario { None, Stable, Variable, Blizzard, Increasing };
enum class Surface { Snow, Mat };

struct PrecipitationParameters {
    PrecipitationScenario scenario=PrecipitationScenario::None;
    double intensity=0; // 0..10; independent of precipitation kind
    double variation=2;
    double correlationSeconds=120;
    double increasePerMinute=.12;
    PrecipitationKind kind=PrecipitationKind::Snow;
    void validate() const;
};

// Stateless, deterministic field queries. The same location/time always gives
// the same conditions; no dependence on worker count, UI speed or lookup order.
class ContinuousWeather final : public WeatherField {
public:
    ContinuousWeather(Seed, WindParameters = {}, PrecipitationParameters = {});
    Wind wind(double distance, double seconds) const override;
    double precipitation(double seconds) const override;
    PrecipitationKind precipitationKind() const override { return precip.kind; }
private:
    Seed seed;
    WindParameters config;
    PrecipitationParameters precip;
};

struct WeatherProposalContext {
    double k=120;
    int variabilityPerk=0; // positive lottery, negative stable, magnitude 1..4
    int directionPerk=0; // positive head, negative tail, magnitude 1..4
    Surface surface=Surface::Snow;
    int month=0; // unknown date = 0
    std::string country;
};

struct WeatherProposal {
    WindParameters wind;
    PrecipitationParameters precipitation;
};

// Hill traits influence proposals only. ContinuousWeather never reads them.
WeatherProposal proposeWeather(const WeatherProposalContext &, RandomSource &);

}
