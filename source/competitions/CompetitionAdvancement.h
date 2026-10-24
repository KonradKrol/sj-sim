#ifndef COMPETITIONADVANCEMENT_H
#define COMPETITIONADVANCEMENT_H

#include "StartListCompetitorStatus.h"
#include <QVector>

namespace CompetitionAdvancement {
struct Entry {
    const void *competitor = nullptr;
    qint64 points = 0;
    int round = 1;
    int maximumRound = 1;
    int order = 0;
    bool pending = true;
    bool excluded = false;
    bool protectedByDistance = false;
};

inline bool better(const Entry &left, const Entry &right, bool includeTies) {
    if(left.round != right.round)return left.round > right.round;
    if(left.points != right.points)return left.points > right.points;
    return !includeTies && left.order < right.order;
}

inline int status(const Entry &candidate, const QVector<Entry> &field, int limit, bool includeTies = true) {
    if(limit < 0)return StartListCompetitorStatus::Waiting;
    if(candidate.excluded || limit == 0)return StartListCompetitorStatus::SureDroppedOut;
    if(candidate.pending)return StartListCompetitorStatus::Waiting;
    if(candidate.protectedByDistance && includeTies)return StartListCompetitorStatus::SureAdvanced;
    int bestPosition = 1, worstPosition = 1;
    for(const auto &other : field) {
        if(other.competitor == candidate.competitor || other.excluded)continue;
        if(other.pending) {
            if(other.maximumRound >= candidate.round)++worstPosition;
        } else if(better(other,candidate,includeTies)) {
            ++bestPosition;++worstPosition;
        }
    }
    if(bestPosition > limit)return StartListCompetitorStatus::SureDroppedOut;
    if(worstPosition <= limit)return StartListCompetitorStatus::SureAdvanced;
    return StartListCompetitorStatus::Waiting;
}

enum ModelRole {
    RoundStatusRole = Qt::UserRole + 100,
    QualificationStatusRole,
    QualificationTargetRole
};
}

#endif
