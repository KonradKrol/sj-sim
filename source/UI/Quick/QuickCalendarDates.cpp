#include "QuickCalendar.h"
#include "../Seasons/CalendarEditor/CalendarEditorWidget.h"
#include "../Seasons/SimulationSaveManagerWindow.h"
#include <QLocale>
#include <algorithm>

bool QuickCalendar::datesEnabled() const
{
    return m_save && m_save->getActualSeason() && m_save->getActualSeason()->getDatesEnabled();
}
bool QuickCalendar::canEnableDates() const
{
    return m_save && m_save->getActualSeason() && !m_save->getActualSeason()->hasStarted();
}
void QuickCalendar::setDatesEnabled(bool enabled)
{
    sync();
    if(!m_save || !m_save->getActualSeason())return;
    if(!m_save->getActualSeason()->setDatesEnabled(enabled)){
        fail(tr("Daty można włączyć tylko przed rozpoczęciem sezonu."));return;
    }
    clearUndo();
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())
        if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))manager->fillNextCompetitionInformations();
    emit m_editor->changed();m_dirty=false;refresh();
}
QString QuickCalendar::normalizeDate(const QString &value) const
{
    const auto text=value.trimmed();
    QDate date=QDate::fromString(text,Qt::ISODate);
    if(!date.isValid())date=QLocale().toDate(text,QLocale::ShortFormat);
    if(!date.isValid())date=QLocale().toDate(text,QLocale::LongFormat);
    return date.isValid() && date.year()>0 && date.year()<=9999?date.toString(Qt::ISODate):QString();
}
QString QuickCalendar::dateText(const QString &iso) const
{
    const auto date=QDate::fromString(iso,Qt::ISODate);
    return date.isValid()?QLocale().toString(date,QLocale::ShortFormat):QString();
}
QString QuickCalendar::offsetDate(const QString &iso,int days,int months) const
{
    if(qAbs(qint64(days))>366000 || qAbs(qint64(months))>12000)return {};
    auto date=QDate::fromString(iso,Qt::ISODate).addMonths(months).addDays(days);
    return date.year()>0 && date.year()<=9999?date.toString(Qt::ISODate):QString();
}
QString QuickCalendar::suggestedDate() const
{
    for(auto *event:selected())if(event->getScheduledDate().isValid())return event->getScheduledDate().toString(Qt::ISODate);
    for(auto *event:mains())if(event->getScheduledDate().isValid() && !locked(event))return event->getScheduledDate().toString(Qt::ISODate);
    const int year=m_save && m_save->getActualSeason()?m_save->getActualSeason()->getSeasonNumber():QDate::currentDate().year();
    return QDate(qBound(1,year,9999),11,1).toString(Qt::ISODate);
}
QVariantMap QuickCalendar::monthData(const QString &iso) const
{
    auto anchor=QDate::fromString(iso,Qt::ISODate);
    if(!anchor.isValid())anchor=QDate::fromString(suggestedDate(),Qt::ISODate);
    const QDate first(anchor.year(),anchor.month(),1);
    const QDate start=first.addDays(1-first.dayOfWeek());
    QVariantList days;QStringList weekdays;
    const QLocale locale;
    for(int weekday=1;weekday<=7;++weekday)weekdays.append(locale.standaloneDayName(weekday,QLocale::ShortFormat));
    int count=0,undated=0;QString firstEvent;
    for(const auto &entry:m_events){const auto event=entry.toMap();const auto date=QDate::fromString(event.value("date").toString(),Qt::ISODate);
        if(!date.isValid())++undated;
        else {if(firstEvent.isEmpty() || event.value("date").toString()<firstEvent)firstEvent=event.value("date").toString();if(date.year()==first.year()&&date.month()==first.month())++count;}}
    for(int i=0;i<42;++i){const auto date=start.addDays(i);QVariantList events;
        for(const auto &entry:m_events)if(entry.toMap().value("date").toString()==date.toString(Qt::ISODate))events.append(entry);
        days.append(QVariantMap{{"iso",date.toString(Qt::ISODate)},{"day",date.day()},{"current",date.month()==first.month()},
            {"weekend",date.dayOfWeek()>=6},{"today",date==QDate::currentDate()},{"events",events}});
    }
    return {{"title",locale.standaloneMonthName(first.month(),QLocale::LongFormat)+" "+QString::number(first.year())},
        {"iso",first.toString(Qt::ISODate)},{"days",days},{"weekdays",weekdays},{"count",count},{"undated",undated},{"firstEvent",firstEvent}};
}
QVariantList QuickCalendar::dateWarnings() const
{
    QVariantList warnings;if(!datesEnabled())return warnings;
    QDate previous;int missing=0;QHash<QDate,QString> venues;
    for(auto *event:mains()){
        const auto date=event->getScheduledDate();if(!date.isValid()){++missing;continue;}
        const int number=mains().indexOf(event)+1;
        if(previous.isValid() && date<previous)warnings.append(tr("Konkurs %1 ma datę wcześniejszą niż poprzedni. Symulacja nadal używa kolejności listy.").arg(number));
        previous=date;
        const auto venue=event->getHill()?event->getHill()->getName():QString();
        if(venues.contains(date) && venues.value(date)!=venue)warnings.append(tr("%1: zawody w różnych miejscowościach tego samego dnia.").arg(dateText(date.toString(Qt::ISODate))));
        venues.insert(date,venue);
        auto *source=event->getAdvancementCompetition();
        if(source && source->getScheduledDate().isValid() && source->getScheduledDate()>date)warnings.append(tr("Konkurs %1: kwalifikacje mają późniejszą datę.").arg(number));
        for(auto *training:event->getTrainings())if(training->getScheduledDate().isValid() && training->getScheduledDate()>date){warnings.append(tr("Konkurs %1: trening ma późniejszą datę niż zawody.").arg(number));break;}
    }
    if(missing)warnings.prepend(tr("Bez daty: %1 konkursów. Możesz uzupełnić je zbiorczo.").arg(missing));
    return warnings;
}
void QuickCalendar::discardDatePlan()
{
    if(m_datePlan.isEmpty() && m_plannedDates.isEmpty())return;
    m_datePlan.clear();m_plannedDates.clear();emit changed();
}
void QuickCalendar::planDates(const QString &start,const QString &pattern,int interval,bool future,int trainingDays)
{
    sync();discardDatePlan();if(!datesEnabled())return;
    QDate date=QDate::fromString(normalizeDate(start),Qt::ISODate);
    if(!date.isValid() || interval<1 || interval>366 || trainingDays<0 || trainingDays>14 || !QStringList{"interval","weekends","weekly","same","preserve"}.contains(pattern)){
        fail(tr("Wybierz poprawną datę i odstęp od 1 do 366 dni."));return;
    }
    auto targets=selected();
    if(future){targets.clear();for(auto *event:mains())if(!locked(event))targets.append(event);}
    if(targets.isEmpty() || std::any_of(targets.begin(),targets.end(),[&](auto *e){return locked(e);})){fail(tr("Wybierz nierozegrane konkursy do zaplanowania."));return;}
    qint64 shift=0;
    if(pattern=="preserve"){
        for(auto *event:targets)if(!event->getScheduledDate().isValid()){fail(tr("Aby zachować odstępy, najpierw nadaj daty wszystkim wybranym konkursom."));return;}
        shift=targets.first()->getScheduledDate().daysTo(date);
    }
    if(pattern=="weekends")date=date.addDays((6-date.dayOfWeek()+7)%7);
    for(auto *event:targets){
        if(pattern=="preserve")date=event->getScheduledDate().addDays(shift);
        const auto trainingDate=date.addDays(-trainingDays);
        if(!date.isValid() || date.year()>9999 || !trainingDate.isValid() || trainingDate.year()<1){discardDatePlan();fail(tr("Plan wykracza poza obsługiwany zakres dat."));return;}
        m_plannedDates.insert(event,date);
        for(auto *training:event->getTrainings())m_plannedDates.insert(training,pattern=="preserve" && training->getScheduledDate().isValid()?training->getScheduledDate().addDays(shift):trainingDate);
        if(event->getTrialRound())m_plannedDates.insert(event->getTrialRound(),pattern=="preserve" && event->getTrialRound()->getScheduledDate().isValid()?event->getTrialRound()->getScheduledDate().addDays(shift):date);
        m_datePlan.append(QVariantMap{{"id",event->getIDStr()},{"number",mains().indexOf(event)+1},
            {"title",event->getHill()?event->getHill()->getName():QString()},{"flag",hillFlag(event->getHill())},
            {"before",event->getScheduledDate().isValid()?dateText(event->getScheduledDate().toString(Qt::ISODate)):tr("Bez daty")},
            {"after",dateText(date.toString(Qt::ISODate))},{"date",date.toString(Qt::ISODate)}});
        date=date.addDays(pattern=="weekends"?(date.dayOfWeek()==6?1:6):pattern=="weekly"?7:pattern=="same"?0:interval);
    }
    for(const auto &value:m_plannedDates)if(!value.isValid() || value.year()<1 || value.year()>9999){discardDatePlan();fail(tr("Plan wykracza poza obsługiwany zakres dat."));return;}
    m_message=tr("Podgląd: %1 konkursów. Terminy zmienią się po zastosowaniu.").arg(targets.size());emit changed();
}
void QuickCalendar::applyDates(const QHash<CompetitionInfo *,QDate> &dates)
{
    mutate([&]{for(auto it=dates.begin();it!=dates.end();++it)it.key()->setScheduledDate(it.value());});
}
void QuickCalendar::applyDatePlan()
{
    sync();if(!datesEnabled() || m_plannedDates.isEmpty())return;
    for(auto *event:m_plannedDates.keys())if(!m_calendar->getCompetitionsReference().contains(event) || locked(m_calendar->getMainCompetitionForEvent(event))){discardDatePlan();return;}
    const auto dates=m_plannedDates;m_message=tr("Zastosowano plan dat. Możesz cofnąć zmianę.");applyDates(dates);
}
void QuickCalendar::assignDate(const QString &iso)
{
    planDates(iso,"same",1);applyDatePlan();
}
void QuickCalendar::shiftDates(int days)
{
    sync();if(!datesEnabled() || !editable() || qAbs(qint64(days))>36600)return;
    QHash<CompetitionInfo *,QDate> dates;
    for(auto *event:selected()){
        auto parts=event->getTrainings();parts.append(event);if(event->getTrialRound())parts.append(event->getTrialRound());
        for(auto *part:parts)if(part->getScheduledDate().isValid()){
            const auto shifted=part->getScheduledDate().addDays(days);
            if(shifted.year()<1 || shifted.year()>9999){fail(tr("Przesunięcie wykracza poza obsługiwany zakres dat."));return;}
            dates.insert(part,shifted);
        }
    }
    if(dates.isEmpty()){fail(tr("Zaznaczone konkursy nie mają jeszcze dat."));return;}
    m_message=tr("Przesunięto terminy z zachowaniem odstępów.");applyDates(dates);
}
void QuickCalendar::clearDates()
{
    sync();if(!datesEnabled() || !editable())return;
    QHash<CompetitionInfo *,QDate> dates;
    for(auto *event:selected()){dates.insert(event,{});for(auto *training:event->getTrainings())dates.insert(training,{});if(event->getTrialRound())dates.insert(event->getTrialRound(),{});}
    m_message=tr("Usunięto daty zaznaczonych konkursów. Możesz cofnąć zmianę.");
    // Clear after normalization's inheritance step, which also sees invalid parent dates.
    applyDates(dates);
}
void QuickCalendar::selectUndated()
{
    sync();m_selected.clear();for(auto *event:mains())if(!locked(event) && !event->getScheduledDate().isValid())m_selected.append(event->getIDStr());refresh();
}
void QuickCalendar::sortDates()
{
    sync();if(!datesEnabled())return;
    auto order=mains();int boundary=0;
    for(int i=0;i<order.size();++i)if(locked(order[i]))boundary=i+1;
    std::stable_sort(order.begin()+boundary,order.end(),[](auto *a,auto *b){const auto x=a->getScheduledDate(),y=b->getScheduledDate();return x.isValid() && (!y.isValid() || x<y);});
    m_message=tr("Uporządkowano przyszłe konkursy według dat. Sprawdź powiązania kwalifikacji.");mutate([&]{setOrder(order);});
}
