#pragma once

#include "../global/DataPack.h"
#include <QPair>
#include <QStringList>

namespace SimulationV2 {

struct PackMigration {
    DataPack pack;
    QString warning;
    // Source image name and destination path relative to the new pack folder.
    QVector<QPair<QString,QString>> photos;
};

// Creates a separate pack; never mutates the source or ready v2 profiles.
bool approximateLegacyPack(const DataPack &, double legacyMaximumSkills, PackMigration &, QString &error);

}
