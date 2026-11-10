#pragma once
#include "FormSimulation.h"
#include <QDialog>
#include <QJsonArray>
#include <QFutureWatcher>
#include <QTimer>
class QStandardItemModel;
class QSortFilterProxyModel;

class FormLabController final:public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap config READ config NOTIFY changed)
    Q_PROPERTY(QVariantMap options READ options NOTIFY changed)
    Q_PROPERTY(QVariantList definitions READ definitions CONSTANT)
    Q_PROPERTY(QStringList packs READ packs CONSTANT)
    Q_PROPERTY(QObject* athleteRows READ athleteRows CONSTANT)
    Q_PROPERTY(int selectedAthletes READ selectedAthletes NOTIFY selectionChanged)
    Q_PROPERTY(int athleteCount READ athleteCount NOTIFY selectionChanged)
    Q_PROPERTY(QStringList plotNames READ plotNames NOTIFY plotNamesChanged)
    Q_PROPERTY(QVariantList plottedIndexes READ plottedIndexes NOTIFY plotsChanged)
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
    QObject *athleteRows()const;
    int selectedAthletes()const{return m_selectedAthletes;}
    int athleteCount()const;
    Q_INVOKABLE void setAthlete(const QString &id,const QString &key,const QVariant &);
    Q_INVOKABLE void selectAthletes(bool selected);
    Q_INVOKABLE void resetAthlete(const QString &id);
    Q_INVOKABLE void filterAthletes(const QString &text);
    QStringList plotNames()const{return m_plotNames;}
    QVariantList plottedIndexes()const{return m_plottedIndexes;}
    Q_INVOKABLE void setPlottedAthletes(const QVariantList &);
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
    void plotNamesChanged();void plotsChanged();
    void changed();void resultChanged();void progressChanged();void selectionChanged();
private:
    AutoForm::Config m_config,m_reference;
    QVariantMap m_options;
    QVector<QJsonObject> m_packs;
    QStandardItemModel *m_athletes;
    QSortFilterProxyModel *m_filteredAthletes;
    QHash<QString,int> m_athleteRows;
    QHash<QString,QVariantMap> m_athleteOverrides;
    int m_selectedAthletes=0;
    QJsonObject m_result;
    QJsonArray m_traces;QStringList m_plotNames;QVariantList m_plottedIndexes;
    QFutureWatcher<QJsonObject> m_watcher;
    QTimer m_progressTimer;
    std::shared_ptr<AutoForm::Control> m_control;
    QString m_message;
    qint64 m_total=0;
    bool m_busy=false,m_stale=false;
    void edited();
    void rebuildAthletes();
    QString athleteKey(const QString &)const;
};
class FormLabWindow final:public QDialog {
    Q_OBJECT
public:
    explicit FormLabWindow(QWidget *parent=nullptr);
    void reject()override;
};
