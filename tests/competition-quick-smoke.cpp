#include "../source/UI/Competition/Quick/CompetitionQuickBridge.h"
#include "../source/UI/Competition/CompetitionConfigWindow.h"
#include "../source/UI/EditorWidgets/HillEditorWidget.h"
#include "../source/UI/EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../source/UI/Competition/CompetitionManagerWindow.h"
#include "../source/global/GlobalDatabase.h"
#include "../source/global/GlobalAppSettings.h"
#include "../source/global/GlobalSimulationSettings.h"
#include <QApplication>
#include <QDir>
#include <QQuickWidget>
#include <QQuickItem>
#include <QSpinBox>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QTemporaryDir>
#include <QDirIterator>
#include <QFile>
#include <QtTest/QTest>
#include <QDebug>
#include <cstdlib>

static void check(bool condition, const char *message) {
    if (!condition) { qCritical() << "FAIL:" << message; std::exit(1); }
}
static QQuickItem *findItem(QQuickItem *root, const QString &name) {
    if (root->objectName() == name) return root;
    for (auto *child : root->childItems())
        if (auto *found = findItem(child, name)) return found;
    return nullptr;
}
static QVariantMap findField(CompetitionQuickBridge *bridge, const QString &label) {
    for (const auto &card : bridge->cards())
        for (const auto &field : card.toMap().value("fields").toList())
            if (field.toMap().value("label").toString() == label) return field.toMap();
    qFatal("Missing field: %s", qPrintable(label));
    return {};
}
static void set(CompetitionQuickBridge *bridge, const QString &label, const QVariant &value) {
    bridge->setField(findField(bridge, label).value("id").toInt(), value);
    QTest::qWait(30);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("SJ-Sim-Quick-Smoke");
    QCoreApplication::setApplicationName("SJ-Sim-Quick-Smoke");
    // Simulation writes results/database records. Always test in a disposable
    // copy, never in the preview or user's live data directory.
    QTemporaryDir data;
    check(data.isValid(), "temporary test data directory");
    const QString source = QCoreApplication::applicationDirPath();
    for (const QString directory : {QString("userData"), QString("flags")}) {
        QDirIterator files(source + "/" + directory, QDir::Files, QDirIterator::Subdirectories);
        while (files.hasNext()) {
            const QString file = files.next();
            const QString relative = QDir(source).relativeFilePath(file);
            const QString destination = data.path() + "/" + relative;
            QDir().mkpath(QFileInfo(destination).absolutePath());
            check(QFile::copy(file, destination), "copy test data");
        }
    }
    QDir::setCurrent(data.path());
    GlobalDatabase::get()->loadFromJson();
    GlobalAppSettings::get()->loadFromJson();
    GlobalSimulationSettings::get()->loadFromFile();
    CompetitionQuickWindow window;
    window.show();
    QTest::qWait(200);
    check(window.view()->status() == QQuickWidget::Ready, "QML loads");
    auto *bridge = window.bridge();
    auto *original = bridge->configuration();
    // Exercise the actual QML popup: a populated C++ model alone does not
    // guarantee that the choices are visible or selectable in Qt Quick.
    auto *hillChoice = findItem(window.view()->rootObject(), "quick-Skocznia z bazy");
    check(hillChoice != nullptr, "QML hill selector");
    check(hillChoice->property("count").toInt() > 1, "hill choices loaded");
    auto *hillPopup = hillChoice->property("popup").value<QObject *>();
    check(hillPopup != nullptr, "hill popup exists");
    QMetaObject::invokeMethod(hillPopup, "open");
    QTest::qWait(100);
    check(hillPopup->property("visible").toBool(), "hill popup opens");
    check(hillPopup->property("height").toDouble() >= 100, "hill choices have visible popup height");
    auto *hillList = hillPopup->property("contentItem").value<QObject *>();
    check(hillList && hillList->property("count").toInt() == hillChoice->property("count").toInt(), "popup shows all hill choices");
    window.activateWindow();
    window.view()->setFocus();
    hillChoice->forceActiveFocus();
    QTest::keyClick(window.view(), Qt::Key_Down);
    QTest::keyClick(window.view(), Qt::Key_Return);
    QTest::qWait(100);
    check(hillChoice->property("currentIndex").toInt() == 1, "select hill from open popup using keyboard");
    check(original->getHillEditor()->getKPointFromInput() > 0, "visible hill selection reaches C++ configuration");
    QMetaObject::invokeMethod(hillPopup, "close");
    set(bridge, "Skocznia z bazy", 1);
    set(bridge, "Zasady z bazy", 1);
    set(bridge, "Belka startowa", 17);
    check(original->getStartingGateFromInput() == 17, "QML -> C++ gate");
    original->findChild<QSpinBox *>("spinBox_startGate")->setValue(23);
    bridge->refresh();
    check(findField(bridge, "Belka startowa").value("value").toInt() == 23, "Widgets -> QML gate");
    set(bridge, "Przeprowadź kwalifikacje", true);
    check(bridge->summary().value("qualifications").toBool(), "qualifications state");
    set(bridge, "Przeprowadź kwalifikacje", false);
    auto *gateInput = findItem(window.view()->rootObject(), "quick-Belka startowa");
    check(gateInput != nullptr, "QML gate control");
    window.activateWindow();
    window.view()->setFocus();
    gateInput->forceActiveFocus();
    QTest::qWait(30);
    QTest::keyClick(window.view(), Qt::Key_A, Qt::ControlModifier);
    QTest::keyClicks(window.view(), "31");
    findItem(window.view()->rootObject(), "commitFocus")->forceActiveFocus();
    QTest::qWait(60);
    check(original->getStartingGateFromInput() == 31, "keyboard input commits to original configuration");
    bridge->setSection(2);
    set(bridge, "Liczba serii", 3);
    check(original->getCompetitionRulesEditor()->getRoundsFromInput().size() == 3, "round creation");
    check(bridge->cards().size() == 5, "all rounds exposed");
    set(bridge, "Rodzaj konkursu", 1);
    check(bridge->summary().value("team").toBool(), "team configuration");
    set(bridge, "Rodzaj konkursu", 0);
    set(bridge, "Seria KO", true);
    check(bridge->summary().value("ko").toBool(), "KO configuration");
    set(bridge, "Seria KO", false);
    bridge->setSection(1);
    set(bridge, "Punkt K · m", 120.0);
    check(original->getHillEditor()->getKPointFromInput() == 120, "hill geometry");
    bridge->setSection(3);
    check(bridge->cards().size() > 1, "wind sectors created");
    set(bridge, "Bazowa siła · m/s", 1.75);
    check(qAbs(original->getWindGeneratorSettingsEditor()->getWindsGenerationSettingsFromInputs().first().getBaseWindStrength() - 1.75) < .001, "wind input survives bridge");
    const int count = original->getCompetitionJumpers().size();
    bridge->action("remove", {1, 3});
    QTest::qWait(100);
    check(original->getCompetitionJumpers().size() == count - 2, "multi-selection removal");

    // The real Widgets dialog reuses exactly the same configuration instance.
    QTimer::singleShot(100, original, [original] {
        original->findChild<QSpinBox *>("spinBox_startGate")->setValue(29);
        original->reject();
    });
    bridge->action("widgets");
    QTest::qWait(200);
    check(bridge->summary().value("gate").toInt() == 29, "Widgets round trip");

    // Render every section at desktop, intermediate and compact widths.
    for (const QSize size : {QSize(1440, 900), QSize(900, 700), QSize(520, 720)}) {
        window.resize(size);
        for (int section = 0; section < 5; ++section) {
            bridge->setSection(section);
            QTest::qWait(250);
            check(window.view()->rootObject()->width() == window.view()->width(), "responsive root width");
            const auto screenshot = window.view()->grabFramebuffer();
            check(!screenshot.isNull(), "rendered frame");
            screenshot.save(source + QString("/quick-%1-%2.png").arg(size.width()).arg(section));
        }
    }
    // Invalid input must still use the original C++ validation.
    bridge->setSection(2);
    set(bridge, "Liczba serii", 0);
    bool validationShown = false;
    QTimer closer;
    QObject::connect(&closer, &QTimer::timeout, [&] {
        for (auto *widget : QApplication::topLevelWidgets()) {
            if (auto *message = qobject_cast<QMessageBox *>(widget); message && message->isVisible()) {
                validationShown = true;
                message->accept();
            }
        }
    });
    closer.start(40);
    bridge->action("start");
    QTest::qWait(200);
    closer.stop();
    check(validationShown, "original validation invoked");
    // A valid setup reaches the actual competition manager (not a mock).
    bridge->setSection(0);
    set(bridge, "Zasady z bazy", 1);
    set(bridge, "Skocznia z bazy", 1);
    // Selecting an already active preset does not reload it in the legacy editor.
    set(bridge, "Zasady z bazy", 0);
    set(bridge, "Zasady z bazy", 1);
    bool managerShown = false;
    QTimer managerCloser;
    QObject::connect(&managerCloser, &QTimer::timeout, [&] {
        for (auto *widget : QApplication::topLevelWidgets()) {
            if (auto *manager = qobject_cast<CompetitionManagerWindow *>(widget); manager && manager->isVisible()) {
                managerShown = true;
                manager->reject();
            }
            if (auto *message = qobject_cast<QMessageBox *>(widget); message && message->isVisible())
                message->accept();
        }
    });
    managerCloser.start(40);
    bridge->action("start");
    QTest::qWait(300);
    managerCloser.stop();
    check(managerShown, "actual competition manager opens");
    qInfo() << "PASS: QML loading, bridge round trip, dynamic rounds/wind/KO/teams, multi-selection, validation, live manager, 15 responsive renders";
    return 0;
}
