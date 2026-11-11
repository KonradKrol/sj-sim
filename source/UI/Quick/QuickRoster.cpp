#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include "../../global/CountryFlagsManager.h"
#include <QRandomGenerator>
#include <QSpinBox>
#include <QCheckBox>
#include "../../form-engine/DailyForm.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QListWidget>
#include <QMessageBox>
#include "../Competition/TeamsSquadsTreeModel.h"
#include "../../simulation-v2/ProfileBridge.h"
#include "../../simulation-v2/WeatherSetup.h"
#include "../../playground/PlaygroundModel.h"
#include "../../balance/BalanceRuntime.h"
#include "QuickRoster.h"
#include "../Competition/CompetitionConfigWindow.h"
#include "../Seasons/SimulationSaveManagerWindow.h"
#include "../EditorWidgets/JumpersListEditorWindow.h"
#include "../EditorWidgets/TeamsEditing/TeamEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/CountryCodes.h"
#include "../../seasons/SaveJumpersList.h"
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QTimer>
#include <algorithm>
#include <random>

namespace {
const QList<QByteArray> roles{"athleteId","athleteName","countryCode","included","marked","position","activeSlot","flagSource"};
const QList<QByteArray> countryRoles{"country","flag","count","limit","over"};
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
QHash<int,QByteArray> QuickRosterCountryRows::roleNames() const { QHash<int,QByteArray> out;for(int i=0;i<countryRoles.size();++i)out[Qt::UserRole+1+i]=countryRoles[i];return out; }
QVariant QuickRosterCountryRows::data(const QModelIndex &index,int role) const { if(!index.isValid()||index.row()<0||index.row()>=m_rows.size())return {};const int column=role-Qt::UserRole-1;return column>=0&&column<countryRoles.size()?m_rows[index.row()].toMap().value(countryRoles[column]):QVariant(); }
void QuickRosterCountryRows::sync(const QVariantList &rows) {
    const int oldCount=m_rows.size();QSet<QString> countries;for(const auto &row:rows)countries.insert(row.toMap().value("country").toString());
    for(int i=m_rows.size()-1;i>=0;--i)if(!countries.contains(m_rows[i].toMap().value("country").toString())){beginRemoveRows({},i,i);m_rows.removeAt(i);endRemoveRows();}
    for(int i=0;i<rows.size();++i){const auto code=rows[i].toMap().value("country");int at=i;while(at<m_rows.size()&&m_rows[at].toMap().value("country")!=code)++at;
        if(at==m_rows.size()){beginInsertRows({},i,i);m_rows.insert(i,rows[i]);endInsertRows();}
        else if(at!=i){beginMoveRows({},at,at,{},i);m_rows.move(at,i);endMoveRows();}
        if(m_rows[i]!=rows[i]){m_rows[i]=rows[i];emit dataChanged(index(i),index(i));}
    }
    if(oldCount!=m_rows.size())emit countChanged();
}
QString QuickRoster::key(Jumper *jumper) { return QString::number(reinterpret_cast<quintptr>(jumper),16); }
QuickRoster::QuickRoster(QWidget *source,QObject *parent):QObject(parent),m_source(source),m_library(new QuickRosterRows(this)),m_ordered(new QuickRosterRows(this)),m_groupedRows(new QuickRosterRows(this)),m_groupedLibrary(new QuickRosterRows(this)),m_limitRows(new QuickRosterCountryRows(this)){m_grouped=savedList();sync();}
bool QuickRoster::membership() const { return !qobject_cast<TeamEditorWidget *>(m_source); }
bool QuickRoster::savedList() const { return qobject_cast<JumpersListEditorWindow *>(m_source) || qobject_cast<SimulationSaveManagerWindow *>(m_source); }
void QuickRoster::setGrouped(bool value) { if(!savedList() || m_grouped==value)return; m_grouped=value;emit changed(); }
void QuickRoster::setSeasonRoster(const QString &id) {
    if(!qobject_cast<SimulationSaveManagerWindow *>(m_source) || id==m_seasonRosterId)return;
    m_seasonRosterId=id;m_hasUndo=false;m_undo.clear();m_undoLimits.clear();m_undoLimitChange=false;m_marked.clear();m_candidateMarked.clear();m_query.clear();m_orderQuery.clear();m_country=0;
    m_current.clear();m_pool.clear();m_countryCodes.clear();
    QTimer::singleShot(0,this,[this]{sync();});
}
QMap<QString,int> QuickRoster::readCountryLimits() const {
    if(auto *editor=qobject_cast<JumpersListEditorWindow *>(m_source))return editor->getCountryLimits();
    if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(m_source))for(const auto &list:manager->getSimulationSave()->getJumpersLists())if(list.getId()==m_seasonRosterId)return list.getCountryLimits();
    return {};
}
void QuickRoster::writeCountryLimits(const QMap<QString,int> &limits) {
    if(auto *editor=qobject_cast<JumpersListEditorWindow *>(m_source))editor->setCountryLimits(limits);
    if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(m_source))manager->setRosterCountryLimits(m_seasonRosterId,limits);
}
void QuickRoster::refreshCountryLimits(bool updateModel) {
    if(!savedList()){m_countryLimits.clear();if(updateModel)m_limitRows->sync({});return;}
    const auto limits=readCountryLimits();QStringList codes;
    for(const auto &code:m_countryCodes)if(code.size()==3 && std::all_of(code.cbegin(),code.cend(),[](QChar c){return c.unicode()>='A' && c.unicode()<='Z';}))codes.append(code);
    for(auto it=limits.cbegin();it!=limits.cend();++it)if(!codes.contains(it.key()))codes.append(it.key());
    codes.sort();QVariantList rows;QHash<QString,int> counts;
    for(auto *jumper:m_current)++counts[jumper->getCountryCode().trimmed().toUpper()];
    for(const auto &code:codes){const int count=counts.value(code,0);
        const int limit=limits.value(code,-1);
        rows.append(QVariantMap{{"country",code},{"flag",flagSource(code)},{"count",count},{"limit",limit},{"over",limit>=0 && count>limit}});
    }
    m_countryLimits=rows;if(updateModel)m_limitRows->sync(rows);
}
int QuickRoster::exceededCountries() const {
    int count=0;for(const auto &row:countryLimits())if(row.toMap().value("over").toBool())++count;return count;
}
bool QuickRoster::setCountryLimit(const QString &country,int limit) {
    if(!savedList())return false;
    const auto currentLimits=readCountryLimits();
    SaveJumpersList validated;validated.setCountryLimits(currentLimits);
    if(!validated.setCountryLimit(country,limit))return false;
    if(currentLimits==validated.getCountryLimits())return true;
    m_undoLimits=currentLimits;m_undoLimitChange=true;m_hasUndo=true;
    writeCountryLimits(validated.getCountryLimits());
    // Keep the focused text field intact until its editingFinished callback returns.
    refreshCountryLimits(false);
    QTimer::singleShot(0,this,[this]{m_limitRows->sync(m_countryLimits);emit changed();});
    return true;
}
bool QuickRoster::active() const {
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source))return (config->seasonCompetition?config->seasonCompetition->getRules().getCompetitionType():config->competitionRulesEditor->getCompetitionTypeFromInput())==CompetitionRules::Individual;
    return true;
}
int QuickRoster::activeLimit() const { auto *team=qobject_cast<TeamEditorWidget *>(m_source);return team?team->getJumpersCount():0; }
QVector<Jumper *> QuickRoster::read() const {
    if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(m_source)){for(const auto &list:manager->getSimulationSave()->getJumpersLists())if(list.getId()==m_seasonRosterId)return list.getJumpers();return {};}
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source))return config->type==CompetitionConfigWindow::SingleCompetition?config->competitionJumpers:config->seasonCompetitionJumpers;
    if(auto *list=qobject_cast<JumpersListEditorWindow *>(m_source))return list->getSelectedJumpers();
    if(auto *team=qobject_cast<TeamEditorWidget *>(m_source))return team->getJumpers();
    return {};
}
QVector<Jumper *> QuickRoster::candidates() const {
    if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(m_source))return manager->getSimulationSave()->getJumpers();
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source)){
        if(config->type==CompetitionConfigWindow::SingleCompetition)return config->availableJumpers();
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
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);config&&config->type==CompetitionConfigWindow::SingleCompetition && !config->usesCanonicalCatalog())pool=unique(pool+m_pool+current);
    else pool=unique(pool+current);
    if(current==m_current&&pool==m_pool&&!m_countryCodes.isEmpty()&&m_presetNames==presets())return;
    m_presetNames=presets();
    if(current!=m_current){m_hasUndo=false;m_undo.clear();m_undoLimits.clear();m_undoLimitChange=false;}m_current=current;m_pool=pool;
    m_countryCodes.clear();for(auto *j:m_pool){const auto code=j->getCountryCode().trimmed().toUpper();if(!m_countryCodes.contains(code))m_countryCodes.append(code);}m_countryCodes.sort();m_country=qBound(0,m_country,m_countryCodes.size());refresh();
}
QStringList QuickRoster::countries() const { auto out=m_countryCodes;out.prepend(tr("Wszystkie kraje"));return out; }
QStringList QuickRoster::presets() const { QStringList out;if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);config&&config->simulationSave)for(const auto &list:config->simulationSave->getJumpersLists())out.append(list.getName());return out; }
QVector<Jumper *> QuickRoster::visiblePool() const { QVector<Jumper *> out;for(auto *j:m_pool)if(matches(j,m_query)&&(!m_country||j->getCountryCode().trimmed().toUpper()==m_countryCodes.value(m_country-1)))out.append(j);return out; }
void QuickRoster::refresh(){
    QSet<QString> existing;for(auto *j:m_current)existing.insert(key(j));m_marked.intersect(existing);
    auto available=visiblePool();
    QSet<QString> poolIds;for(auto *j:m_pool)poolIds.insert(key(j));m_candidateMarked.intersect(poolIds);
    for(auto *j:m_current)m_candidateMarked.remove(key(j));
    QVariantList library,ordered;for(auto *j:available)library.append(QVariantMap{{"athleteId",key(j)},{"athleteName",j->getNameAndSurname()},{"countryCode",j->getCountryCode()},{"flagSource",flagSource(j->getCountryCode())},{"included",m_current.contains(j)},{"marked",m_candidateMarked.contains(key(j))}});
    for(int i=0;i<m_current.size();++i){auto *j=m_current[i];if(!matches(j,m_orderQuery))continue;ordered.append(QVariantMap{{"athleteId",key(j)},{"athleteName",j->getNameAndSurname()},{"countryCode",j->getCountryCode()},{"flagSource",flagSource(j->getCountryCode())},{"included",true},{"marked",m_marked.contains(key(j))},{"position",i+1},{"activeSlot",!activeLimit()||i<activeLimit()}});}
    auto grouped=ordered;
    if(savedList())std::stable_sort(grouped.begin(),grouped.end(),[](const QVariant &a,const QVariant &b){return a.toMap().value("countryCode").toString().compare(b.toMap().value("countryCode").toString(),Qt::CaseInsensitive)<0;});
    else grouped.clear();
    auto groupedLibrary=library;
    if(savedList())std::stable_sort(groupedLibrary.begin(),groupedLibrary.end(),[](const QVariant &a,const QVariant &b){return a.toMap().value("countryCode").toString().compare(b.toMap().value("countryCode").toString(),Qt::CaseInsensitive)<0;});
    else groupedLibrary.clear();
    m_library->sync(library);m_groupedLibrary->sync(groupedLibrary);m_ordered->sync(ordered);m_groupedRows->sync(grouped);refreshCountryLimits();emit changed();
}
void QuickRoster::setQuery(const QString &value){if(m_query==value)return;m_query=value;refresh();}
void QuickRoster::setOrderQuery(const QString &value){if(m_orderQuery==value)return;m_orderQuery=value;refresh();}
void QuickRoster::setCountry(int value){if(value<0||value>m_countryCodes.size()||value==m_country)return;m_country=value;refresh();}
Jumper *QuickRoster::jumper(const QString &id) const { for(auto *j:m_pool)if(key(j)==id)return j;return nullptr; }
void QuickRoster::write(const QVector<Jumper *> &value){
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);config && config->teamSquadsLocked())return;
    if(auto *manager=qobject_cast<SimulationSaveManagerWindow *>(m_source)){manager->setRosterMembers(m_seasonRosterId,value);return;}
    if(auto *config=qobject_cast<CompetitionConfigWindow *>(m_source)){
        if(config->type==CompetitionConfigWindow::SingleCompetition)config->competitionJumpers=value;else config->seasonCompetitionJumpers=value;
        config->jumpersListView->setupListModel();
        if(config->usesCanonicalCatalog())config->updateCanonicalReadiness();
        else if(config->getWeatherSetup())config->updateCanonicalGateSuggestion();
        if(config->KOGroupsList&&!config->comboBox_groupsSelectionType->isHidden()&&config->comboBox_groupsSelectionType->currentIndex()>0){
            auto &groups=config->type==CompetitionConfigWindow::SingleCompetition?config->competitionGroups:config->seasonCompetitionGroups;
            const auto rules=config->seasonCompetition?config->seasonCompetition->getRules():config->competitionRulesEditor->getCompetitionRulesFromWidgetInputs();
            if(!rules.getRounds().isEmpty()){auto round=rules.getRounds().first();auto jumpers=value;groups=KOGroup::constructKOGroups(&round,&jumpers,config->comboBox_groupsSelectionType->currentIndex()-1,config->seasonCompetition);config->KOGroupsList->setKOGroups(&groups);config->KOGroupsList->fillListLayout();}
        }
    }else if(auto *list=qobject_cast<JumpersListEditorWindow *>(m_source)){
        list->setSelectedJumpers(value);list->updateUnselectedJumpers();list->getSelectedJumpersListView()->setupListModel();list->getUnselectedJumpersListView()->setupListModel();
    }else if(auto *team=qobject_cast<TeamEditorWidget *>(m_source))team->setJumpers(value);
}
void QuickRoster::apply(QVector<Jumper *> value){value=unique(value);if(value==m_current)return;for(auto *j:value)if(!m_pool.contains(j))return;m_undo=m_current;m_undoLimits.clear();m_undoLimitChange=false;m_hasUndo=true;m_current=value;write(value);refresh();}
void QuickRoster::toggle(const QString &id){sync();if(!membership())return;auto *j=jumper(id);if(!j)return;auto next=m_current;if(next.contains(j))next.removeAll(j);else next.append(j);apply(next);}
void QuickRoster::addVisible(){sync();if(!membership())return;auto next=m_current;for(auto *j:visiblePool())if(!next.contains(j))next.append(j);apply(next);}
void QuickRoster::selectCandidate(const QString &id){auto *j=jumper(id);if(!j||m_current.contains(j))return;if(m_candidateMarked.contains(id))m_candidateMarked.remove(id);else m_candidateMarked.insert(id);refresh();}
void QuickRoster::addMarkedCandidates(){sync();if(!membership())return;auto next=m_current;for(auto *j:m_pool)if(m_candidateMarked.contains(key(j))&&!next.contains(j))next.append(j);apply(next);}
void QuickRoster::include(const QString &id){sync();if(!membership())return;auto *j=jumper(id);if(!j||m_current.contains(j))return;auto next=m_current;next.append(j);apply(next);}
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
    if(qobject_cast<TeamEditorWidget *>(m_source)){auto next=m_current;qSwap(next[next.indexOf(source)],next[next.indexOf(destination)]);m_marked={id};apply(next);return;}
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
void QuickRoster::sort(const QString &key){sync();auto next=m_current;if(key=="random"){std::mt19937 engine(std::random_device{}());std::shuffle(next.begin(),next.end(),engine);}else if(key=="overall-asc"||key=="overall-desc")std::stable_sort(next.begin(),next.end(),[&](auto *a,auto *b){const double left=a->getSimulationProfile()?a->getSimulationProfile()->overall:0;const double right=b->getSimulationProfile()?b->getSimulationProfile()->overall:0;return key=="overall-asc"?left<right:left>right;});else if(key=="name"||key=="country")std::stable_sort(next.begin(),next.end(),[&](auto *a,auto *b){return QString::localeAwareCompare(key=="name"?a->getNameAndSurname():a->getCountryCode(),key=="name"?b->getNameAndSurname():b->getCountryCode())<0;});else return;apply(next);}
void QuickRoster::undo(){sync();if(!m_hasUndo)return;if(m_undoLimitChange){writeCountryLimits(m_undoLimits);m_hasUndo=false;m_undoLimitChange=false;m_undoLimits.clear();refreshCountryLimits();emit changed();return;}const auto previous=m_undo;m_hasUndo=false;m_undo.clear();m_current=previous;write(previous);refresh();}
void QuickRoster::applyPreset(int index,int operation){if(operation==3){sync();if(index<0){apply(m_pool);return;}auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);if(config && config->simulationSave && index<config->simulationSave->getJumpersListsReference().size()){QVector<Jumper *> next;for(auto *j:config->simulationSave->getJumpersListsReference()[index].getJumpers())if(m_pool.contains(j))next.append(j);apply(next);}return;}sync();auto *config=qobject_cast<CompetitionConfigWindow *>(m_source);if(!config||!config->simulationSave||!membership()||index<0||index>=config->simulationSave->getJumpersListsReference().size())return;const auto members=config->simulationSave->getJumpersListsReference()[index].getJumpers();auto next=m_current;if(operation==0){for(auto *j:members)if(m_pool.contains(j)&&!next.contains(j))next.append(j);}else if(operation==1){for(auto *j:members)next.removeAll(j);}else if(operation==2){next.clear();for(auto *j:members)if(m_current.contains(j))next.append(j);for(auto *j:m_current)if(!next.contains(j))next.append(j);}else return;apply(next);}

bool QuickRoster::teams() const {
    auto *c=qobject_cast<CompetitionConfigWindow *>(m_source);if(!c)return false;
    return (c->seasonCompetition?c->seasonCompetition->getRules():c->competitionRulesEditor->getCompetitionRulesFromWidgetInputs()).getCompetitionType()==CompetitionRules::Team;
}
bool QuickRoster::knockout() const {
    auto *c=qobject_cast<CompetitionConfigWindow *>(m_source);if(!c||teams())return false;
    const auto rules=c->seasonCompetition?c->seasonCompetition->getRules():c->competitionRulesEditor->getCompetitionRulesFromWidgetInputs();
    return !rules.getRounds().isEmpty() && rules.getRounds().first().getKO();
}
void QuickRoster::tool(const QString &operation) {
    sync();auto *c=qobject_cast<CompetitionConfigWindow *>(m_source);
    if(!c || knockout())return;
    const bool team=teams();auto next=m_current;auto teamList=c->competitionTeams;
    QHash<QString,double> scores;QHash<QString,int> countries;
    const auto id=[](Jumper *j){return j?j->getIDStr():QString();};
    const auto teamKey=[](const Team &t){return t.getCountryCode();};
    std::mt19937 random(std::random_device{}());
    if(operation=="countries-manual") {
        QDialog dialog(c);dialog.setWindowTitle(tr("Kolejność krajów"));auto *layout=new QVBoxLayout(&dialog);
        layout->addWidget(new QLabel(tr("Przeciągnij kraje, aby ustalić kolejność."),&dialog));
        auto *list=new QListWidget(&dialog);list->setDragDropMode(QAbstractItemView::InternalMove);
        QStringList codes;for(auto *j:next)if(!codes.contains(j->getCountryCode()))codes.append(j->getCountryCode());
        list->addItems(codes);layout->addWidget(list);
        auto *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dialog);layout->addWidget(buttons);
        connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept);connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
        if(dialog.exec()!=QDialog::Accepted)return;
        for(int i=0;i<list->count();++i)countries[list->item(i)->text()]=i;
    } else if(operation=="copy-order" || operation=="copy-exact" || operation=="results" || operation=="classification") {
        if(!c->simulationSave){QMessageBox::information(c,tr("Brak źródeł"),tr("Brak rozegranych konkursów i klasyfikacji w tym trybie."));return;}
        QDialog dialog(c);dialog.setWindowTitle(operation=="classification"?tr("Wybierz klasyfikację"):tr("Wybierz konkurs"));
        auto *layout=new QVBoxLayout(&dialog);auto *season=new QComboBox(&dialog);auto *source=new QComboBox(&dialog);layout->addWidget(season);layout->addWidget(source);
        QVector<CompetitionInfo *> events;QVector<Classification *> classes;
        auto &seasons=c->simulationSave->getSeasonsReference();int current=0;
        for(int i=0;i<seasons.size();++i){season->addItem(tr("Sezon %1").arg(seasons[i].getSeasonNumber()));if(&seasons[i]==c->simulationSave->getActualSeason())current=i;}
        const auto fill=[&] {
            source->clear();events.clear();classes.clear();if(season->currentIndex()<0)return;
            for(auto *calendar:seasons[season->currentIndex()].getCalendars()) {
                if(operation=="classification") {
                    for(auto *entry:calendar->getClassifications())if(entry->getClassificationType()==(team?CompetitionRules::Team:CompetitionRules::Individual)){classes.append(entry);source->addItem(entry->getName());}
                } else {
                    const auto all=calendar->getCompetitions();
                    for(int i=all.size()-1;i>=0;--i){auto *event=all[i];if(!event->getPlayed() || event->getRules().getCompetitionType()!=(team?CompetitionRules::Team:CompetitionRules::Individual))continue;
                        events.append(event);auto *hill=event->getHill();QString name=hill?hill->getName()+" HS"+QString::number(hill->getHSPoint()):tr("Brak skoczni");
                        if(event->getSerieType()==CompetitionInfo::Qualifications)name+=" Q";
                        if(event->getScheduledDate().isValid())name+=" ("+QLocale().dayName(event->getScheduledDate().dayOfWeek(),QLocale::ShortFormat)+")";
                        if(team)name+=" TEAM";source->addItem(hill?QIcon(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower())):QIcon(),name);
                    }
                }
            }
        };
        connect(season,QOverload<int>::of(&QComboBox::currentIndexChanged),&dialog,[&]{fill();});season->setCurrentIndex(current);fill();
        auto *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,&dialog);layout->addWidget(buttons);
        connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept);connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
        dialog.resize(440,160);if(dialog.exec()!=QDialog::Accepted || source->currentIndex()<0)return;
        if(operation=="classification") {
            for(auto *result:classes[source->currentIndex()]->getRankedResults())scores[team?result->getTeamCode():id(result->getJumper())]=result->getPointsSum();
        } else {
            auto *event=events[source->currentIndex()];
            if(operation=="results") {
                for(const auto &result:event->getResultsReference().getResultsReference())scores[team && result.getTeam()?result.getTeam()->getCountryCode():id(result.getJumper())]=result.getPointsSum();
            } else {
                QStringList order;
                if(team){for(const auto &t:event->getTeams())order.append(teamKey(t));}
                else for(auto *j:event->getStartList())order.append(id(j));
                if(operation=="copy-exact") {
                    if(QMessageBox::question(c,tr("Zastąp listę startową"),tr("Zastąpić uczestników i kolejność listy startowej?"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;
                    if(team){
                        if(!c->teamSquadsLocked()){
                            const auto rules=c->seasonCompetition?c->seasonCompetition->getRules():c->competitionRulesEditor->getCompetitionRulesFromWidgetInputs();
                            for(const auto &candidate:Team::constructTeamsVectorByJumpersList(m_pool,rules.getJumpersInTeamCount())) {
                                bool exists=false;for(const auto &t:teamList)if(teamKey(t)==teamKey(candidate)){exists=true;break;}
                                if(!exists)teamList.append(candidate);
                            }
                        }
                        QVector<Team> copied;for(const auto &key:order)for(const auto &t:teamList)if(teamKey(t)==key){copied.append(t);break;}teamList=copied;
                    }
                    else {next.clear();for(const auto &key:order)for(auto *j:m_pool)if(id(j)==key){next.append(j);break;}}
                }
                for(int i=0;i<order.size();++i)scores[order[i]]=i;
            }
        }
    } else if(operation=="ovr" || operation=="strength") {
        const auto parameters=Playground::engineParameters(BalanceRuntime::gameCapture());
        const auto hill=c->configuredHill();
        const SimulationV2::QualityModel quality(parameters.quality,SimulationV2::HillSkillCurve(parameters.skillAnchors,parameters.curveExtension));
        const auto strength=[&](Jumper *j) {
            const auto profile=j->getSimulationProfile();if(!profile)return 0.;
            if(operation=="ovr")return double(profile->overall);
            auto actor=SimulationV2::athleteInput(*profile);
            if(c->simulationSave && c->simulationSave->getActualSeason()->getDatesEnabled() && c->seasonCompetition && c->seasonCompetition->getScheduledDate().isValid())
                actor.form=AutoForm::dailyForm(actor.form,SimulationV2::deriveSeed(c->simulationSave->formEngine().seed,quint64(c->seasonCompetition->getScheduledDate().toJulianDay())),profile->id);
            else if(auto *enabled=c->findChild<QCheckBox *>("checkBox_dailyForm");enabled && enabled->isChecked()) {
                if(!c->property("dayFormSeed").isValid())c->setProperty("dayFormSeed",QVariant::fromValue<qulonglong>(QRandomGenerator::global()->generate64()));
                actor.form=AutoForm::dailyForm(actor.form,c->property("dayFormSeed").toULongLong(),profile->id);
            }
            const auto venue=hill.getSimulationProfile();if(!venue)return double(actor.overall);
            const auto input=SimulationV2::hillInput(*venue);
            const auto type=c->seasonCompetition?c->seasonCompetition->getSerieType():CompetitionInfo::Competition;
            const auto event=type==CompetitionInfo::Training?SimulationV2::EventKind::Training:type==CompetitionInfo::TrialRound?SimulationV2::EventKind::Trial:type==CompetitionInfo::Qualifications?SimulationV2::EventKind::Qualification:SimulationV2::EventKind::Competition;
            double value=quality.baseline(actor,input.k,input.takeoffBias,event).value();
            if(auto *weather=c->getWeatherSetup();weather && weather->field()) {
                const SimulationV2::FlightModel flight(parameters.flight);
                const int gate=c->findChild<QSpinBox *>("spinBox_startGate")?c->findChild<QSpinBox *>("spinBox_startGate")->value():0;
                const double reserve=flight.reserve(value,input,gate,0,weather->field()->wind(0,0).head);
                return flight.fly(reserve,input,*weather->field(),0,false,{},actor.perks[1]).distance;
            }
            return value;
        };
        if(team)for(const auto &t:teamList){double value=0;for(auto *j:t.getJumpers())value+=strength(j);scores[teamKey(t)]=t.getJumpers().isEmpty()?0:value/t.getJumpers().size();}
        else for(auto *j:next)scores[id(j)]=strength(j);
    }
    const bool ranked=!scores.isEmpty();
    if(team) {
        if(operation=="reverse")std::reverse(teamList.begin(),teamList.end());
        else if(operation=="random")std::shuffle(teamList.begin(),teamList.end(),random);
        else if(ranked){std::shuffle(teamList.begin(),teamList.end(),random);std::stable_sort(teamList.begin(),teamList.end(),[&](const Team &a,const Team &b){return scores.value(teamKey(a),-1e100)<scores.value(teamKey(b),-1e100);});}
        else if(operation=="name")std::stable_sort(teamList.begin(),teamList.end(),[&](const Team &a,const Team &b){return QString::localeAwareCompare(teamKey(a),teamKey(b))<0;});
        c->competitionTeams=teamList;c->teamsSquadsModel->setupTreeItems();emit changed();return;
    }
    if(operation=="reverse")std::reverse(next.begin(),next.end());
    else if(operation=="random")std::shuffle(next.begin(),next.end(),random);
    else if(ranked){std::shuffle(next.begin(),next.end(),random);std::stable_sort(next.begin(),next.end(),[&](Jumper *a,Jumper *b){return scores.value(id(a),-1e100)<scores.value(id(b),-1e100);});}
    else if(operation=="name")std::stable_sort(next.begin(),next.end(),[](Jumper *a,Jumper *b){return QString::localeAwareCompare(a->getSurname()+" "+a->getName(),b->getSurname()+" "+b->getName())<0;});
    else if(operation=="country" || operation=="countries-manual")std::stable_sort(next.begin(),next.end(),[&](Jumper *a,Jumper *b){return operation=="country"?QString::localeAwareCompare(a->getCountryCode(),b->getCountryCode())<0:countries.value(a->getCountryCode())<countries.value(b->getCountryCode());});
    apply(next);
}
