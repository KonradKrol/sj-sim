import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: preview
    property var samples: []
    property string heading
    property var backend
    property bool interactive: true
    readonly property int stationCount: Math.max(1,samples.length)
    property int selectedStation: -1
    padding: Theme.dp(14)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Surface { fillColor: Theme.stadium; sweep: Theme.dp(24) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(6)
        Label { text: preview.heading; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
        Item {
            id: slope
            Layout.fillWidth: true; Layout.preferredHeight: Math.max(Theme.dp(140),Theme.bodyMetrics.height*3+Theme.dp(30))
            Image { anchors.fill: parent; source: "qrc:/ui/icons/weather/slope.svg"; sourceSize.width: 1600; sourceSize.height: 320; fillMode: Image.Stretch; Accessible.ignored: true }
            Repeater {
                model: preview.samples
                AbstractButton {
                    id: station
                    required property int index
                    required property var modelData
                    readonly property bool available: modelData.available !== false
                    objectName: "windStation-"+index
                    x: (index+.5)*slope.width/preview.stationCount-width/2
                    y: slope.height*(.02+.35*Math.pow((index+.5)/preview.stationCount,1.4))
                    width: slope.width/preview.stationCount-Theme.dp(2); height: Math.max(Theme.dp(74),contentItem.implicitHeight+Theme.dp(6))
                    hoverEnabled: true
                    focusPolicy: preview.interactive ? Qt.StrongFocus : Qt.NoFocus
                    onPressed: if(preview.interactive)forceActiveFocus()
                    checked: preview.interactive && preview.selectedStation===index
                    Accessible.role: preview.interactive ? Accessible.Button : Accessible.StaticText
                    Accessible.name: !available ? qsTr("%1 m: poza skokiem").arg(Math.round(modelData.distance)) : qsTr("%1 m: %2 m/s").arg(Math.round(modelData.distance)).arg(Number(modelData.strength).toLocaleString(Qt.locale(Theme.localeName),'f',1))
                    background: Rectangle { color: station.checked?Theme.track:station.hovered && preview.interactive?"#244157":"transparent"; radius: Theme.dp(4); border.width: station.checked || (station.hovered && preview.interactive) || station.visualFocus ? 1 : 0; border.color: station.checked?Theme.signal:station.visualFocus?Theme.accent:Theme.track }
                    contentItem: Column {
                        spacing: Theme.dp(2)
                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: !station.available || station.modelData.strength<.01 ? "—" : "↑"; rotation: !station.available || station.modelData.strength<.01 ? 0 : station.modelData.direction-65; color: !station.available ? Theme.headerMuted : station.modelData.head>station.modelData.strength*.35 ? Theme.signal : station.modelData.head< -station.modelData.strength*.35 ? Theme.stadiumDanger : Theme.headerMuted; font.pixelSize: Theme.dp(36); font.bold: true; Accessible.ignored: true }
                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: station.available ? Number(station.modelData.strength).toLocaleString(Qt.locale(Theme.localeName),'f',1) : "—"; color: Theme.snow; font.family: Theme.mono; font.pointSize: Theme.body; Accessible.ignored: true }
                        Label { anchors.horizontalCenter: parent.horizontalCenter; text: qsTr("%1 m").arg(Math.round(station.modelData.distance)); color: Theme.headerMuted; font.pointSize: Theme.caption; Accessible.ignored: true }
                    }
                    onClicked: if(preview.interactive)preview.selectedStation=preview.selectedStation===index?-1:index
                    ToolTip.visible: hovered && !down && !checked
                    ToolTip.delay: 700
                    ToolTip.text: preview.interactive ? qsTr("Zmień wiatr w tym miejscu") : !station.available ? qsTr("Odcinek poza skokiem") : qsTr("Odcinek %1–%2 m").arg(Number(modelData.begin).toLocaleString(Qt.locale(Theme.localeName),'f',1)).arg(Number(modelData.end).toLocaleString(Qt.locale(Theme.localeName),'f',1))
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("↖ Pod narty    ↘ W plecy"); color: Theme.headerMuted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Label { text: "m/s"; color: Theme.headerMuted; font.pointSize: Theme.caption; Accessible.ignored: true }
        }
        Loader {
            active: preview.selectedStation>=0 && !!preview.backend
            visible: active
            Layout.fillWidth: true
            Layout.preferredHeight: status===Loader.Ready && item ? item.implicitHeight : 0
            sourceComponent: Component {
                Control {
                    padding: Theme.dp(10)
                    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
                    background: Rectangle { color: Theme.surface; radius: Theme.dp(5); border.width: 2; border.color: Theme.accent }
                    contentItem: WindStationEditor { backend: preview.backend; stationIndex: preview.selectedStation; onClosed: preview.selectedStation=-1 }
                }
            }
        }
    }
}
