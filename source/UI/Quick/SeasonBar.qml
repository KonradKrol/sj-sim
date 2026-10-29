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
    implicitWidth: 0
    padding: compressed ? 8 : 12
    background: Rectangle { color: Theme.surface }
    contentItem: GridLayout {
        columns: bar.width < 700 ? 1 : 2
        columnSpacing: 24; rowSpacing: 8
        ColumnLayout {
            visible: bar.width >= 700 || !!bar.dateNode && bar.dateNode.values.available===true
            Layout.fillWidth: true
            spacing: 2
            Label { visible: !!bar.dateNode && bar.dateNode.values.available===true; text: bar.dateNode ? bar.dateNode.values.text : ""; color: Theme.accent; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: bar.width>=700; text: bar.nameNode ? bar.nameNode.values.text : ""; color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: bar.width>=700; text: qsTr("%1 · %2").arg(bar.yearNode ? bar.yearNode.values.text : "").arg(bar.calendarNode ? bar.calendarNode.values.text : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
        }
        RowLayout {
            Layout.fillWidth: bar.width < 600
            Node { visible: !bar.compressed || bar.width >= 600; node: ui.find("pushButton_formGenerator"); Layout.fillWidth: true; Layout.preferredHeight: implicitHeight }
            Node { node: ui.find("quickSeasonTools"); Layout.fillWidth: true; Layout.preferredHeight: implicitHeight }
            Node { node: ui.find("pushButton_saveToFile"); Layout.fillWidth: true; Layout.preferredHeight: implicitHeight }
        }
    }
}
