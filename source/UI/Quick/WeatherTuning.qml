import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: tuning
    property var backend
    readonly property var values: backend.values
    spacing: Theme.dp(10)
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(740) ? 4 : width>Theme.dp(400) ? 2 : 1
        columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(8)
        ColumnLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Kierunek wyjściowy"); color: Theme.muted; font.pointSize: Theme.caption }
            SChoice {
                objectName: "weatherDirection"
                Layout.fillWidth: true
                model: [qsTr("Pod narty"),qsTr("W plecy"),qsTr("Boczny z lewej"),qsTr("Boczny z prawej")]
                readonly property var angles: [0,180,90,270]
                currentIndex: {var angle=tuning.values.direction;return angle<45 || angle>=315 ? 0 : angle<135 ? 2 : angle<225 ? 1 : 3}
                Accessible.name: qsTr("Kierunek wyjściowy")
                onActivated: tuning.backend.setValue("direction",angles[index])
            }
        }
        WeatherNumber { backend: tuning.backend; fieldKey: "strength"; label: qsTr("Siła wyjściowa · m/s"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("Punkt wyjścia. Mocny wiatr może być stabilny, a słaby — bardzo zmienny.") }
        WeatherNumber { backend: tuning.backend; fieldKey: "variability"; factor: .1; label: qsTr("Zmienność w czasie · 0–10"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("0: bez zmian w czasie. Wyżej: większe i szybsze zmiany, podmuchy oraz możliwe odwrócenia kierunku.") }
        WeatherNumber { backend: tuning.backend; fieldKey: "differences"; factor: .1; label: qsTr("Różnice na zeskoku · 0–10"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("0: podobny wiatr wzdłuż zeskoku. Wyżej: poszczególne fragmenty mogą mieć znacznie inne warunki.") }
    }
}
