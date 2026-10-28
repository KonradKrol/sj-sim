#pragma once
#include "../source/simulation-v2/FieldWindMeasurement.h"
#include <functional>

static void simulationV2MeasurementRegressions()
{
    using namespace SimulationV2;
    struct Field final:WeatherField {
        std::function<SimulationV2::Wind(double,double)> sample;
        SimulationV2::Wind wind(double distance,double time) const override {return sample(distance,time);}
        double precipitation(double) const override {return 0;}
    } field;
    CompetitionRules rules;rules.setWindAverageCalculatingType(WindsCalculator::Original);rules.setWindCompensationDistanceEffect(0);
    FlightResult flight;flight.distance=125;flight.duration=4;flight.experiencedWind={1.25,.7};
    field.sample=[](double,double){return SimulationV2::Wind{1.25,.7};};
    for(int mode=0;mode<=CompetitionRules::FlightWindMeasurement;++mode) {
        check(rules.setWindMeasurementType(mode),"measurement mode accepted");
        check(FieldWindMeasurement(120,rules).measure(field,flight,100)==1.25,"all measurement modes agree in uniform steady wind");
        const auto json=CompetitionRules::getJsonObject(rules);
        check(CompetitionRules::validateWindMeasurementJson(json) && CompetitionRules::getFromJson(json).getWindMeasurementType()==mode,"measurement selection survives rule serialization");
    }
    auto old=CompetitionRules::getJsonObject(rules);old.remove("wind-measurement-type");
    check(CompetitionRules::getFromJson(old).getWindMeasurementType()==CompetitionRules::LegacyWindMeasurement,"old rule packs retain their earlier measurement policy");
    old["wind-measurement-type"]=6;check(!CompetitionRules::validateWindMeasurementJson(old) && !rules.setWindMeasurementType(6),"unknown methods cannot silently replace known rules");
    old["wind-measurement-type"]=2.5;check(!CompetitionRules::validateWindMeasurementJson(old),"fractional method ids are rejected");
    DataPack pack;pack.kind=DataPack::Rules;pack.rules={rules};DataPack decoded;QString error;
    auto invalid=pack.toJson();auto entries=invalid["competitionsRules"].toArray();check(!entries.isEmpty(),"rules pack serialization fixture");
    auto entry=entries[0].toObject();entry["wind-measurement-type"]=6;entries[0]=entry;invalid["competitionsRules"]=entries;
    check(!DataPack::fromJson(invalid,decoded,error),"unsupported measurement is rejected at the pack boundary");
    field.sample=[](double,double time){return SimulationV2::Wind{time-100,0};};
    rules.setWindMeasurementType(CompetitionRules::InstantWindMeasurement);
    check(FieldWindMeasurement(120,rules).measure(field,flight,100)==0,"instant mode reads at takeoff rather than halfway through flight");
    rules.setWindMeasurementType(CompetitionRules::SensorWindMeasurement);
    check(FieldWindMeasurement(120,rules).measure(field,flight,100)==2,"sensor window includes later conditions at already passed locations");
    field.sample=[](double distance,double){return SimulationV2::Wind{distance/120,0};};
    const double uniform=FieldWindMeasurement(120,rules).measure(field,flight,100);
    rules.setWindMeasurementType(CompetitionRules::WeightedWindMeasurement);
    check(FieldWindMeasurement(120,rules).measure(field,flight,100)>uniform,"weighted mode gives more influence to the middle and later slope");
    field.sample=[](double distance,double){const double offset=distance-132*10.5/27;return SimulationV2::Wind{5*std::exp(-offset*offset),0};};
    rules.setWindMeasurementType(CompetitionRules::SensorWindMeasurement);const double missed=FieldWindMeasurement(120,rules).measure(field,flight,100);
    rules.setWindMeasurementType(CompetitionRules::DenseWindMeasurement);
    check(FieldWindMeasurement(120,rules).measure(field,flight,100)>missed,"dense network detects a gust between sparse sensors without inventing a random measurement error");
    field.sample=[](double,double){return SimulationV2::Wind{0,4};};flight.experiencedWind={0,4};
    for(int mode=0;mode<=CompetitionRules::FlightWindMeasurement;++mode){rules.setWindMeasurementType(mode);check(FieldWindMeasurement(120,rules).measure(field,flight,100)==0,"pure crosswind has no longitudinal measurement");}
    FieldWindMeasurementParameters parameters;parameters.temporalSamples=0;bool rejected=false;
    try{FieldWindMeasurement invalidParameters(120,rules,parameters);}catch(const std::invalid_argument &){rejected=true;}
    check(rejected,"invalid tuning parameters cannot reach a worker");
    CompetitionRulesEditorWidget editor;editor.setCompetitionRules(&rules);editor.fillCompetitionRulesInputs();
    auto *choice=editor.findChild<QComboBox *>("comboBox_windMeasurementType");
    check(choice && choice->count()==6,"one compact choice exposes six distinct methods including compatibility");
    choice->setCurrentIndex(choice->findData(CompetitionRules::SensorWindMeasurement));
    check(editor.getCompetitionRulesFromWidgetInputs().getWindMeasurementType()==CompetitionRules::SensorWindMeasurement && editor.findChild<QComboBox *>("comboBox_windAverageCalculatingType")->isHidden(),"new methods hide obsolete direction and distance switches and are used by the actual rules editor");
    choice->setCurrentIndex(choice->findData(CompetitionRules::LegacyWindMeasurement));
    check(!editor.findChild<QComboBox *>("comboBox_windAverageCalculatingType")->isHidden(),"compatibility selection keeps earlier configuration available");
    auto profile=AthleteProfile::create();profile.overall=80;auto venue=HillProfile::create();Jumper athlete;::Hill hill;
    check(athlete.setSimulationProfile(profile,error) && hill.setSimulationProfile(venue,error),"measurement engine fixtures validate");
    auto moving=std::make_shared<Field>();moving->sample=[](double,double time){return SimulationV2::Wind{time-100,0};};
    rules.setHasWindCompensations(true);rules.setHasJudgesPoints(true);rules.setHasDsq(false);
    JumpSimulator simulator;JumpManipulator manipulation;simulator.setJumper(&athlete);simulator.setHill(&hill);simulator.setCompetitionRules(&rules);simulator.setManipulator(&manipulation);simulator.setGate(10);simulator.setCompetitionStartGate(10);
    QVector<JumpData> results;
    for(int mode=0;mode<=CompetitionRules::FlightWindMeasurement;++mode) {
        rules.setWindMeasurementType(mode);simulator.setSimulationWeather(moving,std::make_shared<FieldWindMeasurement>(120,rules));simulator.setSimulationTime(100);simulator.setSimulationSeed(12345);simulator.simulateJump();results.append(simulator.getJumpData());
        check(results.last().getSimulationV2Data()["windMeasurementType"].toInt()==mode,"historical result records its selected measurement method");
        check(results.last().getDistance()==results.first().getDistance() && results.last().getJudges()==results.first().getJudges() && results.last().getSimulationV2Data()["rawDistance"]==results.first().getSimulationV2Data()["rawDistance"],"changing measurement never changes physical flight, landing or judges for the same seed");
    }
    check(results[CompetitionRules::InstantWindMeasurement].getAveragedWind()==0 && results[CompetitionRules::SensorWindMeasurement].getAveragedWind()>0 && results[CompetitionRules::SensorWindMeasurement].getPoints()!=results[CompetitionRules::InstantWindMeasurement].getPoints(),"the same physical jump receives method-dependent wind compensation");
    QVector<::Wind> exact(6);for(auto &wind:exact){wind.setDirection(0);wind.setStrength(3);}manipulation.setExactWinds(exact);
    for(int mode=1;mode<=CompetitionRules::FlightWindMeasurement;++mode) {
        rules.setWindMeasurementType(mode);simulator.setSimulationTime(100);simulator.simulateJump();
        check(simulator.getJumpData().getAveragedWind()==3,"exact wind manipulation retains the selected measurement policy on its override field");
    }
    simulator.setSimulationSkip(true);simulator.simulateJump();
    check(simulator.getJumpData().getDNS() && simulator.getJumpData().getPoints()==0,"new measurement methods preserve DNS without requiring a flight");
    qInfo()<<"PASS: six wind measurement modes, real sampling differences, stable physical flight, rule editor and persistence";
}
