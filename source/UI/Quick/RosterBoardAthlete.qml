import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: row
    property var athlete
    property bool selected: false
    property bool removable: false
    signal removed()
    implicitHeight: Math.max(Theme.dp(34),Theme.bodyMetrics.height+Theme.dp(12))
    Accessible.name: athlete.athleteName
    background: Rectangle { radius: Theme.dp(4); color: row.selected ? Theme.selection : Theme.surface; border.color: row.selected ? Theme.accent : Theme.divider }
    contentItem: RowLayout {
        spacing: Theme.dp(6)
        MouseArea {
            id: grip
            objectName: "rosterBoardGrip-"+row.athlete.athleteId
            Layout.preferredWidth: Theme.dp(24); Layout.fillHeight: true
            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
            property var athlete: row.athlete
            drag.target: preview
            onPressed: {var p=mapToItem(preview.parent,0,0);preview.x=p.x;preview.y=p.y}
            onReleased: preview.Drag.drop()
            Label { anchors.centerIn: parent; text: "⠿"; color: Theme.muted }
        }
        Image { source: row.athlete.flagSource || ""; sourceSize.width: 28; sourceSize.height: 20; Layout.preferredWidth: Theme.dp(20); Layout.preferredHeight: Theme.dp(14); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
        Label { text: row.athlete.athleteName; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
        Label { text: row.athlete.countryCode; color: Theme.muted; font.pointSize: Theme.caption }
        SButton { visible: row.removable; text: "×"; quiet: true; Layout.preferredWidth: Theme.dp(26); Accessible.name: qsTr("Usuń z listy"); onClicked: row.removed() }
    }
    Rectangle {
        id: preview; parent: Overlay.overlay
        visible: grip.drag.active; width: Theme.dp(220); height: Theme.dp(36); radius: Theme.dp(4)
        color: Theme.stadium; border.color: Theme.signal; z: 100
        Drag.active: grip.drag.active; Drag.source: grip; Drag.keys: ["roster-board"]; Drag.hotSpot.x: 10; Drag.hotSpot.y: 10
        Label { anchors.fill: parent; anchors.margins: Theme.dp(7); text: row.athlete.athleteName; color: Theme.snow; elide: Text.ElideRight; font.pointSize: Theme.body }
    }
}
