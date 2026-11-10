#include "../source/form-engine/FormEngine.h"
#include "../source/form-engine/FormSimulation.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <cstdlib>
#include <iostream>

void check(bool ok,const char *message){if(!ok){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}}
int main(int argc,char **argv){
    QCoreApplication app(argc,argv);QTemporaryDir temporary;app.setProperty("formEngineStorageRoot",temporary.path());
    AutoForm::Config c,copy;QString error;check(c.validate(error),"defaults validate");check(AutoForm::Config::fromJson(c.toJson(),copy,error) && copy.toJson()==c.toJson(),"config roundtrip");
    QFile calibrationFile(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath()+"/../docs/calibration/form-defaults-2026-11-10.json");
    check(calibrationFile.open(QIODevice::ReadOnly),"recorded local form calibration exists");
    check(c.toJson()==QJsonDocument::fromJson(calibrationFile.readAll()).object(),"hardcoded form defaults exactly match the owner's local profile");
    auto broken=c.toJson();auto values=broken["values"].toObject();values["days.0"]=0;broken["values"]=values;check(!AutoForm::Config::fromJson(broken,copy,error),"reject zero holding time");
    check(AutoForm::saveDefaultConfig(c,error) && AutoForm::defaultConfig().toJson()==c.toJson(),"default config atomic roundtrip");
    const QDate start(2026,11,1);
    for(int f:{1,20,50,90,100})for(int seed=0;seed<100;++seed){auto s=AutoForm::initialize(f,start,seed,c);check(s.form==f,"preserve F0 exactly");}
    for(int perk=-4;perk<=4;++perk){
        auto a=AutoForm::initialize(70,start,SimulationV2::Seed(100+perk),c),b=a;
        AutoForm::advance(a,start.addDays(150),perk,c);
        for(int day=1;day<=150;++day)AutoForm::advance(b,start.addDays(day),perk,c);
        check(a.toJson()==b.toJson(),"chunk-independent updates including CTMC");
        auto saved=QJsonDocument::fromJson(QJsonDocument(b.toJson()).toJson()).object();AutoForm::State restored;
        check(AutoForm::State::fromJson(saved,restored,error),"state load");
        AutoForm::advance(a,start.addDays(1000),perk,c);AutoForm::advance(restored,start.addDays(1000),perk,c);
        check(a.toJson()==restored.toJson(),"exact RNG and latent state continuation after save");
        const auto unchanged=a.toJson();AutoForm::advance(a,a.date,perk,c);check(a.toJson()==unchanged,"same-day update consumes no RNG");
        bool rejected=false;try{AutoForm::advance(a,start,perk,c);}catch(const std::invalid_argument &){rejected=true;}check(rejected,"reject backwards time");
        auto probabilities=AutoForm::transitionProbabilities(2,perk,c);double sum=0;for(double p:probabilities){check(std::isfinite(p) && p>=0,"finite transition probability");sum+=p;}check(std::abs(sum-1)<1e-12 && probabilities[2]==0,"normalized CTMC row");
    }
    const auto stable=AutoForm::transitionProbabilities(2,-4,c),neutral=AutoForm::transitionProbabilities(2,0,c),volatileForm=AutoForm::transitionProbabilities(2,4,c);
    check(stable[0]<neutral[0] && neutral[0]<volatileForm[0],"form perk changes crisis risk in correct direction");
    check(stable[4]<neutral[4] && neutral[4]<volatileForm[4],"form perk changes peak chance in correct direction");
    // Independent daily draws, away from boundaries; test the variance-normalized Laplace noise.
    c.baseDeviation=0;double sum=0,squares=0;
    for(int i=0;i<100000;++i){AutoForm::State s;s.date=start;s.remainingDays=1e8;s.randomState=SimulationV2::deriveSeed(77,i);AutoForm::advance(s,start.addDays(1),0,c);const double x=s.form-50;sum+=x;squares+=x*x;}
    const double variance=squares/100000-std::pow(sum/100000,2),expected=c.formDeviation*c.formDeviation*(1-std::exp2(-2/c.formHalfLife));
    check(std::abs(sum/100000)<.05 && std::abs(variance/expected-1)<.035,"daily noise has specified variance, not twice that variance");
    AutoForm::Experiment experiment;experiment.repeats=8;experiment.days=150;experiment.threads=1;
    for(int i=0;i<40;++i)experiment.athletes.append({QString::number(i),50.,i%9-4});
    auto control=std::make_shared<AutoForm::Control>();const auto serial=AutoForm::simulate(experiment,control);
    check(control->completed.load()==qint64(8)*150*40,"batch progress reaches exact total");
    experiment.threads=4;const auto threaded=AutoForm::simulate(experiment,std::make_shared<AutoForm::Control>());
    check(serial==threaded,"batch is deterministic across thread counts");
    experiment.compare=true;experiment.reference=experiment.config;
    const auto comparison=AutoForm::simulate(experiment,std::make_shared<AutoForm::Control>());
    const auto metrics=comparison["metrics"].toArray();check(metrics.size()==2 && metrics[0]==metrics[1],"identical A/B configs share identical samples");
    control=std::make_shared<AutoForm::Control>();control->cancelled=true;check(AutoForm::simulate(experiment,control)["cancelled"].toBool(),"batch cancellation");
    experiment.days=3650;experiment.repeats=10000;check(!experiment.validate(error),"batch bounds prevent unbounded work");
    AutoForm::Experiment calibration;calibration.days=150;calibration.repeats=30;calibration.threads=4;
    double stableMean=0,volatileMean=0;
    for(int perk:{-4,0,4}){
        calibration.athletes.clear();for(int i=0;i<100;++i)calibration.athletes.append({QString::number(i),50.,perk});
        const auto result=AutoForm::simulate(calibration,std::make_shared<AutoForm::Control>());const auto stats=result["metrics"].toArray()[0].toArray();
        const auto mean=stats[0].toObject()["mean"].toDouble();if(perk==-4)stableMean=mean;if(perk==4)volatileMean=mean;
        std::cout<<"perk="<<perk<<" mean F="<<mean<<" visited regimes="<<stats[3].toObject()["mean"].toDouble()<<'\n';
    }
    check(stableMean>volatileMean,"working calibration gives stable form an average advantage over volatile form");
    std::cout<<"PASS: form initialization, CTMC normalization, daily invariance, persistence, RNG and Laplace variance\n";
}
