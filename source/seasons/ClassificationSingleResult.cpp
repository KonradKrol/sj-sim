#include "ClassificationSingleResult.h"
#include "../global/CountryCodes.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../seasons/Classification.h"
#include "../competitions/CompetitionInfo.h"

ClassificationSingleResult::ClassificationSingleResult(Classification *classification, Jumper * jumper) : jumper(jumper), position(0), pointsSum(0), classification(classification)
{

}

ClassificationSingleResult::ClassificationSingleResult(Classification *classification, QString teamCode) : teamCode(CountryCodes::toFis(teamCode)), position(0), pointsSum(0), classification(classification), jumper(nullptr)
{

}

void ClassificationSingleResult::updateSingleResults()
{
    singleResults.clear();
    for(auto & results : competitionsResults)
    {
        int competitionType = results->getCompetition()->getRulesPointer()->getCompetitionType();
        for(auto & singleResult : results->getResultsReference())
        {
            if(singleResult.isTeamDisqualified())continue;
            if(classification->getClassificationType() == Classification::Individual)
            {
                if(classification->getPunctationType() == Classification::PointsForPlaces)
                {
                    if(competitionType == CompetitionRules::Individual){
                        if(singleResult.getJumper() == this->getJumper()){
                            singleResults.push_back(&singleResult);
                            //break;
                        }
                    }
                    else if(competitionType == CompetitionRules::Team){
                        if(singleResult.getTeam()->getJumpersReference().contains(this->getJumper()))
                        {
                            singleResults.push_back(&singleResult);
                        }
                    }
                }
                else if(classification->getPunctationType() == Classification::CompetitionPoints)
                {
                    if(competitionType == CompetitionRules::Individual){
                        if(singleResult.getJumper() == this->getJumper()){
                            singleResults.push_back(&singleResult);
                            //break;
                        }
                    }
                    else if(competitionType == CompetitionRules::Team)
                    {
                        for(auto &jumperResult : singleResult.getTeamJumpersResultsReference()) {
                            if(jumperResult.getJumper() == this->getJumper())
                                singleResults.push_back(&jumperResult);
                        }
                    }
                }
            }
            else if(classification->getClassificationType() == Classification::Team)
            {
                if(classification->getPunctationType() == Classification::PointsForPlaces)
                {
                    if(competitionType == CompetitionRules::Team){
                        if(singleResult.getTeam()->getCountryCode() == this->getTeamCode()){
                            singleResults.push_back(&singleResult);
                        }
                    }
                    else if(competitionType == CompetitionRules::Individual)
                    {
                        if(singleResult.getJumper()->getCountryCode() == this->getTeamCode()){
                            singleResults.push_back(&singleResult);
                        }
                    }
                }
                else if(classification->getPunctationType() == Classification::CompetitionPoints)
                {
                    if(competitionType == CompetitionRules::Team){
                        if(singleResult.getTeam()->getCountryCode() == this->getTeamCode()){
                            singleResults.push_back(&singleResult);
                            //break;
                        }
                    }
                    else if(competitionType == CompetitionRules::Individual)
                    {
                        if(singleResult.getJumper()->getCountryCode() == this->getTeamCode()){
                            singleResults.push_back(&singleResult);
                            //break;
                        }
                    }
                }
            }
        }
    }
}

void ClassificationSingleResult::addCompetitionResults(CompetitionResults &results)
{
    competitionsResults.push_back(&results);
    for(auto & singleResult : results.getResultsReference()){
        if(singleResult.getTeam() != nullptr){
            if(singleResult.getTeam()->getCountryCode() != "")\
                    singleResults.push_back(&singleResult);
        }
        else if(singleResult.getJumper() == jumper)
            singleResults.push_back(&singleResult);
    }
}

void ClassificationSingleResult::updatePointsSum()
{
    pointsSum=0;eligibleParticipation=false;
    if(!classification)return;
    QVector<CompetitionInfo *> events;
    const auto append=[&](CompetitionInfo *event){if(event && !events.contains(event))events.append(event);};
    for(auto *results:competitionsResults)if(results)append(results->getCompetition());
    for(auto *result:singleResults)if(result)append(result->getCompetition());
    for(auto *event:events)for(const auto &c:classification->contributions(event)) {
        const bool matches=classification->getClassificationType()==Classification::Individual?c.jumper==jumper:c.country==teamCode;
        if(matches){pointsSum+=c.points;eligibleParticipation=true;}
    }
}

QJsonObject ClassificationSingleResult::getJsonObject(ClassificationSingleResult &result)
{
    QJsonObject object;
    object.insert("id", result.getIDStr());
    object.insert("classification-id", result.getClassification()->getIDStr());
    if(result.getJumper() != nullptr)
        object.insert("jumper-id", result.getJumper()->getIDStr());
    if(result.getTeamCode() != "")
        object.insert("team-code", result.getTeamCode());

    QJsonArray competitionsResultsArray;
    for(auto & competitionResults : result.getCompetitionsResultsReference())
    {
        competitionsResultsArray.push_back(competitionResults->getIDStr());
    }
    object.insert("competitions-results-ids", competitionsResultsArray);

    object.insert("points-sum", result.getPointsSum());
    object.insert("position", result.getPosition());

    return object;
}

ClassificationSingleResult ClassificationSingleResult::getFromJson(QJsonObject json, IdentifiableObjectsStorage *storage)
{
    ClassificationSingleResult result(nullptr);
    result.setID(sole::rebuild(json.value("id").toString().toStdString()));
    result.setClassification(static_cast<Classification *>(storage->get(json.value("classification-id").toString())));
    result.setJumper(static_cast<Jumper *>(storage->get(json.value("jumper-id").toString())));
    result.setTeamCode(json.value("team-code").toString());

    result.setPointsSum(json.value("points-sum").toDouble());
    result.setPosition(json.value("position").toInt());

    return result;
}

Classification *ClassificationSingleResult::getClassification() const
{
    return classification;
}

void ClassificationSingleResult::setClassification(Classification *newClassification)
{
    classification = newClassification;
}

int ClassificationSingleResult::getPosition() const
{
    return position;
}

void ClassificationSingleResult::setPosition(int newPosition)
{
    position = newPosition;
}

double ClassificationSingleResult::getPointsSum() const
{
    return pointsSum;
}

void ClassificationSingleResult::setPointsSum(double newPointsSum)
{
    pointsSum = newPointsSum;
}

QVector<CompetitionResults *> ClassificationSingleResult::getCompetitionsResults() const
{
    return competitionsResults;
}

QVector<CompetitionResults *> &ClassificationSingleResult::getCompetitionsResultsReference()
{
    return competitionsResults;
}

void ClassificationSingleResult::setCompetitionsResults(const QVector<CompetitionResults *> &newCompetitionsResults)
{
    competitionsResults = newCompetitionsResults;
}

QVector<CompetitionSingleResult *> ClassificationSingleResult::getSingleResults() const
{
    return singleResults;
}

QVector<CompetitionSingleResult *> &ClassificationSingleResult::getSingleResultsReference()
{
    return singleResults;
}

void ClassificationSingleResult::setSingleResults(const QVector<CompetitionSingleResult *> &newSingleResults)
{
    singleResults = newSingleResults;
}

Jumper *ClassificationSingleResult::getJumper() const
{
    return jumper;
}

void ClassificationSingleResult::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

QString ClassificationSingleResult::getTeamCode() const
{
    return teamCode;
}

void ClassificationSingleResult::setTeamCode(const QString &newTeamCode)
{
    teamCode = CountryCodes::toFis(newTeamCode);
}
