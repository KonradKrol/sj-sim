#pragma once
#include <QSettings>

namespace SettingsMigrations {
inline void apply210(QSettings &settings)
{
    const QString marker=QStringLiteral("migrations/2.1.0/distanceSpreadStandard");
    if(settings.value(marker,false).toBool())return;
    settings.setValue(QStringLiteral("simulation/simple/distanceSpread"),2);
    settings.setValue(marker,true);
    settings.sync();
}
}
