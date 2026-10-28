#include "../simulator/JumpSimulator.h"
#include "ProfileBridge.h"
#include "../global/MyRandom.h"
#include "../simulator/wind-generation/WindsGenerator.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <limits>

namespace {
class LegacyMeasurement final:public SimulationV2::WindMeasurement {
public:
    LegacyMeasurement(QVector<Wind> values,double k,const CompetitionRules &rules):values(std::move(values)),k(k),rules(rules){}
    double measure(const SimulationV2::WeatherField &,const SimulationV2::FlightResult &flight,double) const override {
        return WindsCalculator::getAveragedWind(WindsCalculator::getScoringWinds(values,flight.distance,k,rules.getWindCompensationDistanceEffect()),rules.getWindAverageCalculatingType()).getStrengthToAveragedWind();
    }
private:
    QVector<Wind> values;double k;CompetitionRules rules;
};

SimulationV2::ContinuousWeather referenceWeather(const QVector<Wind> &winds,double k,double precipitation)
{
    SimulationV2::WindParameters config;config.references.clear();
    config.temporalAmplitude=config.spatialAmplitude=config.gustAmplitude=0;
    for(int i=0;i<winds.size();++i) {
        const auto &w=winds[i];const double angle=w.getDirection()*3.14159265358979323846/180;
        config.references.push_back({(i+.5)*WindsGenerator::getWindSegmentDistance(k,winds.size()),{w.getStrength()*std::cos(angle),w.getStrength()*std::sin(angle)}});
    }
    if(config.references.empty())config.references.push_back({0,{}});
    SimulationV2::PrecipitationParameters precip;precip.scenario=SimulationV2::PrecipitationScenario::Stable;precip.intensity=std::clamp(precipitation,0.0,10.0);
    return SimulationV2::ContinuousWeather(0,config,precip);
}

std::optional<SimulationV2::ValueRange> range(QPair<double,double> value,double lo,double hi)
{
    if(value.first<=lo && value.second<0)return std::nullopt;
    return SimulationV2::ValueRange{std::max(lo,value.first),value.second<0?hi:value.second};
}

SimulationV2::JumpRequest requestFor(const Jumper &jumper,const Hill &hill,const CompetitionRules &rules,
    int gate,int referenceGate,bool hasCoachGate,int coachGate,double dsq,SimulationV2::EventKind event,double time,const JumpManipulator *manipulator)
{
    const auto athlete=jumper.getSimulationProfile();const auto venue=hill.getSimulationProfile();
    if(!athlete || !venue)throw std::invalid_argument("New simulation requires both a new athlete profile and a new hill profile");
    SimulationV2::JumpRequest input;
    input.athlete=SimulationV2::athleteInput(*athlete);input.hill=SimulationV2::hillInput(*venue);input.event=event;input.startSeconds=time;
    input.scoring.juryGate=gate;input.scoring.referenceGate=referenceGate;
    input.scoring.coachReduction=hasCoachGate?gate-coachGate:0;
    input.scoring.windCompensation=rules.getHasWindCompensations();input.scoring.gateCompensation=rules.getHasGateCompensations();
    input.scoring.judges=rules.getHasJudgesPoints();input.scoring.coach95PercentRule=rules.getHas95HSRule();
    // The legacy setting means one in N, not N percent.
    input.dsqProbability=rules.getHasDsq() && std::isfinite(dsq) && dsq>0?1/std::max(1.0,dsq):0;
    if(manipulator) {
        auto &m=input.manipulation;
        m.distance={manipulator->getDistanceBonus(),range(manipulator->getDistanceRange(),0,1000)};
        m.precipitation={manipulator->getInrunSnowBonus(),range(manipulator->getInrunSnowRange(),0,10)};
        m.measuredWind.bonus=manipulator->getAveragedWindBonus();
        if(manipulator->getAveragedWindRangeEnabled()) {const auto r=manipulator->getAveragedWindRange();m.measuredWind.range=SimulationV2::ValueRange{r.first,r.second};}
        if(manipulator->getExactLandingType()>=0)m.landing=static_cast<SimulationV2::LandingKind>(manipulator->getExactLandingType());
        const auto marks=manipulator->getExactJudges();for(int i=0;i<std::min(5,marks.size());++i)if(marks[i]>=0)m.judges[std::size_t(i)]=marks[i];
        if(manipulator->getExactDSQProbability()>=0 && rules.getHasDsq()) {
            const int inverse=manipulator->getExactDSQProbability();m.dsqProbability=inverse==0?0:1.0/inverse;
        }
    }
    return input;
}

QJsonArray vector(SimulationV2::Wind wind) {return {wind.head,wind.cross};}
QJsonObject metadata(const SimulationV2::JumpResult &result,SimulationV2::Seed seed,double time,SimulationV2::EventKind event)
{
    QJsonArray exposure;
    for(const auto &part:result.flight.exposures)exposure.append(QJsonObject{{"begin",part.begin},{"end",part.end},{"traversed",part.traversed},
        {"before",vector(part.before)},{"experienced",vector(part.experienced)},{"after",vector(part.after)},
        {"minimumHead",part.minimumHead},{"maximumHead",part.maximumHead}});
    const auto &q=result.quality;
    return {{"engineVersion",SimulationV2::JumpResult::engineVersion},{"seed",QString::number(seed)},{"startSeconds",time},{"event",int(event)},
        {"quality",QJsonObject{{"overallAndForm",q.overallAndForm},{"takeoffWeight",q.takeoffWeight},{"hill",q.hillEffect},{"event",q.eventEffect},{"random",q.randomEffect},{"manipulation",q.manipulationEffect}}},
        {"rawDistance",result.flight.distance},{"reserve",result.flight.reserve},{"precipitation",result.flight.precipitation},
        {"experiencedWind",vector(result.flight.experiencedWind)},{"meanAbsoluteCrosswind",result.flight.meanAbsoluteCrosswind},
        {"exposures",exposure},{"manipulated",result.manipulated},{"distanceLimitedByGeometry",result.distanceLimitedByGeometry}};
}
}

void JumpSimulator::setSimulationWeather(std::shared_ptr<const SimulationV2::WeatherField> weather,std::shared_ptr<const SimulationV2::WindMeasurement> measurement)
{
    if(bool(weather)!=bool(measurement))throw std::invalid_argument("Weather and its measurement policy must be supplied together");
    simulationWeather=std::move(weather);simulationMeasurement=std::move(measurement);
}

void JumpSimulator::simulateV2()
{
    const auto activeWinds=manipulator && !manipulator->getExactWinds().isEmpty()?manipulator->getExactWinds():winds;
    std::optional<SimulationV2::ContinuousWeather> fallbackWeather;
    if(!simulationWeather)fallbackWeather.emplace(referenceWeather(activeWinds,hill->getKPoint(),inrunSnow));
    const LegacyMeasurement measurement(activeWinds,hill->getKPoint(),*competitionRules);
    const auto input=requestFor(*jumper,*hill,*competitionRules,gate,competitionStartGate,hasCoachGate,coachGate,DSQBaseProbability,simulationEvent,simulationTime,manipulator);
    // Sequence the draws explicitly: operand evaluation order must not change
    // the seed between compilers/platforms.
    const SimulationV2::Seed seed=[this] {
        if(simulationSeed)return *simulationSeed;
        const SimulationV2::Seed high=MyRandom::randomInt(0,0x3fffff);
        const SimulationV2::Seed middle=MyRandom::randomInt(0,0x1fffff);
        const SimulationV2::Seed low=MyRandom::randomInt(0,0x1fffff);
        return (high<<42)|(middle<<21)|low;
    }();
    SimulationV2::SeededRandom rng(seed);
    const auto result=simulationEngine.simulate(input,rng,simulationWeather?*simulationWeather:*fallbackWeather,
        simulationMeasurement?*simulationMeasurement:static_cast<const SimulationV2::WindMeasurement &>(measurement));
    jumpData=JumpData(jumper,hill);jumpData.setSimulator(this);simulationData=&jumpData.getSimulationDataReference();
    jumpData.setGate(gate);jumpData.setHasCoachGate(hasCoachGate);jumpData.setCoachGate(coachGate);
    jumpData.setJumperForm(input.athlete.form);jumpData.setDSQ(result.status==SimulationV2::JumpStatus::Disqualified);
    jumpData.setDNS(result.status==SimulationV2::JumpStatus::DidNotStart);
    jumpData.setDistance(result.measuredDistance);jumpData.setPoints(result.score.total);
    jumpData.setLanding(Landing(int(result.landing)));
    QVector<double> marks;for(double mark:result.judges)marks.append(mark);jumpData.setJudges(marks);jumpData.setJudgesPoints(result.score.judges);
    jumpData.setWinds(activeWinds);jumpData.setAveragedWind(result.score.measuredWind);
    jumpData.setWindCompensation(result.score.wind);jumpData.setGateCompensation(result.score.gate);
    jumpData.setTotalCompensation(result.score.wind+result.score.gate);jumpData.setBeats95HSPercents(result.score.coachThresholdReached);
    simulationData->setInrunSnow(result.flight.precipitation);
    simulationData->setDSQProbability(std::isfinite(DSQBaseProbability) && DSQBaseProbability>=0 && DSQBaseProbability<=std::numeric_limits<int>::max()?int(DSQBaseProbability):0);
    jumpData.setSimulationV2Data(metadata(result,seed,simulationTime,simulationEvent));
    simulationTime+=30;
}

double JumpSimulator::previewV2() const
{
    if(!competitionRules)throw std::invalid_argument("New preview requires competition rules");
    const auto activeWinds=manipulator && !manipulator->getExactWinds().isEmpty()?manipulator->getExactWinds():winds;
    std::optional<SimulationV2::ContinuousWeather> fallbackWeather;
    if(!simulationWeather)fallbackWeather.emplace(referenceWeather(activeWinds,hill->getKPoint(),inrunSnow));
    const auto input=requestFor(*jumper,*hill,*competitionRules,gate,competitionStartGate,hasCoachGate,coachGate,DSQBaseProbability,simulationEvent,simulationTime,manipulator);
    return simulationEngine.previewDistance(input,simulationWeather?*simulationWeather:*fallbackWeather);
}
