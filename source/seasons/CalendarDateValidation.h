#pragma once
#include "SeasonCalendar.h"
#include "../competitions/CompetitionInfo.h"
#include <QCoreApplication>
#include <QStringList>

namespace CalendarDateValidation {
inline QStringList issues(const SeasonCalendar *calendar,bool enabled) {
    QStringList result;if(!calendar)return result;
    const auto tr=[](const char *text){return QCoreApplication::translate("CalendarDates",text);};
    QDate previous;QSet<CompetitionInfo *> auxiliary;
    for(auto *event:calendar->getCompetitions())if(event){for(auto *training:event->getTrainings())auxiliary.insert(training);if(event->getTrialRound())auxiliary.insert(event->getTrialRound());}
    int number=0;
    for(auto *event:calendar->getCompetitions()) {
        if(!event || auxiliary.contains(event))continue;
        ++number;
        if(event->getCancelled())continue;
        if(event->getPendingAdvancementCompetition())result.append(tr("Konkurs %1: źródło kwalifikacji musi być wcześniej w kalendarzu.").arg(number));
        if(!enabled)continue;
        const auto date=event->getScheduledDate();
        if(!date.isValid()){result.append(tr("Konkurs %1: ustaw datę.").arg(number));continue;}
        if(previous.isValid() && date<previous)result.append(tr("Konkurs %1 ma datę wcześniejszą niż poprzedni konkurs.").arg(number));
        if(auto *source=event->getAdvancementCompetition();source && source->getScheduledDate().isValid() && source->getScheduledDate()>date)
            result.append(tr("Konkurs %1: kwalifikacje zaplanowano po konkursie.").arg(number));
        auto sessions=event->getTrainings();if(event->getTrialRound())sessions.append(event->getTrialRound());
        auto sessionDate=previous;
        for(auto *session:sessions)if(session) {
            const auto value=session->getScheduledDate();
            if(!value.isValid() || value>date || (sessionDate.isValid() && value<sessionDate))
                result.append(tr("Konkurs %1: popraw termin treningu lub serii próbnej.").arg(number));
            if(value.isValid())sessionDate=value;
        }
        previous=date;
    }
    return result;
}
}
