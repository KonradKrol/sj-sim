#pragma once
#include "../seasons/SeasonCalendar.h"
#include "../seasons/Classification.h"

namespace StartListOrder {
struct Source { CompetitionInfo *competition=nullptr; Classification *classification=nullptr; };
inline Source resolve(SeasonCalendar *calendar,CompetitionInfo *target,int kind) {
    Source source;if(!calendar || !target)return source;
    if(target->startListSortMode==1) {
        source.competition=target->getAdvancementCompetition();
        if(!source.competition)source.classification=target->getAdvancementClassification();
    } else if(target->startListSortMode==2) {
        for(auto *event:calendar->getCompetitions()) {
            if(event==target)break;
            if(event && (target->startListSortCompetitionId.isEmpty() || event->getIDStr()==target->startListSortCompetitionId)
                && event->getRules().getCompetitionType()==kind)source.competition=event;
        }
    } else if(target->startListSortMode==3) {
        for(auto *ranking:calendar->getClassifications())if(ranking && ranking->getIDStr()==target->startListSortClassificationId
            && ranking->getClassificationType()==kind)source.classification=ranking;
    }
    if(source.competition && (source.competition->getCancelled() || !source.competition->getPlayed()
        || source.competition->getRules().getCompetitionType()!=kind || source.competition->getResultsReference().getResultsReference().isEmpty()))source.competition=nullptr;
    if(source.classification && (source.classification->getClassificationType()!=kind || source.classification->getRankedResults().isEmpty()))source.classification=nullptr;
    if(!source.competition && !source.classification)
        for(auto *ranking:calendar->getClassifications())if(ranking && ranking->getClassificationType()==kind){source.classification=ranking;break;}
    return source;
}
}
