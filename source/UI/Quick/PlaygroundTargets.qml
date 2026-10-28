import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: targets
    property var backend
    readonly property var rows: backend.config.targets || []
    readonly property var results: backend.report.targets || []
    function change(index,key,value){var copy=JSON.parse(JSON.stringify(rows));copy[index][key]=value;backend.setValue("targets",copy)}
    contentWidth: availableWidth; clip: true
    ColumnLayout {
        width: targets.availableWidth; spacing: 12
        Label { text: qsTr("Cele kalibracji"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
        Label { text: qsTr("Zapisz oczekiwany zakres różnicy wariantu względem bazy dla wybranego skoczka. Ocena korzysta z przedziału 95% po ręcznym uruchomieniu pełnej próby. Wewnątrz zakresu: spełniony; całkowicie poza nim: niespełniony; pozostałe: nierozstrzygnięty."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Repeater {
            model: targets.rows
            Pane {
                required property var modelData
                required property int index
                Layout.fillWidth: true; padding: 12
                background: Rectangle { color: Theme.surface; radius: Theme.dp(6) }
                contentItem: ColumnLayout {
                    spacing: 8
                    SField { Layout.fillWidth: true; text: modelData.name; Accessible.name: qsTr("Nazwa celu"); enabled: !targets.backend.busy; onEditingFinished: targets.change(index,"name",text) }
                    GridLayout {
                        Layout.fillWidth: true; columns: targets.width<700?1:2
                        SChoice { Layout.fillWidth: true; model: targets.backend.config.variants.slice(1).map(function(v){return v.name}); currentIndex: modelData.variant-1; enabled: !targets.backend.busy; Accessible.name: qsTr("Wariant celu"); onActivated: targets.change(index,"variant",currentIndex+1) }
                        SChoice { Layout.fillWidth: true; model: [qsTr("Różnica odległości · m"),qsTr("Różnica sumy · pkt"),qsTr("Różnica miejsca")]; readonly property var keys: ["pairedDistance","pairedPoints","pairedPlace"]; currentIndex: keys.indexOf(modelData.metric); enabled: !targets.backend.busy; Accessible.name: qsTr("Miara celu"); onActivated: targets.change(index,"metric",keys[currentIndex]) }
                        PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Minimum"); value: modelData.minimum; minimum: -10000; maximum: 10000; decimals: 2; step: .1; enabled: !targets.backend.busy; onCommitted: targets.change(index,"minimum",value) }
                        PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Maksimum"); value: modelData.maximum; minimum: -10000; maximum: 10000; decimals: 2; step: .1; enabled: !targets.backend.busy; onCommitted: targets.change(index,"maximum",value) }
                    }
                    SButton { text: qsTr("Usuń cel"); quiet: true; enabled: !targets.backend.busy; onClicked: {var copy=targets.rows.slice();copy.splice(index,1);targets.backend.setValue("targets",copy)} }
                }
            }
        }
        SButton { text: qsTr("Dodaj cel"); enabled: !targets.backend.busy && targets.backend.config.variants.length>1 && targets.rows.length<64; onClicked: {var copy=targets.rows.slice();copy.push({name:qsTr("Nowy cel"),variant:1,metric:"pairedDistance",minimum:0,maximum:5});targets.backend.setValue("targets",copy)} }
        Label { text: qsTr("Wyniki ostatniej pełnej próby"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Repeater {
            model: targets.results
            Label {
                required property var modelData
                text: modelData.name+" · "+(modelData.state==="pass"?qsTr("Spełniony"):modelData.state==="fail"?qsTr("Niespełniony"):qsTr("Nierozstrzygnięty"))+" · "+(typeof modelData.result.mean==="number"?Number(modelData.result.mean).toLocaleString(Qt.locale(Theme.localeName),'f',2):"—")
                color: modelData.state==="fail"?Theme.warning:Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap
            }
        }
    }
}
