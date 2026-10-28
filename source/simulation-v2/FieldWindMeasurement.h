#pragma once
#include "Scoring.h"
#include "../competitions/CompetitionRules.h"
#include "../simulator/Wind.h"
#include <QVector>

namespace SimulationV2 {
struct FieldWindMeasurementParameters {
    int temporalSamples=17;
    double extentK=1.1,denseSpacingMetres=5,weightPeak=.65,weightStrength=1.5;
    void validate() const;
};

// Separate selectable measurement from physical flight. Legacy settings retain
// their original interpretation. Additional models are experimental, not FIS.
class FieldWindMeasurement final:public WindMeasurement {
public:
    FieldWindMeasurement(double k,const CompetitionRules &,FieldWindMeasurementParameters={});
    double measure(const WeatherField &,const FlightResult &,double startSeconds) const override;
private:
    double k;
    CompetitionRules rules;
    FieldWindMeasurementParameters parameters;
};
QVector<::Wind> fieldWindSnapshot(const WeatherField &,double k,double seconds);
}
