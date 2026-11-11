#pragma once
#include "../simulation-v2/RandomSource.h"
#include <QDate>
#include <QJsonObject>
#include <QVariantList>
#include <array>
#include <functional>

namespace AutoForm {
constexpr int version=1;
constexpr int regimeCount=7;
struct Config {
    std::array<double,regimeCount> centers{{20,31.3333333339,41.3333333329,50,63.333333333999995,76.3333333327,89}};
    std::array<double,regimeCount> meanDays{{104,104,104,104,104,104,104}};
    std::array<double,regimeCount> entryDeviation{{4,4,4.3333333333,5,5,4.666666666699999,4}};
    std::array<std::array<double,regimeCount>,regimeCount> weights{{{{0,1,0.25,0.0625,0.015625,0.00390625,0.000146484375}},{{0.15,0,1,0.25,0.0625,0.015625,0.0005859375}},{{0.0375,1,0,1,0.25,0.0625,0.00234375}},{{0.009375,0.25,1,0,1,0.25,0.009375}},{{0.00234375,0.0625,0.25,1,0,1,0.0375}},{{0.0005859375,0.015625,0.0625,0.25,1,0,0.15}},{{0.000146484375,0.00390625,0.015625,0.0625,0.25,1,0}}}};
    double formHalfLife=13,formDeviation=13,baseHalfLife=180,baseDeviation=25,entryInertia=0.12;
    double initialDeviation=5,initialOutlierChance=0.05,initialOutlierDeviation=15;
    std::array<double,9> noise{{0.55,0.65,0.78,0.9,1,1.15,1.36,1.52,1.75}};
    std::array<double,9> crisis{{0.4,0.6,0.75,0.875,1,1.16,2,1.7,2.55}};
    std::array<double,9> peak{{0.8,0.85,0.9,0.95,1,1.1,1.2,1.35,1.55}};
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &,Config &,QString &);
    static QVariantList definitions();
    bool validate(QString &) const;
    Config withVariability(int level) const;
};
struct State {
    double form=50,base=50,remainingDays=1;
    int regime=3,visited=8,transitions=0;
    SimulationV2::Seed randomState=0;
    QDate date;
    int displayed() const;
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &,State &,QString &);
};
QString regimeName(int);
State initialize(double form,QDate,SimulationV2::Seed,const Config &);
void advance(State &,QDate,int perk,const Config &,const std::function<void(const State &)> &observer={});
std::array<double,regimeCount> transitionProbabilities(int regime,int perk,const Config &);
// Shared default for newly created saves. Existing saves keep their own snapshot.
Config defaultConfig(QString *error=nullptr);
bool saveDefaultConfig(const Config &,QString &);
}
