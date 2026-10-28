import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: editor
    property var backend
    property int stationIndex: -1
    readonly property var point: {var references=backend.references;return backend.station(stationIndex)}
    signal closed()
    spacing: Theme.dp(7)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Wiatr · %1 m").arg(Number(editor.point.distance || 0).toLocaleString(Qt.locale(Theme.localeName),'f',0)); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true }
        SButton { text: "×"; quiet: true; Accessible.name: qsTr("Zamknij edycję miejsca"); onClicked: editor.closed() }
    }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.fontSize(540)?2:1; columnSpacing: Theme.dp(14); rowSpacing: Theme.dp(7)
        WindDirectionDial { compact: true; degrees: Number(editor.point.direction || 0); label: qsTr("Skąd wieje"); recordKey: editor.stationIndex+":"+editor.backend.values.seed; Layout.fillWidth: true; Layout.minimumWidth: 0; onEdited: function(value){editor.backend.setStation(editor.stationIndex,"direction",value)} }
        ColumnLayout {
            Layout.fillWidth: true; Layout.minimumWidth: 0
            Label { text: qsTr("Siła wyjściowa · m/s"); color: Theme.ink; font.pointSize: Theme.caption; font.bold: true }
            NumberField {
                objectName: "weather-station-strength"
                Layout.fillWidth: true; direct: false; flexibleDecimal: true; label: qsTr("Siła wiatru w wybranym miejscu")
                recordKey: editor.stationIndex+":"+editor.backend.values.seed
                fields: ({value:Number(editor.point.strength || 0),minimum:0,maximum:15,step:.1,decimals:1,name:"wind-station-strength"})
                onCommitted: function(value){editor.backend.setStation(editor.stationIndex,"strength",value)}
            }
        }
    }
}
