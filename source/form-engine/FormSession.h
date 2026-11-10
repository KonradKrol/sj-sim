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
