import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: workspace
    objectName: "teamRosterWorkspace"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var backend: fields.backend
    RowLayout {
    anchors.fill: parent
    spacing: Theme.dp(18)
    ColumnLayout {
        Layout.fillHeight: true; Layout.minimumHeight: 0
        Layout.preferredWidth: Math.min(Theme.dp(380),workspace.width*0.4)
        Layout.minimumWidth: Theme.dp(200); Layout.maximumWidth: Theme.dp(420)
        spacing: Theme.dp(8)
        Label {
            visible: !!workspace.fields.locked
            text: qsTr("Składy ustalono przed wcześniejszym wydarzeniem.")
            color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
        }
        RosterLibrary {
            objectName: "teamEligibilityLibrary"
            backend: workspace.backend; heading: qsTr("Mogą startować")
            enabled: !workspace.fields.locked
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: workspace.backend ? qsTr("Dostępni: %1").arg(workspace.backend.count) : ""; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            SButton { text: qsTr("Cofnij"); dense: true; quiet: true; enabled: !workspace.fields.locked && workspace.backend && workspace.backend.canUndo; onClicked: workspace.backend.undo() }
        }
    }
    Rectangle { Layout.fillHeight: true; implicitWidth: 1; color: Theme.divider }
    Node {
        node: workspace.fields.card || null
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
        tableHeightLimit: Math.max(Theme.dp(150),workspace.height-Theme.dp(145))
        fillTableHeight: true
    }
}
}
