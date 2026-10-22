#ifndef QUICKRACESTATS_H
#define QUICKRACESTATS_H

#include "../../competitions/CompetitionResults.h"
#include <QVariantMap>
#include <algorithm>
#include <functional>

namespace QuickRaceStats {
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
        double points = 0;
        for (int i = 0; i < previousCount; ++i)
            points += jumps[i].getPoints();
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
