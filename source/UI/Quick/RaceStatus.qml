import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: status
    signal conditionsRequested()
    readonly property var hill: ui.find("label_hillNameAndHSPoint")
    readonly property var wind: ui.find("label_actualAvgWind")
    readonly property var snow: ui.find("label_snowLevel")
    readonly property var advancement: ui.context.biggestAdvancement || ({})
    readonly property var advancingEntries: advancement.entries || []
    padding: 6
    background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
    contentItem: GridLayout {
        columns: status.width >= 850 ? 4 : 2
        columnSpacing: 20; rowSpacing: 8
        RowLayout {
            Layout.fillWidth: true; spacing: 6
            Image { source: ui.context.hillFlag || ""; sourceSize.width: 48; sourceSize.height: 36; Layout.preferredWidth: 24; Layout.preferredHeight: 18; fillMode: Image.PreserveAspectFit }
            Label { text: status.hill ? status.hill.values.text : ""; font.pointSize: Theme.caption; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        Label {
            objectName: "raceRoundStatus"
            text: qsTr("Seria %1 / %2").arg(ui.context.round || 1).arg(ui.context.roundCount || 1)
                  +(ui.context.group ? qsTr(" · Grupa %1").arg(ui.context.group) : "")
                  +(status.advancement.places ? (status.width<600 ? "\n" : " · ")
                    +(status.width<600 ? qsTr("Awans: %1 ↑%2") : qsTr("Największy awans: %1 ↑%2")).arg(status.advancement.name).arg(status.advancement.places)
                    +(status.advancingEntries.length>1 ? " (+"+(status.advancingEntries.length-1)+")" : "") : "")
            font.pointSize: Theme.caption; color: Theme.muted
            Layout.fillWidth: true; wrapMode: Text.WordWrap
            MouseArea { id: advancementHover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }
            ToolTip.visible: advancementHover.containsMouse && status.advancement.places>0
            ToolTip.text: qsTr("Względem poprzedniej serii, po odjęciu zawodników oczekujących na skok.")
                          +"\n"+status.advancingEntries.map(function(entry){return entry.name+" · "+entry.from+" → "+entry.to}).join("\n")
        }

        SButton { objectName: "raceWindConditions"; foreground: ui.context.windValue>0 ? Theme.success : ui.context.windValue<0 ? Theme.danger : Theme.ink; implicitHeight: 24; padding: 0; quiet: true; onClicked: status.conditionsRequested(); text: qsTr("Wiatr: %1").arg(status.wind ? status.wind.values.text : "—"); font.pointSize: Theme.caption; Layout.fillWidth: true }
        SButton { objectName: "raceSnowConditions"; implicitHeight: 24; padding: 0; quiet: true; onClicked: status.conditionsRequested(); text: qsTr("Śnieg: %1").arg(status.snow ? status.snow.values.text : "—"); font.pointSize: Theme.caption; Layout.fillWidth: true }
    }
}
