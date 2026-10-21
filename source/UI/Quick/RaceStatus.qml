import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: status
    signal conditionsRequested()
    readonly property var hill: ui.find("label_hillNameAndHSPoint")
    readonly property var wind: ui.find("label_actualAvgWind")
    readonly property var snow: ui.find("label_snowLevel")
    padding: 6
    background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
    contentItem: GridLayout {
        columns: status.width >= 850 ? 4 : 2
        columnSpacing: 20; rowSpacing: 8
        Label { text: status.hill ? status.hill.values.text : ""; font.pointSize: Theme.caption; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: qsTr("Seria %1 / %2 · Start %3 / %4").arg(ui.context.round || 1).arg(ui.context.roundCount || 1).arg(ui.context.athlete || 1).arg(ui.context.athleteCount || 0) + (ui.context.group ? qsTr(" · Grupa %1").arg(ui.context.group) : ""); font.pointSize: Theme.caption; color: Theme.muted; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SButton { objectName: "raceWindConditions"; implicitHeight: 24; padding: 0; quiet: true; onClicked: status.conditionsRequested(); text: qsTr("Wiatr: %1").arg(status.wind ? status.wind.values.text : "—"); font.pointSize: Theme.caption; Layout.fillWidth: true }
        SButton { objectName: "raceSnowConditions"; implicitHeight: 24; padding: 0; quiet: true; onClicked: status.conditionsRequested(); text: qsTr("Śnieg: %1").arg(status.snow ? status.snow.values.text : "—"); font.pointSize: Theme.caption; Layout.fillWidth: true }
    }
}
