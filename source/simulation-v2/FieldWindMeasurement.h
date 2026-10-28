#pragma once
#include "Scoring.h"
#include "../competitions/CompetitionRules.h"
#include "../simulator/Wind.h"
#include <QVector>

namespace SimulationV2 {
// Retains existing selectable averaging/distance policies. Temporal station
// samples now come from the continuous field, independently of flown exposure.
// This is a compatibility policy, not a claim of exact FIS measurement.
class FieldWindMeasurement final:public WindMeasurement {
public:
    FieldWindMeasurement(double k,const CompetitionRules &);
    double measure(const WeatherField &,const FlightResult &,double startSeconds) const override;
private:
    double k;
    CompetitionRules rules;
};
QVector<::Wind> fieldWindSnapshot(const WeatherField &,double k,double seconds);
}
