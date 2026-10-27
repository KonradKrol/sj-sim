#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace SimulationV2 {

using Seed = std::uint64_t;

Seed deriveSeed(Seed parent, std::uint64_t stream);

class RandomSource {
public:
    virtual ~RandomSource() = default;
    virtual std::uint64_t next() = 0;

    double unit();
    double uniform(double lower, double upper);
    double normal(double mean, double deviation);
    double laplace(double location, double scale);
    bool event(double probability);
    std::size_t weightedIndex(const std::vector<double> &weights);
};

class SeededRandom final : public RandomSource {
public:
    explicit SeededRandom(Seed seed) : state(seed) {}
    std::uint64_t next() override;
private:
    Seed state;
};

}
