 #include "Classification.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QJsonValue>
#include "../seasons/Season.h"
#include "../seasons/SimulationSave.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../global/GlobalDatabase.h"
#include "../global/MyRandom.h"
#include "../global/GlobalAppSettings.h"
#include <QtConcurrent>

extern const QString appVersion;

Classification::Classification(QString name) : name(name),
    Identifiable()
{
    classificationType = 0;
    punctationType = 0;
}

Classification::~Classification()
{

}

void Classification::updatePositions()
{
    double previousResultPoints = 0;
    int actualPosition = 1;
    int add = 1;
    int i=0;
    for(auto & result : results){
        if(i==0){
            result->setPosition(1);
            previousResultPoints = result->getPointsSum();
            i++;
            continue;
        }

        if(previousResultPoints == result->getPointsSum())
        {
            result->setPosition(actualPosition);
            add += 1;
        }
        else{
            actualPosition += add;
            add = 1;
            result->setPosition(actualPosition);
        }

        previousResultPoints = result->getPointsSum();
        i++;
    }
}

void Classification::sortInDescendingOrder()
{
    std::stable_sort(results.begin(), results.end(), comparePointers);
    updatePositions();
}

void Classification::sortInAscendingOrder()
{
    //std::sort(results.begin(), results.end(), std::less<ClassificationSingleResult *>());
    updatePositions();
}


ClassificationSingleResult *Classification::getResultOfIndividualJumper(Jumper *jumper)
{
    for(auto & result : results)
    {
        if(result->getJumper() == jumper){
            return result;
        }
    }
    return nullptr;
}

ClassificationSingleResult *Classification::getResultOfTeam(QString teamCode)
{
    for(auto & result : results)
    {
        if(result->getTeamCode() == teamCode){
            return result;
        }
    }
    return nullptr;
}

ClassificationSingleResult *Classification::getResultByIndex(int index)
{
    return const_cast<ClassificationSingleResult *>(results[index]);
}

QVector<Classification *> Classification::getSpecificTypeClassifications(QVector<Classification *> classifications, int type)
{
    QVector<Classification *> toReturn;
    for(auto & classification : classifications)
    {
        if(classification->getClassificationType() == type)
            toReturn.push_back(classification);
    }
    return toReturn;
}

QMap<int, double> Classification::getAltPointsForPlaces() const
{
    return altPointsForPlaces;
}

QMap<int, double> &Classification::getAltPointsForPlacesReference()
{
    return altPointsForPlaces;
}

void Classification::setAltPointsForPlaces(const QMap<int, double> &newAltPointsForPlaces)
{
    altPointsForPlaces = newAltPointsForPlaces;
}

Classification * Classification::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage, int * oldId)
{
    Classification * classification = new Classification();
    if(oldId != nullptr)
    {
        *oldId = obj.value("id").toString().toULong();
    }
    classification->setID(sole::rebuild(obj.value("id").toString().toStdString()));
    classification->setName(obj.value("name").toString());
    classification->setClassificationType(obj.value("classification-type").toInt());
    classification->setPunctationType(obj.value("punctation-type").toInt());
    QJsonArray array = obj.value("points-for-places").toArray();
    int i=1;
    for(auto val : array){
        classification->getPointsForPlacesReference().insert(i, val.toDouble());
        i++;
    }
    QJsonArray altArray = obj.value("alt-points-for-places").toArray();
    int ii=1;
    for(auto val : altArray){
        classification->getAltPointsForPlacesReference().insert(ii, val.toDouble());
        ii++;
    }

    QJsonArray resultsArray = obj.value("results").toArray();
    QVector<ClassificationSingleResult *> results;
    for(const auto &value : resultsArray)
        results.append(new ClassificationSingleResult(ClassificationSingleResult::getFromJson(value.toObject(), storage)));
    classification->setResults(results);
    for(auto & res : classification->getResultsReference())
        res->setClassification(classification);

    storage->add(classification->getResultsReference());

    return classification;
}

QJsonObject Classification::getJsonObject(Classification * classification)
{
    qDebug()<<"classification name: "<<classification->getName();
    QJsonObject obj;
    obj.insert("id", classification->getIDStr());
    obj.insert("name", classification->getName());
    obj.insert("classification-type", classification->getClassificationType());
    obj.insert("punctation-type", classification->getPunctationType());

    QJsonArray pointsForPlacesArray;
    for(int i=1; i<=classification->getPointsForPlacesReference().count(); i++){
        pointsForPlacesArray.push_back(classification->getPointsForPlacesReference().value(i));
    }
    obj.insert("points-for-places", pointsForPlacesArray);

    QJsonArray altPointsForPlacesArray;
    for(int i=1; i<=classification->getAltPointsForPlacesReference().count(); i++){
        altPointsForPlacesArray.push_back(classification->getAltPointsForPlacesReference().value(i));
    }
    obj.insert("alt-points-for-places", altPointsForPlacesArray);

    QJsonArray resultsArray;
    for(auto *result : classification->getResultsReference())
        resultsArray.append(ClassificationSingleResult::getJsonObject(*result));
    obj.insert("results", resultsArray);
    qDebug()<<"cls return";
    return obj;
}

QHash<QString, QHash<CompetitionInfo *, int> > Classification::constructTeamsArchiveResults(Season *season, SeasonCalendar *calendar)
{
    QHash<QString, QHash<CompetitionInfo *, int>> results;
    QHash<QString, double> tempResultsToSort;

    for(auto & seasonCompetition : calendar->getCompetitionsReference())
    {
        if(seasonCompetition->getPlayed() == true)
        {
            if(seasonCompetition->getClassificationsReference().contains(this) == true)
            {
                QHash<QString, int> poss;
                QSet<QString> remainingTeams;
                if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
                {
                    QVector<QString> teams = CompetitionResults::getTeamsCodesByIndividualResults(&seasonCompetition->getResultsReference());
                    for(auto & teamCode : teams)
                    {
                        double teamResultPoints = 0;
                        for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                        {
                            if(singleResult.getJumper()->getCountryCode() == teamCode){
                                if(this->punctationType == Classification::PointsForPlaces)
                                    teamResultPoints += this->altPointsForPlaces.value(singleResult.getPosition());
                                else
                                    teamResultPoints += singleResult.getPointsSum();
                            }
                        }
                        tempResultsToSort[teamCode] = tempResultsToSort.value(teamCode) + teamResultPoints;
                        remainingTeams.insert(teamCode);
                    }
                    poss = CompetitionResults::getResultsWithTeamsPositionsForClassificationArchiveResults(tempResultsToSort);

                    for(auto & teamCode : teams)
                    {
                        for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                        {
                            if(teamCode == singleResult.getJumper()->getCountryCode()){
                                QHash<CompetitionInfo *, int> resultsMap;
                                if(results.contains(teamCode))
                                {
                                    resultsMap = results.value(teamCode);
                                }
                                else
                                {
                                    resultsMap = QHash<CompetitionInfo *, int>();
                                }
                                resultsMap.insert(seasonCompetition, poss.value(teamCode));

                                results[teamCode] = resultsMap;
                                if(remainingTeams.contains(teamCode))
                                    remainingTeams.remove(teamCode);
                            }
                        }
                    }
                }
                else
                {
                    for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                    {
                        double teamResultPoints = 0;
                        if(this->punctationType == Classification::PointsForPlaces)
                            teamResultPoints += this->pointsForPlaces.value(singleResult.getPosition());
                        else
                            teamResultPoints += singleResult.getPointsSum();
                        tempResultsToSort[singleResult.getTeam()->getCountryCode()] = tempResultsToSort.value(singleResult.getTeam()->getCountryCode()) + teamResultPoints;
                        remainingTeams.insert(singleResult.getTeam()->getCountryCode());
                    }
                    poss = CompetitionResults::getResultsWithTeamsPositionsForClassificationArchiveResults(tempResultsToSort);
                    for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                    {
                        QHash<CompetitionInfo *, int> resultsMap;
                        if(results.contains(singleResult.getTeam()->getCountryCode()))
                        {
                            resultsMap = results.value(singleResult.getTeam()->getCountryCode());
                        }
                        else
                        {
                            resultsMap = QHash<CompetitionInfo *, int>();
                        }
                        resultsMap.insert(seasonCompetition, poss.value(singleResult.getTeam()->getCountryCode()));

                        results[singleResult.getTeam()->getCountryCode()] = resultsMap;
                        if(remainingTeams.contains(singleResult.getTeam()->getCountryCode()))
                            remainingTeams.remove(singleResult.getTeam()->getCountryCode());
                    }
                }
                //After else
                for(auto & remainingTeam : remainingTeams)
                {
                    QHash<CompetitionInfo *, int> resultsMap;
                    if(results.contains(remainingTeam))
                    {
                        resultsMap = results.value(remainingTeam);
                    }
                    else
                    {
                        resultsMap = QHash<CompetitionInfo *, int>();
                    }
                    resultsMap.insert(seasonCompetition, poss.value(remainingTeam));
                    results[remainingTeam] = resultsMap;
                }
            }
        }
        else break;
    }

    return results;
}

QString Classification::getSingleResultsTextForWebhook()
{
    QString s;
    if(classificationType == Individual)
    {
        for(auto & res : results)
        {
            if(res->getPointsSum() != 0)
            s += QString::number(res->getPosition()) + ". " + res->getJumper()->getTextForDiscord() + " -> " + QString::number(res->getPointsSum(), 'f', 1) + QObject::tr("pkt") + "\n";
        }
    }
    else
    {
        for(auto & res : results)
        {
            if(res->getPointsSum() != 0)
            s += QString::number(res->getPosition()) + ". " + GlobalDatabase::get()->getCountryByAlpha3(res->getTeamCode())->getName() + QString(" :flag_%1:").arg(GlobalDatabase::get()->getCountryByAlpha3(res->getTeamCode())->getAlpha2().toLower()) + " -> " + QString::number(res->getPointsSum(), 'f', 1) + QObject::tr("pkt") + "\n";
        }
    }

    return s;
}

dpp::message Classification::getMessageForResultsWebhook(SimulationSave * save)
{
    dpp::message msg;

    SeasonCalendar * cal;
    for(auto & season : save->getSeasonsReference())
        for(auto & c : season.getCalendarsReference())
            if(c->getClassificationsReference().contains(this))
            {
                cal = c;
                break;
            }
    int howMany = 0;
    int played;
    for(auto & comp : cal->getCompetitionsReference())
    {

        if(comp->getClassificationsReference().contains(this))
        {
            howMany++;
            if(comp->getPlayed() == true)
                played++;
        }
    }

    QString typeText;
    if(classificationType == Individual)
        typeText += QObject::tr("(indywidualna)");
    else
        typeText += QObject::tr("(drużynowa)");
    msg.content = "**" + name.toStdString() + " " + typeText.toStdString() + "**\n";
    msg.content += QObject::tr("*Po %1 z %2 konkursów*").arg(QString::number(played)).arg(QString::number(howMany)).toStdString() + "\n";
    msg.content += QObject::tr("__Wyniki:__\n").toStdString();
    msg.content += getSingleResultsTextForWebhook().toStdString();
    msg.content += QObject::tr("\n\n*Wiadomość wysłana z poziomu Sj.Sim").toStdString() + appVersion.toStdString() + "*";

    return msg;
}

void Classification::sendResultsWebhook(SimulationSave * save)
{
    dpp::message message = getMessageForResultsWebhook(save);
    std::string content = message.content;
    int i = 0;
    while (!content.empty())
    {
        dpp::cluster bot("");
        dpp::webhook wh(GlobalAppSettings::get()->getCompetitionResultsWebhook().toStdString());
        std::string newContent = content.substr(0, std::min(2000, int(content.length())));
        content.replace(0, newContent.length(), "");
        bot.execute_webhook(wh, dpp::message(newContent));
        i++;
    }
}

QHash<Jumper *, QHash<CompetitionInfo *, int>> Classification::constructJumpersArchiveResults(Season *season, SeasonCalendar * calendar)
{
    QHash<Jumper *, QHash<CompetitionInfo *, int>> results;
    QHash<Jumper *, double> tempResultsToSort;

    for(auto & seasonCompetition : calendar->getCompetitionsReference())
    {
        if(seasonCompetition->getPlayed() == true)
        {
            if(seasonCompetition->getClassificationsReference().contains(this) == true)
            {
                QHash<Jumper *, int> poss;
                QSet<Jumper *> remainingJumpers;
                if(seasonCompetition->getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
                {
                    QVector<Jumper *> jumpers = CompetitionResults::getJumpersByTeamResults(&seasonCompetition->getResultsReference());
                    for(auto & jumper : jumpers){
                        double jumperResultPoints = 0;
                        for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                        {
                            if(singleResult.getTeam()->getCountryCode() == jumper->getCountryCode())
                            {
                                if(this->punctationType == Classification::PointsForPlaces)
                                    jumperResultPoints += this->altPointsForPlaces.value(singleResult.getPosition());
                                else
                                    jumperResultPoints += singleResult.getTeamJumperResult(jumper)->getPointsSum();
                            }
                        }
                        tempResultsToSort[jumper] = tempResultsToSort.value(jumper) + jumperResultPoints;
                        remainingJumpers.insert(jumper);
                    }
                    poss = CompetitionResults::getResultsWithJumpersPositionsForClassificationArchiveResults(tempResultsToSort);
                    for(auto & jumper : jumpers)
                    {
                        for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                        {
                            if(jumper->getCountryCode() == singleResult.getTeam()->getCountryCode()){
                                QHash<CompetitionInfo *, int> resultsMap;
                                if(results.contains(jumper))
                                {
                                    resultsMap = results.value(jumper);
                                }
                                else
                                {
                                    resultsMap = QHash<CompetitionInfo *, int>();
                                }
                                resultsMap.insert(seasonCompetition, poss.value(jumper));

                                results[jumper] = resultsMap;
                                if(remainingJumpers.contains(jumper))
                                    remainingJumpers.remove(jumper);
                            }
                        }
                    }
                }
                else{
                    for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                    {
                        double jumperResultPoints = 0;
                        if(this->punctationType == Classification::PointsForPlaces)
                            jumperResultPoints += this->pointsForPlaces.value(singleResult.getPosition());
                        else
                            jumperResultPoints += singleResult.getPointsSum();

                        tempResultsToSort[singleResult.getJumper()] = tempResultsToSort.value(singleResult.getJumper()) + jumperResultPoints;
                        remainingJumpers.insert(singleResult.getJumper());
                    }
                    poss = CompetitionResults::getResultsWithJumpersPositionsForClassificationArchiveResults(tempResultsToSort);
                    for(auto & singleResult : seasonCompetition->getResultsReference().getResultsReference())
                    {
                        QHash<CompetitionInfo *, int> resultsMap;
                        if(results.contains(singleResult.getJumper()))
                        {
                            resultsMap = results.value(singleResult.getJumper());
                        }
                        else
                        {
                            resultsMap = QHash<CompetitionInfo *, int>();
                        }
                        resultsMap.insert(seasonCompetition, poss.value(singleResult.getJumper()));

                        results[singleResult.getJumper()] = resultsMap;
                        if(remainingJumpers.contains(singleResult.getJumper()))
                            remainingJumpers.remove(singleResult.getJumper());
                    }
                }
                //After else
                for(auto & remainingJumper : remainingJumpers)
                {
                    QHash<CompetitionInfo *, int> resultsMap;
                    if(results.contains(remainingJumper))
                    {
                        resultsMap = results.value(remainingJumper);
                    }
                    else
                    {
                        resultsMap = QHash<CompetitionInfo *, int>();
                    }
                    resultsMap.insert(seasonCompetition, poss.value(remainingJumper));
                    results[remainingJumper] = resultsMap;
                }
            }
        }
        else break;
    }

    return results;
}

QVector<ClassificationSingleResult *> Classification::getResults() const
{
    return results;
}

QVector<ClassificationSingleResult *> &Classification::getResultsReference()
{
    return results;
}

void Classification::setResults(const QVector<ClassificationSingleResult *> &newResults)
{
    results = newResults;
}

short Classification::getPunctationType() const
{
    return punctationType;
}

void Classification::setPunctationType(short newPunctationType)
{
    punctationType = newPunctationType;
}

QString Classification::getName() const
{
    return name;
}

void Classification::setName(const QString &newName)
{
    name = newName;
}

QMap<int, double> Classification::getPointsForPlaces() const
{
    return pointsForPlaces;
}

QMap<int, double> &Classification::getPointsForPlacesReference()
{
    return pointsForPlaces;
}

void Classification::setPointsForPlaces(const QMap<int, double> &newPointsForPlaces)
{
    pointsForPlaces = newPointsForPlaces;
}

short Classification::getClassificationType() const
{
    return classificationType;
}

void Classification::setClassificationType(short newClassificationType)
{
    classificationType = newClassificationType;
}
