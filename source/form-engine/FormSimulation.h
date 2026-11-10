#pragma once
#include "FormEngine.h"
#include <QVector>
#include <atomic>
#include <memory>

namespace AutoForm {
struct Sample {QString name;double form=50;int perk=0;};
struct Experiment {
    Config config,reference;
    bool compare=false;
    QDate start{2026,11,1};
    int days=150,repeats=1,threads=4;
    SimulationV2::Seed seed=1;
    QVector<Sample> athletes;
    bool validate(QString &) const;
};
struct Control {std::atomic<bool> cancelled{false};std::atomic<qint64> completed{0};};
QJsonObject plotTraces(const QJsonArray &,const QVector<int> &);
QJsonObject simulate(const Experiment &,const std::shared_ptr<Control> &);
}
