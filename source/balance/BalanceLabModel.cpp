#include "BalanceLabModel.h"
#include <QObject>
#include <QSet>
#include <QJsonDocument>
#include <QMap>
#include <cmath>
#include <algorithm>

namespace BalanceLab {
QJsonArray questions() {
    const auto q=[](const char *id,const QString &name,const QString &prompt,const QString &groups,const char *axis,double lo,double hi,const char *view){return QJsonObject{{"id",id},{"name",name},{"prompt",prompt},{"groups",groups.split(',').join(',')},{"axis",axis},{"from",lo},{"to",hi},{"view",view}};};
    return {
        q("ability",QObject::tr("OVR i forma"),QObject::tr("Ile daje zmiana OVR lub formy na tej stawce?"),"athlete,field,quality,hill","athlete.overall",40,100,"curve"),
        q("quality",QObject::tr("Rozrzut jakości"),QObject::tr("Jak często skoczek zawodzi lub oddaje wyjątkowy skok?"),"quality,athlete,rules","quality.eventDeviation.3",0,10,"curve"),
        q("generator",QObject::tr("Losowanie pogody"),QObject::tr("Jakie warunki rzeczywiście losują presety i automatyczna propozycja?"),"generator,hill","generator.auto.strengthScale",.3,1.5,"generator"),
        q("field",QObject::tr("Pole wiatru"),QObject::tr("Jak wiatr zmienia się podczas konkursu i wzdłuż zeskoku?"),"wind,analysis,hill","wind.temporalAmplitude",0,3,"wind"),
        q("wind",QObject::tr("Wykorzystanie wiatru"),QObject::tr("Ile metrów daje wiatr i jak często potrafi zepsuć skok?"),"wind,flight,athlete,quality,analysis","wind.mean",-5,6,"wind"),
        q("gate",QObject::tr("Belka"),QObject::tr("Która belka daje najwięcej punktów i jakie niesie ryzyko?"),"flight,wind,hill,rules","gate",6,14,"curve"),
        q("scoring",QObject::tr("Pomiar i punktacja"),QObject::tr("Co zostaje z przewagi wiatru po rekompensacie?"),"measurement,rules,wind,hill,flight","wind.mean",-4,4,"curve"),
        q("geometry",QObject::tr("Geometria skoczni"),QObject::tr("Jak wypłaszczenie i charakter skoczni ograniczają odległość?"),"hill,flight,quality","hill.realHs",120,155,"curve"),
        q("landing",QObject::tr("Lądowanie"),QObject::tr("Gdzie rośnie ryzyko podpórki i upadku — i co dzieje się w pełnych skokach?"),"landing,hill,flight,wind,athlete,analysis","hill.realHs",120,155,"landing"),
        q("marks",QObject::tr("Noty"),QObject::tr("Jak klasa lądowania i kary zmieniają noty oraz punkty?"),"landing,athlete,hill,wind","athlete.landing",0,4,"landing"),
        q("perks",QObject::tr("Perki"),QObject::tr("Co daje każdy poziom perka na różnych skoczniach i w różnych seriach?"),"athlete,field,quality,hill,rules","athlete.perk.1",-3,3,"curve"),
        q("precipitation",QObject::tr("Opady"),QObject::tr("Ile kosztują śnieg i deszcz przy różnym natężeniu?"),"precipitation,flight,athlete","precipitation.intensity",0,10,"curve"),
        q("contest",QObject::tr("Pełny konkurs"),QObject::tr("Jak zmieniają się zwycięzcy, miejsca i awanse w mojej paczce?"),"athlete,field,rules,wind,hill,quality","athlete.form",30,100,"contest"),
        q("rules",QObject::tr("Dyskwalifikacje i zasady"),QObject::tr("Jak DSQ, awans i odstępy między skokami zmieniają wyniki?"),"rules,clock","dsq",0,.15,"curve"),
        q("targets",QObject::tr("Cele balansu"),QObject::tr("Które oczekiwania spełnia obecny profil i co pogorszyła zmiana?"),"quality,flight,landing,wind","athlete.overall",40,100,"targets")
    };
}
QJsonObject engineValues(const QJsonObject &values) {
    QJsonObject out;
    for(auto i=values.begin();i!=values.end();++i)
        if(i.key().startsWith("quality.") || i.key().startsWith("flight.") || i.key().startsWith("landing.") || i.key().startsWith("curve.") || i.key().startsWith("measurement.") || i.key().startsWith("generator."))out[i.key()]=i.value();
    return out;
}
SimulationV2::WeatherGeneratorParameters generatorValues(const QJsonObject &values) {
    SimulationV2::WeatherGeneratorParameters out;
    for(auto i=values.begin();i!=values.end();++i)if(i.key().startsWith("generator."))out.values[i.key().toStdString()]=i.value().toDouble();
    return out;
}
bool validateProfile(const QJsonObject &values,QString &error) {
    QHash<QString,QJsonObject> defs;
    for(auto row:Playground::parameters()){auto d=row.toObject();defs[d["key"].toString()]=d;}
    for(auto it=values.begin();it!=values.end();++it){const auto d=defs.value(it.key());const double v=it.value().toDouble();if(d.isEmpty() || !engineValues(QJsonObject{{it.key(),it.value()}}).contains(it.key()) || !it.value().isDouble() || !std::isfinite(v) || v<d["minimum"].toDouble() || v>d["maximum"].toDouble() || (d["type"]=="integer" && v!=std::floor(v))){error=QObject::tr("Nieprawidłowy współczynnik profilu: %1").arg(it.key());return false;}}
    try {Playground::engineParameters(values);Playground::measurementParameters(values);Playground::validateGenerator(values);const auto g=generatorValues(values);for(int i=0;i<SimulationV2::weatherPresetCount;++i){SimulationV2::SeededRandom rng(123);SimulationV2::sampleWeatherPreset(SimulationV2::WeatherPreset(i),120,rng,g);}for(int level:{-3,0,3}){SimulationV2::SeededRandom rng(123);SimulationV2::WeatherProposalContext context;context.variabilityPerk=level;SimulationV2::proposeWeather(context,rng,g);}}
    catch(const std::exception &e){error=QString::fromUtf8(e.what());return false;}return true;
}
QJsonObject initialConfig() {
    auto c=Playground::example();c["name"]=QObject::tr("Mój eksperyment balansu");c["competitions"]=500;c["targets"]=QJsonArray();
    c["variants"]=QJsonArray{QJsonObject{{"name",QObject::tr("Balans z gry")},{"changes",QJsonObject()}},QJsonObject{{"name",QObject::tr("Zapamiętane ustawienia")},{"changes",QJsonObject()}},QJsonObject{{"name",QObject::tr("Twoje zmiany")},{"changes",QJsonObject()}}};
    c["labTargets"]=QJsonArray();return c;
}
QJsonObject profileDiff(const QJsonObject &a,const QJsonObject &b) {
    QJsonArray changes;QSet<QString> keys;for(auto i=a.begin();i!=a.end();++i)keys.insert(i.key());for(auto i=b.begin();i!=b.end();++i)keys.insert(i.key());QStringList sorted=keys.values();sorted.sort();
    for(const auto &k:sorted)if(a[k]!=b[k])changes.append(QJsonObject{{"key",k},{"before",a[k]},{"after",b[k]}});
    return {{"changes",changes},{"count",changes.size()}};
}
namespace {
bool canceled(const std::shared_ptr<Playground::Control> &c){return c && c->canceled.load();}
void overrideValue(QJsonObject &config,int variant,const QString &key,double value) {
    auto variants=config["variants"].toArray();auto v=variants[variant].toObject(),changes=v["changes"].toObject();changes[key]=value;
    if(key=="hill.k"){auto p=Playground::valuesFor(config,variant);changes["hill.hs"]=value*p["hill.hs"].toDouble()/p["hill.k"].toDouble();changes["hill.realHs"]=value*p["hill.realHs"].toDouble()/p["hill.k"].toDouble();}
    if(key=="hill.perk.0")changes.remove("hill.takeoffBias");if(key=="hill.perk.1")changes.remove("hill.realHs");
    if(key.startsWith("athlete.")){QString field=key.mid(8);field.remove('.');auto edits=v["athleteEdits"].toArray();for(int i=0;i<edits.size();++i){auto patch=edits[i].toObject();if(patch["index"].toInt()==config["focus"].toInt()){patch.remove(field);edits[i]=patch;}}v["athleteEdits"]=edits;}
    v["changes"]=changes;variants[variant]=v;config["variants"]=variants;
}
QJsonObject summary(const QJsonObject &variant,int focus) {
    auto athlete=variant["athletes"].toArray().at(focus).toObject();const double starts=athlete["starts"].toDouble(),jumps=qMax(0.,athlete["jumps"].toDouble()-athlete["dsq"].toDouble());
    auto place=athlete["place"].toObject();for(auto q:QVector<QPair<QString,double>>{{"p10",.1},{"p90",.9}}){double sum=0;for(auto bin:athlete["places"].toArray()){auto b=bin.toObject();if(b["place"].toDouble()>place["max"].toDouble())continue;sum+=b["count"].toDouble();if(sum>=place["count"].toDouble()*q.second){place[q.first]=b["place"];break;}}}athlete["place"]=place;
    return {{"distance",athlete["distance"]},{"points",athlete["points"]},{"place",athlete["place"]},{"total",athlete["total"]},{"wins",starts?100*athlete["wins"].toDouble()/starts:0},{"podiums",starts?100*athlete["podiums"].toDouble()/starts:0},{"falls",jumps?100*athlete["falls"].toDouble()/jumps:0},{"starts",starts},{"jumps",jumps},{"dsq",athlete["dsq"]}};
}
}
static QJsonArray targetResults(const QJsonObject &config,const QJsonObject &result,const std::shared_ptr<Playground::Control> &control) {
    QJsonArray out;QMap<QByteArray,QJsonObject> reports;
    int targetIndex=0;
    for(auto entry:config["labTargets"].toArray()){
        if(control)control->labProgressDone.store(targetIndex++);
        if(canceled(control))return {};
        auto t=entry.toObject();const int variant=t["variant"].toInt(2);QJsonObject metric;QString state="inconclusive";const QString kind=t["metric"].toString();
        auto scene=config;auto report=result["report"].toObject();
        if(!t["scenarioId"].toString().isEmpty()){
            scene=config["labScenarios"].toObject()[t["scenarioId"].toString()].toObject();if(scene.isEmpty()){t["state"]="inconclusive";out.append(t);continue;}
            auto parameters=scene["parameters"].toObject();for(auto i=parameters.begin();i!=parameters.end();)if(!engineValues(QJsonObject{{i.key(),i.value()}}).isEmpty())i=parameters.erase(i);else ++i;scene["parameters"]=parameters;
            auto variants=scene["variants"].toArray();for(int v=0;v<variants.size();++v){auto row=variants[v].toObject(),changes=row["changes"].toObject();for(auto i=changes.begin();i!=changes.end();)if(!engineValues(QJsonObject{{i.key(),i.value()}}).isEmpty())i=changes.erase(i);else ++i;const auto engine=engineValues(Playground::valuesFor(config,v));for(auto i=engine.begin();i!=engine.end();++i)changes[i.key()]=i.value();row["changes"]=changes;variants[v]=row;}scene["variants"]=variants;scene["labTargets"]=QJsonArray();scene.remove("labScenarios");scene["competitions"]=result["full"].toBool()?config["competitions"].toInt():7;scene["sampleLimit"]=0;
            if(kind!="fallProbability"){const auto key=QJsonDocument(scene).toJson(QJsonDocument::Compact);if(!reports.contains(key))reports[key]=Playground::run(scene,control);report=reports.value(key);}
        }
        auto variants=report["variants"].toArray();
        if(variant>=0 && variant<variants.size()){
            if(kind=="fallProbability"){
                auto p=Playground::valuesFor(scene,variant);auto hill=Playground::hillInput(p);SimulationV2::LandingModel landing(Playground::engineParameters(p).landing);double value=100*landing.probabilities(t["distance"].toDouble(hill.hs),hill.realHs,std::abs(p["wind.cross"].toDouble())).values[3];metric={{"mean",value},{"ciLow",value},{"ciHigh",value},{"exact",true}};
            }else if(kind=="meanDistance" || kind=="meanPlace" || kind=="meanPoints"){
                auto focus=variants[variant].toObject()["focus"].toObject();metric=focus[kind=="meanDistance"?"distance":kind=="meanPlace"?"place":"points"].toObject();
            }else metric=variants[variant].toObject()[kind].toObject();
            const bool enough=metric["exact"].toBool() || metric.value("clusters").toDouble(metric["count"].toDouble())>=30;
            if(enough && metric["ciLow"].isDouble() && metric["ciHigh"].isDouble()) {const double lo=metric["ciLow"].toDouble(),hi=metric["ciHigh"].toDouble();if(lo>=t["minimum"].toDouble() && hi<=t["maximum"].toDouble())state="pass";else if(hi<t["minimum"].toDouble() || lo>t["maximum"].toDouble())state="fail";}
        }
        t["state"]=state;t["result"]=metric;out.append(t);
    }return out;
}
QJsonArray evaluateTargets(const QJsonObject &config,const QJsonObject &result){return targetResults(config,result,{});}
QJsonObject evaluate(QJsonObject config,QJsonObject options,bool full,const std::shared_ptr<Playground::Control> &control) {
    QString error;if(!Playground::validate(config,error))throw std::invalid_argument(error.toStdString());
    const auto original=config;const int focus=config["focus"].toInt();int perContest=0;for(auto r:config["rounds"].toArray())perContest+=r.toInt();
    if(!full){config["competitions"]=7;config["sampleLimit"]=1;config["threads"]=2;}
    const QString view=options["view"].toString("curve");
    const bool matrix=options["matrix"].toBool();
    if(control){std::lock_guard<std::mutex> lock(control->labProgressMutex);control->labProgress={};control->labProgress.phases=2+int(view=="landing" || view=="wind")+int(view=="curve" || matrix)+int(view=="generator")+int(!config["labTargets"].toArray().isEmpty());}
    if(control)control->beginLabPhase(1,config["competitions"].toInt(),true);
    auto report=Playground::run(config,control);if(canceled(control))return {{"canceled",true}};
    QJsonObject out{{"report",report},{"full",full},{"config",original},{"options",options},{"engineBuild",Playground::engineBuild()}};
    QJsonArray summaries;for(auto v:report["variants"].toArray())summaries.append(summary(v.toObject(),focus));out["summaries"]=summaries;
    if(view=="landing" || view=="wind") {
        if(control)control->beginLabPhase(2,3);
        QJsonArray analyses;for(int i=0;i<3;++i){if(canceled(control))return {{"canceled",true}};auto c=config;if(!full){auto variants=c["variants"].toArray();for(int v=0;v<variants.size();++v){auto row=variants[v].toObject(),changes=row["changes"].toObject();auto values=Playground::valuesFor(c,v);changes["analysis.jumpSamples"]=qMin(24,values["analysis.jumpSamples"].toInt());changes["analysis.timeSamples"]=qMin(41,values["analysis.timeSamples"].toInt());changes["analysis.spaceSamples"]=qMin(17,values["analysis.spaceSamples"].toInt());row["changes"]=changes;variants[v]=row;}c["variants"]=variants;}c["diagnosticsMode"]=view;if(!full)c["diagnosticsPreview"]=true;
            int same=-1;auto signature=c["variants"].toArray()[i].toObject();signature.remove("name");for(int previous=0;previous<i;++previous){auto other=c["variants"].toArray()[previous].toObject();other.remove("name");if(signature==other){same=previous;break;}}
            auto diagnostic=same>=0?analyses[same].toObject():Playground::diagnostics(c,i,control);diagnostic["variant"]=i;analyses.append(diagnostic);if(control)control->labProgressDone.fetch_add(1);}out["analyses"]=analyses;
    }
    if(view=="curve" || matrix) {
        const QString x=options["axisX"].toString("athlete.overall"),y=options["axisY"].toString("athlete.form");const int steps=!full?3:matrix?qBound(3,options["resolution"].toInt(5),15):qBound(3,options["resolution"].toInt(7),21);const int cells=matrix?steps*steps:steps;
        QJsonArray grid,series;auto base=config;base["targets"]=QJsonArray();base["sampleLimit"]=0;base["competitions"]=full?qMin(config["competitions"].toInt(),qMax(2,200000/qMax(1,cells*perContest*3))):7;
        QHash<QString,bool> integers;for(auto d:Playground::parameters()){auto def=d.toObject();integers[def["key"].toString()]=def["type"]=="integer";}
        const int xSteps=integers.value(x)?qMin(steps,qMax(1,int(std::abs(options["toX"].toDouble(100)-options["fromX"].toDouble(40)))+1)):steps;
        const int ySteps=!matrix?1:integers.value(y)?qMin(steps,qMax(1,int(std::abs(options["toY"].toDouble(100)-options["fromY"].toDouble(20)))+1)):steps;
        if(control)control->beginLabPhase(3,xSteps*ySteps*base["competitions"].toInt(),true);
        out["columns"]=xSteps;out["rows"]=ySteps;
        for(int iy=0;iy<ySteps;++iy)for(int ix=0;ix<xSteps;++ix){if(canceled(control))return {{"canceled",true}};
            double xv=options["fromX"].toDouble(40)+(options["toX"].toDouble(100)-options["fromX"].toDouble(40))*(xSteps>1?double(ix)/(xSteps-1):0);double yv=options["fromY"].toDouble(20)+(options["toY"].toDouble(100)-options["fromY"].toDouble(20))*(ySteps>1?double(iy)/(ySteps-1):0);if(integers.value(x))xv=std::round(xv);if(integers.value(y))yv=std::round(yv);
            auto c=base;for(int v=0;v<3;++v){overrideValue(c,v,x,xv);if(matrix)overrideValue(c,v,y,yv);}QString invalid;
            if(!Playground::validate(c,invalid)){grid.append(QJsonObject{{"x",xv},{"y",yv},{"error",invalid}});if(control)control->labSkippedRuns.fetch_add(base["competitions"].toInt());continue;}
            auto r=Playground::run(c,control);QJsonArray values;for(auto v:r["variants"].toArray())values.append(summary(v.toObject(),focus));grid.append(QJsonObject{{"x",xv},{"y",yv},{"values",values},{"count",base["competitions"]}});
        }
        out["grid"]=grid;out["gridSamplesPerCell"]=base["competitions"];out["matrix"]=matrix;
    }
    if(view=="generator") {
        if(control)control->beginLabPhase(4,3*(SimulationV2::weatherPresetCount+1)*(full?2000:128));
        QJsonArray generators;for(int v=0;v<3;++v){auto p=Playground::valuesFor(config,v);auto tuning=generatorValues(p);SimulationV2::WeatherProposalContext context;context.k=p["hill.k"].toDouble();context.country=config["hill"].toObject()["source"].toObject()["country-code"].toString().toStdString();context.variabilityPerk=p["hill.perk.2"].toInt();context.directionPerk=p["hill.perk.3"].toInt();context.month=p["weather.month"].toInt();context.surface=p["weather.surface"].toInt()==1?SimulationV2::Surface::Mat:SimulationV2::Surface::Snow;QJsonArray scenarios;const int count=full?2000:128;for(int preset=-1;preset<SimulationV2::weatherPresetCount;++preset){double sum=0,sq=0,head=0,tail=0,cross=0,lo=1e9,hi=-1e9;QJsonArray samples;for(int n=0;n<count;++n){if(canceled(control))return {{"canceled",true}};SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(config["seed"].toString().toULongLong(),n));auto proposal=preset<0?SimulationV2::proposeWeather(context,rng,tuning):SimulationV2::sampleWeatherPreset(SimulationV2::WeatherPreset(preset),p["hill.k"].toDouble(),rng,tuning);double strength=0;for(auto r:proposal.wind.references){strength+=r.value.strength()/proposal.wind.references.size();head+=double(r.value.head>.1)/(count*proposal.wind.references.size());tail+=double(r.value.head<-.1)/(count*proposal.wind.references.size());cross+=std::abs(r.value.cross)/(count*proposal.wind.references.size());}sum+=strength;sq+=strength*strength;lo=qMin(lo,strength);hi=qMax(hi,strength);samples.append(strength);if(control)control->labProgressDone.fetch_add(1);}scenarios.append(QJsonObject{{"preset",preset},{"mean",sum/count},{"sd",std::sqrt(qMax(0.,sq/count-std::pow(sum/count,2)))},{"min",lo},{"max",hi},{"headShare",head},{"tailShare",tail},{"meanCross",cross},{"count",count},{"samples",samples}});}generators.append(scenarios);}out["generators"]=generators;
    }
    if(control && !original["labTargets"].toArray().isEmpty())control->beginLabPhase(5,original["labTargets"].toArray().size());
    out["targets"]=targetResults(original,out,control);if(canceled(control))return {{"canceled",true}};if(control)control->beginLabPhase(6,1);return out;
}
}
