#include "CompetitionQuickBridge.h"
#include "../CompetitionConfigWindow.h"
#include "ui_CompetitionConfigWindow.h"
#include "../../EditorWidgets/HillEditorWidget.h"
#include "../../EditorWidgets/CharacteristicsEditor.h"
#include "../../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../../ResponsiveWindowUtils.h"
#include "../../../simulator/Hill.h"
#include <QApplication>
#include <QAbstractItemView>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QQuickWidget>
#include <QQmlContext>
#include <QScrollArea>
#include <QSpinBox>
#include <QTimer>
#include <QToolBox>
#include <QMessageBox>
#include <functional>

CompetitionQuickBridge::CompetitionQuickBridge(CompetitionConfigWindow *configuration, QObject *parent)
    : QObject(parent), m_configuration(configuration)
{
    refresh();
}

QVariantMap CompetitionQuickBridge::field(QWidget *widget, const QString &label)
{
    if (!widget) return {};
    const int id = ++m_nextId;
    m_fields.insert(id, widget);
    QVariantMap result{{"id", id}, {"label", label}, {"enabled", widget->isEnabled()},
                       {"hint", widget->toolTip()}};
    if (auto *spin = qobject_cast<QSpinBox *>(widget)) {
        result.insert("kind", "number");
        result.insert("value", spin->value());
        result.insert("minimum", spin->minimum());
        result.insert("maximum", spin->maximum());
        result.insert("step", spin->singleStep());
        result.insert("decimals", 0);
    } else if (auto *spin = qobject_cast<QDoubleSpinBox *>(widget)) {
        result.insert("kind", "number");
        result.insert("value", spin->value());
        result.insert("minimum", spin->minimum());
        result.insert("maximum", spin->maximum());
        result.insert("step", spin->singleStep());
        result.insert("decimals", spin->decimals());
    } else if (auto *combo = qobject_cast<QComboBox *>(widget)) {
        QStringList choices;
        for (int i = 0; i < combo->count(); ++i) choices << combo->itemText(i);
        result.insert("kind", "choice");
        result.insert("choices", choices);
        result.insert("value", combo->currentIndex());
    } else if (auto *check = qobject_cast<QCheckBox *>(widget)) {
        result.insert("kind", "check");
        result.insert("value", check->isChecked());
    } else if (auto *edit = qobject_cast<QLineEdit *>(widget)) {
        result.insert("kind", "text");
        result.insert("value", edit->text());
        result.insert("maxLength", edit->maxLength());
    }
    return result;
}

void CompetitionQuickBridge::card(const QString &title, QWidget *editor, const QString &schema)
{
    static const QJsonObject schemas = [] {
        QFile file(":/competition-quick/fields.json");
        file.open(QIODevice::ReadOnly);
        return QJsonDocument::fromJson(file.readAll()).object();
    }();
    QVariantList fields;
    for (const auto &entry : schemas.value(schema).toArray()) {
        const auto pair = entry.toArray();
        auto *widget = editor->findChild<QWidget *>(pair[0].toString());
        if (widget && !widget->isHidden()) fields << field(widget, tr(qPrintable(pair[1].toString())));
    }
    m_cards << QVariantMap{{"title", title}, {"fields", fields}};
}

void CompetitionQuickBridge::refresh()
{
    m_fields.clear();
    m_cards.clear();
    auto *c = m_configuration;
    auto one = [this](const QString &title, const QVariantList &fields) {
        m_cards << QVariantMap{{"title", title}, {"fields", fields}};
    };
    if (m_section == 0) {
        one(tr("Szybki wybór"), {field(c->ui->comboBox_existingHill, tr("Skocznia z bazy")),
                                 field(c->existingCompetitionRulesComboBox, tr("Zasady z bazy")),
                                 field(c->checkBox_singleCompetitionQualifications, tr("Przeprowadź kwalifikacje"))});
        card(tr("Parametry konkursu"), c, "options");
    } else if (m_section == 1) {
        one(tr("Baza skoczni"), {field(c->ui->comboBox_existingHill, tr("Skocznia"))});
        card(tr("Profil skoczni"), c->hillEditor, "HillEditorWidget");
    } else if (m_section == 2) {
        one(tr("Gotowe zasady"), {field(c->existingCompetitionRulesComboBox, tr("Zestaw zasad"))});
        card(tr("Zasady konkursu"), c->competitionRulesEditor, "CompetitionRulesEditorWidget");
        auto *rounds = c->competitionRulesEditor->findChild<QToolBox *>("toolBox_rounds");
        for (int i = 0; i < rounds->count(); ++i)
            card(rounds->itemText(i), rounds->widget(i), "RoundInfoEditorWidget");
    } else if (m_section == 3) {
        auto *sectors = c->windsGeneratorSettingsEditor->findChild<QToolBox *>("toolBox");
        for (int i = 0; i < sectors->count(); ++i)
            card(sectors->itemText(i), sectors->widget(i), "WindsGeneratorSettingsWidgetInputItem");
        card(tr("Śnieg na najeździe"), c->inrunSnowGeneratorSettingsEditor, "InrunSnowGeneratorSettingsEditorWidget");
    } else if (m_section == 4 && !c->comboBox_groupsSelectionType->isHidden()) {
        one(tr("Grupy KO"), {field(c->comboBox_groupsSelectionType, tr("Tworzenie grup KO"))});
    }
    emit changed();
}

void CompetitionQuickBridge::setSection(int section)
{
    if (section < 0 || section > 4 || section == m_section) return;
    m_section = section;
    refresh();
}

void CompetitionQuickBridge::setField(int id, const QVariant &value)
{
    QPointer<QWidget> widget = m_fields.value(id);
    if (!widget || !widget->isEnabled()) return;
    if (auto *spin = qobject_cast<QSpinBox *>(widget.data())) {
        spin->setValue(value.toInt());
        emit spin->editingFinished();
    } else if (auto *spin = qobject_cast<QDoubleSpinBox *>(widget.data())) {
        spin->setValue(value.toDouble());
        emit spin->editingFinished();
    } else if (auto *combo = qobject_cast<QComboBox *>(widget.data())) {
        const int index = value.toInt();
        if (index >= 0 && index < combo->count()) {
            combo->setCurrentIndex(index);
            emit combo->activated(index);
        }
    } else if (auto *check = qobject_cast<QCheckBox *>(widget.data())) {
        check->setChecked(value.toBool());
    } else if (auto *edit = qobject_cast<QLineEdit *>(widget.data())) {
        edit->setText(value.toString());
        emit edit->editingFinished();
    }
    // Updating K / presets can destroy and recreate the sector/round widgets.
    // Defer the snapshot until all original signal handlers have completed.
    QTimer::singleShot(0, this, &CompetitionQuickBridge::refresh);
}

QVariantList CompetitionQuickBridge::jumpers() const
{
    QVariantList rows;
    int i = 0;
    for (auto *jumper : m_configuration->competitionJumpers) {
        rows << QVariantMap{{"row", i++}, {"name", jumper->getNameAndSurname()},
                            {"country", jumper->getCountryCode()}};
    }
    return rows;
}

QVariantMap CompetitionQuickBridge::summary() const
{
    auto *c = m_configuration;
    const Hill hill = c->hillEditor->getHillFromWidgetInput();
    const auto rules = c->competitionRulesEditor->getCompetitionRulesFromWidgetInputs();
    return {{"hill", hill.getName().isEmpty() ? tr("Wybierz skocznię") : hill.getName()},
            {"k", hill.getKPoint()}, {"hs", hill.getHSPoint()},
            {"count", c->competitionJumpers.size()}, {"teams", c->competitionTeams.size()},
            {"team", rules.getCompetitionType() == CompetitionRules::Team},
            {"rounds", rules.getRounds().size()}, {"gate", c->getStartingGateFromInput()},
            {"qualifications", c->checkBox_singleCompetitionQualifications->isChecked()},
            {"ko", !c->comboBox_groupsSelectionType->isHidden()}};
}

// Only auxiliary specialized editors remain Widgets. Move the actual editor
// temporarily, then put it back at precisely the same layout position.
void CompetitionQuickBridge::specialEditor(QWidget *editor, const QString &title)
{
    QWidget *oldParent = editor->parentWidget();
    QBoxLayout *oldLayout = nullptr;
    int oldIndex = -1;
    std::function<void(QLayout *)> locate = [&](QLayout *layout) {
        if (!layout) return;
        for (int i = 0; i < layout->count(); ++i) {
            if (layout->itemAt(i)->widget() == editor) {
                oldLayout = qobject_cast<QBoxLayout *>(layout);
                oldIndex = i;
                return;
            }
            locate(layout->itemAt(i)->layout());
        }
    };
    locate(oldParent->layout());
    if (!oldLayout) return;
    const bool wasHidden = editor->isHidden();
    QDialog dialog(qobject_cast<QWidget *>(parent()));
    dialog.setWindowTitle(title);
    auto *layout = new QVBoxLayout(&dialog);
    auto *scroll = new QScrollArea(&dialog);
    scroll->setWidgetResizable(true);
    oldLayout->removeWidget(editor);
    scroll->setWidget(editor);
    layout->addWidget(scroll);
    auto *done = new QPushButton(tr("Gotowe"), &dialog);
    layout->addWidget(done);
    connect(done, &QPushButton::clicked, &dialog, &QDialog::accept);
    editor->show();
    ResponsiveWindowUtils::fitToAvailableScreen(&dialog, QSize(760, 650));
    dialog.exec();
    scroll->takeWidget();
    editor->setParent(oldParent);
    oldLayout->insertWidget(oldIndex, editor);
    editor->setHidden(wasHidden);
}

void CompetitionQuickBridge::action(const QString &name, const QVariantList &selection)
{
    // Dispatch after QML has committed focused text fields and left its handler.
    QTimer::singleShot(0, this, [this, name, selection] {
        auto *c = m_configuration;
        if (name == "widgets") {
            c->exec();
        } else if (name == "start") {
            c->on_pushButton_submit_clicked();
        } else if (name == "autoGate") {
            c->on_pushButton_autoGate_clicked();
        } else if (name == "load") {
            c->on_pushButton_loadJumpers_clicked();
        } else if (name == "randomOrder") {
            c->on_pushButton_sortStartListRandomly_clicked();
        } else if (name == "countryOrder") {
            c->on_pushButton_sortStartListByCountries_clicked();
        } else if (name == "randomWind") {
            c->push_button_randomWind->click();
        } else if (name == "fillWind") {
            c->windsGeneratorSettingsEditor->askForSettingsForFillAll();
        } else if (name == "characteristics") {
            specialEditor(c->hillEditor->getCharacteristicsEditor(), tr("Cechy skoczni"));
        } else if (name == "teams") {
            specialEditor(c->teamsTreeView, tr("Składy drużyn"));
        } else if (name == "ko") {
            specialEditor(c->KOGroupsList, tr("Grupy KO"));
        } else if (name == "up" || name == "down" || name == "remove") {
            auto *view = c->jumpersListView->getListView();
            view->selectionModel()->clearSelection();
            for (const auto &row : selection) {
                if (row.toInt() < 0 || row.toInt() >= view->model()->rowCount()) continue;
                const auto index = view->model()->index(row.toInt(), 0);
                view->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
                view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
            }
            if (!selection.isEmpty()) {
                QMetaObject::invokeMethod(c->jumpersListView,
                    name == "up" ? "onUpActionTriggered" : name == "down" ? "onDownActionTriggered" : "onRemoveActionTriggered");
            }
        }
        refresh();
    });
}

CompetitionQuickWindow::CompetitionQuickWindow(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Sj.Sim · Pojedynczy konkurs · Qt Quick"));
    setWindowFlags(Qt::Window);
    setMinimumSize(480, 480);
    auto *configuration = new CompetitionConfigWindow(CompetitionConfigWindow::SingleCompetition, this);
    m_bridge = new CompetitionQuickBridge(configuration, this);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_view = new QQuickWidget(this);
    m_view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_view->rootContext()->setContextProperty("competition", m_bridge);
    m_view->setSource(QUrl("qrc:/competition-quick/CompetitionConfig.qml"));
    layout->addWidget(m_view);
    if (m_view->status() == QQuickWidget::Error) {
        auto *fallback = new QPushButton(tr("Nie można załadować QML. Otwórz konfigurację Widgets."), this);
        layout->addWidget(fallback);
        connect(fallback, &QPushButton::clicked, this, [this] { m_bridge->action("widgets"); });
    }
    ResponsiveWindowUtils::fitToAvailableScreen(this, QSize(1280, 840));
    ResponsiveWindowUtils::manageWindowGeometry(this, "CompetitionQuickWindow");
}

CompetitionQuickWindow::~CompetitionQuickWindow()
{
    // Destroy QML bindings before their context and the Widgets state owner.
    delete m_view;
}
