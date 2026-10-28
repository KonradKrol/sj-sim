import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: plot
    property string caption
    property string xLabel
    property string yLabel
    property var series: []
    property int selectedPoint: 0
    onSeriesChanged: selectedPoint=Math.max(0,Math.min(selectedPoint,series.length ? series[0].points.length-1 : 0))
    property real minimumX: 0
    property real maximumX: 1
    property real minimumY: 0
    property real maximumY: 1
    readonly property var palette: [Theme.accent,Theme.warning,Theme.ink,Theme.muted]
    function format(v){return Number(v).toLocaleString(Qt.locale(Theme.localeName),'f',2)}
    function px(v){return (v-minimumX)/Math.max(.00001,maximumX-minimumX)*graph.width}
    function py(v){return graph.height-(v-minimumY)/Math.max(.00001,maximumY-minimumY)*graph.height}
    spacing: 5
    Label { text: plot.caption; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: plot.yLabel+" · "+plot.format(plot.minimumY)+" – "+plot.format(plot.maximumY); color: Theme.muted; font.pointSize: Theme.caption }
    Rectangle {
        id: surface
        activeFocusOnTab: true
        Keys.onLeftPressed: plot.selectedPoint=Math.max(0,plot.selectedPoint-1)
        Keys.onRightPressed: plot.selectedPoint=Math.max(0,Math.min(plot.series.length ? plot.series[0].points.length-1 : 0,plot.selectedPoint+1))
        Layout.fillWidth: true; implicitHeight: Theme.dp(190)
        color: Theme.surface; radius: Theme.dp(5)
        Item {
            id: graph
            clip: true
            anchors.fill: parent; anchors.margins: Theme.dp(10)
            Repeater { model: 5; Rectangle { required property int index; width: graph.width; height: 1; y: index*graph.height/4; color: Theme.divider } }
            Repeater {
                model: plot.series
                Shape {
                    required property var modelData
                    required property int index
                    anchors.fill: parent
                    ShapePath {
                        startX: modelData.points.length?plot.px(modelData.points[0].x):0; startY: modelData.points.length?plot.py(modelData.points[0].y):0
                        strokeColor: plot.palette[index%plot.palette.length]; strokeWidth: 2; fillColor: "transparent"
                        PathPolyline { path: modelData.points.map(function(p){return Qt.point(plot.px(p.x),plot.py(p.y))}) }
                    }
                }
            }
            Rectangle { visible: hover.containsMouse || surface.activeFocus; x: plot.series.length && plot.series[0].points.length ? plot.px(plot.series[0].points[Math.max(0,Math.min(plot.selectedPoint,plot.series[0].points.length-1))].x) : 0; height: parent.height; width: 1; color: Theme.ink }
            MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton; onPositionChanged: plot.selectedPoint=Math.round(mouseX/Math.max(1,width)*(plot.series.length ? plot.series[0].points.length-1 : 0)) }
        }
        Accessible.role: Accessible.Graphic
        Accessible.name: plot.caption+" · "+plot.xLabel+" · "+plot.yLabel
    }
    Label { text: plot.xLabel+" · "+plot.format(plot.minimumX)+" – "+plot.format(plot.maximumX); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
    Repeater {
        model: plot.series
        Label {
            required property var modelData
            required property int index
            readonly property int selected: Math.max(0,Math.min(modelData.points.length-1,plot.selectedPoint))
            text: modelData.name+(modelData.points.length ? " · "+plot.format(modelData.points[selected].x)+": "+plot.format(modelData.points[selected].y) : "")
            color: plot.palette[index%plot.palette.length]; font.pointSize: Theme.caption
            Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
    }
}
