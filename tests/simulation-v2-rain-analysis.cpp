#include "../source/simulation-v2/Engine.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace SimulationV2;
namespace {
struct Conditions final:WeatherField {
    double intensity=0;
    Wind wind(double,double) const override {return {};}
    double precipitation(double) const override {return intensity;}
    PrecipitationKind precipitationKind() const override {return PrecipitationKind::Rain;}
};
}
int main(int argc,char **argv) {
    try {
        const int count=argc>1?std::stoi(argv[1]):5000;
        if(count<1 || count>1000000)throw std::invalid_argument("sample count must be 1..1000000");
        std::cout<<"K,HS,rain,model,jumps,mean_metres,mean_points,falls_percent\n"<<std::fixed<<std::setprecision(3);
        for(const auto &dimensions:{std::pair<double,double>{95,109},{120,134},{200,225}})for(double intensity:{0.,2.,5.,8.,10.})for(int revised=0;revised<2;++revised) {
            EngineParameters parameters;if(!revised)parameters.flight.rainHeavyLossAtMaximum=0;
            Engine engine(parameters);JumpRequest input;input.hill.k=dimensions.first;input.hill.hs=input.hill.realHs=dimensions.second;
            input.hill.pointsAtK=dimensions.first>=165?120:60;input.hill.pointsPerMetre=dimensions.first>=165?1.2:dimensions.first<100?2:1.8;
            input.athlete.overall=80;input.athlete.form=70;input.scoring.juryGate=input.scoring.referenceGate=10;input.captureExposure=false;
            Conditions weather;weather.intensity=intensity;SensorWindMeasurement measurement({{0,1},{dimensions.first*.5,1},{dimensions.first,1}});
            double distance=0,points=0;int falls=0;
            for(int i=0;i<count;++i){SeededRandom rng(deriveSeed(20261028,i));const auto result=engine.simulate(input,rng,weather,measurement);distance+=result.measuredDistance;points+=result.score.total;falls+=result.landing==LandingKind::Fall;}
            std::cout<<dimensions.first<<','<<dimensions.second<<','<<intensity<<','<<(revised?"revised":"previous")<<','<<count<<','<<distance/count<<','<<points/count<<','<<100.*falls/count<<'\n';
        }
    } catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}
}
