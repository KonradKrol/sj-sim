#include "Manipulation.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {

double ValueAdjustment::apply(double value) const
{
    value+=bonus;return range ? std::clamp(value,range->minimum,range->maximum) : value;
}

void ValueAdjustment::validate(double bound) const
{
    if(!std::isfinite(bonus) || std::abs(bonus)>bound
        || (range && (!std::isfinite(range->minimum) || !std::isfinite(range->maximum)
            || std::abs(range->minimum)>bound || std::abs(range->maximum)>bound || range->minimum>range->maximum)))
        throw std::invalid_argument("Invalid manipulation adjustment/range");
}

void JumpManipulation::validate() const
{
    quality.validate(1000);distance.validate(1000);precipitation.validate(10);measuredWind.validate(100);
    if(precipitation.range && (precipitation.range->minimum<0 || precipitation.range->maximum>10))
        throw std::invalid_argument("Manipulated precipitation must stay in [0,10]");
    if(distance.range && distance.range->minimum<0)throw std::invalid_argument("Manipulated distance cannot be negative");
    if(landing && (int(*landing)<0 || int(*landing)>3))throw std::invalid_argument("Invalid forced landing");
    if(dsqProbability && (!std::isfinite(*dsqProbability) || *dsqProbability<0 || *dsqProbability>1))
        throw std::invalid_argument("Invalid forced DSQ probability");
    for(const auto &mark:judges)if(mark && (!std::isfinite(*mark) || *mark<0 || *mark>20 || *mark*2!=std::round(*mark*2)))
        throw std::invalid_argument("Forced judge marks must be 0..20 in half-point steps");
}

bool JumpManipulation::active() const
{
    if(quality.active() || distance.active() || precipitation.active() || measuredWind.active() || landing || dsqProbability)return true;
    for(const auto &mark:judges)if(mark)return true;
    return false;
}

}
