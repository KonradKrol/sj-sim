import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: generator
    property var data: ({})
    property var shown: [true,true,true]
    readonly property var names: [qsTr("Automatyczna propozycja"),qsTr("Stały wiatr w plecy"),qsTr("Silny wiatr pod narty"),qsTr("Loteryjny"),qsTr("Zmienny boczny"),qsTr("Cisza"),qsTr("Stały pod narty"),qsTr("Silny w plecy"),qsTr("Stały boczny"),qsTr("Zmienny pod narty"),qsTr("Zmienny w plecy"),qsTr("Różne kierunki na zeskoku")]
    readonly property var seriesNames: [qsTr("Ustawienia gry"),qsTr("Punkt odniesienia"),qsTr("Bieżące")]
    function format(v){return Number(v).toLocaleString(Qt.locale(Theme.localeName),'f',2)}
    function series(){var out=[];for(var v=0;v<3;++v){if(!shown[v])continue;out.push({name:seriesNames[v],colorIndex:v,points:(data.generators[v] || []).map(function(r,i){return {x:i,y:r.mean,lo:r.min,hi:r.max}})})}return out}
    spacing: Theme.dp(12)
    Label { text: qsTr("Każdy preset losujemy wielokrotnie. Wykres pokazuje średnią siłę referencyjnego wiatru oraz minimum i maksimum w tej próbie — zanim zacznie działać zmienność pola. To analiza generatora, nie odległości skoków."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("Jak silne warunki losują presety?"); xLabel: qsTr("Numer scenariusza — nazwy poniżej"); yLabel: "m/s"; series: generator.series() }
    Repeater {
        model: generator.data.generators ? generator.data.generators[2] : []
        Pane {
            required property var modelData
            required property int index
            Layout.fillWidth: true; padding: Theme.dp(10)
            background: Rectangle { color: Theme.surface; radius: Theme.dp(6) }
            contentItem: ColumnLayout {
                Label { text: index+" · "+generator.names[index]; color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Średnia %1 m/s · zakres %2–%3 · pod narty %4% · w plecy %5% · prób %6").arg(generator.format(modelData.mean)).arg(generator.format(modelData.min)).arg(generator.format(modelData.max)).arg(generator.format(100*modelData.headShare)).arg(generator.format(100*modelData.tailShare)).arg(modelData.count); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
        }
    }
}
