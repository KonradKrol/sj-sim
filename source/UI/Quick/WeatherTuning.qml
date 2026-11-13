import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: tuning
    property var backend
    property real viewportWidth
    readonly property var values: backend.values
    padding: Theme.dp(12)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: Theme.raised; radius: Theme.dp(5); border.width: 1; border.color: Theme.outline }
    contentItem: ColumnLayout {
        spacing: Theme.dp(12)
        RowLayout {
            Layout.fillWidth: true
            Rectangle { Layout.preferredWidth: Theme.dp(4); Layout.preferredHeight: Theme.dp(24); color: Theme.accent }
            Label { text: qsTr("Cała skocznia"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            HelpDot { explanation: qsTr("Kierunek i siła dotyczą całej skoczni. Różnice na zeskoku losują nowy układ miejsc. Zmienność określa późniejsze zmiany w czasie.") }
        }
        Label { text: qsTr("Kierunek i siła"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true }
        GridLayout {
            Layout.fillWidth: true; columns: tuning.width>Theme.dp(520)?2:1
            columnSpacing: Theme.dp(18); rowSpacing: Theme.dp(10)
            WindDirectionDial { degrees: tuning.values.direction; Layout.fillWidth: true; Layout.minimumWidth: 0; onEdited: function(value){tuning.backend.setValue("direction",value)} }
            WeatherNumber { backend: tuning.backend; fieldKey: "strength"; label: qsTr("Siła wyjściowa · m/s"); maximum: 10; step: .5; decimals: 2; Layout.fillWidth: true; explanation: qsTr("Mocny wiatr może być stabilny, a słaby — zmienny.") }
        }
        Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
        Label { text: qsTr("Zmienność wiatru"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true }
        GridLayout {
            Layout.fillWidth: true; columns: tuning.width>Theme.dp(520)?2:1
            columnSpacing: Theme.dp(18); rowSpacing: Theme.dp(10)
            Repeater {
                model: [
                    {key: "variability", title: qsTr("Zmiany w czasie"), help: qsTr("Jak bardzo wiatr zmienia się w trakcie konkursu.")},
                    {key: "differences", title: qsTr("Różnice na zeskoku"), help: qsTr("Jak bardzo wiatr różni się między miejscami na skoczni.")}
                ]
                delegate: ColumnLayout {
                    id: levelControl
                    required property var modelData
                    readonly property bool custom: !!tuning.values[modelData.key+"Custom"]
                    Layout.fillWidth: true; spacing: Theme.dp(3)
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.title; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
                        HelpDot { explanation: modelData.help }
                    }
                    SChoice {
                        objectName: "weather-"+modelData.key+"-level"
                        Layout.fillWidth: true
                        model: [qsTr("Bardzo niskie"), qsTr("Niskie"), qsTr("Średnie"), qsTr("Wysokie"), qsTr("Bardzo wysokie"), qsTr("Własna wartość…")]
                        currentIndex: levelControl.custom ? 5 : Number(tuning.values[modelData.key+"Level"])
                        onActivated: function(index) {
                            if (index === 5) tuning.backend.setCustom(modelData.key)
                            else tuning.backend.setLevel(modelData.key,index)
                        }
                    }
                    WeatherNumber {
                        visible: levelControl.custom
                        Layout.fillWidth: true
                        backend: tuning.backend
                        fieldKey: modelData.key
                        label: qsTr("Dokładna wartość · 0–100")
                        minimum: 0; maximum: 100; step: 1; decimals: 0
                        explanation: qsTr("Wpisz dokładną wartość od 0 do 100. Pozostałe opcje losują wartość z wybranego przedziału.")
                    }
                }
            }
        }
    }
}
