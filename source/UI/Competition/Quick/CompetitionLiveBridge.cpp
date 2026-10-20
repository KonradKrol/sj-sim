#include "CompetitionLiveBridge.h"
#include "../CompetitionManagerWindow.h"
#include "../../../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include <QApplication>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QTabWidget>
#include <QScrollArea>
#include <QLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTimer>
#include <QSettings>
#include <QBuffer>
#include <QBrush>
#include <QDialogButtonBox>
#include <QPersistentModelIndex>

namespace {
QString picture(const QPixmap &pixmap) {
    if (pixmap.isNull()) return {};
    QByteArray bytes; QBuffer buffer(&bytes); buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    return "data:image/png;base64," + QString::fromLatin1(bytes.toBase64());
}
QString decoration(const QVariant &v) {
    if (v.canConvert<QIcon>()) return picture(qvariant_cast<QIcon>(v).pixmap(32, 24));
    if (v.canConvert<QPixmap>()) return picture(qvariant_cast<QPixmap>(v));
    return {};
}
}

class CompetitionDialogFilter : public QObject {
public:
    explicit CompetitionDialogFilter(CompetitionManagerWindow *owner) : QObject(owner), owner(owner) { qApp->installEventFilter(this); }
    bool eventFilter(QObject *object, QEvent *event) override {
        auto *dialog = qobject_cast<QDialog *>(object);
        if (event->type() != QEvent::Show || !dialog || dialog == owner || dialog->property("liveQuickInstalled").toBool() || !CompetitionLiveBridge::quickEnabled(owner)) return false;
        bool belongs = false;
        for (QWidget *p = dialog->parentWidget(); p; p = p->parentWidget()) if (p == owner) belongs = true;
        if (!belongs || !dialog->layout()) return false;
        attach(dialog, false);
        return false;
    }
    static void attach(QDialog *dialog, bool competition) {
        dialog->setProperty("liveQuickInstalled", true);
        auto *legacy = new QWidget(dialog);
        legacy->setObjectName("competitionLegacyContent");
        legacy->setLayout(dialog->layout());
        auto *layout = new QVBoxLayout(dialog); layout->setContentsMargins(0, 0, 0, 0);
        auto *bridge = new CompetitionLiveBridge(dialog, legacy, competition);
        auto *view = new QQuickWidget(dialog); view->setObjectName("competitionLiveQuickView");
        bridge->m_view = view;
        view->setResizeMode(QQuickWidget::SizeRootObjectToView);
        view->rootContext()->setContextProperty("live", bridge);
        view->setSource(QUrl("qrc:/competition-quick/CompetitionLive.qml"));
        layout->addWidget(view);
        legacy->hide();
        if (view->status() == QQuickWidget::Error) { bridge->m_classic=true; view->hide(); layout->addWidget(legacy); legacy->show(); }
        dialog->setMinimumSize(480, 360);
        dialog->setStyleSheet("");
        bridge->refresh();
    }
    QPointer<CompetitionManagerWindow> owner;
};

void CompetitionLiveBridge::install(CompetitionManagerWindow *window) {
    new CompetitionDialogFilter(window);
    CompetitionDialogFilter::attach(window, true);
}
bool CompetitionLiveBridge::quickEnabled(QWidget *widget) {
    auto *bridge = widget ? widget->findChild<CompetitionLiveBridge *>(QString(), Qt::FindDirectChildrenOnly) : nullptr;
    return bridge && bridge->m_view && bridge->m_view->isVisible();
}
CompetitionLiveBridge::CompetitionLiveBridge(QDialog *dialog, QWidget *legacy, bool competition)
    : QObject(dialog), m_dialog(dialog), m_legacy(legacy), m_competition(competition) {
    setObjectName("competitionLiveBridge");
    auto *timer = new QTimer(this); timer->setInterval(180);
    connect(timer, &QTimer::timeout, this, [this] {
        // The legacy caller can retain a closed window after deleting its manager.
        // Never inspect models or jump data after the competition dialog closes.
        if(m_dialog && m_dialog->isVisible()) refresh();
    }); timer->start();
}
CompetitionLiveBridge::~CompetitionLiveBridge() {
    // Tear down bindings while the context object is still alive.
    if(m_view) { m_view->setSource(QUrl()); delete m_view; }
}
bool CompetitionLiveBridge::dark() const { return QSettings().value("competitionQuick/dark", false).toBool(); }
void CompetitionLiveBridge::setDark(bool value) { QSettings().setValue("competitionQuick/dark", value); emit themeChanged(); }
int CompetitionLiveBridge::identify(QWidget *widget) {
    auto it = m_ids.find(widget);
    if (it != m_ids.end()) return *it;
    const int id = ++m_nextId; m_ids.insert(widget, id); m_widgets.insert(id, widget);
    connect(widget, &QObject::destroyed, this, [this, widget, id] { m_ids.remove(widget); m_widgets.remove(id); });
    return id;
}
QVariantList CompetitionLiveBridge::layoutNodes(QLayout *layout) {
    QVariantList result;
    if (!layout) return result;
    for (int i = 0; i < layout->count(); ++i) {
        auto *item = layout->itemAt(i);
        if (item->widget()) { auto n = node(item->widget()); if (!n.isEmpty()) result.append(n); }
        else if (item->layout()) result.append(QVariantMap{{"kind", "group"}, {"children", layoutNodes(item->layout())}});
    }
    return result;
}
QVariantMap CompetitionLiveBridge::node(QWidget *w) {
    if (!w || w->isHidden()) return {};
    QVariantMap n{{"id", identify(w)}, {"name", w->objectName()}, {"enabled", w->isEnabled()}, {"tip", w->toolTip()}};
    if (auto *scroll = qobject_cast<QScrollArea *>(w)) return node(scroll->widget());
    if (auto *tabs = qobject_cast<QTabWidget *>(w)) {
        QStringList choices; for (int i=0;i<tabs->count();++i) choices << tabs->tabText(i);
        n["kind"]="tabs"; n["options"]=choices; n["value"]=tabs->currentIndex();
        // Tab pages are hidden by QTabWidget; inspect the active page's layout directly.
        n["children"]=layoutNodes(tabs->currentWidget() ? tabs->currentWidget()->layout() : nullptr);
    } else if (auto *view = qobject_cast<QAbstractItemView *>(w)) {
        n["kind"]="table"; auto *model=view->model(); QVariantList rows; QStringList headers;
        if(model && !m_observedModels.contains(model)) {
            m_observedModels.insert(model,true);
            auto dirty=[this] { m_tablesDirty=true; };
            connect(model,&QAbstractItemModel::dataChanged,this,dirty);
            connect(model,&QAbstractItemModel::modelReset,this,dirty);
            connect(model,&QAbstractItemModel::layoutChanged,this,dirty);
            connect(model,&QAbstractItemModel::rowsInserted,this,dirty);
            connect(model,&QAbstractItemModel::rowsRemoved,this,dirty);
            connect(model,&QObject::destroyed,this,[this,model] { m_observedModels.remove(model); m_tablesDirty=true; });
        }
        const int id=n.value("id").toInt();
        if(!m_tablesDirty && m_tables.contains(id)) return m_tables.value(id);
        if (model) {
            for(int c=0;c<model->columnCount();++c) headers << model->headerData(c,Qt::Horizontal).toString();
            std::function<void(QModelIndex,int)> append = [&](QModelIndex parent,int depth) {
                for(int r=0;r<model->rowCount(parent);++r) {
                    QVariantList cells;
                    const auto bg=model->index(r,0,parent).data(Qt::BackgroundRole);
                    const QColor color = bg.canConvert<QBrush>() ? qvariant_cast<QBrush>(bg).color() : qvariant_cast<QColor>(bg);
                    for(int c=0;c<model->columnCount(parent);++c) {
                        auto index=model->index(r,c,parent);
                        cells.append(QVariantMap{{"text",index.data().toString()}, {"icon",decoration(index.data(Qt::DecorationRole))}, {"background",color.isValid()?color.name():QString()}, {"tip",index.data(Qt::ToolTipRole).toString()}});
                    }
                    auto first=model->index(r,0,parent);
                    rows.append(QVariantMap{{"cells",cells},{"depth",depth},{"index",QVariant::fromValue(QPersistentModelIndex(first))},{"current",view->currentIndex().row()==r && view->currentIndex().parent()==parent}, {"branch",model->hasChildren(first)}, {"expanded",qobject_cast<QTreeView *>(view) && static_cast<QTreeView *>(view)->isExpanded(first)}});
                    if(model->hasChildren(first) && (!qobject_cast<QTreeView *>(view) || static_cast<QTreeView *>(view)->isExpanded(first))) append(first,depth+1);
                }
            }; append({},0);
        }
        n["headers"]=headers; n["rows"]=rows;
        m_tables.insert(id,n);
    } else if (auto *button=qobject_cast<QAbstractButton *>(w)) {
        n["kind"]=button->isCheckable()?"check":"button"; n["text"]=button->text().remove('&'); n["value"]=button->isChecked();
        QMenu *menu=nullptr; if(auto *tool=qobject_cast<QToolButton *>(w)) menu=tool->menu();
        if(menu) { QVariantList actions; for(auto *a:menu->actions()) actions.append(QVariantMap{{"text",a->text()},{"enabled",a->isEnabled()}}); n["actions"]=actions; }
    } else if (auto *spin=qobject_cast<QSpinBox *>(w)) {
        n["kind"]="number"; n["value"]=spin->value(); n["min"]=spin->minimum(); n["max"]=spin->maximum(); n["step"]=spin->singleStep(); n["decimals"]=0; n["suffix"]=spin->suffix();
    } else if (auto *spin=qobject_cast<QDoubleSpinBox *>(w)) {
        n["kind"]="number"; n["value"]=spin->value(); n["min"]=spin->minimum(); n["max"]=spin->maximum(); n["step"]=spin->singleStep(); n["decimals"]=spin->decimals(); n["suffix"]=spin->suffix();
    } else if (auto *combo=qobject_cast<QComboBox *>(w)) {
        n["kind"]="choice"; QStringList choices; for(int i=0;i<combo->count();++i) choices << combo->itemText(i); n["options"]=choices; n["value"]=combo->currentIndex();
    } else if (auto *slider=qobject_cast<QSlider *>(w)) {
        n["kind"]="slider"; n["value"]=slider->value(); n["min"]=slider->minimum(); n["max"]=slider->maximum();
    } else if (auto *label=qobject_cast<QLabel *>(w)) {
        n["kind"]="label"; n["text"]=label->text(); n["image"]=picture(label->pixmap(Qt::ReturnByValue));
    } else if (auto *line=qobject_cast<QLineEdit *>(w)) {
        n["kind"]="text"; n["value"]=line->text(); n["enabled"]=w->isEnabled()&&!line->isReadOnly();
    } else if(auto *text=qobject_cast<QTextEdit *>(w)) { n["kind"]="label"; n["text"]=text->toHtml(); }
    else {
        n["kind"]="group";
        if(auto *group=qobject_cast<QGroupBox *>(w)) n["text"]=group->title();
        if(w->layout()) n["children"]=layoutNodes(w->layout());
        else {
            QVariantList children;
            for(auto *child:w->findChildren<QWidget *>(QString(),Qt::FindDirectChildrenOnly)) { auto c=node(child); if(!c.isEmpty()) children.append(c); }
            n["children"]=children;
        }
    }
    return n;
}
void CompetitionLiveBridge::refresh() {
    if (!m_dialog || m_classic) return;
    QVariantList sections;
    if (m_competition) {
        auto section=[&](QString title, QStringList names) {
            QVariantList children;
            for(const auto &name:names) { auto n=node(m_legacy->findChild<QWidget *>(name)); if(!n.isEmpty()) children.append(n); }
            sections.append(QVariantMap{{"title",title},{"children",children}});
        };
        section(tr("Konkurs"), {"label_hillNameAndHSPoint","label_hillFlag","label_record","scrollAreaWidgetContents_competitionControls","pushButton_goToNext","toolButton_autoSimulationActions","toolButton_cancelActions","checkBox_liveCompetition","pushButton_sendResultsWebhook"});
        auto header=sections.first().toMap();
        auto *competitionWindow=qobject_cast<CompetitionManagerWindow *>(m_dialog.data());
        auto *manager=competitionWindow->getManager();
        QString stage=tr("Seria %1 / %2").arg(manager->getActualRound()).arg(manager->getCompetitionRules()->getRoundsReference().size());
        if(auto *team=dynamic_cast<TeamCompetitionManager *>(manager)) stage+=tr(" · Grupa %1").arg(team->getActualGroup());
        stage+=tr(" · Zawodnik %1 / %2").arg(qMin(manager->getActualStartListIndex()+1,manager->getStartListStatusesReference().size())).arg(manager->getStartListStatusesReference().size());
        header["stage"]=stage; sections[0]=header;
        QVariantList results; for(auto *view:m_legacy->findChildren<QAbstractItemView *>()) if(view->objectName()=="tableView_results" || (qobject_cast<QTreeView *>(view) && view->parentWidget()->objectName()=="panel_competition")) { auto n=node(view); if(!n.isEmpty()) results.append(n); }
        sections.append(QVariantMap{{"title",tr("Wyniki")},{"children",results}});
        section(tr("Lista startowa / KO"),{"tableView_KOGroupResults","listView_startList"});
        QVariantList details; for(auto *w:m_legacy->findChildren<JumperCompetitionResultsWidget *>()) { auto n=node(w); if(!n.isEmpty()) details.append(n); }
        sections.append(QVariantMap{{"title",tr("Szczegóły skoków")},{"children",details}});
        QVariantList classification; auto combo=node(m_legacy->findChild<QWidget *>("comboBox_virtualClassification")); if(!combo.isEmpty()) classification.append(combo);
        for(auto *w:m_legacy->findChildren<ClassificationResultsTableView *>()) { auto n=node(w); if(!n.isEmpty()) classification.append(n); }
        sections.append(QVariantMap{{"title",tr("Klasyfikacja na żywo")},{"children",classification}});
    } else sections.append(QVariantMap{{"title",m_dialog->windowTitle()},{"children",layoutNodes(m_legacy->layout())}});
    m_tablesDirty=false;
    if(sections!=m_sections) { emit aboutToChange(); m_sections=sections; emit changed(); }
}
void CompetitionLiveBridge::edit(int id,const QVariant &value) {
    auto *w=m_widgets.value(id).data(); if(!w || !w->isEnabled()) return;
    if(auto *v=qobject_cast<QSpinBox *>(w)) v->setValue(value.toInt());
    else if(auto *v=qobject_cast<QDoubleSpinBox *>(w)) v->setValue(value.toDouble());
    else if(auto *v=qobject_cast<QSlider *>(w)) v->setValue(value.toInt());
    else if(auto *v=qobject_cast<QComboBox *>(w)) { v->setCurrentIndex(value.toInt()); QMetaObject::invokeMethod(v,"activated",Q_ARG(int,value.toInt())); }
    else if(auto *v=qobject_cast<QTabWidget *>(w)) v->setCurrentIndex(value.toInt());
    else if(auto *v=qobject_cast<QLineEdit *>(w)) { v->setText(value.toString()); QMetaObject::invokeMethod(v,"editingFinished"); }
    m_tablesDirty=true;
    QTimer::singleShot(0,this,&CompetitionLiveBridge::refresh);
}
void CompetitionLiveBridge::activate(int id,int entry) {
    QPointer<QAbstractButton> button=qobject_cast<QAbstractButton *>(m_widgets.value(id));
    if(!button || !button->isEnabled()) return;
    // Dispatch outside the QML event stack: a simulation can open a nested modal
    // loop or close this window. Never destroy the emitting delegate mid-call.
    QTimer::singleShot(0,this,[this,button,entry] {
        if(!button) return;
        if(entry>=0) { auto *tool=qobject_cast<QToolButton *>(button); if(tool && tool->menu() && entry<tool->menu()->actions().size()) tool->menu()->actions()[entry]->trigger(); }
        else button->click();
        m_tablesDirty=true;
        refresh();
    });
}
void CompetitionLiveBridge::select(int id,int row,bool details) {
    auto *view=qobject_cast<QAbstractItemView *>(m_widgets.value(id)); if(!view || !view->model()) return;
    auto n=node(view); auto rows=n.value("rows").toList(); if(row<0 || row>=rows.size()) return;
    QModelIndex index=rows[row].toMap().value("index").value<QPersistentModelIndex>();
    view->setCurrentIndex(index);
    // Keep the delegate alive between the first and second click.
    if(!details) return;
    m_tablesDirty=true;
    if(details) { QMetaObject::invokeMethod(view,"doubleClicked",Q_ARG(QModelIndex,index));
        // Only a team member row fills the detail widget without showing it.
        // Start-list double clicks and team headings must not expose empty data.
        if(qobject_cast<QTreeView *>(view) && view->parentWidget()->objectName()=="panel_competition" && index.parent().isValid())
            for(auto *detail:m_legacy->findChildren<JumperCompetitionResultsWidget *>()) detail->show();
    }
    refresh();
}
void CompetitionLiveBridge::classic() {
    if(!m_view || m_classic) return;
    m_classic=true;
    m_view->hide(); m_dialog->layout()->addWidget(m_legacy); m_legacy->show();
    auto *back=new QToolButton(m_legacy); back->setText(tr("Widok Qt Quick"));
    m_legacy->layout()->addWidget(back);
    connect(back,&QToolButton::clicked,this,[this,back] { m_legacy->hide(); m_dialog->layout()->removeWidget(m_legacy); m_classic=false; m_view->show(); back->deleteLater(); m_tablesDirty=true; refresh(); });
}

void CompetitionLiveBridge::toggleTree(int id,int row) {
    auto *tree=qobject_cast<QTreeView *>(m_widgets.value(id)); if(!tree) return;
    auto rows=node(tree).value("rows").toList(); if(row<0 || row>=rows.size()) return;
    QModelIndex index=rows[row].toMap().value("index").value<QPersistentModelIndex>();
    tree->setExpanded(index,!tree->isExpanded(index)); m_tablesDirty=true; refresh();
}
