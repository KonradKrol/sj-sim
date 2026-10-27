#include "ProfileBridge.h"

#include <stdexcept>

namespace SimulationV2 {

Athlete athleteInput(const AthleteProfile &profile)
{
    Athlete value;value.overall=profile.overall;value.form=profile.form;value.landing=profile.landing;value.perks=profile.perks;
    value.validate();return value;
}

Hill hillInput(const HillProfile &profile)
{
    Hill value;value.k=profile.k;value.hs=profile.hs;value.realHs=profile.realHs();value.takeoffBias=profile.takeoffBias();
    value.pointsAtK=profile.pointsAtK;value.pointsPerMetre=profile.pointsPerMetre;value.gatePoints=profile.gatePoints;
    value.headwindPoints=profile.headwindPoints;value.tailwindPoints=profile.tailwindPoints;value.validate();return value;
}

WeatherProposalContext weatherProposalContext(const HillProfile &profile,Surface surface,int month)
{
    return {profile.k,profile.perks[2],profile.perks[3],surface,month,profile.country.toStdString()};
}

}
