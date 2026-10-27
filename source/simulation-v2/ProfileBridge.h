#pragma once

#include "Engine.h"
#include "../athletes/AthleteProfile.h"
#include "../hills/HillProfile.h"

namespace SimulationV2 {

Athlete athleteInput(const AthleteProfile &);
Hill hillInput(const HillProfile &);
WeatherProposalContext weatherProposalContext(const HillProfile &, Surface, int month=0);

}
