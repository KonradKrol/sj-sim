import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Item {
    id: row
    property var event
    property var backend
    property bool compact: false
    property var viewport
    readonly property string eventId: event.id
    objectName: "calendar-row-"+event.id
    signal activated(string eventId)
    activeFocusOnTab: true
    Keys.onReturnPressed: row.activated(row.event.id)
    Keys.onSpacePressed: row.select(Qt.ControlModifier)
    function select(modifiers) {
        var view=row.ListView.view;var position=row.event.number-1;var source=row.backend;var eventId=row.event.id
        Qt.callLater(function(){if(view && view.visible){view.currentIndex=position;view.positionViewAtIndex(position,ListView.Contain);var current=view.itemAtIndex(position);if(current)current.forceActiveFocus()}})
        source.select(eventId,modifiers)
    }
    implicitHeight: Theme.compact ? Math.max(56,rowContent.implicitHeight+8) : (event.pending || event.dependents ? 112 : 88)+(backend.datesEnabled ? 22 : 0)
    Accessible.role: Accessible.ListItem
    Accessible.name: event.title + ", " + event.type
    Accessible.selected: !!event.selected
    Surface { anchors.fill: parent; fillColor: row.event.selected ? Theme.selection : Theme.surface; strokeWidth: row.event.selected ? 2 : 0; edgeColor: Theme.accent; sweep: 16 }
    MouseArea {
        objectName: "calendar-select-"+row.event.id
        anchors.fill: parent
        onClicked: row.select(mouse.modifiers)
        onDoubleClicked: if(!(mouse.modifiers & (Qt.ControlModifier|Qt.ShiftModifier)))row.activated(row.event.id)
    }
    RowLayout {
        id: rowContent
        anchors.fill: parent; anchors.margins: Theme.compact ? 4 : 10
        spacing: Theme.compact ? 4 : 10
        SButton {
            id: grip
            objectName: "calendar-grip-"+row.event.id
            text: "⠿"; quiet: true; enabled: !row.event.locked
            Layout.preferredWidth: 30; Layout.preferredHeight: Theme.compact ? 28 : 40
            Accessible.name: qsTr("Przenieś konkurs %1").arg(row.event.number)
            hint: qsTr("Przeciągnij; klawiatura: Ctrl+↑ / Ctrl+↓")
            MouseArea {
                anchors.fill: parent; drag.target: preview; drag.threshold: 8
                cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                onPressed: {grip.forceActiveFocus();var point=mapToItem(preview.parent,mouse.x,mouse.y);preview.x=point.x-20;preview.y=point.y-preview.height/2}
                onPositionChanged: if(drag.active && !preview.Drag.active)preview.Drag.active=true
                onReleased: {if(preview.Drag.active){preview.Drag.drop();preview.Drag.active=false}}
                onCanceled: preview.Drag.active=false
                onClicked: row.select(0)
            }
        }
        SCheck { Layout.preferredWidth: 24; text: ""; checked: !!row.event.selected; Accessible.name: qsTr("Zaznacz %1").arg(row.event.title); onClicked: row.backend.select(row.event.id,Qt.ControlModifier) }
        ColumnLayout {
            Layout.fillWidth: true; spacing: Theme.compact ? 1 : 3
            Label { text: row.event.number + ". " + row.event.title; font.family: Theme.displayFamily; font.pointSize: Theme.section; color: Theme.ink; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: row.backend.datesEnabled; text: row.event.dateText; color: row.event.date ? Theme.accent : Theme.warning; font.pointSize: Theme.caption; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { text: row.event.type + (row.event.team ? " · " + qsTr("Drużynowy") : "") + (row.event.trainings ? " · " + qsTr("Treningi: %1").arg(row.event.trainings) : "") + (row.event.trial ? " · " + qsTr("Próbna") : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: !!row.event.pending || !!row.event.dependents; text: row.event.pending ? qsTr("Kwalifikacja z konkursu %1 — popraw kolejność").arg(row.event.pendingSource) : qsTr("Źródło kwalifikacji dla: %1 — popraw kolejność").arg(row.event.dependents); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { visible: !!row.event.locked; text: row.event.played ? qsTr("Rozegrany") : row.event.cancelled ? qsTr("Odwołany") : qsTr("W trakcie"); color: Theme.warning; font.pointSize: Theme.caption }
        }
        SButton { visible: row.compact; text: "›"; quiet: true; Accessible.name: qsTr("Edytuj %1").arg(row.event.title); onClicked: row.activated(row.event.id) }
    }
    DropArea {
        id: dropTarget
        anchors.fill: parent; enabled: !row.event.locked; keys: ["sj-calendar"]
        property bool after: false
        onEntered: after=drag.y>height/2
        onPositionChanged: after=drag.y>height/2
        onDropped: function(drop) {
            if(drop.source && drop.source.backend===row.backend){
                var backend=row.backend;var source=drop.source.eventId;var target=row.eventId;var after=dropTarget.after
                drop.acceptProposedAction();Qt.callLater(function(){backend.moveBefore(source,target,after)})
            }
        }
    }
    Rectangle { visible: dropTarget.containsDrag; x: 0; y: dropTarget.after ? parent.height-2 : 0; width: parent.width; height: 3; color: Theme.accent; z: 2 }
    Rectangle {
        id: preview
        parent: row.Window.window ? row.Window.window.contentItem : row
        visible: Drag.active; width: Math.min(300,row.width); height: 48; radius: 8
        color: Theme.stadium; border.color: Theme.signal; border.width: 2; z: 100
        Drag.source: row; Drag.keys: ["sj-calendar"]; Drag.supportedActions: Qt.MoveAction
        Drag.hotSpot.x: 20; Drag.hotSpot.y: height/2
        Label { anchors.fill: parent; anchors.margins: 10; text: row.event.title; color: Theme.snow; font.pointSize: Theme.body; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
        Timer {
            interval: 50; repeat: true; running: preview.Drag.active && !!row.viewport
            onTriggered: {var point=preview.mapToItem(row.viewport,20,24);var step=point.y<36 ? -12 : point.y>row.viewport.height-36 ? 12 : 0;if(step)row.viewport.contentY=Math.max(row.viewport.originY,Math.min(row.viewport.contentY+step,row.viewport.originY+Math.max(0,row.viewport.contentHeight-row.viewport.height)))}
        }
    }

}
