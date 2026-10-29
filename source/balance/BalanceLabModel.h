#pragma once
#include "../playground/PlaygroundModel.h"
#include "../simulation-v2/WeatherCalibration.h"

namespace BalanceLab {
QJsonArray questions();
QJsonObject initialConfig();
QJsonObject evaluate(QJsonObject config,QJsonObject options,bool full,const std::shared_ptr<Playground::Control> &);
QJsonObject engineValues(const QJsonObject &);
SimulationV2::WeatherGeneratorParameters generatorValues(const QJsonObject &);
QJsonObject profileDiff(const QJsonObject &,const QJsonObject &);
bool validateProfile(const QJsonObject &,QString &error);
QJsonArray evaluateTargets(const QJsonObject &config,const QJsonObject &result);
}
