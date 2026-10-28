import QtQuick 2.15
import QtQuick.Controls 2.15

Control {
    id: preview
    property var samples: []
    property string heading
    implicitHeight: Math.max(Theme.dp(225),Theme.bodyMetrics.height*5+Theme.dp(70))
    padding: Theme.dp(14)
    background: Surface { fillColor: Theme.stadium; sweep: Theme.dp(24) }
    contentItem: Item {
        Label { text: preview.heading; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; elide: Text.ElideRight; width: parent.width }
        Item {
            id: slope
            anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.topMargin: Theme.dp(30); anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.bodyMetrics.height+Theme.dp(3)
            Image {
                anchors.fill: parent
                source: "qrc:/ui/icons/weather/slope.svg"
                sourceSize.width: 1600; sourceSize.height: 320
                fillMode: Image.Stretch
                Accessible.ignored: true
            }
            Repeater {
                model: preview.samples
                Item {
                    id: station
                    required property int index
                    required property var modelData
                    x: (index+.5)*slope.width/6-width/2
                    y: slope.height*(.02+.35*Math.pow((index+.5)/6,1.4))
                    width: slope.width/6; height: Theme.dp(70)
                    Accessible.role: Accessible.StaticText
                    Accessible.name: qsTr("%1 m: %2 m/s, kierunek %3 stopni").arg(Math.round(modelData.distance)).arg(Number(modelData.strength).toLocaleString(Qt.locale(Theme.localeName),'f',1)).arg(Math.round(modelData.direction))
                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: Theme.dp(2)
                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: station.modelData.strength<.01 ? "—" : "↑"; rotation: station.modelData.strength<.01 ? 0 : station.modelData.direction-65; color: station.modelData.head>station.modelData.strength*.35 ? Theme.signal : station.modelData.head< -station.modelData.strength*.35 ? Theme.stadiumDanger : Theme.headerMuted; font.pixelSize: Theme.dp(42); font.bold: true; Accessible.ignored: true }
                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: Number(station.modelData.strength).toLocaleString(Qt.locale(Theme.localeName),'f',1); color: Theme.snow; font.family: Theme.mono; font.pointSize: Theme.body; Accessible.ignored: true }
                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: qsTr("%1 m").arg(Math.round(station.modelData.distance)); color: Theme.headerMuted; font.pointSize: Theme.caption; Accessible.ignored: true }
                    }
                }
            }

        }
        Label { anchors.left: parent.left; anchors.bottom: parent.bottom; text: qsTr("↖ Pod narty    ↘ W plecy"); color: Theme.headerMuted; font.pointSize: Theme.caption }
        Label { anchors.right: parent.right; anchors.bottom: parent.bottom; text: "m/s"; color: Theme.headerMuted; font.pointSize: Theme.caption; Accessible.ignored: true }

    }
}
