#include "../source/simulation-v2/Engine.h"

#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace SimulationV2;
namespace {
void check(bool ok,const char *message) { if(!ok)throw std::runtime_error(message); }
void near(double a,double b,double tolerance,const char *message) {check(std::abs(a-b)<tolerance,message);}
template<class F> void rejects(F f) {bool rejected=false;try{f();}catch(const std::invalid_argument&){rejected=true;}check(rejected,"invalid engine input rejected");}
struct ConstantWeather:WeatherField {
    Wind value;double precip=0;
    PrecipitationKind kind=PrecipitationKind::Snow;
    Wind wind(double,double) const override {return value;}
    double precipitation(double) const override {return precip;}
    PrecipitationKind precipitationKind() const override {return kind;}
};
struct FixedMeasurement:WindMeasurement {
    double value=0;
    double measure(const WeatherField &,const FlightResult &,double) const override {return value;}
};
struct RescueWeather:WeatherField {
    double begins=70;
    mutable double furthestRead=0;
    Wind wind(double x,double) const override {furthestRead=std::max(furthestRead,x);return {x<begins?-2.5:4,0};}
    double precipitation(double) const override {return 0;}
};
}

int main()
{
    try {
        Engine engine;JumpRequest request;ConstantWeather calm;FixedMeasurement measurement;
        SeededRandom rng(42);
        auto result=engine.simulate(request,rng,calm,measurement);
        check(result.status==JumpStatus::Completed && result.measuredDistance>80 && result.measuredDistance<140,"ordinary jump completes");
        near(std::fmod(result.measuredDistance*2,1),0,1e-12,"half-metre measured distance");
        double traversed=0;for(const auto &s:result.flight.exposures) {check(s.traversed>=0 && s.traversed<=s.end-s.begin+1e-8,"bounded exposure section");traversed+=s.traversed;}
        near(traversed,result.flight.distance,1e-8,"exposure covers reached distance only");
        check(engine.flight().fly(50,request.hill,calm,0,true).exposures.back().traversed==0,"unreached section remains explicitly empty");
        const double preview=engine.previewDistance(request,calm);
        near(preview,engine.previewDistance(request,calm),1e-12,"preview deterministic and uses no RNG");
        ConstantWeather head,tail,cross;head.value={2,0};tail.value={-2,0};cross.value={0,2};
        check(engine.previewDistance(request,head)>preview && engine.previewDistance(request,tail)<preview,"wind signs affect flight");
        check(engine.previewDistance(request,cross)<=preview,"side wind does not act as headwind");
        calm.precip=10;const auto snowy=engine.previewDistance(request,calm);check(snowy<preview,"snow worsens jump");
        calm.kind=PrecipitationKind::Rain;const auto rainy=engine.previewDistance(request,calm);
        check(rainy<snowy && rainy<preview,"maximum rain can be substantially more damaging than snowfall");
        double previousRain=preview;for(int level=0;level<=100;++level){calm.precip=level/10.;const double current=engine.previewDistance(request,calm);check(current<=previousRain+1e-10 && previousRain-current<2,"rain effect grows continuously rather than switching to an arbitrary cutoff");previousRain=current;}
        calm.precip=2;near(engine.previewDistance(request,calm),preview,1e-12,"light rain is not automatically penalized like snow");
        const auto rainyJump=engine.simulate(request,rng,calm,measurement);check(rainyJump.flight.precipitationKind==PrecipitationKind::Rain,"result retains precipitation kind");
        calm.precip=0;calm.kind=PrecipitationKind::Snow;
        request.scoring.juryGate=9;check(engine.previewDistance(request,calm)<preview,"lower gate shortens jump");request.scoring.juryGate=10;

        FlightModel flight;Hill hill;RescueWeather rescue;ConstantWeather adverse;adverse.value={-2.5,0};
        const auto lost=flight.fly(95,hill,adverse,0,false);
        const auto saved=flight.fly(95,hill,rescue,0,false);
        check(saved.distance>lost.distance+1,"late favourable patch rescues part of weak jump");
        rescue.begins=100;rescue.furthestRead=0;auto notReached=flight.fly(95,hill,rescue,0,false);
        near(notReached.distance,lost.distance,1e-8,"unreached favourable patch cannot help");
        check(rescue.furthestRead<=notReached.distance,"physics queries only reached wind");
        ConstantWeather extreme;extreme.value={100,100};
        auto far=flight.fly(1000000,hill,extreme,0,false);
        check(std::isfinite(far.distance) && far.distance<=flight.landingLimit(hill) && far.integrationSteps<2000,"extreme reserve/wind terminate inside usable slope");
        auto zero=flight.fly(0,hill,extreme,0,false);near(zero.distance,0,1e-12,"zero reserve lands immediately");
        // Finer numerical steps converge without changing the engine model.
        FlightParameters fineParameters;fineParameters.stepMetres=.1;FlightModel fine(fineParameters);
        near(flight.fly(140,hill,head,0,false).distance,fine.fly(140,hill,head,0,false).distance,.12,"one-metre step converges to 10cm step");
        WindParameters dynamic;dynamic.gustProbability=1;dynamic.gustAmplitude=4;dynamic.gustPeriodSeconds=2;
        ContinuousWeather varying(77,dynamic);
        near(flight.fly(125,hill,varying,20,false).distance,fine.fly(125,hill,varying,20,false).distance,.3,"convergence with moving gusts");
        const double a=flight.fly(125,hill,calm,0,false).distance,b=flight.fly(145,hill,calm,0,false).distance,c=flight.fly(165,hill,calm,0,false).distance;
        check(c-b<b-a,"geometry makes additional long-jump metres progressively harder");

        // Response is coherent and reproducible, not resampled per integration step.
        const WindResponseSample good{1,1,1},bad{-1,0,1};
        ConstantWeather light,strong,back,side;
        light.value={.1,0};strong.value={6,0};back.value={-6,0};side.value={0,6};
        check(flight.fly(114,hill,strong,0,false,good).distance>flight.fly(114,hill,strong,0,false,bad).distance+10,"strong headwind has broad response range");
        check(flight.fly(114,hill,strong,0,false,bad).distance<flight.fly(114,hill,calm,0,false).distance,"strong headwind can hurt");
        near(flight.fly(114,hill,light,0,false,good).distance,flight.fly(114,hill,light,0,false,{-1,1,1}).distance,.02,"light wind almost independent of response");
        for(auto *w:{&back,&side})check(flight.fly(114,hill,*w,0,false,bad).distance<flight.fly(114,hill,*w,0,false,{-1,1,1}).distance-5,"adverse tail and side responses cause extra loss");
        near(flight.fly(114,hill,strong,0,false,bad).distance,fine.fly(114,hill,strong,0,false,bad).distance,.15,"stochastic response converges across integration steps");
        near(flight.fly(114,hill,calm,0,false,bad).distance,flight.fly(114,hill,calm,0,false,good).distance,1e-12,"response cannot alter calm flight");
        rejects([]{auto p=FlightParameters();p.windAdverseOnset=0;FlightModel invalid(p);});
        // Remove quality noise to isolate wind-response population statistics.
        EngineParameters isolated;isolated.quality.eventDeviation.fill(0);Engine windEngine(isolated);
        SeededRandom coupledA(123),coupledB(123);
        const auto noisyResponse=engine.simulate(request,coupledA,strong,measurement).flight.windResponse;
        const auto quietResponse=windEngine.simulate(request,coupledB,strong,measurement).flight.windResponse;
        near(noisyResponse.variation,quietResponse.variation,1e-12,"quality noise does not change wind response stream");
        near(noisyResponse.adverseQuantile,quietResponse.adverseQuantile,1e-12,"quality noise does not change adverse event quantile");
        double weakSum=0,weakSquares=0,strongSum=0,strongSquares=0;int hurt=0;
        for(int i=0;i<3000;++i){SeededRandom a(i),b(i);const double x=windEngine.simulate(request,a,light,measurement).flight.distance,y=windEngine.simulate(request,b,strong,measurement).flight.distance;weakSum+=x;weakSquares+=x*x;strongSum+=y;strongSquares+=y*y;hurt+=y<windEngine.previewDistance(request,calm);}
        check(strongSquares/3000-std::pow(strongSum/3000,2)>weakSquares/3000-std::pow(weakSum/3000,2)+1,"strong wind increases population spread");
        check(hurt>50 && hurt<2000,"strong headwind has both beneficial and harmful outcomes");
        LandingModel landing;double previousFall=0;
        for(double distance=20;distance<170;distance+=.1) {
            const auto p=landing.probabilities(distance,134,0);double sum=0;
            for(double probability:p.values){check(probability>=0 && probability<=1,"valid landing probabilities");sum+=probability;}
            near(sum,1,1e-12,"landing probabilities sum to one");
            check(p.values[3]>=previousFall,"smooth rising fall risk");previousFall=p.values[3];
        }
        SeededRandom styleRng(8);int highMarks=0;
        for(int i=0;i<10000;++i)for(double v:landing.judges(4,LandingKind::Telemark,120,134,styleRng))highMarks+=v>=19.5;
        check(highMarks>20000,"stylists can regularly receive 19.5 or 20 for a good landing");
        request.athlete.landing=0;SeededRandom uglySeed(91);auto ugly=engine.simulate(request,uglySeed,varying,measurement);
        request.athlete.landing=4;SeededRandom elegantSeed(91);auto elegant=engine.simulate(request,elegantSeed,varying,measurement);
        near(ugly.flight.distance,elegant.flight.distance,1e-12,"landing class changes no distance");
        check(ugly.landing==elegant.landing && elegant.score.judges>ugly.score.judges,"landing class affects marks only, not fall probability");
        request.athlete.landing=2;

        auto scoring=ScoringRules();std::array<double,5> marks{{18,19,19.5,17.5,18.5}};
        hill.headwindPoints=9.26;auto scored=scoreJump(120,marks,.39,hill,scoring);
        near(scored.wind,-3.6,1e-12,"official .39m/s times 9.26 example");near(scored.judges,55.5,1e-12,"one highest and one lowest mark discarded");
        scoring.juryGate=9;scoring.coachReduction=1;
        near(scoreJump(120,marks,0,hill,scoring).gate,5,1e-12,"jury reduction compensated below coach threshold");
        near(scoreJump(127.5,marks,0,hill,scoring).gate,10,1e-12,"coach credit added at 95% HS");
        near(scoreJump(127,marks,0,hill,scoring).gate,5,1e-12,"coach credit withheld below 95% HS");
        scoring.windCompensation=scoring.gateCompensation=scoring.judges=false;
        scored=scoreJump(120,marks,.39,hill,scoring);near(scored.total,60,1e-12,"disabled scoring components stay disabled");
        WindParameters refs;refs.references={{0,{1,0}},{100,{-1,0}}};refs.temporalAmplitude=refs.spatialAmplitude=refs.gustAmplitude=0;
        ContinuousWeather sensorField(1,refs);SensorWindMeasurement sensors({{0,3},{100,1}});
        FlightResult measuredFlight;measuredFlight.distance=80;measuredFlight.duration=3;
        near(sensors.measure(sensorField,measuredFlight,10),.5,1e-12,"measurement uses explicit sensor weights, not experienced mean");
        SensorWindMeasurement reachedSensors({{0,3},{100,1}},9,0,0,true);
        near(reachedSensors.measure(sensorField,measuredFlight,10),1,1e-12,"explicit reached-sensor policy");

        request.skip=true;SeededRandom noJump(4),untouched(4);
        check(engine.simulate(request,noJump,calm,measurement).status==JumpStatus::DidNotStart && noJump.next()==untouched.next(),"DNS consumes no jump randomness");
        request.skip=false;request.dsqProbability=1;
        auto dsq=engine.simulate(request,rng,calm,measurement);check(dsq.status==JumpStatus::Disqualified && dsq.score.total==0 && dsq.flight.exposures.empty(),"DSQ skips flight/scoring");request.dsqProbability=0;
        request.manipulation.distance.range=ValueRange{130,130};request.manipulation.landing=LandingKind::Telemark;
        request.manipulation.judges[0]=0;request.manipulation.measuredWind.bonus=.39;
        auto forced=engine.simulate(request,rng,calm,measurement);
        check(forced.manipulated && forced.landing==LandingKind::Telemark && forced.judges[0]==0,"explicit overrides applied, including zero mark");
        near(forced.flight.distance,130,.0001,"distance override realised through consistent flight");
        double forcedCovered=0;for(const auto &s:forced.flight.exposures)forcedCovered+=s.traversed;
        near(forcedCovered,forced.flight.distance,1e-8,"forced distance retains true exposure");
        near(engine.previewDistance(request,calm),130,.0001,"preview applies distance manipulation");
        request.manipulation.distance.range=ValueRange{500,500};
        forced=engine.simulate(request,rng,calm,measurement);
        check(forced.distanceLimitedByGeometry && forced.flight.distance<=engine.flight().landingLimit(request.hill),"manipulation cannot invent impossible landing beyond slope");
        request.manipulation={};
        rejects([&]{auto bad=request;bad.manipulation.distance.range=ValueRange{130,100};engine.simulate(bad,rng,calm,measurement);});

        // Calibration includes normal jump variability, geometry and actual rounding.
        constexpr int count=20000;double gap=0;
        const auto begin=std::chrono::steady_clock::now();request.captureExposure=false;
        for(int i=0;i<count;++i) {
            SeededRandom strongSeed(deriveSeed(17,Seed(i))),weakSeed(deriveSeed(17,Seed(i)));
            request.athlete.overall=70;const auto weak=engine.simulate(request,weakSeed,calm,measurement);
            request.athlete.overall=90;const auto strong=engine.simulate(request,strongSeed,calm,measurement);
            gap+=strong.measuredDistance-weak.measuredDistance;
        }
        gap/=count;check(gap>=14 && gap<=17,"OVR90 vs70 calibration falls within 14..17m at K120");
        const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();
        // Full engine can be shared; each job owns its RNG. Work scheduling is irrelevant.
        auto batch=[&](int worker){double sum=0;for(int i=0;i<1000;++i){SeededRandom random(deriveSeed(Seed(worker),Seed(i)));sum+=engine.simulate(request,random,varying,measurement).score.total;}return sum;};
        std::vector<std::future<double>> tasks;for(int i=0;i<4;++i)tasks.push_back(std::async(std::launch::async,batch,i));
        for(int i=0;i<4;++i)near(tasks[std::size_t(i)].get(),batch(i),1e-12,"full simulation reproducible across worker scheduling");
        SeededRandom edge(611);
        for(int i=0;i<4000;++i) {
            JumpRequest varied;varied.hill.k=edge.uniform(1,400);varied.hill.hs=edge.uniform(varied.hill.k,500);
            varied.hill.realHs=edge.uniform(.5*varied.hill.k,600);varied.hill.takeoffBias=edge.uniform(-1,1);
            varied.athlete.overall=1+int(edge.uniform(0,100));varied.athlete.form=1+int(edge.uniform(0,100));
            varied.athlete.landing=int(edge.uniform(0,5));
            for(int &perk:varied.athlete.perks)perk=int(edge.uniform(0,7))-3;
            varied.event=static_cast<EventKind>(int(edge.uniform(0,4)));varied.startSeconds=edge.uniform(0,100000);
            varied.scoring.juryGate=int(edge.uniform(0,201));varied.scoring.coachReduction=int(edge.uniform(0,varied.scoring.juryGate+1));
            ConstantWeather conditions;conditions.value={edge.uniform(-100,100),edge.uniform(-100,100)};conditions.precip=edge.uniform(0,10);
            auto jump=engine.simulate(varied,edge,conditions,measurement);
            check(std::isfinite(jump.score.total) && jump.flight.distance<=engine.flight().landingLimit(varied.hill),"random edge cases remain finite and bounded");
            double covered=0;for(const auto &s:jump.flight.exposures) {check(s.traversed<=s.end-s.begin+1e-7,"extreme hill exposure stays inside section");covered+=s.traversed;}
            near(covered,jump.flight.distance,1e-7,"extreme hill exposure covers full jump");
        }
        rejects([&]{auto bad=request;bad.scoring.coachReduction=100;engine.simulate(bad,rng,calm,measurement);});
        rejects([&]{auto bad=request;bad.athlete.overall=0;engine.previewDistance(bad,calm);});
        rejects([&]{auto bad=request;bad.startSeconds=std::numeric_limits<double>::quiet_NaN();engine.previewDistance(bad,calm);});
        rejects([]{FlightParameters p;p.maximumWindAssistance=1;FlightModel invalid(p);});
        std::cout<<"PASS simulation-v2 engine: flight, geometry, exposure, marks, scoring, DNS/DSQ, parallel determinism; OVR gap="<<gap<<"m; "<<2*count<<" calm jumps in "<<elapsed<<"s\n";
    } catch(const std::exception &e) {std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}
}
