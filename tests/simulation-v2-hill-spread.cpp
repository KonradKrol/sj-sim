#include "../source/simulation-v2/Engine.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace SimulationV2;
struct Calm:WeatherField {
    Wind wind(double,double)const override{return {};}
    double precipitation(double)const override{return 0;}
};
int main(){
    try{
        Engine engine;Calm calm;
        for(int overall:{60,80}){
            JumpRequest request;request.athlete.overall=overall;request.athlete.form=50;
            const double base=engine.previewDistance(request,calm);
            for(int level:{-4,-1,0,1,4}){
                request.hill.distanceSpreadPerk=level;
                const double actual=engine.previewDistance(request,calm);
                const double expected=request.hill.k+(base-request.hill.k)*(1+.02*level*(level<0?5.:1.));
                if(std::abs(actual-expected)>.001)throw std::runtime_error("incorrect final spread");
            }
            auto disabled=EngineParameters{};disabled.flight.hillSpreadPerLevel=0;
            if(std::abs(Engine(disabled).previewDistance(request,calm)-base)>1e-8)throw std::runtime_error("cannot disable perk");
        }
        std::cout<<"PASS: compact/spacious hill levels, both sides of K, neutral and disabled effects\n";
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
