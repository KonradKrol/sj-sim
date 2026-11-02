#include "../global/AtomicJsonFile.h"
#include <QObject>
#include "SeasonCalendarPreset.h"
#include <QJsonValue>
#include <QHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>
#include <QUuid>
#include "../global/JsonGraphValidation.h"

namespace {
QJsonObject normalizeLegacyPreset(QJsonObject json) {
    if(json.contains("schema-version"))return json;
    if(!json.value("calendar").isObject() || !json.value("calendar").toObject().value("competitions").isArray())return json;
    auto calendar=json.value("calendar").toObject();auto events=calendar.value("competitions").toArray();
    for(int i=0;i<events.size();++i){auto event=events[i].toObject();
        // Old configuration-only exports used zero IDs for these snapshots.
        // They carry no results or references and need fresh local identities.
        for(const auto &key:{"rules","results"}){auto snapshot=event.value(key).toObject();const auto id=snapshot.value("id").toString();
            if(id=="00000000-0000-0000-0000-000000000000" || id=="0-0" || id=="0") {
                snapshot["id"]=QUuid::createUuid().toString(QUuid::WithoutBraces);event[key]=snapshot;
            }
        }
        events[i]=event;
    }
    calendar["competitions"]=events;json["calendar"]=calendar;return json;
}
}

SeasonCalendarPreset::SeasonCalendarPreset(SeasonCalendar *seasonCalendar)
{
    if(seasonCalendar != nullptr)
    {
        SeasonCalendar * presetCalendar = &this->calendar;
        presetCalendar->defaultRules=seasonCalendar->defaultRules;
        QHash<CompetitionInfo *, CompetitionInfo *> competitions; //1. Konkurs z kalendarza, 2. Konkurs z presetu
        for(auto & comp : seasonCalendar->getCompetitionsReference())
        {
            CompetitionInfo * presetCompetition = new CompetitionInfo(*comp);
            presetCompetition->simulationBalance.reset();
            presetCompetition->reassign();
            presetCompetition->getResultsReference().reassign();
            presetCompetition->getResultsReference().setCompetition(presetCompetition);
            presetCompetition->setPlayed(false);presetCompetition->setCancelled(false);presetCompetition->setExceptionalRoundsCount(0);
            presetCompetition->getResultsReference().getResultsReference().clear();
            presetCompetition->getTeamsReference().clear();presetCompetition->getRoundsKOGroupsReference().clear();presetCompetition->getStartListReference().clear();
            presetCalendar->getCompetitionsReference().push_back(presetCompetition);
            competitions.insert(comp, presetCompetition);
        }
        for(auto & presetCompetition : presetCalendar->getCompetitionsReference())
        {
            for(auto & calendarCompetition : seasonCalendar->getCompetitionsReference())
            {
                if(presetCompetition->getAdvancementCompetition() == calendarCompetition)
                    presetCompetition->setAdvancementCompetition(competitions.value(calendarCompetition));
                if(presetCompetition->getPendingAdvancementCompetition()==calendarCompetition)presetCompetition->setPendingAdvancementCompetition(competitions.value(calendarCompetition));
                if(presetCompetition->getTrialRound() == calendarCompetition)
                    presetCompetition->setTrialRound(competitions.value(calendarCompetition));
                for(auto & presetTraining : presetCompetition->getTrainingsReference())
                {
                    if(presetTraining == calendarCompetition){
                        presetTraining = competitions.value(calendarCompetition);
                    }
                }
            }
        }
        QHash<Classification *, Classification *> classifications; //1. Klasyfikacja z kalendarza, 2. Klasyfikacja z presetu
        for(auto & cls : seasonCalendar->getClassificationsReference())
        {
            Classification * presetClassification = new Classification(*cls);
            presetClassification->reassign();
            presetClassification->setResults({});
            presetCalendar->getClassificationsReference().push_back(presetClassification);
            classifications.insert(cls, presetClassification);
        }
        for(auto & presetCompetition : presetCalendar->getCompetitionsReference())
        {
            for(auto & calendarClassification : seasonCalendar->getClassificationsReference())
            {
                if(presetCompetition->getAdvancementClassification() == calendarClassification)
                    presetCompetition->setAdvancementClassification(classifications.value(calendarClassification));
                for(auto & presetCls : presetCompetition->getClassificationsReference())
                {
                    if(presetCls == calendarClassification){
                        presetCompetition->remapClassificationScoring(calendarClassification->getIDStr(),classifications.value(calendarClassification)->getIDStr());
                        presetCls = classifications.value(calendarClassification);
                    }
                }
            }
        }
        presetCalendar->updateCompetitionsQualifyingCompetitions();
    }
}

SeasonCalendarPreset::~SeasonCalendarPreset()
{
    /*for(auto & comp : this->calendar.getCompetitionsReference())
        delete comp;
    for(auto & cls : this->calendar.getClassificationsReference())
        delete cls;*/
}

QJsonObject SeasonCalendarPreset::getJsonObject(SeasonCalendarPreset preset)
{
    QJsonObject object;
    object.insert("schema-version",1);
    object.insert("name", preset.getName());
    auto calendarJson = SeasonCalendar::getJsonObject(preset.getCalendarReference());
    auto competitions = calendarJson.value("competitions").toArray();
    for(int i = 0; i < competitions.size(); ++i) {
        auto competition = competitions[i].toObject();
        competition.remove("hill-id");
        competitions[i] = competition;
    }
    calendarJson.insert("competitions", competitions);
    object.insert("calendar", calendarJson);
    QJsonArray hillsArray;
    for(auto & hill : preset.getHillsReference()){
        QJsonObject hillObject;
        hillObject.insert("name", hill.first);
        hillObject.insert("hs-point", hill.second);
        hillsArray.push_back(hillObject);
    }
    object.insert("hills", hillsArray);
    return object;
}

bool SeasonCalendarPreset::validateJson(const QJsonObject &input,QString &error,bool legacyIds)
{
    const auto json=normalizeLegacyPreset(input);
    const auto fail=[&](const QString &message){error=message;return false;};
    if(json.contains("schema-version") && (!json.value("schema-version").isDouble() || json.value("schema-version").toDouble()!=1))return fail("Unsupported calendar preset version");
    const auto calendar=json.value("calendar").toObject();
    if(!json.value("name").isString() || json.value("name").toString().trimmed().isEmpty() || !json.value("calendar").isObject()
        || !calendar.value("name").isString() || !calendar.value("competitions").isArray() || !calendar.value("classifications").isArray()
        || !json.value("hills").isArray())return fail("Missing calendar preset fields");
    const auto name=json.value("name").toString();
    if(name.contains('/') || name.contains('\\') || name=="." || name=="..")return fail("Invalid preset file name");
    const auto events=calendar.value("competitions").toArray(),hills=json.value("hills").toArray();
    if(events.size()!=hills.size())return fail("Hill list does not match the event list");
    for(const auto &value:hills){const auto hill=value.toObject();const double hs=hill.value("hs-point").toDouble(-1);
        if(!hill.value("name").isString() || hill.value("name").toString().trimmed().isEmpty() || !hill.value("hs-point").isDouble() || !std::isfinite(hs) || hs<=0)return fail("Invalid hill description");}
    for(const auto &value:events){const auto event=value.toObject(),rules=event.value("rules").toObject();
        if(!event.value("serie-type").isDouble() || event.value("serie-type").toDouble()<0 || event.value("serie-type").toDouble()>3
            || event.value("serie-type").toDouble()!=event.value("serie-type").toInt()
            || !rules.value("rounds").isArray() || rules.value("rounds").toArray().isEmpty()
            || !rules.value("competition-type").isDouble() || (rules.value("competition-type").toDouble()!=0 && rules.value("competition-type").toDouble()!=1)
            || !CompetitionRules::validateWindMeasurementJson(rules) || !event.value("results").isObject()
            || !event.value("results").toObject().value("results").isArray())return fail("Invalid event or competition rules");
        // Presets contain configuration, never saved participants or results.
        for(const auto &key:{"start-list","teams","rounds-ko-groups"})if(event.contains(key) && !event.value(key).isArray())return fail("Invalid participant collection");
        if(!event.value("start-list").toArray().isEmpty() || !event.value("teams").toArray().isEmpty()
            || !event.value("results").toObject().value("results").toArray().isEmpty())return fail("Preset contains saved participants or results");
        for(const auto &groups:event.value("rounds-ko-groups").toArray())if(!groups.isArray() || !groups.toArray().isEmpty())return fail("Preset contains saved KO participants");
    }
    JsonGraphValidation::Registry registry;registry.legacyIds=legacyIds;
    if(!registry.calendar(calendar) || !registry.references(calendar))return fail(registry.error);
    const auto reference=[&](const QJsonValue &value,JsonGraphValidation::Registry::Kind kind,bool optional){
        const auto id=value.toString();if(optional && (value.isUndefined() || id.isEmpty() || id=="-1"))return true;
        return value.isString() && registry.accepts(id) && registry.types.contains(registry.key(id)) && registry.types.value(registry.key(id))==kind;
    };
    for(const auto &value:events){const auto event=value.toObject();
        for(const auto &key:{"trial-round-id","advancement-competition-id","pending-advancement-competition-id"})
            if(!reference(event.value(key),JsonGraphValidation::Registry::Competition,true) || event.value(key)==event.value("id"))return fail("Invalid event link");
        if(!reference(event.value("advancement-classification-id"),JsonGraphValidation::Registry::Classification,true))return fail("Invalid advancement classification");
        for(const auto &key:{"training-ids","classifications-ids"}) {
            if(event.contains(key) && !event.value(key).isArray())return fail("Invalid link list");
            for(const auto &id:event.value(key).toArray())if(!reference(id,QString::fromLatin1(key)=="training-ids"?JsonGraphValidation::Registry::Competition:JsonGraphValidation::Registry::Classification,false)
                || id==event.value("id"))return fail("Unresolved calendar link");
        }
    }
    for(const auto &value:calendar.value("classifications").toArray())if(!value.toObject().value("results").toArray().isEmpty())return fail("Preset contains classification results");
    error.clear();return true;
}

SeasonCalendarPreset SeasonCalendarPreset::getFromJson(QJsonObject json, IdentifiableObjectsStorage * storage, QMap<ulong, Identifiable *> *before120Map)
{
    json=normalizeLegacyPreset(json);
    SeasonCalendarPreset preset;
    preset.setName(json.value("name").toString());
    preset.setCalendar(SeasonCalendar::getFromJson(json.value("calendar").toObject(), storage, before120Map));
    QJsonArray hillsArray = json.value("hills").toArray();
    for(auto val : hillsArray)
    {
        QPair<QString, double> hill;
        hill.first = val.toObject().value("name").toString();
        hill.second = val.toObject().value("hs-point").toDouble();
        preset.getHillsReference().push_back(hill);
    }
    return preset;
}

QVector<SeasonCalendarPreset> SeasonCalendarPreset::getVectorFromJson(QByteArray json)
{
    QVector<SeasonCalendarPreset> presets;
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(json, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, QObject::tr("Błąd przy wczytytywaniu presetów kalendarzy"), QObject::tr("Treść błędu: ") + error.errorString(),  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("calendarPresets");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, QObject::tr("Błąd przy wczytytywaniu presetów kalendarzy"),QObject::tr("Prawdopodobnie w tym pliku nie ma listy z presetami\nUpewnij się, że wybrałeś właściwy plik!"));
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonArray array = value.toArray();
    for(const auto &val:array) {
        QString error;
        if(!val.isObject() || !validateJson(val.toObject(),error)) {
            QMessageBox::warning(nullptr,QObject::tr("Preset kalendarza"),
                QObject::tr("Nie udało się wczytać presetów kalendarza. Dane są nieprawidłowe:\n%1").arg(error));
            return {};
        }
    }
    for(const auto & val : array)
    {
        IdentifiableObjectsStorage manager;
        presets.push_back(SeasonCalendarPreset::getFromJson(val.toObject(), &manager));
    }
    return presets;
}

bool SeasonCalendarPreset::saveToFile(QString dir)
{
    QJsonDocument document;
    QJsonObject mainObject;
    mainObject.insert("calendar-preset", SeasonCalendarPreset::getJsonObject(*this));
    document.setObject(mainObject);
    return AtomicJsonFile::write(dir + getName() + ".json",document);
}

QString SeasonCalendarPreset::getName() const
{
    return name;
}

void SeasonCalendarPreset::setName(const QString &newName)
{
    name = newName;
}

SeasonCalendar &SeasonCalendarPreset::getCalendarReference()
{
    return calendar;
}

void SeasonCalendarPreset::setHills(const QVector<QPair<QString, double> > &newHills)
{
    hills = newHills;
}

void SeasonCalendarPreset::setCalendar(const SeasonCalendar &newCalendar)
{
    calendar = newCalendar;
}

QVector<QPair<QString, double> > &SeasonCalendarPreset::getHillsReference()
{
    return hills;
}
