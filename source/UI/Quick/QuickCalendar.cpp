#include "QuickCalendar.h"
#include "../Seasons/CalendarEditor/CalendarEditorWidget.h"
#include "../Seasons/SimulationSaveManagerWindow.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../EditorWidgets/ClassificationEditorWidget.h"
#include <QComboBox>
#include <QTableView>
#include <QTimer>
#include "../DatabaseEditor/DatabaseItemsListView.h"
#include "../DatabaseEditor/ListModels/CalendarsListModel.h"
#include "../../global/GlobalDatabase.h"
#include <QItemSelectionModel>
#include <QListView>
#include <QPushButton>
#include <QLineEdit>
#include <QAction>
#include <QCheckBox>
#include <QSpinBox>
#include <QAbstractButton>
#include <QJsonDocument>
#include <algorithm>
#include <QLocale>
#include <QAbstractListModel>
#include <QDir>
#include <QFile>
#include <QUrl>
#include "../../global/CountryCodes.h"

class QuickCalendarEvents final : public QAbstractListModel {
public:
    explicit QuickCalendarEvents(QObject *parent):QAbstractListModel(parent) {}
    int rowCount(const QModelIndex &parent={}) const override { return parent.isValid()?0:m_rows.size(); }
    QVariant data(const QModelIndex &index,int role) const override {
        return index.isValid() && index.row()>=0 && index.row()<m_rows.size() && role==Qt::UserRole+1 ? m_rows[index.row()] : QVariant();
    }
    QHash<int,QByteArray> roleNames() const override { return {{Qt::UserRole+1,"entry"}}; }
    void sync(const QVariantList &rows) {
        QSet<QString> retained;
        for(const auto &row:rows)retained.insert(row.toMap().value("id").toString());
        for(int i=m_rows.size()-1;i>=0;--i)if(!retained.contains(m_rows[i].toMap().value("id").toString())) {
            beginRemoveRows({},i,i);m_rows.removeAt(i);endRemoveRows();
        }
        for(int i=0;i<rows.size();++i) {
            const auto id=rows[i].toMap().value("id").toString();int at=i;
            while(at<m_rows.size() && m_rows[at].toMap().value("id").toString()!=id)++at;
            if(at==m_rows.size()) { beginInsertRows({},i,i);m_rows.insert(i,rows[i]);endInsertRows(); }
            else {
                if(at!=i) { beginMoveRows({},at,at,{},i);m_rows.move(at,i);endMoveRows(); }
                if(m_rows[i]!=rows[i]) { m_rows[i]=rows[i];emit dataChanged(index(i,0),index(i,0),{Qt::UserRole+1}); }
            }
        }
    }
private:
    QVariantList m_rows;
};

namespace {
bool mainEvent(CompetitionInfo *event) { return event && event->getSerieType()<=CompetitionInfo::Qualifications; }
QJsonObject ruleValue(const CompetitionRules &rules) { auto value=CompetitionRules::getJsonObject(rules);value.remove("id");return value; }
int presetIndex(const CompetitionRules &value,const QVector<CompetitionRules> &rules) {
    for(int i=0;i<rules.size();++i)if(ruleValue(value)==ruleValue(rules[i]))return i+1;
    return 0;
}
}
QuickCalendar::QuickCalendar(CalendarEditorWidget *editor,QObject *parent):QObject(parent),m_editor(editor) {
    m_eventModel=new QuickCalendarEvents(this);
    m_save=editor->getSave();
    m_rulesEditor=new CompetitionRulesEditorWidget(editor);m_rulesEditor->setObjectName("quickCalendarRules");
    m_rulesEditor->setProperty("quickExposePage",true);m_rulesEditor->removeSubmitButton();m_rulesEditor->hide();
    m_rulesEditor->setCompetitionRules(&m_customRules);
    connect(m_rulesEditor,&CompetitionRulesEditorWidget::configurationChanged,this,[this]{
        if(m_mutating || m_rulesPending)return;m_rulesPending=true;auto *calendar=m_calendar;const auto ids=m_selected;const int target=m_rulesTarget;const auto revision=++m_rulesRevision;
        QTimer::singleShot(0,this,[this,calendar,ids,target,revision]{if(revision!=m_rulesRevision || !m_rulesPending)return;if(calendar==m_calendar && ids==m_selected && target==m_rulesTarget)commitPendingRules();else m_rulesPending=false;});
    });
    m_classEditor=new ClassificationEditorWidget(nullptr,editor);m_classEditor->setObjectName("quickCalendarClassification");m_classEditor->setProperty("quickExposePage",true);m_classEditor->hide();
    if(auto *button=m_classEditor->findChild<QPushButton *>("pushButton_submit"))button->hide();
    const auto changed=[this]{if(m_loadingClass||m_classPending)return;m_classPending=true;const auto id=m_classId;auto *calendar=m_calendar;QTimer::singleShot(0,this,[this,id,calendar]{m_classPending=false;if(id==m_classId && calendar==m_calendar)applyClassification();});};
    connect(m_classEditor,&ClassificationEditorWidget::configurationChanged,this,changed);
    for(auto *input:m_classEditor->findChildren<QLineEdit *>())connect(input,&QLineEdit::editingFinished,this,changed);
    for(auto *choice:m_classEditor->findChildren<QComboBox *>())connect(choice,QOverload<int>::of(&QComboBox::activated),this,changed);
    for(auto *table:m_classEditor->findChildren<QTableView *>())if(table->model()){connect(table->model(),&QAbstractItemModel::dataChanged,this,changed);connect(table->model(),&QAbstractItemModel::rowsInserted,this,changed);connect(table->model(),&QAbstractItemModel::rowsRemoved,this,changed);}
    const auto dirty=[this]{if(!m_mutating){m_dirty=true;}};
    connect(editor,&CalendarEditorWidget::changed,this,dirty);
    connect(editor,&CalendarEditorWidget::calendarPresetLoaded,this,dirty);
    connect(editor->getModel(),&QAbstractItemModel::modelReset,this,dirty);
    connect(editor->getModel(),&QAbstractItemModel::dataChanged,this,dirty);
    sync();
}
QuickCalendar::~QuickCalendar(){m_history=History{};collectRetired();}
CompetitionRulesEditorWidget *QuickCalendar::rulesEditor() const { return m_rulesEditor; }
QVector<CompetitionInfo *> QuickCalendar::mains() const {
    QVector<CompetitionInfo *> result;if(m_calendar)for(auto *event:m_calendar->getCompetitionsReference())if(mainEvent(event))result.append(event);return result;
}
QVector<CompetitionInfo *> QuickCalendar::selected() const {
    QVector<CompetitionInfo *> result;for(auto *event:mains())if(m_selected.contains(event->getIDStr()))result.append(event);return result;
}
bool QuickCalendar::locked(CompetitionInfo *event) const {
    if(!event || !m_calendar || !m_editor)return true;
    QVector<CompetitionInfo *> block=event->getTrainings();if(event->getTrialRound())block.append(event->getTrialRound());block.append(event);
    for(auto *part:block)if(part->getPlayed() || part->getCancelled() || m_calendar->getCompetitionsReference().indexOf(part)<m_editor->getModel()->getDontModifyBefore())return true;
    return false;
}
bool QuickCalendar::editable() const { const auto items=selected();return !items.isEmpty() && std::none_of(items.begin(),items.end(),[this](auto *event){return locked(event);}); }
bool QuickCalendar::referencedByHistory(CompetitionInfo *event) const {
    for(auto *other:mains())if(other->getAdvancementCompetition()==event && locked(other))return true;
    return false;
}
QAbstractItemModel *QuickCalendar::eventModel() const { return m_eventModel; }
QString QuickCalendar::hillFlag(Hill *hill) const {
    if(!hill)return {};
    const auto path=QDir("flags").absoluteFilePath(CountryCodes::toIso(hill->getCountryCode())+".svg");
    return QFile::exists(path)?QUrl::fromLocalFile(path).toString():QString();
}
QStringList QuickCalendar::hillFlags() const {
    QStringList result;
    if(m_editor)for(auto *hill:*m_editor->getModel()->getHillsList())result.append(hillFlag(hill));
    return result;
}
QStringList QuickCalendar::hills() const { QStringList result;if(m_editor)for(auto *hill:*m_editor->getModel()->getHillsList())result.append(hill->getName()+" HS"+QString::number(hill->getHSPoint()));return result; }
QStringList QuickCalendar::rules() const { QStringList result{tr("Zasady własne")};if(m_editor)for(const auto &rules:*m_editor->getModel()->getRulesList())result.append(rules.getName());return result; }
QStringList QuickCalendar::auxiliaryRules() const { auto result=rules();result.prepend(tr("Takie jak konkurs"));return result; }
void QuickCalendar::setRulesTarget(int target) { commitPendingRules(); if(target<0||target>2||target==m_rulesTarget)return;m_rulesTarget=target;resetCustomRules();emit changed(); }
DatabaseItemsListView *QuickCalendar::calendarList() const {
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))return manager->getCalendarsListView();
    return nullptr;
}
QStringList QuickCalendar::calendars() const { QStringList result;if(auto *list=calendarList())for(auto *calendar:*list->getCalendars())result.append(calendar->getName());return result; }
int QuickCalendar::calendarIndex() const { auto *list=calendarList();return list?list->getCalendars()->indexOf(m_calendar):-1; }
QString QuickCalendar::calendarName() const { return m_calendar?m_calendar->getName():QString(); }
void QuickCalendar::collectRetired() {
    QSet<CompetitionInfo *> retained;
    QSet<Classification *> classes;
    if(m_save)for(const auto &season:m_save->getSeasonsReference())for(auto *calendar:season.getCalendars()){for(auto *event:calendar->getCompetitionsReference())retained.insert(event);for(auto *entry:calendar->getClassificationsReference())classes.insert(entry);}
    classes.unite(m_liveClasses);
    retained.unite(m_live);
    for(const auto &stack:{m_history.undo,m_history.redo})for(const auto &state:stack){for(auto *event:state.order)retained.insert(event);for(auto *entry:state.classes)classes.insert(entry);for(auto *calendar:state.calendars){for(auto *event:calendar->getCompetitionsReference())retained.insert(event);for(auto *entry:calendar->getClassificationsReference())classes.insert(entry);}}
    for(auto *event:m_retired)if(!retained.contains(event))delete event;
    m_retired.intersect(retained);for(auto *entry:m_retiredClasses)if(!classes.contains(entry))delete entry;m_retiredClasses.intersect(classes);
}
void QuickCalendar::clearUndo() {m_history=History{};collectRetired();}
bool QuickCalendar::activeCalendar() const {
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))return manager->getSimulationSave()->getActualSeason()->getActualCalendar()==m_calendar;
    return false;
}
QVariantList QuickCalendar::calendarEntries() const {
    QVariantList result;if(auto *list=calendarList())for(auto *calendar:*list->getCalendars())result.append(QVariantMap{{"name",calendar->getName()},{"index",list->getCalendars()->indexOf(calendar)},{"selected",calendar==m_calendar},{"active",[&]{for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))return manager->getSimulationSave()->getActualSeason()->getActualCalendar()==calendar;return false;}()},{"count",[&]{int count=0;for(auto *event:calendar->getCompetitionsReference())if(mainEvent(event))++count;return count;}()}});return result;
}
void QuickCalendar::activateCalendar() {
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner)){m_history.undo.append(capture());m_history.redo.clear();m_mutating=true;manager->activateEditingCalendar();m_mutating=false;m_dirty=false;emit changed();return;}
}
void QuickCalendar::sync() {
    if(!m_editor)return;
    const void *season=nullptr;for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))season=manager->getSimulationSave()->getActualSeason();
    if(m_season && m_season!=season)clearUndo();m_season=season;
    auto *calendar=m_editor->getModel()->getCalendar();
    if(calendar==m_calendar && !m_dirty && m_language==tr("Konkurs"))return;
    if(m_calendar==calendar && m_dirty && !m_mutating)clearUndo();if(m_calendar!=calendar){m_selected.clear();m_selection.clear();m_message.clear();m_classId.clear();}m_calendar=calendar;m_dirty=false;m_language=tr("Konkurs");
    m_live.clear();if(m_calendar)for(auto *event:m_calendar->getCompetitionsReference())m_live.insert(event);
    refresh();loadClass();
}
void QuickCalendar::refresh() {
    discardDatePlan();discardWeekend();
    m_liveClasses.clear();if(m_calendar)for(auto *entry:m_calendar->getClassificationsReference())m_liveClasses.insert(entry);
    const auto all=mains();QStringList existing;for(auto *event:all)existing.append(event->getIDStr());
    for(int i=m_selected.size()-1;i>=0;--i)if(!existing.contains(m_selected[i]))m_selected.removeAt(i);
    if(m_selected.isEmpty() && !all.isEmpty() && m_selection.isEmpty()){auto *first=all.first();for(auto *event:all)if(!locked(event)){first=event;break;}m_selected.append(first->getIDStr());}
    const auto chosen=selected();m_events.clear();
    for(int i=0;i<all.size();++i){auto *event=all[i];const auto hill=event->getHill();
        QStringList dependencies;for(auto *other:all)if(other->getPendingAdvancementCompetition()==event)dependencies.append(QString::number(all.indexOf(other)+1));
        auto *pending=event->getPendingAdvancementCompetition();
        m_events.append(QVariantMap{{"pending",pending!=nullptr},{"pendingSource",pending?all.indexOf(pending)+1:0},{"dependents",dependencies.join(", ")},{"id",event->getIDStr()},{"number",i+1},{"title",hill?hill->getName()+" HS"+QString::number(hill->getHSPoint()):tr("Brak skoczni")},
            {"flag",hillFlag(hill)},{"qualification",event->getSerieType()==CompetitionInfo::Qualifications},{"weekday",event->getScheduledDate().isValid()?QLocale().standaloneDayName(event->getScheduledDate().dayOfWeek(),QLocale::ShortFormat):QString()},
            {"date",event->getScheduledDate().toString(Qt::ISODate)},{"dateText",datesEnabled()?(event->getScheduledDate().isValid()?dateText(event->getScheduledDate().toString(Qt::ISODate)):tr("Bez daty")):QString()},
            {"type",event->getSerieType()==CompetitionInfo::Qualifications?tr("Kwalifikacje"):tr("Konkurs")},{"team",event->getRules().getCompetitionType()==CompetitionRules::Team},
            {"trainings",event->getTrainingsReference().size()},{"trial",event->getTrialRound()!=nullptr},{"locked",locked(event)},{"played",event->getPlayed()},{"cancelled",event->getCancelled()},{"selected",m_selected.contains(event->getIDStr())}});
    }
    m_eventModel->sync(m_events);
    m_selection={{"id",chosen.size()==1?chosen.first()->getIDStr():QString()},{"flag",chosen.size()==1?hillFlag(chosen.first()->getHill()):QString()},{"count",chosen.size()},{"locked",!editable()},{"title",chosen.size()==1?m_events.value(all.indexOf(chosen.first())).toMap().value("title"):tr("%1 konkursów").arg(chosen.size())}};
    auto common=[&](const QString &key,const std::function<QVariant(CompetitionInfo *)> &read){
        if(chosen.isEmpty())return;QVariant value=read(chosen.first());for(auto *event:chosen)if(read(event)!=value){value=-1;break;}m_selection[key]=value;
    };
    if(m_editor){common("hill",[&](auto *e){return m_editor->getModel()->getHillsList()->indexOf(e->getHill());});common("rules",[&](auto *e){return presetIndex(e->getRules(),*m_editor->getModel()->getRulesList());});}
    common("date",[](auto *e){return e->getScheduledDate().toString(Qt::ISODate);});
    common("kind",[](auto *e){return e->getRules().getCompetitionType();});common("type",[](auto *e){return e->getSerieType();});
    common("trainings",[](auto *e){return e->getTrainingsReference().size();});common("trial",[](auto *e){return int(e->getTrialRound()!=nullptr);});common("importance",[](auto *e){return e->getJumpsImportance();});
    common("trainingImportance",[](auto *e){return e->getTrainings().isEmpty()?5:e->getTrainings().first()->getJumpsImportance();});
    common("trialImportance",[](auto *e){return e->getTrialRound()?e->getTrialRound()->getJumpsImportance():5;});
    common("trainingRules",[&](auto *e){return e->getTrainings().isEmpty() || ruleValue(e->getTrainings().first()->getRules())==ruleValue(e->getRules())?0:1+presetIndex(e->getTrainings().first()->getRules(),*m_editor->getModel()->getRulesList());});
    common("trialRules",[&](auto *e){return !e->getTrialRound() || ruleValue(e->getTrialRound()->getRules())==ruleValue(e->getRules())?0:1+presetIndex(e->getTrialRound()->getRules(),*m_editor->getModel()->getRulesList());});
    m_classifications.clear();if(m_calendar)for(int i=0;i<m_calendar->getClassificationsReference().size();++i){auto *cls=m_calendar->getClassificationsReference()[i];int count=0;for(auto *event:chosen)if(event->getClassificationsReference().contains(cls))++count;
        QStringList methodNames{tr("Domyślny")},methodIds{QString()};
        const auto options=cls->getOptions();
        for(const auto &method:options.methods){methodNames.append(method.name);methodIds.append(method.id);}
        int methodIndex=0;QVariantMap parameters,mixedParameters;
        if(!chosen.isEmpty()){
            const auto first=chosen.first()->scoringFor(cls);methodIndex=qMax(0,int(methodIds.indexOf(first.methodId)));parameters=first.parameters.toVariantMap();
            for(auto *event:chosen)if(event->scoringFor(cls).methodId!=first.methodId)methodIndex=-1;
            for(const auto &key:{"multiplier","points-limit","minimum-age","maximum-age","country-count"})
                for(auto *event:chosen)if(event->scoringFor(cls).parameters.value(key)!=first.parameters.value(key))mixedParameters[key]=true;
        }
        m_classifications.append(QVariantMap{{"name",cls->getName()},{"index",i},{"checked",!chosen.isEmpty()&&count==chosen.size()},
            {"mixed",count>0&&count<chosen.size()},{"methodNames",methodNames},{"methodIds",methodIds},{"methodIndex",methodIndex},
            {"contextAllowed",options.contextParameters},{"parameters",parameters},{"mixedParameters",mixedParameters},
            {"defaults",QVariantMap{{"multiplier",1.},{"points-limit",0},{"minimum-age",options.minimumAge},{"maximum-age",options.maximumAge},{"country-count",options.countryCount}}}});
    }
    m_advancementOptions={nullptr};m_advancementLabels=QStringList{tr("Brak")};
    for(auto *target:all){bool allowed=!chosen.isEmpty();for(auto *event:chosen)if(all.indexOf(target)>=all.indexOf(event)||target->getRules().getCompetitionType()!=event->getRules().getCompetitionType())allowed=false;
        if(allowed){m_advancementOptions.append(target);m_advancementLabels.append(QString::number(all.indexOf(target)+1)+". "+(target->getHill()?target->getHill()->getName():tr("Brak skoczni")));}}
    if(chosen.size()==1)if(auto *source=chosen.first()->getPendingAdvancementCompetition();source && all.contains(source) && source->getRules().getCompetitionType()==chosen.first()->getRules().getCompetitionType() && !m_advancementOptions.contains(source)) {m_advancementOptions.append(source);m_advancementLabels.append(tr("%1. %2 — oczekuje").arg(all.indexOf(source)+1).arg(source->getHill()?source->getHill()->getName():tr("Brak skoczni")));}
    m_selection["pendingSource"]=chosen.size()==1 && chosen.first()->getPendingAdvancementCompetition()?all.indexOf(chosen.first()->getPendingAdvancementCompetition())+1:0;
    common("advancement",[&](auto *e){return m_advancementOptions.indexOf(e->getAdvancementCompetition()?e->getAdvancementCompetition():e->getPendingAdvancementCompetition());});
    m_advancementClasses={nullptr};m_advancementClassLabels=QStringList{tr("Brak")};
    if(m_calendar)for(auto *classification:m_calendar->getClassificationsReference()){bool compatible=!chosen.isEmpty();for(auto *event:chosen)if(event->getRules().getCompetitionType()!=classification->getClassificationType())compatible=false;if(compatible){m_advancementClasses.append(classification);m_advancementClassLabels.append(classification->getName());}}
    common("advancementClass",[&](auto *e){return m_advancementClasses.indexOf(e->getAdvancementClassification());});
    resetCustomRules();
    emit changed();
}
void QuickCalendar::select(const QString &id,int modifiers) {
    commitPendingRules();
    sync();const auto all=mains();int row=-1;for(int i=0;i<all.size();++i)if(all[i]->getIDStr()==id){row=i;break;}if(row<0)return;
    if(modifiers & Qt::ShiftModifier && !m_selected.isEmpty()){
        int anchor=0;for(int i=0;i<all.size();++i)if(all[i]->getIDStr()==m_selected.first())anchor=i;
        m_selected.clear();for(int i=qMin(anchor,row);i<=qMax(anchor,row);++i)m_selected.append(all[i]->getIDStr());
    }else if(modifiers & Qt::ControlModifier){if(m_selected.contains(id))m_selected.removeAll(id);else m_selected.append(id);}else m_selected=QStringList{id};
    if(m_editor->getTableView()->selectionModel())m_editor->getTableView()->selectionModel()->setCurrentIndex(m_editor->getModel()->index(row,0),QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
    m_message.clear();refresh();
}
void QuickCalendar::clearSelection() {commitPendingRules();sync();m_selected.clear();refresh();}
void QuickCalendar::selectAll(bool futureOnly) {commitPendingRules(); sync();m_selected.clear();for(auto *event:mains())if(!futureOnly||!locked(event))m_selected.append(event->getIDStr());refresh(); }
QuickCalendar::Snapshot QuickCalendar::capture() const {
    Snapshot value;value.owner=m_calendar;if(auto *list=calendarList())value.calendars=*list->getCalendars();for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))value.active=manager->getSimulationSave()->getActualSeason()->getActualCalendar();value.selected=m_selected;value.classId=m_classId;value.boundary=m_editor?m_editor->getModel()->getDontModifyBefore():0;if(!m_calendar)return value;value.order=m_calendar->getCompetitionsReference();value.name=m_calendar->getName();value.classes=m_calendar->getClassificationsReference();for(auto *entry:value.classes)value.classStates.insert(entry,*entry);
    for(auto *e:value.order){State s;s.date=e->getScheduledDate();s.played=e->getPlayed();s.cancelled=e->getCancelled();s.hill=e->getHill();s.rules=e->getRules();s.type=e->getSerieType();s.importance=e->getJumpsImportance();s.trainings=e->getTrainings();s.trial=e->getTrialRound();s.advancement=e->getAdvancementCompetition();s.pending=e->getPendingAdvancementCompetition();s.classification=e->getAdvancementClassification();s.classifications=e->getClassifications();s.scoring=e->getClassificationScoring();value.states.insert(e,s);}return value;
}
void QuickCalendar::mutate(const std::function<void()> &operation) {
    if(!m_editor||!m_calendar)return;
    auto &history=m_history;history.undo.append(capture());history.redo.clear();m_mutating=true;
    m_editor->getModel()->applyChange([&]{operation();for(auto *event:mains()){for(auto *training:event->getTrainings())if(!training->getScheduledDate().isValid())training->setScheduledDate(event->getScheduledDate());if(event->getTrialRound()&&!event->getTrialRound()->getScheduledDate().isValid())event->getTrialRound()->setScheduledDate(event->getScheduledDate());}m_calendar->normalizeCompetitionEvents();m_calendar->fixAdvancementCompetitions();m_calendar->fixAdvancementClassifications();m_calendar->updateCompetitionsQualifyingCompetitions();});
    const QSet<CompetitionInfo *> before=m_live;m_live.clear();for(auto *event:m_calendar->getCompetitionsReference())m_live.insert(event);for(auto *event:before)if(!m_live.contains(event))m_retired.insert(event);
    emit m_editor->changed();m_mutating=false;m_dirty=false;refresh();collectRetired();
}
void QuickCalendar::fail(const QString &message){m_message=message;emit changed();}
void QuickCalendar::update(const QVariantMap &fields) {
    commitPendingRules();
    sync();if(!editable()){fail(tr("Rozegrane wydarzenia są zablokowane."));return;}
    bool differs=false;
    const QStringList keys{"hill","rules","kind","type","trainings","trial","trainingRules","trialRules","importance","trainingImportance","trialImportance","advancement","advancementClass"};
    for(auto it=fields.cbegin();it!=fields.cend();++it) {
        if(!keys.contains(it.key()))return;
        if(it.key()=="rules"&&it.value().toInt()==0)continue;
        if((it.key()=="trainingRules"||it.key()=="trialRules")&&it.value().toInt()==1)continue;
        if(m_selection.value(it.key())!=it.value())differs=true;
    }
    if(!differs)return;
    const auto items=selected();const auto *hills=m_editor->getModel()->getHillsList();const auto *rules=m_editor->getModel()->getRulesList();
    for(auto it=fields.cbegin();it!=fields.cend();++it){int value=it.value().toInt();
        if(it.key()=="hill"&&(value<0||value>=hills->size()))return;
        if(it.key()=="rules"&&(value<0||value>rules->size()))return;
        if((it.key()=="trainingRules"||it.key()=="trialRules")&&(value<0||value>rules->size()+1))return;
        if((it.key()=="kind"||it.key()=="type")&&(value<0||value>1))return;
        if(it.key()=="trainings"&&(value<0||value>30))return;
        if(it.key()=="advancement"&&(value<0||value>=m_advancementOptions.size()))return;
        if(it.key()=="advancementClass"&&(value<0||value>=m_advancementClasses.size()))return;
        if((it.key()=="importance"||it.key()=="trainingImportance"||it.key()=="trialImportance")&&(!qIsFinite(it.value().toDouble())||it.value().toDouble()<0||it.value().toDouble()>1000))return;
    }
    if(fields.contains("kind"))for(auto *event:items)if(referencedByHistory(event)&&fields.value("kind").toInt()!=event->getRules().getCompetitionType()){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    if(fields.contains("rules") && fields.value("rules").toInt()>0)for(auto *event:items)if(referencedByHistory(event)&&rules->at(fields.value("rules").toInt()-1).getCompetitionType()!=event->getRules().getCompetitionType()){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    for(auto *event:items){int kind=event->getRules().getCompetitionType();if(fields.contains("rules") && fields.value("rules").toInt()>0)kind=rules->at(fields.value("rules").toInt()-1).getCompetitionType();if(fields.contains("kind"))kind=fields.value("kind").toInt();
        if(fields.contains("advancementClass")){auto *source=m_advancementClasses.value(fields.value("advancementClass").toInt());if(source && source->getClassificationType()!=kind){fail(tr("Źródło awansu musi mieć ten sam rodzaj konkursu."));return;}}
        if(fields.contains("advancement")){auto *source=m_advancementOptions.value(fields.value("advancement").toInt());if(source && (source->getRules().getCompetitionType()!=kind || mains().indexOf(source)>=mains().indexOf(event))){fail(tr("Kwalifikacje muszą być wcześniejszym konkursem tego samego rodzaju."));return;}}
    }
    const auto advancement=m_advancementOptions;const auto advancementClasses=m_advancementClasses;
    m_message=tr("Zmiany zapisane w kalendarzu");
    mutate([&]{for(auto *e:items){
        if(fields.contains("hill")){e->setHill(hills->at(fields.value("hill").toInt()));for(auto *t:e->getTrainings())t->setHill(e->getHill());if(e->getTrialRound())e->getTrialRound()->setHill(e->getHill());}
        if(fields.contains("rules")&&fields.value("rules").toInt()>0)e->setRules(rules->at(fields.value("rules").toInt()-1));
        if(fields.contains("kind"))e->getRulesPointer()->setCompetitionType(fields.value("kind").toInt());
        if(fields.contains("type"))e->setSerieType(fields.value("type").toInt());
        if(fields.contains("importance"))e->setJumpsImportance(fields.value("importance").toDouble());
        if(fields.contains("trainings")){
            const int count=fields.value("trainings").toInt();auto trainings=e->getTrainings();const auto defaultRules=trainings.isEmpty()?e->getRules():trainings.first()->getRules();
            while(trainings.size()>count){auto *t=trainings.takeLast();m_calendar->getCompetitionsReference().removeAll(t);}
            while(trainings.size()<count){auto *t=new CompetitionInfo(e->getHill());t->setRules(defaultRules);t->setSerieType(CompetitionInfo::Training);trainings.append(t);}
            e->setTrainings(trainings);
        }
        if(fields.contains("trial")){
            if(fields.value("trial").toBool()&&!e->getTrialRound()){auto *t=new CompetitionInfo(e->getHill());t->setRules(e->getRules());t->setSerieType(CompetitionInfo::TrialRound);e->setTrialRound(t);}
            else if(!fields.value("trial").toBool()&&e->getTrialRound()){m_calendar->getCompetitionsReference().removeAll(e->getTrialRound());e->setTrialRound(nullptr);}
        }
        if(fields.contains("trainingRules")){const int n=fields.value("trainingRules").toInt();if(n!=1)for(auto *t:e->getTrainings())t->setRules(n?rules->at(n-2):e->getRules());}
        if(fields.contains("trialRules")&&e->getTrialRound()){const int n=fields.value("trialRules").toInt();if(n!=1)e->getTrialRound()->setRules(n?rules->at(n-2):e->getRules());}
        if(fields.contains("trainingImportance"))for(auto *t:e->getTrainings())t->setJumpsImportance(fields.value("trainingImportance").toDouble());
        if(fields.contains("trialImportance")&&e->getTrialRound())e->getTrialRound()->setJumpsImportance(fields.value("trialImportance").toDouble());
        if(fields.contains("advancement")){e->setAdvancementCompetition(advancement[fields.value("advancement").toInt()]);if(e->getAdvancementCompetition())e->setAdvancementClassification(nullptr);}
        if(fields.contains("advancementClass")){const int n=fields.value("advancementClass").toInt();e->setAdvancementClassification(advancementClasses[n]);if(n)e->setAdvancementCompetition(nullptr);}
    }});
}
void QuickCalendar::setClassification(int index,bool checked) {
    sync();if(!editable()||index<0||index>=m_calendar->getClassificationsReference().size())return;auto *cls=m_calendar->getClassificationsReference()[index];const auto items=selected();
    m_message=tr("Zmiany zapisane w kalendarzu");mutate([&]{for(auto *e:items){if(checked&&!e->getClassificationsReference().contains(cls))e->getClassificationsReference().append(cls);else if(!checked)e->getClassificationsReference().removeAll(cls);}});
}
void QuickCalendar::setClassificationScoring(int index,const QVariantMap &patch)
{
    sync();if(!editable() || !m_calendar || index<0 || index>=m_calendar->getClassificationsReference().size())return;
    auto *classification=m_calendar->getClassificationsReference()[index];
    const auto options=classification->getOptions();
    QStringList ids{QString()};for(const auto &method:options.methods)ids.append(method.id);
    const auto events=selected();QHash<CompetitionInfo *,ClassificationCompetitionSettings> next;
    for(auto *event:events){
        if(!event->getClassificationsReference().contains(classification))continue;
        auto settings=event->scoringFor(classification);
        for(auto it=patch.begin();it!=patch.end();++it){
            if(it.key()=="method"){
                if(!ids.contains(it.value().toString()))return;
                settings.methodId=it.value().toString();continue;
            }
            if(!options.contextParameters)return;
            const QStringList keys{"multiplier","points-limit","minimum-age","maximum-age","country-count"};
            if(!keys.contains(it.key()))return;
            if(it.value().isNull())settings.parameters.remove(it.key());
            else settings.parameters.insert(it.key(),QJsonValue::fromVariant(it.value()));
        }
        const auto valid=ClassificationCompetitionSettings::fromJson(settings.toJson());
        if(valid.parameters!=settings.parameters){fail(tr("Nieprawidłowa wartość parametru klasyfikacji."));return;}
        if(settings.parameters.value("minimum-age").toInt(options.minimumAge)>settings.parameters.value("maximum-age").toInt(options.maximumAge)){
            fail(tr("Minimalny wiek nie może przekraczać maksymalnego."));return;
        }
        next.insert(event,settings);
    }
    if(next.isEmpty())return;
    bool changed=false;for(auto it=next.begin();it!=next.end();++it)if(it.key()->scoringFor(classification).toJson()!=it.value().toJson())changed=true;
    if(!changed)return;
    mutate([&]{for(auto it=next.begin();it!=next.end();++it)it.key()->setScoringFor(classification,it.value());});
}
void QuickCalendar::setOrder(const QVector<CompetitionInfo *> &events) {
    QVector<CompetitionInfo *> order;for(auto *e:events){order+=e->getTrainings();if(e->getTrialRound())order.append(e->getTrialRound());order.append(e);}
    for(auto *e:m_calendar->getCompetitionsReference())if(!mainEvent(e)&&!order.contains(e)&&!m_retired.contains(e)){
        bool owned=false;for(auto *old:mains())if(old->getTrainings().contains(e)||old->getTrialRound()==e)owned=true;if(!owned)order.append(e);
    }
    m_calendar->setCompetitions(order);
}
CompetitionInfo *QuickCalendar::clone(CompetitionInfo *e) {
    const auto copySettings=[](CompetitionInfo *event) {
        auto *copy=new CompetitionInfo(event->getHill());
        copy->setRules(event->getRules());
        copy->setSerieType(event->getSerieType());
        copy->setExceptionalRoundsCount(event->getExceptionalRoundsCount());
        copy->setJumpsImportance(event->getJumpsImportance());
        copy->setClassifications(event->getClassifications());
        copy->setClassificationScoring(event->getClassificationScoring());
        copy->setScheduledDate(event->getScheduledDate());
        copy->setAdvancementCompetition(event->getAdvancementCompetition());
        copy->setPendingAdvancementCompetition(event->getPendingAdvancementCompetition());
        copy->setAdvancementClassification(event->getAdvancementClassification());
        return copy;
    };
    auto *copy=copySettings(e);
    for(auto *training:e->getTrainings())copy->getTrainingsReference().append(copySettings(training));
    if(e->getTrialRound())copy->setTrialRound(copySettings(e->getTrialRound()));
    return copy;
}
void QuickCalendar::add() {addMany(1);}
void QuickCalendar::addMany(int count) {
    sync();if(!m_calendar||count<1||count>100)return;if(hills().isEmpty()){fail(tr("Najpierw dodaj skocznię do zapisu."));return;}
    const auto items=selected();m_message=tr("Dodano konkursy: %1").arg(count);mutate([&]{auto order=mains();int at=order.size();if(!items.isEmpty()&&!locked(items.last()))at=order.indexOf(items.last())+1;
        m_selected.clear();for(int i=0;i<count;++i){auto *event=new CompetitionInfo(m_editor->getModel()->getHillsList()->first());if(!m_editor->getModel()->getRulesList()->isEmpty())event->setRules(m_editor->getModel()->getRulesList()->first());order.insert(at++,event);m_selected.append(event->getIDStr());}setOrder(order);});
}
void QuickCalendar::duplicate() {
    sync();const auto items=selected();if(items.isEmpty())return;
    auto proposed=mains();int insertion=locked(items.last())?proposed.size():proposed.indexOf(items.last())+1;
    for(auto *event:items)proposed.insert(insertion++,event);
    if(!validateDateOrder(proposed))return;
    m_message=tr("Skopiowano konkursy");
    mutate([&]{
        auto order=mains();int at=locked(items.last())?order.size():order.indexOf(items.last())+1;
        QHash<CompetitionInfo *,CompetitionInfo *> copies;m_selected.clear();
        for(auto *event:items){
            auto *copy=clone(event);copies.insert(event,copy);
            const auto trainings=event->getTrainings(),copiedTrainings=copy->getTrainings();
            for(int i=0;i<trainings.size();++i)copies.insert(trainings[i],copiedTrainings[i]);
            if(event->getTrialRound())copies.insert(event->getTrialRound(),copy->getTrialRound());
            order.insert(at++,copy);m_selected.append(copy->getIDStr());
        }
        for(auto *copy:copies){
            auto *source=copy->getAdvancementCompetition();auto *pending=copy->getPendingAdvancementCompetition();
            if(copies.contains(source))copy->setAdvancementCompetition(copies.value(source));
            if(copies.contains(pending))copy->setPendingAdvancementCompetition(copies.value(pending));
        }
        setOrder(order);
    });
}
void QuickCalendar::remove() {
    sync();if(!editable())return;const auto items=selected();for(auto *event:items)if(referencedByHistory(event)){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    QStringList dependencies;for(auto *event:mains())if(!items.contains(event) && (items.contains(event->getAdvancementCompetition())||items.contains(event->getPendingAdvancementCompetition())))dependencies.append(QString::number(mains().indexOf(event)+1));
    if(!dependencies.isEmpty()){fail(tr("Najpierw zmień źródło kwalifikacji w konkursach: %1.").arg(dependencies.join(", ")));return;}
    m_message=tr("Usunięto konkursy. Możesz cofnąć zmianę.");mutate([&]{auto order=mains();for(auto *event:items){order.removeAll(event);m_retired.insert(event);for(auto *t:event->getTrainings())m_retired.insert(t);if(event->getTrialRound())m_retired.insert(event->getTrialRound());}setOrder(order);m_selected.clear();});
}
void QuickCalendar::move(int direction) {
    sync();if(!editable()||(direction!=-1&&direction!=1))return;const auto items=selected();auto order=mains();bool moved=false;
    if(direction<0){for(int i=1;i<order.size();++i)if(items.contains(order[i])&&!items.contains(order[i-1])&&!locked(order[i-1])){qSwap(order[i],order[i-1]);moved=true;}}
    else for(int i=order.size()-2;i>=0;--i)if(items.contains(order[i])&&!items.contains(order[i+1])&&!locked(order[i+1])){qSwap(order[i],order[i+1]);moved=true;}
    if(moved){
        if(!validateDateOrder(order))return;
        m_message=tr("Zmieniono kolejność konkursów");mutate([&]{setOrder(order);});
    }
}
bool QuickCalendar::canRestore(const Snapshot &state) const {
    for(auto it=state.states.cbegin();it!=state.states.cend();++it)if(it.key()->getPlayed()!=it.value().played || it.key()->getCancelled()!=it.value().cancelled)return false;
    if(state.owner)for(auto *event:state.owner->getCompetitionsReference())if((event->getPlayed()||event->getCancelled())&&!state.order.contains(event))return false;
    if(auto *list=calendarList())for(auto *calendar:*list->getCalendars())if(!state.calendars.contains(calendar))for(auto *event:calendar->getCompetitionsReference())if(event->getPlayed()||event->getCancelled())return false;
    return true;
}
void QuickCalendar::restore(const Snapshot &state) {
    m_mutating=true;
    if(m_calendar)for(auto *event:m_calendar->getCompetitionsReference())if(!state.order.contains(event))m_retired.insert(event);
    if(auto *list=calendarList())if(auto *model=qobject_cast<CalendarsListModel *>(list->getListModel()))model->resetContents(state.calendars);
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner)) {
        manager->getSimulationSave()->getActualSeason()->setActualCalendar(state.active);manager->setEditingCalendar(state.owner);emit manager->actualCalendarChanged();break;
    }
    m_calendar=state.owner;
    if(m_calendar)for(auto *entry:m_calendar->getClassificationsReference())if(!state.classes.contains(entry))m_retiredClasses.insert(entry);
    if(m_calendar)m_editor->getModel()->applyChange([&]{
        for(auto it=state.states.cbegin();it!=state.states.cend();++it){auto *e=it.key();const auto &v=it.value();e->setScheduledDate(v.date);e->setHill(v.hill);e->setRules(v.rules);e->setSerieType(v.type);e->setJumpsImportance(v.importance);e->setTrainings(v.trainings);e->setTrialRound(v.trial);e->setClassifications(v.classifications);e->setClassificationScoring(v.scoring);e->setAdvancementCompetition(v.advancement);e->setPendingAdvancementCompetition(v.pending);e->setAdvancementClassification(v.classification);}
        m_calendar->setName(state.name);m_calendar->setClassifications(state.classes);for(auto it=state.classStates.cbegin();it!=state.classStates.cend();++it)*it.key()=it.value();m_classId=state.classId;
        m_calendar->setCompetitions(state.order);m_calendar->normalizeCompetitionEvents();m_calendar->fixAdvancementCompetitions();m_calendar->updateCompetitionsQualifyingCompetitions();});
    m_selected=state.selected;m_live.clear();if(m_calendar)for(auto *e:m_calendar->getCompetitionsReference())m_live.insert(e);emit m_editor->changed();m_mutating=false;m_dirty=false;refresh();loadClass();collectRetired();
}
void QuickCalendar::undo() {
    commitPendingRules();
    sync();if(!canUndo()||!m_editor)return;auto &history=m_history;
    if(history.undo.last().owner==m_calendar && history.undo.last().boundary!=m_editor->getModel()->getDontModifyBefore()){clearUndo();fail(tr("Rozegrane wydarzenia są zablokowane."));return;}
    const auto state=history.undo.takeLast();history.redo.append(capture());m_message=tr("Cofnięto zmianę");restore(state);
}
void QuickCalendar::redo() {
    commitPendingRules();
    sync();if(!canRedo()||!m_editor)return;auto &history=m_history;
    if(history.redo.last().owner==m_calendar && history.redo.last().boundary!=m_editor->getModel()->getDontModifyBefore()){clearUndo();return;}
    const auto state=history.redo.takeLast();history.undo.append(capture());m_message=tr("Ponowiono zmianę");restore(state);
}
void QuickCalendar::moveBefore(const QString &sourceId,const QString &targetId,bool after) {
    sync();auto order=mains();CompetitionInfo *source=nullptr,*target=nullptr;for(auto *event:order){if(event->getIDStr()==sourceId)source=event;if(event->getIDStr()==targetId)target=event;}
    if(!source||!target||source==target||locked(source)||locked(target))return;
    if(!m_selected.contains(sourceId))m_selected=QStringList{sourceId};const auto items=selected();if(!editable()||items.contains(target))return;
    for(auto *event:items)order.removeAll(event);int at=order.indexOf(target)+(after?1:0);for(auto *event:items)order.insert(at++,event);
    if(!validateDateOrder(order))return;
    m_message=tr("Zmieniono kolejność konkursów");mutate([&]{setOrder(order);});
}
void QuickCalendar::resetCustomRules(){
    const auto items=selected();if(items.isEmpty()||!m_rulesEditor)return;
    auto *event=items.first();
    if(m_rulesTarget==1&&!event->getTrainings().isEmpty())event=event->getTrainings().first();
    else if(m_rulesTarget==2&&event->getTrialRound())event=event->getTrialRound();
    m_customRules=event->getRules();m_rulesEditor->fillCompetitionRulesInputs();m_rulesEditor->fillRoundsInputs(true);
}
void QuickCalendar::commitPendingRules(){if(m_rulesPending){m_rulesPending=false;applyCustomRules();}}
void QuickCalendar::applyCustomRules(){
    ++m_rulesRevision;m_rulesPending=false;
    sync();if(!editable()||!m_rulesEditor)return;
    const auto value=m_rulesEditor->getCompetitionRulesFromWidgetInputs();
    if(value.getRounds().isEmpty()){fail(tr("Dodaj co najmniej jedną serię."));return;}
    const auto items=selected();
    for(auto *e:items){
        if(m_rulesTarget==1&&e->getTrainings().isEmpty()){fail(tr("Najpierw dodaj treningi."));return;}
        if(m_rulesTarget==2&&!e->getTrialRound()){fail(tr("Najpierw dodaj serię próbną."));return;}
        if(m_rulesTarget==0&&referencedByHistory(e)&&e->getRules().getCompetitionType()!=value.getCompetitionType()){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    }
    bool differs=false;for(auto *event:items){if(m_rulesTarget==1){for(auto *training:event->getTrainings())if(ruleValue(training->getRules())!=ruleValue(value))differs=true;}else {auto *target=m_rulesTarget==2?event->getTrialRound():event;if(ruleValue(target->getRules())!=ruleValue(value))differs=true;}}
    if(!differs)return;
    m_message=tr("Zmiany zapisane w kalendarzu");
    mutate([&]{for(auto *e:items){
        if(m_rulesTarget==1)for(auto *training:e->getTrainings())training->setRules(value);
        else if(m_rulesTarget==2)e->getTrialRound()->setRules(value);
        else e->setRules(value);
    }});
}
void QuickCalendar::selectCalendar(int index) {
    commitPendingRules();
    if(m_classPending){m_classPending=false;applyClassification();}
    auto *list=calendarList();if(!list||index<0||index>=list->getCalendars()->size())return;
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner)){manager->setEditingCalendar(list->getCalendars()->at(index));break;}
    m_dirty=false;sync();
}
bool QuickCalendar::newCalendar() const {return m_calendar && m_calendar->getIDStr()==m_newCalendarId;}
void QuickCalendar::renameCalendar(const QString &name) {
    const bool pendingRename=newCalendar();m_newCalendarId.clear();
    if(!m_calendar||name.trimmed().isEmpty()||name.trimmed()==m_calendar->getName()){if(pendingRename)emit changed();return;}mutate([&]{m_calendar->setName(name.trimmed());});
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(owner->inherits("SimulationSaveManagerWindow"))if(auto *input=owner->findChild<QLineEdit *>("lineEdit_calendarName")){input->setText(name.trimmed());QMetaObject::invokeMethod(input,"textEdited",Q_ARG(QString,name.trimmed()));}
    if(auto *list=calendarList())emit list->getListModel()->dataChanged(list->getListModel()->index(0,0),list->getListModel()->index(list->getListModel()->rowCount()-1,0));emit changed();
}
void QuickCalendar::catalogAction(const QString &operation) {
    auto *list=calendarList();if(!list)return;
    if(operation=="remove"&&m_calendar)for(auto *e:m_calendar->getCompetitionsReference())if(e->getPlayed()||e->getCancelled()){fail(tr("Kalendarz zawiera rozegrane wydarzenia."));return;}
    const auto old=*list->getCalendars();
    const QString suffix=operation=="add"?"A":operation=="remove"?"D":operation=="up"?"Up":operation=="down"?"Down":QString();if(suffix.isEmpty())return;
    m_history.undo.append(capture());m_history.redo.clear();m_mutating=true;
    for(auto *action:list->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly))if(action->shortcut().toString(QKeySequence::PortableText).endsWith(suffix)){action->trigger();break;}
    if(operation=="add")for(auto *calendar:*list->getCalendars())if(!old.contains(calendar)){m_newCalendarId=calendar->getIDStr();calendar->setName(tr("Nowy kalendarz %1").arg(list->getCalendars()->size()));selectCalendar(list->getCalendars()->indexOf(calendar));m_message=tr("Utworzono kalendarz. Dodaj konkursy lub wczytaj preset.");break;}
    for(auto *calendar:old)if(!list->getCalendars()->contains(calendar)){for(auto *event:calendar->getCompetitionsReference())m_retired.insert(event);for(auto *entry:calendar->getClassificationsReference())m_retiredClasses.insert(entry);}
    m_dirty=true;sync();m_mutating=false;
}
void QuickCalendar::preset(bool save) {
    if(!m_editor || !m_calendar)return;
    if(!save && (!m_editor->getSave() || GlobalDatabase::get()->getEditableGlobalCalendarPresets().isEmpty())){fail(tr("Brak presetów kalendarza do wczytania."));return;}
    const auto before=capture();m_mutating=true;m_editor->setProperty("quickRetainPresetEvents",true);
    if(auto *button=m_editor->findChild<QPushButton *>(save?"pushButton_saveCalendarPreset":"pushButton_loadFromPreset"))button->click();
    m_editor->setProperty("quickRetainPresetEvents",false);
    if(!save && before.order!=m_calendar->getCompetitionsReference()) {
        m_history.undo.append(before);m_history.redo.clear();for(auto *event:before.order)m_retired.insert(event);for(auto *entry:before.classes)m_retiredClasses.insert(entry);m_selected.clear();m_selection.clear();m_classId.clear();
    }
    m_dirty=false;m_live.clear();for(auto *event:m_calendar->getCompetitionsReference())m_live.insert(event);m_mutating=false;refresh();loadClass();
}

Classification *QuickCalendar::selectedClass() const {
    if(m_calendar)for(auto *entry:m_calendar->getClassificationsReference())if(entry->getIDStr()==m_classId)return entry;return nullptr;
}
bool QuickCalendar::classLocked() const {
    auto *entry=selectedClass();if(!entry)return true;if(!entry->getResults().isEmpty())return true;
    for(auto *event:m_calendar->getCompetitionsReference())if(event->getPlayed() && (event->getClassificationsReference().contains(entry)||event->getAdvancementClassification()==entry))return true;return false;
}
QVariantList QuickCalendar::classEntries() const {
    QVariantList result;if(m_calendar)for(auto *entry:m_calendar->getClassificationsReference())result.append(QVariantMap{{"id",entry->getIDStr()},{"name",entry->getName()},{"team",entry->getClassificationType()==Classification::Team},{"selected",entry->getIDStr()==m_classId}});return result;
}
void QuickCalendar::loadClass() {
    if(m_save && m_save->getActualSeason())m_save->getActualSeason()->synchronizeClassificationYears();
    m_loadingClass=true;auto *entry=selectedClass();
    if(!entry && m_calendar && !m_calendar->getClassificationsReference().isEmpty()){entry=m_calendar->getClassificationsReference().first();m_classId=entry->getIDStr();}
    m_classEditor->setClassification(entry);
    if(entry){m_classEditor->resetInputs();m_classEditor->fillInputs();}
    const bool locked=classLocked();m_classEditor->setProperty("quickScoringLocked",locked);for(auto *choice:m_classEditor->findChildren<QComboBox *>())choice->setEnabled(!locked || choice->objectName()=="classificationScoringMethod");for(auto *table:m_classEditor->findChildren<QTableView *>())table->setEnabled(!locked);
    for(auto *input:m_classEditor->findChildren<QSpinBox *>())input->setEnabled(!locked);
    for(auto *button:m_classEditor->findChildren<QAbstractButton *>())button->setEnabled(!locked || button->objectName()=="classificationAlternativeHelp" || button->objectName()=="classificationAdvanced");
    if(auto *name=m_classEditor->findChild<QLineEdit *>("classificationMethodName"))name->setEnabled(!locked);
    if(auto *remove=m_classEditor->findChild<QPushButton *>("classificationRemoveMethod"))remove->setEnabled(!locked && m_classEditor->findChild<QComboBox *>("classificationScoringMethod")->currentIndex()>0);
    for(auto *action:m_classEditor->actions())action->setEnabled(!locked);
    m_loadingClass=false;emit changed();
}
void QuickCalendar::selectClass(const QString &id) {if(id==m_classId)return;if(m_classPending){m_classPending=false;applyClassification();}m_classId=id;loadClass();}
void QuickCalendar::applyClassification() {
    if(m_loadingClass||!selectedClass())return;
    auto *entry=selectedClass();std::unique_ptr<Classification> input(m_classEditor->getClassificationFromInputs());const auto name=input->getName().trimmed();if(name.isEmpty()){loadClass();return;}
    const bool locked=classLocked();
    if(name==entry->getName() && (locked || (input->getClassificationType()==entry->getClassificationType() && input->getPunctationType()==entry->getPunctationType() && input->getPointsForPlaces()==entry->getPointsForPlaces() && input->getAltPointsForPlaces()==entry->getAltPointsForPlaces() && input->getOptions().toJson()==entry->getOptions().toJson())))return;
    mutate([&]{entry->setName(name);if(!locked){entry->copyConfiguration(*input);}m_calendar->fixCompetitionsClassifications();});
}
void QuickCalendar::classAction(const QString &action) {
    sync();if(!m_calendar)return;auto *entry=selectedClass();
    if(action=="remove") {
        if(!entry||classLocked()){fail(tr("Klasyfikacja ma rozegrane wyniki. Możesz zmienić nazwę lub utworzyć kopię."));return;}
        QStringList used;for(auto *event:mains())if(event->getClassificationsReference().contains(entry)||event->getAdvancementClassification()==entry)used.append(QString::number(mains().indexOf(event)+1));
        if(!used.isEmpty()){fail(tr("Najpierw usuń powiązania klasyfikacji w konkursach: %1.").arg(used.join(", ")));return;}
    }
    if(action!="add" && action!="copy" && action!="remove")return;if(action=="copy"&&!entry)return;
    mutate([&]{if(action=="remove"){m_calendar->getClassificationsReference().removeAll(entry);m_retiredClasses.insert(entry);m_classId.clear();}
        else {auto *next=action=="copy"?new Classification(*entry):new Classification(tr("Nowa klasyfikacja"));next->reassign();next->setResults({});if(action=="copy")next->setName(entry->getName()+tr(" — kopia"));m_calendar->getClassificationsReference().append(next);m_classId=next->getIDStr();}});loadClass();
}
