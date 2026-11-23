#pragma once

#include <QHash>
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>
#include "../../global/Country.h"

// Backend of the countries screen. All edits go to a draft copy; the database changes only
// when the player saves. Each country carries what the screen needs to explain it: its flag,
// how many athletes and hills use it, and whether it can be removed.
class CountriesController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList countries READ countries NOTIFY changed)
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY changed)
    Q_PROPERTY(QVariantMap current READ current NOTIFY changed)
    Q_PROPERTY(bool dirty READ dirty NOTIFY changed)
    Q_PROPERTY(int total READ total NOTIFY changed)
    Q_PROPERTY(QString flagsFolder READ flagsFolder CONSTANT)
public:
    explicit CountriesController(QObject *parent=nullptr);

    QVariantList countries() const;
    QString search() const { return m_search; }
    void setSearch(const QString &value);
    QVariantMap current() const;
    bool dirty() const { return m_dirty; }
    int total() const { return m_draft.size(); }
    const QVector<Country> &draft() const { return m_draft; }

    Q_INVOKABLE void select(int index);
    // Returns an explanation when the name cannot be used, otherwise an empty string.
    Q_INVOKABLE QString rename(int index, const QString &name);
    // Live check of the "new country" form: problems per field, flag preview and readiness.
    Q_INVOKABLE QVariantMap checkNew(const QString &name, const QString &code, const QString &iso) const;
    Q_INVOKABLE bool add(const QString &name, const QString &code, const QString &iso);
    Q_INVOKABLE bool removeSelected();
    QString flagsFolder() const;
    // Opens (and creates when missing) the folder for the player's own flags.
    Q_INVOKABLE void openFlagsFolder();
    // Looks for flag files again, e.g. after the player copied a new flag into the folder.
    Q_INVOKABLE void refresh() { emit changed(); }
    Q_INVOKABLE void save();
    Q_INVOKABLE void cancel();

signals:
    void changed();
    void finished(bool saved);

private:
    QVariantMap row(int index) const;
    int usage(const QString &code, bool hills) const;
    static QString flagUrl(const QString &code);
    static bool flagExists(const QString &code);

    QVector<Country> m_draft;
    QHash<QString,int> m_athletes, m_hills;
    QString m_search;
    int m_selected=-1;
    bool m_dirty=false;
};
