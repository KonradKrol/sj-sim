import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: group
    property var node
    property bool focused: false
    readonly property var fields: node ? node.values : ({})
    readonly property bool panel: fields.kind === "panel"
    padding: panel ? (Theme.compact ? 6 : 16) : 0
    background: Surface { visible: group.panel; fillColor: Theme.surface; sweep: 28 }
    contentItem: Column {
        spacing: Theme.gap
        Label {
            visible: group.panel && !!group.fields.text
            width: parent.width
            text: group.fields.text || ""
            color: Theme.ink
            font.family: Theme.displayFamily
            font.pointSize: Theme.section
            font.weight: Font.ExtraBold
            wrapMode: Text.WordWrap
        }
        SCheck {
            visible: group.panel && !!group.fields.checkable
            text: qsTr("Włącz")
            checked: !!group.fields.value
            onClicked: ui.edit(group.fields.id, checked)
        }
        SectionTabs {
            visible: group.fields.kind === "tabs"
            width: parent.width
            options: group.fields.options || []
            optionEnabled: group.fields.optionEnabled || []
            currentIndex: group.fields.value || 0
            onSelected: ui.edit(group.fields.id, index)
        }
        GridLayout {
            id: grid
            width: parent.width
            columnSpacing: Theme.compact ? 8 : 16; rowSpacing: Theme.gap
            readonly property bool parallel: group.fields.kind === "row" || group.fields.kind === "split" || group.fields.kind === "sections" || group.fields.kind === "form"
            columns: parallel ? Math.max(1, Math.min(childrenRepeater.count, group.fields.kind === "form" ? 3 : 100, Math.floor((width + 16) / (group.fields.kind === "split" || group.fields.kind === "sections" ? 400 : group.fields.kind === "form" ? 220 : 260)))) : 1
            Repeater {
                id: childrenRepeater
                model: group.node ? group.node.children : []
                Loader {
                    id: childLoader
                    property var projectedNode: modelData
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: group.fields.weights && group.fields.weights.length > index ? group.fields.weights[index] : 240
                    Layout.preferredHeight: item ? item.implicitHeight : 0
                    source: Qt.resolvedUrl("Node.qml")
                    onLoaded: item.node = Qt.binding(function() { return childLoader.projectedNode })
                }
            }
        }
        Repeater {
            model: group.fields.shortcuts || []
            Item {
                width: 0; height: 0
                Shortcut { sequence: modelData.sequence; enabled: group.focused && group.visible && modelData.enabled && !(group.fields.menuActions && modelData.sequence === "Ctrl+A") && !(ui.editingText && ["Ctrl+A","Ctrl+D","Ctrl+Up","Ctrl+Down"].indexOf(modelData.sequence) >= 0); onActivated: ui.triggerAction(modelData.id) }
            }
        }
        Flow {
            width: parent.width
            spacing: Theme.compact ? 3 : 6
            visible: !group.fields.menuActions && !!group.fields.shortcuts && group.fields.shortcuts.length > 0
            Repeater {
                model: group.fields.shortcuts || []
                SButton { text: modelData.text; hint: modelData.sequence; enabled: modelData.enabled; onClicked: ui.triggerAction(modelData.id) }
            }
        }
    }
}
