import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: precipitation
    ViewArrival { target: precipitation; expressive: false }
    objectName: "weatherPrecipitation"
    property var backend
    readonly property var values: backend.values
    padding: Theme.dp(12)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: Theme.dp(20) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        Label { text: qsTr("Opady"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
        GridLayout {
            Layout.fillWidth: true; columns: width>Theme.dp(520)?3:width>Theme.dp(320)?2:1
            columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(6)
            SChoice {
                objectName: "weatherPrecipitationKind"
                Layout.fillWidth: true; Layout.minimumWidth: 0
                model: [qsTr("Bez opadów"),qsTr("Śnieg"),qsTr("Deszcz")]
                optionImages: ["qrc:/ui/icons/weather/none.svg","qrc:/ui/icons/weather/snow.svg","qrc:/ui/icons/weather/rain.svg"]
                currentIndex: precipitation.values.precipitationType
                Accessible.name: qsTr("Rodzaj opadów")
                onActivated: precipitation.backend.setValue("precipitationType",index)
            }
            SChoice {
                objectName: "weatherPrecipitationLevel"
                Layout.fillWidth: true; Layout.minimumWidth: 0
                visible: precipitation.values.precipitationType>0
                model: [qsTr("Słabe opady"),qsTr("Umiarkowane opady"),qsTr("Silne opady")]
                optionImages: ["qrc:/ui/icons/weather/intensity-light.svg","qrc:/ui/icons/weather/intensity-medium.svg","qrc:/ui/icons/weather/intensity-heavy.svg"]
                currentIndex: Math.max(0,precipitation.values.precipitationLevel-1)
                Accessible.name: qsTr("Natężenie opadów")
                onActivated: precipitation.backend.setValue("precipitationLevel",index+1)
            }
            SChoice {
                objectName: "weatherPrecipitationPattern"
                visible: precipitation.values.precipitationLevel>0
                Layout.fillWidth: true; Layout.minimumWidth: 0
                model: [qsTr("Stałe natężenie"),qsTr("Zmienne natężenie"),qsTr("Coraz silniejsze")]
                optionImages: ["qrc:/ui/icons/weather/pattern-steady.svg","qrc:/ui/icons/weather/pattern-variable.svg","qrc:/ui/icons/weather/pattern-increasing.svg"]
                currentIndex: precipitation.values.precipitationPattern
                Accessible.name: qsTr("Przebieg opadów")
                onActivated: precipitation.backend.setValue("precipitationPattern",index)
            }
        }
        Label {
            visible: precipitation.values.precipitationType>0
            text: precipitation.values.precipitationKind===0 ? qsTr("Śnieg stopniowo skraca skoki. Silniejsze opady oznaczają większą stratę odległości.") : qsTr("Słaby deszcz ma niewielki wpływ. Przy ulewie strata odległości rośnie znacznie szybciej.")
            color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
        }
    }
}
