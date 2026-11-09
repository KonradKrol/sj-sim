import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: wind
    property var recorded: ({})
    property var forecastSamples: []
    property string recordedKey: ""
    property bool forecast: false
    property bool framed: true
    property real forecastWind: 0
    readonly property var beforeSamples: forecastSamples.length ? forecastSamples : recorded.before || []
    readonly property var samples: forecast ? beforeSamples : recorded.during || []
    signal dismissed()
    onRecordedKeyChanged: if(recordedKey.length)forecast=false
    Component.onCompleted: if(!(recorded.during || []).length)forecast=true
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
            Label { text: qsTr("Wiatr"); font.pointSize: Theme.body; font.bold: true; color: Theme.ink; Layout.fillWidth: true }
            Repeater {
                model: [qsTr("Przed"),qsTr("Po")]
                SButton {
                    required property int index
                    required property string modelData
                    readonly property bool selected: wind.forecast ? index===0 : index===1
                    text: modelData; dense: true
                    enabled: index===0 ? wind.beforeSamples.length>0 : (wind.recorded.during || []).length>0
                    foreground: selected ? Theme.snow : Theme.ink
                    background: Rectangle { color: parent.selected ? Theme.stadium : parent.hovered ? Theme.selection : Theme.raised; radius: Theme.dp(4) }
                    onClicked: wind.forecast=index===0
                }
            }
            SButton { text: "×"; dense: true; quiet: true; Accessible.name: qsTr("Zamknij podgląd wiatru"); onClicked: wind.dismissed() }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: wind.forecast ? qsTr("Przed następnym skokiem") : qsTr("Zapis skoku"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Label { text: wind.forecast ? wind.number(wind.forecastWind)+" m/s" : wind.recorded.measuredWind!==undefined ? wind.number(wind.recorded.measuredWind)+" m/s" : ""; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body; font.bold: true }
        }
        WindSlopePreview {
            Layout.fillWidth: true
            heading: ""
            interactive: false
            samples: wind.samples
        }
        WindMeasurementComparison { recorded: wind.forecast ? ({}) : wind.recorded; Layout.fillWidth: true }
    }
    }
}
