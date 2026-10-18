#ifndef COMPETITIONQUICKBRIDGE_H
#define COMPETITIONQUICKBRIDGE_H

#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QHash>
#include <QDialog>

class CompetitionConfigWindow;
class QWidget;
class QQuickWidget;

// A view adapter, not a second simulation/configuration model. All mutations go
// through the existing controls and therefore their existing C++ signal handlers.
class CompetitionQuickBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList cards READ cards NOTIFY changed)
    Q_PROPERTY(QVariantList jumpers READ jumpers NOTIFY changed)
    Q_PROPERTY(QVariantMap summary READ summary NOTIFY changed)
    Q_PROPERTY(int section READ section WRITE setSection NOTIFY changed)
public:
    explicit CompetitionQuickBridge(CompetitionConfigWindow *configuration, QObject *parent = nullptr);
    QVariantList cards() const { return m_cards; }
    QVariantList jumpers() const;
    QVariantMap summary() const;
    int section() const { return m_section; }
    void setSection(int section);
    Q_INVOKABLE void setField(int id, const QVariant &value);
    Q_INVOKABLE void action(const QString &name, const QVariantList &selection = {});
    Q_INVOKABLE void refresh();
    CompetitionConfigWindow *configuration() const { return m_configuration; }
signals:
    void changed();
private:
    CompetitionConfigWindow *m_configuration;
    int m_section = 0;
    int m_nextId = 0;
    QVariantList m_cards;
    QHash<int, QPointer<QWidget>> m_fields;
    QVariantMap field(QWidget *widget, const QString &label);
    void card(const QString &title, QWidget *editor, const QString &schema);
    void specialEditor(QWidget *editor, const QString &title);
};

class CompetitionQuickWindow : public QDialog
{
    Q_OBJECT
public:
    explicit CompetitionQuickWindow(QWidget *parent = nullptr);
    ~CompetitionQuickWindow() override;
    CompetitionQuickBridge *bridge() const { return m_bridge; }
    QQuickWidget *view() const { return m_view; }
private:
    CompetitionQuickBridge *m_bridge;
    QQuickWidget *m_view;
};
#endif
