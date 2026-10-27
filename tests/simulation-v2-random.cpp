#include "../source/simulation-v2/RandomSource.h"

#include <cmath>
#include <future>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace SimulationV2;

static void check(bool condition, const char *message) {
    if(!condition)throw std::runtime_error(message);
}

template<class Action> static void rejects(Action action) {
    try { action(); }
    catch(const std::invalid_argument &) { return; }
    throw std::runtime_error("Invalid random parameters were accepted");
}

class FixedRandom final : public RandomSource {
public:
    std::uint64_t value = 0;
    int calls = 0;
    std::uint64_t next() override { ++calls; return value; }
};

static std::vector<double> samples(Seed seed) {
    SeededRandom random(seed);
    std::vector<double> result;
    for(int i = 0; i < 200; ++i) {
        result.push_back(random.normal(10, 3));
        result.push_back(random.laplace(0, 2));
        result.push_back(random.uniform(-5, 5));
    }
    return result;
}

int main() {
    FixedRandom fixed;
    check(fixed.unit() > 0, "Lower random endpoint must be open");
    fixed.value = std::numeric_limits<std::uint64_t>::max();
    check(fixed.unit() < 1, "Upper random endpoint must be open");
    check(std::isfinite(fixed.normal(0, 1)) && std::isfinite(fixed.laplace(0, 1)), "Extreme source draws must produce finite samples");
    const int calls = fixed.calls;
    check(!fixed.event(0) && fixed.event(1), "Certain events must be exact");
    check(fixed.normal(8, 0) == 8 && fixed.laplace(9, 0) == 9 && fixed.uniform(4, 4) == 4, "Zero-scale distributions must preserve their location");
    check(fixed.calls == calls, "Degenerate distributions must not consume randomness");
    rejects([&]{fixed.event(-.1);});rejects([&]{fixed.event(1.1);});
    rejects([&]{fixed.normal(0, -1);});rejects([&]{fixed.laplace(0, -1);});
    rejects([&]{fixed.uniform(2, 1);});rejects([&]{fixed.uniform(0, std::numeric_limits<double>::infinity());});
    rejects([&]{fixed.normal(std::numeric_limits<double>::quiet_NaN(), 1);});
    rejects([&]{fixed.weightedIndex({});});rejects([&]{fixed.weightedIndex({0, 0});});
    rejects([&]{fixed.weightedIndex({1, -1});});rejects([&]{fixed.weightedIndex({1, std::numeric_limits<double>::infinity()});});
    fixed.value = 0;
    check(fixed.weightedIndex({0, 1e308, 1e308}) == 1, "Large weights must not overflow");
    fixed.value = std::numeric_limits<std::uint64_t>::max();
    check(fixed.weightedIndex({0, 1e308, 1e308, 0}) == 2, "Zero-weight outcomes must never be selected");

    const auto baseline = samples(deriveSeed(42, 7));
    check(baseline == samples(deriveSeed(42, 7)), "Seeds must reproduce samples");
    check(baseline != samples(deriveSeed(42, 8)), "Distinct streams must differ");
    std::vector<std::future<std::vector<double>>> workers;
    for(int i = 0; i < 8; ++i)workers.push_back(std::async(std::launch::async, [i]{return samples(deriveSeed(42, i));}));
    for(int i = 7; i >= 0; --i)check(workers[i].get() == samples(deriveSeed(42, i)), "Worker scheduling must not affect results");

    SeededRandom random(1234);
    constexpr int count = 200000;
    double normalSum = 0, normalSquares = 0, laplaceSum = 0, laplaceSquares = 0;
    int positive = 0;
    for(int i = 0; i < count; ++i) {
        const double normal = random.normal(0, 1), laplace = random.laplace(0, 2);
        normalSum += normal;normalSquares += normal * normal;
        laplaceSum += laplace;laplaceSquares += laplace * laplace;
        positive += laplace > 0;
    }
    check(std::abs(normalSum / count) < .02 && std::abs(normalSquares / count - 1) < .03, "Normal sample moments must match the requested scale");
    check(std::abs(laplaceSum / count) < .04 && std::abs(laplaceSquares / count - 8) < .3, "Laplace sample moments must match the requested scale");
    check(std::abs(double(positive) / count - .5) < .01, "Laplace samples must have the correct sign balance");
    std::cout << "PASS: simulation-v2 randomness, boundaries, validation, weighted outcomes, moments and parallel reproducibility\n";
}
