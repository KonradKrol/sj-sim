#include "PlaygroundModel.h"
#include <algorithm>
#include <QImage>
#include <QBuffer>
#include <QMap>
#include <cmath>
#include <stdexcept>

namespace Playground {
QJsonObject diagnostics(const QJsonObject &config,int variant,const std::shared_ptr<Control> &control) {
    QString error;if(!validate(config,error))throw std::invalid_argument(error.toStdString());
    if(variant<0 || variant>=config["variants"].toArray().size())throw std::out_of_range("Analysis variant");
    auto p=valuesFor(config,variant);auto ep=engineParameters(p);SimulationV2::Engine engine(ep);auto hill=hillInput(p);auto athlete=athleteInput(p,"athlete");
    auto seed=config["seed"].toString().toULongLong();SimulationV2::ContinuousWeather weather(seed,windParameters(p),precipitationParameters(p));
    auto canceled=[&]{return control && control->canceled.load(std::memory_order_relaxed);};
    QJsonArray curves,timeline,spatial,grid;const double cross=std::abs(p["wind.cross"].toDouble());
    // Exact landing probabilities. Judge expectations include clipping/rounding and
    // are Monte Carlo estimates, sharing seeds between neighbouring distances.
    const int samples=128;
    for(int i=0;i<=80;++i){if(canceled())return {};double d=hill.realHs*(.6+.6*i/80);auto probabilities=engine.landing().probabilities(d,hill.realHs,cross);QJsonArray probs;for(double v:probabilities.values)probs.append(v);
        double markSum=0;std::array<int,4> observed{};double sampledNotes=0;int high=0;
        for(int n=0;n<samples;++n){SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(seed,n));auto kind=engine.landing().sample(d,hill.realHs,cross,rng);++observed[int(kind)];auto marks=engine.landing().judges(athlete.landing,kind,d,hill.realHs,rng);double note=0;for(double mark:marks){note+=mark/5;if(mark>=19.5)++high;}sampledNotes+=note;}
        for(int kind=0;kind<4;++kind){double sum=0;for(int n=0;n<samples;++n){SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(seed,n));auto marks=engine.landing().judges(athlete.landing,SimulationV2::LandingKind(kind),d,hill.realHs,rng);double note=0;for(double mark:marks)note+=mark/5;sum+=note;}markSum+=probabilities.values[kind]*sum/samples;}
        QJsonArray empirical;for(int n:observed)empirical.append(double(n)/samples);
        curves.append(QJsonObject{{"distance",d},{"probabilities",probs},{"observed",empirical},{"meanMark",markSum},{"sampledMark",sampledNotes/samples},{"highMarks",double(high)/(samples*5)},{"samples",samples}});
    }
    const double horizon=p["analysis.duration"].toDouble(3600),slice=p["analysis.time"].toDouble(0),location=p["analysis.distance"].toDouble(hill.k*.65);
    double headSum=0,crossSum=0,headSquares=0,lo=1e99,hi=-1e99;int reversals=0;double lastHead=0,temporalVariation=0,spatialVariation=0;
    const int timeSamples=p["analysis.timeSamples"].toInt(241),spaceSamples=p["analysis.spaceSamples"].toInt(49);
    for(int i=0;i<timeSamples;++i){if(canceled())return {};double t=horizon*i/(timeSamples-1);auto w=weather.wind(location,t);timeline.append(QJsonObject{{"time",t},{"head",w.head},{"cross",w.cross},{"strength",w.strength()},{"direction",std::atan2(w.cross,w.head)*180/3.14159265358979323846},{"precipitation",weather.precipitation(t)}});
        if(i){if(lastHead*w.head<0)++reversals;temporalVariation+=std::abs(lastHead-w.head);}lastHead=w.head;
        for(int j=0;j<spaceSamples;++j){double d=hill.k*1.2*j/(spaceSamples-1);auto v=weather.wind(d,t);grid.append(QJsonObject{{"time",t},{"distance",d},{"head",v.head},{"cross",v.cross},{"strength",v.strength()}});headSum+=v.head;crossSum+=v.cross;headSquares+=v.head*v.head;lo=std::min(lo,v.head);hi=std::max(hi,v.head);if(j){auto previous=weather.wind(d-hill.k*1.2/(spaceSamples-1),t);spatialVariation+=std::abs(v.head-previous.head);}}
    }
    for(int j=0;j<=120;++j){double d=hill.k*1.2*j/120;auto w=weather.wind(d,slice);spatial.append(QJsonObject{{"distance",d},{"head",w.head},{"cross",w.cross},{"strength",w.strength()},{"direction",std::atan2(w.cross,w.head)*180/3.14159265358979323846}});}
    const double count=timeSamples*spaceSamples,mean=headSum/count;
    QJsonArray jumps,weatherLanding;QMap<int,QJsonObject> landingBins;
    SimulationV2::JumpRequest request;request.hill=hill;request.athlete=athlete;request.event=SimulationV2::EventKind(p["event"].toInt());request.scoring.juryGate=request.scoring.referenceGate=p["gate"].toInt()>0?p["gate"].toInt():ep.flight.referenceGate;request.captureExposure=false;request.scoring.windCompensation=p["rules.windCompensation"].toInt()!=0;request.scoring.gateCompensation=p["rules.gateCompensation"].toInt()!=0;request.scoring.judges=p["rules.judges"].toInt()!=0;
    CompetitionRules rules;rules.setWindMeasurementType(p["rules.measurement"].toInt());SimulationV2::FieldWindMeasurement measurement(hill.k,rules,measurementParameters(p));
    class NoWind final:public SimulationV2::WeatherField {const SimulationV2::WeatherField &base;public:explicit NoWind(const SimulationV2::WeatherField &base):base(base){}SimulationV2::Wind wind(double,double)const override{return {};}double precipitation(double t)const override{return base.precipitation(t);}SimulationV2::PrecipitationKind precipitationKind()const override{return base.precipitationKind();}} calm(weather);
    const int jumpSamples=p["analysis.jumpSamples"].toInt(512);
    for(int n=0;n<jumpSamples;++n){if(canceled())return {};request.startSeconds=horizon*n/std::max(1,jumpSamples-1);auto jumpSeed=SimulationV2::deriveSeed(seed,n);SimulationV2::SeededRandom rng(jumpSeed),same(jumpSeed);auto r=engine.simulate(request,rng,weather,measurement);auto neutral=engine.simulate(request,same,calm,measurement);
        jumps.append(QJsonObject{{"time",request.startSeconds},{"distance",r.measuredDistance},{"noWindDistance",neutral.measuredDistance},{"windMetres",r.measuredDistance-neutral.measuredDistance},{"measured",r.score.measuredWind},{"experienced",r.flight.experiencedWind.head},{"crosswind",r.flight.meanAbsoluteCrosswind},{"error",r.score.measuredWind-r.flight.experiencedWind.head},{"compensation",r.score.wind},{"points",r.score.total},{"landing",int(r.landing)}});
        int bin=int(std::floor(r.measuredDistance/2));auto b=landingBins.value(bin,QJsonObject{{"distance",bin*2+1},{"count",0},{"fall",0},{"markSum",0}});b["count"]=b["count"].toInt()+1;b["fall"]=b["fall"].toInt()+(r.landing==SimulationV2::LandingKind::Fall?1:0);double mark=0;for(double v:r.judges)mark+=v/5;b["markSum"]=b["markSum"].toDouble()+mark;landingBins[bin]=b;
    }
    for(auto b:landingBins){b["fallPercent"]=100.*b["fall"].toDouble()/b["count"].toDouble();b["meanMark"]=b["markSum"].toDouble()/b["count"].toDouble();weatherLanding.append(b);}
    const double scale=p["analysis.scale"].toDouble(3);QImage heatmap(timeSamples,spaceSamples,QImage::Format_RGB32);
    for(const auto &entry:grid){auto v=entry.toObject();int x=int(std::round(v["time"].toDouble()/horizon*(timeSamples-1))),y=int(std::round(v["distance"].toDouble()/(hill.k*1.2)*(spaceSamples-1)));double a=std::min(1.,std::abs(v["head"].toDouble())/scale);QColor target=v["head"].toDouble()>=0?QColor(35,95,215):QColor(200,85,20);heatmap.setPixelColor(x,y,QColor(int(240+(target.red()-240)*a),int(240+(target.green()-240)*a),int(240+(target.blue()-240)*a)));}
    QByteArray png;QBuffer buffer(&png);buffer.open(QIODevice::WriteOnly);heatmap.save(&buffer,"PNG");
    return {{"jumpSamples",jumps},{"weatherLanding",weatherLanding},{"heatmap",QString("data:image/png;base64,")+QString::fromLatin1(png.toBase64())},{"heatmapScale",scale},{"variant",variant},{"parameters",p},{"landing",curves},{"timeline",timeline},{"spatial",spatial},{"grid",grid},{"duration",horizon},{"time",slice},{"distance",location},{"realHs",hill.realHs},{"landingLimit",engine.flight().landingLimit(hill)},
        {"windSummary",QJsonObject{{"meanHead",mean},{"meanCross",crossSum/count},{"sdHead",std::sqrt(std::max(0.,headSquares/count-mean*mean))},{"minHead",lo},{"maxHead",hi},{"reversals",reversals},{"meanTimeChange",temporalVariation/(timeSamples-1)},{"meanSpatialChange",spatialVariation/(timeSamples*(spaceSamples-1))},{"gridSamples",int(count)}}}};
}
}
