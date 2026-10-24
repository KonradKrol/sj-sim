#include "QuickRoster.h"
#include "../Competition/CompetitionConfigWindow.h"
#include "../EditorWidgets/JumpersListEditorWindow.h"
#include "../EditorWidgets/TeamsEditing/TeamEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/CountryCodes.h"
#include <QDir>
#include <QFile>
#include <QUrl>
#include <algorithm>
#include <random>

namespace {
const QList<QByteArray> roles{"athleteId","athleteName","countryCode","included","marked","position","activeSlot","flagSource"};
QVector<Jumper *> unique(const QVector<Jumper *> &input) { QVector<Jumper *> out;QSet<Jumper *> seen;for(auto *j:input)if(j&&!seen.contains(j)){seen.insert(j);out.append(j);}return out; }

QString flagSource(const QString &country) {
    const auto file=QDir::current().absoluteFilePath("flags/"+CountryCodes::toIso(country)+".svg");
    static QHash<QString,QString> cache;
    if(!cache.contains(file))cache.insert(file,QFile::exists(file)?QUrl::fromLocalFile(file).toString():QString());
    return cache.value(file);
}

bool matches(Jumper *jumper,const QString &query) { const auto words=query.simplified().split(' ',Qt::SkipEmptyParts);const auto text=jumper->getNameAndSurname()+" "+jumper->getCountryCode();for(const auto &word:words)if(!text.contains(word,Qt::CaseInsensitive))return false;return true; }
}
QHash<int,QByteArray> QuickRosterRows::roleNames() const { QHash<int,QByteArray> out;for(int i=0;i<roles.size();++i)out[Qt::UserRole+1+i]=roles[i];return out; }
QVariant QuickRosterRows::data(const QModelIndex &index,int role) const { if(!index.isValid()||index.row()<0||index.row()>=m_rows.size())return {};const int column=role-Qt::UserRole-1;return column>=0&&column<roles.size()?m_rows[index.row()].toMap().value(roles[column]):QVariant(); }
void QuickRosterRows::sync(const QVariantList &rows) {
    QSet<QString> ids;for(const auto &row:rows)ids.insert(row.toMap().value("athleteId").toString());
    for(int i=m_rows.size()-1;i>=0;--i)if(!ids.contains(m_rows[i].toMap().value("athleteId").toString())){beginRemoveRows({},i,i);m_rows.removeAt(i);endRemoveRows();}
    for(int i=0;i<rows.size();++i){const auto id=rows[i].toMap().value("athleteId");int at=i;while(at<m_rows.size()&&m_rows[at].toMap().value("athleteId")!=id)++at;
        if(at==m_rows.size()){beginInsertRows({},i,i);m_rows.insert(i,rows[i]);endInsertRows();}
        else if(at!=i){beginMoveRows({},at,at,{},i);m_rows.move(at,i);endMoveRows();}
        if(m_rows[i]!=rows[i]){m_rows[i]=rows[i];emit dataChanged(index(i),index(i));}
    }
    emit countChanged();
}
QString QuickRoster::key(Jumper *jumper) { return QString::number(reinterpret_cast<quintptr>(jumper),16); }
QuickRoster::QuickRoster(QWidget *source,QObject *parent):QObject(parent),m_source(source),m_library(new QuickRosterRows(this)),m_ordered(new QuickRosterRows(this)){sync();}
bool QuickRoster::membership() const { return !qobject_cast<TeamEditorWidget *>(m_source); }
bool QuickRoster::active() const {
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source))return (config->seasonCompetition?config->seasonCompetition->getRules().getCompetitionType():config->competitionRulesEditor->getCompetitionTypeFromInput())==CompetitionRules::Individual;
    return true;
}
int QuickRoster::activeLimit() const { auto *team=qobject_cast<TeamEditorWidget *>(m_source);return team?team->getJumpersCount():0; }
QVector<Jumper *> QuickRoster::read() const {
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source))return config->type==CompetitionConfigWindow::SingleCompetition?config->competitionJumpers:config->seasonCompetitionJumpers;
    if(auto *list=qobject_cast<JumpersListEditorWindow *>(m_source))return list->getSelectedJumpers();
    if(auto *team=qobject_cast<TeamEditorWidget *>(m_source))return team->getJumpers();
    return {};
}
QVector<Jumper *> QuickRoster::candidates() const {
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source)){
        if(config->type==CompetitionConfigWindow::SingleCompetition){QVector<Jumper *> out;for(auto &j:GlobalDatabase::get()->getEditableGlobalJumpers())out.append(&j);return out;}
        if(!config->simulationSave||!config->seasonCompetition)return {};
        auto pool=config->simulationSave->getJumpers();auto *event=config->seasonCompetition;
        for(auto *main:config->simulationSave->getActualSeason()->getActualCalendar()->getCompetitionsReference())if(main->getTrialRound()==event){event=main;break;}
        if(event->getAdvancementCompetition()&&!event->getAdvancementCompetition()->getCancelled())pool=IndividualCompetitionManager::getFilteredJumpersAfterQualifications(event,pool);
        else if(event->getAdvancementClassification())pool=IndividualCompetitionManager::getFilteredJumpersByClassification(event,event->getAdvancementClassification(),pool);
        return pool;
    }
    if(auto *list=qobject_cast<JumpersListEditorWindow *>(m_source))return list->getAllJumpers();
    return read();
}
void QuickRoster::sync(){
    const auto current=unique(read());auto pool=unique(candidates());
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);config&&config->type==CompetitionConfigWindow::SingleCompetition)pool=unique(pool+m_pool+current);
    else pool=unique(pool+current);
    if(current==m_current&&pool==m_pool&&!m_countryCodes.isEmpty()&&m_presetNames==presets())return;
    m_presetNames=presets();
    if(current!=m_current){m_hasUndo=false;m_undo.clear();}m_current=current;m_pool=pool;
    m_countryCodes.clear();for(auto *j:m_pool)if(!m_countryCodes.contains(j->getCountryCode()))m_countryCodes.append(j->getCountryCode());m_countryCodes.sort();m_country=qBound(0,m_country,m_countryCodes.size());refresh();
}
QStringList QuickRoster::countries() const { auto out=m_countryCodes;out.prepend(tr("Wszystkie kraje"));return out; }
QStringList QuickRoster::presets() const { QStringList out;if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);config&&config->simulationSave)for(const auto &list:config->simulationSave->getJumpersLists())out.append(list.getName());return out; }
QVector<Jumper *> QuickRoster::visiblePool() const { QVector<Jumper *> out;for(auto *j:m_pool)if(matches(j,m_query)&&(!m_country||j->getCountryCode()==m_countryCodes.value(m_country-1)))out.append(j);return out; }
void QuickRoster::refresh(){
    QSet<QString> existing;for(auto *j:m_current)existing.insert(key(j));m_marked.intersect(existing);
    QVariantList library,ordered;for(auto *j:visiblePool())library.append(QVariantMap{{"athleteId",key(j)},{"athleteName",j->getNameAndSurname()},{"countryCode",j->getCountryCode()},{"flagSource",flagSource(j->getCountryCode())},{"included",m_current.contains(j)}});
    for(int i=0;i<m_current.size();++i){auto *j=m_current[i];if(!matches(j,m_orderQuery))continue;ordered.append(QVariantMap{{"athleteId",key(j)},{"athleteName",j->getNameAndSurname()},{"countryCode",j->getCountryCode()},{"flagSource",flagSource(j->getCountryCode())},{"included",true},{"marked",m_marked.contains(key(j))},{"position",i+1},{"activeSlot",!activeLimit()||i<activeLimit()}});}
    m_library->sync(library);m_ordered->sync(ordered);emit changed();
}
void QuickRoster::setQuery(const QString &value){if(m_query==value)return;m_query=value;refresh();}
void QuickRoster::setOrderQuery(const QString &value){if(m_orderQuery==value)return;m_orderQuery=value;refresh();}
void QuickRoster::setCountry(int value){if(value<0||value>m_countryCodes.size()||value==m_country)return;m_country=value;refresh();}
Jumper *QuickRoster::jumper(const QString &id) const { for(auto *j:m_pool)if(key(j)==id)return j;return nullptr; }
void QuickRoster::write(const QVector<Jumper *> &value){
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source)){
        if(config->type==CompetitionConfigWindow::SingleCompetition)config->competitionJumpers=value;else config->seasonCompetitionJumpers=value;
        config->jumpersListView->setupListModel();
        if(config->KOGroupsList&&!config->comboBox_groupsSelectionType->isHidden()&&config->comboBox_groupsSelectionType->currentIndex()>0){
            auto &groups=config->type==CompetitionConfigWindow::SingleCompetition?config->competitionGroups:config->seasonCompetitionGroups;
            const auto rules=config->seasonCompetition?config->seasonCompetition->getRules():config->competitionRulesEditor->getCompetitionRulesFromWidgetInputs();
            if(!rules.getRounds().isEmpty()){auto round=rules.getRounds().first();auto jumpers=value;groups=KOGroup::constructKOGroups(&round,&jumpers,config->comboBox_groupsSelectionType->currentIndex()-1,config->seasonCompetition);config->KOGroupsList->setKOGroups(&groups);config->KOGroupsList->fillListLayout();}
        }
    }else if(auto *list=qobject_cast<JumpersListEditorWindow *>(m_source)){
        list->setSelectedJumpers(value);list->updateUnselectedJumpers();list->getSelectedJumpersListView()->setupListModel();list->getUnselectedJumpersListView()->setupListModel();
    }else if(auto *team=qobject_cast<TeamEditorWidget *>(m_source))team->setJumpers(value);
}
void QuickRoster::apply(QVector<Jumper *> value){value=unique(value);if(value==m_current)return;for(auto *j:value)if(!m_pool.contains(j))return;m_undo=m_current;m_hasUndo=true;m_current=value;write(value);refresh();}
void QuickRoster::toggle(const QString &id){sync();if(!membership())return;auto *j=jumper(id);if(!j)return;auto next=m_current;if(next.contains(j))next.removeAll(j);else next.append(j);apply(next);}
void QuickRoster::addVisible(){sync();if(!membership())return;auto next=m_current;for(auto *j:visiblePool())if(!next.contains(j))next.append(j);apply(next);}
void QuickRoster::removeVisible(){sync();if(!membership())return;auto next=m_current;for(auto *j:visiblePool())next.removeAll(j);apply(next);}
void QuickRoster::mark(const QString &id,int modifiers){
    auto *j=jumper(id);if(!j||!m_current.contains(j))return;
    if((modifiers&Qt::ShiftModifier)&&!m_anchor.isEmpty()){int a=0,b=m_current.indexOf(j);for(int i=0;i<m_current.size();++i)if(key(m_current[i])==m_anchor)a=i;m_marked.clear();for(int i=qMin(a,b);i<=qMax(a,b);++i)m_marked.insert(key(m_current[i]));}
    else if(modifiers&Qt::ControlModifier){if(m_marked.contains(id))m_marked.remove(id);else m_marked.insert(id);m_anchor=id;}
    else {m_marked={id};m_anchor=id;}refresh();
}
void QuickRoster::markAll(){m_marked.clear();for(const auto &row:m_current)if(matches(row,m_orderQuery))m_marked.insert(key(row));refresh();}
void QuickRoster::removeMarked(){sync();if(!membership())return;auto next=m_current;for(int i=next.size()-1;i>=0;--i)if(m_marked.contains(key(next[i])))next.removeAt(i);apply(next);}
void QuickRoster::moveBefore(const QString &id,const QString &target,bool after){
    sync();auto *source=jumper(id),*destination=jumper(target);
    if(!source||!destination||!m_current.contains(source)||!m_current.contains(destination)||source==destination)return;
    if(!m_marked.contains(id))m_marked={id};if(m_marked.contains(target))return;
    QVector<Jumper *> remaining;for(auto *j:m_current)if(!m_marked.contains(key(j)))remaining.append(j);
    moveMarked(remaining.indexOf(destination)+(after?1:0)+1);
}
void QuickRoster::moveMarked(int position){
    sync();if(m_marked.isEmpty())return;
    QVector<Jumper *> block,next;for(auto *j:m_current)if(m_marked.contains(key(j)))block.append(j);else next.append(j);
    const int at=qBound(0,position-1,next.size());for(int i=0;i<block.size();++i)next.insert(at+i,block[i]);apply(next);
}
void QuickRoster::step(int direction){sync();if(direction!=-1&&direction!=1)return;auto next=m_current;if(direction<0){for(int i=1;i<next.size();++i)if(m_marked.contains(key(next[i]))&&!m_marked.contains(key(next[i-1])))qSwap(next[i],next[i-1]);}else for(int i=next.size()-2;i>=0;--i)if(m_marked.contains(key(next[i]))&&!m_marked.contains(key(next[i+1])))qSwap(next[i],next[i+1]);apply(next);}
void QuickRoster::sort(const QString &key){sync();auto next=m_current;if(key=="random"){std::mt19937 engine(std::random_device{}());std::shuffle(next.begin(),next.end(),engine);}else if(key=="name"||key=="country")std::stable_sort(next.begin(),next.end(),[&](auto *a,auto *b){return QString::localeAwareCompare(key=="name"?a->getNameAndSurname():a->getCountryCode(),key=="name"?b->getNameAndSurname():b->getCountryCode())<0;});else return;apply(next);}
void QuickRoster::undo(){sync();if(!m_hasUndo)return;const auto previous=m_undo;m_hasUndo=false;m_undo.clear();m_current=previous;write(previous);refresh();}
void QuickRoster::applyPreset(int index,int operation){sync();auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);if(!config||!config->simulationSave||!membership()||index<0||index>=config->simulationSave->getJumpersListsReference().size())return;const auto members=config->simulationSave->getJumpersListsReference()[index].getJumpers();auto next=m_current;if(operation==0){for(auto *j:members)if(m_pool.contains(j)&&!next.contains(j))next.append(j);}else if(operation==1){for(auto *j:members)next.removeAll(j);}else if(operation==2){next.clear();for(auto *j:members)if(m_current.contains(j))next.append(j);for(auto *j:m_current)if(!next.contains(j))next.append(j);}else return;apply(next);}
