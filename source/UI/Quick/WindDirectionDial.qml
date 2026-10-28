import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: direction
    property real degrees: 0
    property string label: qsTr("Kierunek wyjściowy")
    signal edited(real degrees)
    spacing: Theme.dp(10)
    Dial {
        id: dial
        objectName: "weatherDirection"
        Layout.preferredWidth: Math.max(84,Theme.dp(116)); Layout.preferredHeight: Layout.preferredWidth
        from: 0; to: 360; stepSize: 1; wrap: true
        value: direction.degrees
        Accessible.name: direction.label
        Accessible.description: qsTr("0°: pod narty. 180°: w plecy. Przeciągnij wskazówkę lub użyj strzałek.")
        onMoved: direction.edited(Math.round(value)%360)
        background: Rectangle {
            width: dial.width; height: dial.height; radius: width/2; color: Theme.stadium
            border.width: dial.visualFocus?3:1; border.color: dial.visualFocus?Theme.accent:Theme.outline
            Repeater {
                model: 12
                Item {
                    required property int index
                    anchors.centerIn: parent; width: 1; height: parent.height-Theme.dp(9); rotation: index*30
                    Rectangle { anchors.top: parent.top; width: 1; height: Theme.dp(5); color: index%3===0?Theme.snow:Theme.outline }
                }
            }
            Label { anchors.horizontalCenter: parent.horizontalCenter; y: Theme.dp(9); text: "0°"; color: Theme.headerMuted; font.pointSize: Theme.caption*.85; Accessible.ignored: true }
        }
        handle: Item {
            anchors.centerIn: parent
            width: dial.width*.55; height: dial.height*.55
            rotation: dial.value
            Rectangle { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; width: Theme.dp(3); height: parent.height*.56; radius: width/2; color: Theme.signal }
            Label { anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: -Theme.dp(9); text: "▲"; color: Theme.signal; font.pointSize: Theme.caption; Accessible.ignored: true }
            Rectangle { anchors.centerIn: parent; width: Theme.dp(9); height: width; radius: width/2; color: Theme.snow }
        }
    }
    ColumnLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: Theme.dp(4)
        Label { text: direction.label; color: Theme.ink; font.pointSize: Theme.caption; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        NumberField {
            objectName: "weather-direction-degrees"
            Layout.fillWidth: true
            direct: false; label: direction.label
            fields: ({value:Math.round(direction.degrees),minimum:0,maximum:359,step:1,decimals:0,suffix:"°",name:"wind-direction-degrees"})
            onCommitted: function(value){direction.edited(value)}
        }
        Label { text: qsTr("0° pod narty · 180° w plecy"); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    }
}
