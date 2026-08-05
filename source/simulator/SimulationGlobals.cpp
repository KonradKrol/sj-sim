#include "SimulationGlobals.h"

SimulationGlobals::SimulationGlobals()
{
    object = new SimulationGlobals;
}

SimulationGlobals::~SimulationGlobals()
{
    delete object;
}
