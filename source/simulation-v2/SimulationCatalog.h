#pragma once

#include "ProfilePackRepository.h"

namespace SimulationV2 {

struct CatalogPack {
    DataPack data;
    QString directory;
    QString migrationNotice;
};

struct CatalogSnapshot {
    QVector<CatalogPack> packs;
    QStringList active{QString(),QString(),QString()};
    QStringList issues;
    bool selectionWritable=true;
    const CatalogPack *find(DataPack::Kind,const QString &id) const;
    const CatalogPack *selected(DataPack::Kind kind) const { return find(kind,active.value(int(kind))); }
};

struct CatalogPaths {
    QString athletes, hills, selection, legacyPhotos;
    static CatalogPaths defaults();
};

// Profile files are authoritative. The selection file stores only order/IDs;
// rule packs are read from the compatible legacy rules source supplied by caller.
class SimulationCatalog {
public:
    explicit SimulationCatalog(CatalogPaths = CatalogPaths::defaults());
    CatalogSnapshot load(const QVector<DataPack> &legacyPacks,double legacyMaximumSkills,
                         const QStringList &legacyActive={}) const;
    bool select(CatalogSnapshot &,DataPack::Kind,const QString &id,QString &error) const;
private:
    CatalogPaths paths;
    bool saveSelection(const CatalogSnapshot &,QString &error) const;
};

}
