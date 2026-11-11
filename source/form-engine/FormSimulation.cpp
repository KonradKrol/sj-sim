#include "FormSimulation.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <future>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace AutoForm {
namespace {
struct Cancelled {};
struct Run {
    std::array<double,7> metrics{};
    std::array<double,20> histogram{};
    QJsonArray forms,bases,regimes,population,traces;
};
int bits(int mask){int count=0;for(;mask;mask>>=1)count+=mask&1;return count;}
double student95(int df){
    const double z=1.959963984540054;
    if(df>30)return z+(z*z*z+z)/(4*df)+(5*std::pow(z,5)+16*z*z*z+3*z)/(96.*df*df);
    const double scale=std::exp(std::lgamma((df+1)/2.)-std::lgamma(df/2.))/std::sqrt(df*3.14159265358979323846);
    const auto density=[&](double x){return scale*std::pow(1+x*x/df,-(df+1)/2.);};
    double lo=0,hi=32;
    for(int iteration=0;iteration<40;++iteration){
        const double mid=(lo+hi)/2,step=mid/512;double sum=density(0)+density(mid);
        for(int i=1;i<512;++i)sum+=(i%2?4:2)*density(i*step);
        if(sum*step/3<.475)lo=mid;else hi=mid;
    }
    return (lo+hi)/2;
}
QJsonObject point(int x,double y){return {{"x",x},{"y",y}};}
Run run(const Experiment &e,int repeat,int variant,const Config &config,const std::shared_ptr<Control> &control){
    Run out;double sum=0,squares=0,ever=0,visited=0,transitions=0,jumps=0,peakDays=0;
    QVector<double> daily(e.days+1);QJsonArray samples;
    const auto root=SimulationV2::deriveSeed(e.seed,repeat);
    const int stride=qMax(1,qMax((e.days+499)/500,int((qint64(e.days)*e.athletes.size()+99999)/100000)));
    for(int i=0;i<e.athletes.size();++i){
        if(control->cancelled.load())throw Cancelled();
        const auto &athlete=e.athletes[i];auto s=initialize(athlete.form,e.start,SimulationV2::deriveSeed(root,i),config,athlete.perk);bool reached=s.form>=90;double previous=s.form;
        samples=QJsonArray();
        const auto collect=[&](const State &state){
            if(control->cancelled.load())throw Cancelled();
            const int day=int(e.start.daysTo(state.date));sum+=state.form;squares+=state.form*state.form;daily[day]+=state.form;peakDays+=state.form>=90;
            out.histogram[qBound(0,int((state.form-1)/5),19)]++;
            if(day>0 && std::abs(state.form-previous)>=10)++jumps;
            previous=state.form;reached=reached || state.form>=90;
            if(repeat==0 && (day%stride==0 || day==e.days))
                samples.append(QJsonArray{day,state.form,state.base,state.regime});
        };
        collect(s);advance(s,e.start.addDays(e.days),athlete.perk,config,collect);
        if(repeat==0)out.traces.append(QJsonObject{{"name",athlete.name},{"samples",samples}});
        ever+=reached;visited+=bits(s.visited);transitions+=s.transitions;
        control->completed.fetch_add(e.days);
    }
    const double n=e.athletes.size(),observations=n*(e.days+1);
    out.metrics={{sum/observations,std::sqrt(std::max(0.,squares/observations-std::pow(sum/observations,2))),ever/n*100,visited/n,transitions/n,jumps/(n*e.days)*100,peakDays/observations*100}};
    for(auto &v:out.histogram)v=v/observations*100;
    if(repeat==0){
        QVector<int> selected;for(int i=0;i<qMin(10,e.athletes.size());++i)selected.append(i);
        const auto plots=plotTraces(out.traces,selected);
        out.forms=plots["forms"].toArray();out.bases=plots["bases"].toArray();out.regimes=plots["regimes"].toArray();
        QJsonArray points;for(int day=0;day<=e.days;++day)if(day%stride==0 || day==e.days)points.append(point(day,daily[day]/n));
        out.population.append(QJsonObject{{"name",variant==0?QString("A"):QString("B")},{"colorIndex",variant},{"points",points}});
    }
    return out;
}
}
QJsonObject plotTraces(const QJsonArray &traces,const QVector<int> &selected){
    QJsonArray series[3];
    for(int color=0;color<qMin(10,selected.size());++color){
        const int index=selected[color];if(index<0 || index>=traces.size())continue;
        const auto trace=traces[index].toObject();QJsonArray points[3];
        for(const auto &sample:trace["samples"].toArray()){
            const auto row=sample.toArray();
            for(int field=0;field<3;++field){
                if(field==2 && !points[field].isEmpty())points[field].append(point(row[0].toInt(),points[field].last().toObject()["y"].toDouble()));
                points[field].append(point(row[0].toInt(),row[field+1].toDouble()));
            }
        }
        for(int field=0;field<3;++field)series[field].append(QJsonObject{{"name",trace["name"]},{"colorIndex",color},{"points",points[field]}});
    }
    return {{"forms",series[0]},{"bases",series[1]},{"regimes",series[2]}};
}
bool Experiment::validate(QString &error) const{
    if(!config.validate(error) || (compare && !reference.validate(error)))return false;
    if(!start.isValid() || days<1 || days>3650 || !start.addDays(days).isValid() || repeats<1 || repeats>10000 || athletes.isEmpty() || athletes.size()>10000 || threads<1 || threads>32 || qint64(days)*repeats*athletes.size()*(compare?2:1)>50000000){error=QStringLiteral("Nieprawidłowa próba lub ponad 50 mln osobodni. Zmniejsz liczbę zawodników, dni albo powtórzeń.");return false;}
    for(const auto &a:athletes)if(!std::isfinite(a.form) || a.form<1 || a.form>100 || a.perk< -4 || a.perk>4){error=QStringLiteral("Nieprawidłowa forma lub perk zawodnika próby.");return false;}
    return true;
}
QJsonObject simulate(const Experiment &e,const std::shared_ptr<Control> &control){
    QString error;if(!e.validate(error))throw std::invalid_argument(error.toStdString());
    const int variants=e.compare?2:1,tasks=e.repeats*variants;std::vector<Run> results(tasks);
    std::atomic<int> next{0};std::exception_ptr failure;std::mutex failureMutex;
    const auto worker=[&]{try{
        for(int index=next.fetch_add(1);index<tasks && !control->cancelled.load();index=next.fetch_add(1)){
            const int variant=index%variants;
            results[index]=run(e,index/variants,variant,e.compare && variant==0?e.reference:e.config,control);
        }
    }catch(const Cancelled &){}catch(...){std::lock_guard<std::mutex> lock(failureMutex);failure=std::current_exception();control->cancelled.store(true);}};
    std::vector<std::future<void>> pool;
    for(int i=0;i<std::min(e.threads,tasks);++i)pool.emplace_back(std::async(std::launch::async,worker));
    for(auto &thread:pool)thread.get();
    if(failure)std::rethrow_exception(failure);
    if(control->cancelled.load())return {{"cancelled",true}};
    const char *labels[]{"Średnia forma","Odchylenie formy","Zawodnicy z formą ≥90 (%)","Odwiedzone okresy / zawodnik","Zmiany okresu / zawodnik","Zmiany ≥10 pkt/dzień (%)","Dni z formą ≥90 (%)"};
    QJsonArray metrics,histograms,views,traces;const double critical=e.repeats>1?student95(e.repeats-1):0;
    for(int v=0;v<variants;++v){
        QJsonArray values,histogram;
        for(int m=0;m<7;++m){
            double sum=0,squares=0;for(int r=0;r<e.repeats;++r){const double x=results[r*variants+v].metrics[m];sum+=x;squares+=x*x;}
            const double mean=sum/e.repeats,variance=e.repeats>1?std::max(0.,(squares-sum*sum/e.repeats)/(e.repeats-1)):0;
            QJsonObject metric{{"label",QString::fromUtf8(labels[m])},{"mean",mean}};
            if(e.repeats>1)metric["ci95"]=critical*std::sqrt(variance/e.repeats);
            values.append(metric);
        }
        for(int bin=0;bin<20;++bin){double sum=0;for(int r=0;r<e.repeats;++r)sum+=results[r*variants+v].histogram[bin];histogram.append(point(bin*5+3,sum/e.repeats));}
        metrics.append(values);histograms.append(QJsonObject{{"name",v==0?QString("A"):QString("B")},{"colorIndex",v},{"points",histogram}});
        const auto &trace=results[v];traces.append(trace.traces);views.append(QJsonObject{{"forms",trace.forms},{"bases",trace.bases},{"regimes",trace.regimes},{"population",trace.population}});
    }
    QJsonArray roster;for(const auto &athlete:e.athletes)roster.append(QJsonObject{{"name",athlete.name},{"form",athlete.form},{"perk",athlete.perk}});
    return {{"version",version},{"seed",QString::number(e.seed)},{"days",e.days},{"athletes",e.athletes.size()},{"roster",roster},{"repeats",e.repeats},{"start",e.start.toString(Qt::ISODate)},{"config",e.config.toJson()},{"reference",e.reference.toJson()},{"compare",e.compare},{"metrics",metrics},{"histograms",histograms},{"views",views},{"traces",traces}};
}
}
