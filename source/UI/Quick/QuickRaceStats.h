#ifndef QUICKRACESTATS_H
#define QUICKRACESTATS_H

#include "../../competitions/CompetitionResults.h"
#include <QVariantMap>
#include <algorithm>
#include <functional>
#include <limits>
#include <cmath>

namespace QuickRaceStats {
inline QVariantMap roundStartGap(const CompetitionSingleResult &athlete,const QVector<const CompetitionSingleResult *> &rivals,int round) {
    if(round<2 || !athlete.getCompetition())return {};
    const auto weights=athlete.getCompetition()->getRulesPointer()->getRoundPointWeights(round);
    bool carriesPoints=false;for(int i=0;i<round-1;++i)if(std::abs(weights.value(i,1.))>1e-9)carriesPoints=true;
    if(!carriesPoints)return {};
    const auto *rules=athlete.getCompetition()->getRulesPointer();
    const int perRound=athlete.getType()==CompetitionSingleResult::TeamResult?qMax(1,rules->getJumpersInTeamCount()):1;
    const int previousCount=(round-1)*perRound;
    if(athlete.getJumps().size()<previousCount)return {};
    const auto own=qRound64(athlete.pointsForJumps(previousCount,round)*10.);
    qint64 best=std::numeric_limits<qint64>::min();
    for(const auto *rival:rivals)if(rival && rival!=&athlete && !rival->isTeamDisqualified() && rival->getJumps().size()>=previousCount)best=qMax(best,qRound64(rival->pointsForJumps(previousCount,round)*10.));
    return best==std::numeric_limits<qint64>::min()?QVariantMap():QVariantMap{{"gap",(best-own)/10.}};
}

inline QVariantMap currentLeaderGap(const CompetitionSingleResult &athlete,const QVector<const CompetitionSingleResult *> &rivals,int round) {
    if(roundStartGap(athlete,rivals,round).isEmpty())return {};
    const auto *rules=athlete.getCompetition()->getRulesPointer();
    const int perRound=athlete.getType()==CompetitionSingleResult::TeamResult?qMax(1,rules->getJumpersInTeamCount()):1;
    const auto own=qRound64(athlete.pointsForJumps(athlete.getJumps().size(),round)*10.);
    qint64 best=std::numeric_limits<qint64>::min();
    for(const auto *rival:rivals)if(rival && rival!=&athlete && !rival->isTeamDisqualified() && rival->getJumps().size()>=(round-1)*perRound)best=qMax(best,qRound64(rival->pointsForJumps(rival->getJumps().size(),round)*10.));
    return best==std::numeric_limits<qint64>::min()?QVariantMap():QVariantMap{{"gap",(best-own)/10.}};
}

inline QVariantMap previousStanding(const CompetitionResults &results, const CompetitionSingleResult &athlete, int jumpCount) {
    if(jumpCount < 1 || athlete.getJumps().size() < jumpCount) return {};
    const auto score = [jumpCount](const CompetitionSingleResult &entry) {
        return qRound64(entry.pointsForJumps(jumpCount) * 10);
    };
    const auto own = score(athlete);
    qint64 bestRival = std::numeric_limits<qint64>::min();
    int position = 1;
    for(const auto &entry : results.getResults()) {
        if(entry.getJumps().size() < jumpCount || (entry.getJumper() == athlete.getJumper() && entry.getTeam() == athlete.getTeam())) continue;
        const auto rival = score(entry);
        bestRival = qMax(bestRival, rival);
        if(rival > own) ++position;
    }
    QVariantMap standing{{"position", position}};
    if(bestRival != std::numeric_limits<qint64>::min()) standing["gap"] = (bestRival - own) / 10.;
    return standing;
}

inline QVariantMap biggestAdvancement(const CompetitionResults &results, int round, int jumpsPerRound, int remainingCompetitors = 0) {
    if (round < 2 || jumpsPerRound < 1)
        return {};
    const int previousCount = (round - 1) * jumpsPerRound;
    struct Candidate { const CompetitionSingleResult *result; qint64 previousPoints; };
    QVector<Candidate> candidates;
    QVector<qint64> scores;
    const auto resultsSnapshot = results.getResults();
    for (const auto &result : resultsSnapshot) {
        const auto jumps = result.getJumps();
        if (jumps.size() < previousCount)
            continue;
        const double points = result.pointsForJumps(previousCount);
        const auto score = qRound64(points * 10);
        scores.append(score);
        if (jumps.size() > previousCount && result.getPosition() > 0)
            candidates.append({&result, score});
    }
    std::sort(scores.begin(), scores.end(), std::greater<qint64>());
    int best = 0;
    QVariantList entries;
    for (const auto &candidate : candidates) {
        const auto *result = candidate.result;
        const int previous = int(std::lower_bound(scores.cbegin(), scores.cend(), candidate.previousPoints, std::greater<qint64>()) - scores.cbegin()) + 1;
        const int correctedPosition = result->getPosition() + qMax(0, remainingCompetitors);
        const int places = previous - correctedPosition;
        const QString name = result->getTeam() ? result->getTeam()->getCountryCode() : result->getJumper() ? result->getJumper()->getNameAndSurname() : QString();
        if (places <= 0 || places < best || name.isEmpty())
            continue;
        if (places > best) { best = places; entries.clear(); }
        entries.append(QVariantMap{{"name", name}, {"from", previous}, {"to", correctedPosition}});
    }
    if (entries.isEmpty())
        return {};
    std::sort(entries.begin(), entries.end(), [](const QVariant &a, const QVariant &b) {
        const auto left = a.toMap(), right = b.toMap();
        if (left.value("to") != right.value("to"))
            return left.value("to").toInt() < right.value("to").toInt();
        return QString::localeAwareCompare(left.value("name").toString(), right.value("name").toString()) < 0;
    });
    return {{"places", best}, {"name", entries.first().toMap().value("name")}, {"entries", entries}};
}
}

#endif
