#include "ClassificationSingleResult.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../seasons/Classification.h"
#include "../competitions/CompetitionInfo.h"

ClassificationSingleResult::ClassificationSingleResult(Classification *classification, Jumper * jumper) : jumper(jumper), position(0), pointsSum(0), classification(classification)
{

}

ClassificationSingleResult::ClassificationSingleResult(Classification *classification, QString teamCode) : teamCode(teamCode), position(0), pointsSum(0), classification(classification), jumper(nullptr)
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
                        if(singleResult.getTeam()->getCountryCode() == this->getJumper()->getCountryCode()){
                            for(auto & jumperResult : singleResult.getTeamJumpersResultsReference()){
                                if(jumperResult.getJumper() == this->getJumper()){
                                    singleResults.push_back(&jumperResult);
                                    //break;
                                }
                            }
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
    pointsSum = 0;
    for(auto & result : singleResults)
    {
        if(classification->getPunctationType() == Classification::CompetitionPoints)
            pointsSum += result->getPointsSum();
        else if(classification->getPunctationType() == Classification::PointsForPlaces)
        {
            if(classification->getClassificationType() == Classification::Individual)
            {
                if(result->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
                {
                    pointsSum += classification->getPointsForPlacesReference().value(result->getPosition(), 0);
                }
                else if(result->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
                {
                    pointsSum += classification->getAltPointsForPlacesReference().value(result->getPosition(), 0);
                }
            }
            else if(classification->getClassificationType() == Classification::Team)
            {
                if(result->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
                {
                    pointsSum += classification->getAltPointsForPlacesReference().value(result->getPosition(), 0);
                }
                else if(result->getCompetition()->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
                {
                    pointsSum += classification->getPointsForPlacesReference().value(result->getPosition(), 0);
                }
            }
        }
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
    teamCode = newTeamCode;
}
