import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: jump
    objectName: "raceLastJump"
    property bool dense: false
    padding: dense ? 8 : 12
    background: Rectangle {
        color: Theme.surface; border.color: Theme.outline
        Rectangle { width: 24; height: 3; color: Theme.signal }
    }
    contentItem: ColumnLayout {
        spacing: 4
        Label { text: ui.context.liveStage ? ["",qsTr("Lot"),qsTr("Odległość"),qsTr("Lądowanie"),qsTr("Noty"),qsTr("Wynik")][ui.context.liveStage] : qsTr("Ostatni skok"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: ui.jumpSummary.name || "—"; color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; elide: Text.ElideRight }
        RowLayout {
            Layout.fillWidth: true; spacing: 16
            Label { objectName: "lastJumpDistance"; text: ui.jumpSummary.distance || "—"; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { objectName: "lastJumpPoints"; text: ui.jumpSummary.points || "—"; color: Theme.accent; font.family: Theme.mono; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
        }
    }
}
