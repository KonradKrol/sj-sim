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
    padding: 12
    background: Surface { fillColor: Theme.stadium; sweep: 28 }
    contentItem: ColumnLayout {
        spacing: 4
        Label { text: ui.context.complete ? qsTr("Zakończono") : ui.context.roundComplete ? qsTr("Koniec serii") : qsTr("NA BELCE"); color: Theme.signal; font.pointSize: Theme.caption; font.family: Theme.mono }
        RowLayout {
            Layout.fillWidth: true
            Image { source: athlete.flagNode ? athlete.flagNode.values.image || "" : ""; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: 28; Layout.preferredHeight: 20; fillMode: Image.PreserveAspectFit }
            Label { text: athlete.nameNode ? athlete.nameNode.values.text : "—"; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
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
