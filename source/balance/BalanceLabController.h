#pragma once
#include "BalanceLabModel.h"
#include "../playground/PlaygroundController.h"
#include <QDialog>
#include <QFutureWatcher>
#include <QTimer>
#include <QElapsedTimer>

class BalanceLabController final:public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* sources READ sources CONSTANT)
    Q_PROPERTY(QVariantList questions READ questions NOTIFY changed)
    Q_PROPERTY(int question READ question NOTIFY changed)
    Q_PROPERTY(QVariantMap config READ config NOTIFY changed)
    Q_PROPERTY(QVariantMap options READ options NOTIFY changed)
    Q_PROPERTY(QVariantList definitions READ definitions NOTIFY definitionsChanged)
    Q_PROPERTY(QVariantMap values READ values NOTIFY changed)
    Q_PROPERTY(QVariantList athletes READ athletes NOTIFY changed)
    Q_PROPERTY(QVariantMap result READ result NOTIFY resultChanged)
    Q_PROPERTY(QVariantMap fullResult READ fullResult NOTIFY resultChanged)
    Q_PROPERTY(QVariantMap previewResult READ previewResult NOTIFY resultChanged)
    Q_PROPERTY(QVariantMap replay READ replay NOTIFY resultChanged)
    Q_PROPERTY(QVariantList profiles READ profiles NOTIFY changed)
    Q_PROPERTY(QVariantList history READ history NOTIFY changed)
    Q_PROPERTY(QVariantMap diff READ diff NOTIFY changed)
    Q_PROPERTY(QString activeProfile READ activeProfile NOTIFY changed)
    Q_PROPERTY(QString message READ message NOTIFY changed)
    Q_PROPERTY(bool busy READ busy NOTIFY progressChanged)
    Q_PROPERTY(bool previewBusy READ previewBusy NOTIFY progressChanged)
    Q_PROPERTY(bool previewEnabled READ previewEnabled WRITE setPreviewEnabled NOTIFY changed)
    Q_PROPERTY(bool stale READ stale NOTIFY changed)
    Q_PROPERTY(int completed READ completed NOTIFY progressChanged)
    Q_PROPERTY(double elapsed READ elapsed NOTIFY progressChanged)
public:
    explicit BalanceLabController(QObject *parent=nullptr);
    ~BalanceLabController() override;
    QObject *sources()const{return m_sources;}
    QVariantList questions()const{return BalanceLab::questions().toVariantList();}
    int question()const{return m_question;}
    QVariantMap config()const{return m_config.toVariantMap();}
    QVariantMap options()const{return m_options.toVariantMap();}
    QVariantMap values()const;
    QVariantList definitions()const;
    QVariantList athletes()const;
    QVariantMap result()const{return m_result.toVariantMap();}
    QVariantMap fullResult()const{return m_full.toVariantMap();}
    QVariantMap previewResult()const{return m_preview.toVariantMap();}
    QVariantMap replay()const{return m_replay.toVariantMap();}
    QVariantList profiles()const{return m_profiles;}
    QVariantList history()const{return m_history;}
    QVariantMap diff()const;
    QString activeProfile()const;
    QString message()const{return m_message;}
    bool busy()const{return m_fullPending;}
    bool previewBusy()const{return m_pending || (property("balanceJobPending").toBool() && !m_fullPending);}
    bool previewEnabled()const{return m_options["previewEnabled"].toBool(false);}
    Q_INVOKABLE void setPreviewEnabled(bool);
    bool stale()const{return m_full.isEmpty() || m_full["sourceRevision"].toString()!=QString::number(m_revision);}
    int completed()const{return m_control?m_control->completed.load():0;}
    double elapsed()const{return m_clock.isValid()?m_clock.elapsed()/1000.:0;}
    Q_INVOKABLE void chooseQuestion(int);
    Q_INVOKABLE void setOption(const QString &,const QVariant &);
    Q_INVOKABLE bool configure(const QVariantMap &);
    Q_INVOKABLE void setContext(const QString &,const QVariant &);
    Q_INVOKABLE void setParameter(const QString &,double);
    Q_INVOKABLE void resetParameter(const QString &);
    Q_INVOKABLE void freeze();
    Q_INVOKABLE void resetCurrent();
    Q_INVOKABLE void undo();
    Q_INVOKABLE void choosePack(int);
    Q_INVOKABLE void chooseHill(int);
    Q_INVOKABLE void chooseRules(int);
    Q_INVOKABLE void setFormat(int format,int teamSize);
    Q_INVOKABLE void editAthletes(const QVariantList &indices,const QString &key,double value,bool relative);
    Q_INVOKABLE void setTeam(const QVariantList &indices,int team);
    Q_INVOKABLE void setTargets(const QVariantList &);
    Q_INVOKABLE void start(bool matrix=false);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void showFullResult();
    Q_INVOKABLE void selectCell(double x,double y);
    Q_INVOKABLE void saveProfile(const QString &name);
    Q_INVOKABLE void loadProfile(int);
    Q_INVOKABLE void applyProfile();
    Q_INVOKABLE void standardProfile();
    Q_INVOKABLE void loadHistory(int);
    Q_INVOKABLE void exportResults();
    Q_INVOKABLE void saveExperiment();
    Q_INVOKABLE void loadExperiment();
    Q_INVOKABLE void replayContest(int competition,int variant);
    void invalidateText(){m_rosterRevision=~quint64(0);emit definitionsChanged();emit changed();}
signals:
    void definitionsChanged();
    void changed();
    void resultChanged();
    void progressChanged();
private:
    PlaygroundController *m_sources;
    QJsonObject m_config,m_options,m_result,m_full,m_preview,m_replay;
    QVariantList m_profiles,m_history;
    mutable quint64 m_rosterRevision=~quint64(0);
    mutable QVariantList m_rosterCache;
    QVector<QJsonObject> m_undo;
    QFutureWatcher<QJsonObject> m_watcher;
    std::shared_ptr<Playground::Control> m_control;
    QTimer m_debounce,m_progress,m_autosave;
    QElapsedTimer m_clock;
    quint64 m_revision=0;
    int m_question=0;
    bool m_pending=false,m_fullPending=false,m_restoring=false;
    QString m_message;
    bool commit(QJsonObject,bool undo=true);
    void schedule();
    void launch(bool full);
    void refreshFiles();
    void saveWorkspace();
};

class BalanceLabWindow final:public QDialog {
    Q_OBJECT
public:
    explicit BalanceLabWindow(QWidget *parent=nullptr);
    void reject()override;
};
