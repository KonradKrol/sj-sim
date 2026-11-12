#ifndef SIMULATIONSAVELISTORDER_H
#define SIMULATIONSAVELISTORDER_H

#include "SimulationSave.h"
#include <QHash>
#include "../global/UserDataPaths.h"
#include "../global/AtomicJsonFile.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <algorithm>
#include <limits>

namespace SimulationSaveListOrder {
inline void remember(const QVector<SimulationSave *> &saves) {
    QStringList names;
    for(const auto *save:saves)if(save)names.append(save->getName());
    AtomicJsonFile::write(UserDataPaths::path("Saves/.order"),QJsonDocument(QJsonObject{{"names",QJsonArray::fromStringList(names)}}));
}
inline void restore(QVector<SimulationSave *> &saves) {
    QFile order(UserDataPaths::path("Saves/.order"));QStringList names;
    if(order.open(QIODevice::ReadOnly))for(const auto &name:QJsonDocument::fromJson(order.readAll()).object()["names"].toArray())names.append(name.toString());
    QHash<QString,int> ranks;
    for(int i=0;i<names.size();++i)if(!ranks.contains(names[i]))ranks.insert(names[i],i);
    const auto rank=[&](const SimulationSave *save){return ranks.value(save?save->getName():QString(),std::numeric_limits<int>::max());};
    std::stable_sort(saves.begin(),saves.end(),[&](const SimulationSave *a,const SimulationSave *b){return rank(a)<rank(b);});
}
}
#endif
