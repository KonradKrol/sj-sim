#pragma once
#include "../source/simulation-v2/WeatherSetup.h"
#include "../source/simulation-v2/FieldWindMeasurement.h"

static void simulationV2WeatherSetupRegressions() {
    SimulationWeatherSetup setup;auto hill=HillProfile::create();hill.name="Weather test";hill.perks[2]=3;setup.setHill(hill);
    check(setup.values()["ready"].toBool() && setup.field(),"hill selection automatically proposes ready wind and precipitation");
    setup.applyPreset(0);const auto quiet=setup.windParameters();
    setup.setValue("strength",3.8);const auto strong=setup.windParameters();
    check(quiet.temporalAmplitude==strong.temporalAmplitude && quiet.trendAmplitude==strong.trendAmplitude && quiet.gustAmplitude==strong.gustAmplitude && quiet.gustProbability==strong.gustProbability,"strength changes never increase wind lottery or gust frequency");
    check(std::abs(setup.values()["strength"].toDouble()-3.8)<1e-9,"simple strength control changes the actual base vector");
    setup.setValue("variability",0);const auto stable=setup.field();
    for(double distance:{0.,40.,100.,150.}) {
        const auto a=stable->wind(distance,0),b=stable->wind(distance,10000);
        check(a.head==b.head && a.cross==b.cross,"zero temporal variability is truly static even with different references along the hill");
    }
    setup.applyPreset(4);check(setup.values()["strength"].toDouble()>0 && setup.values()["strength"].toDouble()<=.5,"calm is weak wind rather than zero");
    setup.setValue("strength",0);setup.setValue("variability",0);setup.setValue("differences",0);setup.setValue("direction",180);setup.setValue("strength",1.8);
    check(setup.field()->wind(70,0).head< -1.79,"direction chosen in calm conditions is retained when strength is added");
    const auto before=setup.values()["seed"];const auto preserved=setup.field()->wind(70,200);
    setup.setPreviewSeconds(180);setup.preview();setup.values();setup.setPreviewSeconds(0);
    check(setup.values()["seed"]==before && setup.field()->wind(70,200).head==preserved.head,"preview never rerolls or advances the configured field");
    hill.perks[2]=-3;hill.perks[3]=3;hill.name="Changed weather hill";setup.setHill(hill);
    check(setup.values()["hillName"]==hill.name,"context changes update the weather header without rerolling");
    check(setup.values()["seed"]==before && setup.field()->wind(70,200).head==preserved.head,"hill perks never secretly alter manually chosen weather");
    setup.setValue("precipitationScenario",1);setup.setValue("precipitationIntensity",7);setup.applyPreset(2);
    check(setup.field()->precipitation(0)==7,"wind preset does not reset precipitation");
    setup.setValue("precipitationKind",1);check(setup.field()->precipitationKind()==SimulationV2::PrecipitationKind::Rain && setup.values()["surface"].toInt()==0,"rain is independent of snow landing surface");
    setup.setValue("precipitationLevel",1);check(setup.field()->precipitation(0)==2,"simple light precipitation selects its actual intensity");
    setup.setValue("precipitationPattern",2);check(setup.field()->precipitation(600)>setup.field()->precipitation(0),"simple increasing option really evolves in time");
    setup.setValue("precipitationLevel",0);check(setup.field()->precipitation(600)==0,"no precipitation disables the temporal trend");
    setup.setValue("precipitationLevel",3);check(setup.values()["precipitationLevel"].toInt()==3,"rain intensity can be reenabled after no precipitation");
    check(setup.presets().size()==SimulationV2::weatherPresetCount,"all scenario names match available sampler choices");
    setup.setValue("variability",0);const auto localSeed=setup.values()["seed"];
    setup.putReference(113,1.7,25);const auto local=setup.referenceAt(113);
    check(local["index"].toInt()>=0 && std::abs(local["strength"].toDouble()-1.7)<1e-8 && std::abs(local["direction"].toDouble()-25)<1e-8,"local reference accepts an exact metre and arbitrary degree angle");
    check(std::abs(setup.field()->wind(113,0).head-1.7*std::cos(25*3.14159265358979323846/180))<1e-8,"local edit changes the actual continuous field");
    check(std::abs(setup.field()->wind(113-.0001,0).head-setup.field()->wind(113+.0001,0).head)<.001,"local edit remains continuous around the reference rather than a hard sector");
    check(setup.values()["seed"]==localSeed,"editing a point never secretly rerolls weather");
    const auto pointCount=setup.references().size();setup.putReference(113,2.1,138);
    check(setup.references().size()==pointCount,"editing the same location updates instead of duplicating a reference");
    setup.removeReference(setup.referenceAt(113)["index"].toInt());check(setup.references().size()==pointCount-1,"local reference can be removed");
    const auto neighbour=setup.station(1);const auto fixedSeed=setup.values()["seed"];
    for(int i=0;i<20;++i)setup.setStation(3,"strength",1.+i*.1);
    check(setup.references().size()==6 && std::abs(setup.station(1)["strength"].toDouble()-neighbour["strength"].toDouble())<1e-8,"repeated direct slope edits keep six controls and preserve other reference values");
    setup.setStation(3,"strength",0);setup.setStation(3,"direction",137);setup.setStation(3,"strength",2);
    check(std::abs(setup.station(3)["direction"].toDouble()-137)<1e-8 && setup.values()["seed"]==fixedSeed,"zero-strength point remembers a direction chosen before adding wind");
    for(int index=5;index<SimulationV2::weatherPresetCount;++index){setup.applyPreset(index);check(setup.values()["preset"].toInt()==index,"all six extra scenarios are selectable");}
    setup.setValue("precipitationScenario",1);setup.setValue("precipitationIntensity",7);
    setup.setValue("precipitationScenario",3);setup.setValue("precipitationIntensity",2);check(setup.precipitationParameters().intensity==8,"heavy precipitation displays its actual minimum intensity");setup.setValue("precipitationScenario",1);setup.setValue("precipitationIntensity",7);
    const auto frozen=setup.field();const auto frozenSample=frozen->wind(80,300);const auto oldSeed=setup.values()["seed"];
    setup.reroll();check(setup.values()["seed"]!=oldSeed && setup.field()->precipitation(0)==7,"explicit reroll produces a new preset variant without changing manual precipitation");
    check(frozen->wind(80,300).head==frozenSample.head,"jobs keep an immutable snapshot when setup is changed later");
    const auto valid=setup.field();setup.setValue("gustProbability",2);
    check(setup.field()==valid && !setup.message().isEmpty(),"invalid advanced inputs preserve the valid configuration");
    setup.setReference(1,"distance",0);check(setup.field()==valid && !setup.message().isEmpty(),"invalid reference ordering never replaces the valid field");
    const auto oldLast=setup.references().last().toMap()["distance"].toDouble();hill.k=140;hill.hs=160;setup.setHill(hill);
    check(std::abs(setup.references().last().toMap()["distance"].toDouble()-oldLast*140/120)<1e-8,"editing hill size rescales the reference layout instead of discarding conditions");
    auto brokenHill=hill;brokenHill.k=0;const auto safeField=setup.field();setup.setHill(brokenHill);
    check(setup.field()==safeField && !setup.message().isEmpty(),"invalid hill input never corrupts the weather configuration");
    CompetitionRules rules;rules.setWindAverageCalculatingType(WindsCalculator::Original);rules.setWindCompensationDistanceEffect(0);
    SimulationV2::FieldWindMeasurement measurement(120,rules);
    SimulationV2::WindParameters constant;constant.temporalAmplitude=constant.spatialAmplitude=constant.gustAmplitude=0;constant.references={{0,{1.25,.7}},{150,{1.25,.7}}};
    SimulationV2::ContinuousWeather field(123,constant);SimulationV2::FlightResult flight;flight.distance=125;flight.duration=4;
    check(std::abs(measurement.measure(field,flight,100)-1.25)<1e-9,"continuous wind measurement preserves longitudinal projection without truncating angles");
    setup.clearHill();check(!setup.values()["ready"].toBool(),"empty hill selection has an explicit weather empty state");
    qInfo()<<"PASS: weather setup, independent strength/lottery, immutable variants, manual overrides and continuous measurement";
}
