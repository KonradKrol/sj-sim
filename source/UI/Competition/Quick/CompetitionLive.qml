import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: screen
    color: dark ? "#101722" : "#edf1f6"
    Material.theme: dark ? Material.Dark : Material.Light
    Material.accent: Material.Blue
    property bool dark: live.dark
    property int page: 1
    property var offsets: ({})
    property color ink: dark ? "#edf2fc" : "#19283e"
    property color muted: dark ? "#9cacc2" : "#67788d"
    property color panel: dark ? "#1b2635" : "#ffffff"
    property color line: dark ? "#304055" : "#dce3ed"
    property bool wide: width >= 860 && height >= 600
    property real savedMainY: 0
    function remember(key, x, y) { offsets[key]={x:x,y:y} }
    function position(key) { return offsets[key] || {x:0,y:0} }
    function section(index) { return live.sections.length>index ? live.sections[index].children : [] }
    function find(name, nodes) {
        nodes=nodes || section(0)
        for(var i=0;i<nodes.length;++i) {
            if(nodes[i].name===name) return nodes[i]
            if(nodes[i].children) { var child=find(name,nodes[i].children); if(child.id) return child }
        }
        return {kind:"group",children:[]}
    }
    function text(name) { return find(name).text || "—" }
    function jump() {
        var next=find("pushButton_goToNext"), jump=find("pushButton_jump")
        if(next.id && next.enabled) live.activate(next.id)
        else if(jump.id && jump.enabled) live.activate(jump.id)
    }
    Shortcut { sequence:"Ctrl+Return"; enabled:live.competition; onActivated:screen.jump() }
    Shortcut { sequence:"Alt+1"; onActivated:screen.page=1 }
    Shortcut { sequence:"Alt+2"; onActivated:screen.page=1 }
    Shortcut { sequence:"Alt+3"; onActivated:screen.page=2 }
    Shortcut { sequence:"Alt+4"; onActivated:screen.page=3 }
    Shortcut { sequence:"Alt+5"; onActivated:screen.page=4 }
    ColumnLayout {
        anchors.fill:parent; anchors.margins:screen.width<700 ? 10 : 16; spacing:10
        RowLayout {
            Layout.fillWidth:true; spacing:10
            ColumnLayout {
                Layout.fillWidth:true; spacing:2
                Label { text:live.competition ? screen.text("label_hillNameAndHSPoint") : (live.sections.length ? live.sections[0].title : ""); color:screen.ink; font.pixelSize:20; font.weight:Font.DemiBold; Layout.fillWidth:true; elide:Text.ElideRight }
                Label { visible:live.competition; text:(live.sections.length ? (live.sections[0].stage || qsTr("Konkurs")) : qsTr("Konkurs"))+(screen.width>=700 ? "     ·     Wiatr "+screen.text("label_actualAvgWind")+qsTr("     ·     Śnieg: ")+screen.text("label_snowLevel") : ""); color:screen.muted; font.pixelSize:12; Layout.fillWidth:true; elide:Text.ElideRight }
            }
            LiveButton { theme:screen; text:screen.dark ? "☀" : "☾"; implicitWidth:40; onClicked:live.dark=!live.dark; ToolTip.visible:hovered; ToolTip.text:qsTr("Jasny / ciemny motyw") }
            LiveButton { theme:screen; visible:live.competition; text:"Qt Widgets"; onClicked:live.classic() }
        }
        LiveArena { visible:live.competition; Layout.fillWidth:true; Layout.fillHeight:true; theme:screen }
        Rectangle {
            visible:!live.competition; Layout.fillWidth:true; Layout.fillHeight:true; color:screen.panel; radius:9
            ScrollView {
                id:mainScroll; objectName:"liveMainScroll"
                anchors.fill:parent; anchors.margins:14; clip:true; contentWidth:availableWidth
                Column {
                    width:mainScroll.availableWidth; spacing:10
                    Repeater { model:!live.competition ? screen.section(0) : []; LiveNode { width:parent.width; dataNode:modelData; theme:screen } }
                }
            }
        }
    }
    Connections {
        target:live
        function onAboutToChange() { if(!live.competition) screen.savedMainY=mainScroll.contentItem.contentY }
        function onChanged() {
            if(!live.competition) Qt.callLater(function() {
                mainScroll.contentItem.contentY=Math.max(0,Math.min(screen.savedMainY,mainScroll.contentItem.contentHeight-mainScroll.availableHeight))
            })
        }
    }
}
