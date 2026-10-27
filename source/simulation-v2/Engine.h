#pragma once

#include "LandingModel.h"
#include "QualityModel.h"
#include "Scoring.h"

namespace SimulationV2 {

struct EngineParameters {
    QualityParameters quality;
    std::vector<SkillAnchor> skillAnchors=HillSkillCurve::defaultAnchors();
    CurveExtension curveExtension=CurveExtension::Constant;
    FlightParameters flight;
    LandingParameters landing;
};

struct JumpRequest {
    Athlete athlete;
    Hill hill;
    EventKind event=EventKind::Competition;
    ScoringRules scoring;
    double startSeconds=0;
    double dsqProbability=0;
    bool skip=false;
    bool captureExposure=true;
};

enum class JumpStatus { Completed, Disqualified, DidNotStart };

struct JumpResult {
    static constexpr int engineVersion=2;
    JumpStatus status=JumpStatus::Completed;
    JumpQuality quality;
    FlightResult flight;
    double measuredDistance=0;
    LandingKind landing=LandingKind::Telemark;
    LandingProbabilities landingProbabilities;
    std::array<double,5> judges{};
    Score score;
};

class Engine {
public:
    explicit Engine(const EngineParameters & = {});
    JumpResult simulate(const JumpRequest &, RandomSource &, const WeatherField &, const WindMeasurement &) const;
    // No randomness consumed: same quality/weather model as the full jump.
    double previewDistance(const JumpRequest &, const WeatherField &) const;
    const QualityModel &quality() const { return qualityModel; }
    const FlightModel &flight() const { return flightModel; }
    const LandingModel &landing() const { return landingModel; }
private:
    QualityModel qualityModel;
    FlightModel flightModel;
    LandingModel landingModel;
};

}
