#pragma once
#include "FormEngine.h"
#include <QHash>
#include <QSet>
#include <QVector>
class Jumper;

namespace AutoForm {
struct Session {
    bool enabled=false,anchorPending=true;
    SimulationV2::Seed seed=0;
    Config config;
    QHash<QString,State> states;
    Session();
    QJsonObject toJson() const;
    static bool fromJson(const QJsonObject &,Session &,QString &);
    bool setEnabled(bool enabled,bool datesEnabled,QString &);
    bool update(QDate,const QVector<Jumper *> &,bool datesEnabled,QString &);
    QVariantMap diagnostic(const Jumper &) const;
    // 0/1 gradual rise/fall, 2/3 crisis/peak, 4 exact (1–100), 5/6 immediate rise/fall.
    bool steer(Jumper &,int action,int value,QString &error);
private:
    // Exact edits can precede the first dated event, when no State exists yet.
    QSet<QString> pendingSteeringLocks;
};
class Checkpoint {
    Session &live;
    Session before;
    const QVector<Jumper *> &athletes;
    QHash<QString,int> forms;
    bool finished=false;
public:
    Checkpoint(Session &,const QVector<Jumper *> &);
    ~Checkpoint();
    void commit(){finished=true;}
    void restore();
};
}
