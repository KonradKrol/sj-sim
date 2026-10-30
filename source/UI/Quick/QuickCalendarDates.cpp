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
    QDate date=QDate::fromString(text,"dd.MM.yyyy");
    if(!date.isValid())date=QDate::fromString(text,Qt::ISODate);
    if(!date.isValid())date=QLocale().toDate(text,QLocale::ShortFormat);
    if(!date.isValid())date=QLocale().toDate(text,QLocale::LongFormat);
    return date.isValid() && date.year()>0 && date.year()<=9999?date.toString(Qt::ISODate):QString();
}
QString QuickCalendar::dateText(const QString &iso) const
{
    const auto date=QDate::fromString(iso,Qt::ISODate);
    return date.isValid()?date.toString("dd.MM.yyyy"):QString();
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
        if(previous.isValid() && date<previous)warnings.append(tr("Konkurs %1 ma datę wcześniejszą niż poprzedni konkurs. Zmień daty lub kolejność — konkursy odbywają się w kolejności listy.").arg(number));
        previous=date;
        const auto venue=event->getHill()?event->getHill()->getName():QString();
        if(venues.contains(date) && venues.value(date)!=venue)warnings.append(tr("%1: konkursy odbywają się w różnych miejscowościach tego samego dnia. Sprawdź, czy taki plan jest zamierzony.").arg(dateText(date.toString(Qt::ISODate))));
        venues.insert(date,venue);
        auto *source=event->getAdvancementCompetition();
        if(source && source->getScheduledDate().isValid() && source->getScheduledDate()>date)warnings.append(tr("Kwalifikacje do konkursu %1 zaplanowano po konkursie. Zmień ich datę.").arg(number));
        for(auto *training:event->getTrainings())if(training->getScheduledDate().isValid() && training->getScheduledDate()>date){warnings.append(tr("Trening do konkursu %1 zaplanowano po konkursie. Zmień jego datę.").arg(number));break;}
    }
    if(missing)warnings.prepend(tr("%1 konkursów nie ma daty. Możesz ustalić ich terminy w szybkim planowaniu.").arg(missing));
    return warnings;
}
void QuickCalendar::discardDatePlan()
{
    if(m_datePlan.isEmpty() && m_plannedDates.isEmpty())return;
    m_datePlan.clear();m_plannedDates.clear();emit changed();
}
void QuickCalendar::planDates(const QString &start,const QString &pattern,int interval,bool future,int trainingDays,bool onlyMissing)
{
    commitPendingRules();sync();discardDatePlan();discardWeekend();if(!datesEnabled())return;
    QDate date=QDate::fromString(normalizeDate(start),Qt::ISODate);
    if(!date.isValid() || interval<1 || interval>366 || trainingDays<0 || trainingDays>14 || !QStringList{"interval","weekends","fri-sun","weekly","same","preserve"}.contains(pattern)){
        fail(tr("Wybierz poprawną datę i odstęp od 1 do 366 dni."));return;
    }
    auto targets=selected();
    if(future){targets.clear();for(auto *event:mains())if(!locked(event))targets.append(event);}
    if(onlyMissing)targets.erase(std::remove_if(targets.begin(),targets.end(),[](auto *event){return event->getScheduledDate().isValid();}),targets.end());
    if(onlyMissing && targets.isEmpty()){fail(tr("Wszystkie wybrane konkursy mają już daty."));return;}
    if(targets.isEmpty() || std::any_of(targets.begin(),targets.end(),[&](auto *e){return locked(e);})){fail(tr("Wybierz nierozegrane konkursy do zaplanowania."));return;}
    qint64 shift=0;
    if(pattern=="preserve"){
        for(auto *event:targets)if(!event->getScheduledDate().isValid()){fail(tr("Aby zachować odstępy, najpierw nadaj daty wszystkim wybranym konkursom."));return;}
        shift=targets.first()->getScheduledDate().daysTo(date);
    }
    if(pattern=="weekends" || pattern=="fri-sun")date=date.addDays(((pattern=="fri-sun"?5:6)-date.dayOfWeek()+7)%7);
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
        date=date.addDays(pattern=="weekends"?(date.dayOfWeek()==6?1:6):pattern=="fri-sun"?(date.dayOfWeek()<7?1:5):pattern=="weekly"?7:pattern=="same"?0:interval);
    }
    for(const auto &value:m_plannedDates)if(!value.isValid() || value.year()<1 || value.year()>9999){discardDatePlan();fail(tr("Plan wykracza poza obsługiwany zakres dat."));return;}
    if(!validateDateOrder(mains(),m_plannedDates)){discardDatePlan();return;}
    m_message=tr("Podgląd: %1 konkursów. Terminy zmienią się po zastosowaniu.").arg(targets.size());emit changed();
}
bool QuickCalendar::validateDateOrder(const QVector<CompetitionInfo *> &order,const QHash<CompetitionInfo *,QDate> &dates)
{
    if(!datesEnabled())return true;
    QDate previous;int previousIndex=0;
    for(int i=0;i<order.size();++i) {
        const auto date=dates.value(order[i],order[i]->getScheduledDate());if(!date.isValid())continue;
        if(previous.isValid() && date<previous) {
            fail(tr("Konkurs %1 (%2) nie może być przed konkursem %3 (%4). Popraw daty lub kolejność.").arg(i+1).arg(dateText(date.toString(Qt::ISODate))).arg(previousIndex+1).arg(dateText(previous.toString(Qt::ISODate))));return false;
        }
        QDate sessionDate=previous;
        auto sessions=order[i]->getTrainings();if(order[i]->getTrialRound())sessions.append(order[i]->getTrialRound());
        for(auto *session:sessions) {
            const auto value=dates.value(session,session->getScheduledDate());if(!value.isValid())continue;
            if(value>date || (sessionDate.isValid() && value<sessionDate)) {
                fail(tr("Trening lub seria próbna konkursu %1 musi mieścić się między poprzednim konkursem a tym konkursem.").arg(i+1));return false;
            }
            sessionDate=value;
        }
        previous=date;previousIndex=i;
    }
    return true;
}
void QuickCalendar::applyDates(const QHash<CompetitionInfo *,QDate> &dates)
{
    if(!validateDateOrder(mains(),dates))return;
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

void QuickCalendar::discardWeekend(){if(m_weekendPreview.isEmpty() && m_weekendPlan.calendar.isEmpty())return;m_weekendPreview.clear();m_weekendPlan=WeekendPlan{};emit changed();}
void QuickCalendar::previewWeekend(const QString &start,int hillIndex,int rulesIndex,bool saturdayQualification,bool sharedSunday){
    commitPendingRules();sync();discardDatePlan();discardWeekend();if(!datesEnabled() || !m_calendar || !m_editor)return;
    auto date=QDate::fromString(normalizeDate(start),Qt::ISODate);const auto *hills=m_editor->getModel()->getHillsList();const auto *rules=m_editor->getModel()->getRulesList();
    if(!date.isValid() || hillIndex<0 || hillIndex>=hills->size() || rulesIndex<0 || rulesIndex>=rules->size() || rules->at(rulesIndex).getRounds().isEmpty()){fail(tr("Wybierz datę, skocznię i zasady z co najmniej jedną serią."));return;}
    date=date.addDays((5-date.dayOfWeek()+7)%7);if(!date.isValid() || !date.addDays(2).isValid() || date.addDays(2).year()>9999){fail(tr("Weekend wykracza poza obsługiwany zakres dat."));return;}
    m_weekendPlan.calendar=m_calendar->getIDStr();m_weekendPlan.friday=date;m_weekendPlan.hill=hills->at(hillIndex)->getIDStr();m_weekendPlan.rules=rules->at(rulesIndex).getIDStr();m_weekendPlan.definition=rules->at(rulesIndex);m_weekendPlan.saturdayQualification=saturdayQualification;m_weekendPlan.sharedSunday=sharedSunday;
    const QStringList descriptions{saturdayQualification?tr("Bez wydarzeń"):tr("2 treningi · kwalifikacje"),saturdayQualification?tr("Kwalifikacje · konkurs"):tr("Seria próbna · konkurs"),sharedSunday?tr("Seria próbna · konkurs"):tr("Kwalifikacje · konkurs")};
    for(int day=0;day<3;++day)m_weekendPreview.append(QVariantMap{{"day",day},{"date",dateText(date.addDays(day).toString(Qt::ISODate))},{"events",descriptions[day]},{"qualification",day==2 && sharedSunday?tr("Awans z pierwszych kwalifikacji"):QString()}});
    m_message=tr("Podgląd nowego weekendu: dwa konkursy i kwalifikacje.");emit changed();
}
bool QuickCalendar::applyWeekend(){
    commitPendingRules();sync();if(!datesEnabled() || !m_calendar || m_weekendPlan.calendar!=m_calendar->getIDStr() || m_weekendPreview.isEmpty())return false;
    const auto plan=m_weekendPlan;Hill *hill=nullptr;for(auto *candidate:*m_editor->getModel()->getHillsList())if(candidate->getIDStr()==plan.hill){hill=candidate;break;}
    bool validRules=false;for(const auto &candidate:*m_editor->getModel()->getRulesList())if(candidate.getIDStr()==plan.rules && CompetitionRules::getJsonObject(candidate)==CompetitionRules::getJsonObject(plan.definition)){validRules=true;break;}
    if(!hill || !validRules){discardWeekend();fail(tr("Skocznia lub zasady zmieniły się. Przygotuj podgląd ponownie."));return false;}
    const auto chosen=selected();
    auto proposed=mains();int insertion=proposed.size();if(!chosen.isEmpty() && !locked(chosen.last()))insertion=proposed.indexOf(chosen.last())+1;
    CompetitionInfo firstDate(hill),lastDate(hill);firstDate.setScheduledDate(plan.friday.addDays(plan.saturdayQualification?1:0));lastDate.setScheduledDate(plan.friday.addDays(2));
    proposed.insert(insertion,&firstDate);proposed.insert(insertion+1,&lastDate);if(!validateDateOrder(proposed))return false;
    m_message=tr("Dodano weekend. Dwa konkursy są zaznaczone; możesz przypisać im klasyfikacje.");
    mutate([&]{
        auto order=mains();int at=order.size();if(!chosen.isEmpty() && !locked(chosen.last()))at=order.indexOf(chosen.last())+1;
        CompetitionRules session=plan.definition;session.setRounds({RoundInfo(qMax(1,m_save?int(m_save->getJumpersReference().size()):plan.definition.getRounds().first().getCount()),true)});
        const auto event=[&](int kind,const QDate &date,const CompetitionRules &rules){auto *entry=new CompetitionInfo(hill);entry->setSerieType(kind);entry->setScheduledDate(date);entry->setRules(rules);return entry;};
        auto *firstQualification=event(CompetitionInfo::Qualifications,plan.friday.addDays(plan.saturdayQualification?1:0),session);
        if(!plan.saturdayQualification){QVector<CompetitionInfo *> trainings;for(int i=0;i<2;++i)trainings.append(event(CompetitionInfo::Training,plan.friday,session));firstQualification->setTrainings(trainings);}
        auto *saturday=event(CompetitionInfo::Competition,plan.friday.addDays(1),plan.definition);saturday->setAdvancementCompetition(firstQualification);if(!plan.saturdayQualification)saturday->setTrialRound(event(CompetitionInfo::TrialRound,plan.friday.addDays(1),session));
        order.insert(at++,firstQualification);order.insert(at++,saturday);
        auto *sundayQualification=firstQualification;if(!plan.sharedSunday){sundayQualification=event(CompetitionInfo::Qualifications,plan.friday.addDays(2),session);order.insert(at++,sundayQualification);}
        auto *sunday=event(CompetitionInfo::Competition,plan.friday.addDays(2),plan.definition);sunday->setAdvancementCompetition(sundayQualification);if(plan.sharedSunday)sunday->setTrialRound(event(CompetitionInfo::TrialRound,plan.friday.addDays(2),session));order.insert(at,sunday);
        m_selected=QStringList{saturday->getIDStr(),sunday->getIDStr()};setOrder(order);
    });
    return true;
}
