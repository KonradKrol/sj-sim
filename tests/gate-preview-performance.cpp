#include <QApplication>
#include <QElapsedTimer>
#include <QMap>
#include <iostream>
#include <cmath>
#include "../source/seasons/SimulationSave.h"
#include "../source/simulator/JumpSimulator.h"
#include "../source/balance/BalanceRuntime.h"
#include "../source/simulation-v2/FieldWindMeasurement.h"
class Breeze final:public SimulationV2::WeatherField {double head;public:explicit Breeze(double h):head(h){}SimulationV2::Wind wind(double x,double t)const override{return {head+.4*std::sin(x/19+t/7),.8*std::cos(x/23)};}double precipitation(double)const override{return 0;}};
int main(int argc,char**argv){QApplication app(argc,argv);std::unique_ptr<SimulationSave> save(SimulationSave::loadFromFile(argv[1]));if(!save)return 2;auto *hill=save->getHillsReference().first();CompetitionRules rules;rules.setRounds({RoundInfo(83,true)});JumpSimulator simulator;simulator.setHill(hill);simulator.setCompetitionRules(&rules);simulator.setSimulationBalanceProfile(BalanceRuntime::defaults());simulator.setGate(10);simulator.setCompetitionStartGate(10);simulator.setDayFormSeed(123);simulator.setWinds({});
 for(double head:{-2.,0.,2.})for(int gate:{1,10,30,80}){simulator.setGate(gate);simulator.setSimulationWeather(std::make_shared<Breeze>(head),std::make_shared<SimulationV2::FieldWindMeasurement>(hill->getKPoint(),rules));double full=-1,fast=-1;QElapsedTimer t;t.start();for(auto *jumper:save->getJumpersReference()){simulator.setJumper(jumper);full=std::max(full,simulator.previewDistance());}auto before=t.nsecsElapsed();t.restart();QMap<int,QPair<Jumper*,double>> groups;for(auto *jumper:save->getJumpersReference()){simulator.setJumper(jumper);const double q=simulator.previewQuality();const int perk=jumper->getSimulationProfile()->perks[1];if(!groups.contains(perk)||q>groups[perk].second)groups[perk]={jumper,q};}for(const auto &candidate:groups){simulator.setJumper(candidate.first);fast=std::max(fast,simulator.previewDistance());}auto after=t.nsecsElapsed();if(std::abs(full-fast)>.000001){std::cerr<<"gate preview mismatch "<<full<<" "<<fast;return 3;}std::cout<<"gate="<<gate<<" full_ms="<<before/1e6<<" grouped_ms="<<after/1e6<<" candidates="<<groups.size()<<std::endl;}}
