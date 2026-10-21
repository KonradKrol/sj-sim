#pragma once
#include <QAbstractListModel>
#include <QPointer>
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
class QuickRoster : public QObject {
    Q_OBJECT
    Q_PROPERTY(QuickRosterRows *library READ library CONSTANT)
    Q_PROPERTY(QuickRosterRows *ordered READ ordered CONSTANT)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY changed)
    Q_PROPERTY(QString orderQuery READ orderQuery WRITE setOrderQuery NOTIFY changed)
    Q_PROPERTY(QStringList countries READ countries NOTIFY changed)
    Q_PROPERTY(int country READ country WRITE setCountry NOTIFY changed)
    Q_PROPERTY(int count READ count NOTIFY changed)
    Q_PROPERTY(int markedCount READ markedCount NOTIFY changed)
    Q_PROPERTY(bool membership READ membership CONSTANT)
    Q_PROPERTY(bool active READ active NOTIFY changed)
    Q_PROPERTY(int activeLimit READ activeLimit NOTIFY changed)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY changed)
    Q_PROPERTY(QStringList presets READ presets NOTIFY changed)
public:
    explicit QuickRoster(QWidget *source,QObject *parent);
    void sync();
    void invalidateText() { refresh(); }
    static QString key(Jumper *jumper);
    QuickRosterRows *library() const { return m_library; }
    QuickRosterRows *ordered() const { return m_ordered; }
    QString query() const { return m_query; }
    QString orderQuery() const { return m_orderQuery; }
    QStringList countries() const;
    int country() const { return m_country; }
    int count() const { return m_current.size(); }
    int markedCount() const { return m_marked.size(); }
    bool membership() const;
    bool active() const;
    int activeLimit() const;
    bool canUndo() const { return m_hasUndo; }
    QStringList presets() const;
    void setQuery(const QString &value);
    void setOrderQuery(const QString &value);
    void setCountry(int value);
    Q_INVOKABLE void toggle(const QString &id);
    Q_INVOKABLE void addVisible();
    Q_INVOKABLE void removeVisible();
    Q_INVOKABLE void removeMarked();
    Q_INVOKABLE void mark(const QString &id,int modifiers=0);
    Q_INVOKABLE void markAll();
    Q_INVOKABLE void moveBefore(const QString &id,const QString &target,bool after=false);
    Q_INVOKABLE void moveMarked(int position);
    Q_INVOKABLE void step(int direction);
    Q_INVOKABLE void sort(const QString &key);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void applyPreset(int index,int operation);
private:
    QPointer<QWidget> m_source;
    QuickRosterRows *m_library,*m_ordered;
    QVector<Jumper *> m_pool,m_current,m_undo;
    QSet<QString> m_marked;
    QStringList m_countryCodes,m_presetNames;
    QString m_query,m_orderQuery,m_anchor;
    int m_country=0;
    bool m_hasUndo=false;
    QVector<Jumper *> read() const;
    QVector<Jumper *> candidates() const;
    void write(const QVector<Jumper *> &value);
    void apply(QVector<Jumper *> value);
    void refresh();
    QVector<Jumper *> visiblePool() const;
    Jumper *jumper(const QString &id) const;
signals:
    void changed();
};
