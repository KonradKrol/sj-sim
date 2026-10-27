#include "HillSkillCurve.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace SimulationV2 {

std::vector<SkillAnchor> HillSkillCurve::defaultAnchors()
{
    return {{50,.85}, {80,.72}, {95,.62}, {110,.50}, {125,.40},
            {140,.30}, {200,.15}, {250,.08}};
}

HillSkillCurve::HillSkillCurve(std::vector<SkillAnchor> anchors, CurveExtension mode)
    : knots(std::move(anchors)), extension(mode)
{
    if (knots.size() < 2 || knots.size()>1000 || (mode != CurveExtension::Constant && mode != CurveExtension::LinearBounded))
        throw std::invalid_argument("Invalid hill skill curve");
    std::vector<double> h(knots.size()-1), secants(knots.size()-1);
    for (std::size_t i=0; i<knots.size(); ++i) {
        const auto &p = knots[i];
        if (!std::isfinite(p.k) || p.k <= 0 || p.k>10000 || !std::isfinite(p.takeoff) || p.takeoff < 0 || p.takeoff > 1)
            throw std::invalid_argument("Hill skill anchors must have positive K and weights in [0,1]");
        if (!i) continue;
        const auto &before = knots[i-1];
        if (p.k <= before.k || p.takeoff > before.takeoff)
            throw std::invalid_argument("Hill skill anchors must increase in K and not increase in takeoff weight");
        h[i-1] = p.k-before.k;
        secants[i-1] = (p.takeoff-before.takeoff)/h[i-1];
    }
    slopes.resize(knots.size());
    // Weighted harmonic interior derivatives (PCHIP). Flat intervals stay flat.
    for (std::size_t i=1; i+1<knots.size(); ++i) {
        const double a=secants[i-1], b=secants[i];
        if (a == 0 || b == 0) slopes[i]=0;
        else {
            const double w1=2*h[i]+h[i-1], w2=h[i]+2*h[i-1];
            slopes[i]=(w1+w2)/(w1/a+w2/b);
        }
    }
    const auto endpoint=[](double a, double b, double ha, double hb) {
        const double derivative=((2*ha+hb)*a-ha*b)/(ha+hb);
        if (derivative*a <= 0) return 0.0;
        return std::max(derivative, 3*a); // all slopes are nonpositive
    };
    if (knots.size()==2) slopes.front()=slopes.back()=secants.front();
    else {
        slopes.front()=endpoint(secants[0],secants[1],h[0],h[1]);
        const auto n=secants.size();
        slopes.back()=endpoint(secants[n-1],secants[n-2],h[n-1],h[n-2]);
    }
}

double HillSkillCurve::takeoffWeight(double k) const
{
    if (!std::isfinite(k) || k <= 0) throw std::invalid_argument("K must be positive and finite");
    if (k <= knots.front().k)
        return extension == CurveExtension::Constant ? knots.front().takeoff
            : std::clamp(knots.front().takeoff+(k-knots.front().k)*slopes.front(),0.0,1.0);
    if (k >= knots.back().k)
        return extension == CurveExtension::Constant ? knots.back().takeoff
            : std::clamp(knots.back().takeoff+(k-knots.back().k)*slopes.back(),0.0,1.0);
    const auto upper=std::upper_bound(knots.begin(),knots.end(),k,
                                    [](double x,const SkillAnchor &p){return x<p.k;});
    const auto i=std::size_t(upper-knots.begin()-1);
    const double h=knots[i+1].k-knots[i].k, t=(k-knots[i].k)/h;
    const double t2=t*t, t3=t2*t;
    return (2*t3-3*t2+1)*knots[i].takeoff+(t3-2*t2+t)*h*slopes[i]
        +(-2*t3+3*t2)*knots[i+1].takeoff+(t3-t2)*h*slopes[i+1];
}

}
