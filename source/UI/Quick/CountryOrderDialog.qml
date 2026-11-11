import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Popup {
    id: dialog
    property var backend
    property var draggingTile: null
    parent: Overlay.overlay
    modal: true; focus: true
    width: Math.min(Theme.dp(400),parent ? parent.width-24 : Theme.dp(400))
    height: Math.min(Theme.dp(540),parent ? parent.height-24 : Theme.dp(540))
    x: parent ? (parent.width-width)/2 : 0
    y: parent ? (parent.height-height)/2 : 0
    padding: Theme.dp(16)
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.outline; strokeWidth: 1 }
    ListModel { id: countries }
    onAboutToShow: {
        countries.clear()
        var rows=backend.orderingCountries()
        for(var i=0;i<rows.length;++i)countries.append(rows[i])
    }
    Timer {
        interval: 20; repeat: true; running: !!dialog.draggingTile
        onTriggered: {
            var point=dialog.draggingTile.mapToItem(list,0,dialog.draggingTile.height/2)
            var delta=point.y<Theme.dp(35) ? -Theme.dp(8) : point.y>list.height-Theme.dp(35) ? Theme.dp(8) : 0
            list.contentY=Math.max(0,Math.min(Math.max(0,list.contentHeight-list.height),list.contentY+delta))
        }
    }
    onClosed: draggingTile=null
    contentItem: ColumnLayout {
        spacing: Theme.dp(12)
        Label { text: qsTr("Kolejność krajów"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true }
        Label { text: qsTr("Przeciągnij kraj na wybrane miejsce."); color: Theme.muted; font.pointSize: Theme.caption }
        ListView {
            id: list
            Layout.fillWidth: true; Layout.fillHeight: true
            model: countries; spacing: Theme.dp(5); clip: true
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}
            delegate: DropArea {
                id: row
                required property int index
                required property string code
                required property string flag
                width: list.width; height: Theme.dp(46)
                z: grip.drag.active ? 10 : 0
                onEntered: function(event){if(event.source && event.source!==row)countries.move(event.source.index,index,1)}
                Rectangle {
                    id: tile
                    width: row.width; height: row.height
                    color: grip.drag.active ? Theme.selection : Theme.raised
                    border.color: grip.drag.active ? Theme.accent : Theme.outline
                    radius: Theme.controlRadius
                    Drag.active: grip.drag.active
                    Drag.source: row
                    Drag.hotSpot.x: width/2; Drag.hotSpot.y: height/2
                    states: State {
                        when: grip.drag.active
                        ParentChange { target: tile; parent: list }
                        PropertyChanges { target: tile; z: 100 }
                    }

                    RowLayout {
                        anchors.fill: parent; anchors.margins: Theme.dp(10); spacing: Theme.dp(10)
                        Label { text: row.index+1; color: Theme.muted; font.family: Theme.mono; Layout.preferredWidth: Theme.dp(25) }
                        Image { source: row.flag; Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(18); fillMode: Image.PreserveAspectFit }
                        Label { text: row.code; color: Theme.ink; font.bold: true; Layout.fillWidth: true }
                        Label { text: "≡"; color: Theme.muted }
                    }
                    MouseArea {
                        id: grip; anchors.fill: parent; cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                        drag.target: tile; drag.axis: Drag.YAxis
                        onPositionChanged: if(drag.active)dialog.draggingTile=tile
                        onReleased: {dialog.draggingTile=null;tile.Drag.cancel();Qt.callLater(function(){tile.y=0})}
                        onCanceled: {dialog.draggingTile=null;Qt.callLater(function(){tile.y=0})}
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            SButton { text: qsTr("Anuluj"); quiet: true; onClicked: dialog.close() }
            SButton { text: qsTr("Ok"); primary: true; onClicked: {var order=[];for(var i=0;i<countries.count;++i)order.push(countries.get(i).code);backend.orderCountries(order);dialog.close()} }
        }
    }
}
