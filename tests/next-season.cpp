#include <QApplication>
#include <QDebug>
#include "../source/seasons/SimulationSave.h"
#include "../source/athletes/AthleteProfile.h"
#include "../source/hills/HillProfile.h"
#include "../source/global/JsonGraphValidation.h"
#include <cstdlib>
#include <memory>

static void check(bool ok,const char *message){if(!ok){qCritical()<<message;std::exit(1);}}
int main(int argc,char **argv){
    QApplication app(argc,argv);
    SimulationSave save;save.setEngineVersion(2);save.setName("Next season test");
    auto *jumper=new Jumper;auto athlete=AthleteProfile::create();athlete.form=83;QString error;
    check(jumper->setSimulationProfile(athlete,error),"athlete profile");save.setJumpers({jumper});
    auto *hill=new Hill;check(hill->setSimulationProfile(HillProfile::create(),error),"hill profile");save.setHills({hill});
    CompetitionRules rules;rules.setName("Retained rules");rules.setRounds({RoundInfo(1,true)});save.setCompetitionRules({rules});
    auto *summer=new SeasonCalendar("Lato");auto *winter=new SeasonCalendar("Zima");
    auto *ranking=new Classification;ranking->setName("Ranking");summer->setClassifications({ranking});
    auto make=[&](QDate date){auto *event=new CompetitionInfo(hill);event->setRules(rules);event->getRulesPointer()->reassign();event->setScheduledDate(date);event->setPlayed(true);return event;};
    auto *training=make(QDate(2026,6,1));training->setSerieType(CompetitionInfo::Training);
    auto *qualifications=make(QDate(2026,6,1));qualifications->setSerieType(CompetitionInfo::Qualifications);
    auto *competition=make(QDate(2026,6,2));competition->setTrainings({training});competition->setAdvancementCompetition(qualifications);competition->setClassifications({ranking});
    competition->startListSortMode=2;competition->startListSortCompetitionId=qualifications->getIDStr();competition->startListSortClassificationId=ranking->getIDStr();
    ClassificationCompetitionSettings scoring;scoring.parameters["multiplier"]=2;competition->setScoringFor(ranking,scoring);
    CompetitionSingleResult result;result.setJumper(jumper);result.setCompetition(competition);competition->getResultsReference().getResultsReference().append(result);
    auto *winterEvent=make(QDate(2027,2,28));winterEvent->setCancelled(true);winterEvent->setPlayed(false);
    summer->setCompetitions({qualifications,training,competition});SaveJumpersList roster;roster.setName("Retained roster");roster.setJumpers({jumper});save.getJumpersListsReference().append(roster);summer->setJumpersListId(roster.getId());winter->setCompetitions({winterEvent});
    Season source;source.setSeasonNumber(2026);source.setDatesEnabled(true);source.setCalendars({winter,summer});source.setActualCalendar(winter);
    save.setSeasons({source});save.setActualSeason(&save.getSeasonsReference().first());
    const auto old=Season::getJsonObject(source);Season next;
    check(source.prepareNext(true,next,error),qPrintable(error));
    check(Season::getJsonObject(source)==old,"source history unchanged");
    check(next.getSeasonNumber()==2027 && next.getDatesEnabled() && !next.hasStarted(),"new season metadata");
    check(next.getCalendars().size()==2 && next.getActualCalendar()->getName()=="Lato","earliest calendar selected across calendars");
    auto *newSummer=next.getCalendars()[1];auto *newCompetition=newSummer->getCompetitions().last();auto *newRanking=newSummer->getClassifications().first();
    check(newSummer!=summer && newSummer->getID()!=summer->getID() && newSummer->jumpersListId()==roster.getId(),"independent calendars and retained roster");
    check(newCompetition!=competition && newCompetition->getHill()==hill && newCompetition->getID()!=competition->getID(),"fresh event identity and same hill");
    check(newCompetition->getResultsReference().getResults().isEmpty() && newCompetition->getStartList().isEmpty(),"results and participants cleared");
    check(!newCompetition->getPlayed() && !newCompetition->getCancelled() && newRanking->getResultsReference().isEmpty(),"new contest and classification start empty");
    check(newCompetition->getAdvancementCompetition()==newSummer->getCompetitions().first(),"qualification points to copied event");
    check(newCompetition->getTrainings().first()!=training && newCompetition->getTrainings().first()->getScheduledDate()==QDate(2027,6,1),"training remapped and dates shifted");
    check(newCompetition->startListSortCompetitionId==newCompetition->getAdvancementCompetition()->getIDStr() && newCompetition->startListSortClassificationId==newRanking->getIDStr(),"sorting references remapped");
    check(newCompetition->getClassifications().first()==newRanking && newCompetition->scoringFor(newRanking).parameters["multiplier"].toInt()==2,"classification and custom scoring remapped");
    save.getSeasonsReference().append(next);save.setActualSeason(&save.getSeasonsReference().last());save.updateNextCompetitionIndex();
    check(jumper->getSimulationProfile()->form==83 && save.getCompetitionRulesReference().first().getID()==rules.getID(),"athlete form and source rules preserved");
    const auto json=SimulationSave::getJsonObject(save);check(JsonGraphValidation::validateSave(json,error),qPrintable(error));
    IdentifiableObjectsStorage loadedStorage;std::unique_ptr<SimulationSave> loaded(SimulationSave::getFromJson(json,&loadedStorage));
    check(loaded && loaded->getSeasonsReference().size()==2 && loaded->getActualSeason()->getSeasonNumber()==2027,"two seasons reload with the new active season");
    check(loaded->getActualSeason()->getCalendars()[1]->getCompetitions().last()->getAdvancementCompetition()==loaded->getActualSeason()->getCalendars()[1]->getCompetitions().first(),"copied qualification survives save reload");
    Season empty;check(source.prepareNext(false,empty,error) && empty.getCalendars().isEmpty() && empty.getDatesEnabled(),"empty season option");
    competition->setPlayed(false);Season untouched;untouched.setSeasonNumber(42);
    check(!source.prepareNext(true,untouched,error) && untouched.getSeasonNumber()==42,"unfinished season rejected without changing output");competition->setPlayed(true);
    competition->setScheduledDate(QDate(2026,5,1));check(!source.prepareNext(true,untouched,error) && untouched.getSeasonNumber()==42,"invalid copied chronology rejected transactionally");
    qInfo()<<"PASS: next season cloning, dates, history, qualification, scoring, roster, identity, empty mode and failure rollback";
}
