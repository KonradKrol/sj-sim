#include "../source/simulation-v2/QualityModel.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace SimulationV2;

namespace {
void check(bool ok,const char *message) { if (!ok) throw std::runtime_error(message); }
void near(double actual,double expected,double tolerance,const char *message) { check(std::abs(actual-expected)<tolerance,message); }
template<class F> void rejects(F f) { bool rejected=false;try { f(); } catch (const std::invalid_argument &) { rejected=true; } check(rejected,"invalid input rejected"); }
struct Moments {
    double sum=0,squares=0,negative=0,positive=0;
    int negativeCount=0;
    void add(double v) { sum+=v;squares+=v*v;if(v<0){negative-=v;++negativeCount;}else positive+=v; }
};
}

int main()
{
    try {
        HillSkillCurve curve;
        for (const auto &p:curve.anchors()) near(curve.takeoffWeight(p.k),p.takeoff,1e-12,"exact user anchor");
        double previous=1;
        for (int i=1;i<=40000;++i) {
            const double current=curve.takeoffWeight(i/100.0);
            check(current<=previous+1e-12 && current>=0 && current<=1,"monotonic curve, no overshoot");previous=current;
        }
        for (const auto &p:curve.anchors()) {
            near(curve.takeoffWeight(p.k-.00001),p.takeoff,.000001,"left continuity");
            near(curve.takeoffWeight(p.k+.00001),p.takeoff,.000001,"right continuity");
        }
        near(curve.takeoffWeight(1),.85,1e-12,"explicit constant extrapolation below K50");
        near(curve.takeoffWeight(400),.08,1e-12,"explicit constant extrapolation above K250");
        HillSkillCurve flat({{5,.6},{15,.6},{20,.2},{40,.2}},CurveExtension::LinearBounded);
        near(flat.takeoffWeight(10),.6,1e-12,"flat interval stays flat");
        near(flat.takeoffWeight(30),.2,1e-12,"second flat interval stays flat");
        HillSkillCurve linear({{50,.85},{250,.08}},CurveExtension::LinearBounded);
        near(linear.takeoffWeight(400),0,1e-12,"linear extrapolation is bounded");
        rejects([]{HillSkillCurve({{50,.5},{40,.4}});});
        rejects([]{HillSkillCurve({{50,.5},{80,.6}});});
        rejects([]{HillSkillCurve({{50,.5},{50.000000001,.4}});});
        rejects([&]{curve.takeoffWeight(std::numeric_limits<double>::quiet_NaN());});

        QualityModel model;
        Athlete athlete;athlete.overall=90;athlete.form=30;
        auto q=model.baseline(athlete,120,0,EventKind::Competition);
        near(q.overallAndForm,72,1e-12,"OVR and independent form use 70/30");
        near(model.baseline(athlete,120,.55,EventKind::Competition).value(),q.value(),1e-12,"neutral athlete unaffected by hill preference");
        athlete.perks[1]=3;
        check(model.baseline(athlete,200,0,EventKind::Competition).hillEffect>0,"flier benefits on big hills");
        check(model.baseline(athlete,80,0,EventKind::Competition).hillEffect<0,"flier loses on small hills");
        near(model.baseline(athlete,110,0,EventKind::Competition).hillEffect,0,1e-12,"K110 is balanced");
        check(model.baseline(athlete,120,.55,EventKind::Competition).hillEffect<model.baseline(athlete,120,-.55,EventKind::Competition).hillEffect,"hill takeoff preference interacts with flier");
        athlete.perks={{3,0,0}};
        near(model.baseline(athlete,120,0,EventKind::Qualification).eventEffect,2.5,1e-12,"qualification gets half training bonus");
        check(model.baseline(athlete,120,0,EventKind::Competition).eventEffect<0,"training perk weakens competition");
        athlete.perks[0]=-3;
        near(model.baseline(athlete,120,0,EventKind::Competition).eventEffect,0,1e-12,"conserving effort gives no competition bonus");
        check(model.baseline(athlete,120,0,EventKind::Trial).eventEffect<0,"conserving effort weakens trial");
        athlete.perks={{0,0,0}};
        constexpr int n=200000;
        Moments base,regular,unpredictable,training;
        SeededRandom a(42),b(42),c(42),d(42);
        for (int i=0;i<n;++i) {
            athlete.perks[2]=0;base.add(model.sample(athlete,120,0,EventKind::Competition,a).randomEffect);
            training.add(model.sample(athlete,120,0,EventKind::Training,d).randomEffect);
            athlete.perks[2]=3;regular.add(model.sample(athlete,120,0,EventKind::Competition,b).randomEffect);
            athlete.perks[2]=-3;unpredictable.add(model.sample(athlete,120,0,EventKind::Competition,c).randomEffect);
        }
        near(base.negative/(base.positive+base.negative),.65,.004,"65% is aggregate influence");
        near(double(base.negativeCount)/n,.5,.004,"65% is not negative event probability");
        check(regular.sum>base.sum && unpredictable.sum<base.sum,"regularity affects mean in expected directions");
        check(regular.squares<base.squares && unpredictable.squares>base.squares,"regularity changes spread");
        check(training.squares>base.squares*3.8,"training is more variable");
        athlete.perks={{0,0,0}};
        auto before=model.baseline(athlete,120,0,EventKind::Competition);
        athlete.landing=4;auto after=model.baseline(athlete,120,0,EventKind::Competition);
        near(before.value(),after.value(),1e-12,"landing skill cannot change jump quality");
        QualityParameters params;params.overallWeight=0;
        params.specialistBias[0]=20;
        near(QualityModel(params).baseline(athlete,120,0,EventKind::Competition).value(),athlete.form,1e-12,"OVR/form weights editable");
        params.noise=NoiseShape::Laplace;QualityModel laplace(params);SeededRandom laplaceRng(5);
        double negative=0,positive=0;
        for(int i=0;i<n;++i){const auto v=laplace.sample(athlete,120,0,EventKind::Competition,laplaceRng).randomEffect;if(v<0)negative-=v;else positive+=v;}
        near(negative/(negative+positive),.65,.005,"Laplace preserves asymmetry semantics");
        rejects([&]{auto p=params;p.overallWeight=1.1;QualityModel invalid(p);});
        rejects([&]{auto invalid=athlete;invalid.form=0;model.baseline(invalid,120,0,EventKind::Competition);});
        std::cout<<"PASS simulation-v2 quality: curve, perks, 65/35, 200000-sample comparisons\n";
    } catch(const std::exception &e) { std::cerr<<"FAIL: "<<e.what()<<'\n';return 1; }
}
