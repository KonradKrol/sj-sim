import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15

Rectangle {
    id: chart
    property var points: []
    property int selected: -1
    readonly property var current: points[Math.max(0,selected)] || ({})
    readonly property int worst: points.reduce(function(value,p){return Math.max(value,p.position)},1)
    readonly property int step: Math.max(1,Math.ceil((worst-1)/5))
    readonly property int bottomRank: 1+step*5
    readonly property bool inspecting: points.length>0 && (pointer.containsMouse || activeFocus)
    color: Theme.surface; radius: Theme.radius; border.color: Theme.divider
    implicitHeight: Theme.dp(340)
    activeFocusOnTab: points.length>0
    Accessible.role: Accessible.Graphic; Accessible.name: qsTr("Miejsca w kolejnych zawodach")
    Accessible.description: qsTr("Liczba występów: %1").arg(points.length)
    Keys.onLeftPressed: selected=Math.max(0,selected-1)
    Keys.onRightPressed: selected=Math.min(points.length-1,selected+1)
    function px(index) {return points.length>1 ? index/(points.length-1)*graph.width : graph.width/2}
    function py(position) {return (position-1)/(bottomRank-1)*graph.height}
    onPointsChanged: selected=-1
    Label { x: Theme.dp(16); y: Theme.dp(12); text: qsTr("Miejsce"); color: Theme.muted; font.pointSize: Theme.caption }
    Item {
        id: graph
        anchors.fill: parent; anchors.leftMargin: Theme.dp(48); anchors.rightMargin: Theme.dp(24); anchors.topMargin: Theme.dp(48); anchors.bottomMargin: Theme.dp(38)
        Repeater {
            model: 6
            Item {
                required property int index
                y: index*graph.height/5; width: graph.width
                Rectangle { width: parent.width; height: 1; color: Theme.divider }
                Label { anchors.right: parent.left; anchors.rightMargin: Theme.dp(10); y: -height/2; text: 1+index*chart.step; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
            }
        }
        Shape {
            anchors.fill: parent; visible: chart.points.length>1
            ShapePath {
                strokeColor: Theme.accent; strokeWidth: Theme.dp(2.5); fillColor: "transparent"; capStyle: ShapePath.RoundCap; joinStyle: ShapePath.RoundJoin
                PathPolyline { path: chart.points.map(function(point,index){return Qt.point(chart.px(index),chart.py(point.position))}) }
            }
        }
        Repeater {
            model: chart.points.length<=180 ? chart.points : []
            Rectangle {
                required property int index
                required property var modelData
                x: chart.px(index)-width/2; y: chart.py(modelData.position)-height/2
                width: Theme.dp(5); height: width; radius: width/2; color: Theme.accent; border.color: Theme.surface; border.width: 1
            }
        }
        Rectangle { visible: chart.inspecting; x: chart.px(Math.max(0,chart.selected)); width: 1; height: graph.height; color: Theme.muted; opacity: .45 }
        Rectangle { visible: chart.inspecting; x: chart.px(Math.max(0,chart.selected))-width/2; y: chart.py(chart.current.position || 1)-height/2; width: Theme.dp(10); height: width; radius: width/2; color: Theme.accent; border.color: Theme.surface; border.width: 2 }
        MouseArea {
            id: pointer
            anchors.fill: parent; hoverEnabled: true
            function inspect(x) {chart.selected=Math.max(0,Math.min(chart.points.length-1,Math.round(x/Math.max(1,width)*(chart.points.length-1))))}
            onEntered: inspect(mouseX)
            onPositionChanged: inspect(mouseX)
            onClicked: {inspect(mouse.x);chart.forceActiveFocus()}
        }
        Label { anchors.left: parent.left; anchors.top: parent.bottom; anchors.topMargin: Theme.dp(12); text: chart.points.length ? "1" : ""; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        Label { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.bottom; anchors.topMargin: Theme.dp(12); text: qsTr("Kolejne występy"); color: Theme.muted; font.pointSize: Theme.caption }
        Label { anchors.right: parent.right; anchors.top: parent.bottom; anchors.topMargin: Theme.dp(12); text: chart.points.length>1 ? chart.points.length : ""; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
    }
    Rectangle {
        visible: chart.inspecting; z: 2
        width: Math.min(Theme.dp(270),chart.width-Theme.dp(24)); height: details.implicitHeight+Theme.dp(24)
        x: Math.max(Theme.dp(8),Math.min(chart.width-width-Theme.dp(8),graph.x+chart.px(Math.max(0,chart.selected))-width/2))
        y: Math.max(Theme.dp(8),Math.min(chart.height-height-Theme.dp(8),graph.y+chart.py(chart.current.position || 1)-height-Theme.dp(12)))
        color: Theme.raised; radius: Theme.dp(8); border.color: Theme.accent
        Column {
            id: details
            anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.margins: Theme.dp(12); spacing: Theme.dp(5)
            Label { width: parent.width; text: chart.current.event || ""; color: Theme.ink; font.bold: true; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
            Label { width: parent.width; text: [chart.current.date,chart.current.kind].filter(function(v){return !!v}).join(" · "); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
            Label { text: qsTr("%1. miejsce · %2 pkt").arg(chart.current.position || "").arg(chart.current.points || ""); color: Theme.accent; font.family: Theme.mono; font.bold: true; font.pointSize: Theme.body }
        }
    }
    EmptyState { visible: !chart.points.length; anchors.centerIn: parent; width: Math.min(parent.width-Theme.dp(32),Theme.dp(400)); title: qsTr("Brak wyników"); description: qsTr("Wybierz rodzaje wydarzeń lub rozegraj zawody.") }
}
