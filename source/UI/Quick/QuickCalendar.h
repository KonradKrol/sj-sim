#pragma once
#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QVariantMap>
#include <QSet>
#include <functional>
#include <memory>
#include "../../competitions/CompetitionInfo.h"

class CalendarEditorWidget;
class CompetitionRulesEditorWidget;
class DatabaseItemsListView;
class QuickCalendar : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList events READ events NOTIFY changed)
    Q_PROPERTY(QVariantMap selection READ selection NOTIFY changed)
    Q_PROPERTY(QStringList hills READ hills NOTIFY changed)
    Q_PROPERTY(QStringList rules READ rules NOTIFY changed)
    Q_PROPERTY(QStringList auxiliaryRules READ auxiliaryRules NOTIFY changed)
    Q_PROPERTY(int rulesTarget READ rulesTarget WRITE setRulesTarget NOTIFY changed)
    Q_PROPERTY(QVariantList classifications READ classifications NOTIFY changed)
    Q_PROPERTY(QStringList advancementCompetitions READ advancementCompetitions NOTIFY changed)
    Q_PROPERTY(QStringList advancementClassifications READ advancementClassifications NOTIFY changed)
    Q_PROPERTY(QStringList calendars READ calendars NOTIFY changed)
    Q_PROPERTY(int calendarIndex READ calendarIndex NOTIFY changed)
    Q_PROPERTY(QString calendarName READ calendarName NOTIFY changed)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY changed)
    Q_PROPERTY(QString message READ message NOTIFY changed)
public:
    explicit QuickCalendar(CalendarEditorWidget *editor,QObject *parent);
    ~QuickCalendar() override;
    void sync();
    CompetitionRulesEditorWidget *rulesEditor() const;
    QVariantList events() const { return m_events; }
    QVariantMap selection() const { return m_selection; }
    QStringList hills() const;
    QStringList rules() const;
    QStringList auxiliaryRules() const;
    int rulesTarget() const { return m_rulesTarget; }
    void setRulesTarget(int target);
    QVariantList classifications() const { return m_classifications; }
    QStringList advancementCompetitions() const { return m_advancementLabels; }
    QStringList advancementClassifications() const;
    QStringList calendars() const;
    int calendarIndex() const;
    QString calendarName() const;
    bool canUndo() const { return bool(m_undo); }
    QString message() const { return m_message; }
    Q_INVOKABLE void select(const QString &id,int modifiers=0);
    Q_INVOKABLE void selectAll(bool futureOnly=false);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void update(const QVariantMap &fields);
    Q_INVOKABLE void setClassification(int index,bool checked);
    Q_INVOKABLE void add();
    Q_INVOKABLE void duplicate();
    Q_INVOKABLE void remove();
    Q_INVOKABLE void move(int direction);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void applyCustomRules();
    Q_INVOKABLE void resetCustomRules();
    Q_INVOKABLE void selectCalendar(int index);
    Q_INVOKABLE void renameCalendar(const QString &name);
    Q_INVOKABLE void catalogAction(const QString &action);
    Q_INVOKABLE void preset(bool save);
signals:
    void changed();
private:
    struct State {
        Hill *hill=nullptr;CompetitionRules rules;short type=0;double importance=5;
        QVector<CompetitionInfo *> trainings;CompetitionInfo *trial=nullptr,*advancement=nullptr;
        QVector<Classification *> classifications;Classification *classification=nullptr;
    };
    struct Snapshot { QVector<CompetitionInfo *> order;QHash<CompetitionInfo *,State> states;QStringList selected;int boundary=0; };
    QPointer<CalendarEditorWidget> m_editor;
    QPointer<CompetitionRulesEditorWidget> m_rulesEditor;
    SeasonCalendar *m_calendar=nullptr;
    CompetitionRules m_customRules;
    std::unique_ptr<Snapshot> m_undo;
    QSet<CompetitionInfo *> m_retired,m_live;
    QStringList m_selected,m_advancementLabels;
    QVector<CompetitionInfo *> m_advancementOptions;
    QVariantList m_events,m_classifications;
    QVariantMap m_selection;
    QString m_message,m_language;
    bool m_dirty=true,m_mutating=false;
    int m_rulesTarget=0;
    QVector<CompetitionInfo *> mains() const;
    QVector<CompetitionInfo *> selected() const;
    bool locked(CompetitionInfo *event) const;
    bool editable() const;
    bool referencedByHistory(CompetitionInfo *event) const;
    Snapshot capture() const;
    void mutate(const std::function<void()> &operation);
    void clearUndo();
    void refresh();
    void setOrder(const QVector<CompetitionInfo *> &events);
    void fail(const QString &message);
    CompetitionInfo *clone(CompetitionInfo *event);
    DatabaseItemsListView *calendarList() const;
};
