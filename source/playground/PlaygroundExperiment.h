#ifndef PLAYGROUNDEXPERIMENT_H
#define PLAYGROUNDEXPERIMENT_H
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <atomic>
#include <memory>

namespace Playground {
struct Control {
    std::atomic_bool canceled{false};
    std::atomic_int completed{0};
    std::atomic<qint64> jumps{0};
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
