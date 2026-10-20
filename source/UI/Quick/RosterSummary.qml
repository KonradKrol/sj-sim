import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: summary
    property var node
    readonly property var backend: node.values.backend
    padding: 16
    background: Surface { fillColor: Theme.surface; sweep: 28 }
    contentItem: ColumnLayout {
        spacing: 12
        Label { text: qsTr("Lista startowa"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
        Label { text: qsTr("Zawodnicy: %1").arg(summary.backend.count); color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body }
        SButton { objectName: "editStartList"; text: qsTr("Edytuj listę"); primary: true; Layout.fillWidth: true; onClicked: ui.edit(ui.root.values.id,1) }
    }
}
