#pragma once
#include "PlaygroundExperiment.h"
#include "../simulation-v2/Engine.h"
#include "../simulation-v2/FieldWindMeasurement.h"

namespace Playground {
QJsonObject valuesFor(const QJsonObject &,int variant);
SimulationV2::EngineParameters engineParameters(const QJsonObject &);
SimulationV2::WindParameters windParameters(const QJsonObject &);
SimulationV2::PrecipitationParameters precipitationParameters(const QJsonObject &);
SimulationV2::FieldWindMeasurementParameters measurementParameters(const QJsonObject &);
SimulationV2::Hill hillInput(const QJsonObject &);
SimulationV2::Athlete athleteInput(const QJsonObject &,const QString &prefix);
QJsonObject diagnostics(const QJsonObject &,int variant,const std::shared_ptr<Control> &control={});
}
