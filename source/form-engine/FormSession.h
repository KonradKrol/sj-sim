#pragma once
#include "FormEngine.h"
#include <QHash>
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
    // Quick shortcuts: 0 raise / 1 lower by one regime, 2 crisis, 3 peak, 4 exact value (1–100).
    bool steer(Jumper &,int action,int value,QString &error);
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
