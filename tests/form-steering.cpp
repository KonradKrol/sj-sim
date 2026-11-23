#include "../source/form-engine/FormSession.h"
#include "../source/simulator/Jumper.h"
#include <QApplication>
#include <iostream>
#include <cmath>
#include <cstdlib>
static void check(bool ok,const char *message){if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
int main(int argc,char **argv){
 QApplication app(argc,argv);QString error;Jumper athlete;auto profile=AthleteProfile::create();profile.name="Test";profile.surname="Athlete";profile.form=50;check(athlete.setSimulationProfile(profile,error),"valid athlete");
 AutoForm::Session session;session.config=AutoForm::Config{};session.enabled=true;session.anchorPending=false;
 AutoForm::State initial;initial.form=50;initial.base=50;initial.regime=3;initial.visited=8;initial.remainingDays=100;initial.date=QDate(2026,11,23);initial.randomState=42;
 const auto key=athlete.getIDStr();session.states[key]=initial;
 check(session.steer(athlete,0,0,error),"gradual rise succeeds");auto &state=session.states[key];check(state.regime==4 && state.form==50 && state.steeringLocked,"gradual changes target, not immediate form");
 for(int first=0;first<=6;++first){
  session.states[key]=initial;
  check(session.steer(athlete,first,first==4?73:0,error),"each action can be the first");
  const auto locked=state.toJson();const int form=athlete.getSimulationProfile()->form;
  for(int next=0;next<=6;++next)
   check(!session.steer(athlete,next,next==4?71:0,error) && state.toJson()==locked
         && athlete.getSimulationProfile()->form==form,"all actions are blocked without changes");
 }
 AutoForm::Session loaded;check(AutoForm::Session::fromJson(session.toJson(),loaded,error) && loaded.states[key].steeringLocked,"lock survives save/load");
 state.remainingDays=.1;AutoForm::advance(state,state.date.addDays(1),0,session.config);check(!state.steeringLocked,"natural regime transition unlocks all actions");
 session.states[key]=initial;check(session.steer(athlete,2,0,error),"crisis succeeds");check(session.states[key].regime==0 && athlete.getSimulationProfile()->form==12,"crisis is immediate");
 session.states[key]=initial;check(session.steer(athlete,3,0,error),"peak succeeds");check(session.states[key].regime==6 && athlete.getSimulationProfile()->form==98,"peak is immediate");
 session.states[key]=initial;check(session.steer(athlete,4,73,error) && athlete.getSimulationProfile()->form==73,"exact value applied");const auto before=session.toJson();check(!session.steer(athlete,4,101,error) && before==session.toJson() && athlete.getSimulationProfile()->form==73,"invalid exact value is atomic");
 for(int action:{5,6}){
  double sum=0;bool one=false,two=false,three=false;
  for(int i=0;i<3000;++i){session.states[key]=initial;session.states[key].randomState=i+1;check(session.steer(athlete,action,0,error),"sudden action succeeds");const auto s=session.states[key];const int steps=std::abs(s.regime-3);one|=steps==1;two|=steps==2;three|=steps==3;const double delta=s.form-50;check((action==5?delta>0:delta<0) && s.base==50,"immediate directional change preserves baseline");if(action==5 && steps==1)check(delta>=1.30*(session.config.centers[4]-50),"weakest upward shock is stronger");sum+=std::abs(delta);}
  const double gap=std::abs(session.config.centers[action==5?4:2]-50);check(one&&two&&three && std::abs(sum/3000/gap-(action==5?1.755:1.5))<.08,"sudden changes span 1-3 regimes at the calibrated strength");
 }
 session.states.clear();
 check(session.steer(athlete,4,60,error) && !session.steer(athlete,4,70,error),"precise edit locks before the first event");
 AutoForm::Session pending;
 check(AutoForm::Session::fromJson(session.toJson(),pending,error) && !pending.steer(athlete,4,70,error),"pending lock survives reload");
 check(pending.update(initial.date,{&athlete},true,error) && pending.states[key].steeringLocked,
       "first event carries the lock into the athlete state");
 session.enabled=false;check(session.steer(athlete,4,1,error) && athlete.getSimulationProfile()->form==1,"exact works without automatic form");check(!session.steer(athlete,5,0,error),"dynamic action requires automatic form");
 std::cout<<"PASS: all-action lock, persistence, natural unlock, exact bounds, stronger upward shock\n";
}
