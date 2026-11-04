import QtQuick 2.15

Item {
    id: handle
    z: 2
    property var view
    property int rowIndex: -1
    signal moved(int from, int to)
    implicitWidth: Theme.dp(28)
    implicitHeight: Theme.dp(28)
    Accessible.role: Accessible.Button
    Accessible.name: qsTr("Zmień kolejność")
    function locate() {
        var p=pointer.mapToItem(view.contentItem,pointer.mouseX,pointer.mouseY)
        var target=view.indexAt(Math.max(1,Math.min(view.width-1,p.x)),p.y)
        if(target<0 && view.count>0)target=p.y<0?0:view.count-1
        view.reorderTarget=target
    }
    Item {
        anchors.centerIn: parent; width: 8; height: 13
        Repeater { model: 6; Rectangle { required property int index; x: (index%2)*5; y: Math.floor(index/2)*5; width: 2; height: 2; radius: 1; color: pointer.dragging?Theme.accent:Theme.muted } }
    }
    MouseArea {
        id: pointer
        anchors.fill: parent; cursorShape: Qt.SizeVerCursor; preventStealing: true
        property real startY: 0
        property bool dragging: false
        onPressed: {startY=mouse.y;dragging=false}
        onPositionChanged: if(pressed && (dragging || Math.abs(mouse.y-startY)>8)){dragging=true;handle.locate()}
        onReleased: {
            var target=handle.view.reorderTarget,from=handle.rowIndex,shouldMove=dragging && target>=0 && target!==from
            handle.view.reorderTarget=-1
            dragging=false
            if(shouldMove)handle.moved(from,target)
        }
        onCanceled: {dragging=false;handle.view.reorderTarget=-1}
        Timer {
            interval: 40; repeat: true; running: pointer.pressed && pointer.dragging
            onTriggered: {
                var p=pointer.mapToItem(handle.view,pointer.mouseX,pointer.mouseY),edge=Theme.dp(40)
                var direction=p.y<edge?-1:p.y>handle.view.height-edge?1:0
                if(direction)handle.view.contentY=Math.max(0,Math.min(Math.max(0,handle.view.contentHeight-handle.view.height),handle.view.contentY+direction*Theme.dp(22)))
                handle.locate()
            }
        }
    }
}
