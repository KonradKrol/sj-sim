#include "QuickCalendar.h"
#include "../Seasons/CalendarEditor/CalendarEditorWidget.h"
#include "../Seasons/SimulationSaveManagerWindow.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../DatabaseEditor/DatabaseItemsListView.h"
#include "../../global/GlobalDatabase.h"
#include <QItemSelectionModel>
#include <QListView>
#include <QPushButton>
#include <QLineEdit>
#include <QAction>
#include <QJsonDocument>
#include <algorithm>

namespace {
bool mainEvent(CompetitionInfo *event) { return event && event->getSerieType()<=CompetitionInfo::Qualifications; }
QJsonObject ruleValue(const CompetitionRules &rules) { auto value=CompetitionRules::getJsonObject(rules);value.remove("id");return value; }
int presetIndex(const CompetitionRules &value,const QVector<CompetitionRules> &rules) {
    for(int i=0;i<rules.size();++i)if(ruleValue(value)==ruleValue(rules[i]))return i+1;
    return 0;
}
}
QuickCalendar::QuickCalendar(CalendarEditorWidget *editor,QObject *parent):QObject(parent),m_editor(editor) {
    m_rulesEditor=new CompetitionRulesEditorWidget(editor);m_rulesEditor->setObjectName("quickCalendarRules");
    m_rulesEditor->setProperty("quickExposePage",true);m_rulesEditor->removeSubmitButton();m_rulesEditor->hide();
    m_rulesEditor->setCompetitionRules(&m_customRules);
    const auto dirty=[this]{if(!m_mutating){m_dirty=true;}};
    connect(editor,&CalendarEditorWidget::changed,this,dirty);
    connect(editor,&CalendarEditorWidget::calendarPresetLoaded,this,dirty);
    connect(editor->getModel(),&QAbstractItemModel::modelReset,this,dirty);
    connect(editor->getModel(),&QAbstractItemModel::dataChanged,this,dirty);
    sync();
}
QuickCalendar::~QuickCalendar(){clearUndo();}
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
QStringList QuickCalendar::hills() const { QStringList result;if(m_editor)for(auto *hill:*m_editor->getModel()->getHillsList())result.append(hill->getName()+" HS"+QString::number(hill->getHSPoint()));return result; }
QStringList QuickCalendar::rules() const { QStringList result{tr("Zasady własne")};if(m_editor)for(const auto &rules:*m_editor->getModel()->getRulesList())result.append(rules.getName());return result; }
QStringList QuickCalendar::auxiliaryRules() const { auto result=rules();result.prepend(tr("Takie jak konkurs"));return result; }
void QuickCalendar::setRulesTarget(int target) { if(target<0||target>2||target==m_rulesTarget)return;m_rulesTarget=target;resetCustomRules();emit changed(); }
QStringList QuickCalendar::advancementClassifications() const { QStringList result{tr("Brak")};if(m_calendar)for(auto *item:m_calendar->getClassificationsReference())result.append(item->getName());return result; }
DatabaseItemsListView *QuickCalendar::calendarList() const {
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(owner))return manager->getCalendarsListView();
    return nullptr;
}
QStringList QuickCalendar::calendars() const { QStringList result;if(auto *list=calendarList())for(auto *calendar:*list->getCalendars())result.append(calendar->getName());return result; }
int QuickCalendar::calendarIndex() const { auto *list=calendarList();return list?list->getCalendars()->indexOf(m_calendar):-1; }
QString QuickCalendar::calendarName() const { return m_calendar?m_calendar->getName():QString(); }
void QuickCalendar::clearUndo() {
    m_undo.reset();
    for(auto *event:m_retired)if(!m_live.contains(event))delete event;
    m_retired.clear();
}
void QuickCalendar::sync() {
    if(!m_editor)return;
    auto *calendar=m_editor->getModel()->getCalendar();
    if(calendar==m_calendar && !m_dirty && m_language==tr("Konkurs"))return;
    clearUndo();if(m_calendar!=calendar){m_selected.clear();m_selection.clear();m_message.clear();}m_calendar=calendar;m_dirty=false;m_language=tr("Konkurs");
    m_live.clear();if(m_calendar)for(auto *event:m_calendar->getCompetitionsReference())m_live.insert(event);
    refresh();resetCustomRules();
}
void QuickCalendar::refresh() {
    const auto all=mains();QStringList existing;for(auto *event:all)existing.append(event->getIDStr());
    for(int i=m_selected.size()-1;i>=0;--i)if(!existing.contains(m_selected[i]))m_selected.removeAt(i);
    if(m_selected.isEmpty() && !all.isEmpty() && m_selection.isEmpty()){auto *first=all.first();for(auto *event:all)if(!locked(event)){first=event;break;}m_selected.append(first->getIDStr());}
    const auto chosen=selected();m_events.clear();
    for(int i=0;i<all.size();++i){auto *event=all[i];const auto hill=event->getHill();
        m_events.append(QVariantMap{{"id",event->getIDStr()},{"number",i+1},{"title",hill?hill->getName()+" HS"+QString::number(hill->getHSPoint()):tr("Brak skoczni")},
            {"type",event->getSerieType()==CompetitionInfo::Qualifications?tr("Kwalifikacje"):tr("Konkurs")},{"team",event->getRules().getCompetitionType()==CompetitionRules::Team},
            {"trainings",event->getTrainingsReference().size()},{"trial",event->getTrialRound()!=nullptr},{"locked",locked(event)},{"played",event->getPlayed()},{"cancelled",event->getCancelled()},{"selected",m_selected.contains(event->getIDStr())}});
    }
    m_selection={{"count",chosen.size()},{"locked",!editable()},{"title",chosen.size()==1?m_events.value(all.indexOf(chosen.first())).toMap().value("title"):tr("%1 konkursów").arg(chosen.size())}};
    auto common=[&](const QString &key,const std::function<QVariant(CompetitionInfo *)> &read){
        if(chosen.isEmpty())return;QVariant value=read(chosen.first());for(auto *event:chosen)if(read(event)!=value){value=-1;break;}m_selection[key]=value;
    };
    if(m_editor){common("hill",[&](auto *e){return m_editor->getModel()->getHillsList()->indexOf(e->getHill());});common("rules",[&](auto *e){return presetIndex(e->getRules(),*m_editor->getModel()->getRulesList());});}
    common("kind",[](auto *e){return e->getRules().getCompetitionType();});common("type",[](auto *e){return e->getSerieType();});
    common("trainings",[](auto *e){return e->getTrainingsReference().size();});common("trial",[](auto *e){return int(e->getTrialRound()!=nullptr);});common("importance",[](auto *e){return e->getJumpsImportance();});
    common("trainingImportance",[](auto *e){return e->getTrainings().isEmpty()?5:e->getTrainings().first()->getJumpsImportance();});
    common("trialImportance",[](auto *e){return e->getTrialRound()?e->getTrialRound()->getJumpsImportance():5;});
    common("trainingRules",[&](auto *e){return e->getTrainings().isEmpty() || ruleValue(e->getTrainings().first()->getRules())==ruleValue(e->getRules())?0:1+presetIndex(e->getTrainings().first()->getRules(),*m_editor->getModel()->getRulesList());});
    common("trialRules",[&](auto *e){return !e->getTrialRound() || ruleValue(e->getTrialRound()->getRules())==ruleValue(e->getRules())?0:1+presetIndex(e->getTrialRound()->getRules(),*m_editor->getModel()->getRulesList());});
    m_classifications.clear();if(m_calendar)for(int i=0;i<m_calendar->getClassificationsReference().size();++i){auto *cls=m_calendar->getClassificationsReference()[i];int count=0;for(auto *event:chosen)if(event->getClassificationsReference().contains(cls))++count;
        m_classifications.append(QVariantMap{{"name",cls->getName()},{"index",i},{"checked",!chosen.isEmpty()&&count==chosen.size()},{"mixed",count>0&&count<chosen.size()}});
    }
    m_advancementOptions={nullptr};m_advancementLabels=QStringList{tr("Brak")};
    for(auto *target:all){bool allowed=!chosen.isEmpty();for(auto *event:chosen)if(all.indexOf(target)<=all.indexOf(event)||target->getRules().getCompetitionType()!=event->getRules().getCompetitionType())allowed=false;
        if(allowed){m_advancementOptions.append(target);m_advancementLabels.append(QString::number(all.indexOf(target)+1)+". "+(target->getHill()?target->getHill()->getName():tr("Brak skoczni")));}}
    common("advancement",[&](auto *e){return m_advancementOptions.indexOf(e->getAdvancementCompetition());});
    common("advancementClass",[&](auto *e){return m_calendar->getClassificationsReference().indexOf(e->getAdvancementClassification())+1;});
    emit changed();
}
void QuickCalendar::select(const QString &id,int modifiers) {
    sync();const auto all=mains();int row=-1;for(int i=0;i<all.size();++i)if(all[i]->getIDStr()==id){row=i;break;}if(row<0)return;
    if(modifiers & Qt::ShiftModifier && !m_selected.isEmpty()){
        int anchor=0;for(int i=0;i<all.size();++i)if(all[i]->getIDStr()==m_selected.first())anchor=i;
        m_selected.clear();for(int i=qMin(anchor,row);i<=qMax(anchor,row);++i)m_selected.append(all[i]->getIDStr());
    }else if(modifiers & Qt::ControlModifier){if(m_selected.contains(id))m_selected.removeAll(id);else m_selected.append(id);}else m_selected=QStringList{id};
    if(m_editor->getTableView()->selectionModel())m_editor->getTableView()->selectionModel()->setCurrentIndex(m_editor->getModel()->index(row,0),QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
    m_message.clear();refresh();resetCustomRules();
}
void QuickCalendar::clearSelection() {sync();m_selected.clear();refresh();resetCustomRules();}
void QuickCalendar::selectAll(bool futureOnly) { sync();m_selected.clear();for(auto *event:mains())if(!futureOnly||!locked(event))m_selected.append(event->getIDStr());refresh();resetCustomRules(); }
QuickCalendar::Snapshot QuickCalendar::capture() const {
    Snapshot value;value.selected=m_selected;value.boundary=m_editor?m_editor->getModel()->getDontModifyBefore():0;if(!m_calendar)return value;value.order=m_calendar->getCompetitionsReference();
    for(auto *e:value.order){State s;s.hill=e->getHill();s.rules=e->getRules();s.type=e->getSerieType();s.importance=e->getJumpsImportance();s.trainings=e->getTrainings();s.trial=e->getTrialRound();s.advancement=e->getAdvancementCompetition();s.classification=e->getAdvancementClassification();s.classifications=e->getClassifications();value.states.insert(e,s);}return value;
}
void QuickCalendar::mutate(const std::function<void()> &operation) {
    if(!m_editor||!m_calendar)return;
    clearUndo();m_undo=std::make_unique<Snapshot>(capture());m_mutating=true;
    m_editor->getModel()->applyChange([&]{operation();m_calendar->normalizeCompetitionEvents();m_calendar->fixAdvancementCompetitions();m_calendar->fixAdvancementClassifications();m_calendar->updateCompetitionsQualifyingCompetitions();});
    const QSet<CompetitionInfo *> before=m_live;m_live.clear();for(auto *event:m_calendar->getCompetitionsReference())m_live.insert(event);for(auto *event:before)if(!m_live.contains(event))m_retired.insert(event);
    emit m_editor->changed();m_mutating=false;m_dirty=false;refresh();resetCustomRules();
}
void QuickCalendar::fail(const QString &message){m_message=message;emit changed();}
void QuickCalendar::update(const QVariantMap &fields) {
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
        if(it.key()=="advancementClass"&&(value<0||value>m_calendar->getClassificationsReference().size()))return;
        if((it.key()=="importance"||it.key()=="trainingImportance"||it.key()=="trialImportance")&&(!qIsFinite(it.value().toDouble())||it.value().toDouble()<0||it.value().toDouble()>1000))return;
    }
    if(fields.contains("kind"))for(auto *event:items)if(referencedByHistory(event)&&fields.value("kind").toInt()!=event->getRules().getCompetitionType()){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    if(fields.contains("rules") && fields.value("rules").toInt()>0)for(auto *event:items)if(referencedByHistory(event)&&rules->at(fields.value("rules").toInt()-1).getCompetitionType()!=event->getRules().getCompetitionType()){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    const auto advancement=m_advancementOptions;
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
        if(fields.contains("advancementClass")){const int n=fields.value("advancementClass").toInt();e->setAdvancementClassification(n?m_calendar->getClassificationsReference()[n-1]:nullptr);if(n)e->setAdvancementCompetition(nullptr);}
    }});
}
void QuickCalendar::setClassification(int index,bool checked) {
    sync();if(!editable()||index<0||index>=m_calendar->getClassificationsReference().size())return;auto *cls=m_calendar->getClassificationsReference()[index];const auto items=selected();
    m_message=tr("Zmiany zapisane w kalendarzu");mutate([&]{for(auto *e:items){if(checked&&!e->getClassificationsReference().contains(cls))e->getClassificationsReference().append(cls);else if(!checked)e->getClassificationsReference().removeAll(cls);}});
}
void QuickCalendar::setOrder(const QVector<CompetitionInfo *> &events) {
    QVector<CompetitionInfo *> order;for(auto *e:events){order+=e->getTrainings();if(e->getTrialRound())order.append(e->getTrialRound());order.append(e);}
    for(auto *e:m_calendar->getCompetitionsReference())if(!mainEvent(e)&&!order.contains(e)&&!m_retired.contains(e)){
        bool owned=false;for(auto *old:mains())if(old->getTrainings().contains(e)||old->getTrialRound()==e)owned=true;if(!owned)order.append(e);
    }
    m_calendar->setCompetitions(order);
}
CompetitionInfo *QuickCalendar::clone(CompetitionInfo *e) {
    auto *copy=new CompetitionInfo(e->getHill());copy->setRules(e->getRules());copy->setSerieType(e->getSerieType());copy->setJumpsImportance(e->getJumpsImportance());copy->setClassifications(e->getClassifications());copy->setAdvancementCompetition(e->getAdvancementCompetition());copy->setAdvancementClassification(e->getAdvancementClassification());
    for(auto *t:e->getTrainings()){auto *next=new CompetitionInfo(t->getHill());next->setRules(t->getRules());next->setSerieType(CompetitionInfo::Training);copy->getTrainingsReference().append(next);}if(e->getTrialRound()){auto *next=new CompetitionInfo(e->getHill());next->setRules(e->getTrialRound()->getRules());next->setSerieType(CompetitionInfo::TrialRound);copy->setTrialRound(next);}return copy;
}
void QuickCalendar::add() {
    sync();if(!m_calendar||hills().isEmpty()){fail(tr("Najpierw dodaj skocznię do zapisu."));return;}
    const auto items=selected();m_message=tr("Dodano konkurs");mutate([&]{auto order=mains();int at=order.size();if(!items.isEmpty()&&!locked(items.last()))at=order.indexOf(items.last())+1;
        auto *event=new CompetitionInfo(m_editor->getModel()->getHillsList()->first());if(!m_editor->getModel()->getRulesList()->isEmpty())event->setRules(m_editor->getModel()->getRulesList()->first());order.insert(at,event);m_selected=QStringList{event->getIDStr()};setOrder(order);});
}
void QuickCalendar::duplicate() {
    sync();const auto items=selected();if(items.isEmpty())return;m_message=tr("Skopiowano konkursy");mutate([&]{auto order=mains();int at=locked(items.last())?order.size():order.indexOf(items.last())+1;m_selected.clear();for(auto *event:items){auto *copy=clone(event);order.insert(at++,copy);m_selected.append(copy->getIDStr());}setOrder(order);});
}
void QuickCalendar::remove() {
    sync();if(!editable())return;const auto items=selected();for(auto *event:items)if(referencedByHistory(event)){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    m_message=tr("Usunięto konkursy. Możesz cofnąć zmianę.");mutate([&]{auto order=mains();for(auto *event:items){order.removeAll(event);m_retired.insert(event);for(auto *t:event->getTrainings())m_retired.insert(t);if(event->getTrialRound())m_retired.insert(event->getTrialRound());}setOrder(order);m_selected.clear();});
}
void QuickCalendar::move(int direction) {
    sync();if(!editable()||(direction!=-1&&direction!=1))return;const auto items=selected();auto order=mains();bool moved=false;
    if(direction<0){for(int i=1;i<order.size();++i)if(items.contains(order[i])&&!items.contains(order[i-1])&&!locked(order[i-1])){qSwap(order[i],order[i-1]);moved=true;}}
    else for(int i=order.size()-2;i>=0;--i)if(items.contains(order[i])&&!items.contains(order[i+1])&&!locked(order[i+1])){qSwap(order[i],order[i+1]);moved=true;}
    if(moved){
        for(auto *event:order)if(event->getAdvancementCompetition() && order.indexOf(event->getAdvancementCompetition())<=order.indexOf(event)) {
            fail(tr("Kwalifikacje muszą poprzedzać konkurs, do którego prowadzą."));return;
        }
        m_message=tr("Zmieniono kolejność konkursów");mutate([&]{setOrder(order);});
    }
}
void QuickCalendar::undo() {
    sync();
    if(!m_undo||!m_calendar||!m_editor)return;
    if(m_undo->boundary!=m_editor->getModel()->getDontModifyBefore()){clearUndo();fail(tr("Rozegrane wydarzenia są zablokowane."));return;}
    const auto state=*m_undo;m_mutating=true;
    m_editor->getModel()->applyChange([&]{for(auto *e:m_calendar->getCompetitionsReference())if(!state.order.contains(e))m_retired.insert(e);
        for(auto it=state.states.cbegin();it!=state.states.cend();++it){auto *e=it.key();const auto &s=it.value();e->setHill(s.hill);e->setRules(s.rules);e->setSerieType(s.type);e->setJumpsImportance(s.importance);e->setTrainings(s.trainings);e->setTrialRound(s.trial);e->setClassifications(s.classifications);e->setAdvancementCompetition(s.advancement);e->setAdvancementClassification(s.classification);}
        m_calendar->setCompetitions(state.order);m_calendar->normalizeCompetitionEvents();m_calendar->updateCompetitionsQualifyingCompetitions();});
    m_selected=state.selected;m_live.clear();for(auto *e:m_calendar->getCompetitionsReference())m_live.insert(e);clearUndo();emit m_editor->changed();m_mutating=false;m_dirty=false;m_message=tr("Cofnięto zmianę");refresh();resetCustomRules();
}
void QuickCalendar::resetCustomRules(){
    const auto items=selected();if(items.isEmpty()||!m_rulesEditor)return;
    auto *event=items.first();
    if(m_rulesTarget==1&&!event->getTrainings().isEmpty())event=event->getTrainings().first();
    else if(m_rulesTarget==2&&event->getTrialRound())event=event->getTrialRound();
    m_customRules=event->getRules();m_rulesEditor->fillCompetitionRulesInputs();m_rulesEditor->fillRoundsInputs(true);
}
void QuickCalendar::applyCustomRules(){
    sync();if(!editable()||!m_rulesEditor)return;
    const auto value=m_rulesEditor->getCompetitionRulesFromWidgetInputs();
    if(value.getRounds().isEmpty()){fail(tr("Dodaj co najmniej jedną serię."));return;}
    const auto items=selected();
    for(auto *e:items){
        if(m_rulesTarget==1&&e->getTrainings().isEmpty()){fail(tr("Najpierw dodaj treningi."));return;}
        if(m_rulesTarget==2&&!e->getTrialRound()){fail(tr("Najpierw dodaj serię próbną."));return;}
        if(m_rulesTarget==0&&referencedByHistory(e)&&e->getRules().getCompetitionType()!=value.getCompetitionType()){fail(tr("Konkurs jest powiązany z rozegranymi kwalifikacjami."));return;}
    }
    m_message=tr("Zmiany zapisane w kalendarzu");
    mutate([&]{for(auto *e:items){
        if(m_rulesTarget==1)for(auto *training:e->getTrainings())training->setRules(value);
        else if(m_rulesTarget==2)e->getTrialRound()->setRules(value);
        else e->setRules(value);
    }});
}
void QuickCalendar::selectCalendar(int index) {
    auto *list=calendarList();if(!list||index<0||index>=list->getListModel()->rowCount())return;clearUndo();auto *view=list->getListView();const auto row=list->getListModel()->index(index,0);view->selectionModel()->setCurrentIndex(row,QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);QMetaObject::invokeMethod(view,"clicked",Q_ARG(QModelIndex,row));m_dirty=true;sync();
}
void QuickCalendar::renameCalendar(const QString &name) {
    if(!m_calendar||name.trimmed().isEmpty())return;m_calendar->setName(name.trimmed());
    for(QWidget *owner=m_editor.data();owner;owner=owner->parentWidget())if(owner->inherits("SimulationSaveManagerWindow"))if(auto *input=owner->findChild<QLineEdit *>("lineEdit_calendarName")){input->setText(name.trimmed());QMetaObject::invokeMethod(input,"textEdited",Q_ARG(QString,name.trimmed()));}
    if(auto *list=calendarList())emit list->getListModel()->dataChanged(list->getListModel()->index(0,0),list->getListModel()->index(list->getListModel()->rowCount()-1,0));emit changed();
}
void QuickCalendar::catalogAction(const QString &operation) {
    auto *list=calendarList();if(!list)return;
    if(operation=="remove"&&m_calendar)for(auto *e:m_calendar->getCompetitionsReference())if(e->getPlayed()||e->getCancelled()){fail(tr("Kalendarz zawiera rozegrane wydarzenia."));return;}
    const auto old=*list->getCalendars();clearUndo();
    const QString suffix=operation=="add"?"A":operation=="remove"?"D":operation=="up"?"Up":operation=="down"?"Down":QString();if(suffix.isEmpty())return;
    for(auto *action:list->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly))if(action->shortcut().toString(QKeySequence::PortableText).endsWith(suffix)){action->trigger();break;}
    if(operation=="add")for(auto *calendar:*list->getCalendars())if(!old.contains(calendar)){selectCalendar(list->getCalendars()->indexOf(calendar));break;}
    m_dirty=true;sync();
}
void QuickCalendar::preset(bool save) {
    if(!m_editor || !m_calendar)return;
    if(!save && (!m_editor->getSave() || GlobalDatabase::get()->getEditableGlobalCalendarPresets().isEmpty())){fail(tr("Brak presetów kalendarza do wczytania."));return;}
    clearUndo();if(auto *button=m_editor->findChild<QPushButton *>(save?"pushButton_saveCalendarPreset":"pushButton_loadFromPreset"))button->click();m_dirty=true;sync();
}
