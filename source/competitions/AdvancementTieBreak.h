#pragma once
#include <QHash>
#include <QPair>
#include <QString>
#include <QVector>
#include <algorithm>
#include <limits>
#include "CompetitionInfo.h"
#include "../seasons/Classification.h"
#include "../seasons/ClassificationSingleResult.h"
#include "../simulator/Jumper.h"
#include "../simulator/Team.h"

// Deterministic order of competitors with equal points, used when a round needs an
// exact number of advancers (for example before a KO round) and a tie would exceed it.
// 1. The competitor placed higher in the first classification of the competition that
//    matches the subject (jumpers use an individual classification, teams a team one).
// 2. Otherwise the competitor ID (country code for teams), so the result never depends
//    on the order in which the results happened to be stored.
namespace AdvancementTieBreak {
using Key = QPair<int,QString>;

inline Key key(CompetitionInfo *competition,Jumper *jumper) {
    Key result{std::numeric_limits<int>::max(),jumper?jumper->getIDStr():QString()};
    if(!competition || !jumper)return result;
    for(auto *ranking:competition->getClassifications()) {
        if(!ranking || ranking->getClassificationType()!=Classification::Individual)continue;
        if(auto *entry=ranking->getResultOfIndividualJumper(jumper);entry && entry->getPosition()>0)result.first=entry->getPosition();
        break;
    }
    return result;
}

inline Key key(CompetitionInfo *competition,Team *team) {
    Key result{std::numeric_limits<int>::max(),team?team->getCountryCode():QString()};
    if(!competition || !team)return result;
    for(auto *ranking:competition->getClassifications()) {
        if(!ranking || ranking->getClassificationType()!=Classification::Team)continue;
        if(auto *entry=ranking->getResultOfTeam(team->getCountryCode());entry && entry->getPosition()>0)result.first=entry->getPosition();
        break;
    }
    return result;
}

// Rank 0 advances first. Ranks are unique inside the given field.
template<class T>
inline QHash<const void *,int> ranks(CompetitionInfo *competition,const QVector<T *> &field) {
    QVector<QPair<Key,const void *>> keyed;
    keyed.reserve(field.size());
    for(auto *competitor:field)keyed.append({key(competition,competitor),static_cast<const void *>(competitor)});
    std::sort(keyed.begin(),keyed.end(),[](const auto &left,const auto &right){
        if(left.first!=right.first)return left.first<right.first;
        return left.second<right.second;
    });
    QHash<const void *,int> result;
    for(int i=0;i<keyed.size();++i)result.insert(keyed[i].second,i);
    return result;
}
}
