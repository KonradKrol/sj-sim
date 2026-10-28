import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: analysis
    property var backend
    property bool wind: false
    property bool settingsOpen: false
    readonly property var data: backend.diagnostics
    readonly property var values: data.parameters || ({})
    readonly property var landing: data.landing || []
    readonly property var timeline: data.timeline || []
    readonly property var spatial: data.spatial || []
    readonly property var jumps: data.jumpSamples || []
    readonly property var sampledLanding: data.weatherLanding || []
    readonly property var summary: data.windSummary || ({})
    function points(rows,x,y){return rows.map(function(r){return {x:r[x],y:r[y]}})}
    function probability(index,observed){return landing.map(function(r){return {x:r.distance,y:100*r[observed ? "observed" : "probabilities"][index]}})}
    function series(rows,x){return [{name:qsTr("Przód (+) / tył (−)"),points:points(rows,x,"head")},{name:qsTr("Bok"),points:points(rows,x,"cross")},{name:qsTr("Siła"),points:points(rows,x,"strength")} ]}
    function lower(rows){return rows.reduce(function(v,r){return Math.min(v,r.head,r.cross)},0)}
    function upper(rows){return Math.max(1,rows.reduce(function(v,r){return Math.max(v,r.strength)},0))}
    function format(value){return typeof value==="number" ? value.toLocaleString(Qt.locale(Theme.localeName),'f',2) : "—"}
    function parameter(key){return backend.definitions.filter(function(d){return d.key===key})[0]}
    contentWidth: availableWidth; clip: true
    ColumnLayout {
        width: analysis.availableWidth; spacing: Theme.dp(14)
        RowLayout {
            Layout.fillWidth: true
            Label { text: analysis.wind ? qsTr("Analiza ciągłego pola wiatru") : qsTr("Lądowanie i noty względem odległości"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            SChoice { Layout.preferredWidth: Math.min(220,analysis.width*.4); model: analysis.backend.config.variants.map(function(v){return v.name}); currentIndex: analysis.data.variant || 0; Accessible.name: qsTr("Analizowany wariant"); onActivated: analysis.backend.setAnalysisVariant(currentIndex) }
        }
        Label { text: analysis.backend.previewBusy ? qsTr("Przeliczanie analizy…") : analysis.data.error || qsTr("Zmieniaj parametry w Eksperymencie. Analiza przelicza się w tle."); color: analysis.data.error ? Theme.warning : Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: !analysis.wind; text: qsTr("Linie pokazują dokładne prawdopodobieństwa przy ustalonym bocznym wietrze. Noty i częstości są estymacją z 128 prób na odległość. Klasa lądowania zmienia noty, nie ryzyko upadku."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SButton { visible: analysis.wind; text: qsTr("Ustawienia analizy"); quiet: true; checkable: true; checked: analysis.settingsOpen; onClicked: analysis.settingsOpen=!analysis.settingsOpen }
        Loader {
            active: !analysis.wind; Layout.fillWidth: true
            sourceComponent: ColumnLayout {
                spacing: 14
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Prawdopodobieństwo rodzaju lądowania"); xLabel: qsTr("Odległość · m"); yLabel: "%"; minimumX: analysis.landing.length?analysis.landing[0].distance:0; maximumX: (analysis.data.realHs || 134)*1.2; maximumY: 100; series: [{name:qsTr("Telemark"),points:analysis.probability(0,false)},{name:qsTr("Dwie nogi"),points:analysis.probability(1,false)},{name:qsTr("Podpórka"),points:analysis.probability(2,false)},{name:qsTr("Upadek"),points:analysis.probability(3,false)}] }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Upadki · model i próba kontrolowana"); xLabel: qsTr("Odległość · m"); yLabel: "%"; minimumX: analysis.landing.length?analysis.landing[0].distance:0; maximumX: (analysis.data.realHs || 134)*1.2; maximumY: 100; series: [{name:qsTr("Model"),points:analysis.probability(3,false)},{name:qsTr("Próba"),points:analysis.probability(3,true)}] }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Średnia nota jednego sędziego"); xLabel: qsTr("Odległość · m"); yLabel: qsTr("Punkty"); minimumX: analysis.landing.length?analysis.landing[0].distance:0; maximumX: (analysis.data.realHs || 134)*1.2; maximumY: 20; series: [{name:qsTr("Średnia ważona rodzajem lądowania"),points:analysis.points(analysis.landing,"distance","meanMark")},{name:qsTr("Próba"),points:analysis.points(analysis.landing,"distance","sampledMark")}] }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Upadki w pełnej próbie pogodowej"); xLabel: qsTr("Odległość · m"); yLabel: "%"; maximumX: analysis.data.landingLimit || 150; maximumY: 100; series: [{name:qsTr("Częstość · przedziały po 2 m"),points:analysis.points(analysis.sampledLanding,"distance","fallPercent")}] }
                Label { text: qsTr("Próba pogodowa używa pełnego lotu. Wyniki przy rzadkich odległościach mogą mieć niewiele obserwacji; liczebności przedziałów są w eksporcie analizy."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Wypłaszczenie: %1 m · granica modelu zeskoku: %2 m").arg(analysis.format(analysis.data.realHs)).arg(analysis.format(analysis.data.landingLimit)); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
        }
        Loader {
            active: analysis.wind; Layout.fillWidth: true
            sourceComponent: ColumnLayout {
                spacing: 14
                GridLayout {
                    visible: analysis.settingsOpen; Layout.fillWidth: true; columns: analysis.width<650?1:3
                    Repeater { model: ["analysis.duration","analysis.time","analysis.distance","analysis.timeSamples","analysis.spaceSamples","analysis.scale","analysis.jumpSamples"]; PlaygroundNumber { required property string modelData; readonly property var definition: analysis.parameter(modelData); Layout.fillWidth: true; caption: definition.label; value: analysis.values[modelData] || 0; minimum: definition.minimum; maximum: definition.maximum; step: definition.step; decimals: 1; onCommitted: analysis.backend.setVariantParameter(analysis.data.variant || 0,modelData,value) } }
                }
                Label { text: qsTr("Średni wiatr: %1 m/s · odchylenie: %2 · zakres: %3…%4 · zmiany znaku: %5").arg(analysis.format(analysis.summary.meanHead)).arg(analysis.format(analysis.summary.sdHead)).arg(analysis.format(analysis.summary.minHead)).arg(analysis.format(analysis.summary.maxHead)).arg(analysis.summary.reversals || 0); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Wiatr w czasie · %1 m").arg(analysis.format(analysis.data.distance)); xLabel: qsTr("Czas · s"); yLabel: "m/s"; maximumX: analysis.data.duration || 3600; minimumY: analysis.lower(analysis.timeline); maximumY: analysis.upper(analysis.timeline); series: analysis.series(analysis.timeline,"time") }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Wiatr wzdłuż zeskoku · %1 s").arg(analysis.format(analysis.data.time)); xLabel: qsTr("Odległość · m"); yLabel: "m/s"; maximumX: (analysis.values["hill.k"] || 120)*1.2; minimumY: analysis.lower(analysis.spatial); maximumY: analysis.upper(analysis.spatial); series: analysis.series(analysis.spatial,"distance") }
                Label { text: qsTr("Mapa przód / tył · czas poziomo, odległość pionowo"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Image { Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(180); source: analysis.data.heatmap || ""; sourceSize.width: analysis.values["analysis.timeSamples"] || 241; sourceSize.height: analysis.values["analysis.spaceSamples"] || 49; smooth: false; fillMode: Image.Stretch; Accessible.name: qsTr("Mapa wiatru: niebieski pod narty, pomarańczowy w plecy. Wartości liczbowe na wykresach.") }
                Label { text: qsTr("Niebieski: pod narty · pomarańczowy: w plecy · jasny: blisko zera. Mapa ma stałą skalę ±%1 m/s.").arg(analysis.format(analysis.data.heatmapScale)); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Przebieg opadów"); xLabel: qsTr("Czas · s"); yLabel: qsTr("Poziom 0–10"); maximumX: analysis.data.duration || 3600; maximumY: 10; series: [{name:qsTr("Opady"),points:analysis.points(analysis.timeline,"time","precipitation")}] }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Odczuty wiatr i pomiar do punktacji"); xLabel: qsTr("Czas startu · s"); yLabel: "m/s"; maximumX: analysis.data.duration || 3600; minimumY: -analysis.upper(analysis.timeline); maximumY: analysis.upper(analysis.timeline); series: [{name:qsTr("Odczuty wiatr"),points:analysis.points(analysis.jumps,"time","experienced")},{name:qsTr("Pomiar do punktacji"),points:analysis.points(analysis.jumps,"time","measured")}] }
                AnalysisPlot { Layout.fillWidth: true; caption: qsTr("Wpływ wiatru na odległość · te same seedy"); xLabel: qsTr("Czas startu · s"); yLabel: "m"; maximumX: analysis.data.duration || 3600; minimumY: analysis.jumps.reduce(function(v,r){return Math.min(v,r.windMetres)},0); maximumY: Math.max(1,analysis.jumps.reduce(function(v,r){return Math.max(v,r.windMetres)},0)); series: [{name:qsTr("Różnica względem skoku bez wiatru"),points:analysis.points(analysis.jumps,"time","windMetres")}] }
                Label { text: qsTr("Pomiar do punktacji i odczuty wiatr są zapisane osobno przy skokach w widoku Konkurs oraz w eksporcie. Podgląd pola nie jest oficjalnym pomiarem FIS."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
        }
    }
}
