#ifndef SIMULATIONSAVELISTORDER_H
#define SIMULATIONSAVELISTORDER_H

#include "SimulationSave.h"
#include <QHash>
#include <QSettings>
#include <algorithm>
#include <limits>

namespace SimulationSaveListOrder {
inline void remember(const QVector<SimulationSave *> &saves) {
    QStringList names;
    for(const auto *save:saves)if(save)names.append(save->getName());
    QSettings().setValue("simulationSaves/order",names);
}
inline void restore(QVector<SimulationSave *> &saves) {
    const auto names=QSettings().value("simulationSaves/order").toStringList();
    QHash<QString,int> ranks;
    for(int i=0;i<names.size();++i)if(!ranks.contains(names[i]))ranks.insert(names[i],i);
    const auto rank=[&](const SimulationSave *save){return ranks.value(save?save->getName():QString(),std::numeric_limits<int>::max());};
    std::stable_sort(saves.begin(),saves.end(),[&](const SimulationSave *a,const SimulationSave *b){return rank(a)<rank(b);});
}
}
#endif
