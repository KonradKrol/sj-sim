#ifndef PLAYGROUNDCONTROLLER_H
#define PLAYGROUNDCONTROLLER_H
#include "PlaygroundExperiment.h"
#include "../simulation-v2/SimulationCatalog.h"
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
    Q_PROPERTY(QVariantList definitions READ definitions NOTIFY configChanged)
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)
    Q_PROPERTY(QString sourceNotes READ sourceNotes NOTIFY sourcesChanged)
    Q_PROPERTY(QStringList athletePacks READ athletePacks NOTIFY sourcesChanged)
    Q_PROPERTY(QStringList hillNames READ hillNames NOTIFY sourcesChanged)
    Q_PROPERTY(QStringList ruleNames READ ruleNames NOTIFY sourcesChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY progressChanged)
    Q_PROPERTY(int completed READ completed NOTIFY progressChanged)
    Q_PROPERTY(qint64 jumpCount READ jumpCount NOTIFY progressChanged)
    Q_PROPERTY(double elapsed READ elapsed NOTIFY progressChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap previewReport READ previewReport NOTIFY previewChanged)
    Q_PROPERTY(QVariantMap diagnostics READ diagnostics NOTIFY previewChanged)
    Q_PROPERTY(bool previewBusy READ previewBusy NOTIFY previewChanged)
    Q_PROPERTY(QVariantList effective READ effective NOTIFY configChanged)
    QFutureWatcher<SimulationV2::CatalogSnapshot> m_catalogWatcher;
    QFutureWatcher<QJsonObject> m_previewWatcher;
    QTimer m_debounce;
    std::shared_ptr<Playground::Control> m_previewControl;
    quint64 m_revision=0;
    int m_analysisVariant=0;
    bool m_previewPending=false;
    QVariantMap m_previewReport,m_diagnostics;
    QVariantList m_definitions,m_effective;
    void refreshEffective();
    void startPreview();
    QJsonObject m_config,m_report,m_replay;
    QVariantMap m_reportMap,m_replayMap;
    QFutureWatcher<QJsonObject> m_watcher,m_replayWatcher;
    std::shared_ptr<Playground::Control> m_control;
    QTimer m_progress;
    QElapsedTimer m_clock;
    QVariantList m_history;
    QString m_message,m_sourceNotes;
    QStringList m_packIds,m_athletePacks,m_hillNames,m_ruleNames;
    QVector<QJsonObject> m_rosters,m_hills,m_rules;
    void setMessage(const QString &message);
    void refreshHistory();
public:
    explicit PlaygroundController(QObject *parent=nullptr);
    void invalidateText(){m_definitions=Playground::parameters().toVariantList();m_athletePacks[0]=tr("Stawka syntetyczna");m_hillNames[0]=tr("Skocznia syntetyczna");m_ruleNames[0]=tr("Zasady eksperymentu");emit sourcesChanged();emit configChanged();}
    ~PlaygroundController() override;
    QVariantMap config()const{return m_config.toVariantMap();}
    QVariantMap report()const{return m_reportMap;}
    QVariantMap replayResult()const{return m_replayMap;}
    QVariantList definitions()const{return m_definitions;}
    QVariantList history()const{return m_history;}
    QString sourceNotes()const{return m_sourceNotes;}
    QStringList athletePacks()const{return m_athletePacks;}
    QStringList hillNames()const{return m_hillNames;}
    QStringList ruleNames()const{return m_ruleNames;}
    bool busy()const{return property("playgroundPendingRun").toBool() || property("playgroundPendingReplay").toBool() || m_watcher.isRunning() || m_replayWatcher.isRunning();}
    int completed()const{return m_control?m_control->completed.load():0;}
    qint64 jumpCount()const{return m_control?m_control->jumps.load():0;}
    double elapsed()const{return m_clock.isValid()?m_clock.elapsed()/1000.:0;}
    QString message()const{return m_message;}
    QVariantMap previewReport()const{return m_previewReport;}
    QVariantMap diagnostics()const{return m_diagnostics;}
    bool previewBusy()const{return m_previewPending || m_previewWatcher.isRunning();}
    QVariantList effective()const{return m_effective;}
    Q_INVOKABLE void setAnalysisVariant(int index);
    Q_INVOKABLE void resetParameter(int variant,const QString &key);
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
    void sourcesChanged();
    void previewChanged();
    void configChanged();
    void reportChanged();
    void replayChanged();
    void progressChanged();
    void messageChanged();
    void historyChanged();
};

class PlaygroundWindow : public QDialog {
    Q_OBJECT
public:
    explicit PlaygroundWindow(QWidget *parent=nullptr);
    void reject() override;
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif
