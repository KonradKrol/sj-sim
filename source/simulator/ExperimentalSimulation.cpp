#include "ExperimentalSimulation.h"
namespace ExperimentalSimulation {
namespace {thread_local Parameters parameters;}
Parameters current(){return parameters;}
Scoped::Scoped(double takeoff,double flight,double wind):previous(parameters){parameters={true,takeoff,flight,wind};}
Scoped::~Scoped(){parameters=previous;}
}
