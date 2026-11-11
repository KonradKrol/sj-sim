#pragma once
#include "../source/seasons/CalendarDateValidation.h"

static void calendarDateValidationRegressions()
{
    SeasonCalendar calendar;
    CompetitionInfo first,second,training,trial;
    calendar.setCompetitions({&first,&second});
    first.setScheduledDate(QDate(2027,1,2));second.setScheduledDate(QDate(2027,1,3));
    check(CalendarDateValidation::issues(&calendar,true).isEmpty(),"chronological dates are valid");
    second.setScheduledDate(QDate(2027,1,1));
    check(!CalendarDateValidation::issues(&calendar,true).isEmpty(),"date draft detects reversed chronology");
    check(CalendarDateValidation::issues(&calendar,false).isEmpty(),"disabled dates do not block calendar");
    second.setScheduledDate(QDate(2027,1,2));
    check(CalendarDateValidation::issues(&calendar,true).isEmpty(),"same day events retain manual order");
    first.setTrainings({&training});first.setTrialRound(&trial);
    training.setScheduledDate(QDate(2027,1,1));trial.setScheduledDate(QDate(2027,1,3));
    check(!CalendarDateValidation::issues(&calendar,true).isEmpty(),"trial after parent is blocked");
    trial.setScheduledDate(QDate(2027,1,2));
    check(CalendarDateValidation::issues(&calendar,true).isEmpty(),"correcting draft clears blockers");
    second.setScheduledDate(QDate());
    check(!CalendarDateValidation::issues(&calendar,true).isEmpty(),"missing date is blocked");
    calendar.setCompetitions({});
}
