import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Pane {
    id: bar
    readonly property bool compressed: Window.window && Window.window.height < 500
    readonly property var dateNode: ui.find("quickSeasonDate")
    readonly property var nameNode: ui.find("label_saveName")
    readonly property var yearNode: ui.find("label_seasonNumber")
    readonly property var calendarNode: ui.find("label_calendarName")
    readonly property var saveNode: ui.find("pushButton_saveToFile")
    implicitWidth: 0
    padding: Theme.dp(compressed ? 5 : 8)
    background: Rectangle { color: Theme.surface; Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider } }
    contentItem: GridLayout {
        columns: bar.width < 760 && !bar.compressed ? 1 : 2
        columnSpacing: Theme.dp(16); rowSpacing: Theme.dp(5)
        RowLayout {
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            spacing: Theme.dp(10)
            Label { visible: !!bar.dateNode && bar.dateNode.values.available===true; text: bar.dateNode ? bar.dateNode.values.text : ""; color: Theme.accent; font.pointSize: Theme.body; font.bold: true; elide: Text.ElideRight; Layout.maximumWidth: bar.width * 0.38 }
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                Label { text: bar.nameNode ? bar.nameNode.values.text : ""; color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                Label { visible: bar.width>=760; text: qsTr("%1 · %2").arg(bar.yearNode ? bar.yearNode.values.text : "").arg(bar.calendarNode ? bar.calendarNode.values.text : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            }
        }
        RowLayout {
            Layout.fillWidth: bar.width < 760
            spacing: Theme.dp(6)
            Node { visible: bar.width >= 900; node: ui.find("pushButton_formGenerator"); Layout.preferredWidth: Theme.dp(145); Layout.preferredHeight: implicitHeight }
            Node { node: ui.find("quickSeasonTools"); Layout.preferredWidth: Theme.dp(110); Layout.fillWidth: bar.width < 760; Layout.preferredHeight: implicitHeight }
            SButton {
                objectName: "action-pushButton_saveToFile"
                visible: !!bar.saveNode && bar.saveNode.values.available !== false
                enabled: !!bar.saveNode && bar.saveNode.values.enabled !== false
                Layout.fillWidth: bar.width < 760
                implicitHeight: Math.max(Theme.controlHeight, Theme.dp(40))
                text: qsTr("Zapisz sezon")
                primary: true
                font.pointSize: Theme.body
                onClicked: ui.activate(bar.saveNode.values.id)
            }
        }
    }
}
