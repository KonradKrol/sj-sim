#pragma once
#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QVariantMap>
#include <QSet>
#include <functional>
#include <memory>
#include "../../competitions/CompetitionInfo.h"
#include "../../seasons/Classification.h"

class CalendarEditorWidget;
class CompetitionRulesEditorWidget;
class DatabaseItemsListView;
class ClassificationEditorWidget;
class SimulationSave;
class QuickCalendar : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool datesEnabled READ datesEnabled NOTIFY changed)
    Q_PROPERTY(bool canEnableDates READ canEnableDates NOTIFY changed)
    Q_PROPERTY(QVariantList datePlan READ datePlan NOTIFY changed)
    Q_PROPERTY(QVariantList dateWarnings READ dateWarnings NOTIFY changed)
    Q_PROPERTY(QString suggestedDate READ suggestedDate NOTIFY changed)
    Q_PROPERTY(QVariantList classEntries READ classEntries NOTIFY changed)
    Q_PROPERTY(QString classId READ classId NOTIFY changed)
    Q_PROPERTY(bool classLocked READ classLocked NOTIFY changed)
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
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY changed)
    Q_PROPERTY(bool activeCalendar READ activeCalendar NOTIFY changed)
    Q_PROPERTY(QVariantList calendarEntries READ calendarEntries NOTIFY changed)
    Q_PROPERTY(QString message READ message NOTIFY changed)
public:
    explicit QuickCalendar(CalendarEditorWidget *editor,QObject *parent);
    ~QuickCalendar() override;
    void sync();
    bool datesEnabled() const;
    bool canEnableDates() const;
    QString suggestedDate() const;
    QVariantList datePlan() const { return m_datePlan; }
    QVariantList dateWarnings() const;
    Q_INVOKABLE void setDatesEnabled(bool enabled);
    Q_INVOKABLE QString normalizeDate(const QString &value) const;
    Q_INVOKABLE QString dateText(const QString &iso) const;
    Q_INVOKABLE QString offsetDate(const QString &iso,int days,int months=0) const;
    Q_INVOKABLE QVariantMap monthData(const QString &iso) const;
    Q_INVOKABLE void planDates(const QString &start,const QString &pattern,int interval,bool future=false,int trainingDays=0);
    Q_INVOKABLE void applyDatePlan();
    Q_INVOKABLE void discardDatePlan();
    Q_INVOKABLE void shiftDates(int days);
    Q_INVOKABLE void clearDates();
    Q_INVOKABLE void assignDate(const QString &iso);
    Q_INVOKABLE void sortDates();
    Q_INVOKABLE void selectUndated();
    CompetitionRulesEditorWidget *rulesEditor() const;
    ClassificationEditorWidget *classificationEditor() const { return m_classEditor; }
    QVariantList classEntries() const;
    QString classId() const { return m_classId; }
    bool classLocked() const;
    Q_INVOKABLE void selectClass(const QString &id);
    Q_INVOKABLE void classAction(const QString &action);
    Q_INVOKABLE void applyClassification();
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
    bool canUndo() const { return !m_history.undo.isEmpty() && canRestore(m_history.undo.last()); }
    bool canRedo() const { return !m_history.redo.isEmpty() && canRestore(m_history.redo.last()); }
    bool activeCalendar() const;
    QVariantList calendarEntries() const;
    QString message() const { return m_message; }
    Q_INVOKABLE void select(const QString &id,int modifiers=0);
    Q_INVOKABLE void selectAll(bool futureOnly=false);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void update(const QVariantMap &fields);
    Q_INVOKABLE void setClassification(int index,bool checked);
    Q_INVOKABLE void setClassificationScoring(int index,const QVariantMap &patch);
    Q_INVOKABLE void add();
    Q_INVOKABLE void addMany(int count);
    Q_INVOKABLE void duplicate();
    Q_INVOKABLE void remove();
    Q_INVOKABLE void move(int direction);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void moveBefore(const QString &source,const QString &target,bool after=false);
    Q_INVOKABLE void activateCalendar();
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
        QDate date;
        bool played=false,cancelled=false;Hill *hill=nullptr;CompetitionRules rules;short type=0;double importance=5;
        QVector<CompetitionInfo *> trainings;CompetitionInfo *trial=nullptr,*advancement=nullptr,*pending=nullptr;
        QHash<QString,ClassificationCompetitionSettings> scoring;
        QVector<Classification *> classifications;Classification *classification=nullptr;
    };
    struct Snapshot { SeasonCalendar *owner=nullptr,*active=nullptr;QVector<SeasonCalendar *> calendars;QVector<CompetitionInfo *> order;QHash<CompetitionInfo *,State> states;QVector<Classification *> classes;QHash<Classification *,Classification> classStates;QString name,classId;QStringList selected;int boundary=0; };
    QPointer<CalendarEditorWidget> m_editor;
    QPointer<CompetitionRulesEditorWidget> m_rulesEditor;
    ClassificationEditorWidget *m_classEditor=nullptr;
    QString m_classId;
    bool m_loadingClass=false,m_classPending=false;
    Classification *selectedClass() const;
    void loadClass();
    SeasonCalendar *m_calendar=nullptr;
    SimulationSave *m_save=nullptr;
    CompetitionRules m_customRules;
    struct History { QVector<Snapshot> undo,redo; };
    History m_history;
    QSet<CompetitionInfo *> m_retired,m_live;
    QSet<Classification *> m_retiredClasses,m_liveClasses;
    const void *m_season=nullptr;
    QStringList m_selected,m_advancementLabels;
    QVector<CompetitionInfo *> m_advancementOptions;
    QVariantList m_events,m_classifications;
    QVariantList m_datePlan;
    QHash<CompetitionInfo *,QDate> m_plannedDates;
    void applyDates(const QHash<CompetitionInfo *,QDate> &dates);
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
    bool canRestore(const Snapshot &snapshot) const;
    void restore(const Snapshot &snapshot);
    void collectRetired();
    void refresh();
    void setOrder(const QVector<CompetitionInfo *> &events);
    void fail(const QString &message);
    CompetitionInfo *clone(CompetitionInfo *event);
    DatabaseItemsListView *calendarList() const;
};
