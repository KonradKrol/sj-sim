#ifndef PLAYGROUNDCONTROLLER_H
#define PLAYGROUNDCONTROLLER_H
#include "PlaygroundExperiment.h"
#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QFutureWatcher>
#include <QTimer>
#include <QElapsedTimer>
#include <QDialog>

class PlaygroundController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap config READ config NOTIFY configChanged)
    Q_PROPERTY(QVariantMap report READ report NOTIFY reportChanged)
    Q_PROPERTY(QVariantMap replayResult READ replayResult NOTIFY replayChanged)
    Q_PROPERTY(QVariantList definitions READ definitions CONSTANT)
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)
    Q_PROPERTY(QStringList athletePacks READ athletePacks CONSTANT)
    Q_PROPERTY(QStringList hillNames READ hillNames CONSTANT)
    Q_PROPERTY(QStringList ruleNames READ ruleNames CONSTANT)
    Q_PROPERTY(bool busy READ busy NOTIFY progressChanged)
    Q_PROPERTY(int completed READ completed NOTIFY progressChanged)
    Q_PROPERTY(qint64 jumpCount READ jumpCount NOTIFY progressChanged)
    Q_PROPERTY(double elapsed READ elapsed NOTIFY progressChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    QJsonObject m_config,m_report,m_replay;
    QVariantMap m_reportMap,m_replayMap;
    QFutureWatcher<QJsonObject> m_watcher,m_replayWatcher;
    std::shared_ptr<Playground::Control> m_control;
    QTimer m_progress;
    QElapsedTimer m_clock;
    QVariantList m_history;
    QString m_message;
    QStringList m_packIds,m_athletePacks,m_hillNames,m_ruleNames;
    QVector<QJsonObject> m_rosters,m_hills,m_rules;
    void setMessage(const QString &message);
    void refreshHistory();
public:
    explicit PlaygroundController(QObject *parent=nullptr);
    ~PlaygroundController() override;
    QVariantMap config()const{return m_config.toVariantMap();}
    QVariantMap report()const{return m_reportMap;}
    QVariantMap replayResult()const{return m_replayMap;}
    QVariantList definitions()const{return Playground::parameters().toVariantList();}
    QVariantList history()const{return m_history;}
    QStringList athletePacks()const{return m_athletePacks;}
    QStringList hillNames()const{return m_hillNames;}
    QStringList ruleNames()const{return m_ruleNames;}
    bool busy()const{return m_watcher.isRunning() || m_replayWatcher.isRunning();}
    int completed()const{return m_control?m_control->completed.load():0;}
    qint64 jumpCount()const{return m_control?m_control->jumps.load():0;}
    double elapsed()const{return m_clock.isValid()?m_clock.elapsed()/1000.:0;}
    QString message()const{return m_message;}
    Q_INVOKABLE bool configure(const QVariantMap &config);
    Q_INVOKABLE void useReportConfig();
    Q_INVOKABLE void setValue(const QString &key,const QVariant &value);
    Q_INVOKABLE void setParameter(const QString &key,double value);
    Q_INVOKABLE void setVariantParameter(int index,const QString &key,double value);
    Q_INVOKABLE void renameVariant(int index,const QString &name);
    Q_INVOKABLE void addVariant();
    Q_INVOKABLE void removeVariant(int index);
    Q_INVOKABLE void sweep(const QString &key,double from,double to,int count);
    Q_INVOKABLE void preset(const QString &name);
    Q_INVOKABLE void useAthletePack(int index);
    Q_INVOKABLE void useHill(int index);
    Q_INVOKABLE void useRules(int index);
    Q_INVOKABLE void start();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void replay(int variant,int competition);
    Q_INVOKABLE void exportResults();
    Q_INVOKABLE void saveConfig();
    Q_INVOKABLE void loadConfig();
    Q_INVOKABLE void loadRun(int index);
signals:
    void configChanged();
    void reportChanged();
    void replayChanged();
    void progressChanged();
    void messageChanged();
    void historyChanged();
};

class PlaygroundWindow : public QDialog {
public:
    explicit PlaygroundWindow(QWidget *parent=nullptr);
};
#endif
