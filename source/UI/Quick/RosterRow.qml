import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Item {
    id: row
    property var backend
    property string athleteId
    property string athleteName
    property string countryCode
    property bool included: false
    property bool marked: false
    property int position: 0
    property bool activeSlot: true
    property bool ordered: false
    property var viewport
    signal moveRequested()
    implicitHeight: Theme.compact ? Math.max(30,Theme.bodyMetrics.height+(ordered && backend.activeLimit > 0 ? Theme.captionMetrics.height : 0)+6) : Math.max(backend.activeLimit > 0 ? 56 : 44,Theme.bodyMetrics.height+16)
    objectName: (ordered ? "roster-entry-" : "roster-candidate-") + athleteId
    activeFocusOnTab: true
    Accessible.role: ordered ? Accessible.ListItem : Accessible.CheckBox
    Accessible.name: ordered ? qsTr("%1. %2, %3").arg(position).arg(athleteName).arg(countryCode) : athleteName + ", " + countryCode
    Accessible.selected: marked
    Accessible.checked: included
    Keys.onSpacePressed: ordered ? backend.mark(athleteId,event.modifiers) : backend.toggle(athleteId)
    Keys.onDeletePressed: if(ordered && backend.membership){backend.mark(athleteId);backend.removeMarked()}
    Keys.onPressed: {
        if(ordered && (event.modifiers & Qt.ControlModifier) && (event.key === Qt.Key_Up || event.key === Qt.Key_Down)){
            if(!marked)backend.mark(athleteId)
            backend.step(event.key === Qt.Key_Up ? -1 : 1);event.accepted=true
        }
    }
    Rectangle { anchors.fill: parent; radius: 6; color: row.marked ? Theme.selection : row.activeSlot ? Theme.surface : Theme.raised; border.color: row.activeFocus ? Theme.accent : Theme.divider; border.width: row.activeFocus ? 2 : 1 }
    MouseArea { anchors.fill: parent; onClicked: {row.forceActiveFocus();row.ordered ? row.backend.mark(row.athleteId,mouse.modifiers) : row.backend.toggle(row.athleteId)} }
    RowLayout {
        anchors.fill: parent; anchors.leftMargin: 6; anchors.rightMargin: 8
        spacing: Theme.compact ? 4 : 8
        SCheck { visible: !row.ordered; checked: row.included; text: ""; Layout.preferredWidth: 30; Accessible.name: row.athleteName; onClicked: row.backend.toggle(row.athleteId) }
        SButton {
            id: grip
            objectName: row.ordered ? "roster-grip-" + row.athleteId : ""
            visible: row.ordered; text: "⠿"; quiet: true
            Layout.preferredWidth: 34; Layout.preferredHeight: Theme.compact ? 28 : 40
            Accessible.name: qsTr("Przenieś %1").arg(row.athleteName)
            hint: qsTr("Przeciągnij lub wybierz pozycję")
            onClicked: {if(!row.marked)row.backend.mark(row.athleteId);row.moveRequested()}
            MouseArea {
                id: handle
                anchors.fill: parent
                drag.target: preview; drag.threshold: 8
                cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                onPressed: {
                    grip.forceActiveFocus()
                    var point=mapToItem(preview.parent,mouse.x,mouse.y)
                    preview.x=point.x-20;preview.y=point.y-preview.height/2
                }
                onPositionChanged: if(drag.active && !preview.Drag.active)preview.Drag.active=true
                onReleased: {if(preview.Drag.active){preview.Drag.drop();preview.Drag.active=false}}
                onCanceled: preview.Drag.active=false
                onClicked: grip.clicked()
            }
        }
        Label { visible: row.ordered; text: row.position; font.family: Theme.mono; font.pointSize: Theme.caption; color: Theme.muted; Layout.preferredWidth: 30; horizontalAlignment: Text.AlignRight }
        ColumnLayout {
            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
            Label { text: row.athleteName; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: row.ordered && row.backend.activeLimit > 0; text: row.activeSlot ? qsTr("Skład") : qsTr("Rezerwa"); color: row.activeSlot ? Theme.muted : Theme.warning; font.pointSize: Theme.caption }
        }
        Label { text: row.countryCode; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        SButton { visible: row.ordered && row.backend.membership; text: "×"; quiet: true; Layout.preferredWidth: 32; Accessible.name: qsTr("Usuń %1 z listy").arg(row.athleteName); onClicked: row.backend.toggle(row.athleteId) }
    }
    DropArea {
        id: dropTarget
        anchors.fill: parent
        enabled: row.ordered
        keys: ["sj-roster"]
        property bool after: false
        onEntered: after=drag.y>height/2
        onPositionChanged: after=drag.y>height/2
        onDropped: function(event) {
            if(event.source && event.source.backend === row.backend){row.backend.moveBefore(event.source.athleteId,row.athleteId,dropTarget.after);event.acceptProposedAction()}
        }
    }
    Rectangle { visible: dropTarget.containsDrag; x: 0; y: dropTarget.after ? parent.height-2 : 0; width: parent.width; height: 3; color: Theme.accent; z: 2 }
    Rectangle {
        id: preview
        parent: row.Window.window ? row.Window.window.contentItem : row
        visible: Drag.active
        width: Math.min(300,row.width); height: 48; radius: 8
        color: Theme.stadium; border.color: Theme.signal; border.width: 2
        z: 100
        Drag.source: row; Drag.keys: ["sj-roster"]; Drag.supportedActions: Qt.MoveAction
        Drag.hotSpot.x: 20; Drag.hotSpot.y: height/2
        Label { anchors.fill: parent; anchors.margins: 10; text: row.athleteName; color: Theme.snow; font.pointSize: Theme.body; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
        Timer {
            interval: 50; repeat: true; running: preview.Drag.active && !!row.viewport
            onTriggered: {
                var point=preview.mapToItem(row.viewport,20,24)
                var step=point.y < 36 ? -12 : point.y > row.viewport.height-36 ? 12 : 0
                if(step)row.viewport.contentY=Math.max(row.viewport.originY,Math.min(row.viewport.contentY+step,row.viewport.originY+Math.max(0,row.viewport.contentHeight-row.viewport.height)))
            }
        }
    }
}
