#include "RandomSource.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace SimulationV2 {
namespace {
constexpr Seed increment = 0x9e3779b97f4a7c15ULL;

Seed mix(Seed value) {
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

void validateScale(double location, double scale) {
    if(!std::isfinite(location) || !std::isfinite(scale) || scale < 0)
        throw std::invalid_argument("Random distribution needs a finite location and nonnegative scale");
}

double finiteSample(double value) {
    if(!std::isfinite(value))throw std::overflow_error("Random sample exceeded the finite range");
    return value;
}
}

Seed deriveSeed(Seed parent, std::uint64_t stream) {
    return mix(parent ^ mix(stream + increment));
}

std::uint64_t SeededRandom::next() {
    state += increment;
    return mix(state);
}

double RandomSource::unit() {
    // Midpoints on a 52-bit grid: neither endpoint rounds to zero or one.
    return (static_cast<double>(next() >> 12) + .5) * 0x1p-52;
}

double RandomSource::uniform(double lower, double upper) {
    if(!std::isfinite(lower) || !std::isfinite(upper) || lower > upper)
        throw std::invalid_argument("Uniform bounds must be finite and ordered");
    if(lower == upper)return lower;
    const double draw = unit();
    return std::clamp(finiteSample(lower * (1 - draw) + upper * draw), lower, upper);
}

double RandomSource::normal(double mean, double deviation) {
    validateScale(mean, deviation);
    if(deviation == 0)return mean;
    constexpr double tau = 6.283185307179586476925286766559;
    const double radius = std::sqrt(-2 * std::log(unit()));
    const double angle = tau * unit();
    return finiteSample(mean + deviation * radius * std::cos(angle));
}

double RandomSource::laplace(double location, double scale) {
    validateScale(location, scale);
    if(scale == 0)return location;
    const double draw = unit() - .5;
    return finiteSample(location - scale * std::copysign(1., draw) * std::log1p(-2 * std::abs(draw)));
}

bool RandomSource::event(double probability) {
    if(!std::isfinite(probability) || probability < 0 || probability > 1)
        throw std::invalid_argument("Probability must be between zero and one");
    if(probability == 0)return false;
    if(probability == 1)return true;
    return unit() < probability;
}

std::size_t RandomSource::weightedIndex(const std::vector<double> &weights) {
    double largest = 0;
    for(double weight : weights) {
        if(!std::isfinite(weight) || weight < 0)
            throw std::invalid_argument("Weights must be finite and nonnegative");
        largest = std::max(largest, weight);
    }
    if(largest == 0)throw std::invalid_argument("At least one weight must be positive");
    double total = 0;
    for(double weight : weights)total += weight / largest;
    const double target = unit() * total;
    double cumulative = 0;
    std::size_t lastPositive = 0;
    for(std::size_t i = 0; i < weights.size(); ++i) {
        if(weights[i] == 0)continue;
        lastPositive = i;
        cumulative += weights[i] / largest;
        if(target < cumulative)return i;
    }
    return lastPositive;
}

}
