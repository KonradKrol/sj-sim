#pragma once
#include "Season.h"
#include "../competitions/CompetitionInfo.h"
#include <QCoreApplication>

namespace SeasonChronology {
struct Block {
    CompetitionInfo *event=nullptr;
    SeasonCalendar *calendar=nullptr;
    QDate lastDate;
    bool stale=false,invalidDate=false,invalidOrder=false;
    bool blocked() const {return event!=nullptr;}
};
inline bool pending(const CompetitionInfo *event){return event && !event->getPlayed() && !event->getCancelled();}
inline Block inspect(const Season *season,CompetitionInfo *candidate) {
    Block result;if(!season || !season->getDatesEnabled())return result;
    CompetitionInfo *earliest=nullptr;SeasonCalendar *owner=nullptr;
    for(auto *calendar:season->getCalendars())if(calendar)for(auto *event:calendar->getCompetitions())if(event) {
        const auto date=event->getScheduledDate();
        if(!pending(event) && date.isValid())result.lastDate=qMax(result.lastDate,date);
        if(pending(event) && date.isValid() && (!earliest || date<earliest->getScheduledDate())){earliest=event;owner=calendar;}
    }
    if(earliest && result.lastDate.isValid() && earliest->getScheduledDate()<result.lastDate) {
        result.event=earliest;result.calendar=owner;result.stale=true;return result;
    }
    for(auto *calendar:season->getCalendars())if(calendar)for(auto *event:calendar->getCompetitions())
        if(pending(event) && !event->getScheduledDate().isValid()){result.event=event;result.calendar=calendar;result.invalidDate=true;return result;}
    for(auto *calendar:season->getCalendars())if(calendar) {
        QDate previous;
        for(auto *event:calendar->getCompetitions())if(event && !event->getCancelled()) {
            const auto date=event->getScheduledDate();
            if(pending(event) && date.isValid() && previous.isValid() && date<previous){result.event=event;result.calendar=calendar;result.invalidOrder=true;return result;}
            if(date.isValid())previous=qMax(previous,date);
        }
    }
    if(!earliest)return result;
    if(!candidate || candidate->getScheduledDate()>earliest->getScheduledDate()){result.event=earliest;result.calendar=owner;return result;}
    for(auto *calendar:season->getCalendars())if(calendar) {
        CompetitionInfo *first=nullptr;
        for(auto *event:calendar->getCompetitions())if(pending(event)){first=event;break;}
        if(calendar->getCompetitions().contains(candidate) && first!=candidate){result.event=first;result.calendar=calendar;return result;}
    }
    if(auto *source=candidate->getAdvancementCompetition();pending(source)) {
        for(auto *calendar:season->getCalendars())if(calendar && calendar->getCompetitions().contains(source)){result.event=source;result.calendar=calendar;return result;}
    }
    return result;
}
inline QString description(const Block &block) {
    if(!block.blocked())return {};
    const auto tr=[](const char *text){return QCoreApplication::translate("SeasonChronology",text);};
    auto *hill=block.event->getHill();
    const QString event=block.event->getLongSerieTypeText()+" · "+(hill?hill->getName():tr("Brak skoczni"));
    const QString calendar=block.calendar?block.calendar->getName():QString();
    if(block.invalidOrder)return tr("Daty w kalendarzu „%1” nie są chronologiczne. Popraw daty lub kolejność wydarzeń.").arg(calendar);
    if(block.invalidDate)return tr("%1 — kalendarz „%2” nie ma daty wydarzenia. Ustal termin przed dalszą grą.").arg(event,calendar);
    const QString date=block.event->getScheduledDate().toString("dd.MM.yyyy");
    if(block.stale)return tr("Zaległe wydarzenie: %1 · %2 · „%3”. Sezon doszedł już do %4. Zmień datę lub odwołaj wydarzenie.").arg(event,date,calendar,block.lastDate.toString("dd.MM.yyyy"));
    return tr("Najpierw rozegraj lub odwołaj: %1 · %2 · kalendarz „%3”.").arg(event,date,calendar);
}
}
