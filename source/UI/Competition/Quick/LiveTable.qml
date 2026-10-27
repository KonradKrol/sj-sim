import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: tableRoot
    property var table
    property var theme
    property bool compact:false
    property int columns: table.headers ? table.headers.length : 1
    property real cellWidth: columns>1 ? (compact ? 58 : 96) : Math.max(180,width)
    property real nameWidth: columns>1 ? (compact ? Math.max(140,width-116) : 210) : cellWidth
    property real totalWidth: columns>1 ? (columns-1)*cellWidth+nameWidth : cellWidth
    property bool follow: table.name==="listView_startList"
    function shade(hex) {
        if(!hex) return theme.panel
        if(!theme.dark) return hex
        var r=parseInt(hex.substr(1,2),16),g=parseInt(hex.substr(3,2),16),b=parseInt(hex.substr(5,2),16)
        var high=Math.max(r,g,b),low=Math.min(r,g,b)
        if(high-low<8) return theme.panel
        return Qt.rgba((r-low)/(high-low)*.10+.10,(g-low)/(high-low)*.10+.14,(b-low)/(high-low)*.10+.20,1)
    }
    property int followedRow:-1
    function followCurrent() {
        list.forceLayout()
        var row=currentRow(), rowHeight=compact ? 32 : 36
        if(row>=0) list.contentY=Math.max(0,Math.min(row*rowHeight-(list.height-rowHeight)/2,Math.max(0,list.contentHeight-list.height)))
    }
    onTableChanged: {
        if(follow && table.rows && currentRow()!==followedRow) {
            followedRow=currentRow()
            Qt.callLater(followCurrent)
        }
    }
    onHeightChanged:if(follow) Qt.callLater(followCurrent)
    property var savedPosition: theme.position("table"+table.id)
    function columnWidth(c) { return c===1 ? nameWidth : cellWidth }
    function currentRow() { for(var i=0;i<(table.rows || []).length;++i) if(table.rows[i].current) return i; return -1 }
    Component.onCompleted: {
        var p=savedPosition
        horizontal.contentX=Math.min(p.x,Math.max(0,totalWidth-width))
        if(follow) Qt.callLater(followCurrent)
        else list.contentY=Math.min(p.y,Math.max(0,list.contentHeight-list.height))
    }
    Flickable {
        id:horizontal
        anchors.fill:parent; clip:true
        contentWidth:Math.max(width,tableRoot.totalWidth); contentHeight:height
        boundsBehavior:Flickable.StopAtBounds; flickableDirection:Flickable.HorizontalFlick
        ScrollBar.horizontal:ScrollBar { policy:ScrollBar.AsNeeded }
        onContentXChanged:theme.remember("table"+table.id,contentX,list.contentY)
        Column {
            width:horizontal.contentWidth; height:horizontal.height
            Row {
                id:header; height:columns===1 ? 0 : (compact ? 26 : 32); visible:columns>1
                Repeater {
                    model:table.headers || []
                    Rectangle {
                        width:columnWidth(index); height:header.height; color:theme.dark ? "#25354b" : "#e7edf6"
                        Label { anchors.fill:parent; anchors.margins:8; text:modelData; color:theme.ink; font.bold:true; font.pixelSize:11; elide:Text.ElideRight; verticalAlignment:Text.AlignVCenter }
                    }
                }
            }
            ListView {
                id:list; objectName:"rows-"+(table.name || table.id)
                width:horizontal.contentWidth; height:parent.height-header.height-14
                model:table.rows || []; currentIndex:-1; clip:true; boundsBehavior:Flickable.StopAtBounds
                ScrollBar.vertical:ScrollBar { policy:ScrollBar.AsNeeded; parent:tableRoot; anchors.top:parent.top; anchors.bottom:parent.bottom; anchors.right:parent.right }
                onContentYChanged:theme.remember("table"+table.id,horizontal.contentX,contentY)
                delegate:Item {
                    id:resultRow
                    property var rowData:modelData
                    width:list.width; height:compact ? 32 : 36
                    Row {
                        Repeater {
                            model:rowData.cells
                            Rectangle {
                                width:columnWidth(index); height:resultRow.height
                                color: (rowData.current || resultRow.ListView.isCurrentItem) ? (theme.dark ? "#294568" : "#dcecff") : tableRoot.shade(modelData.background)
                                Rectangle { anchors.bottom:parent.bottom; width:parent.width; height:1; color:theme.dark ? "#2c394b" : "#e5ebf3" }
                                Row {
                                    anchors.fill:parent; anchors.leftMargin:8+(index===0 ? rowData.depth*16+(rowData.branch?20:0) : 0); spacing:7
                                    Image { visible:!!modelData.icon; source:modelData.icon; width:visible?28:0; height:24; anchors.verticalCenter:parent.verticalCenter; fillMode:Image.PreserveAspectFit }
                                    Label { text:modelData.text; color:theme.ink; width:parent.width-(modelData.icon?40:8); height:parent.height; verticalAlignment:Text.AlignVCenter; elide:Text.ElideRight; font.pixelSize:13 }
                                }
                            }
                        }
                    }
                    MouseArea {
                        anchors.fill:parent; hoverEnabled:true
                        onClicked: { list.currentIndex=index; live.select(table.id,index,false) }
                        onDoubleClicked: {
                            var adapter=live, root=theme, competition=live.competition
                            var id=table.id, row=index, results=table.name==="tableView_results" || table.name===""
                            Qt.callLater(function() { adapter.select(id,row,true); if(competition && results) root.page=3 })
                        }
                    }
                    ToolButton {
                        visible:!!rowData.branch; width:24; height:32; y:4; x:3
                        text:rowData.expanded ? "▾" : "▸"
                        contentItem:Text { text:parent.text; color:theme.ink; horizontalAlignment:Text.AlignHCenter; verticalAlignment:Text.AlignVCenter }
                        background:Item {}
                        onClicked:live.toggleTree(table.id,index)
                    }
                }
            }
        }
    }
    Label { visible:!(table.rows || []).length; anchors.centerIn:parent; text:qsTr("Brak wyników"); color:theme.muted }
}
