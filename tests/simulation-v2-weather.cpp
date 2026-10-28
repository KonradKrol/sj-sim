#include "../source/simulation-v2/Weather.h"

#include <cmath>
#include <future>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace SimulationV2;
namespace {
void check(bool ok,const char *message) { if(!ok) throw std::runtime_error(message); }
void near(double a,double b,double tolerance,const char *message) { check(std::abs(a-b)<tolerance,message); }
template<class F> void rejects(F f) { bool rejected=false;try{f();}catch(const std::invalid_argument&){rejected=true;}check(rejected,"invalid weather rejected"); }
WindParameters constant() { WindParameters p;p.temporalAmplitude=p.spatialAmplitude=p.gustAmplitude=0;return p; }
}

int main()
{
    try {
        auto p=constant();p.references={{20,{2,1}},{100,{-2,3}}};
        ContinuousWeather calm(42,p);
        near(calm.wind(0,0).head,2,1e-12,"first value extends to start");
        near(calm.wind(20,5).head,2,1e-12,"first reference preserved");
        near(calm.wind(60,100).head,0,1e-12,"smooth direction transition through crosswind");
        near(calm.wind(60,100).cross,2,1e-12,"cross component interpolated continuously");
        near(calm.wind(300,99).head,-2,1e-12,"last reference extends beyond HS");
        for(double x=0;x<=400;x+=.1)check(std::abs(calm.wind(x,0).head)<=2,"interpolation creates no overshoot");
        WindParameters moving;moving.gustProbability=1;moving.gustAmplitude=5;
        moving.spatialAmplitude=2;moving.temporalAmplitude=3;moving.gustPeriodSeconds=4;
        ContinuousWeather weather(57,moving),same(57,moving),other(58,moving);
        double change=0,betweenSeeds=0,neighbourDelta=0,distantDelta=0;
        std::vector<double> expected;
        for(int i=0;i<10000;++i) {
            const double t=i*.05, x=double(i%250);
            const auto w=weather.wind(x,t);
            expected.push_back(w.head);
            check(std::isfinite(w.head) && std::isfinite(w.cross) && w.strength()<=moving.maximumStrength+1e-12,"finite bounded weather");
            near(w.head,same.wind(x,t).head,1e-12,"same seed repeats field");
            const auto adjacent=weather.wind(x+.00001,t+.00001);
            near(w.head,adjacent.head,.001,"continuous through grid/reference/gust boundaries");
            change+=std::abs(w.head-weather.wind(x,t+30).head);
            betweenSeeds+=std::abs(w.head-other.wind(x,t).head);
            neighbourDelta+=std::abs(w.head-weather.wind(x+.5,t).head);
            distantDelta+=std::abs(w.head-weather.wind(x+80,t).head);
        }
        check(change>1000 && betweenSeeds>1000,"weather evolves and seed matters");
        check(neighbourDelta<distantDelta*.1,"nearby weather correlated more than distant weather");
        // Reverse and concurrent reads cannot change subsequent values.
        std::vector<std::future<bool>> tasks;
        for(int worker=0;worker<8;++worker)tasks.push_back(std::async(std::launch::async,[&,worker]{
            for(int i=9999-worker;i>=0;i-=8)if(weather.wind(double(i%250),i*.05).head!=expected[std::size_t(i)])return false;
            return true;
        }));
        for(auto &task:tasks)check(task.get(),"thread-safe/order-independent field queries");
        auto extreme=constant();extreme.references={{0,{100,-100}}};extreme.maximumStrength=12;
        near(ContinuousWeather(1,extreme).wind(100,1e12).strength(),12,1e-12,"extreme wind bounded, time range finite");
        PrecipitationParameters rain;rain.scenario=PrecipitationScenario::Stable;rain.intensity=4;
        near(ContinuousWeather(1,p,rain).precipitation(0),4,1e-12,"stable precipitation");
        rain.scenario=PrecipitationScenario::Increasing;rain.variation=0;
        auto trend=ContinuousWeather(1,p,rain);check(trend.precipitation(600)>trend.precipitation(0),"increasing precipitation");
        near(trend.precipitation(1e12),10,1e-12,"precipitation capped at 10");
        for(int scenario=0;scenario<=4;++scenario) {
            rain.scenario=static_cast<PrecipitationScenario>(scenario);rain.variation=10;
            auto field=ContinuousWeather(1,p,rain);
            for(int i=0;i<1000;++i){const auto v=field.precipitation(i*3);check(v>=0 && v<=10,"all precipitation scenarios bounded");}
        }
        // A prevailing tendency can last several jumps, then change sign.
        auto regimes=constant();regimes.references={{0,{1.8,0}},{150,{-.4,.6}}};
        regimes.trendAmplitude=6;regimes.trendPeriodSeconds=150;regimes.trendTransitionSeconds=10;
        ContinuousWeather evolving(42,regimes);
        int plateaus=0;bool front=false,back=false;
        for(int time=0;time<7200;++time) {
            const auto a=evolving.wind(0,time),b=evolving.wind(0,time+.00001);
            check(std::abs(a.head-b.head)<.0001 && std::abs(a.cross-b.cross)<.0001,"weather regime transitions stay continuous");
            if(std::abs(a.head-evolving.wind(0,time+1).head)<1e-12)++plateaus;
            front|=a.head>2;back|=a.head< -2;
            near(a.head-evolving.wind(150,time).head,2.2,1e-10,"prevailing change is shared across positions without erasing local differences");
        }
        check(plateaus>5000 && front && back,"tendencies persist across jumps and can reverse the prevailing direction");
        auto intermittent=constant();intermittent.temporalAmplitude=1;intermittent.correlationSeconds=600;
        auto quiet=ContinuousWeather(123,intermittent);intermittent.activityVariation=1;
        auto active=ContinuousWeather(123,intermittent);
        double slowChanges=0,intermittentChanges=0;
        for(int time=0;time<12000;time+=30) {
            slowChanges+=std::pow(quiet.wind(0,time+30).head-quiet.wind(0,time).head,2);
            intermittentChanges+=std::pow(active.wind(0,time+30).head-active.wind(0,time).head,2);
        }
        check(intermittentChanges>slowChanges*2,"activity episodes permit changes between consecutive jumps rather than only slow drift");
        const auto remembered=evolving.wind(70,1234);
        for(int i=0;i<100;++i)evolving.wind(i*3,7000-i*21);
        near(evolving.wind(70,1234).head,remembered.head,1e-14,"unrelated past/future queries never advance the weather clock");
        rejects([]{WindParameters v;v.activityVariation=1.1;v.validate();});
        rejects([]{WindParameters v;v.trendAmplitude=-1;v.validate();});
        rejects([]{WindParameters v;v.trendTransitionSeconds=100;v.trendPeriodSeconds=100;v.validate();});
        // Same random stream isolates the effect of proposal preferences.
        SeededRandom headRng(12),tailRng(12),stableRng(14),lotteryRng(14);
        int heads=0,tails=0,stable=0,lottery=0;
        constexpr int proposals=10000;
        for(int i=0;i<proposals;++i) {
            WeatherProposalContext context;context.directionPerk=3;
            auto a=proposeWeather(context,headRng);context.directionPerk=-3;auto b=proposeWeather(context,tailRng);
            heads+=a.wind.references.front().value.head>0;tails+=b.wind.references.front().value.head>0;
            context.directionPerk=0;context.variabilityPerk=-3;auto c=proposeWeather(context,stableRng);
            context.variabilityPerk=3;auto d=proposeWeather(context,lotteryRng);
            stable+=c.wind.temporalAmplitude<.2;lottery+=d.wind.temporalAmplitude<.2;
        }
        check(heads>9000 && tails<1000,"direction traits strongly bias automatic proposals");
        check(stable>8000 && lottery<1200,"lottery traits strongly bias automatic proposals");
        rejects([]{WindParameters v;v.references.clear();ContinuousWeather f(1,v);});
        rejects([]{WindParameters v;v.references={{10,{}},{10,{}}};ContinuousWeather f(1,v);});
        rejects([&]{weather.wind(0,std::numeric_limits<double>::infinity());});
        rejects([&]{weather.wind(-1,0);});
        rejects([]{WeatherProposalContext c;c.directionPerk=4;SeededRandom r(0);proposeWeather(c,r);});
        std::cout<<"PASS simulation-v2 weather: continuity, correlation, persistent/reversing tendencies, activity episodes, moving gusts, parallel queries, 10000 proposals\n";
    } catch(const std::exception &e) { std::cerr<<"FAIL: "<<e.what()<<'\n';return 1; }
}
