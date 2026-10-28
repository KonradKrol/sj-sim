import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: precipitation
    objectName: "weatherPrecipitation"
    property var backend
    readonly property var values: backend.values
    property bool rainTuning: false
    padding: Theme.dp(12)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: Theme.dp(20) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Opady"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
            SChoice { Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(120); model: [qsTr("Śnieg"),qsTr("Deszcz")]; currentIndex: precipitation.values.surface; Accessible.name: qsTr("Rodzaj nawierzchni i opadów"); onActivated: precipitation.backend.setValue("surface",index) }
        }
        GridLayout {
            Layout.fillWidth: true; columns: width>=Theme.dp(620)?5:width>=Theme.dp(380)?3:2
            columnSpacing: Theme.dp(6); rowSpacing: Theme.dp(6)
            Repeater {
                model: [qsTr("Brak"),qsTr("Stałe"),qsTr("Zmienne"),precipitation.values.surface===0?qsTr("Śnieżyca"):qsTr("Ulewa"),qsTr("Nasilające się")]
                SButton {
                    required property int index
                    required property string modelData
                    objectName: "weatherPrecipitation-"+index
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    text: modelData; checkable: true; checked: precipitation.values.precipitationScenario===index
                    primary: checked; enabled: precipitation.values.ready
                    font.family: Theme.bodyFamily; font.pointSize: Theme.body; font.weight: Font.DemiBold
                    implicitHeight: Math.max(Theme.dp(40),contentItem.implicitHeight+Theme.dp(12))
                    onClicked: {precipitation.forceActiveFocus();precipitation.backend.setValue("precipitationScenario",index)}
                }
            }
        }
        RowLayout {
            visible: precipitation.values.precipitationScenario>0
            Layout.fillWidth: true
            SButton { text: precipitation.rainTuning ? qsTr("Ukryj natężenie opadów") : qsTr("Dostosuj opady"); quiet: true; onClicked: precipitation.rainTuning=!precipitation.rainTuning }
        }
        GridLayout {
            visible: precipitation.rainTuning && precipitation.values.precipitationScenario>0
            Layout.fillWidth: true; columns: width>Theme.dp(600)?3:width>Theme.dp(360)?2:1; columnSpacing: Theme.dp(10)
            WeatherNumber { backend: precipitation.backend; fieldKey: "precipitationIntensity"; label: qsTr("Natężenie · 0–10"); minimum: precipitation.values.precipitationScenario===3?8:0; maximum: 10; Layout.fillWidth: true; explanation: qsTr("Wyższa wartość mocniej pogarsza skoki. Śnieżyca lub ulewa oznacza co najmniej poziom 8.") }
            WeatherNumber { visible: precipitation.values.precipitationScenario===2; backend: precipitation.backend; fieldKey: "precipitationVariation"; label: qsTr("Zakres wahań · 0–10"); maximum: 10; Layout.fillWidth: true }
            WeatherNumber { visible: precipitation.values.precipitationScenario===4; backend: precipitation.backend; fieldKey: "precipitationIncrease"; label: qsTr("Wzrost natężenia · na minutę"); maximum: 10; Layout.fillWidth: true }
        }
    }
}
