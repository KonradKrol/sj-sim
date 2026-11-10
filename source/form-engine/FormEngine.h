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
    std::array<double,5> meanDays{{130,110,130,95,100}};
    std::array<double,5> entryDeviation{{4,4,5,5,4}};
    std::array<std::array<double,5>,5> weights{{{{0,24,10,2,.3}},{{4,0,24,6,.5}},{{1,18,0,20,1.8}},{{.5,3,23,0,4}},{{.5,2,9,25,0}}}};
    double formHalfLife=7,formDeviation=6,baseHalfLife=180,baseDeviation=3,entryInertia=.12;
    double initialDeviation=3,initialOutlierChance=.05,initialOutlierDeviation=10;
    std::array<double,9> noise{{.55,.65,.78,.9,1,1.15,1.32,1.52,1.75}};
    std::array<double,9> crisis{{.25,.35,.5,.7,1,1.4,2,2.85,4}};
    std::array<double,9> peak{{.62,.70,.79,.89,1,1.12,1.26,1.42,1.6}};
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
