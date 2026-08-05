#include "CompetitionRules.h"

#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>

CompetitionRules::CompetitionRules(const QString & name) : name(name),
    Identifiable()
{
    has95HSRule = hasWindCompensations = hasGateCompensations = hasJudgesPoints = hasDsq = hillRecordBreaking = false;
    competitionType = jumpersInTeamCount = windAverageCalculatingType = 0;
    windCompensationDistanceEffect = 1;
}

bool CompetitionRules::getHillRecordBreaking() const
{
    return hillRecordBreaking;
}

void CompetitionRules::setHillRecordBreaking(bool newHillRecordBreaking)
{
    hillRecordBreaking = newHillRecordBreaking;
}

short CompetitionRules::getWindCompensationDistanceEffect() const
{
    return windCompensationDistanceEffect;
}

void CompetitionRules::setWindCompensationDistanceEffect(short newWindCompensationDistanceEffect)
{
    windCompensationDistanceEffect = newWindCompensationDistanceEffect;
}

short CompetitionRules::getWindAverageCalculatingType() const
{
    return windAverageCalculatingType;
}

void CompetitionRules::setWindAverageCalculatingType(short newWindAverageCalculatingType)
{
    windAverageCalculatingType = newWindAverageCalculatingType;
}

bool CompetitionRules::getHasDsq() const
{
    return hasDsq;
}

void CompetitionRules::setHasDsq(bool newHasDsq)
{
    hasDsq = newHasDsq;
}

int CompetitionRules::getJumpersInTeamCount() const
{
    return jumpersInTeamCount;
}

void CompetitionRules::setJumpersInTeamCount(int newJumpersInTeamCount)
{
    jumpersInTeamCount = newJumpersInTeamCount;
}

QString CompetitionRules::getName() const
{
    return name;
}

void CompetitionRules::setName(const QString &newName)
{
    name = newName;
}

QJsonObject CompetitionRules::getJsonObject(const CompetitionRules &competitionRules)
{
    QJsonObject object;
    object.insert("id", competitionRules.getIDStr());
    object.insert("name", competitionRules.getName());
    object.insert("wind-compensations", competitionRules.getHasWindCompensations());
    object.insert("gate-compensations", competitionRules.getHasGateCompensations());
    object.insert("judges-points", competitionRules.getHasJudgesPoints());
    object.insert("jumpers-in-team-count", competitionRules.getJumpersInTeamCount());
    object.insert("95-hs-rule", competitionRules.getHas95HSRule());
    object.insert("dsq", competitionRules.getHasDsq());
    object.insert("competition-type", competitionRules.getCompetitionType());
    object.insert("wind-compensation-distance-effect", competitionRules.getWindCompensationDistanceEffect());
    object.insert("wind-average-calculating-type", competitionRules.getWindAverageCalculatingType());
    object.insert("hill-record-breaking", competitionRules.getHillRecordBreaking());

    QJsonArray roundsArray;
    for(auto & round : competitionRules.getRounds()){
        QJsonObject roundObject;
        roundObject.insert("count", round.getCount());
        roundObject.insert("sort-start-list", round.getSortStartList());
        roundObject.insert("sort-after-groups", round.getSortAfterGroups());
        roundObject.insert("ko", round.getKO());
        roundObject.insert("ko-group-count", round.getCountInKOGroup());
        roundObject.insert("ko-group-advancing", round.getAdvancingFromKOGroup());
        roundObject.insert("ko-groups-selection-type", round.getKoGroupsSelectionType());
        roundsArray.push_back(roundObject);
    }
    object.insert("rounds", roundsArray);

    return object;
}

QVector<CompetitionRules> CompetitionRules::getCompetitionRulesVectorFromJson(const QByteArray &bytes)
{
    QVector<CompetitionRules> rulesVector;
    rulesVector.clear();
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu zasad konkursów", "Nie udało się wczytać zawodników z pliku userData/GlobalDatabase/globalCompetitionsRules.json\nTreść błędu: " + error.errorString(), QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return rulesVector;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("competitionsRules");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu zasad konkursów", "Prawdopodobnie w tym pliku nie ma listy z zasadami konkursów\nUpewnij się, że wybrałeś właściwy plik!");
        message.setModal(true);
        message.exec();
        return rulesVector;
    }
    QJsonArray array = value.toArray();
    for(const auto & val : array)
    {
        rulesVector.push_back(CompetitionRules::getFromJson(val.toObject()));
    }
    return rulesVector;
}

CompetitionRules CompetitionRules::getFromJson(const QJsonObject &obj)
{
    CompetitionRules rules;
    rules.setID(sole::rebuild(obj.value("id").toString().toStdString()));
    rules.setName(obj.value("name").toString());
    rules.setHas95HSRule(obj.value("95-hs-rule").toBool());
    rules.setHasWindCompensations(obj.value("wind-compensations").toBool());
    rules.setHasGateCompensations(obj.value("gate-compensations").toBool());
    rules.setHasJudgesPoints(obj.value("judges-points").toBool());
    rules.setHasDsq(obj.value("dsq").toBool());
    rules.setJumpersInTeamCount(obj.value("jumpers-in-team-count").toInt());
    rules.setCompetitionType(obj.value("competition-type").toInt());
    rules.setWindCompensationDistanceEffect(obj.value("wind-compensation-distance-effect").toInt());
    rules.setWindAverageCalculatingType(obj.value("wind-average-calculating-type").toInt());
    rules.setHillRecordBreaking(obj.value("hill-record-breaking").toBool(true));

    QJsonArray roundsArray = obj.value("rounds").toArray();
    for(const auto & round : roundsArray)
    {
        RoundInfo roundInfo;
        roundInfo.setCount(round.toObject().value("count").toInt());
        roundInfo.setSortStartList(round.toObject().value("sort-start-list").toBool(true));
        roundInfo.setSortAfterGroups(round.toObject().value("sort-after-groups").toInt());
        roundInfo.setKO(round.toObject().value("ko").toBool());
        roundInfo.setCountInKOGroup(round.toObject().value("ko-group-count").toInt());
        roundInfo.setAdvancingFromKOGroup(round.toObject().value("ko-group-advancing").toInt());
        roundInfo.setKoGroupsSelectionType(round.toObject().value("ko-groups-selection-type").toInt());
        rules.getRoundsReference().push_back(roundInfo);
    }
    return rules;
}

//Błąd występuje po wykonaniu skoku świeżo po posortowaniu
QVector<RoundInfo> CompetitionRules::getRounds() const
{
    return rounds;
}

void CompetitionRules::setRounds(const QVector<RoundInfo> &newRounds)
{
    rounds = newRounds;
}

QVector<RoundInfo> &CompetitionRules::getRoundsReference()
{
    return rounds;
}

short CompetitionRules::getCompetitionType() const
{
    return competitionType;
}

void CompetitionRules::setCompetitionType(short newCompetitionType)
{
    competitionType = newCompetitionType;
}

bool CompetitionRules::getHasJudgesPoints() const
{
    return hasJudgesPoints;
}

void CompetitionRules::setHasJudgesPoints(bool newHasJudgesPoints)
{
    hasJudgesPoints = newHasJudgesPoints;
}

bool CompetitionRules::getHasGateCompensations() const
{
    return hasGateCompensations;
}

void CompetitionRules::setHasGateCompensations(bool newHasGateCompensations)
{
    hasGateCompensations = newHasGateCompensations;
}

bool CompetitionRules::getHasWindCompensations() const
{
    return hasWindCompensations;
}

void CompetitionRules::setHasWindCompensations(bool newHasWindCompensations)
{
    hasWindCompensations = newHasWindCompensations;
}

bool CompetitionRules::getHas95HSRule() const
{
    return has95HSRule;
}

void CompetitionRules::setHas95HSRule(bool newHas95HSRule)
{
    has95HSRule = newHas95HSRule;
}
