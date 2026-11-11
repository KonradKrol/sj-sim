import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ColumnLayout {
    id: stats
    property var node
    readonly property var backend: node ? node.values.backend : null
    readonly property var report: backend ? backend.report : ({})
    spacing: Theme.dp(16)
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(12)
        Image { visible: !!stats.report.photo; source: stats.report.photo || ""; sourceSize.width: 96; sourceSize.height: 112; fillMode: Image.PreserveAspectFit; Layout.preferredWidth: Theme.dp(42); Layout.preferredHeight: Theme.dp(50) }
        Label { text: stats.report.name || ""; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
        Image { source: stats.report.flag || ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(16); fillMode: Image.PreserveAspectFit; Accessible.name: stats.report.country || "" }
    }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(420) ? 4 : 2
        columnSpacing: Theme.dp(20); rowSpacing: Theme.dp(10)
        Repeater {
            model: [{label:qsTr("Najlepsze miejsce"),value:stats.report.best},{label:qsTr("Średnie miejsce"),value:stats.report.mean},{label:qsTr("Najgorsze miejsce"),value:stats.report.worst},{label:qsTr("Odchylenie"),value:stats.report.deviation}]
            Column {
                required property var modelData
                Layout.fillWidth: true; spacing: Theme.dp(3)
                Label { text: modelData.value || "—"; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.section; font.bold: true }
                Label { text: modelData.label; color: Theme.muted; font.pointSize: Theme.caption }
            }
        }
    }
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(6)
        Repeater {
            model: [{label:qsTr("Treningi"),type:3},{label:qsTr("Próbne"),type:2},{label:qsTr("Kwalifikacje"),type:1},{label:qsTr("Konkursy"),type:0}]
            SButton {
                required property var modelData
                required property int index
                text: modelData.label; checkable: true; dense: true; primary: checked
                font.family: Theme.bodyFamily; font.pointSize: Theme.body
                checked: !!(stats.report.eventFilters || [])[index]
                onClicked: stats.backend.setEventFilter(modelData.type,checked)
            }
        }
    }
    AthletePositionPlot {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: Theme.dp(200)
        Layout.preferredHeight: Math.max(Theme.dp(260),Math.min(Theme.dp(520),stats.Window.window ? stats.Window.window.height-Theme.dp(300) : Theme.dp(400)))
        points: stats.report.positions || []
    }
}
