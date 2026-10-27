#pragma once

#include "Weather.h"
#include <vector>

namespace SimulationV2 {

struct Hill {
    double k=120, hs=134, realHs=134, takeoffBias=0;
    double pointsAtK=60, pointsPerMetre=1.8, gatePoints=5;
    double headwindPoints=8, tailwindPoints=12;
    void validate() const;
};

struct FlightParameters {
    double referenceQuality=70;
    double referenceDistanceAtK=.95;
    double qualityMetresAtK120=1.1071428571428572;
    int referenceGate=10;
    double gateMetresAtK120=2.5;
    double gateTailwindInteraction=.04;
    double precipitationLossPerLevel=.008;
    double stepMetres=1;
    double secondsPerK=4; // time coordinate only; no trajectory/speed simulation
    double windReservePerMetre=.055;
    double crosswindDrag=.002;
    double windResponseMetres=5;
    double earlyWindSensitivity=.55;
    double lateWindSensitivity=1.1;
    double maximumWindAssistance=.65;
    double geometryStartRealHs=.94;
    double usableLandingBeyondRealHsAtK=.12;
    double geometryResistance=1.8;
    int exposureSections=6;
    void validate() const;
};

struct WindExposure {
    double begin=0, end=0, traversed=0;
    Wind before, experienced, after;
    double minimumHead=0, maximumHead=0;
};

struct FlightResult {
    double distance=0, duration=0, reserve=0, precipitation=0;
    Wind experiencedWind;
    double meanAbsoluteCrosswind=0;
    std::vector<WindExposure> exposures;
    int integrationSteps=0;
};

class FlightModel {
public:
    explicit FlightModel(FlightParameters = {});
    double reserve(double quality, const Hill &, int gate, double precipitation, double initialHeadwind) const;
    double landingLimit(const Hill &) const;
    FlightResult fly(double reserve, const Hill &, const WeatherField &, double startSeconds, bool captureExposure=true) const;
    const FlightParameters &parameters() const { return config; }
private:
    FlightParameters config;
};

}
