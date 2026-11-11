#pragma once
#include "../simulation-v2/RandomSource.h"
#include <QString>
#include <algorithm>
#include <cmath>
namespace AutoForm {
inline double dailyBonus(SimulationV2::Seed daySeed,const QString &athleteId){
    SimulationV2::Seed hash=14695981039346656037ULL;
    for(unsigned char byte:athleteId.toUtf8()){hash^=byte;hash*=1099511628211ULL;}
    SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(daySeed,hash));
    return rng.laplace(0,2.5);
}
inline int dailyForm(int base,SimulationV2::Seed daySeed,const QString &id){
    return int(std::round(std::clamp(base+dailyBonus(daySeed,id),1.,100.)));
}
}
