#pragma once
#include "../simulator/JumpManipulator.h"
#include "WeatherSetup.h"
#include <QObject>
#include <QVariantMap>

class SimulationJumpDraft : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap values READ values NOTIFY changed)
    Q_PROPERTY(QObject *wind READ wind CONSTANT)
    Q_PROPERTY(QString message READ message NOTIFY changed)
    Q_PROPERTY(bool canApply READ canApply NOTIFY changed)
    Q_PROPERTY(int changesCount READ changesCount NOTIFY changed)
public:
    explicit SimulationJumpDraft(QObject *parent=nullptr);
    void load(const JumpManipulator &,const HillProfile &,const QVector<Wind> &,bool allowDsq);
    QVariantMap values() const {return fields;}
    QObject *wind() const {return weather;}
    QString message() const {return error;}
    bool canApply() const {return loaded && error.isEmpty();}
    int changesCount() const;
    JumpManipulator manipulator() const;
    Q_INVOKABLE void setValue(const QString &,const QVariant &);
    Q_INVOKABLE void setJudge(int index,double value);
    Q_INVOKABLE void reset();
signals:
    void changed();
private:
    QVariantMap fields;
    SimulationWeatherSetup *weather;
    HillProfile hill;
    QVector<Wind> baselineWinds;
    bool loaded=false,dsqAllowed=false;
    int revision=0;
    QString error;
    SimulationV2::JumpManipulation build() const;
    void validate();
};
