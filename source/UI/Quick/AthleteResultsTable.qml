import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: root
    objectName: "athleteResultsTable"
    property var tables: []
    property int selectedIndex: -1
    property string selectedKey: ""
    property int hoverIndex: -1
    readonly property var active: tables.length ? tables[Math.max(0,Math.min(tables.length-1,selectedIndex))] : null
    readonly property var hoveredCompetition: active && hoverIndex>=0 && hoverIndex<active.competitions.length ? active.competitions[hoverIndex] : null
    readonly property real cellWidth: Theme.dp(28)
    readonly property real totalWidth: Theme.dp(46)
    readonly property real seasonHeight: Theme.dp(18)
    readonly property real headerHeight: Theme.dp(18)
    readonly property real resultHeight: Theme.dp(22)
    readonly property real tableWidth: active ? active.competitions.length*cellWidth+totalWidth : 0
    readonly property real scrolled: viewport.contentItem ? viewport.contentItem.contentX : 0
    readonly property color tableBorder: "#A2A9B1"
    readonly property color tableInk: "#202122"
    readonly property color tableMuted: "#54595D"
    readonly property string resultLine: {
        if(!hoveredCompetition) return ""
        if(hoveredCompetition.status) return hoveredCompetition.status
        if(hoveredCompetition.position>0) return qsTr("%1. miejsce%2").arg(hoveredCompetition.position).arg(hoveredCompetition.pointsText ? " · "+hoveredCompetition.pointsText+" pkt" : "")
        return qsTr("Brak wyniku")
    }
    function cellColor(position) {
        if(position===1) return "#FFE9A3"
        if(position===2) return "#E6E8EB"
        if(position===3) return "#F2D2B5"
        if(position>=4 && position<=10) return "#EFFFDF"
        if(position>=11 && position<=30) return "#DFFFDF"
        if(position>=31) return "#CFCFFF"
        return "#BFBFBF"
    }
    function columnAt(x) {
        if(!active) return -1
        var index=Math.floor(x/cellWidth)
        return index>=0 && index<active.competitions.length ? index : -1
    }
    onTablesChanged: {
        if(!tables.length) {selectedIndex=-1;hoverIndex=-1;selectedKey="";return}
        var index=0
        for(var i=0;i<tables.length;++i) if(tables[i].key===selectedKey) {index=i;break}
        selectedIndex=index
        selectedKey=tables[index].key
        if(picker) picker.currentIndex=index
    }
    onSelectedIndexChanged: {
        hoverIndex=-1
        if(picker && picker.currentIndex!==selectedIndex) picker.currentIndex=selectedIndex
    }
    padding: Theme.dp(8)
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(6)
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(10)
            ColumnLayout {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                spacing: 0
                Label { text: qsTr("Miejsca w konkursach"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
                Label { visible: !!root.active; text: root.active ? qsTr("%1 konkursów").arg(root.active.count) : ""; color: Theme.muted; font.pointSize: Theme.caption }
            }
            SChoice {
                id: picker
                visible: root.tables.length>1
                Layout.preferredWidth: Theme.dp(250)
                Layout.maximumWidth: root.width*0.62
                Layout.minimumWidth: 0
                model: root.tables.map(function(table){return table.label})
                currentIndex: root.selectedIndex
                Accessible.name: qsTr("Kalendarz")
                onActivated: {
                    root.selectedIndex=index
                    root.selectedKey=root.tables[index] ? root.tables[index].key : ""
                }
            }
        }
        Label {
            visible: !root.tables.length
            text: qsTr("Brak rozegranych konkursów.")
            color: Theme.muted
            font.pointSize: Theme.caption
        }
        ScrollView {
            id: viewport
            visible: root.tables.length>0
            Layout.fillWidth: true
            implicitHeight: root.seasonHeight+root.headerHeight+root.resultHeight+Theme.dp(12)
            contentWidth: tableContent.width
            contentHeight: tableContent.height
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            ScrollBar.horizontal.policy: ScrollBar.AsNeeded
            Column {
                id: tableContent
                width: root.tableWidth
                spacing: -1
                HoverHandler {
                    onPointChanged: {
                        var index=point.position.y>=root.seasonHeight-1 ? root.columnAt(point.position.x) : -1
                        if(index!==root.hoverIndex) root.hoverIndex=index
                    }
                    onHoveredChanged: if(!hovered) root.hoverIndex=-1
                }
                Rectangle {
                    width: parent.width; height: root.seasonHeight
                    color: "#F0FFFF"; border.color: root.tableBorder
                    Label {
                        anchors.fill: parent; anchors.leftMargin: Theme.dp(4)
                        text: root.active ? root.active.season : ""
                        color: root.tableInk; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; font.bold: true
                        verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
                    }
                }
                Row {
                    width: parent.width; spacing: -1
                    Repeater {
                        model: root.active ? root.active.competitions : []
                        Rectangle {
                            required property var modelData
                            required property int index
                            width: root.cellWidth; height: root.headerHeight
                            color: "#F0FFFF"
                            border.color: root.hoverIndex===index ? Theme.accent : root.tableBorder
                            border.width: root.hoverIndex===index ? 1.5 : 1
                            Image {
                                anchors.centerIn: parent
                                source: modelData.flag || ""
                                sourceSize.width: 32; sourceSize.height: 20
                                width: Theme.dp(16); height: Theme.dp(10)
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                            }
                        }
                    }
                    Rectangle {
                        width: root.totalWidth; height: root.headerHeight
                        color: "#F9F9F9"; border.color: root.tableBorder
                        Label { anchors.centerIn: parent; text: qsTr("Σ pkt"); color: root.tableMuted; font.pointSize: Theme.caption; font.bold: true }
                    }
                }
                Row {
                    width: parent.width; spacing: -1
                    Repeater {
                        model: root.active ? root.active.competitions : []
                        Rectangle {
                            required property var modelData
                            required property int index
                            width: root.cellWidth; height: root.resultHeight
                            color: root.cellColor(modelData.position)
                            border.color: root.hoverIndex===index ? Theme.accent : root.tableBorder
                            border.width: root.hoverIndex===index ? 1.5 : 1
                            Label {
                                anchors.centerIn: parent
                                text: modelData.positionText
                                color: root.tableInk
                                font.family: Theme.mono
                                font.pointSize: modelData.positionText.length>2 ? Theme.caption*0.82 : Theme.caption
                                font.bold: modelData.position>0 && modelData.position<=3
                            }
                        }
                    }
                    Rectangle {
                        width: root.totalWidth; height: root.resultHeight
                        color: "#F9F9F9"; border.color: root.tableBorder
                        Label { anchors.centerIn: parent; text: root.active ? root.active.totalText : ""; color: root.tableInk; font.family: Theme.mono; font.pointSize: Theme.caption; font.bold: true }
                    }
                }
            }
        }
    }
    ToolTip {
        id: tip
        objectName: "athleteResultsTooltip"
        parent: root
        visible: !!root.hoveredCompetition
        delay: 0
        timeout: -1
        padding: Theme.dp(8)
        width: Math.min(Theme.dp(280),root.width-Theme.dp(16))
        x: Math.max(Theme.dp(4),Math.min(root.width-width-Theme.dp(4),
            root.contentItem.x+viewport.x+root.hoverIndex*root.cellWidth+root.cellWidth/2-width/2-root.scrolled))
        y: root.contentItem.y+viewport.y+viewport.height+Theme.dp(4)
        background: Surface { fillColor: Theme.raised; edgeColor: Theme.accent; strokeWidth: 1 }
        contentItem: RowLayout {
            spacing: Theme.dp(8)
            Image {
                source: root.hoveredCompetition ? root.hoveredCompetition.flag||"" : ""
                sourceSize.width: 48; sourceSize.height: 32
                Layout.preferredWidth: 24; Layout.preferredHeight: 16
                fillMode: Image.PreserveAspectFit
                Accessible.ignored: true
            }
            ColumnLayout {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                spacing: Theme.dp(1)
                Label { Layout.fillWidth: true; text: root.hoveredCompetition ? qsTr("Konkurs %1 z %2").arg(root.hoveredCompetition.number).arg(root.active.count) : ""; color: Theme.ink; font.pointSize: Theme.caption; font.bold: true; wrapMode: Text.WordWrap }
                Label { Layout.fillWidth: true; text: root.hoveredCompetition ? [root.hoveredCompetition.hill,"HS"+root.hoveredCompetition.hs].filter(function(v){return v}).join(" ") : ""; color: Theme.ink; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
                Label { Layout.fillWidth: true; text: root.hoveredCompetition ? [root.hoveredCompetition.weekday,root.hoveredCompetition.date].filter(function(v){return v}).join(", ") : ""; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
                Label { Layout.fillWidth: true; text: root.resultLine; color: root.hoveredCompetition && root.hoveredCompetition.position>0 ? Theme.ink : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
                Label { Layout.fillWidth: true; text: root.active ? root.active.label : ""; color: Theme.muted; font.pointSize: Theme.caption*0.9; wrapMode: Text.WordWrap }
            }
        }
    }
}
