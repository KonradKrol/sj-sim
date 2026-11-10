#pragma once
#include "../simulation-v2/RandomSource.h"
#include <QDate>
#include <QJsonObject>
#include <QVariantList>
#include <array>
#include <functional>

namespace AutoForm {
constexpr int version=1;
constexpr int regimeCount=5;
struct Config {
    std::array<double,5> centers{{20,37,50,70,89}};
    std::array<double,5> meanDays{{160,160,160,160,160}};
    std::array<double,5> entryDeviation{{4,4,5,5,4}};
    std::array<std::array<double,5>,5> weights{{{{0,35,10,1.5,0.2}},{{5,0,30,4,0.5}},{{1,18,0,20,1.5}},{{0.5,3,22,0,2}},{{0.5,2,15,36,0}}}};
    double formHalfLife=13,formDeviation=6,baseHalfLife=180,baseDeviation=10,entryInertia=0.12;
    double initialDeviation=5,initialOutlierChance=0.05,initialOutlierDeviation=15;
    std::array<double,9> noise{{0.55,0.65,0.78,0.9,1,1.15,1.32,1.52,1.75}};
    std::array<double,9> crisis{{0.325,0.525,0.725,0.875,1,1.4,2,2.85,4}};
    std::array<double,9> peak{{0.8,0.85,0.9,0.95,1,1.12,1.26,1.42,1.6}};
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &,Config &,QString &);
    static QVariantList definitions();
    bool validate(QString &) const;
};
struct State {
    double form=50,base=50,remainingDays=1;
    int regime=2,visited=4,transitions=0;
    SimulationV2::Seed randomState=0;
    QDate date;
    int displayed() const;
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &,State &,QString &);
};
QString regimeName(int);
State initialize(double form,QDate,SimulationV2::Seed,const Config &);
void advance(State &,QDate,int perk,const Config &,const std::function<void(const State &)> &observer={});
std::array<double,5> transitionProbabilities(int regime,int perk,const Config &);
// Shared default for newly created saves. Existing saves keep their own snapshot.
Config defaultConfig(QString *error=nullptr);
bool saveDefaultConfig(const Config &,QString &);
}
