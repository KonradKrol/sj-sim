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
        GridLayout {
            Layout.fillWidth: true; columns: tuning.viewportWidth>Theme.fontSize(600)?2:1
            columnSpacing: Theme.dp(18); rowSpacing: Theme.dp(10)
            WindDirectionDial { degrees: tuning.values.direction; Layout.fillWidth: true; Layout.minimumWidth: 0; onEdited: function(value){tuning.backend.setValue("direction",value)} }
            WeatherNumber { backend: tuning.backend; fieldKey: "strength"; label: qsTr("Siła wyjściowa · m/s"); maximum: 10; step: .5; quantum: .5; Layout.fillWidth: true; explanation: qsTr("Mocny wiatr może być stabilny, a słaby — zmienny.") }
            Repeater {
                model: [
                    {key: "variability", title: qsTr("Zmiany w czasie"), help: qsTr("Jak bardzo wiatr zmienia się w trakcie konkursu.")},
                    {key: "differences", title: qsTr("Różnice na zeskoku"), help: qsTr("Jak bardzo wiatr różni się między miejscami na skoczni.")}
                ]
                delegate: ColumnLayout {
                    required property var modelData
                    Layout.fillWidth: true; spacing: Theme.dp(3)
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: modelData.title; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
                        HelpDot { explanation: modelData.help }
                    }
                    SChoice {
                        objectName: "weather-"+modelData.key+"-level"
                        Layout.fillWidth: true
                        model: [qsTr("Bardzo niskie"), qsTr("Niskie"), qsTr("Średnie"), qsTr("Wysokie"), qsTr("Bardzo wysokie")]
                        currentIndex: Number(tuning.values[modelData.key+"Level"])
                        onActivated: function(index) { tuning.backend.setLevel(modelData.key,index) }
                    }
                }
            }
        }
    }
}
