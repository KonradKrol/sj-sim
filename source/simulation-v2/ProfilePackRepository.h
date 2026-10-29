#pragma once

#include "ProfileMigration.h"
#include <QJsonObject>
#include <QStringList>

namespace SimulationV2 {

enum class ProfilePackKind { Athletes, Hills };

struct ProfilePackListing {
    QVector<QJsonObject> packs;
    QStringList issues;
    bool needsPerkConversion=false;
};

// Canonical authoring-file persistence shared by studios and the game catalog.
// It never writes legacy data files or keeps a second editable profile model.
class ProfilePackRepository {
public:
    ProfilePackRepository(ProfilePackKind, QString root);
    ProfilePackListing load() const;
    bool read(const QString &file, QJsonObject &, QString &error) const;
    bool save(const QJsonObject &, QString &error, bool validateProfiles=true) const;
    bool validate(const QJsonObject &, QString &error) const;
    QString filePath(const QJsonObject &) const;
    QString packDirectory(const QJsonObject &) const;
    // Idempotent by source pack ID. New folders/files are published atomically.
    // Missing optional photos are reported; existing ready packs are untouched.
    bool migrate(const DataPack &, double maximumSkills, const QString &legacyPhotoRoot,
                 QJsonObject &createdOrExisting, QStringList &issues, QString &error) const;
private:
    ProfilePackKind kind;
    QString root;
    bool validateEnvelope(const QJsonObject &, QString &error) const;
    bool convertPerkLevels(QJsonObject &, bool &changed, QString &error) const;
};

}
