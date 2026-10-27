#pragma once

#include <vector>

namespace SimulationV2 {

struct SkillAnchor {
    double k;
    double takeoff;
};

enum class CurveExtension { Constant, LinearBounded };

// Immutable, monotone cubic Hermite interpolation. No hidden athlete skill.
class HillSkillCurve {
public:
    explicit HillSkillCurve(std::vector<SkillAnchor> anchors = defaultAnchors(),
                            CurveExtension extension = CurveExtension::Constant);
    double takeoffWeight(double k) const;
    const std::vector<SkillAnchor> &anchors() const { return knots; }
    static std::vector<SkillAnchor> defaultAnchors();
private:
    std::vector<SkillAnchor> knots;
    std::vector<double> slopes;
    CurveExtension extension;
};

}
