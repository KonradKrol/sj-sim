#pragma once
#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QHash>
class QWidget;
class QDialog;
class QQuickWidget;
class CompetitionManagerWindow;

// Presents the existing controls/models. Simulation, validation, dialog results
// and signal connections remain owned by their original C++ objects.
class CompetitionLiveBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList sections READ sections NOTIFY changed)
    Q_PROPERTY(bool dark READ dark WRITE setDark NOTIFY themeChanged)
    Q_PROPERTY(bool competition READ competition CONSTANT)
public:
    explicit CompetitionLiveBridge(QDialog *dialog, QWidget *legacy, bool competition);
    ~CompetitionLiveBridge() override;
    QVariantList sections() const { return m_sections; }
    bool dark() const;
    bool competition() const { return m_competition; }
    void setDark(bool value);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void edit(int id, const QVariant &value);
    Q_INVOKABLE void activate(int id, int entry = -1);
    Q_INVOKABLE void select(int id, int row, bool details);
    Q_INVOKABLE void classic();
    Q_INVOKABLE void toggleTree(int id, int row);
    static void install(CompetitionManagerWindow *window);
    static bool quickEnabled(QWidget *widget);
signals:
    void aboutToChange();
    void changed();
    void themeChanged();
private:
    QVariantMap node(QWidget *widget);
    QVariantList layoutNodes(class QLayout *layout);
    int identify(QWidget *widget);
    QPointer<QDialog> m_dialog;
    QWidget *m_legacy;
    QQuickWidget *m_view = nullptr;
    bool m_competition;
    bool m_classic = false;
    QVariantList m_sections;
    QHash<int, QPointer<QWidget>> m_widgets;
    QHash<QWidget *, int> m_ids;
    int m_nextId = 0;
    bool m_tablesDirty = true;
    QHash<int, QVariantMap> m_tables;
    QHash<QObject *, bool> m_observedModels;
    friend class CompetitionDialogFilter;
};
