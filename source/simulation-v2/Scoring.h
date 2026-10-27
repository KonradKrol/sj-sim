#pragma once

#include "FlightModel.h"
#include <array>
#include <vector>

namespace SimulationV2 {

class WindMeasurement {
public:
    virtual ~WindMeasurement() = default;
    virtual double measure(const WeatherField &, const FlightResult &, double startSeconds) const = 0;
};

struct WindSensor {
    double distance=0, weight=1;
};

// Configurable experimental measurement, not a claim of exact FIS compliance.
// Explicit station layout/weights and time window; independent of flown exposure.
class SensorWindMeasurement final : public WindMeasurement {
public:
    SensorWindMeasurement(std::vector<WindSensor>, int temporalSamples=9,
                          double beforeTakeoffSeconds=0, double afterLandingSeconds=0,
                          bool onlyReachedSensors=false);
    double measure(const WeatherField &,const FlightResult &,double startSeconds) const override;
private:
    std::vector<WindSensor> sensors;
    int temporalSamples;
    double before,after;
    bool reachedOnly;
};

struct ScoringRules {
    int juryGate=10, referenceGate=10, coachReduction=0;
    bool windCompensation=true, gateCompensation=true, judges=true, coach95PercentRule=true;
    void validate() const;
};

struct Score {
    double distance=0, judges=0, wind=0, gate=0, total=0, measuredWind=0;
    bool coachThresholdReached=false;
};

Score scoreJump(double measuredDistance,const std::array<double,5> &,double measuredWind,const Hill &,const ScoringRules &);

}
