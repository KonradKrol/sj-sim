import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

MouseArea {
    id: gesture
    objectName: "koDragArea"
    property var viewport
    property bool dragging: false
    property bool canceled: false
    property point origin
    property point pointer
    property string athleteId: ""
    property string athleteName: ""
    property string targetGroup: ""
    property int targetSlot: -1
    signal swapRequested(string groupId, int slot, string athleteId)
    acceptedButtons: Qt.LeftButton
    preventStealing: true
    function memberAt(point) {
        var index=viewport.indexAt(point.x,point.y+viewport.contentY)
        var card=viewport.itemAtIndex(index)
        return card ? card.memberAt(gesture.mapToItem(card,point.x,point.y)) : null
    }
    function locate() {
        var point=mapToItem(preview.parent,pointer.x,pointer.y)
        preview.x=Math.max(4,Math.min(point.x-16,preview.parent.width-preview.width-4))
        preview.y=Math.max(4,Math.min(point.y-preview.height/2,preview.parent.height-preview.height-4))
        var member=pointer.x>=0 && pointer.x<width && pointer.y>=0 && pointer.y<height ? memberAt(pointer) : null
        targetGroup=member ? member.groupId : ""; targetSlot=member ? member.slot : -1
    }
    function stop() { dragging=false;targetGroup="";targetSlot=-1 }
    function scrollBy(delta) {
        viewport.contentY=Math.max(viewport.originY,Math.min(viewport.contentY+delta,viewport.originY+Math.max(0,viewport.contentHeight-viewport.height)))
        locate()
    }
    onPressed: {
        var member=memberAt(Qt.point(mouse.x,mouse.y))
        if (!member || mapToItem(member,mouse.x,mouse.y).x>member.leftPadding+24) {mouse.accepted=false;return}
        canceled=false
        athleteId=member.athleteId;athleteName=member.athleteName
        origin=Qt.point(mouse.x,mouse.y);pointer=origin
        forceActiveFocus()
    }
    onPositionChanged: {
        if (!pressed || canceled) return
        pointer=Qt.point(mouse.x,mouse.y)
        if (!dragging && Math.abs(pointer.x-origin.x)+Math.abs(pointer.y-origin.y)>8) dragging=true
        if (dragging) locate()
    }
    onReleased: {
        var group=targetGroup,slot=targetSlot,id=athleteId,wasDragging=dragging
        stop()
        if (wasDragging && slot>=0) swapRequested(group,slot,id)
    }
    onCanceled: stop()
    onVisibleChanged: if (!visible) stop()
    onWheel: {
        if (dragging) {scrollBy(wheel.pixelDelta.y ? -wheel.pixelDelta.y : -wheel.angleDelta.y*2);wheel.accepted=true}
        else wheel.accepted=false
    }
    Keys.onEscapePressed: {canceled=true;stop()}
    Timer {
        interval: 16; repeat: true; running: gesture.dragging && gesture.visible
        onTriggered: {
            if (gesture.pointer.x<0 || gesture.pointer.x>gesture.width) return
            var edge=Math.min(70,gesture.height*.2)
            var depth=gesture.pointer.y<edge ? (gesture.pointer.y-edge)/edge : gesture.pointer.y>gesture.height-edge ? (gesture.pointer.y-gesture.height+edge)/edge : 0
            if (depth) gesture.scrollBy((depth<0 ? -1 : 1)*(4+22*Math.min(1,depth*depth)))
        }
    }
    Rectangle {
        id: preview
        objectName: "koDragPreview"
        parent: gesture.Window.window ? gesture.Window.window.contentItem : gesture
        visible: gesture.dragging && gesture.visible
        z: 100
        width: Math.min(Theme.dp(280),gesture.width);height: Theme.controlHeight+12
        color: Theme.stadium; border.color: Theme.signal; border.width: 2; radius: Theme.dp(5)
        Label { anchors.fill: parent; anchors.margins: 8; text: gesture.athleteName; color: Theme.snow; font.pointSize: Theme.body; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
    }
}
