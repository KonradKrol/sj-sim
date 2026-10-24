#include "QuickUi.h"
#include "QuickView.h"
#include "QuickCalendar.h"
#include "QuickRoster.h"
#include "QuickRaceStats.h"
#include "../EditorWidgets/JumpersListEditorWindow.h"
#include "../EditorWidgets/TeamsEditing/TeamEditorWidget.h"
#include "../Seasons/CalendarEditor/CalendarEditorWidget.h"
#include "../Seasons/SimulationSaveManagerWindow.h"
#include "../mainwindow.h"
#include "../AuthorWindow.h"
#include "../Competition/CompetitionManagerWindow.h"
#include "../Competition/CompetitionConfigWindow.h"
#include "../ResultsShowing/JumpDataDetailedInfoWindow.h"
#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../EditorWidgets/ClassificationEditorWidget.h"
#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h"
#include "../DatabaseEditor/DatabaseItemsListView.h"
#include "../../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include "../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../competitions/CompetitionManagers/KORoundManager.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/CountryFlagsManager.h"
#include <QApplication>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QTabWidget>
#include <QToolBox>
#include <QStackedWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QGridLayout>
#include <QFormLayout>
#include <QBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QMenu>
#include <QQuickWidget>
#include <QQuickWindow>
#include <QQuickItem>
#include <QSettings>
#include <QTimer>
#include <QChartView>
#include <QDialogButtonBox>
#include <QProgressBar>
#include <QDateTimeEdit>
#include <QInputDialog>
#include <QValidator>
#include <QShortcut>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCheckBox>
#include <QRegularExpression>
#include <cmath>
#include <functional>
#include <algorithm>
QT_CHARTS_USE_NAMESPACE

QString quickPicture(const QPixmap &pixmap);
QString quickDecoration(const QVariant &value);
namespace {
QString standardCaption(int button) {
    switch(button) {
    case QMessageBox::Ok:return QuickBridge::tr("OK");
    case QMessageBox::Yes:return QuickBridge::tr("Tak");
    case QMessageBox::No:return QuickBridge::tr("Nie");
    case QMessageBox::Cancel:return QuickBridge::tr("Anuluj");
    case QMessageBox::Close:return QuickBridge::tr("Zamknij");
    case QMessageBox::Save:return QuickBridge::tr("Zapisz");
    case QMessageBox::SaveAll:return QuickBridge::tr("Zapisz wszystko");
    case QMessageBox::Discard:return QuickBridge::tr("Odrzuć");
    case QMessageBox::Apply:return QuickBridge::tr("Zastosuj");
    case QMessageBox::Reset:return QuickBridge::tr("Resetuj");
    case QMessageBox::RestoreDefaults:return QuickBridge::tr("Przywróć domyślne");
    case QMessageBox::Retry:return QuickBridge::tr("Ponów");
    case QMessageBox::Ignore:return QuickBridge::tr("Ignoruj");
    case QMessageBox::Abort:return QuickBridge::tr("Przerwij");
    case QMessageBox::YesToAll:return QuickBridge::tr("Tak dla wszystkich");
    case QMessageBox::NoToAll:return QuickBridge::tr("Nie dla wszystkich");
    case QMessageBox::Open:return QuickBridge::tr("Otwórz");
    case QMessageBox::Help:return QuickBridge::tr("Pomoc");
    default:return {};
    }
}
QString readableHtml(QString html) {
    html.remove(QRegularExpression("(?:color|background-color|font-family|font-size)\\s*:[^;\"]+;?",QRegularExpression::CaseInsensitiveOption));
    html.remove(QRegularExpression("color=[\"][^\"]*[\"]",QRegularExpression::CaseInsensitiveOption));
    return html;
}
}

void QuickNode::update(const QVariantMap &values,const QVariantList &children) {
    if(m_values!=values) { m_values=values; emit valuesChanged(); }
    if(m_children!=children) { m_children=children; emit childrenChanged(); }
}
QuickAppearance::QuickAppearance(QObject *parent) : QObject(parent) {
    QSettings settings;
    m_dark=settings.value("quickUi/dark",false).toBool();
    m_compactUi=settings.value("quickUi/compactUi",false).toBool();
    m_reducedMotion=settings.value("quickUi/reducedMotion",false).toBool();
    m_textScale=qBound(.75,settings.value("quickUi/textScale",1.).toDouble(),1.5);
}
void QuickAppearance::setDark(bool value) { if(value==m_dark) return; m_dark=value; QSettings().setValue("quickUi/dark",value); emit changed(); }
void QuickAppearance::setCompactUi(bool value) { if(value==m_compactUi)return; m_compactUi=value;QSettings().setValue("quickUi/compactUi",value);emit changed(); }
void QuickAppearance::setReducedMotion(bool value) { if(value==m_reducedMotion) return; m_reducedMotion=value; QSettings().setValue("quickUi/reducedMotion",value); emit changed(); }
void QuickAppearance::setTextScale(qreal value) { value=qBound(.75,value,1.5); if(qFuzzyCompare(value,m_textScale)) return; m_textScale=value; QSettings().setValue("quickUi/textScale",value); emit changed(); }

QuickBridge::QuickBridge(QWidget *window,QWidget *content,QuickAppearance *appearance,bool home)
    : QObject(window), m_window(window), m_content(content), m_appearance(appearance), m_root(new QuickNode(this)), m_home(home) {
    auto *timer=new QTimer(this); timer->setInterval(120);
    connect(timer,&QTimer::timeout,this,[this] { if(m_view && m_view->isVisible()) refresh(); });
    connect(window,&QWidget::windowTitleChanged,this,&QuickBridge::changed);
    if(auto *competition=qobject_cast<CompetitionManagerWindow *>(window))
        connect(competition,&CompetitionManagerWindow::liveStageChanged,this,[this](int stage){
            for(auto *table:m_tables)table->setFrozen(stage>0);
            refresh();
        });
    timer->start(); refresh();
}
void QuickBridge::setView(QQuickWidget *view) {
    m_view=view;
    connect(view->quickWindow(),&QQuickWindow::activeFocusItemChanged,this,&QuickBridge::focusChanged);
}
QQuickWidget *QuickBridge::view() const { return m_view; }
bool QuickBridge::editingText() const {
    auto *item=m_view?m_view->quickWindow()->activeFocusItem():nullptr;
    return item && (item->inherits("QQuickTextInput") || item->inherits("QQuickTextEdit"));
}
void QuickBridge::suspend() { if(m_view) m_view->setSource(QUrl()); }
void QuickBridge::resume() {
    if(m_view && m_view->source().isEmpty()) { refresh();QuickView::load(m_view,QUrl(m_view->property("quickSource").toString())); }
}
QuickBridge::~QuickBridge() {
    // Drop the scene while the context and table adapters are still valid.
    if(m_view) { m_view->setSource(QUrl()); delete m_view; }
}
QString QuickBridge::title() const {
    if(!m_window)return {};
    const QHash<QString,QString> titles{{"JumpersFormGeneratorConfigWindow",tr("Generator formy")},{"JumpersListEditorWindow",tr("Lista zawodników")},{"CompetitionConfigWindow",tr("Konkurs")},{"CompetitionManagerWindow",tr("Konkurs")},{"SingleJumpsConfigWindow",tr("Pojedyncze skoki")},{"AppSettingsWindow",tr("Ustawienia")},{"DatabaseEditorWindow",tr("Baza danych")},{"SimulationSavesWindow",tr("Zapisy")},{"SimulationSaveManagerWindow",tr("Sezon")},{"NewSimulationSaveConfigurationWindow",tr("Nowy zapis")}};
    if(profile()=="AuthorWindow")return AuthorWindow::tr("O autorze");
    if(profile()=="KOGroupEditorWidget")return tr("Grupa KO");
    if(profile()=="QProgressDialog" && m_window->windowTitle().isEmpty())return tr("Postęp");
    return titles.value(profile(),m_window->windowTitle());
}
QString QuickBridge::profile() const { return m_home ? "home" : m_window ? QString::fromLatin1(m_window->metaObject()->className()) : QString(); }
QString QuickBridge::version() const { return appVersion; }
QVariantMap QuickBridge::library() const {
    return {{"jumpers",GlobalDatabase::get()->getEditableGlobalJumpers().size()},
            {"hills",GlobalDatabase::get()->getEditableGlobalHills().size()}};
}
int QuickBridge::idFor(QObject *object) {
    if(!object) return -1;
    if(m_ids.contains(object)) return m_ids.value(object);
    const int id=++m_nextId; m_ids.insert(object,id); m_objects.insert(id,object);
    auto *widget=qobject_cast<QWidget *>(object);
    if(auto *spin=qobject_cast<QSpinBox *>(object))connect(spin,QOverload<int>::of(&QSpinBox::valueChanged),this,&QuickBridge::deferRefresh);
    else if(auto *spin=qobject_cast<QDoubleSpinBox *>(object))connect(spin,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&QuickBridge::deferRefresh);
    else if(auto *choice=qobject_cast<QComboBox *>(object))connect(choice,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&QuickBridge::deferRefresh);
    else if(auto *input=qobject_cast<QLineEdit *>(object))connect(input,&QLineEdit::textChanged,this,&QuickBridge::deferRefresh);
    else if(auto *button=qobject_cast<QAbstractButton *>(object))connect(button,&QAbstractButton::toggled,this,&QuickBridge::deferRefresh);
    connect(object,&QObject::destroyed,this,[this,object,id,widget] {
        auto *entry=m_nodes.take(object);
        if(entry) entry->deleteLater();
        m_captions.remove(widget);m_windUndo.remove(object);
        m_ids.remove(object); m_objects.remove(id);
    });
    return id;
}
QuickNode *QuickBridge::identity(QObject *object) {
    idFor(object);
    if(!m_nodes.contains(object)) m_nodes.insert(object,new QuickNode(this));
    return m_nodes.value(object);
}
QVariantList QuickBridge::childNodes(QLayout *layout) {
    QVariantList children;
    if(!layout) return children;
    if(auto *form=qobject_cast<QFormLayout *>(layout)) {
        for(int row=0;row<form->rowCount();++row) {
            // Qt also returns a spanning item for FieldRole. Project it once,
            // before handling the separate label/field cells.
            if(auto *item=form->itemAt(row,QFormLayout::SpanningRole)) {
                auto *entry=item->widget()?node(item->widget()):layoutNode(item->layout());
                if(entry)children.append(QVariant::fromValue(entry));
                continue;
            }
            auto *labelItem=form->itemAt(row,QFormLayout::LabelRole);
            auto *fieldItem=form->itemAt(row,QFormLayout::FieldRole);
            if(labelItem && fieldItem && labelItem->widget() && (fieldItem->widget() || fieldItem->layout())) {
                auto *label=qobject_cast<QLabel *>(labelItem->widget());
                if(fieldItem->widget()) m_captions.insert(fieldItem->widget(),label ? label->text().remove('&') : QString());
                auto *field=fieldItem->widget()?node(fieldItem->widget()):layoutNode(fieldItem->layout());
                if(field) {
                    auto values=field->values();
                    values["label"]=label ? label->text().remove('&') : QString();
                    field->update(values,field->children()); children.append(QVariant::fromValue(field));
                }
            } else for(auto role:{QFormLayout::LabelRole,QFormLayout::FieldRole}) {
                if(auto *item=form->itemAt(row,role)) {
                    auto *entry=item->widget()?node(item->widget()):layoutNode(item->layout());
                    if(entry) children.append(QVariant::fromValue(entry));
                }
            }
        }
    } else if(auto *grid=qobject_cast<QGridLayout *>(layout)) {
        // Preserve each labelled row as a unit when a dense desktop grid stacks.
        for(int row=0;row<grid->rowCount();++row) {
            QVariantList entries;
            for(int i=0;i<grid->count();++i) {
                int r,c,rs,cs; grid->getItemPosition(i,&r,&c,&rs,&cs);
                if(r!=row) continue;
                auto *item=grid->itemAt(i);
                auto *entry=item->widget()?node(item->widget()):layoutNode(item->layout());
                if(entry) entries.append(QVariant::fromValue(entry));
            }
            if(entries.isEmpty()) continue;
            const QByteArray key=QByteArray("quickRow")+QByteArray::number(row);
            auto *rowObject=layout->property(key).value<QObject *>();
            if(!rowObject) { rowObject=new QObject(layout); layout->setProperty(key,rowObject?QVariant::fromValue(rowObject):QVariant()); }
            auto *entry=identity(rowObject);
            entry->update({{"id",idFor(rowObject)}, {"kind","row"}},entries);
            children.append(QVariant::fromValue(entry));
        }
    } else {
        for(int i=0;i<layout->count();++i) {
            auto *item=layout->itemAt(i);
            auto *entry=item->widget()?node(item->widget()):layoutNode(item->layout());
            if(entry) children.append(QVariant::fromValue(entry));
        }
    }
    return children;
}
QuickNode *QuickBridge::layoutNode(QLayout *layout) {
    if(!layout) return nullptr;
    auto *entry=identity(layout);
    const auto children=childNodes(layout);
    QString kind="column";
    if(qobject_cast<QFormLayout *>(layout)) kind="form";
    if(profile()=="SimulationSaveManagerWindow" && layout->objectName()=="formLayout") kind="row";
    if(auto *box=qobject_cast<QBoxLayout *>(layout);box && (box->direction()==QBoxLayout::LeftToRight || box->direction()==QBoxLayout::RightToLeft)) kind="row";
    entry->update({{"id",idFor(layout)}, {"kind",kind}},children);
    return entry;
}
QVariantList QuickBridge::widgetChildren(QWidget *widget) {
    if(widget->layout()) return {QVariant::fromValue(layoutNode(widget->layout()))};
    QVariantList children;
    for(auto *child:widget->findChildren<QWidget *>(QString(),Qt::FindDirectChildrenOnly))
        if(auto *entry=node(child)) children.append(QVariant::fromValue(entry));
    return children;
}
QuickNode *QuickBridge::node(QWidget *widget,bool force) {
    if(!widget || (widget->isHidden() && !force) || qobject_cast<QQuickWidget *>(widget)) return nullptr;
    if(!force && widget->property("quickMenuOnly").toBool()) return nullptr;
    if(auto *scroll=qobject_cast<QScrollArea *>(widget)) return node(scroll->widget(),true);
    auto *entry=identity(widget);
    QVariantMap data{{"id",idFor(widget)}, {"name",widget->objectName()}, {"kind","column"},
                     {"enabled",widget->isEnabled()}, {"available",!widget->isHidden() || widget->property("quickExposePage").toBool()}, {"tip",widget->toolTip()}, {"label",m_captions.value(widget,widget->accessibleName())}};
    QVariantList shortcuts;
    for(auto *action:widget->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly)) {
        const QString sequence=action->shortcut().toString(QKeySequence::PortableText);
        if(sequence.isEmpty()) continue;
        QString text=action->text().remove('&');
        if(text.isEmpty()) text=sequence.contains("Up")?tr("W górę"):sequence.contains("Down")?tr("W dół"):sequence.endsWith("D")?tr("Usuń"):sequence.endsWith("A")?tr("Dodaj"):sequence.endsWith("E")?tr("Edytuj"):sequence.endsWith("F")?tr("Wypełnij wszystkie"):tr("Szczegóły");
        shortcuts.append(QVariantMap{{"id",idFor(action)},{"text",text},{"sequence",sequence},{"enabled",action->isEnabled()}});
    }
    data["shortcuts"]=shortcuts;
    data["menuActions"]=widget->property("quickMenuActions").toBool();
    // The classification editor has explicit actions for each points table.
    // Keep its keyboard shortcuts without rendering another button row.
    if(qobject_cast<ClassificationEditorWidget *>(widget))data["menuActions"]=true;
    if(auto *list=qobject_cast<DatabaseItemsListView *>(widget)) {
        QVariantList actions;
        for(const auto &value:shortcuts) {
            auto action=value.toMap();const auto sequence=action.value("sequence").toString();
            if((sequence.endsWith("A") && !list->getAllowInserting()) || (sequence.endsWith("D") && !list->getAllowRemoving()) || ((sequence.endsWith("Up") || sequence.endsWith("Down")) && !list->getAllowMoving()))continue;
            action["insert"]=sequence.endsWith("A");actions.append(action);
        }
        widget->setProperty("quickTableActions",actions);data["menuActions"]=true;
    }
    QVariantList children;
    if(qobject_cast<TeamEditorWidget *>(widget))return rosterNode(widget);
    if(auto *calendar=qobject_cast<CalendarEditorWidget *>(widget)) {
        auto *backend=m_calendars.value(calendar);
        if(!backend){backend=new QuickCalendar(calendar,this);m_calendars.insert(calendar,backend);connect(backend,&QuickCalendar::changed,this,&QuickBridge::deferRefresh);connect(calendar,&QObject::destroyed,this,[this,calendar,backend]{m_calendars.remove(calendar);backend->deleteLater();});}
        backend->sync();data["editRequest"]=calendar->property("quickEditRequest").toInt();data["kind"]="calendar";data["enabled"]=true;data["backend"]=QVariant::fromValue(static_cast<QObject *>(backend));
        data["rulesEditor"]=QVariant::fromValue(node(backend->rulesEditor(),true));
        data["classificationEditor"]=QVariant::fromValue(node(backend->classificationEditor(),true));
    } else if(auto *wind=qobject_cast<WindsGeneratorSettingsEditorWidget *>(widget)) {
        data["kind"]="wind";data["kPoint"]=wind->getKPoint();
        if(entry->values().value("kPoint").toDouble()!=wind->getKPoint()) { m_windUndo.remove(wind);wind->setProperty("quickWindSector",0); }
        data["selectedSector"]=wind->property("quickWindSector").toInt();
        data["canUndo"]=m_windUndo.contains(wind);
        if(auto *box=wind->findChild<QToolBox *>("toolBox"))for(int i=0;i<box->count();++i) {
            auto *page=box->widget(i);auto *sector=identity(page);
            QVariantMap values{{"number",i+1}};
            const auto range=WindsGenerator::getRangeOfWindSector(i+1,wind->getKPoint());
            values["range"]=range.second<0?QString("%1+ m").arg(range.first):QString("%1–%2 m").arg(range.first).arg(range.second);
            for(const auto &field:QList<QPair<QString,QString>>{{"strength","doubleSpinBox_baseWindStrength"},{"direction","doubleSpinBox_baseWindDirection"},{"strengthVariation","doubleSpinBox_windStrengthChange"},{"directionVariation","doubleSpinBox_windDirectionChange"}})
                if(auto *input=page->findChild<QDoubleSpinBox *>(field.second))values[field.first]=node(input,true)->values();
            if(auto *details=page->findChild<CharacteristicsEditor *>())values["details"]=QVariant::fromValue(node(details,true));
            sector->update(values);children.append(QVariant::fromValue(sector));
        }
        if(auto *submit=wind->findChild<QPushButton *>("pushButton_submit",Qt::FindDirectChildrenOnly))data["submit"]=QVariant::fromValue(node(submit,true));
    } else if(auto *tabs=qobject_cast<QTabWidget *>(widget)) {
        QStringList options; QVariantList enabled;
        for(int i=0;i<tabs->count();++i) { options<<tabs->tabText(i).remove('&'); enabled<<tabs->isTabEnabled(i); }
        data["kind"]="tabs"; data["options"]=options; data["optionEnabled"]=enabled; data["value"]=tabs->currentIndex();
        if(auto *child=node(tabs->currentWidget(),true)) children.append(QVariant::fromValue(child));
    } else if(auto *box=qobject_cast<QToolBox *>(widget)) {
        QStringList options; QVariantList enabled;
        for(int i=0;i<box->count();++i) { options<<box->itemText(i).remove('&'); enabled<<box->isItemEnabled(i); }
        data["kind"]="tabs"; data["options"]=options; data["optionEnabled"]=enabled; data["value"]=box->currentIndex();
        if(auto *child=node(box->currentWidget(),true)) children.append(QVariant::fromValue(child));
    } else if(auto *stack=qobject_cast<QStackedWidget *>(widget)) {
        if(auto *child=node(stack->currentWidget(),true)) children.append(QVariant::fromValue(child));
    } else if(auto *split=qobject_cast<QSplitter *>(widget)) {
        data["kind"]="split";
        QVariantList weights;for(int value:split->sizes()) weights.append(qMax(120,value));data["weights"]=weights;
        for(int i=0;i<split->count();++i) if(auto *child=node(split->widget(i))) children.append(QVariant::fromValue(child));
    } else if(auto *view=qobject_cast<QAbstractItemView *>(widget)) {
        data["kind"]="table";data["follow"]=widget->objectName()=="listView_startList";
        data["tableActions"]=view->parentWidget()?view->parentWidget()->property("quickTableActions"):QVariant();
        if(!m_tables.contains(view)) {
            auto *table=new QuickTableModel(view,this); m_tables.insert(view,table);
            connect(view,&QObject::destroyed,this,[this,view,table] { m_tables.remove(view); table->deleteLater(); });
        }
        m_tables.value(view)->sync();
        data["table"]=QVariant::fromValue(static_cast<QObject *>(m_tables.value(view)));
    } else if(auto *chart=qobject_cast<QChartView *>(widget)) {
        data["kind"]="chart"; data["chart"]=QVariant::fromValue(static_cast<QObject *>(chart->chart()));
    } else if(auto *button=qobject_cast<QAbstractButton *>(widget)) {
        data["kind"]=button->isCheckable()?"check":"button";
        data["text"]=button->text().remove('&'); data["value"]=button->isChecked();
        data["icon"]=quickDecoration(button->icon());
        if(data["text"].toString().isEmpty()) data["text"]=button->toolTip();
        if(data["text"].toString().isEmpty()) {
            const QString name=button->objectName().toLower();
            data["text"]=name.contains("add")?tr("Dodaj"):name.contains("remove")?tr("Usuń"):name.contains("up")?tr("W górę"):name.contains("down")?tr("W dół"):tr("Wybierz");
        }
        QMenu *menu=nullptr;
        if(auto *tool=qobject_cast<QToolButton *>(button)) menu=tool->menu();
        if(auto *push=qobject_cast<QPushButton *>(button)) { menu=push->menu();
            if(auto *box=qobject_cast<QDialogButtonBox *>(push->parentWidget())) { const auto role=box->buttonRole(push); data["primary"]=role==QDialogButtonBox::AcceptRole || role==QDialogButtonBox::YesRole; } }
        const auto name=widget->objectName().toLower();
        data["primary"]=data.value("primary").toBool() || name.contains("submit") || name=="pushbutton_generate" || name=="pushbutton_jump" || name=="pushbutton_gotonext" || name=="pushbutton_ok" || name=="pushbutton_competitionconfig";
        if(menu) {
            QVariantList actions;
            for(auto *action:menu->actions()) actions.append(QVariantMap{{"id",idFor(action)},{"text",action->text().remove('&')},{"enabled",action->isEnabled()},{"separator",action->isSeparator()}});
            data["actions"]=actions;
        }
    } else if(auto *spin=qobject_cast<QSpinBox *>(widget)) {
        data["kind"]="number"; data["value"]=spin->value(); data["minimum"]=spin->minimum(); data["maximum"]=spin->maximum(); data["step"]=spin->singleStep(); data["decimals"]=0; data["specialText"]=spin->specialValueText(); data["suffix"]=spin->suffix(); data["prefix"]=spin->prefix(); data["enabled"]=spin->isEnabled()&&!spin->isReadOnly();
    } else if(auto *spin=qobject_cast<QDoubleSpinBox *>(widget)) {
        data["kind"]="number"; data["value"]=spin->value(); data["minimum"]=spin->minimum(); data["maximum"]=spin->maximum(); data["step"]=spin->singleStep(); data["decimals"]=spin->decimals(); data["suffix"]=spin->suffix(); data["prefix"]=spin->prefix(); data["enabled"]=spin->isEnabled()&&!spin->isReadOnly();
    } else if(auto *combo=qobject_cast<QComboBox *>(widget)) {
        QStringList options,images; for(int i=0;i<combo->count();++i) { options<<combo->itemText(i);images<<quickDecoration(QVariant::fromValue(combo->itemIcon(i))); }
        data["optionImages"]=images;
        data["searchable"]=widget->property("quickSearchableChoice").toBool();
        data["kind"]="choice"; data["options"]=options; data["value"]=combo->currentIndex(); data["editable"]=combo->isEditable(); data["text"]=combo->currentText();
    } else if(auto *slider=qobject_cast<QSlider *>(widget)) {
        data["kind"]="slider"; data["value"]=slider->value(); data["minimum"]=slider->minimum(); data["maximum"]=slider->maximum(); data["step"]=slider->singleStep();
    } else if(auto *line=qobject_cast<QLineEdit *>(widget)) {
        data["kind"]="text"; data["value"]=line->text(); data["placeholder"]=line->placeholderText(); data["readOnly"]=line->isReadOnly(); data["password"]=line->echoMode()!=QLineEdit::Normal; data["maxLength"]=line->maxLength();
    } else if(auto *label=qobject_cast<QLabel *>(widget)) {
        data["kind"]="label"; data["text"]=readableHtml(label->text()); data["image"]=quickPicture(label->pixmap(Qt::ReturnByValue));
        data["heading"]=widget->objectName().contains("title",Qt::CaseInsensitive);
        if(label->text().isEmpty() && data["image"].toString().isEmpty()) return nullptr;
    } else if(auto *text=qobject_cast<QTextEdit *>(widget)) {
        data["kind"]=text->isReadOnly()?"richtext":"multiline"; data["text"]=text->isReadOnly()?readableHtml(text->toHtml()):text->toPlainText();
    } else if(auto *text=qobject_cast<QPlainTextEdit *>(widget)) {
        data["kind"]=text->isReadOnly()?"richtext":"multiline"; data["text"]=text->toPlainText();
    } else if(auto *group=qobject_cast<QGroupBox *>(widget)) {
        data["kind"]="panel"; data["text"]=group->title().remove('&'); data["checkable"]=group->isCheckable(); data["value"]=group->isChecked(); children=widgetChildren(widget);
    } else if(auto *progress=qobject_cast<QProgressBar *>(widget)) {
        data["kind"]="progress"; data["value"]=progress->value(); data["minimum"]=progress->minimum(); data["maximum"]=progress->maximum(); data["text"]=progress->text();
    } else if(auto *date=qobject_cast<QDateTimeEdit *>(widget)) {
        data["kind"]="date"; data["value"]=date->dateTime().toString(Qt::ISODate); data["text"]=date->text();
    } else children=widgetChildren(widget);
    const QHash<QString,QString> editors{
        {"JumperEditorWidget",tr("Profil zawodnika")},{"HillEditorWidget",tr("Parametry skoczni")},
        {"CompetitionRulesEditorWidget",tr("Zasady konkursu")},{"WindsGeneratorSettingsEditorWidget",tr("Warunki wietrzne")},
        {"InrunSnowGeneratorSettingsEditorWidget",tr("Śnieg na rozbiegu")},{"ClassificationEditorWidget",tr("Klasyfikacja")},
        {"PointsForPlacesPresetEditorWidget",tr("Punkty za miejsca")},{"CompetitionInfoEditorWidget",tr("Konkurs w kalendarzu")},
        {"RoundInfoEditorWidget",tr("Zasady serii")},{"TeamEditorWidget",tr("Skład drużyny")}};
    const QString editor=editors.value(QString::fromLatin1(widget->metaObject()->className()));
    if(!editor.isEmpty()) { if(data.value("kind")!="wind")data["kind"]="panel"; data["text"]=editor; }
    if(widget->inherits("JumperEditorWidget") || widget->inherits("HillEditorWidget")) {
        const bool jumper=widget->inherits("JumperEditorWidget");
        auto *form=widget->findChild<QFormLayout *>(jumper?"formLayout_jumperInfo":"formLayout_input");
        auto *characteristics=widget->findChild<QLayout *>("verticalLayout_characteristicsEditor");
        if(form && characteristics) {
            const auto fields=childNodes(form);QVariantList profile,skills,points;
            std::function<QString(QLayout *)> inputName=[&](QLayout *layout) -> QString {
                for(int i=0;i<layout->count();++i) {
                    auto *item=layout->itemAt(i);
                    if(auto *widget=item->widget();widget && !qobject_cast<QLabel *>(widget))return widget->objectName();
                    if(item->layout()) { const auto name=inputName(item->layout());if(!name.isEmpty())return name; }
                }
                return {};
            };
            for(const auto &value:fields)if(auto *field=value.value<QuickNode *>()) {
                auto *object=m_objects.value(field->values().value("id").toInt()).data();
                const QString name=qobject_cast<QLayout *>(object)?inputName(qobject_cast<QLayout *>(object)):object?object->objectName():QString();
                if(jumper) {
                    if(name=="lineEdit_name" || name=="lineEdit_surname" || name=="lineEdit_countryCode" || name=="lineEdit_img" || name=="doubleSpinBox_personalBest"
                        || name.startsWith("jumperBirth") || name=="jumperAge")profile.append(value);
                    else skills.append(value);
                } else {
                    if(name.contains("Points",Qt::CaseInsensitive) || name.contains("points",Qt::CaseInsensitive))points.append(value);
                    else if(name=="doubleSpinBox_balance" || name=="doubleSpinBox_distanceMultiplier")skills.append(value);
                    else profile.append(value);
                }
            }
            auto page=[&](const QString &key,const QVariantList &fields) {
                const QString id=QString("editor-fields-%1-%2").arg(idFor(widget)).arg(key);
                auto *entry=m_groups.value(id,nullptr);if(!entry){entry=new QuickNode(this);m_groups.insert(id,entry);}
                entry->update({{"kind","form"}},fields);return QVariant::fromValue(entry);
            };
            QVariantList pages{page("profile",profile)};QStringList labels{tr("Dane")};
            if(!points.isEmpty()){pages.append(page("points",points));labels.append(tr("Punktacja"));}
            if(!skills.isEmpty()){pages.append(page("skills",skills));labels.append(jumper?tr("Umiejętności"):tr("Parametry"));}
            pages.append(QVariant::fromValue(layoutNode(characteristics)));labels.append(tr("Cechy"));
            auto *tabs=sectionTabs(QString("editor-%1").arg(idFor(widget)),labels,pages);
            children={QVariant::fromValue(tabs)};
            if(auto *preview=widget->findChild<QWidget *>("label_imgPreview"))if(auto *entry=node(preview))children.append(QVariant::fromValue(entry));
            if(auto *submit=widget->findChild<QWidget *>("pushButton_submit"))if(auto *entry=node(submit))children.append(QVariant::fromValue(entry));
        }
    }
    if(widget->objectName()=="label_title" || widget->objectName()=="label_title_2") {
        if(data.value("text").toString().simplified()==(m_window?m_window->windowTitle().simplified():QString())) return nullptr;
    }
    if(widget->objectName()=="textEdit_shortcuts") return nullptr;
    if(data.value("kind")=="column" && children.isEmpty() && shortcuts.isEmpty()) return nullptr;
    entry->update(data,children); return entry;
}
void QuickBridge::refresh() {
    if(!m_window || !m_content || m_refreshing) return;
    m_refreshing=true;
    if(auto *author=qobject_cast<AuthorWindow *>(m_window)) {
        const auto context=author->details();
        if(context!=m_context){m_context=context;emit changed();}
        m_refreshing=false;
        return;
    }
    if(!m_home) {
        auto *root=layoutNode(m_content->layout());
        compose(root);
    }
    for(auto named=m_named.cbegin();named!=m_named.cend();++named) if(named.value()) node(named.value(),true);
    QVariantList actions;
    for(auto *action:m_window->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly)) {
        if(action->shortcut().isEmpty()) continue;
        if(profile()=="JumpersListEditorWindow")continue;
        auto *owner=qobject_cast<QWidget *>(action->parent());
        const int id=idFor(action);
        QString text=action->text().remove('&');
        const QString sequence=action->shortcut().toString(QKeySequence::PortableText);
        if(text.isEmpty()) text=sequence.contains("Up")?tr("W górę"):sequence.contains("Down")?tr("W dół"):sequence.endsWith("D")?tr("Usuń"):sequence.endsWith("A")?tr("Dodaj"):sequence.endsWith("E")?tr("Edytuj"):sequence.endsWith("F")?tr("Wypełnij wszystkie"):tr("Szczegóły");
        actions.append(QVariantMap{{"id",id},{"owner",owner?idFor(owner):-1},{"text",text},{"sequence",sequence},{"enabled",action->isEnabled() && (!owner || !owner->isHidden())}});
    }
    if(actions!=m_actions) { m_actions=actions; emit changed(); }
    if(auto *configuration=qobject_cast<CompetitionConfigWindow *>(m_window)) {
        QVariantMap context;context["suggestedGate"]=configuration->suggestedGate();if(auto *gate=m_content->findChild<QSpinBox *>("spinBox_startGate"))context["startingGate"]=gate->value();
        if(context!=m_context){m_context=context;emit changed();}
    }
    if(auto *season=qobject_cast<SimulationSaveManagerWindow *>(m_window)) {
        QVariantMap context;
        if(auto *next=season->getSimulationSave()->getNextCompetition();next && next->getHill()) {
            context["nextFlag"]=quickPicture(CountryFlagsManager::getFlagPixmap(next->getHill()->getCountryCode().toLower()));context["nextHill"]=next->getHill()->getName();context["nextHS"]=next->getHill()->getHSPoint();context["nextTeam"]=next->getRules().getCompetitionType()==CompetitionRules::Team;
            if(auto *label=m_content->findChild<QLabel *>("label_serieTypeAndTrainingIndex"))context["nextType"]=label->text();
        }
        if(context!=m_context){m_context=context;emit changed();}
    }
    if(auto *window=qobject_cast<CompetitionManagerWindow *>(m_window);window && window->getManager()) {
        auto *manager=window->getManager();
        QVariantMap context{{"complete",manager->checkCompetitionEnd()},{"roundComplete",manager->checkRoundEnd()},{"liveStage",window->getLiveStage()},{"round",manager->getActualRound()},{"roundCount",manager->effectiveRoundsCount()},{"qualifications",manager->getCompetitionInfo()->getSerieType()==CompetitionInfo::Qualifications},
                            {"athlete",qMin(manager->getActualStartListIndex()+1,manager->getStartListStatusesReference().size())},{"athleteCount",manager->getStartListStatusesReference().size()}};
        if(auto *team=dynamic_cast<TeamCompetitionManager *>(manager))context["group"]=team->getActualGroup();
        if(auto *hill=manager->getCompetitionInfo()->getHill())context["hillFlag"]=quickPicture(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()));
        double wind=0;if(auto *label=m_content->findChild<QLabel *>("label_actualAvgWind")){auto value=label->text();value.remove(QRegularExpression("[^0-9.,+−-]"));value.replace(',', '.');value.replace(QChar(0x2212),'-');wind=value.toDouble();}context["windValue"]=wind;
        if(window->getLiveStage()>0 && window->getLiveStage()<5) {
            if(m_context.value("round")==context.value("round"))context["biggestAdvancement"]=m_context.value("biggestAdvancement");
        } else context["biggestAdvancement"]=QuickRaceStats::biggestAdvancement(*manager->getResults(),manager->getActualRound(),dynamic_cast<TeamCompetitionManager *>(manager)?manager->getCompetitionRules()->getJumpersInTeamCount():1,StartListCompetitorStatus::remainingJumpers(manager->getStartListStatusesReference()));

        if(manager->getActualRound()>1 && !manager->checkRoundEnd() && !manager->checkCompetitionEnd()) {
            auto *team=dynamic_cast<TeamCompetitionManager *>(manager);
            auto *ranked=team?manager->getResults()->getResultOfTeam(team->getActualTeam()):manager->getResults()->getResultOfIndividualJumper(manager->getActualJumper());
            const int count=(manager->getActualRound()-1)*(team?manager->getCompetitionRules()->getJumpersInTeamCount():1);
            if(ranked) {
                const auto standing=QuickRaceStats::previousStanding(*manager->getResults(),*ranked,count);
                context["previousPosition"]=standing.value("position");
                QVector<const CompetitionSingleResult *> rivals;
                if(team)for(auto *entry:team->getActualRoundTeamsReference())rivals.append(manager->getResults()->getResultOfTeam(entry));
                else for(const auto &entry:manager->getStartListStatusesReference())rivals.append(manager->getResults()->getResultOfIndividualJumper(entry.getJumper()));
                const auto gap=QuickRaceStats::roundStartGap(*ranked,rivals,manager->getActualRound());
                if(gap.contains("gap"))context["currentGap"]=gap.value("gap");
                if(auto *individual=dynamic_cast<IndividualCompetitionManager *>(manager);individual && individual->getKOManager() && individual->getKOManager()->getActualGroup()) {
                    rivals.clear();for(auto *jumper:individual->getKOManager()->getActualGroup()->getJumpersReference())rivals.append(manager->getResults()->getResultOfIndividualJumper(jumper));
                    const auto groupGap=QuickRaceStats::roundStartGap(*ranked,rivals,manager->getActualRound());
                    if(groupGap.contains("gap"))context["groupGap"]=groupGap.value("gap");
                }
            }
        }
        if(window->getLiveStage()>0 && window->getLiveStage()<5)
            for(const auto &key:{"currentGap","groupGap"}) {if(m_context.contains(key))context[key]=m_context.value(key);else context.remove(key);}
        if(context!=m_context) { m_context=context;emit changed(); }
        QVariantMap summary;
        QWidget *selected=nullptr;
        for(auto *child:m_content->findChildren<QWidget *>())if(child->inherits("JumperCompetitionResultsWidget")) { selected=child;break; }
        if(selected && selected->isVisibleTo(m_content)) {
            for(const auto &field:QList<QPair<QString,QString>>{
                {"name","label_nameAndSurname"},{"distance","label_distance"},{"points","label_points"},{"wind","label_averageWind"},
                {"gate","label_gate"},{"gatePoints","label_gatePoints"},{"windPoints","label_windPoints"},{"compensation","label_totalCompensation"},
                {"judgesText","label_judges"},{"judgesPoints","label_judgesPoints"},{"landing","label_landingType"},
                {"position","label_actualPosition"},{"totalPoints","label_pointsSum"},{"roundPosition","label_positionInRound"},{"groupPosition","label_positionInGroup"},{"competitionPosition","label_positionInCompetition"},
                {"takeoff","label_takeoffRating"},{"flight","label_flightRating"},{"random","label_random"},{"snow","label_snowRain"},
                {"dsq","label_DSQ"},{"hs95","label_beats95HSPercents"}}) {
                for(auto *label:selected->findChildren<QLabel *>(field.second))if(label->isVisibleTo(m_content)) { summary[field.first]=label->text();break; }
            }
            if(auto *flag=selected->findChild<QLabel *>("label_flag");flag && flag->isVisibleTo(m_content))if(auto *entry=node(flag))summary["flag"]=entry->values().value("image");
            const auto rank=selected->property("quickRankSummary").toMap();
            if(summary.contains("position")) {
                summary["positionNumber"]=rank.value("positionNumber");
                if(rank.contains("leaderGap"))summary["leaderGap"]=rank.value("leaderGap");
            }
            for(auto *info:selected->findChildren<JumpDataDetailedInfoWindow *>())if(info->isVisibleTo(m_content) && info->getJumpData()) {
                auto *jump=info->getJumpData();
                if(jump->getCompetition() && (window->getLiveStage()==0 || window->getLiveStage()>=5)) {
                    const int position=jump->getCompetition()->getRules().getCompetitionType()==CompetitionRules::Team ? jump->getPositionInRoundForTeamCompetition() : jump->getPositionInRound();
                    if(position>0) { summary["roundPositionNumber"]=position; summary["roundPosition"]=QString::number(position); }
                } else summary.remove("roundPosition");
                if(summary.contains("wind"))summary["windValue"]=qRound64(jump->getAveragedWind()*100)/100.;
                if(summary.contains("compensation")){
                    summary["compensationValue"]=qRound64(jump->getTotalCompensation()*10)/10.;
                    summary["windPoints"]=QString::number(jump->getWindCompensation(),'f',1)+tr(" pkt");
                    summary["gatePoints"]=QString::number(jump->getGateCompensation(),'f',1)+tr(" pkt");
                }
                if(summary.contains("judgesText")) {
                    const auto marks=jump->getJudges();int lowest=-1,highest=-1;
                    if(marks.size()>=3){lowest=std::min_element(marks.begin(),marks.end())-marks.begin();for(int index=0;index<marks.size();++index)if(index!=lowest&&(highest<0||marks[index]>marks[highest]))highest=index;}
                    QVariantList judges;for(int index=0;index<marks.size();++index)judges.append(QVariantMap{{"text",QString::number(marks[index],'f',1)},{"discarded",index==lowest||index==highest}});
                    summary["judges"]=judges;
                    summary["judgesPoints"]=QString::number(jump->getJudgesPoints(),'f',1)+tr(" pkt");
                }
                break;
            }
        }
        if(summary!=m_jumpSummary) { m_jumpSummary=summary;emit changed(); }
    }
    m_refreshing=false;
}
void QuickBridge::deferRefresh() {
    if(m_pending) return;
    m_pending=true;
    QTimer::singleShot(0,this,[this]{ m_pending=false; refresh(); });
}
void QuickBridge::edit(int id,const QVariant &value) {
    if(auto *page=qobject_cast<QuickNode *>(m_objects.value(id))) {
        if(page==m_groups.value("season-pages") && value.toInt()>0)
            if(auto *tabs=m_content->findChild<QTabWidget *>("toolBox")){const auto mapping=page->property("quickSourceIndexes").toList();tabs->setCurrentIndex(mapping.isEmpty()?value.toInt()-1:mapping.value(value.toInt()).toInt());}
        page->setProperty("quickTabIndex",value.toInt());deferRefresh();return;
    }
    auto *widget=qobject_cast<QWidget *>(m_objects.value(id)); if(!widget || !widget->isEnabled()) return;
    if(auto *spin=qobject_cast<QSpinBox *>(widget)) { spin->setValue(value.toInt()); QMetaObject::invokeMethod(spin,"editingFinished"); }
    else if(auto *spin=qobject_cast<QDoubleSpinBox *>(widget)) { spin->setValue(value.toDouble()); QMetaObject::invokeMethod(spin,"editingFinished"); }
    else if(auto *combo=qobject_cast<QComboBox *>(widget)) {
        if(value.type()==QVariant::String && combo->isEditable()) combo->setEditText(value.toString());
        else { combo->setCurrentIndex(value.toInt()); QMetaObject::invokeMethod(combo,"activated",Q_ARG(int,value.toInt())); }
    } else if(auto *tabs=qobject_cast<QTabWidget *>(widget)) {
        tabs->setCurrentIndex(value.toInt());
        if(profile()=="SimulationSaveManagerWindow" && tabs->objectName()=="toolBox")
            if(auto *pages=m_groups.value("season-pages")){const auto mapping=pages->property("quickSourceIndexes").toList();pages->setProperty("quickTabIndex",value.toInt()==4?4:mapping.indexOf(value.toInt()));}
    }
    else if(auto *box=qobject_cast<QToolBox *>(widget)) box->setCurrentIndex(value.toInt());
    else if(auto *line=qobject_cast<QLineEdit *>(widget)) {
        if(line->isReadOnly()) return;
        QString input=value.toString(); int position=0;
        if(line->validator() && line->validator()->validate(input,position)!=QValidator::Acceptable) return;
        line->setText(input); QMetaObject::invokeMethod(line,"textEdited",Q_ARG(QString,input)); QMetaObject::invokeMethod(line,"editingFinished");
    } else if(auto *text=qobject_cast<QTextEdit *>(widget)) { if(!text->isReadOnly()) text->setPlainText(value.toString()); }
    else if(auto *text=qobject_cast<QPlainTextEdit *>(widget)) { if(!text->isReadOnly()) text->setPlainText(value.toString()); }
    else if(auto *slider=qobject_cast<QSlider *>(widget)) slider->setValue(value.toInt());
    else if(auto *group=qobject_cast<QGroupBox *>(widget)) group->setChecked(value.toBool());
    else if(auto *date=qobject_cast<QDateTimeEdit *>(widget)) date->setDateTime(QDateTime::fromString(value.toString(),Qt::ISODate));
    deferRefresh();
}
void QuickBridge::activate(int id,int entry) {
    QPointer<QAbstractButton> button=qobject_cast<QAbstractButton *>(m_objects.value(id));
    if(!button || !button->isEnabled()) return;
    QTimer::singleShot(0,this,[this,button,entry] {
        if(!button) return;
        QMenu *menu=nullptr;
        if(auto *tool=qobject_cast<QToolButton *>(button)) menu=tool->menu();
        if(auto *push=qobject_cast<QPushButton *>(button)) menu=push->menu();
        if(entry>=0 && menu && entry<menu->actions().size()) menu->actions()[entry]->trigger();
        else button->click();
        deferRefresh();
    });
}
void QuickBridge::triggerAction(int id) {
    QPointer<QObject> object=m_objects.value(id);
    auto *action=qobject_cast<QAction *>(object);
    auto *shortcut=qobject_cast<QShortcut *>(object);
    if((!action || !action->isEnabled()) && (!shortcut || !shortcut->isEnabled())) return;
    QTimer::singleShot(0,this,[this,object] {
        if(auto *action=qobject_cast<QAction *>(object)) action->trigger();
        else if(auto *shortcut=qobject_cast<QShortcut *>(object)) QMetaObject::invokeMethod(shortcut,"activated");
        deferRefresh();
    });
}
bool QuickBridge::shortcut(int owner,const QString &sequence) {
    auto *widget=qobject_cast<QWidget *>(m_objects.value(owner));
    const QKeySequence key(sequence);
    for(QWidget *parent=widget;parent;parent=parent->parentWidget()) {
        if(auto *list=qobject_cast<DatabaseItemsListView *>(parent);list && (sequence=="Insert" || sequence=="Del")) {
            if((sequence=="Insert" && !list->getAllowInserting()) || (sequence=="Del" && !list->getAllowRemoving()))return false;
            for(auto *action:list->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly))
                if(action->isEnabled() && action->shortcut().toString(QKeySequence::PortableText).endsWith(sequence=="Insert"?"A":"D")) { triggerAction(idFor(action));return true; }
        }
        for(auto *action:parent->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly))
            if(action->shortcut()==key && action->isEnabled()) { triggerAction(idFor(action)); return true; }
        for(auto *shortcut:parent->findChildren<QShortcut *>(QString(),Qt::FindDirectChildrenOnly))
            if(shortcut->key()==key && shortcut->isEnabled()) { triggerAction(idFor(shortcut)); return true; }
        if(parent==m_window) break;
    }
    for(auto *action:m_window->findChildren<QAction *>(QString(),Qt::FindDirectChildrenOnly))
        if(action->shortcut()==key && action->isEnabled()) { triggerAction(idFor(action)); return true; }
    return false;
}
void QuickBridge::launch(const QString &destination) {
    static const QHash<QString,const char *> destinations{{"competition","on_pushButton_singleCompetition_clicked"},{"jumps","on_pushButton_singleJumps_clicked"},
        {"seasons","on_pushButton_season_clicked"},{"database","on_pushButton_databaseEdit_clicked"},{"settings","on_pushButton_settings_clicked"},
        {"author","showAuthor"},{"help","on_pushButton_help_clicked"},{"credits","on_pushButton_thanks_clicked"},{"issue","on_pushButton_reportIssue_clicked"}};
    const auto slot=destinations.value(destination,nullptr);
    if(!m_home || !slot) return;
    QTimer::singleShot(0,this,[this,slot] { if(m_window) QMetaObject::invokeMethod(m_window,slot); emit changed(); });
}
void QuickBridge::showAuthorCredits() {
    if(auto *author=qobject_cast<AuthorWindow *>(m_window))QTimer::singleShot(0,author,&AuthorWindow::showCredits);
}
void QuickBridge::close() { QTimer::singleShot(0,this,[this] { if(m_window) m_window->close(); }); }
QObject *QuickBridge::find(const QString &name) {
    if(m_groups.contains(name))return m_groups.value(name);
    QWidget *widget=nullptr;
    if(name=="quickSelectedName" && m_content) {
        for(auto *child:m_content->findChildren<QWidget *>())if(child->inherits("JumperCompetitionResultsWidget")) {
            widget=child->findChild<QWidget *>("label_nameAndSurname");break;
        }
    }
    if(m_content && !widget) {
        const auto matches=m_content->findChildren<QWidget *>(name);
        for(auto *candidate:matches) if(candidate->isVisibleTo(m_content)) { widget=candidate;break; }
        if(!widget && !matches.isEmpty()) widget=matches.first();
    }
    if(!widget && m_content) {
        const QString className=name=="quickRaceDetails"?"JumperCompetitionResultsWidget":name=="quickTeamResults"?"QTreeView":name=="quickVirtualResults"?"ClassificationResultsTableView":"";
        if(!className.isEmpty()) for(auto *child:m_content->findChildren<QWidget *>())
            if(child->inherits(qPrintable(className))) { widget=child; break; }
    }
    if(widget) m_named.insert(name,widget);
    return widget?node(widget,true):nullptr;
}

void QuickBridge::inputText(int id,const QString &value) {
    auto *line=qobject_cast<QLineEdit *>(m_objects.value(id));
    if(!line || !line->isEnabled() || line->isReadOnly()) return;
    QString input=value; int position=input.size();
    if(line->validator() && line->validator()->validate(input,position)==QValidator::Invalid) return;
    line->setText(input);
    QMetaObject::invokeMethod(line,"textEdited",Q_ARG(QString,input));
    deferRefresh();
}
void QuickBridge::finishText(int id) {
    if(auto *line=qobject_cast<QLineEdit *>(m_objects.value(id));line && line->hasAcceptableInput())
        QMetaObject::invokeMethod(line,"editingFinished");
    deferRefresh();
}

QuickNode *QuickBridge::rosterNode(QWidget *source) {
    auto *backend=m_rosters.value(source);
    if(!backend){backend=new QuickRoster(source,this);m_rosters.insert(source,backend);connect(source,&QObject::destroyed,this,[this,source,backend]{m_rosters.remove(source);backend->deleteLater();});}
    backend->sync();
    auto *entry=identity(source);
    QVariantMap values{{"kind","roster"},{"available",true},{"enabled",true},{"backend",QVariant::fromValue(static_cast<QObject *>(backend))}};
    if(entry->values().contains("externalActions"))values["externalActions"]=entry->values().value("externalActions");
    if(qobject_cast<JumpersListEditorWindow *>(source)) {
        values["nameField"]=QVariant::fromValue(node(source->findChild<QLineEdit *>("lineEdit"),true));
        values["defaultField"]=QVariant::fromValue(node(source->findChild<QCheckBox *>("checkBox_default"),true));
    }
    if(!qobject_cast<CompetitionConfigWindow *>(source))
        if(auto *submit=source->findChild<QPushButton *>("pushButton_submit"))values["submit"]=QVariant::fromValue(node(submit,true));
    entry->update(values);return entry;
}

QuickNode *QuickBridge::sectionTabs(const QString &key,const QStringList &labels,const QVariantList &pages) {
    auto *entry=m_groups.value(key,nullptr);
    if(!entry) { entry=new QuickNode(this);m_groups.insert(key,entry); }
    const int selected=qBound(0,entry->property("quickTabIndex").toInt(),qMax(0,pages.size()-1));
    entry->update({{"kind","tabs"},{"id",idFor(entry)},{"name",key},{"options",labels},{"value",selected}},pages.isEmpty()?QVariantList():QVariantList{pages[selected]});
    return entry;
}

void QuickBridge::compose(QuickNode *base) {
    auto group=[this](const QString &key,const QString &title,const QVariantList &children,const QString &kind=QString("panel")) {
        auto *entry=m_groups.value(key,nullptr);
        if(!entry) { entry=new QuickNode(this);m_groups.insert(key,entry); }
        entry->update({{"kind",kind},{"text",title}},children);
        return QVariant::fromValue(entry);
    };
    auto field=[this](const QString &name,const QString &label=QString()) -> QVariant {
        auto *widget=m_content->findChild<QWidget *>(name);
        if(!widget || widget->isHidden()) return {};
        if(!label.isEmpty()) m_captions.insert(widget,label);
        auto *entry=node(widget);
        return entry?QVariant::fromValue(entry):QVariant();
    };
    auto fields=[&](std::initializer_list<QPair<QString,QString>> schema) {
        QVariantList children;
        for(const auto &item:schema) { auto entry=field(item.first,item.second);if(entry.isValid())children.append(entry); }
        return children;
    };
    auto catalog=[this](QWidget *page,const QVariant &toolbar=QVariant()) -> QuickNode * {
        auto *container=page->findChild<DatabaseItemsListView *>();
        if(!container)return nullptr;
        node(container,true);
        auto *view=container->findChild<QAbstractItemView *>("listView");if(!view)return nullptr;
        const auto key="catalog-"+QString::number(idFor(container));auto *entry=m_groups.value(key);
        if(!entry){entry=new QuickNode(this);m_groups.insert(key,entry);}
        if(!view->property("quickCatalogConnected").toBool()){
            view->setProperty("quickCatalogConnected",true);
            connect(view,&QAbstractItemView::doubleClicked,this,[this,entry]{entry->setProperty("editRequest",entry->property("editRequest").toInt()+1);deferRefresh();});
        }
        QuickNode *editor=nullptr;QWidget *nativeEditor=nullptr;
        const QStringList editorTypes{"JumperEditorWidget","HillEditorWidget","CompetitionRulesEditorWidget","ClassificationEditorWidget","PointsForPlacesPresetEditorWidget","JumpersListsListItemWidget"};
        for(auto *widget:page->findChildren<QWidget *>())if(editorTypes.contains(QString::fromLatin1(widget->metaObject()->className()))&&widget->isVisibleTo(page)){editor=node(widget,true);nativeEditor=widget;break;}
        QVariantList actions;
        for(auto *button:page->findChildren<QPushButton *>())if(button->isVisibleTo(page)&&!container->isAncestorOf(button)&&(!nativeEditor||!nativeEditor->isAncestorOf(button)))actions.append(QVariant::fromValue(node(button,true)));
        entry->update({{"kind","catalog"},{"actionsRow",actions},{"list",QVariant::fromValue(node(view,true))},{"editor",QVariant::fromValue(editor)},{"toolbar",toolbar},{"editRequest",entry->property("editRequest").toInt()}});
        return entry;
    };
    if(profile()=="SimulationSavesWindow") {
        if(auto *view=m_content->findChild<QAbstractItemView *>("listView_simulationSaves")){
            QVariantList actions;
            for(const auto &item:QList<QPair<QString,QString>>{{"pushButton_add",tr("Nowy zapis")},{"pushButton_OK",tr("Wczytaj")},{"pushButton_remove",tr("Usuń")}})if(auto *button=m_content->findChild<QPushButton *>(item.first)){
                auto *entry=m_groups.value(item.first);if(!entry){entry=new QuickNode(this);m_groups.insert(item.first,entry);}
                entry->update({{"kind","button"},{"name",item.first},{"id",idFor(button)},{"text",item.second},{"enabled",button->isEnabled()},{"primary",item.first=="pushButton_OK"}});actions.append(QVariant::fromValue(entry));
            }
            m_root->update({{"kind","catalog"},{"list",QVariant::fromValue(node(view,true))},{"actionsRow",actions}});return;
        }
    }
    if(profile()=="DatabaseEditorWindow") {
        auto *source=m_content->findChild<QTabWidget *>("tabWidget_main");
        if(source)if(auto *content=catalog(source->currentWidget(),source->currentIndex()<2 ? field("pushButton_sortByCountries") : QVariant())){
            auto *tabs=node(source,true);m_root->update(tabs->values(),{QVariant::fromValue(content)});return;
        }
    }
    if(qobject_cast<QInputDialog *>(m_window) || qobject_cast<QProgressDialog *>(m_window)) {
        QVariantList content,buttons;
        const auto append=[&](QWidget *widget,QVariantList &list){auto *entry=node(widget,true);if(!entry)return;auto values=entry->values();values["available"]=true;
            if(auto *input=qobject_cast<QInputDialog *>(m_window);input&&!qobject_cast<QLabel *>(widget)&&!qobject_cast<QAbstractButton *>(widget)){values["label"]=QString();values["accessibleName"]=input->labelText();}
            entry->update(values);list.append(QVariant::fromValue(entry));};
        for(auto *child:m_content->findChildren<QWidget *>(QString(),Qt::FindDirectChildrenOnly)) {
            if(child->isHidden())continue;
            if(auto *box=qobject_cast<QDialogButtonBox *>(child)){for(auto *button:box->buttons()){const auto caption=standardCaption(int(box->standardButton(button)));if(!caption.isEmpty() && button->text()!=caption)button->setText(caption);append(button,buttons);}}
            else if(qobject_cast<QPushButton *>(child))append(child,buttons);
            else if(qobject_cast<QLabel *>(child)||qobject_cast<QProgressBar *>(child)||qobject_cast<QLineEdit *>(child)||qobject_cast<QComboBox *>(child)||qobject_cast<QSpinBox *>(child)||qobject_cast<QDoubleSpinBox *>(child)||qobject_cast<QPlainTextEdit *>(child))append(child,content);
        }
        m_root->update({{"kind","column"},{"buttons",buttons}},content);return;
    }
    if(auto *message=qobject_cast<QMessageBox *>(m_window)) {
        QVariantList content;
        content.append(group("message",readableHtml(message->text()),{},"richtext"));
        if(!message->informativeText().isEmpty()) content.append(group("information",readableHtml(message->informativeText()),{},"richtext"));
        if(!message->detailedText().isEmpty()) content.append(group("details",message->detailedText(),{},"richtext"));
        if(auto *check=message->checkBox()) {
            check->setProperty("quickExposePage",true);
            auto *entry=node(check,true);auto values=entry->values();values["available"]=true;entry->update(values);content.append(QVariant::fromValue(entry));
        }
        QVariantList buttons;
        for(auto *button:message->buttons()) {
            const auto caption=standardCaption(int(message->standardButton(button)));if(!caption.isEmpty() && button->text()!=caption)button->setText(caption);
            button->setProperty("quickExposePage",true);
            auto *entry=node(button,true);auto values=entry->values();values["available"]=true;
            const auto role=message->buttonRole(button);values["primary"]=role==QMessageBox::AcceptRole || role==QMessageBox::YesRole;
            entry->update(values);buttons.append(QVariant::fromValue(entry));
        }
        QVariantList messages;for(const auto &value:content)if(auto *entry=value.value<QuickNode *>();entry && entry->values().value("kind")=="richtext")messages.append(entry->values().value("text"));
        m_root->update({{"kind","column"},{"buttons",buttons},{"messages",messages}},content);return;
    }
    if(profile()=="SingleJumpsResultsWindow") {
        auto *box=m_content->findChild<QToolBox *>("toolBox");
        QVariantList content;
        if(box && box->currentIndex()==0) {
            if(auto *info=m_content->findChild<JumpDataDetailedInfoWindow *>()) {
                for(const auto &section:QList<QPair<QString,QString>>{
                        {"formLayout_mainInformations",tr("Informacje o skoku")},
                        {"formLayout_simulationRating",tr("Silnik symulacji")},
                        {"verticalLayout_windsInfo",tr("Wiatr")}}) {
                    auto children=childNodes(info->findChild<QLayout *>(section.first));
                    if(section.first=="formLayout_mainInformations")
                        for(int i=children.size()-1;i>=0;--i)
                            if(auto *entry=children[i].value<QuickNode *>();entry && entry->values().value("name").toString().startsWith("label_position"))children.removeAt(i);
                    if(!children.isEmpty())content.append(group("single-"+section.first,section.second,children));
                }
                for(const auto *name:{"label_DSQ","label_beats95HSPercents","pushButton"})
                    if(auto *widget=info->findChild<QWidget *>(name);widget && !widget->isHidden())content.append(QVariant::fromValue(node(widget,true)));
            }
        } else if(box)content=childNodes(box->currentWidget()->layout());
        const auto details=group("single-jump-details",QString(),content,"column").value<QuickNode *>();
        m_root->update(details->values(),details->children());return;
    }
    if(profile()=="JumpDataDetailedInfoWindow") {
        auto layout=[this](const QString &name) { return m_content->findChild<QLayout *>(name); };
        QVariantList information=childNodes(layout("horizontalLayout_jumperInfoTitle"));
        QVariantList primary=childNodes(layout("formLayout_mainInformations"));
        for(const auto &name:{QString("label_DSQ"),QString("label_beats95HSPercents")}) {
            auto value=field(name);if(value.isValid())primary.append(value);
        }
        information.append(group("jump-summary",title(),{group("jump-fields",QString(),primary,"row")}));
        const auto simulation=childNodes(layout("formLayout_simulationRating"));
        if(!simulation.isEmpty())information.append(group("jump-simulation",tr("Silnik symulacji"),{group("simulation-fields",QString(),simulation,"row")}));
        const auto winds=childNodes(layout("verticalLayout_windsInfo"));
        if(!winds.isEmpty())information.append(group("jump-wind",tr("Wiatr"),winds));
        const auto share=field("pushButton");if(share.isValid())information.append(share);
        m_root->update({{"kind","column"}},information);return;
    }
    if(auto *list=qobject_cast<JumpersListEditorWindow *>(m_window)){auto *entry=rosterNode(list);m_root->update(entry->values(),entry->children());return;}
    if(profile()=="QDialog")if(auto *team=m_content->findChild<TeamEditorWidget *>()){auto *entry=rosterNode(team);m_root->update(entry->values(),entry->children());return;}
    if(profile()=="JumpersFormGeneratorConfigWindow") {
        m_window->setProperty("quickExplicitFormEdits",true);
        const auto parameters=fields({{"doubleSpinBox_formVariability",tr("Zmienność formy")},{"doubleSpinBox_formBonus",tr("Bonus formy")},{"doubleSpinBox_minForm",tr("Minimalna forma")},{"doubleSpinBox_maxForm",tr("Maksymalna forma")}});
        const auto *table=m_content->findChild<QAbstractItemView *>("tableView");
        m_root->update({{"kind","formGenerator"},{"participantCount",table && table->model() ? table->model()->rowCount() : 0},{"controller",QVariant::fromValue(static_cast<QObject *>(m_window.data()))},{"pending",m_window->property("quickFormDraftPending")},{"parameters",parameters},{"presets",fields({{"pushButton_loadPreset",""},{"pushButton_saveAsPreset",""}})},{"table",field("tableView")},{"lists",field("pushButton_jumpersLists")},{"submit",field("pushButton_generate")}});return;
    }
    if(profile()=="AppSettingsWindow") {
        QVariantList sections;
        sections.append(group("simulation",tr("Silnik symulacji"),{group("simulation-settings-fields",QString(),fields({
            {"comboBox_language",tr("Język interfejsu")},{"spinBox_skillsRange",tr("Zakres umiejętności")},
            {"spinBox_dsqProbability",tr("Bazowe prawdopodobieństwo dyskwalifikacji")},
            {"doubleSpinBox_randomMultiplier",tr("Mnożnik losowości")},{"doubleSpinBox",tr("Wpływ parametrów skoczni")},
            {"doubleSpinBox_takeoffFormEffect",tr("Wpływ formy na wybicie")},{"doubleSpinBox_flightFormEffect",tr("Wpływ formy na lot")},
            {"doubleSpinBox_liveCompSpeed",tr("Tempo konkursu na żywo")},
            {"checkBox_autoAdjustHillEffects",""},{"checkBox_globalDatabaseRecords",""},{"checkBox_liveCompetition",""}}),"form")}));
        sections.append(group("tools",tr("Baza danych i narzędzia"),{group("database-settings-fields",QString(),fields({
            {"pushButton_countriesEditor",""},{"pushButton_jumpInfoChoice",""},{"pushButton_addJumpsImportancePreset",""},
            {"pushButton_autoImages",""},{"pushButton_cutSurnames",""},{"pushButton_loadJumpersWithCSV",""},
            {"pushButton_loadPresetsBefore120",""},{"pushButton_repairDatabase",""},{"pushButton_repairDatabase_2",""},
            {"pushButton_defaultSettings",""}}),"form")}));
        sections.append(group("webhooks",tr("Udostępnianie wyników"),fields({
            {"lineEdit_jumpInfoWh",tr("Webhook: pojedynczy skok")},{"lineEdit_singleResultWh",tr("Webhook: wynik zawodnika")},
            {"lineEdit_compResultsWh",tr("Webhook: wyniki konkursu")},{"lineEdit_clsResultsWh",tr("Webhook: klasyfikacja")}})));
        auto *tabs=sectionTabs("settings-pages",{tr("Symulacja"),tr("Baza danych i narzędzia"),tr("Udostępnianie wyników")},sections);
        m_root->update(tabs->values(),tabs->children());return;
    }
    if(profile()=="SimulationSaveManagerWindow") {
        auto *source=m_content->findChild<QTabWidget *>("toolBox");
        if(source) {
            QVariantList tools;
            for(const auto &name:{"pushButton_formGenerator","pushButton","pushButton_saveAsCopy","pushButton_repairDatabase"})
                if(auto *button=m_content->findChild<QPushButton *>(name))
                    tools.append(QVariantMap{{"id",idFor(button)},{"text",button->text().remove('&')},{"enabled",button->isEnabled()},{"button",true}});
            auto *menuNode=m_groups.value("quickSeasonTools");
            if(!menuNode){menuNode=new QuickNode(this);m_groups.insert("quickSeasonTools",menuNode);}
            menuNode->update({{"kind","button"},{"name","quickSeasonTools"},{"text",tr("Narzędzia")},{"actions",tools}});
            QVariantList overview=fields({{"label_nextCompetitionIndexAndHill",""}});
            if(auto *flag=m_content->findChild<QLabel *>("label_hillFlag");flag && !flag->pixmap(Qt::ReturnByValue).isNull())
                overview.append(QVariant::fromValue(node(flag)));
            const auto type=field("label_serieTypeAndTrainingIndex");if(type.isValid())overview.append(type);
            const auto start=field("pushButton_competitionConfig");if(start.isValid())overview.append(start);
            QVariantList standings;
            if(auto *layout=m_content->findChild<QLayout *>("verticalLayout_classificationResults"))standings=childNodes(layout);
            auto empty=[this](const QString &key,const QString &title,const QString &description,QPushButton *action=nullptr) {
                auto *entry=m_groups.value(key);if(!entry){entry=new QuickNode(this);m_groups.insert(key,entry);}
                entry->update({{"kind","empty"},{"title",title},{"description",description},{"actionText",action?action->text():QString()},{"actionId",action?idFor(action):-1}});return QVariant::fromValue(entry);
            };
            auto *season=qobject_cast<SimulationSaveManagerWindow *>(m_window);
            if(season->getSimulationSave()->getActualSeason()->getCalendarsReference().isEmpty())
                overview={empty("season-no-calendar",tr("Zacznij od kalendarza"),tr("Utwórz kalendarz, a następnie dodaj konkursy lub wczytaj preset."),m_content->findChild<QPushButton *>("pushButton_competitionConfig"))};
            else if(!season->getSimulationSave()->getActualSeason()->getActualCalendar())
                overview={empty("season-no-active-calendar",tr("Wybierz aktywny kalendarz"),tr("W sekcji Kalendarz wybierz kalendarz i użyj przycisku Ustaw jako aktywny."),m_content->findChild<QPushButton *>("pushButton_competitionConfig"))};
            else if(season->getSimulationSave()->getActualSeason()->getActualCalendar()->getCompetitionsReference().isEmpty())
                overview={empty("season-empty-calendar",tr("Dodaj pierwszy konkurs"),tr("Kalendarz jest gotowy. Dodaj konkursy lub wczytaj preset, aby rozpocząć sezon."),m_content->findChild<QPushButton *>("pushButton_competitionConfig"))};
            auto *activeCalendar=season->getSimulationSave()->getActualSeason()->getActualCalendar();
            if(standings.isEmpty() || !activeCalendar || activeCalendar->getClassificationsReference().isEmpty())standings={empty("season-no-standings",tr("Brak wyników klasyfikacji"),tr("Dodaj klasyfikację w kalendarzu i przypisz do niej konkursy. Wyniki pojawią się po ich rozegraniu."))};
            QVariantList pages{group("season-overview",QString(),{
                group("season-next",tr("Następny konkurs"),overview),group("season-standings",tr("Klasyfikacja"),standings)},"sections")};
            QStringList labels{tr("Przegląd")};
            QVariantList sourceIndexes{-1};
            for(int i=0;i<source->count();++i) {
                auto *page=source->widget(i);page->setProperty("quickExposePage",true);
                if(i==4 && page->findChild<ClassificationEditorWidget *>())continue;sourceIndexes.append(i);
                if(i==3 && page->findChild<CalendarEditorWidget *>())pages.append(QVariant::fromValue(node(page->findChild<CalendarEditorWidget *>(),true)));
                else if(auto *content=catalog(page))pages.append(QVariant::fromValue(content));
                else pages.append(QVariant::fromValue(node(page,true)));
                labels.append(source->tabText(i).remove('&'));
            }
            auto *tabs=sectionTabs("season-pages",labels,pages);tabs->setProperty("quickSourceIndexes",sourceIndexes);
            if(!source->property("quickSeasonConnected").toBool()) {
                source->setProperty("quickSeasonConnected",true);
                connect(qobject_cast<SimulationSaveManagerWindow *>(m_window),&SimulationSaveManagerWindow::calendarRequested,this,[this,tabs](bool create,bool addCompetition){
                    tabs->setProperty("quickTabIndex",tabs->property("quickSourceIndexes").toList().indexOf(3));
                    if(create)for(auto *backend:m_calendars) {backend->catalogAction("add");break;}
                    if(addCompetition)for(auto it=m_calendars.cbegin();it!=m_calendars.cend();++it) {
                        const int before=it.value()->events().size();it.value()->add();
                        if(it.value()->events().size()>before)it.key()->setProperty("quickEditRequest",it.key()->property("quickEditRequest").toInt()+1);
                        break;
                    }
                    deferRefresh();
                });
                connect(source,&QTabWidget::currentChanged,this,[this,tabs](int index){const auto mapping=tabs->property("quickSourceIndexes").toList();tabs->setProperty("quickTabIndex",index==4?4:mapping.indexOf(index));deferRefresh();});
            }
            m_root->update(tabs->values(),tabs->children());return;
        }
    }
    if(auto *configuration=qobject_cast<CompetitionConfigWindow *>(m_window)) {
        const bool season=configuration->getType()==CompetitionConfigWindow::SeasonCompetition;
        QVariantList plan;
        if(season && configuration->getSeasonCompetition()) {
            auto *event=configuration->getSeasonCompetition();auto *hill=event->getHill();
            plan.append(group("season-event",hill?hill->getName()+" HS"+QString::number(hill->getHSPoint()):tr("Brak skoczni"),{},"label"));
            plan.append(group("season-rules",event->getShortSerieTypeText()+" · "+event->getRules().getName(),{},"label"));
        }
        if(!season){if(auto *hillChoice=m_content->findChild<QComboBox *>("comboBox_existingHill"))hillChoice->setProperty("quickSearchableChoice",true);const auto hill=field("comboBox_existingHill",tr("Skocznia"));if(hill.isValid())plan.append(hill);}
        auto *box=m_content->findChild<QToolBox *>("toolBox");
        if(box && !season) for(int i=0;i<box->count();++i) {
            auto *page=box->widget(i);
            if(page->findChild<CompetitionRulesEditorWidget *>()) {
                for(auto *combo:page->findChildren<QComboBox *>()) if(combo->objectName().isEmpty()) {
                    m_captions.insert(combo,tr("Zasady konkursu"));plan.append(QVariant::fromValue(node(combo,true)));break;
                }
            }
        }
        if(auto *options=m_content->findChild<QWidget *>("panel_competitionOptions");options && !season) {
            for(auto *check:options->findChildren<QCheckBox *>())if(auto *entry=node(check))plan.append(QVariant::fromValue(entry));
        }
        const auto parameters=fields({{"spinBox_startGate",tr("Belka startowa")},{"pushButton_autoGate",""},
            {"spinBox_dsqProbability",tr("Prawdopodobieństwo dyskwalifikacji")},{"doubleSpinBox_jumpsImportance",tr("Ważność skoków")},
            {"comboBox_jumpsImportancePreset",tr("Preset ważności")}});
        plan.append(group("competition-parameters",QString(),parameters.mid(0,2),"form"));
        QVariantList top{group("plan",tr("Plan symulacji"),plan)};
        if(auto *list=m_content->findChild<QWidget *>("scrollArea_startList")) {
            QVariantList actions;
            for(const auto &name:{"pushButton_defaultStartListOrder","pushButton_sortStartListRandomly","pushButton_sortStartListByCountries","pushButton_orderByJumpersList","pushButton_jumpersLists","pushButton_byOtherComp","pushButton_loadJumpers"}) {
                if(auto *button=list->findChild<QPushButton *>(name)) {
                    button->setProperty("quickMenuOnly",true);
                    if(!button->isHidden())actions.append(QVariantMap{{"id",idFor(button)},{"text",button->text().remove('&')},{"enabled",button->isEnabled()},{"button",true}});
                }
            }
            QVariantList content;
            for(auto *action:list->findChildren<QAction *>()) {
                auto *owner=qobject_cast<QWidget *>(action->parent());
                if(!owner || !owner->isVisibleTo(m_content) || action->shortcut().isEmpty())continue;
                owner->setProperty("quickMenuActions",true);
                auto *ownerNode=node(owner,true);
                if(!ownerNode)continue;
                for(const auto &value:ownerNode->values().value("shortcuts").toList()) {
                    const auto shortcut=value.toMap();
                    if(shortcut.value("id").toInt()!=idFor(action))continue;
                    actions.append(QVariantMap{{"id",idFor(action)},{"text",shortcut.value("text").toString()+"  ("+shortcut.value("sequence").toString()+")"},{"enabled",action->isEnabled()}});
                }
            }
            if(!actions.isEmpty()) {
                auto *entry=m_groups.value("start-list-actions");
                if(!entry){entry=new QuickNode(this);m_groups.insert("start-list-actions",entry);}
                entry->update({{"kind","button"},{"text",tr("Opcje listy")},{"actions",actions}});
                content.append(QVariant::fromValue(entry));
            }
            content.append(QVariant::fromValue(node(list,true)));
            auto *roster=rosterNode(configuration);
            if(!m_rosters.value(configuration)->active())top.append(group("start-list",tr("Lista startowa"),content));
            else {
                auto *summary=m_groups.value("roster-summary");if(!summary){summary=new QuickNode(this);m_groups.insert("roster-summary",summary);}
                summary->update({{"kind","rosterSummary"},{"backend",roster->values().value("backend")}});
                top.append(QVariant::fromValue(summary));
            }
        }
        QVariantList detail;
        QStringList labels{season?tr("Przygotowanie"):tr("Konfiguracja")};
        if(m_rosters.value(configuration) && m_rosters.value(configuration)->active()) {
            detail.append(QVariant::fromValue(rosterNode(configuration)));labels.append(tr("Lista startowa"));
            QVariantList advanced;
            if(auto *list=m_content->findChild<QWidget *>("scrollArea_startList")) {
                for(const auto &name:{"pushButton_loadJumpers","pushButton_byOtherComp","pushButton_defaultStartListOrder","pushButton_sortStartListByCountries","pushButton_orderByJumpersList"})if(auto *button=list->findChild<QPushButton *>(name);button&&!button->isHidden())advanced.append(QVariant::fromValue(node(button,true)));
                for(auto *combo:list->findChildren<QComboBox *>())if(!combo->isHidden())advanced.append(QVariant::fromValue(node(combo,true)));
                for(auto *groups:list->findChildren<QWidget *>())if(groups->inherits("KOGroupsListView")&&!groups->isHidden())advanced.append(QVariant::fromValue(node(groups,true)));
            }
            auto *entry=identity(configuration);auto values=entry->values();QVariantList external;
            for(const auto &value:advanced)if(auto *entry=value.value<QuickNode *>();entry&&entry->values().value("kind")=="button") {const auto fields=entry->values();external.append(QVariantMap{{"id",fields.value("id")},{"text",fields.value("text")},{"enabled",fields.value("enabled")},{"button",true}});}
            values["externalActions"]=external;entry->update(values);
            QVariantList selectors;bool ko=false;
            for(const auto &value:advanced)if(auto *entry=value.value<QuickNode *>();entry&&entry->values().value("kind")!="button"){selectors.append(value);if(entry->values().value("name").toString().contains("KOGroups"))ko=true;}
            if(!selectors.isEmpty()){detail.append(group("start-list-conditions",ko?tr("Grupy KO"):tr("Kolejność według wyników"),selectors));labels.append(ko?tr("Grupy KO"):tr("Kolejność według wyników"));}
        }
        if(auto *hill=configuration->getHillEditor();hill && !season) { hill->setProperty("quickExposePage",true);detail.append(QVariant::fromValue(node(hill,true)));labels.append(tr("Skocznia")); }
        if(auto *rules=configuration->getCompetitionRulesEditor();rules && !season) { rules->setProperty("quickExposePage",true);detail.append(QVariant::fromValue(node(rules,true)));labels.append(tr("Zasady konkursu")); }
        if(box)for(int i=0;i<box->count();++i) {
            auto *page=box->widget(i);
            if(page->inherits("WindsGeneratorSettingsEditorWidget") || page->inherits("InrunSnowGeneratorSettingsEditorWidget")) {
                page->setProperty("quickExposePage",true);detail.append(QVariant::fromValue(node(page,true)));labels.append(page->inherits("WindsGeneratorSettingsEditorWidget")?tr("Wiatr"):tr("Śnieg"));
            }
        }
        detail.append(group("competition-options",tr("Opcje"),{group("competition-option-fields",QString(),season?parameters.mid(2,1):parameters.mid(2),"form")}));labels.append(tr("Opcje"));
        detail.prepend(group("overview",QString(),top,"sections"));
        auto *tabs=sectionTabs("competition-pages",labels,detail);
        m_root->update(tabs->values(),tabs->children());return;
    }
    if(profile()=="SingleJumpsConfigWindow") {
        QVariantList pages;
        pages.append(group("parameters",tr("Konfiguracja"),{group("jump-plan",QString(),fields({
            {"comboBox_existingJumper",tr("Zawodnik")},{"comboBox_existingHill",tr("Skocznia")},
            {"spinBox_jumpsCount",tr("Liczba skoków")},{"spinBox_gate",tr("Belka startowa")},{"pushButton_autoGate",""},
            {"doubleSpinBox_jumpsImportance",tr("Ważność skoków")},{"comboBox_jumpsImportancePreset",tr("Preset ważności")}}),"form")}));
        QStringList labels{tr("Konfiguracja")};
        for(const auto &page:QList<QPair<QString,QString>>{{"page_jumper",tr("Zawodnik")},{"page_hill",tr("Skocznia")},{"page_windsGeneratorSettings",tr("Wiatr")}}) {
            if(auto *widget=m_content->findChild<QWidget *>(page.first)) {
                widget->setProperty("quickExposePage",true);
                auto *entry=node(widget,true);
                if(entry) { pages.append(group(page.first,page.second,{QVariant::fromValue(entry)}));labels.append(page.second); }
            }
        }
        pages.append(group("jump-options",tr("Opcje"),{group("jump-option-fields",QString(),fields({
            {"spinBox_dsqProbability",tr("Prawdopodobieństwo dyskwalifikacji")},
            {"checkBox_changeableWind",""},{"checkBox_hasJudges",""},{"checkBox_hasWindCompensation",""},
            {"comboBox_windAverageCalculatingType",tr("Obliczanie średniego wiatru")},
            {"comboBox_windCompensationDistanceEffect",tr("Wpływ odległości na rekompensatę")}}),"form")}));
        labels.append(tr("Opcje"));
        auto *tabs=sectionTabs("single-jump-pages",labels,pages);
        m_root->update(tabs->values(),tabs->children());return;
    }
    m_root->update(base?base->values():QVariantMap{{"kind","column"}},base?base->children():QVariantList());
}

namespace {
const QList<QPair<QString,QString>> windInputs{{"strength","doubleSpinBox_baseWindStrength"},{"direction","doubleSpinBox_baseWindDirection"},{"strengthVariation","doubleSpinBox_windStrengthChange"},{"directionVariation","doubleSpinBox_windDirectionChange"}};
QVector<WindGenerationSettings> windSnapshot(QToolBox *box) {
    QVector<WindGenerationSettings> result;
    for(int row=0;row<box->count();++row)if(auto *input=qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(row)))result.append(input->getWindGenerationSettingsFromInputs());
    return result;
}
void writeWind(QWidget *page,const QVariantMap &values) {
    for(const auto &field:windInputs)if(values.contains(field.first))if(auto *input=page->findChild<QDoubleSpinBox *>(field.second)) {
        input->setValue(values.value(field.first).toDouble());QMetaObject::invokeMethod(input,"editingFinished");
    }
}
}
void QuickBridge::setWind(int editorId,int sector,const QVariantMap &values,bool all) {
    auto *editor=qobject_cast<WindsGeneratorSettingsEditorWidget *>(m_objects.value(editorId));
    auto *box=editor?editor->findChild<QToolBox *>("toolBox"):nullptr;
    if(!box || sector<0 || sector>=box->count())return;
    m_windUndo[editor]=windSnapshot(box);
    for(int row=0;row<box->count();++row)if(all || row==sector)writeWind(box->widget(row),values);
    deferRefresh();
}
void QuickBridge::randomizeWind(int editorId,double direction,double variability) {
    auto *editor=qobject_cast<WindsGeneratorSettingsEditorWidget *>(m_objects.value(editorId));
    auto *box=editor?editor->findChild<QToolBox *>("toolBox"):nullptr;
    if(!box || box->count()==0 || !std::isfinite(direction) || !std::isfinite(variability))return;
    m_windUndo[editor]=windSnapshot(box);
    QVector<WindGenerationSettings> settings=m_windUndo.value(editor);
    WindGenerationSettings::randomizeSettings(&settings,variability<0?-1:qBound(0.,variability,10.),direction<0?-1:qBound(0.,direction,360.));
    for(int row=0;row<box->count();++row)writeWind(box->widget(row),{{"strength",settings[row].getBaseWindStrength()},{"direction",settings[row].getBaseDirection()},{"strengthVariation",settings[row].getWindStrengthInstability()},{"directionVariation",settings[row].getWindDirectionInstability()}});
    deferRefresh();
}
void QuickBridge::undoWind(int editorId) {
    auto *editor=qobject_cast<WindsGeneratorSettingsEditorWidget *>(m_objects.value(editorId));
    auto *box=editor?editor->findChild<QToolBox *>("toolBox"):nullptr;
    if(!box || !m_windUndo.contains(editor))return;
    const auto previous=m_windUndo.take(editor);
    for(int row=0;row<qMin(box->count(),previous.size());++row)if(auto *input=qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(row)))input->applyInputSettings(previous[row]);
    deferRefresh();
}

void QuickBridge::copyWindSector(int editorId,int sector) {
    auto *editor=qobject_cast<WindsGeneratorSettingsEditorWidget *>(m_objects.value(editorId));
    auto *box=editor?editor->findChild<QToolBox *>("toolBox"):nullptr;
    if(!box || sector<0 || sector>=box->count())return;
    const auto settings=windSnapshot(box);if(settings.size()!=box->count())return;
    m_windUndo[editor]=settings;
    for(int row=0;row<box->count();++row)if(auto *input=qobject_cast<WindsGeneratorSettingsWidgetInputItem *>(box->widget(row)))input->applyInputSettings(settings[sector]);
    deferRefresh();
}

void QuickBridge::selectWindSector(int editorId,int sector) {
    auto *editor=qobject_cast<WindsGeneratorSettingsEditorWidget *>(m_objects.value(editorId));
    auto *box=editor?editor->findChild<QToolBox *>("toolBox"):nullptr;
    if(box && sector>=0 && sector<box->count()) { editor->setProperty("quickWindSector",sector);deferRefresh(); }
}
