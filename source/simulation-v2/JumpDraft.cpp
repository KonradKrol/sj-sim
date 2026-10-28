#include "JumpDraft.h"
#include "../simulator/wind-generation/WindsGenerator.h"
#include <QVariantList>
#include <cmath>
#include <stdexcept>

SimulationJumpDraft::SimulationJumpDraft(QObject *parent):QObject(parent),weather(new SimulationWeatherSetup(this))
{
    connect(weather,&SimulationWeatherSetup::changed,this,[this]{if(loaded){validate();emit changed();}});
}

void SimulationJumpDraft::load(const JumpManipulator &source,const HillProfile &venue,const QVector<Wind> &winds,bool allowDsq)
{
    loaded=false;hill=venue;baselineWinds=winds;dsqAllowed=allowDsq;
    const auto input=source.simulationInput(allowDsq);
    fields={{"revision",++revision},{"dsqAllowed",allowDsq},{"windActive",source.getExactSimulationWind()!=nullptr || !source.getExactWinds().isEmpty()},
        {"landing",input.landing?int(*input.landing):-1},{"dsq",input.dsqProbability?*input.dsqProbability*100:-1}};
    const auto add=[this](const QString &key,const SimulationV2::ValueAdjustment &value,double minimum,double maximum) {
        fields[key+"Active"]=value.active();fields[key+"Bonus"]=value.bonus;fields[key+"Bounded"]=bool(value.range);
        fields[key+"Min"]=value.range?value.range->minimum:minimum;fields[key+"Max"]=value.range?value.range->maximum:maximum;
    };
    add("quality",input.quality,0,120);add("distance",input.distance,0,hill.realHs());
    add("precipitation",input.precipitation,0,10);add("measuredWind",input.measuredWind,-5,5);
    QVariantList judges;for(const auto &value:input.judges)judges.append(value?*value:-1);fields["judges"]=judges;
    weather->setHill(hill);weather->setValue("variability",0);weather->setValue("differences",0);
    const auto initial=source.getExactWinds().isEmpty()?winds:source.getExactWinds();
    SimulationV2::WindParameters parameters;parameters.references.clear();
    parameters.temporalAmplitude=parameters.spatialAmplitude=parameters.gustAmplitude=0;
    for(int i=0;i<initial.size();++i) {
        const auto &wind=initial[i];const double angle=wind.getDirection()*3.14159265358979323846/180;
        parameters.references.push_back({(i+.5)*WindsGenerator::getWindSegmentDistance(hill.k,initial.size()),{wind.getStrength()*std::cos(angle),wind.getStrength()*std::sin(angle)}});
    }
    if(parameters.references.empty())parameters.references.push_back({0,{}});
    const SimulationV2::ContinuousWeather snapshot(0,parameters);
    for(int i=0;i<6;++i) {
        const auto wind=source.getExactSimulationWind()?source.getExactSimulationWind()->wind((i+.5)*hill.k*1.2/6,0):snapshot.wind((i+.5)*hill.k*1.2/6,0);const double strength=wind.strength();
        double direction=strength>1e-9?std::atan2(wind.cross,wind.head)*180/3.14159265358979323846:0;if(direction<0)direction+=360;
        weather->setStation(i,"direction",direction);weather->setStation(i,"strength",strength);
    }
    loaded=true;validate();emit changed();
}

SimulationV2::JumpManipulation SimulationJumpDraft::build() const
{
    SimulationV2::JumpManipulation result;
    const auto adjustment=[this](const QString &key) {
        SimulationV2::ValueAdjustment value;
        if(fields[key+"Active"].toBool()) {
            value.bonus=fields[key+"Bonus"].toDouble();
            if(fields[key+"Bounded"].toBool())value.range=SimulationV2::ValueRange{fields[key+"Min"].toDouble(),fields[key+"Max"].toDouble()};
        }
        return value;
    };
    result.quality=adjustment("quality");result.distance=adjustment("distance");
    result.precipitation=adjustment("precipitation");result.measuredWind=adjustment("measuredWind");
    const int landing=fields.value("landing",-1).toInt();if(landing>=0)result.landing=static_cast<SimulationV2::LandingKind>(landing);
    const auto judges=fields["judges"].toList();for(int i=0;i<5;++i)if(judges.value(i,-1).toDouble()>=0)result.judges[std::size_t(i)]=judges[i].toDouble();
    const double dsq=fields["dsq"].toDouble();if(dsqAllowed && dsq>=0)result.dsqProbability=dsq/100;
    result.validate();return result;
}

void SimulationJumpDraft::validate()
{
    error.clear();
    const QList<QPair<QString,QString>> groups{{"quality",tr("Jakość")},{"distance",tr("Odległość")},{"precipitation",tr("Opady")},{"measuredWind",tr("Pomiar wiatru")}};
    for(const auto &group:groups)if(fields[group.first+"Active"].toBool() && fields[group.first+"Bounded"].toBool()
        && fields[group.first+"Min"].toDouble()>fields[group.first+"Max"].toDouble()) {
        error=tr("%1: minimum nie może przekraczać maksimum.").arg(group.second);return;
    }
    try{build();}catch(const std::invalid_argument &){error=tr("Sprawdź zakresy, noty i prawdopodobieństwo dyskwalifikacji.");}
    if(fields["windActive"].toBool() && !weather->message().isEmpty())error=weather->message();
}

void SimulationJumpDraft::setValue(const QString &key,const QVariant &value)
{
    if(!loaded || !fields.contains(key) || key=="revision" || key=="dsqAllowed" || key=="judges")return;
    if(!key.endsWith("Active") && !key.endsWith("Bounded")) {
        bool valid=false;const double number=value.toDouble(&valid);
        if(!valid || !std::isfinite(number)){error=tr("Wpisz poprawną liczbę.");emit changed();return;}
        if(key=="landing" && (number< -1 || number>3 || std::floor(number)!=number))return;
        if(key=="dsq" && (number< -1 || number>100))return;
    }
    if(fields[key]==value){const auto previous=error;validate();if(previous!=error)emit changed();return;}
    fields[key]=value;validate();emit changed();
}

void SimulationJumpDraft::setJudge(int index,double value)
{
    if(!loaded || index<0 || index>=5 || !std::isfinite(value) || (value!=-1 && (value<0 || value>20)))return;
    if(value>=0)value=std::round(value*2)/2;
    auto judges=fields["judges"].toList();judges[index]=value;fields["judges"]=judges;validate();emit changed();
}

int SimulationJumpDraft::changesCount() const
{
    int count=fields["windActive"].toBool();
    for(const auto &key:{"quality","distance","precipitation","measuredWind"})if(fields[QString(key)+"Active"].toBool())++count;
    if(fields.value("landing",-1).toInt()>=0)++count;if(dsqAllowed && fields["dsq"].toDouble()>=0)++count;
    for(const auto &value:fields["judges"].toList())if(value.toDouble()>=0){++count;break;}
    return count;
}

JumpManipulator SimulationJumpDraft::manipulator() const
{
    if(!canApply())throw std::invalid_argument("Invalid jump draft");
    JumpManipulator result;result.setSimulationInput(build());
    if(fields["windActive"].toBool()) {
        result.setExactSimulationWind(weather->field());
        QVector<Wind> winds;for(int i=0;i<6;++i){const auto station=weather->station(i);Wind wind;wind.setDirection(station["direction"].toDouble());wind.setStrength(station["strength"].toDouble());winds.append(wind);}result.setExactWinds(winds);
    }
    return result;
}

void SimulationJumpDraft::reset()
{
    if(loaded)load(JumpManipulator(),hill,baselineWinds,dsqAllowed);
}
