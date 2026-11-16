#include "Season.h"
#include "CalendarDateValidation.h"
#include <memory>
#include <vector>
#include <limits>

Season::Season() :
    Identifiable(),
    actualCalendar(nullptr),
    seasonNumber(0)
{
}

Season::~Season()
{
}

bool Season::prepareNext(bool repeatCalendars, Season &output, QString &error) const
{
    error.clear();
    if(seasonNumber==std::numeric_limits<int>::max()) {
        error=QObject::tr("Nie można utworzyć kolejnego numeru sezonu.");return false;
    }
    for(auto *calendar:calendars)if(calendar)for(auto *event:calendar->getCompetitions())
        if(event && !event->getPlayed() && !event->getCancelled()) {
            error=QObject::tr("Najpierw rozegraj lub odwołaj pozostałe wydarzenia sezonu.");return false;
        }
    Season next;next.setSeasonNumber(seasonNumber+1);next.setDatesEnabled(datesEnabled);
    if(!repeatCalendars){output=next;return true;}
    std::vector<std::unique_ptr<SeasonCalendar>> ownedCalendars;
    std::vector<std::unique_ptr<CompetitionInfo>> ownedEvents;
    std::vector<std::unique_ptr<Classification>> ownedClassifications;
    QHash<CompetitionInfo *,CompetitionInfo *> eventCopies;
    QHash<Classification *,Classification *> classificationCopies;
    QHash<QString,QString> ids;
    IdentifiableObjectsStorage storage;
    for(auto *oldCalendar:calendars) {
        if(!oldCalendar)continue;
        auto calendar=std::make_unique<SeasonCalendar>(oldCalendar->getName());
        calendar->defaultRules=oldCalendar->defaultRules;
        calendar->setJumpersListId(oldCalendar->jumpersListId());
        for(auto *oldClassification:oldCalendar->getClassifications()) {
            if(!oldClassification)continue;
            if(classificationCopies.contains(oldClassification)) {
                error=QObject::tr("Ta sama klasyfikacja jest przypisana do kilku kalendarzy. Popraw kalendarze przed ich powtórzeniem.");return false;
            }
            auto json=Classification::getJsonObject(oldClassification);json["results"]=QJsonArray();
            auto classification=std::unique_ptr<Classification>(Classification::getFromJson(json,&storage));
            classification->reassign();
            ids.insert(oldClassification->getIDStr(),classification->getIDStr());
            classificationCopies.insert(oldClassification,classification.get());
            calendar->getClassificationsReference().append(classification.get());
            ownedClassifications.push_back(std::move(classification));
        }
        for(auto *oldEvent:oldCalendar->getCompetitions()) {
            if(!oldEvent)continue;
            if(eventCopies.contains(oldEvent)) {
                error=QObject::tr("To samo wydarzenie jest przypisane do kilku kalendarzy. Popraw kalendarze przed ich powtórzeniem.");return false;
            }
            auto event=std::make_unique<CompetitionInfo>(*oldEvent);event->reassign();
            event->setResults(CompetitionResults());event->getResultsReference().setCompetition(event.get());
            event->getRulesPointer()->reassign();
            event->setPlayed(false);event->setCancelled(false);event->setExceptionalRoundsCount(-1);
            event->simulationBalance.reset();
            event->getTeamsReference().clear();event->getRoundsKOGroupsReference().clear();event->getStartListReference().clear();
            event->setQualifyingCompetitions({});
            if(oldEvent->getScheduledDate().isValid()) {
                const auto date=oldEvent->getScheduledDate().addYears(1);
                if(!date.isValid()) {error=QObject::tr("Nie można przesunąć dat konkursów o rok.");return false;}
                event->setScheduledDate(date);
            }
            ids.insert(oldEvent->getIDStr(),event->getIDStr());eventCopies.insert(oldEvent,event.get());
            calendar->getCompetitionsReference().append(event.get());ownedEvents.push_back(std::move(event));
        }
        if(oldCalendar==actualCalendar)next.setActualCalendar(calendar.get());
        next.getCalendarsReference().append(calendar.get());ownedCalendars.push_back(std::move(calendar));
    }
    for(auto it=eventCopies.cbegin();it!=eventCopies.cend();++it) {
        auto *old=it.key();auto *event=it.value();
        event->setAdvancementCompetition(eventCopies.value(old->getAdvancementCompetition()));
        event->setPendingAdvancementCompetition(eventCopies.value(old->getPendingAdvancementCompetition()));
        event->setAdvancementClassification(classificationCopies.value(old->getAdvancementClassification()));
        event->setTrialRound(eventCopies.value(old->getTrialRound()));
        QVector<CompetitionInfo *> trainings;
        for(auto *training:old->getTrainings())if(eventCopies.contains(training))trainings.append(eventCopies.value(training));
        event->setTrainings(trainings);
        QVector<Classification *> classifications;
        for(auto *classification:old->getClassifications())if(classificationCopies.contains(classification))classifications.append(classificationCopies.value(classification));
        event->setClassifications(classifications);
        QHash<QString,ClassificationCompetitionSettings> scoring;
        const auto previousScoring=old->getClassificationScoring();
        for(auto score=previousScoring.cbegin();score!=previousScoring.cend();++score)
            if(ids.contains(score.key()))scoring.insert(ids.value(score.key()),score.value());
        event->setClassificationScoring(scoring);
        event->startListSortCompetitionId=ids.value(old->startListSortCompetitionId,old->startListSortCompetitionId);
        event->startListSortClassificationId=ids.value(old->startListSortClassificationId,old->startListSortClassificationId);
    }
    for(auto *calendar:next.getCalendars()) {
        calendar->normalizeCompetitionEvents();calendar->updateCompetitionsQualifyingCompetitions();
        const auto issues=CalendarDateValidation::issues(calendar,datesEnabled);
        if(!issues.isEmpty()){error=QObject::tr("Nie można powtórzyć kalendarza „%1”: %2").arg(calendar->getName(),issues.first());return false;}
    }
    if(!next.getCalendars().isEmpty()) {
        auto *first=next.getCalendars().first();QDate earliest;
        if(datesEnabled)for(auto *calendar:next.getCalendars())for(auto *event:calendar->getCompetitions())
            if(event->getScheduledDate().isValid() && (!earliest.isValid() || event->getScheduledDate()<earliest)) {
                earliest=event->getScheduledDate();first=calendar;
            }
        next.setActualCalendar(first);
    }
    next.synchronizeClassificationYears();
    output=next;
    for(auto &calendar:ownedCalendars)calendar.release();
    for(auto &event:ownedEvents)event.release();
    for(auto &classification:ownedClassifications)classification.release();
    return true;
}

QJsonObject Season::getJsonObject(const Season &season)
{
    QJsonObject obj;
    obj.insert("id", season.getIDStr());
    obj.insert("season-number", season.getSeasonNumber());
    obj.insert("dates-enabled",season.datesEnabled);
    obj.insert("started",season.hasStarted());
    if(season.getActualCalendar() != nullptr)
        obj.insert("actual-calendar-id", season.getActualCalendar()->getIDStr());
    else
        obj.insert("actual-calendar-id", QString::number(-1));

    QJsonArray calendarsArray;
    for(auto *calendar : season.getCalendars())
        calendarsArray.append(SeasonCalendar::getJsonObject(*calendar));
    obj.insert("calendars", calendarsArray);

    return obj;
}

Season Season::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage)
{
    Season season;
    season.setID(sole::rebuild(obj.value("id").toString().toStdString()));
    season.setSeasonNumber(obj.value("season-number").toInt());
    season.datesEnabled=obj.value("dates-enabled").toBool(false);
    season.started=obj.value("started").toBool(false);


    /*QJsonArray array = obj.value("calendars").toArray();
    QVector<QJsonValue> values;
    for(auto val : array)
        values.push_back(val);
    QFuture<SeasonCalendar *> calendarsFuture = QtConcurrent::mapped(values, [storage](const QJsonValue &value){
        return new SeasonCalendar(SeasonCalendar::getFromJson(value.toObject(), storage));
    });
    season.setCalendars(calendarsFuture.results().toVector());
    if(storage != nullptr)
        storage->add(&season.getCalendarsReference());*/

    QJsonArray array = obj.value("calendars").toArray();
    for(auto a : array)
    {
        season.getCalendarsReference().push_back(new SeasonCalendar(SeasonCalendar::getFromJson(a.toObject(), storage,nullptr,season.getSeasonNumber())));
    }
    if(storage != nullptr)
        storage->add(season.getCalendarsReference());

    if(obj.value("actual-calendar-id").toString() == QStringLiteral("-1"))
        season.setActualCalendar(nullptr);
    else
        season.setActualCalendar(static_cast<SeasonCalendar *>(storage->get(obj.value("actual-calendar-id").toString())));

    return season;
}

bool Season::containsCalendarByName(QString n)
{
    for(auto & c : calendars)
    {
        if(c->getName() == n)
            return true;
    }
    return false;
}

SeasonCalendar *Season::getActualCalendar() const
{
    return actualCalendar;
}

void Season::setActualCalendar(SeasonCalendar *newActualCalendar)
{
    actualCalendar = newActualCalendar;
    synchronizeClassificationYears();
}

QVector<SeasonCalendar *> Season::getCalendars() const
{
    return calendars;
}

QVector<SeasonCalendar *> &Season::getCalendarsReference()
{
    return calendars;
}

void Season::setCalendars(const QVector<SeasonCalendar *> &newCalendars)
{
    calendars = newCalendars;
    synchronizeClassificationYears();
}

int Season::getSeasonNumber() const
{
    return seasonNumber;
}

void Season::setSeasonNumber(int newSeasonNumber)
{
    seasonNumber = newSeasonNumber;
    synchronizeClassificationYears();
}

bool Season::hasStarted() const
{
    if(started)return true;
    for(auto *calendar:calendars)for(auto *event:calendar->getCompetitionsReference())
        if(event->getPlayed() || event->getCancelled() || !event->getResultsReference().getResultsReference().isEmpty())return true;
    return false;
}
bool Season::setDatesEnabled(bool enabled)
{
    if(enabled && !datesEnabled && hasStarted())return false;
    started=hasStarted();datesEnabled=enabled;return true;
}

void Season::synchronizeClassificationYears() const {
    if(seasonNumber<1 || seasonNumber>9999)return;
    for(auto *calendar:calendars)if(calendar)for(auto *entry:calendar->getClassificationsReference())if(entry){auto options=entry->getOptions();options.referenceYear=seasonNumber;entry->setOptions(options);}
}
