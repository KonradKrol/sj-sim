#include "FormEngine.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QSaveFile>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>

namespace AutoForm {
namespace {
using Field=std::function<void(const QString &,double &,double,double,double,const QString &,const QString &,const QString &)>;
QString tr(const char *text){return QCoreApplication::translate("FormEngine",text);}
void fields(Config &c,const Field &f) {
    const auto item=[&](const QString &key,double &value,double lo,double hi,double step,const QString &group,const QString &name,const QString &help){f(key,value,lo,hi,step,group,name,help);};
    for(int r=0;r<5;++r) {
        const auto name=regimeName(r);
        item(QString("center.%1").arg(r),c.centers[r],1,100,1,tr("Okresy formy"),tr("Centrum bazy: %1").arg(name),tr("Baza powoli dąży do tej wartości w tym okresie. Nie ustawia bezpośrednio aktualnej formy."));
        item(QString("days.%1").arg(r),c.meanDays[r],.25,10000,5,tr("Okresy formy"),tr("Średni czas: %1").arg(name),tr("Dni. To średnia, nie termin: okres może trwać znacznie dłużej."));
        item(QString("entry.%1").arg(r),c.entryDeviation[r],0,50,.5,tr("Okresy formy"),tr("Rozrzut nowej bazy: %1").arg(name),tr("Odchylenie przy wejściu w okres. Większa wartość zwiększa różnice baz między zawodnikami."));
        for(int to=0;to<5;++to)if(to!=r)item(QString("transition.%1.%2").arg(r).arg(to),c.weights[r][to],.0001,10000,1,tr("Przejścia"),name+" → "+regimeName(to),tr("Waga względem innych przejść z tego samego okresu, nie procent. Np. 2 i 1 oznaczają dwa razy większą szansę pierwszego przejścia."));
    }
    item("formHalfLife",c.formHalfLife,.25,1000,.5,tr("Zmiany formy"),tr("Powrót formy do bazy — dni"),tr("Po tylu dniach pozostaje połowa początkowej różnicy względem stałej bazy, pomijając losowość."));
    item("formDeviation",c.formDeviation,0,50,.5,tr("Zmiany formy"),tr("Zmienność formy"),tr("Docelowe odchylenie przy nieruchomej bazie, bez ograniczeń 1–100. Np. 6 daje typowe różnice kilku punktów; rzadziej większe."));
    item("baseHalfLife",c.baseHalfLife,1,10000,5,tr("Zmiany formy"),tr("Powrót bazy do centrum — dni"),tr("Tempo długoterminowego powrotu bazy do centrum okresu. Większa liczba oznacza wolniejsze zmiany."));
    item("baseDeviation",c.baseDeviation,0,50,.5,tr("Zmiany formy"),tr("Powolne wahania bazy"),tr("Docelowe odchylenie bazy przy niezmiennym okresie. Zero wyłącza losowy dryf, ale zachowuje powrót do centrum."));
    item("entryInertia",c.entryInertia,0,1,.05,tr("Zmiany formy"),tr("Zachowanie poprzedniej bazy"),tr("Przy zmianie okresu: 0 wybiera okolice nowego centrum, 1 zachowuje poprzednią bazę przed dodaniem losowego odchylenia."));
    item("initialDeviation",c.initialDeviation,0,50,.5,tr("Początek"),tr("Początkowy rozrzut bazy"),tr("Jak daleko ukryta baza może leżeć od wpisanej formy. Sama forma początkowa pozostaje bez zmian."));
    item("initialOutlierChance",c.initialOutlierChance,0,1,.01,tr("Początek"),tr("Szansa większej różnicy początkowej"),tr("Np. 0,05 oznacza 5% inicjalizacji z większym rozrzutem bazy."));
    item("initialOutlierDeviation",c.initialOutlierDeviation,0,50,.5,tr("Początek"),tr("Rozrzut przy większej różnicy"),tr("Odchylenie bazy w rzadkim wariancie początkowym. Nie zmienia wpisanej formy zawodnika."));
    for(int p=-4;p<=4;++p) {
        const auto suffix=p==0?tr("Neutralny"):p<0?tr("Równa %1").arg(-p):tr("Chwiejna %1").arg(p);
        item(QString("noise.%1").arg(p+4),c.noise[p+4],.01,5,.05,tr("Perk formy"),tr("Wahania: %1").arg(suffix),tr("Mnożnik losowych zmian aktualnej formy. 0,7 zmniejsza odchylenie o 30%; 1,5 zwiększa o 50%."));
        item(QString("crisis.%1").arg(p+4),c.crisis[p+4],.001,20,.1,tr("Perk formy"),tr("Kryzys: %1").arg(suffix),tr("Mnożnik wagi wejścia w głęboki kryzys. Pozostałe szanse są następnie normalizowane; czas trwania okresów nie zmienia się."));
        item(QString("peak.%1").arg(p+4),c.peak[p+4],.001,20,.1,tr("Perk formy"),tr("Szczyt: %1").arg(suffix),tr("Mnożnik wagi wejścia w szczytowy okres. 0,8 zmniejsza tę wagę o 20%, przed normalizacją."));
    }
}
double bounded(double x){return std::clamp(x,1.,100.);}
double laplace(SimulationV2::RandomSource &rng,double deviation){return rng.laplace(0,deviation/std::sqrt(2.));}
double waiting(int r,SimulationV2::RandomSource &rng,const Config &c){return -std::log(rng.unit())*c.meanDays[r];}
void drift(State &s,double days,SimulationV2::RandomSource &rng,const Config &c){
    if(days<=0)return;
    const double a=std::exp2(-days/c.baseHalfLife);
    s.base=bounded(c.centers[s.regime]+a*(s.base-c.centers[s.regime])+rng.normal(0,c.baseDeviation*std::sqrt(-std::expm1(-2*std::log(2.)*days/c.baseHalfLife))));
}
QString configPath(){const auto override=QCoreApplication::instance()?QCoreApplication::instance()->property("formEngineStorageRoot").toString():QString();return (override.isEmpty()?QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/form-engine":override)+"/default.json";}
}
QString regimeName(int r){static const char *names[]{"Głęboki kryzys","Słaby okres","Typowy okres","Dobry okres","Szczytowy okres"};return tr(names[std::clamp(r,0,4)]);}
bool Config::validate(QString &error) const {
    auto copy=*this;bool ok=true;
    fields(copy,[&](const QString &key,double &v,double lo,double hi,double,const QString &,const QString &,const QString &){if(!std::isfinite(v) || v<lo || v>hi){error=tr("Parametr poza zakresem: %1").arg(key);ok=false;}});
    for(int i=1;i<5;++i)if(centers[i]<=centers[i-1]){error=tr("Centra kolejnych okresów muszą rosnąć.");ok=false;}
    for(int i=0;i<5;++i)if(weights[i][i]!=0){error=tr("Waga pozostania w tym samym okresie musi wynosić zero.");ok=false;}
    if(noise[4]!=1 || crisis[4]!=1 || peak[4]!=1){error=tr("Neutralny perk musi mieć mnożniki równe 1.");ok=false;}
    if(ok)error.clear();return ok;
}
QJsonObject Config::toJson() const {
    QJsonObject values;auto copy=*this;fields(copy,[&](const QString &key,double &v,double,double,double,const QString &,const QString &,const QString &){values[key]=v;});return {{"version",version},{"values",values}};
}
bool Config::fromJson(const QJsonObject &json,Config &out,QString &error) {
    if(json["version"].toDouble()!=version || !json["values"].isObject()){error=tr("Nieobsługiwana konfiguracja silnika formy.");return false;}
    const auto values=json["values"].toObject();Config candidate;int count=0;bool ok=true;
    fields(candidate,[&](const QString &key,double &v,double,double,double,const QString &,const QString &,const QString &){++count;if(!values[key].isDouble()){error=tr("Brak parametru: %1").arg(key);ok=false;}else v=values[key].toDouble();});
    if(values.size()!=count){error=tr("Nieznane parametry silnika formy.");return false;}
    if(!ok || !candidate.validate(error))return false;out=candidate;return true;
}
QVariantList Config::definitions(){
    Config c;QVariantList result;fields(c,[&](const QString &key,double &v,double lo,double hi,double step,const QString &group,const QString &label,const QString &help){result.append(QVariantMap{{"key",key},{"value",v},{"minimum",lo},{"maximum",hi},{"step",step},{"group",group},{"label",label},{"help",help},{"editable",key!="noise.4" && key!="crisis.4" && key!="peak.4"}});});return result;
}
int State::displayed() const{return int(std::round(bounded(form)));}
QJsonObject State::toJson() const{return {{"form",form},{"base",base},{"regime",regime},{"remainingDays",remainingDays},{"visited",visited},{"transitions",transitions},{"random",QString::number(randomState)},{"date",date.toString(Qt::ISODate)}};}
bool State::fromJson(const QJsonObject &j,State &out,QString &error){
    State s;bool randomOk=false;s.randomState=j["random"].toString().toULongLong(&randomOk);s.date=QDate::fromString(j["date"].toString(),Qt::ISODate);
    const auto number=[&](const char *key,double lo,double hi,double &v){v=j[key].toDouble();return j[key].isDouble() && std::isfinite(v) && v>=lo && v<=hi;};
    double r=0,mask=0,transitions=0;
    if(!randomOk || !s.date.isValid() || !number("form",1,100,s.form) || !number("base",1,100,s.base) || !number("remainingDays",0,1e12,s.remainingDays) || s.remainingDays<=0 || !number("regime",0,4,r) || std::floor(r)!=r || !number("visited",1,31,mask) || std::floor(mask)!=mask || !(int(mask)&(1<<int(r))) || !number("transitions",0,1e8,transitions) || std::floor(transitions)!=transitions){error=tr("Nieprawidłowy stan automatycznej formy.");return false;}
    s.regime=int(r);s.visited=int(mask);s.transitions=int(transitions);out=s;error.clear();return true;
}
std::array<double,5> transitionProbabilities(int r,int perk,const Config &c){
    auto weights=c.weights.at(r);const int p=std::clamp(perk,-4,4)+4;weights[0]*=c.crisis[p];weights[4]*=c.peak[p];double total=0;for(double v:weights)total+=v;for(auto &v:weights)v/=total;return weights;
}
State initialize(double form,QDate date,SimulationV2::Seed seed,const Config &c){
    if(!date.isValid() || !std::isfinite(form) || form<1 || form>100)throw std::invalid_argument("Invalid initial form/date");
    SimulationV2::SeededRandom rng(seed);State s;s.form=form;s.date=date;
    s.base=bounded(form+laplace(rng,rng.event(c.initialOutlierChance)?c.initialOutlierDeviation:c.initialDeviation));
    s.regime=0;for(int r=1;r<5;++r)if(std::abs(s.base-c.centers[r])<std::abs(s.base-c.centers[s.regime]))s.regime=r;
    s.visited=1<<s.regime;s.remainingDays=waiting(s.regime,rng,c);s.randomState=rng.snapshot();return s;
}
void advance(State &s,QDate target,int perk,const Config &c,const std::function<void(const State &)> &observer){
    const qint64 days=s.date.daysTo(target);if(!target.isValid() || days<0 || days>36525)throw std::invalid_argument("Invalid form time interval");
    SimulationV2::SeededRandom rng(s.randomState);
    const double a=std::exp2(-1/c.formHalfLife),deviation=c.formDeviation*c.noise[std::clamp(perk,-4,4)+4]*std::sqrt(-std::expm1(-2*std::log(2.)/c.formHalfLife));
    for(qint64 day=0;day<days;++day){
        double left=1;
        while(s.remainingDays<=left){
            drift(s,s.remainingDays,rng,c);left-=s.remainingDays;
            const auto probabilities=transitionProbabilities(s.regime,perk,c);s.regime=int(rng.weightedIndex(std::vector<double>(probabilities.begin(),probabilities.end())));
            s.base=bounded(c.entryInertia*s.base+(1-c.entryInertia)*c.centers[s.regime]+laplace(rng,c.entryDeviation[s.regime]));
            s.visited|=1<<s.regime;++s.transitions;s.remainingDays=waiting(s.regime,rng,c);
        }
        drift(s,left,rng,c);s.remainingDays-=left;
        s.form=bounded(s.base+a*(s.form-s.base)+laplace(rng,deviation));s.date=s.date.addDays(1);
        if(observer)observer(s);
    }
    s.randomState=rng.snapshot();
}
Config defaultConfig(QString *error){
    Config config;QFile file(configPath());if(!file.exists())return config;
    QString message;
    if(!file.open(QIODevice::ReadOnly) || file.size()>1024*1024 || !Config::fromJson(QJsonDocument::fromJson(file.readAll()).object(),config,message)){
        if(error)*error=message.isEmpty()?tr("Nie udało się odczytać domyślnej konfiguracji formy."):message;
        return Config();
    }
    return config;
}
bool saveDefaultConfig(const Config &c,QString &error){
    if(!c.validate(error))return false;
    const auto path=configPath();QDir().mkpath(QFileInfo(path).absolutePath());QSaveFile file(path);const auto bytes=QJsonDocument(c.toJson()).toJson();
    if(!file.open(QIODevice::WriteOnly) || file.write(bytes)!=bytes.size() || !file.commit()){error=tr("Nie udało się zapisać konfiguracji formy.");return false;}return true;
}
}
