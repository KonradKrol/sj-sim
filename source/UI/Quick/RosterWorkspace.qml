import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: roster
    objectName: "rosterWorkspace"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var backend: fields.backend
    readonly property bool compact: width < 800
    property bool choosing: false
    property bool showPresets: false
    implicitHeight: 580
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        RowLayout {
            visible: !!roster.fields.nameField
            Layout.fillWidth: true
            ProjectedControl { node: roster.fields.nameField || null; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            ProjectedControl { node: roster.fields.defaultField || null; Layout.fillWidth: true; Layout.minimumWidth: 0 }
        }
        RowLayout {
            Layout.fillWidth: true
            SButton { objectName: "rosterChoose"; visible: roster.backend.membership && roster.compact; text: roster.choosing ? qsTr("Lista · %1").arg(roster.backend.count) : qsTr("Dodaj zawodników"); primary: !roster.choosing; Layout.fillWidth: true; onClicked: roster.choosing=!roster.choosing }
            Label { visible: !roster.compact; text: roster.backend.membership ? qsTr("Wybór i kolejność") : qsTr("Kolejność skoczków"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true }
            SButton { objectName: "rosterUndo"; text: qsTr("Cofnij"); enabled: roster.backend.canUndo; quiet: true; onClicked: roster.backend.undo() }
            SButton {
                text: "⋯"; quiet: true; Accessible.name: qsTr("Opcje listy"); onClicked: options.open()
                ActionMenu {
                    id: options
                    entries: [{id:"name",text:qsTr("Według nazwiska")},{id:"country",text:qsTr("Według kraju")},{id:"random",text:qsTr("Losowo")},{id:"preset",text:qsTr("Zapisane listy"),enabled:roster.backend.presets.length > 0}].concat(roster.fields.externalActions || [])
                    onSelected: {if(entry.button)ui.activate(entry.id);else if(entry.id === "preset")roster.showPresets=!roster.showPresets;else roster.backend.sort(entry.id)}
                }
            }
        }
        RowLayout {
            visible: roster.showPresets && roster.backend.presets.length > 0
            Layout.fillWidth: true
            SChoice { id: preset; Layout.fillWidth: true; Layout.minimumWidth: 0; model: roster.backend.presets; Accessible.name: qsTr("Zapisana lista") }
            SButton {
                text: qsTr("Zastosuj"); onClicked: presetActions.open()
                ActionMenu { id: presetActions; entries: [{id:0,text:qsTr("Dodaj zawodników")},{id:1,text:qsTr("Usuń zawodników")},{id:2,text:qsTr("Ustaw kolejność")}]; onSelected: roster.backend.applyPreset(preset.currentIndex,entry.id) }
            }
        }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: 20
            RosterLibrary { visible: roster.backend.membership && (!roster.compact || roster.choosing); backend: roster.backend; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 380 }
            Rectangle { visible: roster.backend.membership && !roster.compact; Layout.fillHeight: true; Layout.preferredWidth: 1; color: Theme.divider }
            RosterOrder { visible: !roster.compact || !roster.choosing || !roster.backend.membership; backend: roster.backend; title: roster.backend.membership ? qsTr("Lista startowa") : qsTr("Drużyna"); Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 500 }
        }
        ProjectedControl { visible: !!roster.fields.submit; node: roster.fields.submit || null; Layout.fillWidth: true; Layout.preferredHeight: visible ? implicitHeight : 0 }
    }
    Shortcut { sequence: "Ctrl+Z"; enabled: roster.visible && roster.backend.canUndo && !ui.editingText; onActivated: roster.backend.undo() }
}
