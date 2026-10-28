import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: local
    property var backend
    property real distance: Math.round(backend.values.k*.6)
    readonly property real maximum: backend.values.k*1.5
    readonly property var sample: {var references=backend.references;return backend.referenceAt(distance)}
    property real zeroDirection: 0
    readonly property real direction: Number(sample.strength || 0)<.001 ? zeroDirection : Number(sample.direction || 0)
    onMaximumChanged: if(distance>maximum)distance=maximum
    onDistanceChanged: zeroDirection=sample ? Number(sample.direction || 0) : 0
    spacing: Theme.dp(8)
    Label { text: qsTr("Wiatr w wybranym miejscu"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Miejsce na zeskoku"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        NumberField {
            objectName: "weather-local-distance"
            Layout.preferredWidth: Theme.dp(145)
            direct: false; label: qsTr("Odległość od progu")
            fields: ({value:local.distance,minimum:0,maximum:local.maximum,step:1,decimals:0,suffix:" m",name:"wind-local-distance"})
            onCommitted: function(value){local.distance=value}
        }
    }
    Rectangle {
        Layout.fillWidth: true; implicitHeight: Theme.dp(70)
        color: Theme.stadium; radius: Theme.dp(4)
        Image { anchors.fill: parent; anchors.margins: Theme.dp(6); source: "icons/weather/slope.svg"; sourceSize.width: 1000; sourceSize.height: 100; fillMode: Image.Stretch; Accessible.ignored: true }
        Repeater {
            model: local.backend.references
            Rectangle {
                required property var modelData
                x: modelData.distance/local.maximum*(parent.width-Theme.dp(12))+Theme.dp(3)
                y: parent.height*.7; width: Theme.dp(6); height: width; radius: width/2; color: Theme.headerMuted
                Accessible.ignored: true
            }
        }
        Rectangle { x: local.distance/local.maximum*(parent.width-Theme.dp(12))+Theme.dp(5); y: Theme.dp(5); width: Theme.dp(2); height: parent.height-Theme.dp(10); color: Theme.signal; Accessible.ignored: true }
        MouseArea {
            objectName: "weatherLocalPosition"
            anchors.fill: parent
            function selectAt(x){local.distance=Math.round(Math.max(0,Math.min(local.maximum,(x-Theme.dp(6))/(width-Theme.dp(12))*local.maximum)))}
            onPressed: selectAt(mouse.x)
            onPositionChanged: if(pressed)selectAt(mouse.x)
            cursorShape: Qt.CrossCursor
            Accessible.role: Accessible.Slider; Accessible.name: qsTr("Wybierz miejsce na zeskoku")
        }
    }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.fontSize(560)?2:1
        columnSpacing: Theme.dp(16); rowSpacing: Theme.dp(8)
        WindDirectionDial { degrees: local.direction; label: qsTr("Kierunek w tym miejscu"); Layout.fillWidth: true; Layout.minimumWidth: 0; onEdited: function(value){local.zeroDirection=value;local.backend.putReference(local.distance,Number(local.sample.strength || 0),value)} }
        ColumnLayout {
            Layout.fillWidth: true; Layout.minimumWidth: 0
            Label { text: qsTr("Siła w tym miejscu · m/s"); color: Theme.ink; font.pointSize: Theme.caption; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            NumberField {
                objectName: "weather-local-strength"
                Layout.fillWidth: true; direct: false; label: qsTr("Siła lokalnego wiatru")
                fields: ({value:Number(local.sample.strength || 0),minimum:0,maximum:15,step:.1,decimals:1,name:"wind-local-strength"})
                onCommitted: function(value){local.backend.putReference(local.distance,value,local.direction)}
            }
            SButton { text: qsTr("Usuń punkt"); quiet: true; enabled: local.sample.index>=0 && local.backend.references.length>1; onClicked: local.backend.removeReference(local.sample.index) }
        }
    }
    Label { text: qsTr("Wiatr przechodzi płynnie między punktami. Podmuchy nadal zależą od zmienności."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
}
