#include "PlaygroundModel.h"
#include "../simulation-v2/ProfileBridge.h"
#include "../simulator/Jumper.h"
#include "../simulator/Hill.h"
#include <QObject>
#include <QSet>
#include <QThread>
#include <cmath>
#include <stdexcept>

namespace Playground {
QJsonObject example() {
    QJsonObject defaults;for(const auto &entry:parameters()){auto d=entry.toObject();defaults[d["key"].toString()]=d["default"];}
    return {{"schema",2},{"name",QObject::tr("Strojenie silnika v2")},{"seed","20261028"},
        {"competitions",1000},{"threads",qMax(1,qMin(16,QThread::idealThreadCount()-1))},{"athletes",50},{"focus",24},{"sampleLimit",64},
        {"hill",QJsonObject{{"name","K120 / HS134"},{"k",120},{"hs",134}}},{"rounds",QJsonArray{50}},
        {"defaults",defaults},{"parameters",QJsonObject{{"gate",10},{"clock.mode",0},{"wind.temporalAmplitude",0},{"wind.spatialAmplitude",0},{"wind.gustAmplitude",0}}},{"variants",QJsonArray{QJsonObject{{"name",QObject::tr("Bazowy")},{"changes",QJsonObject()}},QJsonObject{{"name",QObject::tr("Porównanie")},{"changes",QJsonObject{{"athlete.overall",90}}}}}},
        {"targets",QJsonArray{QJsonObject{{"name","OVR 90–70"},{"metric","pairedDistance"},{"variant",1},{"minimum",14},{"maximum",17}}}}};
}
QJsonObject valuesFor(const QJsonObject &config,int variant) {
    static const QJsonObject baseline=[] {QJsonObject result;for(const auto &row:parameters()){auto d=row.toObject();result[d["key"].toString()]=d["default"];}return result;}();
    QJsonObject p=baseline;
    const auto defaults=config["defaults"].toObject();for(auto it=defaults.begin();it!=defaults.end();++it)p[it.key()]=it.value();
    const auto roster=config["roster"].toArray();
    if(!roster.isEmpty()) {
        auto source=Jumper::getFromJson(roster[config["focus"].toInt()].toObject()).getSimulationProfile();
        if(!source)throw std::invalid_argument("Playground v2 requires canonical athlete profiles");
        p["athlete.overall"]=source->overall;p["athlete.form"]=source->form;p["athlete.landing"]=source->landing;
        for(int i=0;i<3;++i)p[QString("athlete.perk.%1").arg(i)]=source->perks[i];
    }
    auto h=config["hill"].toObject();p["hill.k"]=h["k"];p["hill.hs"]=h["hs"];p["hill.realHs"]=h["hs"];
    if(h["source"].isObject()) {
        auto source=Hill::getFromJson(h["source"].toObject()).getSimulationProfile();
        if(!source)throw std::invalid_argument("Playground v2 requires canonical hill profiles");
        auto input=SimulationV2::hillInput(*source);
        p["hill.k"]=input.k;p["hill.hs"]=input.hs;p["hill.realHs"]=input.realHs;p["hill.takeoffBias"]=input.takeoffBias;
        p["hill.pointsAtK"]=input.pointsAtK;p["hill.pointsPerMetre"]=input.pointsPerMetre;p["hill.gatePoints"]=input.gatePoints;p["hill.headwindPoints"]=input.headwindPoints;p["hill.tailwindPoints"]=input.tailwindPoints;
    }
    if(config["rulesData"].isObject()) {
        auto rules=CompetitionRules::getFromJson(config["rulesData"].toObject());
        p["rules.dsq"]=int(rules.getHasDsq());p["rules.measurement"]=rules.getWindMeasurementType();p["rules.windCompensation"]=int(rules.getHasWindCompensations());p["rules.gateCompensation"]=int(rules.getHasGateCompensations());p["rules.judges"]=int(rules.getHasJudgesPoints());p["rules.95hs"]=int(rules.getHas95HSRule());
    }
    for(const auto &layer:{config["parameters"].toObject(),config["variants"].toArray()[variant].toObject()["changes"].toObject()})
        for(auto it=layer.begin();it!=layer.end();++it)p[it.key()]=it.value();
    return p;
}
bool validate(const QJsonObject &config,QString &error) {
    const auto fail=[&](const QString &message){error=message;return false;};
    if(config["schema"].toInt()!=2)return fail(QObject::tr("Wymagana konfiguracja playgroundu v2. Stare eksperymenty nie opisują nowego silnika."));
    const auto integer=[&](const char *key,int lo,int hi){auto v=config[key];return v.isDouble() && std::isfinite(v.toDouble()) && std::floor(v.toDouble())==v.toDouble() && v.toDouble()>=lo && v.toDouble()<=hi;};
    if(!integer("competitions",1,1000000) || !integer("athletes",2,512) || !integer("focus",0,config["athletes"].toInt()-1) || !integer("threads",1,64) || !integer("sampleLimit",0,4096))return fail(QObject::tr("Nieprawidłowa liczba prób, skoczków, wątków lub indeks skoczka."));
    bool seedOk=false;config["seed"].toString().toULongLong(&seedOk);if(!seedOk)return fail(QObject::tr("Ziarno musi być nieujemną liczbą całkowitą zapisaną jako tekst."));
    auto variants=config["variants"].toArray();if(variants.isEmpty() || variants.size()>32)return fail(QObject::tr("Wybierz od 1 do 32 wariantów."));
    QHash<QString,QJsonObject> defs;for(const auto &row:parameters()){auto d=row.toObject();defs[d["key"].toString()]=d;}
    auto checkValues=[&](const QJsonObject &values){for(auto it=values.begin();it!=values.end();++it){auto d=defs.value(it.key());double v=it.value().toDouble();if(d.isEmpty() || !it.value().isDouble() || !std::isfinite(v) || v<d["minimum"].toDouble() || v>d["maximum"].toDouble() || (d["type"]=="integer" && v!=std::floor(v))){error=QObject::tr("Nieprawidłowy parametr: %1").arg(it.key());return false;}}return true;};
    if(!config["defaults"].isObject() || !checkValues(config["defaults"].toObject()))return fail(QObject::tr("Nieprawidłowy zestaw wartości domyślnych."));
    if(!config["parameters"].isObject() || !checkValues(config["parameters"].toObject()))return false;
    auto rounds=config["rounds"].toArray();if(rounds.isEmpty() || rounds.size()>8)return fail(QObject::tr("Wybierz od 1 do 8 serii."));
    int previous=config["athletes"].toInt();for(const auto &r:rounds){if(!r.isDouble() || r.toDouble()!=std::floor(r.toDouble()) || r.toInt()<1 || r.toInt()>previous)return fail(QObject::tr("Limity serii muszą być dodatnie i nie mogą rosnąć."));previous=r.toInt();}
    if(config.contains("roster")){if(!config["roster"].isArray() || config["roster"].toArray().size()!=config["athletes"].toInt())return fail(QObject::tr("Nieprawidłowa stawka."));for(const auto &row:config["roster"].toArray()){QString e;if(!row.isObject() || !Jumper::validateSimulationProfileJson(row.toObject(),e) || !Jumper::getFromJson(row.toObject()).getSimulationProfile())return fail(QObject::tr("Stawka wymaga pełnych profili v2: %1").arg(e));}}
    if(config.contains("rulesData")) {
        if(!config["rulesData"].isObject() || !CompetitionRules::validateWindMeasurementJson(config["rulesData"].toObject()))return fail(QObject::tr("Nieprawidłowe zasady."));
        auto rules=CompetitionRules::getFromJson(config["rulesData"].toObject());
        if(rules.getCompetitionType()!=CompetitionRules::Individual || rules.getRounds().isEmpty() || rules.getRounds().size()>8)return fail(QObject::tr("Playground v2 obsługuje obecnie konkursy indywidualne."));
        for(const auto &r:rules.getRounds())if(r.getKO() || r.getCount()<1 || r.getCount()>config["athletes"].toInt())return fail(QObject::tr("Na tym etapie użyj zwykłych serii indywidualnych bez KO."));
    }
    QSet<QString> names;
    try {
        for(int i=0;i<variants.size();++i){auto v=variants[i].toObject();auto name=v["name"].toString().trimmed();if(name.isEmpty() || names.contains(name) || !v["changes"].isObject() || !checkValues(v["changes"].toObject()))return fail(error.isEmpty()?QObject::tr("Warianty wymagają różnych nazw i poprawnych parametrów."):error);names.insert(name);
            auto p=valuesFor(config,i);if(p["analysis.timeSamples"].toDouble()*p["analysis.spaceSamples"].toDouble()>200000)throw std::invalid_argument("Analysis grid limit is 200000 samples");engineParameters(p);windParameters(p);precipitationParameters(p);measurementParameters(p);hillInput(p);athleteInput(p,"athlete");
        }
    } catch(const std::exception &e){return fail(QObject::tr("Niespójne parametry: %1").arg(QString::fromUtf8(e.what())));}
    if(config["targets"].toArray().size()>64)return fail(QObject::tr("Maksymalnie 64 cele kalibracji."));
    for(const auto &row:config["targets"].toArray()){auto t=row.toObject();if(!row.isObject() || !t["variant"].isDouble() || t["variant"].toDouble()!=t["variant"].toInt() || t["variant"].toInt()<1 || t["variant"].toInt()>=variants.size() || !QStringList{"pairedDistance","pairedPoints","pairedPlace"}.contains(t["metric"].toString()) || !t["minimum"].isDouble() || !t["maximum"].isDouble() || t["minimum"].toDouble()>t["maximum"].toDouble())return fail(QObject::tr("Nieprawidłowy cel kalibracji."));}
    // Keep retained JSON samples bounded independently of worker memory.
    if(qint64(config["sampleLimit"].toInt())*variants.size()*rounds.size()>65536)return fail(QObject::tr("Zmniejsz liczbę zachowanych przykładów lub wariantów."));
    error.clear();return true;
}
}
