#pragma once

#include "../global/DataPack.h"
#include <QObject>
#include <QVariantList>
#include <QVariantMap>

// A session-local copy, exposing the same editing contract as the profile studios.
// Never writes the authoring pack or modifies a running session's source catalog.
class SimulationProfileDraft : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap profile READ profile NOTIFY changed)
    Q_PROPERTY(QVariantList selectedPerks READ selectedPerks NOTIFY changed)
    Q_PROPERTY(QVariantList countries READ countries CONSTANT)
    Q_PROPERTY(double realHs READ realHs NOTIFY changed)
    Q_PROPERTY(bool valid READ valid NOTIFY changed)
    Q_PROPERTY(QString message READ message NOTIFY changed)
public:
    explicit SimulationProfileDraft(DataPack::Kind kind,QObject *parent=nullptr);
    void load(const Jumper &,const QString &directory);
    void load(const Hill &);
    void clear();
    Jumper configuredJumper() const {return athlete;}
    Hill configuredHill() const {return hill;}
    bool valid() const;
    QVariantMap profile() const;
    QVariantList selectedPerks() const;
    QVariantList countries() const;
    double realHs() const;
    QString message() const {return error;}
    Q_INVOKABLE void setValue(const QString &key,const QVariant &value);
    Q_INVOKABLE void setPerk(int pair,int side,int level,bool toggle=true);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void choosePhoto();
    Q_INVOKABLE void clearPhoto();
    Q_INVOKABLE void openPackFolder();
signals:
    void changed();
private:
    DataPack::Kind kind;
    Jumper athlete,originalAthlete;
    Hill hill,originalHill;
    QString directory,error;
    bool loaded=false;
    bool apply(const QJsonObject &);
};
