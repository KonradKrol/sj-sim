#pragma once

#include <QObject>
#include <QPointer>
#include <QVariantMap>
#include <QHash>
#include <QAbstractListModel>
#include <QPersistentModelIndex>
#include "../../simulator/wind-generation/WindGenerationSettings.h"

class QWidget;
class QDialog;
class QLayout;
class QQuickWidget;
class QAbstractItemView;
class QStackedWidget;
class QuickCalendar;
class QuickRoster;
class KOGroupsListView;
class MainWindow;
class QuickBridge;

class QuickAppearance : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool dark READ dark WRITE setDark NOTIFY changed)
    Q_PROPERTY(bool compactUi READ compactUi WRITE setCompactUi NOTIFY changed)
    Q_PROPERTY(bool reducedMotion READ reducedMotion WRITE setReducedMotion NOTIFY changed)
    Q_PROPERTY(int teamResultsMode READ teamResultsMode WRITE setTeamResultsMode NOTIFY changed)
    Q_PROPERTY(qreal uiScale READ uiScale WRITE setUiScale NOTIFY changed)
    Q_PROPERTY(qreal textScale READ textScale WRITE setTextScale NOTIFY changed)
public:
    static constexpr qreal DefaultUiScale = .70;
    explicit QuickAppearance(QObject *parent = nullptr);
    bool dark() const { return m_dark; }
    bool compactUi() const { return m_compactUi; }
    bool reducedMotion() const { return m_reducedMotion; }
    int teamResultsMode() const { return m_teamResultsMode; }
    void setTeamResultsMode(int mode);
    qreal uiScale() const { return m_uiScale; }
    qreal textScale() const { return m_textScale; }
    void setDark(bool value);
    void setReducedMotion(bool value);
    void setCompactUi(bool value);
    void setUiScale(qreal value);
    void setTextScale(qreal value);
signals:
    void changed();
private:
    bool m_dark, m_reducedMotion, m_compactUi;
    qreal m_textScale,m_uiScale;
    int m_teamResultsMode=0;
};

// Each projected control keeps its identity when values or the layout change.
// The original widget still owns validation, signal handlers and persistence.
class QuickNode : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap values READ values NOTIFY valuesChanged)
    Q_PROPERTY(QVariantList children READ children NOTIFY childrenChanged)
public:
    explicit QuickNode(QObject *parent = nullptr) : QObject(parent) {}
    QVariantMap values() const { return m_values; }
    QVariantList children() const { return m_children; }
    void update(const QVariantMap &values, const QVariantList &children = {});
signals:
    void valuesChanged();
    void childrenChanged();
private:
    QVariantMap m_values;
    QVariantList m_children;
};

class QuickTableModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QStringList headers READ headers NOTIFY structureChanged)
    Q_PROPERTY(int count READ count NOTIFY structureChanged)
    Q_PROPERTY(bool multiple READ multiple NOTIFY structureChanged)
    Q_PROPERTY(int selectedCount READ selectedCount NOTIFY cursorChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY structureChanged)
    Q_PROPERTY(int currentRow READ currentRow NOTIFY cursorChanged)
    Q_PROPERTY(int currentColumn READ currentColumn NOTIFY cursorChanged)
    Q_PROPERTY(bool frozen READ frozen NOTIFY frozenChanged)
public:
    explicit QuickTableModel(QAbstractItemView *view, QObject *parent);
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int,QByteArray> roleNames() const override;
    QStringList headers() const;
    int count() const { return m_rows.size(); }
    bool multiple() const;
    int selectedCount() const;
    QString filter() const { return m_filter; }
    void setFilter(const QString &value);
    int currentRow() const;
    int currentColumn() const;
    void sync();
    void invalidateText();
    void setFrozen(bool frozen);
    bool frozen() const { return m_frozen; }
    Q_INVOKABLE void select(int row, int column, int modifiers, bool open = false);
    Q_INVOKABLE void edit(int row, int column, const QVariant &value);
    Q_INVOKABLE void toggle(int row);
    Q_INVOKABLE void setTeamMode(int mode);
    Q_INVOKABLE void sort(int column, bool descending);
    Q_INVOKABLE void moveCurrent(int delta, int modifiers);
    Q_INVOKABLE void moveColumn(int delta);
    Q_INVOKABLE void toggleCurrent();
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void openCurrent();
signals:
    void structureChanged();
    void cursorChanged();
    void frozenChanged();
private:
    void rebuild();
    QModelIndex cell(int row, int column) const;
    QPointer<QAbstractItemView> m_view;
    QPointer<QAbstractItemModel> m_source;
    QPointer<QObject> m_selection;
    QVector<QPersistentModelIndex> m_rows;
    QList<QMetaObject::Connection> m_connections;
    int m_teamMode=-1;
    int m_anchor = -1;
    bool m_frozen=false;
    QVector<QHash<int,QVariant>> m_snapshot;
    QString m_filter;
};

class QuickBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QuickNode *root READ root CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY changed)
    Q_PROPERTY(QString profile READ profile CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QVariantMap library READ library NOTIFY changed)
    Q_PROPERTY(QVariantList actions READ actions NOTIFY changed)
    Q_PROPERTY(QVariantMap context READ context NOTIFY changed)
    Q_PROPERTY(QVariantMap jumpSummary READ jumpSummary NOTIFY changed)
    Q_PROPERTY(bool editingText READ editingText NOTIFY focusChanged)
public:
    QuickBridge(QWidget *window, QWidget *content, QuickAppearance *appearance, bool home = false);
    ~QuickBridge() override;
    QuickNode *root() const { return m_root; }
    QString title() const;
    QString profile() const;
    QString version() const;
    QVariantMap library() const;
    QVariantList actions() const { return m_actions; }
    QVariantMap context() const { return m_context; }
    QVariantMap jumpSummary() const { return m_jumpSummary; }
    Q_INVOKABLE void edit(int id, const QVariant &value);
    Q_INVOKABLE void inputText(int id, const QString &value);
    Q_INVOKABLE void finishText(int id);
    Q_INVOKABLE void selectPreviewRound(int round);
    Q_INVOKABLE void editNextKoGroups();
    Q_INVOKABLE void skipCurrentJump();
    Q_INVOKABLE void openResultViewOptions() {emit resultViewOptionsRequested();}
    Q_INVOKABLE void assignKoMember(int editorId,const QString &groupId,int slot,const QString &athleteId);
    Q_INVOKABLE void setWind(int editorId,int sector,const QVariantMap &values,bool all);
    Q_INVOKABLE void randomizeWind(int editorId,double direction,double variability);
    Q_INVOKABLE void undoWind(int editorId);
    Q_INVOKABLE void copyWindSector(int editorId,int sector);
    Q_INVOKABLE void selectWindSector(int editorId,int sector);
    Q_INVOKABLE void activate(int id, int entry = -1);
    Q_INVOKABLE void triggerAction(int id);
    Q_INVOKABLE bool shortcut(int owner, const QString &sequence);
    Q_INVOKABLE void launch(const QString &destination);
    Q_INVOKABLE void close();
    Q_INVOKABLE void showAuthorCredits();
    Q_INVOKABLE QObject *find(const QString &name);
    Q_INVOKABLE void refresh();
    void setView(QQuickWidget *view);
    QQuickWidget *view() const;
    bool editingText() const;
    void suspend();
    void resume();
signals:
    void changed();
    void focusChanged();
    void resultViewOptionsRequested();
private:
    QuickNode *node(QWidget *widget, bool force = false);
    QuickNode *layoutNode(QLayout *layout);
    QuickNode *identity(QObject *object);
    QVariantList childNodes(QLayout *layout);
    QVariantList widgetChildren(QWidget *widget);
    int idFor(QObject *object);
    void deferRefresh();
    void compose(QuickNode *base);
    QuickNode *rosterNode(QWidget *source);
    QuickNode *koGroupsNode(KOGroupsListView *list);
    QuickNode *sectionTabs(const QString &key, const QStringList &labels, const QVariantList &pages);
    QPointer<QWidget> m_window;
    QPointer<QWidget> m_content;
    QPointer<QQuickWidget> m_view;
    QuickAppearance *m_appearance;
    QuickNode *m_root;
    QHash<QObject *, QuickNode *> m_nodes;
    QHash<int, QPointer<QObject>> m_objects;
    QHash<QObject *, int> m_ids;
    QHash<QAbstractItemView *, QuickTableModel *> m_tables;
    QHash<QString, QPointer<QWidget>> m_named;
    QHash<QWidget *, QString> m_captions;
    QHash<QString, QuickNode *> m_groups;
    QHash<QObject *,QVector<WindGenerationSettings>> m_windUndo;
    QHash<QObject *,QuickCalendar *> m_calendars;
    QHash<QWidget *,QuickRoster *> m_rosters;
    QVariantList m_actions;
    QVariantMap m_context;
    QVariantMap m_jumpSummary;
    int m_nextId = 0;
    bool m_home, m_pending = false, m_refreshing = false;
};

class QuickUi : public QObject {
    Q_OBJECT
    Q_PROPERTY(QuickBridge *current READ current NOTIFY navigationChanged)
    Q_PROPERTY(QStringList path READ path NOTIFY navigationChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY navigationChanged)
public:
    explicit QuickUi(MainWindow *window);
    ~QuickUi() override;
    static bool enabled();
    QuickBridge *current() const;
    QStringList path() const;
    bool canGoBack() const;
    Q_INVOKABLE void back();
    Q_INVOKABLE void showAppearance();
    Q_INVOKABLE void closeAppearance();
signals:
    void navigationChanged();
protected:
    bool eventFilter(QObject *object, QEvent *event) override;
private:
    void attach(QWidget *dialog);
    void fitTransient(QWidget *dialog);
    void requestTransientFit(QWidget *dialog);
    void push(QWidget *controller,QuickBridge *bridge);
    void removePage(QWidget *controller);
    void showCurrent();
    struct Page { QPointer<QWidget> controller; QPointer<QuickBridge> bridge; };
    QVector<Page> m_pages;
    QStackedWidget *m_stack;
    QPointer<QWidget> m_appearancePopup;
    MainWindow *m_main;
    QuickAppearance *m_appearance;
    bool m_translationPending = false;
    bool m_navigating = false, m_closeRequested = false;
};
