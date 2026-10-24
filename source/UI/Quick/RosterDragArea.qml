import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

MouseArea {
    id: gesture
    objectName: "rosterDragArea"
    property var backend
    property var viewport
    property bool dragging: false
    property bool moved: false
    property bool canceled: false
    property point pressedAt
    property point pointer
    property var athlete: ({})
    readonly property string athleteName: athlete.athleteName || ""
    signal moveRequested()
    acceptedButtons: Qt.LeftButton
    preventStealing: true

    function scrollBy(distance) {
        viewport.contentY=Math.max(viewport.originY,Math.min(viewport.contentY+distance,viewport.originY+Math.max(0,viewport.contentHeight-viewport.height)))
    }
    function placePreview() {
        var point=mapToItem(preview.parent,pointer.x,pointer.y)
        preview.x=Math.max(4,Math.min(point.x-20,preview.parent.width-preview.width-4))
        preview.y=Math.max(4,Math.min(point.y-preview.height/2,preview.parent.height-preview.height-4))
        preview.Drag.hotSpot.x=point.x-preview.x
        preview.Drag.hotSpot.y=point.y-preview.y
    }
    function stop() {dragging=false;preview.Drag.active=false}
    onVisibleChanged: if(!visible)stop()
    function dropAtPointer() {
        if(pointer.x<0 || pointer.x>width || pointer.y<0 || pointer.y>height)return
        var index=viewport.indexAt(pointer.x,pointer.y+viewport.contentY)
        if(index<0)index=viewport.indexAt(pointer.x,pointer.y+viewport.contentY+viewport.spacing+1)
        if(index<0)index=viewport.indexAt(pointer.x,pointer.y+viewport.contentY-viewport.spacing-1)
        if(index<0){if(!viewport.count)return;index=viewport.count-1}
        var target=backend.ordered.get(index)
        var item=viewport.itemAtIndex(index)
        var after=!item || pointer.y+viewport.contentY>item.y+item.height/2
        if(target.athleteId)backend.moveBefore(athlete.athleteId,target.athleteId,after)
    }
    onPressed: {
        var index=viewport.indexAt(mouse.x,mouse.y+viewport.contentY)
        if(mouse.x<6 || mouse.x>40 || index<0){mouse.accepted=false;return}
        athlete=backend.ordered.get(index)
        pressedAt=Qt.point(mouse.x,mouse.y);pointer=pressedAt;moved=false;canceled=false
        forceActiveFocus()
    }
    onPositionChanged: {
        if(!pressed || canceled)return
        pointer=Qt.point(mouse.x,mouse.y)
        if(!dragging && Math.abs(pointer.x-pressedAt.x)+Math.abs(pointer.y-pressedAt.y)>8) {
            if(!athlete.marked)backend.mark(athlete.athleteId)
            moved=true;dragging=true
            placePreview();preview.Drag.active=true
        }
        if(dragging)placePreview()
    }
    onReleased: {if(dragging)dropAtPointer();stop()}
    onCanceled: {canceled=true;stop()}
    onClicked: if(!moved && !canceled){if(!athlete.marked)backend.mark(athlete.athleteId);moveRequested()}
    onWheel: {
        if(dragging){scrollBy(wheel.pixelDelta.y ? -wheel.pixelDelta.y : -wheel.angleDelta.y*2);wheel.accepted=true}
        else wheel.accepted=false
    }
    Keys.onEscapePressed: {canceled=true;moved=true;stop()}
    Timer {
        interval: 16; repeat: true; running: gesture.dragging && gesture.visible
        onTriggered: {
            if(gesture.pointer.x<0 || gesture.pointer.x>gesture.width)return
            var edge=Math.min(80,gesture.height*.22)
            var depth=gesture.pointer.y<edge ? (gesture.pointer.y-edge)/edge : gesture.pointer.y>gesture.height-edge ? (gesture.pointer.y-gesture.height+edge)/edge : 0
            if(depth)gesture.scrollBy((depth<0 ? -1 : 1)*(4+28*Math.min(1,depth*depth)))
        }
    }
    Rectangle {
        id: preview
        objectName: "rosterDragPreview"
        parent: gesture.Window.window ? gesture.Window.window.contentItem : gesture
        visible: gesture.dragging && gesture.visible
        width: Math.min(340,gesture.width);height: 54;radius: 8;z: 100
        color: Theme.stadium;border.color: Theme.signal;border.width: 2
        Drag.source: dragSource;Drag.keys: ["sj-roster"];Drag.supportedActions: Qt.MoveAction
        QtObject {
            id: dragSource
            readonly property var backend: gesture.backend
            readonly property string athleteId: gesture.athlete.athleteId || ""
        }
        RowLayout {
            anchors.fill: parent;anchors.margins: 9;spacing: 8
            Image { source: gesture.athlete.flagSource || "";sourceSize.width: 32;sourceSize.height: 24;Layout.preferredWidth: 30;Layout.preferredHeight: 22;fillMode: Image.PreserveAspectFit;visible: source.toString().length>0;Accessible.ignored: true }
            Label { text: gesture.athleteName;color: Theme.snow;font.pointSize: Theme.body;Layout.fillWidth: true;elide: Text.ElideRight }
            Label { visible: gesture.backend.markedCount>1;text: "+"+(gesture.backend.markedCount-1);color: Theme.signal;font.family: Theme.mono;font.pointSize: Theme.caption }
        }
    }
}
