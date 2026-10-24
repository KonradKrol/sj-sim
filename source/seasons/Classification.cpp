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
    if(classificationType!=Individual || options.countrySelection==0) return visible;
    const auto better=[](auto *a,auto *b) {
        if(a->getPointsSum()!=b->getPointsSum())return a->getPointsSum()>b->getPointsSum();
        const auto an=a->getJumper()?a->getJumper()->getNameAndSurname():QString();
        const auto bn=b->getJumper()?b->getJumper()->getNameAndSurname():QString();
        if(an!=bn)return an<bn;
        return a->getJumper() && b->getJumper() && a->getJumper()->getIDStr()<b->getJumper()->getIDStr();
    };
    std::stable_sort(visible.begin(),visible.end(),better);
    auto selection=visible;
    if(options.countrySelection==2)std::reverse(selection.begin(),selection.end());
    QHash<QString,int> counts; QSet<ClassificationSingleResult *> accepted;
    for(auto *result:selection) if(auto *jumper=result->getJumper())
        if(counts[jumper->getCountryCode()]++<options.countryCount) accepted.insert(result);
    visible.erase(std::remove_if(visible.begin(),visible.end(),[&](auto *r){return !accepted.contains(r);}),visible.end());
    return visible;
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
    const int year=parameters.value("reference-year").toInt(options.referenceYear);
    const int minimumAge=parameters.value("minimum-age").toInt(options.minimumAge);
    const int maximumAge=parameters.value("maximum-age").toInt(options.maximumAge);
    const int countryCount=parameters.value("country-count").toInt(options.countryCount);
    const bool teamEvent=competition->getRulesPointer()->getCompetitionType()==CompetitionRules::Team;
    const bool alternateScoring=(classificationType==Team)!=teamEvent;
    const auto table=alternateScoring?alternate:normal;
    const auto ageAllowed=[&](Jumper *j) {
        if(!j)return false;
        if(!options.ageFilter)return true;
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
        out.erase(std::remove_if(out.begin(),out.end(),[&](const Contribution &c){return count[c.country]++>=countryCount;}),out.end());
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

QString Classification::getSingleResultsTextForWebhook()
{
    QString s;
    if(classificationType == Individual)
    {
        for(auto *res : getRankedResults())
        {
            if(res->getPointsSum() != 0)
            s += QString::number(res->getPosition()) + ". " + res->getJumper()->getTextForDiscord() + " -> " + QString::number(res->getPointsSum(), 'f', 1) + QObject::tr("pkt") + "\n";
        }
    }
    else
    {
        for(auto *res : getRankedResults())
        {
            if(res->getPointsSum() != 0)
            {
                Country *country = GlobalDatabase::get()->getCountryByAlpha3(res->getTeamCode());
                const QString countryName = country != nullptr ? country->getName() : res->getTeamCode();
                const QString flagCode = country != nullptr ? country->getAlpha2().toLower() : QString();
                s += QString::number(res->getPosition()) + ". " + countryName + QString(" :flag_%1:").arg(flagCode) + " -> " + QString::number(res->getPointsSum(), 'f', 1) + QObject::tr("pkt") + "\n";
            }
        }
    }

    return s;
}

dpp::message Classification::getMessageForResultsWebhook(SimulationSave * save)
{
    dpp::message msg;

    SeasonCalendar * cal = nullptr;
    if(save != nullptr)
    for(auto & season : save->getSeasonsReference())
        for(auto & c : season.getCalendarsReference())
            if(c != nullptr && c->getClassificationsReference().contains(this))
            {
                cal = c;
                break;
            }
    int howMany = 0;
    int played = 0;
    if(cal != nullptr)
    for(auto & comp : cal->getCompetitionsReference())
    {

        if(comp != nullptr && comp->getClassificationsReference().contains(this))
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
    const QString webhookUrl = GlobalAppSettings::get()->getClassificationResultsWebhook().trimmed();
    if(webhookUrl.isEmpty())
        return;
    try
    {
        dpp::message message = getMessageForResultsWebhook(save);
        std::string content = message.content;
        while (!content.empty())
        {
            dpp::cluster bot("");
            dpp::webhook wh(webhookUrl.toStdString());
            std::string newContent = content.substr(0, std::min(2000, int(content.length())));
            content.replace(0, newContent.length(), "");
            bot.execute_webhook(wh, dpp::message(newContent));
        }
    }
    catch(const std::exception &error)
    {
        qWarning() << "Classification webhook failed:" << error.what();
    }
    catch(...)
    {
        qWarning() << "Classification webhook failed with an unknown error";
    }
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
