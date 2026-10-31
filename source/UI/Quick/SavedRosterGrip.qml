import QtQuick 2.15
import QtQuick.Controls 2.15

MouseArea {
    id: grip
    property var backend
    property var athlete
    property bool selected: false
    width: Theme.dp(24); height: Theme.dp(28)
    cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
    drag.target: preview; drag.threshold: Theme.dp(5)
    Accessible.role: Accessible.Button
    Accessible.name: qsTr("Przeciągnij %1").arg(athlete.athleteName || "")
    onPressed: {
        var point=mapToItem(preview.parent,0,0)
        preview.x=point.x;preview.y=point.y
    }
    onReleased: {preview.Drag.drop();preview.x=0;preview.y=0}
    Label { anchors.centerIn: parent; text: "⠿"; color: Theme.muted; font.pointSize: Theme.body }
    Rectangle {
        id: preview
        parent: Overlay.overlay
        visible: grip.drag.active
        width: Theme.dp(230); height: Theme.dp(38); radius: Theme.dp(5); z: 100
        color: Theme.stadium; border.color: Theme.signal
        Drag.active: grip.drag.active
        Drag.source: grip; Drag.keys: ["saved-roster"]; Drag.hotSpot.x: 10; Drag.hotSpot.y: 10
        Label { anchors.fill: parent; anchors.margins: Theme.dp(8); text: grip.athlete.athleteName || ""; color: Theme.snow; elide: Text.ElideRight; font.pointSize: Theme.body }
    }
}
