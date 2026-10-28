import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: tuning
    property var backend
    readonly property var values: backend.values
    padding: Theme.dp(12)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: Theme.surface; radius: Theme.dp(5); border.width: 2; border.color: Theme.accent }
    contentItem: ColumnLayout {
        spacing: Theme.dp(12)
        RowLayout {
            Layout.fillWidth: true
            Rectangle { Layout.preferredWidth: Theme.dp(4); Layout.preferredHeight: Theme.dp(24); color: Theme.accent }
            Label { text: qsTr("Dostosowanie wiatru"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        GridLayout {
            Layout.fillWidth: true; columns: width>Theme.fontSize(600)?2:1
            columnSpacing: Theme.dp(18); rowSpacing: Theme.dp(10)
            WindDirectionDial { degrees: tuning.values.direction; Layout.fillWidth: true; Layout.minimumWidth: 0; onEdited: function(value){tuning.backend.setValue("direction",value)} }
            WeatherNumber { backend: tuning.backend; fieldKey: "strength"; label: qsTr("Siła wyjściowa · m/s"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("Mocny wiatr może być stabilny, a słaby — zmienny.") }
            WeatherNumber { backend: tuning.backend; fieldKey: "variability"; factor: .1; label: qsTr("Zmienność w czasie · 0–10"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("0: bez zmian. Wyżej: silniejsze podmuchy i szybsze zmiany.") }
            WeatherNumber { backend: tuning.backend; fieldKey: "differences"; factor: .1; label: qsTr("Różnice na zeskoku · 0–10"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("Wyższa wartość: większe różnice wzdłuż skoczni.") }
        }
    }
}
