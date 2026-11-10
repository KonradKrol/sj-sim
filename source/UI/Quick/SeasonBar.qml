import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Pane {
    id: bar
    objectName: "seasonBar"
    readonly property bool compressed: Window.window && Window.window.height < 650
    readonly property bool stacked: width < actions.implicitWidth+Theme.dp(160)
    readonly property var toolsNode: ui.find("quickSeasonTools")
    readonly property var dateNode: ui.find("quickSeasonDate")
    readonly property var nameNode: ui.find("label_saveName")
    readonly property var yearNode: ui.find("label_seasonNumber")
    readonly property var calendarNode: ui.find("label_calendarName")
    readonly property var saveNode: ui.find("pushButton_saveToFile")
    implicitWidth: 0
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    padding: Theme.dp(compressed ? 3 : 5)
    background: Rectangle { color: Theme.surface; Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider } }
    contentItem: Item {
        implicitHeight: bar.stacked ? identity.implicitHeight+Theme.dp(4)+actions.implicitHeight : Math.max(identity.implicitHeight,actions.implicitHeight)
        RowLayout {
            id: identity
            width: bar.stacked ? parent.width : Math.max(0,parent.width-actions.implicitWidth-Theme.dp(12))
            height: implicitHeight
            spacing: Theme.dp(10)
            Label { visible: !!bar.dateNode && bar.dateNode.values.available===true; text: bar.dateNode ? bar.dateNode.values.text : ""; color: Theme.accent; font.pointSize: Theme.body; font.bold: true; elide: Text.ElideRight; Layout.maximumWidth: bar.width * 0.38 }
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                Label { text: bar.nameNode ? bar.nameNode.values.text : ""; color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                Label { text: qsTr("%1 · %2").arg(bar.yearNode ? bar.yearNode.values.text : "").arg(bar.calendarNode ? bar.calendarNode.values.text : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            }
        }
        Item {
            id: actions
            implicitWidth: toolsButton.implicitWidth+(saveButton.visible ? Theme.dp(6)+saveButton.implicitWidth : 0)
            implicitHeight: Math.max(toolsButton.implicitHeight,saveButton.visible ? saveButton.implicitHeight : 0)
            width: bar.stacked ? parent.width : implicitWidth
            height: implicitHeight
            x: bar.stacked ? 0 : parent.width-width
            y: bar.stacked ? identity.height+Theme.dp(4) : 0
            SToolButton {
                id: toolsButton
                width: implicitWidth; height: implicitHeight
                anchors.verticalCenter: parent.verticalCenter
                objectName: "action-quickSeasonTools"
                text: bar.toolsNode ? bar.toolsNode.values.text : ""
                font.pointSize: Theme.body
                enabled: !!bar.toolsNode
                onClicked: toolsMenu.open()
                ActionMenu { id: toolsMenu; entries: bar.toolsNode ? bar.toolsNode.values.actions || [] : []; onSelected: function(entry){if(entry.button)ui.activate(entry.id);else ui.triggerAction(entry.id)} }
            }
            SButton {
                id: saveButton
                objectName: "action-pushButton_saveToFile"
                visible: !!bar.saveNode && bar.saveNode.values.available !== false
                enabled: !!bar.saveNode && bar.saveNode.values.enabled !== false
                x: toolsButton.width+Theme.dp(6)
                width: bar.stacked ? Math.max(0,parent.width-x) : implicitWidth
                height: implicitHeight
                anchors.verticalCenter: parent.verticalCenter
                dense: true
                implicitHeight: Math.max(Theme.dp(32),contentItem.implicitHeight+Theme.dp(8))
                text: qsTr("Zapisz sezon")
                primary: true
                font.pointSize: Theme.body
                onClicked: ui.activate(bar.saveNode.values.id)
            }
        }
}
}
