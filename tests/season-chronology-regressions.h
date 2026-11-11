#pragma once
#include "../source/seasons/SeasonChronology.h"

static void seasonChronologyRegressions() {
    Season season;SeasonCalendar a,b;CompetitionInfo training,trial,qualification,main,other;
    a.setName("A");b.setName("B");season.setCalendars({&a,&b});season.setActualCalendar(&a);season.setDatesEnabled(true);
    training.setScheduledDate(QDate(2027,1,1));trial.setScheduledDate(QDate(2027,1,2));qualification.setScheduledDate(QDate(2027,1,2));main.setScheduledDate(QDate(2027,1,3));other.setScheduledDate(QDate(2027,1,2));
    a.setCompetitions({&training,&trial,&qualification,&main});b.setCompetitions({&other});
    auto block=SeasonChronology::inspect(&season,&other);
    check(block.event==&training && block.calendar==&a,"earliest event blocks another calendar including training");
    check(!SeasonChronology::inspect(&season,&training).blocked(),"earliest event can start");
    training.setPlayed(true);
    check(!SeasonChronology::inspect(&season,&trial).blocked() && !SeasonChronology::inspect(&season,&other).blocked(),"same day calendars are freely selectable");
    check(SeasonChronology::inspect(&season,&qualification).event==&trial,"same day local order is preserved");
    check(SeasonChronology::inspect(&season,&main).blocked(),"future contest cannot bypass earlier dates");
    trial.setPlayed(true);qualification.setPlayed(true);
    main.setAdvancementCompetition(&other);
    check(SeasonChronology::inspect(&season,&main).event==&other,"qualification source must finish first");
    other.setCancelled(true);other.setPlayed(true);
    check(!SeasonChronology::inspect(&season,&main).blocked(),"cancelled qualification releases dependent contest");
    main.setPlayed(true);other.setCancelled(false);other.setPlayed(false);
    block=SeasonChronology::inspect(&season,&other);
    check(block.stale && block.event==&other,"legacy pending event before last resolved date blocks even itself");
    other.setCancelled(true);other.setPlayed(true);
    check(!SeasonChronology::inspect(&season,nullptr).blocked(),"cancelling legacy backlog releases completed season");
    other.setCancelled(false);other.setPlayed(false);other.setScheduledDate(QDate(2027,1,3));
    check(!SeasonChronology::inspect(&season,&other).blocked(),"moving backlog to current date repairs chronology");
    other.setScheduledDate(QDate());check(SeasonChronology::inspect(&season,&other).invalidDate,"missing dates block every calendar");
    Season undated;undated.setCalendars({&a,&b});
    check(!SeasonChronology::inspect(&undated,&other).blocked(),"disabled dates preserve unrestricted play");
    main.setPlayed(false);other.setScheduledDate(QDate(2027,1,5));a.setCompetitions({&main,&training});training.setPlayed(false);
    check(SeasonChronology::inspect(&season,&main).invalidOrder || SeasonChronology::inspect(&season,&main).stale,"malformed local chronology cannot silently skip a blocked event");
    season.setCalendars({});undated.setCalendars({});a.setCompetitions({});b.setCompetitions({});
}
