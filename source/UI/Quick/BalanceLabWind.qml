import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: wind
    property var backend
    property var analyses: []
    property var shown: [true,true,true]
    property var names: []
    property int selectedVariant: 2
    readonly property var analysis: analyses[selectedVariant] || ({})
    property bool resolutionOpen: false
    readonly property var timeline: analysis.timeline || []
    readonly property var spatial: analysis.spatial || []
    readonly property var jumps: analysis.jumpSamples || []
    function points(rows,x,y){return rows.map(function(r){return {x:r[x],y:r[y]}})}
    function series(rows,x){return [{name:qsTr("Pod narty (+), w plecy (−)"),colorIndex:2,points:points(rows,x,"head")},{name:qsTr("Boczny"),colorIndex:1,points:points(rows,x,"cross")}]}
    function comparison(key){var out=[];for(var i=0;i<analyses.length;++i)if(shown[i])out.push({name:names[i],colorIndex:i,points:points(analyses[i].jumpSamples || [],"time",key)});return out}
    spacing: Theme.dp(14)
    SChoice { Layout.fillWidth: true; model: wind.names; currentIndex: wind.selectedVariant; Accessible.name: qsTr("Wariant pola i pomiaru wiatru"); onActivated: wind.selectedVariant=currentIndex }
    SButton { text: qsTr("Rozdzielczość i zakres analizy"); quiet: true; checkable: true; checked: wind.resolutionOpen; onClicked: wind.resolutionOpen=!wind.resolutionOpen }
    Loader {
        active: wind.resolutionOpen; Layout.fillWidth: true
        sourceComponent: Component {
            GridLayout {
                columns: width>700?2:1; columnSpacing: 10; rowSpacing: 10
                Repeater { model: ["analysis.duration","analysis.distance","analysis.timeSamples","analysis.spaceSamples","analysis.scale","analysis.jumpSamples"]; BalanceLabParameter { required property string modelData; backend: wind.backend; definition: wind.backend.definitions.filter(function(d){return d.key===modelData})[0] || ({}) } }
            }
        }
    }
    Label { text: qsTr("Jedno pole pogody: czas oznacza przebieg konkursu, odległość — miejsce nad zeskokiem. Dodatnia składowa pomaga pod narty, ujemna wieje w plecy. Boczny wiatr jest pokazany osobno."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("1. Jak zmieniają się warunki w jednym miejscu?"); xLabel: qsTr("Czas konkursu · s"); yLabel: "m/s"; series: wind.series(wind.timeline,"time") }
    Label { text: qsTr("Wybierz moment, aby zobaczyć wiatr wzdłuż całej skoczni."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Slider { Layout.fillWidth: true; from: 0; to: wind.analysis.duration || 3600; value: wind.backend.values["analysis.time"] || 0; enabled: !wind.backend.busy; Accessible.name: qsTr("Moment analizy pola wiatru"); onMoved: wind.backend.setParameter("analysis.time",value) }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("2. Gdzie na zeskoku wieje inaczej?"); xLabel: qsTr("Odległość od progu · m"); yLabel: "m/s"; series: wind.series(wind.spatial,"distance") }
    Image { Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(185); source: wind.analysis.heatmap || ""; sourceSize.width: 241; sourceSize.height: 49; smooth: false; fillMode: Image.Stretch; Accessible.name: qsTr("Mapa wiatru: czas poziomo, odległość pionowo") }
    Label { text: qsTr("Mapa: czas poziomo, odległość pionowo. Niebieski — pod narty; pomarańczowy — w plecy; jasny — blisko zera. Nasycenie dochodzi do ±%1 m/s. Dokładne wartości są na wykresach.").arg(wind.analysis.heatmapScale || 3); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("3. Co odczuwa skoczek, a co mierzy punktacja?"); xLabel: qsTr("Czas startu · s"); yLabel: "m/s"; series: [{name:qsTr("Wiatr odczuty w locie"),colorIndex:2,points:wind.points(wind.jumps,"time","experienced")},{name:qsTr("Pomiar do rekompensaty"),colorIndex:1,points:wind.points(wind.jumps,"time","measured")}] }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("4. Ile metrów dał lub zabrał wiatr?"); xLabel: qsTr("Czas startu · s"); yLabel: "m"; series: wind.comparison("windMetres") }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("5. Ile punktów zostało po rekompensacie?"); xLabel: qsTr("Czas startu · s"); yLabel: qsTr("Punkty"); series: wind.comparison("pointEffect") }
    Label { text: qsTr("Każda para używa tego samego skoczka, belki i seeda. Różnica końcowych punktów obejmuje także zmianę odległości i lądowania. Rekompensata nie musi dokładnie wyrównać fizycznego wpływu wiatru."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
