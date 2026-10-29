#ifndef PLAYGROUNDEXPERIMENT_H
#define PLAYGROUNDEXPERIMENT_H
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <atomic>
#include <memory>
#include <mutex>

namespace Playground {
struct Control {
    std::atomic_bool canceled{false};
    std::atomic_int completed{0};
    std::atomic<qint64> jumps{0};
    struct LabProgress { int phase=0, phases=0, stage=0, total=0, runStart=0; bool countRuns=false; };
    mutable std::mutex labProgressMutex;
    LabProgress labProgress;
    std::atomic_int labProgressDone{0}, labSkippedRuns{0};
    void beginLabPhase(int stage,int total,bool countRuns=false) {
        std::lock_guard<std::mutex> lock(labProgressMutex);
        ++labProgress.phase;labProgress.stage=stage;labProgress.total=total;
        labProgress.runStart=completed.load();labProgress.countRuns=countRuns;
        labProgressDone.store(0);labSkippedRuns.store(0);
    }
};
QString engineBuild();
QJsonArray parameters();
QJsonObject example();
bool validate(const QJsonObject &config,QString &error);
QJsonObject run(const QJsonObject &config,const std::shared_ptr<Control> &control);
QJsonObject replay(const QJsonObject &config,int variant,int competition);
bool exportReport(const QJsonObject &report,const QString &directory,QString &error);
int cli(int argc,char **argv);
}
#endif
