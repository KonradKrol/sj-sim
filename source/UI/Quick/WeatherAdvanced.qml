import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: advanced
    property var backend
    spacing: Theme.dp(10)
    Label { text: qsTr("Zmiany między skokami"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(680)?3:width>Theme.dp(380)?2:1; columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(8)
        WeatherNumber { backend: advanced.backend; fieldKey: "trendAmplitude"; label: qsTr("Zmiana tendencji · m/s"); maximum: 10; Layout.fillWidth: true; explanation: qsTr("O ile ogólny wiatr może odchodzić od punktu wyjścia przez kilka kolejnych skoków.") }
        WeatherNumber { backend: advanced.backend; fieldKey: "trendPeriodSeconds"; factor: 1/30; label: qsTr("Jedna tendencja · ok. skoków"); minimum: 1; maximum: 200; step: 1; Layout.fillWidth: true; explanation: qsTr("Orientacyjny czas trwania przy odstępie 30 s między skokami. Poszczególne okresy są różnej długości.") }
        WeatherNumber { backend: advanced.backend; fieldKey: "trendTransitionSeconds"; label: qsTr("Przejście między tendencjami · s"); minimum: .1; maximum: Math.max(.1,advanced.backend.values.trendPeriodSeconds*.3); Layout.fillWidth: true }
        WeatherNumber { backend: advanced.backend; fieldKey: "temporalAmplitude"; label: qsTr("Bieżące wahania · m/s"); maximum: 10; Layout.fillWidth: true }
        WeatherNumber { backend: advanced.backend; fieldKey: "correlationSeconds"; label: qsTr("Tempo bieżących wahań · s"); minimum: .1; maximum: 600; Layout.fillWidth: true; explanation: qsTr("Krótszy czas oznacza szybsze zmiany wiatru.") }
        WeatherNumber { backend: advanced.backend; fieldKey: "activityVariation"; factor: 100; label: qsTr("Okresy większej niestabilności · %"); maximum: 100; step: 1; Layout.fillWidth: true; explanation: qsTr("Jak bardzo spokojniejsze okresy mogą przeplatać się z okresami szybkich zmian.") }
    }
    Label { text: qsTr("Podmuchy i różnice lokalne"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(680)?3:width>Theme.dp(380)?2:1; columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(8)
        WeatherNumber { backend: advanced.backend; fieldKey: "gustAmplitude"; label: qsTr("Siła podmuchów · m/s"); maximum: 15; Layout.fillWidth: true }
        WeatherNumber { backend: advanced.backend; fieldKey: "gustProbability"; factor: 100; label: qsTr("Częstość podmuchów · %"); maximum: 100; step: 1; Layout.fillWidth: true; explanation: qsTr("Szansa pojawienia się podmuchu w jednym przedziale czasu. Nie oznacza szansy na lepszy skok.") }
        WeatherNumber { backend: advanced.backend; fieldKey: "gustPeriodSeconds"; label: qsTr("Odstęp prób podmuchu · s"); minimum: .1; maximum: 600; Layout.fillWidth: true }
        WeatherNumber { backend: advanced.backend; fieldKey: "gustWidthMetres"; label: qsTr("Zasięg podmuchu · m"); minimum: .1; maximum: 600; Layout.fillWidth: true }
        WeatherNumber { backend: advanced.backend; fieldKey: "spatialAmplitude"; label: qsTr("Lokalne wahania · m/s"); maximum: 10; Layout.fillWidth: true }
        WeatherNumber { backend: advanced.backend; fieldKey: "correlationMetres"; label: qsTr("Podobieństwo sąsiednich miejsc · m"); minimum: .1; maximum: 1000; Layout.fillWidth: true; explanation: qsTr("Na jak długim fragmencie lokalne zmiany wiatru są do siebie podobne.") }
    }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Punkty odniesienia"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true }
        HelpDot { explanation: qsTr("Wiatr płynnie zmienia się między punktami. Kierunek: 0° pod narty, 180° w plecy, 90° i 270° boczny. Podmuchy i zmiany w czasie dochodzą do tych wartości.") }
    }
    Repeater {
        model: advanced.backend.references.length
        GridLayout {
            id: reference
            required property int index
            readonly property var entry: advanced.backend.references[index]
            Layout.fillWidth: true; columns: 3; columnSpacing: Theme.dp(8)
            Repeater {
                model: [{key:"distance",label:qsTr("Odległość · m"),maximum:advanced.backend.values.k*1.5},{key:"strength",label:qsTr("Siła · m/s"),maximum:15},{key:"direction",label:qsTr("Kierunek · °"),maximum:360}]
                ColumnLayout {
                    required property var modelData
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    Label { visible: reference.index===0; text: parent.modelData.label; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    NumberField {
                        Layout.fillWidth: true; direct: false; label: parent.modelData.label
                        readonly property string fieldKey: parent.modelData.key
                        fields: ({name:"weather-reference-"+reference.index+"-"+fieldKey,value:reference.entry[fieldKey],minimum:0,maximum:parent.modelData.maximum,step:.1,decimals:1})
                        onCommitted: function(value){advanced.backend.setReference(reference.index,fieldKey,value)}
                    }
                }
            }
        }
    }
}
