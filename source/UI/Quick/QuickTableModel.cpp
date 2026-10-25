#include "QuickUi.h"
#include "../../competitions/CompetitionAdvancement.h"
#include <QAbstractItemView>
#include <QTreeView>
#include <QItemSelectionModel>
#include <QTimer>
#include <QIcon>
#include <QBuffer>
#include <QPixmap>
#include <QBrush>
#include <QFont>
#include <QSet>
#include <QSignalBlocker>
#include "../Competition/Results/TeamResultsTreeModel.h"
#include "../Competition/CompetitionManagerWindow.h"

QString quickPicture(const QPixmap &pixmap) {
    if (pixmap.isNull()) return {};
    static QHash<qint64, QString> cache;
    if (cache.contains(pixmap.cacheKey())) return cache.value(pixmap.cacheKey());
    QByteArray bytes;
    QBuffer buffer(&bytes); buffer.open(QIODevice::WriteOnly); pixmap.save(&buffer, "PNG");
    const QString url = "data:image/png;base64," + QString::fromLatin1(bytes.toBase64());
    if (cache.size() > 1024) cache.clear();
    cache.insert(pixmap.cacheKey(), url);
    return url;
}
QString quickDecoration(const QVariant &value) {
    if (value.canConvert<QIcon>()) return quickPicture(qvariant_cast<QIcon>(value).pixmap(32,24));
    if (value.canConvert<QPixmap>()) return quickPicture(qvariant_cast<QPixmap>(value));
    return {};
}

QuickTableModel::QuickTableModel(QAbstractItemView *view, QObject *parent)
    : QAbstractListModel(parent), m_view(view) { sync(); }
int QuickTableModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_rows.size(); }
QHash<int,QByteArray> QuickTableModel::roleNames() const {
    return {{Qt::UserRole+1,"cells"}, {Qt::UserRole+2,"selected"}, {Qt::UserRole+3,"depth"},
            {Qt::UserRole+4,"branch"}, {Qt::UserRole+5,"expanded"}, {Qt::UserRole+6,"current"}, {Qt::UserRole+7,"statusText"}, {Qt::UserRole+8,"tint"}, {Qt::UserRole+9,"advanceState"}, {Qt::UserRole+10,"qualificationState"}, {Qt::UserRole+11,"qualificationTarget"}, {Qt::UserRole+12,"luckyLoser"}, {Qt::UserRole+13,"latestJump"}};
}
QModelIndex QuickTableModel::cell(int row, int column) const {
    return m_source && row >= 0 && row < m_rows.size() ? m_rows[row].sibling(m_rows[row].row(), column) : QModelIndex();
}
QVariant QuickTableModel::data(const QModelIndex &index, int role) const {
    if(m_frozen)return index.row()>=0 && index.row()<m_snapshot.size()?m_snapshot[index.row()].value(role):QVariant();
    const bool teamSetup=m_source && m_source->inherits("TeamsSquadsTreeModel");
    const auto first = cell(index.row(),0);
    if (!first.isValid() || !m_view) return {};
    auto *selection = m_view->selectionModel();
    switch (role) {
    case Qt::UserRole+1: {
        QVariantList cells;
        for (int c = 0; c < m_source->columnCount(first.parent()); ++c) {
            const auto item = cell(index.row(), c);
            const auto flags = item.flags();
            QString text=item.data().toString();
            if(teamSetup && c==1){QStringList members;for(int r=0;r<m_source->rowCount(first);++r)members.append(m_source->index(r,1,first).data().toString());text=members.join(" · ");}
            cells.append(QVariantMap{{"text",text}, {"value",item.data(Qt::EditRole)},
                {"icon",quickDecoration(item.data(Qt::DecorationRole))}, {"tip",item.data(Qt::ToolTipRole)},
                {"editable",bool(flags & Qt::ItemIsEditable)}, {"enabled",bool(flags & Qt::ItemIsEnabled)},
                {"checkable",bool(flags & Qt::ItemIsUserCheckable) && item.data(Qt::CheckStateRole).isValid()}, {"checked",item.data(Qt::CheckStateRole).toInt() == Qt::Checked}});
        }
        return cells;
    }
    case Qt::UserRole+2:
        if (selection) {
            for (int column = 0; column < m_source->columnCount(first.parent()); ++column)
                if (selection->isSelected(first.sibling(first.row(), column))) return true;
        }
        return false;
    case Qt::UserRole+3: { int depth=0; for(auto p=first.parent();p.isValid();p=p.parent()) ++depth; return depth; }
    case Qt::UserRole+4: return !teamSetup && m_source->hasChildren(first);
    case Qt::UserRole+5: return qobject_cast<QTreeView *>(m_view) && static_cast<QTreeView *>(m_view.data())->isExpanded(first);
    case Qt::UserRole+6: return selection && selection->currentIndex().isValid() && selection->currentIndex().row()==first.row() && selection->currentIndex().parent()==first.parent();
    case Qt::UserRole+9: return first.data(CompetitionAdvancement::RoundStatusRole).isValid()?first.data(CompetitionAdvancement::RoundStatusRole):QVariant(-1);
    case Qt::UserRole+10: return first.data(CompetitionAdvancement::QualificationStatusRole).isValid()?first.data(CompetitionAdvancement::QualificationStatusRole):QVariant(-1);
    case Qt::UserRole+11: return first.data(CompetitionAdvancement::QualificationTargetRole);
    case Qt::UserRole+12: return first.data(CompetitionAdvancement::LuckyLoserRole);
    case Qt::UserRole+13: return first.data(CompetitionAdvancement::LatestJumpRole);
    case Qt::UserRole+7:
    case Qt::UserRole+8: {
        if(role==Qt::UserRole+7) {
            const auto round=first.data(CompetitionAdvancement::RoundStatusRole),qualification=first.data(CompetitionAdvancement::QualificationStatusRole);
            if(round.isValid() || qualification.isValid()) {
                QStringList labels;
                const bool group=QString::fromLatin1(m_source->metaObject()->className())=="KOGroupResultsTableModel";
                if(round.toInt()==StartListCompetitorStatus::SureAdvanced && round.toInt()>=0)labels.append(group?tr("Awans z grupy KO"):tr("Awans do następnej serii"));
                else if(round.toInt()==StartListCompetitorStatus::SureDroppedOut)labels.append(group?tr("Poza awansem z grupy KO"):tr("Poza awansem do następnej serii"));
                if(first.data(CompetitionAdvancement::LuckyLoserRole).toBool())labels.append(tr("LL · Lucky loser"));
                if(qualification.toInt()==StartListCompetitorStatus::SureAdvanced && qualification.toInt()>=0)labels.append(tr("Q · Awans do konkursu"));
                else if(qualification.toInt()==StartListCompetitorStatus::SureDroppedOut)labels.append(tr("Poza awansem do konkursu"));
                if(!labels.isEmpty())return labels.join(QStringLiteral(" · "));
                return QString();
            }
        }
        const auto value=first.data(Qt::BackgroundRole);
        QColor color=value.canConvert<QBrush>()?qvariant_cast<QBrush>(value).color():qvariant_cast<QColor>(value);
        if(role==Qt::UserRole+8) return color.isValid()?color.name():QString();
        const QString hex=color.name();const QString type=QString::fromLatin1(m_source->metaObject()->className());
        if(type=="StartListModel" && qvariant_cast<QFont>(first.data(Qt::FontRole)).italic()) return tr("Skok oddany");
        if(type=="CalendarEditorTableModel") {
            if(hex=="#c8c8c8")return tr("Rozegrany");
            if(hex=="#ffc7c7")return tr("Odwołany");
        }
        if(type=="ResultsTableModel" || type=="TeamResultsTreeModel" || type=="KOGroupResultsTableModel") {
            if(hex=="#e4ffde" || hex=="#e9f5ec" || hex=="#f2fff6")return tr("Awans");
            if(hex=="#fceded" || hex=="#ffd6d6")return tr("Poza awansem");
            if((type=="KOGroupResultsTableModel" || m_view->window()->inherits("CompetitionManagerWindow")) && (hex=="#fffff7" || hex=="#fdfdf9" || hex=="#f8f8f4"))return tr("Oczekuje");
            if(hex=="#e8f3ff")return tr("Ostatni skok");
        }
        return QString();
    }
    default: return {};
    }
}
QStringList QuickTableModel::headers() const {
    QStringList values;
    if(m_source && m_source->inherits("TeamsSquadsTreeModel"))return {tr("Drużyna"),tr("Skład")};
    if(m_source && m_view) for(int c=0;c<m_source->columnCount(m_view->rootIndex());++c) values << m_source->headerData(c,Qt::Horizontal).toString();
    return values;
}
bool QuickTableModel::multiple() const { return m_view && m_view->selectionMode()!=QAbstractItemView::SingleSelection && m_view->selectionMode()!=QAbstractItemView::NoSelection; }
void QuickTableModel::sync() {
    if(m_frozen)return;
    if(!m_view) return;
    if(m_source == m_view->model() && m_selection == m_view->selectionModel()) return;
    for(const auto &connection:m_connections) disconnect(connection);
    m_connections.clear(); m_source=m_view->model(); m_selection=m_view->selectionModel();
    if(m_source) {
        auto update=[this] { rebuild(); };
        m_connections << connect(m_source,&QAbstractItemModel::modelReset,this,update)
                      << connect(m_source,&QAbstractItemModel::layoutChanged,this,update)
                      << connect(m_source,&QAbstractItemModel::rowsInserted,this,update)
                      << connect(m_source,&QAbstractItemModel::rowsRemoved,this,update)
                      << connect(m_source,&QAbstractItemModel::columnsInserted,this,update)
                      << connect(m_source,&QAbstractItemModel::columnsRemoved,this,update)
                      << connect(m_source,&QAbstractItemModel::headerDataChanged,this,[this] { emit structureChanged(); })
                      << connect(m_source,&QAbstractItemModel::dataChanged,this,[this] {
                          if(!m_rows.isEmpty()) emit dataChanged(index(0),index(m_rows.size()-1));
                      });
    }
    if(auto *selection=m_view->selectionModel()) {
        auto update=[this] { if(!m_rows.isEmpty()) emit dataChanged(index(0),index(m_rows.size()-1),{Qt::UserRole+2,Qt::UserRole+6}); emit cursorChanged(); };
        m_connections << connect(selection,&QItemSelectionModel::selectionChanged,this,update)
                      << connect(selection,&QItemSelectionModel::currentChanged,this,update);
    }
    if(auto *tree=qobject_cast<QTreeView *>(m_view)) {
        m_connections << connect(tree,&QTreeView::expanded,this,[this]{ rebuild(); })
                      << connect(tree,&QTreeView::collapsed,this,[this]{ rebuild(); });
    }
    rebuild();
}
void QuickTableModel::setTeamMode(int mode){mode=qBound(0,mode,2);if(mode==m_teamMode)return;m_teamMode=mode;rebuild();}
void QuickTableModel::rebuild() {
    if(m_frozen)return;
    if(auto *tree=qobject_cast<QTreeView *>(m_view);tree && m_source && m_teamMode>=0) {
        const QSignalBlocker blocker(tree);
        auto *teams=qobject_cast<TeamResultsTreeModel *>(m_source);
        if(teams)for(int r=0;r<m_source->rowCount();++r){auto index=m_source->index(r,0);auto *result=teams->getResults()->getResultByIndex(r);tree->setExpanded(index,m_teamMode==2 || (m_teamMode==0 && result && result->getTeam()==teams->getLastTeam()));}
    }
    beginResetModel(); m_rows.clear();
    std::function<void(QModelIndex)> append=[&](QModelIndex parent) {
        if(!m_source) return;
        for(int r=0;r<m_source->rowCount(parent);++r) {
            auto item=m_source->index(r,0,parent);
            bool matches=m_filter.isEmpty();
            for(int c=0;!matches && c<m_source->columnCount(parent);++c) matches=m_source->index(r,c,parent).data().toString().contains(m_filter,Qt::CaseInsensitive);
            if(m_source->inherits("TeamsSquadsTreeModel") && !matches)for(int child=0;!matches && child<m_source->rowCount(item);++child)matches=m_source->index(child,1,item).data().toString().contains(m_filter,Qt::CaseInsensitive);
            if(matches) m_rows.append(item);
            if(auto *tree=qobject_cast<QTreeView *>(m_view); tree && !m_source->inherits("TeamsSquadsTreeModel") && (tree->isExpanded(item) || !m_filter.isEmpty())) append(item);
        }
    };
    if(m_view) append(m_view->rootIndex());
    endResetModel(); emit structureChanged(); emit cursorChanged();
}
void QuickTableModel::select(int row,int column,int modifiers,bool open) {
    if(m_frozen)return;
    QPersistentModelIndex item=cell(row,column);
    if(!item.isValid() || !m_view || !(item.flags() & Qt::ItemIsEnabled)) return;
    if(auto *selection=m_view->selectionModel()) {
        QItemSelectionModel::SelectionFlags flags=QItemSelectionModel::ClearAndSelect;
        if(m_view->selectionBehavior()==QAbstractItemView::SelectRows || m_source->columnCount()==1) flags|=QItemSelectionModel::Rows;
        if(multiple() && (modifiers & Qt::ControlModifier)) flags=QItemSelectionModel::Toggle | (flags & QItemSelectionModel::Rows);
        if(multiple() && (modifiers & Qt::ShiftModifier) && m_anchor>=0 && m_anchor<m_rows.size() && cell(m_anchor,column).parent()==item.parent()) {
            selection->select(QItemSelection(cell(qMin(m_anchor,row),column),cell(qMax(m_anchor,row),column)),flags);
        } else { selection->select(item,flags); m_anchor=row; }
        selection->setCurrentIndex(item,QItemSelectionModel::NoUpdate);
    }
    // Slots may show a modal dialog or delete a model. Leave the QML event first.
    const QString sourceType=QString::fromLatin1(m_source->metaObject()->className());
    const bool history=m_view->objectName()=="historyResultsView",teamHistory=history && m_view->property("historyTeam").toBool();
    const int round=(modifiers & Qt::ShiftModifier)?(sourceType=="ResultsTableModel" || (history && !teamHistory)) && column>=3?(column-3)/2+1:(sourceType=="TeamResultsTreeModel" || teamHistory) && column>=5?column-4:0:0;
    QTimer::singleShot(0,this,[this,item,open,round] {
        if(!m_view || !item.isValid()) return;
        if(round>0 && m_view->objectName()=="historyResultsView"){m_view->setProperty("historyRoundRequested",round);QMetaObject::invokeMethod(m_view,"clicked",Q_ARG(QModelIndex,QModelIndex(item)));return;}
        if(round>0)if(auto *window=qobject_cast<CompetitionManagerWindow *>(m_view->window())){window->showResultJump(item,round);return;}
        QMetaObject::invokeMethod(m_view,open?"doubleClicked":"clicked",Q_ARG(QModelIndex,QModelIndex(item)));
        if(open && m_view && item.isValid()) QMetaObject::invokeMethod(m_view,"activated",Q_ARG(QModelIndex,QModelIndex(item)));
    });
}
void QuickTableModel::edit(int row,int column,const QVariant &value) {
    if(m_frozen)return;
    auto item=cell(row,column); if(!item.isValid() || !m_source) return;
    if((item.flags() & Qt::ItemIsUserCheckable) && item.data(Qt::CheckStateRole).isValid() && value.type()==QVariant::Bool) m_source->setData(item,value.toBool()?Qt::Checked:Qt::Unchecked,Qt::CheckStateRole);
    else if(item.flags() & Qt::ItemIsEditable) m_source->setData(item,value,Qt::EditRole);
}
void QuickTableModel::toggle(int row) {
    if(m_frozen)return;
    if(auto *tree=qobject_cast<QTreeView *>(m_view)) { const auto item=cell(row,0); tree->setExpanded(item,!tree->isExpanded(item)); }
}
void QuickTableModel::sort(int column,bool descending) { if(!m_frozen && m_source) m_source->sort(column,descending?Qt::DescendingOrder:Qt::AscendingOrder); }
void QuickTableModel::moveCurrent(int delta,int modifiers) {
    if(!m_view) return;
    int current=m_rows.indexOf(m_view->currentIndex().sibling(m_view->currentIndex().row(),0));
    select(qBound(0,current+delta,m_rows.size()-1),currentColumn(),modifiers);
}
void QuickTableModel::moveColumn(int delta) {
    if(!m_source || !m_view)return;
    const auto current=m_view->currentIndex();
    select(currentRow(),qBound(0,current.column()+delta,m_source->columnCount(current.parent())-1),0);
}
void QuickTableModel::toggleCurrent() {
    const auto current=cell(currentRow(),currentColumn());
    if((current.flags() & Qt::ItemIsUserCheckable) && current.data(Qt::CheckStateRole).isValid())edit(currentRow(),currentColumn(),current.data(Qt::CheckStateRole).toInt()!=Qt::Checked);
}
void QuickTableModel::selectAll() {
    if(m_frozen)return;
    if(!m_view || !multiple() || !m_view->selectionModel()) return;
    auto *selection=m_view->selectionModel(); selection->clearSelection();
    QItemSelection items;
    for(const auto &row:m_rows) items.select(row,row.sibling(row.row(),m_source->columnCount(row.parent())-1));
    selection->select(items,QItemSelectionModel::Select);
}
void QuickTableModel::setFilter(const QString &value) { if(m_filter==value) return;clearSelection();m_filter=value;rebuild(); }
int QuickTableModel::currentRow() const {
    if(!m_view) return -1;
    const auto index=m_view->currentIndex();return m_rows.indexOf(index.sibling(index.row(),0));
}
int QuickTableModel::currentColumn() const { return m_view?qMax(0,m_view->currentIndex().column()):0; }
void QuickTableModel::openCurrent() {
    if(!m_view) return;
    const auto index=m_view->currentIndex();
    select(m_rows.indexOf(index.sibling(index.row(),0)),index.column(),0,true);
}

void QuickTableModel::invalidateText() {
    if(!m_rows.isEmpty()) emit dataChanged(index(0),index(m_rows.size()-1));
    emit structureChanged();
}

void QuickTableModel::setFrozen(bool frozen) {
    if(frozen==m_frozen)return;
    if(frozen) {
        m_snapshot.clear();
        const auto roles=roleNames();
        for(int row=0;row<rowCount();++row) {
            QHash<int,QVariant> values;
            for(auto role=roles.cbegin();role!=roles.cend();++role)values.insert(role.key(),data(index(row),role.key()));
            m_snapshot.append(values);
        }
        m_frozen=true;
        emit frozenChanged();
    } else {
        m_frozen=false;m_snapshot.clear();sync();rebuild();emit frozenChanged();
    }
}

int QuickTableModel::selectedCount() const {
    if(!m_view || !m_view->selectionModel())return 0;
    QSet<QPersistentModelIndex> rows;
    for(const auto &index:m_view->selectionModel()->selectedIndexes())rows.insert(index.sibling(index.row(),0));
    return rows.size();
}
void QuickTableModel::clearSelection() {
    if(!m_frozen && m_view && m_view->selectionModel())m_view->selectionModel()->clearSelection();
}
