import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: workspace
    objectName: "savedRosterWorkspace"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var backend: fields.backend
    readonly property bool narrow: width < Theme.dp(760)
    property bool showLibrary: true
    property bool limitsOpen: false
    implicitHeight: 580
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.dp(12)
        RowLayout {
            Layout.fillWidth: true
            ProjectedControl { node: workspace.fields.nameField || null; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            SButton { text: qsTr("Cofnij"); enabled: workspace.backend.canUndo; onClicked: workspace.backend.undo() }
        }
        ProjectedControl { node: workspace.fields.defaultField || null; visible: !!workspace.fields.defaultField; Layout.fillWidth: true }
        RowLayout {
            Layout.fillWidth: true
            SectionTabs {
                Layout.fillWidth: true; Layout.minimumWidth: 0
                optionNamePrefix: "savedRosterView-"
                options: [qsTr("Według krajów"), qsTr("Kolejność")]
                currentIndex: workspace.backend.grouped ? 0 : 1
                onSelected: workspace.backend.grouped = index === 0
            }
            SButton { objectName: "rosterLimitsToggle"; text: qsTr("Limity krajów"); primary: workspace.limitsOpen; onClicked: workspace.limitsOpen = !workspace.limitsOpen }
        }
        RosterLimits { visible: workspace.limitsOpen; backend: workspace.backend; Layout.fillWidth: true; listHeight: Math.min(Theme.dp(150), workspace.height * 0.25) }
        SectionTabs {
            visible: workspace.narrow
            Layout.fillWidth: true
            options: [qsTr("Wszyscy"), qsTr("Wybrani (%1)").arg(workspace.backend.count)]
            currentIndex: workspace.showLibrary ? 0 : 1
            onSelected: workspace.showLibrary = index === 0
        }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: Theme.dp(12)
            SavedRosterPane {
                visible: !workspace.narrow || workspace.showLibrary
                backend: workspace.backend
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
            }
            SavedRosterPane {
                visible: !workspace.narrow || !workspace.showLibrary
                backend: workspace.backend; selected: true
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
            }
        }
        ProjectedControl { node: workspace.fields.submit || null; visible: !!workspace.fields.submit; Layout.fillWidth: true }
    }
    Shortcut { sequence: "Ctrl+Z"; enabled: workspace.visible && workspace.backend.canUndo && !ui.editingText; onActivated: workspace.backend.undo() }
}
