import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: direction
    property real degrees: 0
    property string label: qsTr("Kierunek wyjściowy")
    property string recordKey: ""
    property bool compact: false
    readonly property real head: Math.cos(degrees*Math.PI/180)
    readonly property real cross: Math.sin(degrees*Math.PI/180)
    readonly property string description: head>.35 ? (Math.abs(cross)>.35?qsTr("Pod narty + boczny"):qsTr("Pod narty")) : head<-.35 ? (Math.abs(cross)>.35?qsTr("W plecy + boczny"):qsTr("W plecy")) : cross>0 ? qsTr("Boczny z lewej") : qsTr("Boczny z prawej")
    readonly property color tone: head>.35?Theme.signal:head<-.35?Theme.stadiumDanger:Theme.headerMuted
    signal edited(real degrees)
    spacing: Theme.dp(10)
    Control {
        id: dial
        objectName: direction.compact ? "weatherStationDirection" : "weatherDirection"
        Layout.preferredWidth: Math.max(direction.compact?120:144,Theme.dp(direction.compact?150:180)); Layout.preferredHeight: Layout.preferredWidth
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Slider
        Accessible.name: direction.label+": "+Math.round(direction.degrees)+"°, "+direction.description
        Accessible.description: qsTr("Wybierz skąd wieje wiatr. Strzałki klawiatury zmieniają kąt o jeden stopień.")
        function step(delta){direction.edited((Math.round(direction.degrees)+delta+360)%360)}
        Keys.onRightPressed: step(1)
        Keys.onUpPressed: step(1)
        Keys.onLeftPressed: step(-1)
        Keys.onDownPressed: step(-1)
        Keys.onPressed: {if(event.key===Qt.Key_Home)direction.edited(0);else if(event.key===Qt.Key_End)direction.edited(359);else event.accepted=false}
        background: Rectangle { color: Theme.stadium; radius: Theme.dp(8); border.width: dial.visualFocus?2:1; border.color: dial.visualFocus?Theme.accent:Theme.outline }
        contentItem: Item {
            Label { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: Theme.dp(3); text: qsTr("POD NARTY"); color: Theme.signal; font.family: Theme.displayFamily; font.pointSize: Theme.caption; font.bold: true; Accessible.ignored: true }
            Label { anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.dp(3); text: qsTr("W PLECY"); color: Theme.stadiumDanger; font.family: Theme.displayFamily; font.pointSize: Theme.caption; font.bold: true; Accessible.ignored: true }
            Rectangle {
                id: circle
                anchors.centerIn: parent; width: parent.width-Theme.dp(42); height: width; radius: width/2
                color: "transparent"; border.width: 1; border.color: Theme.track
                Repeater {
                    model: 12
                    Item { required property int index; anchors.centerIn: parent; width: 1; height: parent.height; rotation: index*30; Rectangle { width: 1; height: Theme.dp(3); color: Theme.outline } }
                }
                Item {
                    anchors.centerIn: parent; width: Theme.dp(28); height: Theme.dp(42)
                    Rectangle { x: Theme.dp(5); width: Theme.dp(5); height: parent.height; radius: width/2; color: Theme.snow }
                    Rectangle { x: Theme.dp(18); width: Theme.dp(5); height: parent.height; radius: width/2; color: Theme.snow }
                    Rectangle { anchors.horizontalCenter: parent.horizontalCenter; y: Theme.dp(10); width: Theme.dp(13); height: width; radius: width/2; color: Theme.headerMuted }
                    Rectangle { anchors.horizontalCenter: parent.horizontalCenter; y: Theme.dp(22); width: Theme.dp(18); height: Theme.dp(7); radius: Theme.dp(3); color: Theme.accent }
                }
                Item {
                    anchors.centerIn: parent; width: 1; height: parent.height+Theme.dp(6); rotation: -direction.degrees
                    Rectangle { anchors.horizontalCenter: parent.horizontalCenter; y: Theme.dp(3); width: Theme.dp(4); height: Theme.dp(18); radius: width/2; color: direction.tone }
                    Label { anchors.horizontalCenter: parent.horizontalCenter; y: Theme.dp(13); text: "▼"; color: direction.tone; font.pixelSize: Theme.dp(18); Accessible.ignored: true }
                }
            }
            Label { anchors.left: parent.left; anchors.leftMargin: Theme.dp(4); anchors.verticalCenter: parent.verticalCenter; text: "↦"; color: Theme.headerMuted; font.pointSize: Theme.section; Accessible.ignored: true }
            Label { anchors.right: parent.right; anchors.rightMargin: Theme.dp(4); anchors.verticalCenter: parent.verticalCenter; text: "↤"; color: Theme.headerMuted; font.pointSize: Theme.section; Accessible.ignored: true }
            MouseArea {
                objectName: direction.compact?"weatherStationDirectionPointer":"weatherDirectionPointer"
                anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                function selectAt(x,y){var dx=width/2-x,dy=height/2-y;if(dx*dx+dy*dy<100)return;direction.edited((Math.round(Math.atan2(dx,dy)*180/Math.PI)+360)%360)}
                onPressed: {dial.forceActiveFocus();selectAt(mouse.x,mouse.y)}
                onPositionChanged: if(pressed)selectAt(mouse.x,mouse.y)
                onWheel: {dial.step(wheel.angleDelta.y>0?1:-1);wheel.accepted=true}
            }
        }
    }
    ColumnLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: Theme.dp(4)
        Label { text: direction.label; color: Theme.ink; font.pointSize: Theme.caption; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: direction.description; color: direction.head>.35?Theme.success:direction.head<-.35?Theme.danger:Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        NumberField {
            objectName: direction.compact?"weather-station-degrees":"weather-direction-degrees"
            Layout.fillWidth: true
            direct: false; flexibleDecimal: true; label: direction.label; recordKey: direction.recordKey
            fields: ({value:Math.round(direction.degrees),minimum:0,maximum:359,step:1,decimals:0,suffix:"°",name:direction.compact?"wind-station-degrees":"wind-direction-degrees"})
            onCommitted: function(value){direction.edited(value)}
        }
    }
}
