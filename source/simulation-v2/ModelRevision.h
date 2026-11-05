#pragma once
#include <QJsonArray>
#include <QJsonObject>

namespace SimulationV2 {
// Increment whenever algorithms change the meaning of a frozen competition.
inline constexpr int modelRevision=1;
inline bool canResumeSave(const QJsonObject &save) {
    if(save.value("engine-version").toInt(1)!=2)return true;
    for(const auto &season:save.value("seasons").toArray())
        for(const auto &calendar:season.toObject().value("calendars").toArray())
            for(const auto &value:calendar.toObject().value("competitions").toArray()) {
                const auto event=value.toObject();
                const bool underway=!event.value("played").toBool() && !event.value("cancelled").toBool()
                    && !event.value("results").toObject().value("results").toArray().isEmpty();
                if(underway && event.value("simulation-model-revision").toInt(-1)!=modelRevision)return false;
            }
    return true;
}
}
