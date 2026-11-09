import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: wind
    property var recorded: ({})
    property var forecastSamples: []
    property bool forecast: false
    property bool framed: true
    property bool draggable: false
    property real forecastWind: 0
    readonly property var beforeSamples: forecastSamples.length ? forecastSamples : recorded.before || []
    readonly property var samples: forecast ? beforeSamples : recorded.during || []
    signal dismissed()
    signal moveRequested(real dx,real dy)
    implicitWidth: 0
    implicitHeight: details.implicitHeight+topPadding+bottomPadding
    padding: framed ? Theme.dp(10) : 0
    background: Rectangle { visible: wind.framed; color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
    function number(value){return Number(value).toLocaleString(Qt.locale(Theme.localeName),'f',2)}
    contentItem: Item {
    ColumnLayout {
        id: details
        width: parent.width
        spacing: Theme.dp(6)
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(4)
            Item {
                Layout.fillWidth: true
                implicitHeight: Theme.dp(28)
                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.dp(7)
                    Label { visible: wind.draggable; text: "⠿"; color: Theme.muted; font.pixelSize: Theme.dp(17); Accessible.ignored: true }
                    Label { text: qsTr("Prognoza wiatru"); font.pointSize: Theme.body; font.bold: true; color: Theme.ink; Layout.fillWidth: true }
                }
                MouseArea {
                    id: dragHandle
                    anchors.fill: parent
                    enabled: wind.draggable
                    hoverEnabled: true
                    cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                    preventStealing: true
                    activeFocusOnTab: true
                    Keys.onLeftPressed: wind.moveRequested(-10,0)
                    Keys.onRightPressed: wind.moveRequested(10,0)
                    Keys.onUpPressed: wind.moveRequested(0,-10)
                    Keys.onDownPressed: wind.moveRequested(0,10)
                    property point lastPosition
                    onPressed: function(mouse){lastPosition=mapToGlobal(mouse.x,mouse.y)}
                    onPositionChanged: function(mouse){
                        if(!pressed)return
                        var position=mapToGlobal(mouse.x,mouse.y)
                        wind.moveRequested(position.x-lastPosition.x,position.y-lastPosition.y)
                        lastPosition=position
                    }
                    Accessible.name: qsTr("Przesuń podgląd wiatru")
                }
            }
            Label { text: wind.number(wind.forecastWind)+" m/s"; color: wind.forecastWind>0 ? Theme.success : wind.forecastWind<0 ? Theme.danger : Theme.muted; font.family: Theme.mono; font.pointSize: Theme.body; font.bold: true }
            SButton { text: "×"; dense: true; quiet: true; Accessible.name: qsTr("Zamknij podgląd wiatru"); onClicked: wind.dismissed() }
        }
        WindSlopePreview {
            Layout.fillWidth: true
            heading: ""
            compact: true
            interactive: false
            samples: wind.samples
        }
        WindMeasurementComparison { recorded: wind.forecast ? ({}) : wind.recorded; Layout.fillWidth: true }
    }
    }
}
