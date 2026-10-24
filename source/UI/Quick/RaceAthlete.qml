import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: athlete
    objectName: "raceAthlete"
    readonly property var nameNode: ui.find("label_nameAndSurname")
    readonly property var flagNode: ui.find("label_flag")
    readonly property var beatNode: ui.find("label_toBeatDistance")
    readonly property var advanceNode: ui.find("label_toAdvancementDistance")
    property bool dense: false
    padding: dense ? 8 : 12
    background: Surface { fillColor: Theme.stadium; sweep: 28 }
    contentItem: ColumnLayout {
        spacing: 4
        Label { text: ui.context.complete ? qsTr("Zakończono") : ui.context.roundComplete ? qsTr("Koniec serii") : qsTr("NA BELCE"); color: Theme.signal; font.pointSize: Theme.caption; font.family: Theme.mono }
        RowLayout {
            Layout.fillWidth: true
            Image { source: athlete.flagNode ? athlete.flagNode.values.image || "" : ""; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: 28; Layout.preferredHeight: 20; fillMode: Image.PreserveAspectFit }
            Label { text: athlete.nameNode ? athlete.nameNode.values.text : "—"; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            ColumnLayout {
                visible: ui.context.previousPosition>0; spacing: 0
                Label { objectName: "athletePreviousPosition"; text: ui.context.previousPosition>0 ? ui.context.previousPosition+"." : "—"; color: Theme.signal; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold; Layout.alignment: Qt.AlignRight }
                Label { text: qsTr("po %1 serii").arg(ui.context.round-1); color: Theme.headerMuted; font.pointSize: Theme.caption-1 }

            }
        }
        Label {
            objectName: "athleteCurrentGap"
            visible: ui.context.currentGap !== undefined
            readonly property real gap: Number(ui.context.currentGap || 0)
            text: gap<0 ? qsTr("Ma przewagę %1 pkt").arg(Math.abs(gap).toLocaleString(Qt.locale(),'f',1)) : qsTr("Traci %1 pkt").arg(gap.toLocaleString(Qt.locale(),'f',1))
            color: gap<0 ? Theme.signal : gap>0 ? Theme.headerMuted : Theme.snow
            font.pointSize: Theme.caption; font.family: Theme.mono
            Layout.fillWidth: true; wrapMode: Text.WordWrap
            ToolTip.visible: gapHover.containsMouse; ToolTip.delay: 600
            ToolTip.text: qsTr("Punkty sprzed bieżącej serii, po wagach, porównane z punktami aktualnego lidera sprzed tej samej serii.")
            MouseArea { id: gapHover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: 16
            ColumnLayout {
                Layout.fillWidth: true; spacing: 0
                Label { text: qsTr("Do prowadzenia"); color: Theme.headerMuted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: athlete.beatNode ? athlete.beatNode.values.text : "—"; color: Theme.snow; font.pointSize: Theme.body; font.family: Theme.mono; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            ColumnLayout {
                Layout.fillWidth: true; spacing: 0
                Label { text: qsTr("Do awansu"); color: Theme.headerMuted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: athlete.advanceNode ? athlete.advanceNode.values.text : "—"; color: Theme.snow; font.pointSize: Theme.body; font.family: Theme.mono; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
        }
    }
}
