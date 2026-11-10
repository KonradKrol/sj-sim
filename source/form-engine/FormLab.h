#pragma once
#include "FormSimulation.h"
#include <QDialog>
#include <QFutureWatcher>
#include <QTimer>

class FormLabController final:public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap config READ config NOTIFY changed)
    Q_PROPERTY(QVariantMap options READ options NOTIFY changed)
    Q_PROPERTY(QVariantList definitions READ definitions CONSTANT)
    Q_PROPERTY(QStringList packs READ packs CONSTANT)
    Q_PROPERTY(QVariantMap result READ result NOTIFY resultChanged)
    Q_PROPERTY(QString message READ message NOTIFY changed)
    Q_PROPERTY(bool busy READ busy NOTIFY progressChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString progressText READ progressText NOTIFY progressChanged)
    Q_PROPERTY(bool stale READ stale NOTIFY changed)
public:
    explicit FormLabController(QObject *parent=nullptr);
    ~FormLabController() override;
    QVariantMap config()const{return m_config.toJson().toVariantMap();}
    QVariantMap options()const{return m_options;}
    QVariantList definitions()const{return AutoForm::Config::definitions();}
    QStringList packs()const;
    QVariantMap result()const{return m_result.toVariantMap();}
    QString message()const{return m_message;}
    bool busy()const{return m_busy;}
    bool stale()const{return m_stale;}
    double progress()const;
    QString progressText()const;
    Q_INVOKABLE void setParameter(const QString &,double);
    Q_INVOKABLE void setOption(const QString &,const QVariant &);
    Q_INVOKABLE void freezeReference();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void start();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void saveConfig();
    Q_INVOKABLE void loadConfig();
    Q_INVOKABLE void useForNewGames();
    Q_INVOKABLE void exportResult();
signals:
    void changed();void resultChanged();void progressChanged();
private:
    AutoForm::Config m_config,m_reference;
    QVariantMap m_options;
    QVector<QJsonObject> m_packs;
    QJsonObject m_result;
    QFutureWatcher<QJsonObject> m_watcher;
    QTimer m_progressTimer;
    std::shared_ptr<AutoForm::Control> m_control;
    QString m_message;
    qint64 m_total=0;
    bool m_busy=false,m_stale=false;
    void edited();
};
class FormLabWindow final:public QDialog {
    Q_OBJECT
public:
    explicit FormLabWindow(QWidget *parent=nullptr);
    void reject()override;
};
