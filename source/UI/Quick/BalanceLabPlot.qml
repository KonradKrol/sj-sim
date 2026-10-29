import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

ColumnLayout {
    id: plot
    property string caption
    property string xLabel
    property string yLabel
    property var series: []
    property int selectedPoint: 0
    property int forcedPoint: -1
    readonly property int displayedPoint: forcedPoint>=0 ? forcedPoint : selectedPoint
    signal pointSelected(real x)
    readonly property var colors: [Theme.muted,Theme.warning,Theme.accent]
    readonly property var all: series.reduce(function(out,s){return out.concat(s.points || [])},[])
    readonly property real minX: all.reduce(function(v,p){return Math.min(v,p.x)},all.length?all[0].x:0)
    readonly property real maxX: all.reduce(function(v,p){return Math.max(v,p.x)},all.length?all[0].x:1)
    readonly property real minY: all.reduce(function(v,p){return Math.min(v,p.lo!==undefined?p.lo:p.y)},all.length?all[0].y:0)
    readonly property real maxY: all.reduce(function(v,p){return Math.max(v,p.hi!==undefined?p.hi:p.y)},all.length?all[0].y:1)
    readonly property real pad: Math.max(.01,(maxY-minY)*.06)
    function px(x){return (x-minX)/Math.max(.000001,maxX-minX)*graph.width}
    function py(y){return graph.height-(y-minY+pad)/Math.max(.000001,maxY-minY+2*pad)*graph.height}
    function line(points){return points.map(function(p){return Qt.point(px(p.x),py(p.y))})}
    function band(points){return points.map(function(p){return Qt.point(px(p.x),py(p.hi!==undefined?p.hi:p.y))}).concat(points.slice().reverse().map(function(p){return Qt.point(px(p.x),py(p.lo!==undefined?p.lo:p.y))}))}
    function format(v){return Number(v).toLocaleString(Qt.locale(Theme.localeName),'f',2)}
    spacing: 5
    Label { text: plot.caption; color: Theme.ink; font.pointSize: Theme.section; font.family: Theme.displayFamily; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: plot.yLabel+" · "+plot.format(plot.minY)+" – "+plot.format(plot.maxY); color: Theme.muted; font.pointSize: Theme.caption }
    Rectangle {
        id: chart
        Layout.fillWidth: true; implicitHeight: Theme.dp(205)
        color: Theme.surface; radius: Theme.dp(7)
        activeFocusOnTab: true
        Accessible.role: Accessible.Graphic; Accessible.name: plot.caption+" · "+plot.xLabel+" · "+plot.yLabel
        Keys.onLeftPressed: {plot.selectedPoint=Math.max(0,plot.displayedPoint-1);if(plot.forcedPoint>=0 && plot.series.length && plot.series[0].points.length)plot.pointSelected(plot.series[0].points[plot.selectedPoint].x)}
        Keys.onRightPressed: {plot.selectedPoint=Math.min(plot.series.length ? plot.series[0].points.length-1 : 0,plot.displayedPoint+1);if(plot.forcedPoint>=0 && plot.series.length && plot.series[0].points.length)plot.pointSelected(plot.series[0].points[plot.selectedPoint].x)}
        Keys.onReturnPressed: {if(plot.series.length && plot.series[0].points[plot.selectedPoint])plot.pointSelected(plot.series[0].points[plot.selectedPoint].x)}
        Item {
            id: graph
            anchors.fill: parent; anchors.margins: Theme.dp(12); clip: true
            Repeater { model: 5; Rectangle { required property int index; width: graph.width; height: 1; y: index*graph.height/4; color: Theme.divider } }
            Repeater {
                model: plot.series
                Shape {
                    required property var modelData
                    required property int index
                    readonly property color tint: plot.colors[modelData.colorIndex!==undefined?modelData.colorIndex:index]
                    anchors.fill: parent
                    ShapePath { strokeWidth: 0; strokeColor: "transparent"; fillColor: Qt.rgba(tint.r,tint.g,tint.b,.10); PathPolyline { path: plot.band(modelData.points || []) } }
                    ShapePath { strokeWidth: 2; strokeColor: tint; fillColor: "transparent"; startX: modelData.points.length?plot.px(modelData.points[0].x):0; startY: modelData.points.length?plot.py(modelData.points[0].y):0; PathPolyline { path: plot.line(modelData.points || []) } }
                }
            }
            MouseArea {
                id: hover
                anchors.fill: parent; hoverEnabled: true
                onPositionChanged: plot.selectedPoint=Math.max(0,Math.min(plot.series.length ? plot.series[0].points.length-1 : 0,Math.round(mouseX/Math.max(1,width)*(plot.series.length ? plot.series[0].points.length-1 : 0))))
                onClicked: {chart.forceActiveFocus();if(plot.series.length && plot.series[0].points[plot.selectedPoint])plot.pointSelected(plot.series[0].points[plot.selectedPoint].x)}
            }
            Rectangle { visible: plot.forcedPoint>=0 || hover.containsMouse || chart.activeFocus; x: plot.series.length && plot.series[0].points.length ? plot.px(plot.series[0].points[Math.max(0,Math.min(plot.displayedPoint,plot.series[0].points.length-1))].x) : 0; height: graph.height; width: 1; color: Theme.ink; opacity: .45; Accessible.ignored: true }
        }
    }
    Label { text: plot.xLabel+" · "+plot.format(plot.minX)+" – "+plot.format(plot.maxX); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; wrapMode: Text.WordWrap }
    Repeater {
        model: plot.series
        Label {
            required property var modelData
            required property int index
            readonly property var point: modelData.points[Math.max(0,Math.min(plot.displayedPoint,modelData.points.length-1))]
            text: modelData.name+(point ? " · "+plot.format(point.x)+": "+plot.format(point.y)+(point.lo!==undefined?" ["+plot.format(point.lo)+" – "+plot.format(point.hi)+"]":"") : "")
            color: plot.colors[modelData.colorIndex!==undefined?modelData.colorIndex:index]; font.pointSize: Theme.caption
            Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
    }
}
