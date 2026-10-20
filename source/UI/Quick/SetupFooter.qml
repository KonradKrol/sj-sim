import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: footer
    readonly property var submit: ui.find("pushButton_submit")
    padding: 12
    background: Rectangle { color: Theme.surface; border.color: Theme.outline }
    function start() {
        footer.forceActiveFocus()
        if (submit && submit.values.enabled) ui.activate(submit.values.id)
    }
    contentItem: RowLayout {
        spacing: 16
        Label { visible: footer.width >= 700; text: qsTr("Ctrl+Enter · rozpocznij symulację"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        Node { node: footer.submit; Layout.fillWidth: true; Layout.maximumWidth: footer.width >= 700 ? 320 : 10000; Layout.preferredHeight: implicitHeight }
    }
    Shortcut { sequence: "Ctrl+Return"; onActivated: footer.start() }
}
