import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: sectors
    property var recorded: ({})
    readonly property var samples: recorded.during || []
    signal dismissed()
    implicitWidth: 0
    padding: Theme.dp(9)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: 12 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(4)
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Wiatr na zeskoku"); color: Theme.ink; font.bold: true; font.pointSize: Theme.body; Layout.fillWidth: true }
            Label { text: "m/s"; color: Theme.muted; font.pointSize: Theme.caption }
            SButton { text: "×"; quiet: true; dense: true; implicitWidth: Theme.dp(24); Accessible.name: qsTr("Ukryj sektory wiatru"); onClicked: sectors.dismissed() }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(3)
            Repeater {
                model: sectors.samples
                ColumnLayout {
                    required property var modelData
                    Layout.fillWidth: true; Layout.preferredWidth: 0; spacing: Theme.dp(2)
                    Label { Layout.alignment: Qt.AlignHCenter; text: modelData.available ? "↑" : "—"; rotation: modelData.available ? modelData.direction-65 : 0; color: !modelData.available ? Theme.muted : modelData.head>0 ? Theme.success : modelData.head<0 ? Theme.danger : Theme.muted; font.pixelSize: Theme.dp(28) }
                    Label { text: modelData.available ? Number(modelData.strength).toLocaleString(Qt.locale(Theme.localeName),'f',1) : "—"; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body; Layout.alignment: Qt.AlignHCenter }
                    Label { text: Math.round(modelData.distance)+" m"; color: Theme.muted; font.pointSize: Theme.caption; Layout.alignment: Qt.AlignHCenter }
                }
            }
        }
        WindMeasurementComparison { recorded: sectors.recorded; Layout.fillWidth: true }
    }
}
