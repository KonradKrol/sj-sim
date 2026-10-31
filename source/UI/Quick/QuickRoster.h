#pragma once
#include <QAbstractListModel>
#include <QPointer>
#include <QMap>
#include <QSet>
#include <QVariantMap>
#include "../../simulator/Jumper.h"
class QWidget;
class QuickRosterRows : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit QuickRosterRows(QObject *parent):QAbstractListModel(parent){}
    int rowCount(const QModelIndex &parent=QModelIndex()) const override { return parent.isValid()?0:m_rows.size(); }
    QVariant data(const QModelIndex &index,int role) const override;
    QHash<int,QByteArray> roleNames() const override;
    void sync(const QVariantList &rows);
    Q_INVOKABLE QVariantMap get(int row) const { return m_rows.value(row).toMap(); }
private:
    QVariantList m_rows;
signals:
    void countChanged();
};
class QuickRosterCountryRows : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit QuickRosterCountryRows(QObject *parent):QAbstractListModel(parent){}
    int rowCount(const QModelIndex &parent=QModelIndex()) const override { return parent.isValid()?0:m_rows.size(); }
    QVariant data(const QModelIndex &index,int role) const override;
    QHash<int,QByteArray> roleNames() const override;
    void sync(const QVariantList &rows);
private:
    QVariantList m_rows;
signals:
    void countChanged();
};
class QuickRoster : public QObject {
    Q_OBJECT
    Q_PROPERTY(QuickRosterRows *library READ library CONSTANT)
    Q_PROPERTY(QuickRosterRows *groupedLibrary READ groupedLibrary CONSTANT)
    Q_PROPERTY(QuickRosterRows *ordered READ ordered CONSTANT)
    Q_PROPERTY(QuickRosterRows *groupedRows READ groupedRows CONSTANT)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY changed)
    Q_PROPERTY(QString orderQuery READ orderQuery WRITE setOrderQuery NOTIFY changed)
    Q_PROPERTY(QStringList countries READ countries NOTIFY changed)
    Q_PROPERTY(int country READ country WRITE setCountry NOTIFY changed)
    Q_PROPERTY(int count READ count NOTIFY changed)
    Q_PROPERTY(int markedCount READ markedCount NOTIFY changed)
    Q_PROPERTY(int candidateMarkedCount READ candidateMarkedCount NOTIFY changed)
    Q_PROPERTY(bool membership READ membership CONSTANT)
    Q_PROPERTY(bool active READ active NOTIFY changed)
    Q_PROPERTY(int activeLimit READ activeLimit NOTIFY changed)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY changed)
    Q_PROPERTY(QStringList presets READ presets NOTIFY changed)
    Q_PROPERTY(bool savedList READ savedList CONSTANT)
    Q_PROPERTY(bool grouped READ grouped WRITE setGrouped NOTIFY changed)
    Q_PROPERTY(QVariantList countryLimits READ countryLimits NOTIFY changed)
    Q_PROPERTY(QuickRosterCountryRows *limitRows READ limitRows CONSTANT)
    Q_PROPERTY(int exceededCountries READ exceededCountries NOTIFY changed)
public:
    explicit QuickRoster(QWidget *source,QObject *parent);
    void sync();
    // Translation changes labels, not cached athlete values or their ownership.
    void invalidateText() { emit changed(); }
    static QString key(Jumper *jumper);
    QuickRosterRows *library() const { return m_library; }
    QuickRosterRows *groupedLibrary() const { return m_groupedLibrary; }
    QuickRosterRows *ordered() const { return m_ordered; }
    QuickRosterRows *groupedRows() const { return m_groupedRows; }
    QString query() const { return m_query; }
    QString orderQuery() const { return m_orderQuery; }
    QStringList countries() const;
    int country() const { return m_country; }
    int count() const { return m_current.size(); }
    int markedCount() const { return m_marked.size(); }
    int candidateMarkedCount() const { return m_candidateMarked.size(); }
    bool membership() const;
    bool active() const;
    int activeLimit() const;
    bool canUndo() const { return m_hasUndo; }
    QStringList presets() const;
    bool savedList() const;
    bool grouped() const { return m_grouped; }
    void setGrouped(bool value);
    QVariantList countryLimits() const { return m_countryLimits; }
    QuickRosterCountryRows *limitRows() const { return m_limitRows; }
    int exceededCountries() const;
    Q_INVOKABLE bool setCountryLimit(const QString &country,int limit);
    void setQuery(const QString &value);
    void setOrderQuery(const QString &value);
    void setCountry(int value);
    Q_INVOKABLE void toggle(const QString &id);
    Q_INVOKABLE void addVisible();
    Q_INVOKABLE void removeVisible();
    Q_INVOKABLE void removeMarked();
    Q_INVOKABLE void mark(const QString &id,int modifiers=0);
    Q_INVOKABLE void markAll();
    Q_INVOKABLE void selectCandidate(const QString &id);
    Q_INVOKABLE void addMarkedCandidates();
    Q_INVOKABLE void include(const QString &id);
    Q_INVOKABLE void moveBefore(const QString &id,const QString &target,bool after=false);
    Q_INVOKABLE void moveMarked(int position);
    Q_INVOKABLE void step(int direction);
    Q_INVOKABLE void sort(const QString &key);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void applyPreset(int index,int operation);
private:
    QPointer<QWidget> m_source;
    QuickRosterRows *m_library,*m_ordered,*m_groupedRows,*m_groupedLibrary;
    QuickRosterCountryRows *m_limitRows;
    QVector<Jumper *> m_pool,m_current,m_undo;
    QMap<QString,int> m_undoLimits;
    QSet<QString> m_marked;
    QSet<QString> m_candidateMarked;
    QStringList m_countryCodes,m_presetNames;
    QVariantList m_countryLimits;
    QString m_query,m_orderQuery,m_anchor;
    int m_country=0;
    bool m_hasUndo=false;
    bool m_undoLimitChange=false;
    bool m_grouped=false;
    QVector<Jumper *> read() const;
    QVector<Jumper *> candidates() const;
    void write(const QVector<Jumper *> &value);
    void apply(QVector<Jumper *> value);
    void refresh();
    void refreshCountryLimits(bool updateModel=true);
    QVector<Jumper *> visiblePool() const;
    Jumper *jumper(const QString &id) const;
signals:
    void changed();
};
