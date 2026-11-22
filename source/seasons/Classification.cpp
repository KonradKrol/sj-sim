 #include "Classification.h"
#include "../global/CountryCodes.h"

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
#include <QDebug>
#include <exception>
#include <algorithm>
#include <QSet>

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
    for(auto *result:results) result->setPosition(0);
    double previous=0; int rank=0, row=0;
    for(auto *result:getRankedResults()) {
        ++row;
        if(row==1 || result->getPointsSum()!=previous) rank=row;
        result->setPosition(rank); previous=result->getPointsSum();
    }
}

void Classification::copyConfiguration(const Classification &source)
{
    name=source.name; classificationType=source.classificationType;
    punctationType=source.punctationType; pointsForPlaces=source.pointsForPlaces;
    altPointsForPlaces=source.altPointsForPlaces; options=source.options;
}

QVector<ClassificationSingleResult *> Classification::getRankedResults() const
{
    QVector<ClassificationSingleResult *> visible;
    for(auto *result:results)
        if(result && (!options.ageFilter || result->hasEligibleParticipation())) visible.append(result);
    return visible;
}

void Classification::updatePointsSums()
{
    // The cache lives only for this recalculation; edits cannot leave stale scores.
    QHash<CompetitionInfo *,QVector<Contribution>> cache;
    for(auto *row:results) {
        if(!row)continue;
        row->pointsSum=0;row->eligibleParticipation=false;
        QVector<CompetitionInfo *> events;
        const auto append=[&](CompetitionInfo *event){if(event && !events.contains(event))events.append(event);};
        for(auto *result:row->competitionsResults)if(result)append(result->getCompetition());
        for(auto *result:row->singleResults)if(result)append(result->getCompetition());
        for(auto *event:events) {
            if(!cache.contains(event))cache.insert(event,contributions(event));
            for(const auto &c:cache[event]) {
                const bool matches=classificationType==Individual?c.jumper==row->jumper:c.country==row->teamCode;
                if(matches){row->pointsSum+=c.points;row->eligibleParticipation=true;}
            }
        }
    }
}

QVector<Classification::Contribution> Classification::contributions(CompetitionInfo *competition) const
{
    QVector<Contribution> out;
    if(!competition || competition->getCancelled()) return out;
    const auto settings=competition->scoringFor(this);
    int type=punctationType;
    auto normal=pointsForPlaces, alternate=altPointsForPlaces;
    for(const auto &method:options.methods) if(method.id==settings.methodId) {
        type=method.type; normal=method.points; alternate=method.alternativePoints; break;
    }
    const auto parameters=options.contextParameters?settings.parameters:QJsonObject();
    const double multiplier=parameters.value("multiplier").toDouble(1);
    const int pointsLimit=parameters.value("points-limit").toInt(0);
    const int year=options.referenceYear;
    const int minimumAge=qBound(1,parameters.value("minimum-age").toInt(options.minimumAge),100);
    const int maximumAge=qBound(minimumAge,parameters.value("maximum-age").toInt(options.maximumAge),100);
    const int countryCount=qBound(1,parameters.value("country-count").toInt(options.countryCount),10000);
    const bool teamEvent=competition->getRulesPointer()->getCompetitionType()==CompetitionRules::Team;
    const bool alternateScoring=(classificationType==Team)!=teamEvent;
    const auto table=alternateScoring?alternate:normal;
    const auto ageAllowed=[&](Jumper *j) {
        if(!j)return false;
        if(!options.ageFilter)return true;
        if(j->getBirthYear()>year)return false;
        const int age=j->ageInYear(year);
        return age<0?options.includeUnknownAge:age>=minimumAge && age<=maximumAge;
    };
    const auto points=[&](CompetitionSingleResult *r,int place) {
        if(pointsLimit>0 && place>pointsLimit)return 0.;
        return (type==CompetitionPoints?r->getPointsSum():table.value(place))*multiplier;
    };
    for(auto &result:competition->getResultsReference().getResultsReference()) {
        if(result.isTeamDisqualified())continue;
        if(!teamEvent) {
            auto *j=result.getJumper(); if(!ageAllowed(j))continue;
            out.append({j,j->getCountryCode(),points(&result,result.getPosition()),&result});
        } else if(classificationType==Individual) {
            if(!result.getTeam())continue;
            for(auto *j:result.getTeam()->getJumpersReference()) {
                if(!ageAllowed(j))continue;
                auto *member=type==CompetitionPoints?result.getTeamJumperResult(j):&result;
                if(member) out.append({j,result.getTeam()->getCountryCode(),points(member,result.getPosition()),member});
            }
        } else {
            if(!result.getTeam())continue;
            const auto members=result.getTeam()->getJumpers();
            if(options.ageFilter && (members.isEmpty() || !std::all_of(members.cbegin(),members.cend(),ageAllowed)))continue;
            out.append({nullptr,result.getTeam()->getCountryCode(),points(&result,result.getPosition()),&result});
        }
    }
    if(classificationType==Team && !teamEvent && options.countrySelection!=0) {
        std::stable_sort(out.begin(),out.end(),[&](const Contribution &a,const Contribution &b) {
            if(a.result->getPosition()!=b.result->getPosition())return options.countrySelection==1?a.result->getPosition()<b.result->getPosition():a.result->getPosition()>b.result->getPosition();
            if(a.result->getPointsSum()!=b.result->getPointsSum())return options.countrySelection==1?a.result->getPointsSum()>b.result->getPointsSum():a.result->getPointsSum()<b.result->getPointsSum();
            const auto an=a.jumper->getNameAndSurname(),bn=b.jumper->getNameAndSurname();
            if(an!=bn)return an<bn;
            return a.jumper->getIDStr()<b.jumper->getIDStr();
        });
        QHash<QString,int> count;
        out.erase(std::remove_if(out.begin(),out.end(),[&](const Contribution &c){const bool selected=count[c.country]++<countryCount;return options.countryExclude?selected:!selected;}),out.end());
    }
    return out;
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
    teamCode = CountryCodes::toFis(teamCode);
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
    return getRankedResults().value(index,nullptr);
}

QVector<Classification *> Classification::getSpecificTypeClassifications(QVector<Classification *> classifications, int type)
{
    QVector<Classification *> toReturn;
    for(auto & classification : classifications)
    {
        if(classification && classification->getClassificationType() == type)
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
    classification->setOptions(ClassificationOptions::fromJson(obj.value("scoring-options").toObject()));
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
        results.append(classification->ownResult(std::make_unique<ClassificationSingleResult>(ClassificationSingleResult::getFromJson(value.toObject(), storage))));
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
    obj.insert("scoring-options",classification->getOptions().toJson());

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

QHash<QString,QHash<CompetitionInfo *,int>> Classification::constructTeamsArchiveResults(Season *,SeasonCalendar *calendar)
{
    QHash<QString,QHash<CompetitionInfo *,int>> archive;
    QHash<QString,double> totals;
    for(auto *event:calendar->getCompetitionsReference()) {
        if(!event->getPlayed())break;
        if(!event->getClassificationsReference().contains(this) || event->getCancelled())continue;
        for(const auto &c:contributions(event))totals[c.country]+=c.points;
        QStringList countries=totals.keys();std::sort(countries.begin(),countries.end(),[&](const QString &a,const QString &b){return totals[a]==totals[b]?a<b:totals[a]>totals[b];});
        double previous=0;int place=0,row=0;
        for(const auto &country:countries){++row;if(row==1 || totals[country]!=previous)place=row;archive[country].insert(event,place);previous=totals[country];}
    }
    return archive;
}

QHash<Jumper *,QHash<CompetitionInfo *,int>> Classification::constructJumpersArchiveResults(Season *,SeasonCalendar *calendar)
{
    QHash<Jumper *,QHash<CompetitionInfo *,int>> archive;
    QHash<Jumper *,double> totals;
    for(auto *event:calendar->getCompetitionsReference()) {
        if(!event->getPlayed())break;
        if(!event->getClassificationsReference().contains(this) || event->getCancelled())continue;
        for(const auto &c:contributions(event))if(c.jumper)totals[c.jumper]+=c.points;
        Classification ranking(*this);ranking.setResults({});
        QVector<ClassificationSingleResult> rows;rows.reserve(totals.size());
        for(auto it=totals.begin();it!=totals.end();++it){rows.append(ClassificationSingleResult(&ranking,it.key()));rows.last().setPointsSum(it.value());}
        for(auto &row:rows)ranking.getResultsReference().append(&row);
        ranking.sortInDescendingOrder();
        for(auto *row:ranking.getRankedResults())archive[row->getJumper()].insert(event,row->getPosition());
    }
    return archive;
}

QVector<ClassificationSingleResult *> Classification::getResults() const
{
    return getRankedResults();
}

QVector<ClassificationSingleResult *> &Classification::getResultsReference()
{
    return results;
}

void Classification::setResults(const QVector<ClassificationSingleResult *> &newResults)
{
    ownedResults.erase(std::remove_if(ownedResults.begin(),ownedResults.end(),[&](const auto &row){return !newResults.contains(row.get());}),ownedResults.end());
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

ClassificationSingleResult *Classification::ownResult(std::unique_ptr<ClassificationSingleResult> result)
{
    auto *pointer=result.get();ownedResults.emplace_back(std::move(result));return pointer;
}
