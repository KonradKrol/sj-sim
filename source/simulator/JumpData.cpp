#include "JumpData.h"
#include "JumpSimulator.h"
#include "wind-generation/WindsGenerator.h"
#include "../competitions/CompetitionInfo.h"
#include "../competitions/CompetitionSingleResult.h"
#include "../utilities/functions.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>

JumpData::JumpData(Jumper *jumper, Hill *hill) : jumper(jumper),
    hill(hill),
    Identifiable()
{
    DSQ = false;
    DNS = false;
    inSingleJumps = false;
    competition = nullptr;
}

QString JumpData::getJudgesText()
{
    QString s = "|";
    for(auto & jg : judges)
    {
        s += QString::number(jg, 'f', 1) + "|";
    }
    return s;
}

QString JumpData::getWindsText()
{
    QString s;
    int i=0;
    for(auto & wind : winds)
    {
        s += QString::number(i+1) + ". " + QString::number(wind.getStrength(), 'f', 2) + "m/s" + " (" + QString::number(wind.getDirection()) + "°)" + "\n";
        i++;
    }
    return s;
}

int JumpData::getPositionInRound()
{
    int round = MyFunctions::getIndexOfItemInVector(competition->getResultsReference().getResultOfIndividualJumper(jumper)->getJumpsReference(), this) + 1;
    qDebug()<<"Rd "<<round;
    QHash<Jumper *, double> hash;
    for(auto & res : competition->getResultsReference().getResultsReference())
    {
        if(res.getJumpsReference().count() >= round){
            if(hash.contains(jumper))
                hash[res.getJumper()] += res.getJumpsReference()[round - 1].getPoints();
            else
                hash[res.getJumper()] = res.getJumpsReference()[round - 1].getPoints();
        }
    }
    int pos = 1;
    qDebug()<<"ct "<<hash.count();
    for(auto & key : hash.keys())
    {
        if(hash.value(jumper) < hash.value(key))
            pos++;
    }
    return pos;
}

int JumpData::getPositionInGroupForTeamCompetition()
{
    qDebug()<<"JUMPER "<<jumper->getCountryCode();
    qDebug()<<"TEAMS "<<competition->getTeamsReference().count();
    Team * team = Team::getTeamByCountryCode(&competition->getTeamsReference(), jumper->getCountryCode());
    int group = team->getJumpersReference().indexOf(jumper) + 1;
    int round = MyFunctions::getIndexOfItemInVector(competition->getResultsReference().getResultOfTeam(team)->getTeamJumperResult(jumper)->getJumpsReference(), this) + 1;

    QHash<Jumper *, double> hash;
    for(auto & teamResult : competition->getResultsReference().getResultsReference())
    {
        for(auto & jumperResult : teamResult.getTeamJumpersResultsReference())
        {
           if(MyFunctions::getIndexOfItemInVector(teamResult.getTeamJumpersResultsReference(), &jumperResult) + 1 == group && jumperResult.getJumpsReference().count() >= round){
               if(hash.contains(jumperResult.getJumper()))
                    hash[jumperResult.getJumper()] += jumperResult.getJumpsReference()[round - 1].getPoints();
               else
                    hash[jumperResult.getJumper()] = jumperResult.getJumpsReference()[round - 1].getPoints();
           }
        }
    }
    int pos = 1;
    for(auto & key : hash.keys())
    {
        if(hash.value(jumper) < hash.value(key))
           pos++;
    }
    return pos;
}

int JumpData::getPositionInRoundForTeamCompetition()
{
    Team * team = Team::getTeamByCountryCode(&competition->getTeamsReference(), jumper->getCountryCode());
    int group = team->getJumpersReference().indexOf(jumper) + 1;
    int round = MyFunctions::getIndexOfItemInVector(competition->getResultsReference().getResultOfTeam(team)->getTeamJumperResult(jumper)->getJumpsReference(), this) + 1;

    QHash<Jumper *, double> hash;
    for(auto & teamResult : competition->getResultsReference().getResultsReference())
    {
        for(auto & jumperResult : teamResult.getTeamJumpersResultsReference())
        {
           if(jumperResult.getJumpsReference().count() >= round){
               if(hash.contains(jumperResult.getJumper()))
                    hash[jumperResult.getJumper()] += jumperResult.getJumpsReference()[round - 1].getPoints();
               else
                    hash[jumperResult.getJumper()] = jumperResult.getJumpsReference()[round - 1].getPoints();
           }
        }
    }
    int pos = 1;
    for(auto & key : hash.keys())
    {
        if(hash.value(jumper) < hash.value(key))
           pos++;
    }
    return pos;
}

int JumpData::getPositionInCompetitionForTeamCompetition()
{
    QHash<Jumper *, double> hash;
    for(auto & teamResult : competition->getResultsReference().getResultsReference())
    {
        for(auto & jumperResult : teamResult.getTeamJumpersResultsReference())
        {
               if(hash.contains(jumperResult.getJumper()))
                    hash[jumperResult.getJumper()] += jumperResult.getPointsSum();
               else
                    hash[jumperResult.getJumper()] = jumperResult.getPointsSum();
        }
    }
    int pos = 1;
    for(auto & key : hash.keys())
    {
        if(hash.value(jumper) < hash.value(key))
           pos++;
    }
    return pos;
}

bool JumpData::getInSingleJumps() const
{
    return inSingleJumps;
}

void JumpData::setInSingleJumps(bool newInSingleJumps)
{
    inSingleJumps = newInSingleJumps;
}

CompetitionSingleResult *JumpData::getSingleResult() const
{
    return singleResult;
}

void JumpData::setSingleResult(CompetitionSingleResult *newSingleResult)
{
    singleResult = newSingleResult;
}

CompetitionInfo *JumpData::getCompetition() const
{
    return competition;
}

void JumpData::setCompetition(CompetitionInfo *newCompetition)
{
    competition = newCompetition;
}

double JumpData::getJumperForm() const
{
    return jumperForm;
}

void JumpData::setJumperForm(double newJumperForm)
{
    jumperForm = newJumperForm;
}

bool JumpData::getDSQ() const
{
    return DSQ;
}

void JumpData::setDSQ(bool newDSQ)
{
    DSQ = newDSQ;
}

bool JumpData::getBeats95HSPercents() const
{
    return beats95HSPercents;
}

void JumpData::setBeats95HSPercents(bool newBeats95HSPercents)
{
    beats95HSPercents = newBeats95HSPercents;
}

int JumpData::getCoachGate() const
{
    return coachGate;
}

void JumpData::setCoachGate(int newCoachGate)
{
    coachGate = newCoachGate;
}

bool JumpData::getHasCoachGate() const
{
    return hasCoachGate;
}

void JumpData::setHasCoachGate(bool newHasCoachGate)
{
    hasCoachGate = newHasCoachGate;
}

bool JumpData::getDNS() const
{
    return DNS;
}

void JumpData::setDNS(bool newDNS)
{
    DNS = newDNS;
}

QVector<Wind> JumpData::getWinds() const
{
    return winds;
}

QVector<Wind> *JumpData::getWindsPointer()
{
    return &winds;
}

void JumpData::setWinds(const QVector<Wind> &newWinds)
{
    winds = newWinds;
}

JumpSimulationData & JumpData::getSimulationDataReference()
{
    return simulationData;
}

void JumpData::setSimulationData(const JumpSimulationData &newSimulationData)
{
    simulationData = newSimulationData;
}

double JumpData::getAveragedWind() const
{
    return averagedWind;
}

void JumpData::setAveragedWind(double newAveragedWind)
{
    averagedWind = newAveragedWind;
}

int JumpData::getGate() const
{
    return gate;
}

void JumpData::setGate(int newGate)
{
    gate = newGate;
}

JumpSimulator *JumpData::getSimulator() const
{
    return simulator;
}

void JumpData::setSimulator(JumpSimulator *newSimulator)
{
    simulator = newSimulator;
}

void JumpData::reset()
{
    distance = points = gateCompensation = windCompensation = totalCompensation = gate = judgesPoints = 0;
    landing = Landing();
    judges.clear();
    judges.fill(0, 5);
    winds.clear();
    averagedWind = 0;
    jumper = nullptr;
    hill = nullptr;
    simulator = nullptr;
    DSQ = false;
    simulationData.reset();
}

void JumpData::setupForNextJumper()
{
    distance = points = gateCompensation = windCompensation = totalCompensation = judgesPoints = 0;
    landing = Landing();
    judges.clear();
    judges.fill(0, 5);
    winds.clear();
    averagedWind = 0;
    DSQ = false;
    simulationData.reset();
}

QJsonObject JumpData::getJsonObject(JumpData jumpData)
{
    QJsonObject object;
    object.insert("id", jumpData.getIDStr());
    object.insert("distance", jumpData.getDistance());
    object.insert("points", jumpData.getPoints());
    object.insert("gate", jumpData.getGate());
    object.insert("averaged-wind", jumpData.getAveragedWind());
    object.insert("gate-compensation", jumpData.getGateCompensation());
    object.insert("wind-compensation", jumpData.getWindCompensation());
    object.insert("total-compensation", jumpData.getTotalCompensation());
    object.insert("judges-points", jumpData.getJudgesPoints());
    object.insert("landing-type", jumpData.getLanding().getType());
    object.insert("dsq", jumpData.getDSQ());
    object.insert("dns", jumpData.getDNS());
    object.insert("has-coach-gate", jumpData.getHasCoachGate());
    object.insert("coach-gate", jumpData.getCoachGate());
    object.insert("beats-95-hs-percents", jumpData.getBeats95HSPercents());
    QJsonArray judgesArray;
    for(const auto & jg : jumpData.getJudges())
    {
        judgesArray.push_back(QJsonValue(jg));
    }
    object.insert("judges", judgesArray);
    QJsonObject simulationDataObject;
    simulationDataObject.insert("takeoff-rating", jumpData.getSimulationDataReference().getTakeoffRating());
    simulationDataObject.insert("flight-rating", jumpData.getSimulationDataReference().getFlightRating());
    simulationDataObject.insert("dsq-probability", jumpData.getSimulationDataReference().getDSQProbability());
    simulationDataObject.insert("inrun-snow", jumpData.getSimulationDataReference().getInrunSnow());
    simulationDataObject.insert("meters-sum-random", jumpData.getSimulationDataReference().getMetersSumRandom());
    object.insert("simulation-data", simulationDataObject);
    QJsonArray windsArray;
    int i=0;
    for(const auto & wind : jumpData.getWinds())
    {
        QJsonObject windObject;
        windObject.insert("range", QJsonValue(QString::number(WindsGenerator::getRangeOfWindSector(i+1, jumpData.getHill()->getKPoint()).first) + " - " + QString::number(WindsGenerator::getRangeOfWindSector(i+1, jumpData.getHill()->getKPoint()).second)));
        windObject.insert("direction", wind.getDirection());
        windObject.insert("strength", wind.getStrength());
        windsArray.push_back(windObject);
        i++;
    }
    object.insert("winds", windsArray);

    object.insert("jumper-id", jumpData.getJumper()->getIDStr());
    object.insert("jumper-form", jumpData.getJumperForm());
    object.insert("hill-id", jumpData.getHill()->getIDStr());
    object.insert("in-single-jumps", jumpData.getInSingleJumps());
    return object;
}

JumpData JumpData::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage)
{
    JumpData jump;
    jump.setID(sole::rebuild(obj.value("id").toString().toStdString()));
    jump.setDistance(obj.value("distance").toDouble());
    jump.setPoints(obj.value("points").toDouble());
    jump.setGate(obj.value("gate").toInt());
    jump.setAveragedWind(obj.value("averaged-wind").toDouble());
    jump.setGateCompensation(obj.value("gate-compensation").toDouble());
    jump.setWindCompensation(obj.value("wind-compensation").toDouble());
    jump.setTotalCompensation(obj.value("total-compensation").toDouble());
    jump.setJudgesPoints(obj.value("judges-points").toDouble());
    jump.setLanding(Landing(obj.value("landing-type").toInt()));
    jump.setDSQ(obj.value("dsq").toBool());
    jump.setDNS(obj.value("dns").toBool());
    jump.setHasCoachGate(obj.value("has-coach-gate").toBool());
    jump.setCoachGate(obj.value("coach-gate").toInt());
    jump.setBeats95HSPercents(obj.value("beats-95-hs-percents").toBool());
    QJsonArray judgesArray = obj.value("judges").toArray();
    QVector<double> judges;
    for(const auto & jg : judgesArray)
    {
        judges.push_back(jg.toDouble());
    }
    jump.setJudges(judges);
    QJsonObject simulationDataObject = obj.value("simulation-data").toObject();
    JumpSimulationData simulationData;
    simulationData.setTakeoffRating(simulationDataObject.value("takeoff-rating").toDouble());
    simulationData.setFlightRating(simulationDataObject.value("flight-rating").toDouble());
    simulationData.setDSQProbability(simulationDataObject.value("dsq-probability").toInt());
    simulationData.setInrunSnow(simulationDataObject.value("inrun-snow").toDouble());
    simulationData.setMetersSumRandom(simulationDataObject.value("meters-sum-random").toDouble());
    jump.setSimulationData(simulationData);
    QJsonArray windsArray = obj.value("winds").toArray();
    QVector<Wind> winds;
    int i=0;
    for(const auto & wind : windsArray)
    {
        QJsonObject windObject = wind.toObject();
        Wind w;
        w.setDirection(windObject.value("direction").toDouble());
        w.setStrength(windObject.value("strength").toDouble());
        winds.push_back(w);
        i++;
    }
    jump.setWinds(winds);

    jump.setJumper(static_cast<Jumper *>(storage->get(obj.value("jumper-id").toString())));
    jump.setJumperForm(obj.value("jumper-form").toDouble());
    jump.setHill(static_cast<Hill *>(storage->get(obj.value("hill-id").toString())));
    jump.setInSingleJumps(obj.value("in-single-jumps").toBool(false));

    return jump;
}

Jumper *JumpData::getJumper() const
{
    return jumper;
}

void JumpData::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

Hill *JumpData::getHill() const
{
    return hill;
}

void JumpData::setHill(Hill *newHill)
{
    hill = newHill;
}

double JumpData::getJudgesPoints() const
{
    return judgesPoints;
}

void JumpData::setJudgesPoints(double newJudgesPoints)
{
    judgesPoints = newJudgesPoints;
}

QVector<double> JumpData::getJudges() const
{
    return judges;
}

void JumpData::setJudges(const QVector<double> &newJudges)
{
    judges = newJudges;
}

Landing JumpData::getLanding() const
{
    return landing;
}

void JumpData::setLanding(const Landing &newLanding)
{
    landing = newLanding;
}

double JumpData::getTotalCompensation() const
{
    return totalCompensation;
}

void JumpData::setTotalCompensation(double newTotalCompensation)
{
    totalCompensation = newTotalCompensation;
}

double JumpData::getWindCompensation() const
{
    return windCompensation;
}

void JumpData::setWindCompensation(double newWindCompensation)
{
    windCompensation = newWindCompensation;
}

double JumpData::getGateCompensation() const
{
    return gateCompensation;
}

void JumpData::setGateCompensation(double newGateCompensation)
{
    gateCompensation = newGateCompensation;
}

double JumpData::getPoints() const
{
    return points;
}

void JumpData::setPoints(double newPoints)
{
    points = newPoints;
}

double JumpData::getDistance() const
{
    return distance;
}

void JumpData::setDistance(double newDistance)
{
    distance = newDistance;
}

QDebug operator<<(QDebug d, const JumpData & jumpData)
{
    Jumper * jumper = jumpData.getJumper();
    d <<jumper->getNameAndSurname()<<" ("<<jumper->getCountryCode()<<")";
    d<<jumpData.getDistance()<<"m ("<<jumpData.getPoints()<<" pts) --> Wiatr: "<<WindsCalculator::getAveragedWind(jumpData.getWinds(), jumpData.getSimulator()->getCompetitionRules()->getWindAverageCalculatingType()).getStrengthToAveragedWind()<<"m/s, ("<<jumpData.getWindCompensation()<<" pts), Belka "<<jumpData.getGate()<<" ("<<jumpData.getGateCompensation()<<"),   |"<<jumpData.getJudges().at(0)<<"|"<<jumpData.getJudges().at(1)<<"|"<<jumpData.getJudges().at(2)<<"|"<<jumpData.getJudges().at(3)<<"|"<<jumpData.getJudges().at(4)<<"|"<<",   Lądowanie: "<<jumpData.getLanding().getTextLandingType();

    return d;
}
