#include "SeasonCalendar.h"
#include "../utilities/functions.h"
#include "../global/IdentifiableObjectsStorage.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QHash>
#include <QtConcurrent>

SeasonCalendar::SeasonCalendar(QString name) : name(name)
{

}

SeasonCalendar::~SeasonCalendar()
{
}

void SeasonCalendar::debugCalendar(CompetitionInfo * next) const
{
    qDebug()<<"-----------"<<name<<"-----------";
    int i = 0;
    for(auto & comp : competitions)
    {
        QString s = "";
        if(comp->getTrainingsReference().count() > 0)
        {
            s = QString("%1 treningów").arg(comp->getTrainingsReference().count());
        }
        if(comp == next)
        {
            s += " (NASTĘPNY)";
        }
        qDebug()<<QString::number(i)<<". "<<comp->getShortSerieTypeText()<<"  "<<comp->getHill()->getHillText()<<s<<" ("<<comp<<")";
        i++;
    }
}

void SeasonCalendar::fixCompetitionsClassifications()
{
    for(auto & comp : competitions){
        int type = comp->getRulesPointer()->getCompetitionType();
        for(auto & classification : comp->getClassificationsReference()){
            if(classification == nullptr)
                MyFunctions::removeFromVector(comp->getClassificationsReference(), classification);
            else if(MyFunctions::vectorContainsByID(classifications, classification) == false){// || ((type != classification->getClassificationType()) && classification->getPunctationType() == Classification::PointsForPlaces && classification->getClassificationType() == Classification::Individual)){
                MyFunctions::removeFromVectorByID(comp->getClassificationsReference(), classification->getID());
            }
        }
    }
}

void SeasonCalendar::fixCompetitionsHills(QVector<Hill *> *hillsList, Hill * defaultHill)
{
    for(auto & comp : competitions){
        if(comp->getHill() == nullptr)
        {
            comp->setHill(defaultHill);
            continue;
        }
        sole::uuid hillID = comp->getHill()->getID();
        bool contains = false;
        Hill * hillWhichContains = nullptr;
        for(auto & hill : qAsConst(*hillsList)){
            if(hill->getID() == hillID){
                contains = true;
                hillWhichContains = hill;
                break;
            }
        }
        if(contains){
            comp->setHill(hillWhichContains);
        }
        else{
            comp->setHill(defaultHill);
        }
    }
}

void SeasonCalendar::fixAdvancementCompetitions()
{
    for(auto & comp : competitions)
    {
        if(comp->getAdvancementCompetition() != nullptr)
        {
            if(MyFunctions::vectorContains(competitions, comp->getAdvancementCompetition()) == false
                || comp->getAdvancementCompetition()->getRulesPointer()->getCompetitionType() != comp->getRulesPointer()->getCompetitionType())
            {
                comp->setAdvancementCompetition(nullptr);
            }
        }
    }
}

void SeasonCalendar::fixAdvancementClassifications()
{
    for(auto & comp : competitions)
    {
        if(comp->getAdvancementClassification() != nullptr)
        {
            if(MyFunctions::vectorContains(classifications, comp->getAdvancementClassification()) == false
                || comp->getAdvancementClassification()->getClassificationType() != comp->getRulesPointer()->getCompetitionType())
            {
                comp->setAdvancementClassification(nullptr);
            }
        }
    }
}

void SeasonCalendar::updateCompetitionsQualifyingCompetitions()
{
    for(auto & comp : competitions)
    {
        comp->updateQualifyingCompetitions(this);
    }
}

SeasonCalendar SeasonCalendar::getFromJson(QJsonObject json, IdentifiableObjectsStorage * storage, QMap<ulong, Identifiable*> * before120Map)
{
    SeasonCalendar calendar;
    calendar.setID(sole::rebuild(json.value("id").toString().toStdString()));
    calendar.setName(json.value("name").toString());

    QJsonArray classificationsArray = json.value("classifications").toArray();
    for(auto val : classificationsArray){
        int cId = 0;
        Classification * c = Classification::getFromJson(val.toObject(), storage, &cId);
        if(before120Map != nullptr){
            qDebug()<<"BEFORE 120 MAP";
            c->reassign();
            before120Map->insert(cId, c);
        }
        calendar.getClassificationsReference().push_back(c);
    }
    if(storage != nullptr)
        storage->add(calendar.getClassificationsReference());

    QJsonArray competitionsArray = json.value("competitions").toArray();
    QHash<QString, CompetitionInfo *> competitionsById;
    QHash<QString, Classification *> classificationsById;
    QHash<QString, CompetitionResults *> resultsById;
    for(int index = 0; index < classificationsArray.count(); ++index)
        classificationsById.insert(classificationsArray.at(index).toObject().value("id").toString(),
                                   calendar.getClassificationsReference().at(index));

    // Phase 1: construct every competition before resolving cross-references.
    // This keeps storage entries pointed at stable heap-owned objects.
    for(auto val : competitionsArray){
        int cId = 0;
        CompetitionInfo * c = new CompetitionInfo(CompetitionInfo::getFromJson(val.toObject(), storage, &cId, before120Map));
        QString s = "";
        if(!c->getTrainingsReference().isEmpty())
        {
            s = QString(" (%1 treningów)").arg(QString::number(c->getTrainingsReference().count()));
        }
        qDebug()<<c->getShortSerieTypeText()<<s;

        if(before120Map != nullptr)
        {
            c->reassign();
            before120Map->insert(cId, c);
        }
        calendar.getCompetitionsReference().push_back(c);
        competitionsById.insert(val.toObject().value("id").toString(), c);
        resultsById.insert(val.toObject().value("results").toObject().value("id").toString(),
                           &c->getResultsReference());
        if(storage != nullptr) {
            storage->add(c);
            storage->add(c->getTeamsReference());
            for(auto &groups : c->getRoundsKOGroupsReference())
                storage->add(groups);
            storage->add(&c->getResultsReference());
            storage->add(c->getResultsReference().getResultsReference());
        }
    }

    // Phase 2: every target now exists, so forward links (such as a
    // qualification's later trainings) resolve without introducing nullptrs.
    for(int index = 0; index < competitionsArray.count(); ++index)
    {
        const QJsonObject competitionJson = competitionsArray.at(index).toObject();
        CompetitionInfo *competition = calendar.getCompetitionsReference().at(index);

        competition->setTrialRound(
            competitionsById.value(competitionJson.value("trial-round-id").toString(), nullptr));

        QVector<CompetitionInfo *> trainings;
        for(const auto &trainingId : competitionJson.value("training-ids").toArray())
            if(CompetitionInfo *training = competitionsById.value(trainingId.toString(), nullptr))
                trainings.push_back(training);
        competition->setTrainings(trainings);

        QVector<Classification *> classifications;
        for(const auto &classificationId : competitionJson.value("classifications-ids").toArray())
            if(Classification *classification = classificationsById.value(classificationId.toString(), nullptr))
                classifications.push_back(classification);
        competition->setClassifications(classifications);

        QVector<Jumper *> startList;
        for(const auto &jumperId : competitionJson.value("start-list").toArray())
            if(Jumper *jumper = static_cast<Jumper *>(storage->get(jumperId.toString())))
                startList.push_back(jumper);
        competition->setStartList(startList);

        competition->setAdvancementClassification(classificationsById.value(
            competitionJson.value("advancement-classification-id").toString(), nullptr));
        competition->setAdvancementCompetition(competitionsById.value(
            competitionJson.value("advancement-competition-id").toString(), nullptr));

        CompetitionResults &results = competition->getResultsReference();
        results.setCompetition(competition);
        for(auto &singleResult : results.getResultsReference()) {
            singleResult.setCompetition(competition);
            for(auto &jump : singleResult.getJumpsReference()) {
                jump.setCompetition(competition);
                jump.setSingleResult(&singleResult);
            }
            singleResult.updateTeamJumpersResults();
        }
    }
    calendar.updateCompetitionsQualifyingCompetitions();

    if(storage != nullptr){
        for(int index = 0; index < classificationsArray.count(); ++index){
            const QJsonObject classificationJson = classificationsArray.at(index).toObject();
            Classification *classification = calendar.getClassificationsReference().at(index);
            for(auto & singleResult : classification->getResultsReference())
            {
                singleResult->setClassification(classification);
                QJsonArray singleResultsArray = classificationJson.value("results").toArray();
                for(auto jsonRes : singleResultsArray)
                {
                    if(sole::rebuild(jsonRes.toObject().value("id").toString().toStdString()) == singleResult->getID())
                    {
                        QJsonArray compsIds = jsonRes.toObject().value("competitions-results-ids").toArray();
                        for(auto id : compsIds)
                            if(CompetitionResults *results = resultsById.value(id.toString(), nullptr))
                                singleResult->getCompetitionsResultsReference().push_back(results);
                    }
                }
                singleResult->updateSingleResults();
                singleResult->updatePointsSum();
            }
            classification->sortInDescendingOrder();
        }
    }

    return calendar;
}

QJsonObject SeasonCalendar::getJsonObject(SeasonCalendar &calendar)
{
    QJsonObject object;
    object.insert("id", calendar.getIDStr());
    object.insert("name", calendar.getName());

    QJsonArray classificationsArray;
    for(auto & cls : qAsConst(calendar.getClassificationsReference()))
        classificationsArray.push_back(Classification::getJsonObject(cls));
    object.insert("classifications", classificationsArray);

    QJsonArray competitionsArray;
    for(auto & cmp : qAsConst(calendar.getCompetitionsReference()))
        competitionsArray.push_back(CompetitionInfo::getJsonObject(*cmp));
    object.insert("competitions", competitionsArray);

    /*QFuture<QJsonObject> classificationsFuture = QtConcurrent::mapped(calendar.getClassifications(), [](Classification * p){return Classification::getJsonObject(p);});
    QJsonArray classificationsArray;
    for(auto & o : classificationsFuture.results())
        classificationsArray.append(o);
    object.insert("classifications", classificationsArray);*.

    /*QFuture<QJsonObject> competitionsFuture = QtConcurrent::mapped(calendar.getCompetitions(), [](CompetitionInfo * p){return CompetitionInfo::getJsonObject(*p);});
    QJsonArray competitionsArray;
    for(auto & o : competitionsFuture.results())
        competitionsArray.append(o);
    object.insert("competitions", competitionsArray);*/

    return object;
}

int SeasonCalendar::getCompetitionMainIndex(QVector<CompetitionInfo *> &competitions, CompetitionInfo *competition)
{
    int index = 0;
    for(auto & comp : competitions)
    {
        if(comp == competition)
            return index;
        if(comp->getSerieType() == CompetitionInfo::Competition || comp->getSerieType() == CompetitionInfo::Qualifications)
            index++;
    }
    return index;
}

CompetitionInfo *SeasonCalendar::getMainCompetitionByIndex(QVector<CompetitionInfo *> &competitions, int index)
{
    int i=0;
    for(auto & comp : competitions)
    {
        if(comp->getSerieType() == CompetitionInfo::Competition || comp->getSerieType() == CompetitionInfo::Qualifications){
            if(i == index)
                return comp;
            i++;
        }
    }
    return nullptr;
}

CompetitionInfo *SeasonCalendar::getCompetitionByIndexAndType(QVector<CompetitionInfo *> &competitions, int index, int type)
{
    int i=0;
    for(auto & comp : competitions)
    {
        if(comp->getRulesPointer()->getCompetitionType() == type)
        {
            if(i == index)
                return comp;
            i++;
        }
    }
    return nullptr;
}

CompetitionInfo *SeasonCalendar::getCompetitionAfterCompetitionFilteredByType(QVector<CompetitionInfo *> &competitions, int actualIndex, int type, int howMany)
{
    if(actualIndex + howMany > competitions.count()){
        for(int i=actualIndex; i<competitions.count(); i++)
        {
            if(competitions[i]->getRulesPointer()->getCompetitionType() == type)
            {
                if(howMany <= 0)
                    return competitions[i];
                else
                {
                    howMany--;
                }
            }
            i++;
        }
    }
    return nullptr;
}

bool SeasonCalendar::getAllPlayed()
{
    for(auto & comp : competitions)
        if(comp->getPlayed() == false)
            return false;
    return true;
}

int SeasonCalendar::howManyCompetitionsPlayed()
{
    int count=0;
    for(auto & c : competitions)
        if(c->getPlayed())
            count++;
    return count;
}

QString SeasonCalendar::getName() const
{
    return name;
}

void SeasonCalendar::setName(const QString &newName)
{
    name = newName;
}

QVector<Classification *> SeasonCalendar::getClassifications() const
{
    return classifications;
}

QVector<Classification *> &SeasonCalendar::getClassificationsReference()
{
    return classifications;
}

void SeasonCalendar::setClassifications(const QVector<Classification *> &newClassifications)
{
    classifications = newClassifications;
}

void SeasonCalendar::setCompetitions(const QVector<CompetitionInfo *> &newCompetitions)
{
    competitions = newCompetitions;
}

QVector<CompetitionInfo *> SeasonCalendar::getCompetitions() const
{
    return competitions;
}

QVector<CompetitionInfo *> &SeasonCalendar::getCompetitionsReference()
{
    return competitions;
}
