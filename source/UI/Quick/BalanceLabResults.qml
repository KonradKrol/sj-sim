import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: results
    objectName: "balanceLabResults"
    property var backend
    property var question: ({})
    property var shown: [true,true,true]
    readonly property int metric: backend.options.metric || 0
    readonly property int band: backend.options.band || 0
    property int mapVariant: 2
    property int landingKind: 3
    property int tab: 0
    readonly property var data: backend.result || ({})
    readonly property var summaries: data.summaries || []
    readonly property var analyses: data.analyses || []
    readonly property var current: analyses[2] || ({})
    readonly property var names: [qsTr("Ustawienia gry"),qsTr("Punkt odniesienia"),qsTr("Bieżące")]
    readonly property var metrics: ["distance","place","total","points","falls","wins","podiums"]
    readonly property var labels: [qsTr("Odległość · m"),qsTr("Miejsce"),qsTr("Suma w konkursie · pkt"),qsTr("Punkty za skok"),qsTr("Upadki · %"),qsTr("Wygrane · %"),qsTr("Podium · %")]
    function axisLabel(key){var list=backend.definitions;for(var i=0;i<list.length;++i)if(list[i].key===key)return list[i].label;return key || ""}
    function format(v,n){return typeof v==="number" && isFinite(v)?v.toLocaleString(Qt.locale(Theme.localeName),'f',n===undefined?2:n):"—"}
    function toggle(i){var copy=shown.slice();copy[i]=!copy[i];shown=copy}
    function value(summary,key){var v=summary?summary[key]:undefined;return typeof v==="number"?v:v && typeof v.mean==="number"?v.mean:NaN}
    function interval(s,key){var v=s?s[key]:null;if(!v || typeof v!=="object")return ({});return band===0 && v.p10!==undefined && v.p90!==undefined?({lo:v.p10,hi:v.p90}):band===1 && v.ciLow!==null && v.ciHigh!==null?({lo:v.ciLow,hi:v.ciHigh}):({})}
    function curves(){var out=[];for(var v=0;v<3;++v){if(!shown[v])continue;var points=[];(data.grid || []).forEach(function(cell){if(cell.values && cell.values[v]){var s=cell.values[v],p={x:cell.x,y:value(s,metrics[metric])},range=interval(s,metrics[metric]);if(typeof range.lo==="number"){p.lo=range.lo;p.hi=range.hi}if(isFinite(p.y))points.push(p)}});out.push({name:names[v],colorIndex:v,points:points})}return out}
    function landingSeries(){var out=[];for(var v=0;v<3;++v){if(!shown[v] || !analyses[v])continue;out.push({name:names[v],colorIndex:v,points:(analyses[v].landing || []).map(function(r){return {x:r.distance,y:100*r.probabilities[landingKind]}})})}return out}
    function empiricalSeries(){var out=[];for(var v=0;v<3;++v){if(!shown[v] || !analyses[v])continue;out.push({name:names[v],colorIndex:v,points:(analyses[v].weatherLanding || []).map(function(r){return {x:r.distance,y:r.fallPercent}})})}return out}
    function noteSeries(){var out=[];for(var v=0;v<3;++v){if(!shown[v] || !analyses[v])continue;out.push({name:names[v],colorIndex:v,points:(analyses[v].landing || []).map(function(r){return {x:r.distance,y:r.meanMark}})})}return out}
    function windSeries(rows,x){return [{name:qsTr("Pod narty (+), w plecy (−)"),colorIndex:2,points:rows.map(function(r){return {x:r[x],y:r.head}})},{name:qsTr("Boczny"),colorIndex:1,points:rows.map(function(r){return {x:r[x],y:r.cross}})}]}
    function mapped(){return (data.grid || []).map(function(c){return c.values?value(c.values[mapVariant],metrics[metric]):NaN}).filter(function(v){return isFinite(v)})}
    readonly property var mapValues: mapped()
    readonly property real mapMin: mapValues.length?Math.min.apply(null,mapValues):0
    readonly property real mapMax: mapValues.length?Math.max.apply(null,mapValues):1
    contentWidth: availableWidth; clip: true
    ColumnLayout {
        width: results.availableWidth; spacing: Theme.dp(14)
        Label { text: results.data.outdated ? qsTr("Poprzedni wynik — oczekiwanie na aktualny podgląd") : results.data.full ? (results.backend.stale ? qsTr("Ostatnia pełna próba — ustawienia zostały zmienione") : qsTr("Pełna próba")) : qsTr("Szybki podgląd — mała próba"); color: results.data.outdated || results.data.full && results.backend.stale ? Theme.warning : Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SButton { objectName: "balanceLabShowFull"; visible: !!results.backend.fullResult.report && !results.data.full; text: qsTr("Pokaż ostatnią pełną próbę"); quiet: true; enabled: !results.backend.busy; onClicked: results.backend.showFullResult() }
        Label { text: qsTr("%1 konkursów na wariant. Dodatnia zmiana miejsca oznacza pogorszenie; średnia pomija nieklasyfikowane starty.").arg(results.data.report ? results.data.report.completed : 0); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Flow {
            Layout.fillWidth: true
            spacing: Theme.dp(10)
            Repeater { model: results.names; SCheck { required property string modelData; required property int index; text: modelData; checked: results.shown[index]; onClicked: results.toggle(index) } }
        }
        GridLayout {
            Layout.fillWidth: true; columns: width>=850?4:2; columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(10)
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Średnia odległość"); value: results.format(results.value(results.summaries[2],"distance"))+" m"; detail: qsTr("Względem gry: %1 m").arg(results.format(results.value(results.summaries[2],"distance")-results.value(results.summaries[0],"distance"))) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Średnie miejsce"); value: results.format(results.value(results.summaries[2],"place")); detail: qsTr("Podium: %1 %").arg(results.format(results.value(results.summaries[2],"podiums"))) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Średnia suma"); value: results.format(results.value(results.summaries[2],"total"))+qsTr(" pkt"); detail: qsTr("Względem gry: %1 pkt").arg(results.format(results.value(results.summaries[2],"total")-results.value(results.summaries[0],"total"))) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Upadki"); value: results.format(results.value(results.summaries[2],"falls"))+" %"; detail: qsTr("DSQ: %1 · skoki: %2").arg(results.summaries[2]?results.summaries[2].dsq:0).arg(results.summaries[2]?results.summaries[2].jumps:0) }
        }
        SectionTabs { Layout.fillWidth: true; options: [qsTr("Analiza"),qsTr("Stawka"),qsTr("Odtwarzanie"),qsTr("Cele")]; currentIndex: results.tab; onSelected: results.tab=index }
        Loader {
            active: results.tab===1; visible: active; Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(500)
            sourceComponent: Component {
                ListView {
                    clip: true; model: results.data.report && results.data.report.variants ? results.data.report.variants[2].athletes : []
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate { required property var modelData; width: ListView.view.width; text: modelData.name+" · "+results.format(modelData.place.mean)+" · "+results.format(modelData.distance.mean)+" m · "+results.format(modelData.total.mean)+qsTr(" pkt"); onClicked: results.backend.setContext("focus",modelData.index) }
                }
            }
        }
        Loader { active: results.tab===2; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabReplay { backend: results.backend } } }
        Loader { active: results.tab===3; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabTargets { backend: results.backend } } }
        ColumnLayout {
            visible: results.tab===0; Layout.fillWidth: true; spacing: Theme.dp(14)
            RowLayout {
                Layout.fillWidth: true
                SChoice { Layout.fillWidth: true; model: results.labels; currentIndex: results.metric; Accessible.name: qsTr("Miara wyniku"); onActivated: results.backend.setOption("metric",currentIndex) }
                SChoice { Layout.fillWidth: true; model: [qsTr("Rozrzut: środkowe 80%"),qsTr("Przedział 95% średniej"),qsTr("Bez pasma")]; currentIndex: results.band; Accessible.name: qsTr("Rodzaj pasma"); onActivated: results.backend.setOption("band",currentIndex) }
            }
            Label { text: results.band===0 ? qsTr("Rozrzut opisuje różnice między skokami. Pasma z histogramów są przybliżone; nie są niepewnością średniej.") : qsTr("Przedział średniej opisuje niepewność pomiaru. Węższy przedział wymaga większej próby."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Loader {
                active: !results.data.matrix && (results.data.grid || []).length>0; visible: active; Layout.fillWidth: true
                sourceComponent: Component { BalanceLabPlot { Layout.fillWidth: true; caption: results.question.prompt || ""; xLabel: results.axisLabel(results.data.options?results.data.options.axisX:""); yLabel: results.labels[results.metric]; series: results.curves(); onPointSelected: function(x){if(!results.backend.busy && !results.backend.previewBusy && !(results.data.full && results.backend.stale))results.backend.selectCell(x,0)} } }
            }
            ColumnLayout {
                visible: !!results.data.matrix; Layout.fillWidth: true
                SChoice { Layout.fillWidth: true; model: results.names; currentIndex: results.mapVariant; Accessible.name: qsTr("Wariant mapy"); onActivated: results.mapVariant=currentIndex }
                Label { text: qsTr("Mapa: %1 × %2 · %3 konkursów na komórkę · zakres %4 – %5").arg(results.axisLabel(results.data.options?results.data.options.axisX:"")).arg(results.axisLabel(results.data.options?results.data.options.axisY:"")).arg(results.data.gridSamplesPerCell || 0).arg(results.format(results.mapMin)).arg(results.format(results.mapMax)); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                GridLayout {
                    Layout.fillWidth: true; columns: results.data.columns || 5; columnSpacing: 3; rowSpacing: 3
                    Repeater {
                        model: results.data.grid || []
                        Button {
                            id: cell
                            required property var modelData
                            readonly property real amount: modelData.values ? (results.value(modelData.values[results.mapVariant],results.metrics[results.metric])-results.mapMin)/Math.max(.00001,results.mapMax-results.mapMin) : 0
                            Layout.fillWidth: true; Layout.preferredHeight: Math.max(Theme.dp(40),implicitContentHeight+8)
                            text: modelData.error ? "—" : results.format(results.value(modelData.values[results.mapVariant],results.metrics[results.metric]),1)
                            enabled: !modelData.error && !results.backend.busy && !results.backend.previewBusy && !(results.data.full && results.backend.stale)
                            contentItem: Label { text: cell.text; color: cell.amount>.55?Theme.accentText:Theme.ink; font.pointSize: Theme.caption; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                            background: Rectangle { radius: 4; color: cell.amount>.55?Theme.accent:Theme.raised; border.color: cell.activeFocus?Theme.ink:Theme.divider }
                            Accessible.name: results.backend.options.axisX+" "+modelData.x+", "+results.backend.options.axisY+" "+modelData.y+", "+text
                            ToolTip.visible: hovered; ToolTip.text: modelData.error || qsTr("X: %1 · Y: %2 · wynik: %3").arg(results.format(modelData.x)).arg(results.format(modelData.y)).arg(text)
                            onClicked: results.backend.selectCell(modelData.x,modelData.y)
                        }
                    }
                }
            }
            Loader {
                active: (results.question.view==="landing") && results.analyses.length>0; visible: active; Layout.fillWidth: true
                sourceComponent: Component {
                    ColumnLayout {
                        SChoice { Layout.fillWidth: true; model: [qsTr("Telemark"),qsTr("Dwie nogi"),qsTr("Podpórka"),qsTr("Upadek")]; currentIndex: results.landingKind; Accessible.name: qsTr("Rodzaj lądowania"); onActivated: results.landingKind=currentIndex }
                        GridLayout {
                            Layout.fillWidth: true; columns: results.width>=780?2:1; columnSpacing: 12; rowSpacing: 12
                            BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("Sam mechanizm — zadana odległość"); xLabel: qsTr("Odległość · m"); yLabel: "%"; series: results.landingSeries() }
                            BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("Pełne skoki — upadki przy osiąganych odległościach"); xLabel: qsTr("Odległość · m"); yLabel: "%"; series: results.empiricalSeries() }
                        }
                        Label { text: qsTr("Lewy wykres: dokładny model przy ustalonym bocznym wietrze. Prawy: próba pełnych lotów, koszyki po 2 m; rzadkie odległości mogą mieć niewiele obserwacji. Klasa lądowania wpływa na noty, a nie ryzyko upadku."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("Średnia nota jednego sędziego"); xLabel: qsTr("Odległość · m"); yLabel: qsTr("Punkty"); series: results.noteSeries() }
                    }
                }
            }
            Loader { active: results.question.view==="wind" && results.analyses.length>0; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabWind { backend: results.backend; analyses: results.analyses; shown: results.shown; names: results.names } } }
            Loader { active: results.question.view==="generator" && !!results.data.generators; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabGenerator { data: results.data; shown: results.shown } } }
            PlaygroundHistogram { visible: !!results.data.report && !!results.data.report.variants; Layout.fillWidth: true; caption: qsTr("Rozkład odległości wybranego skoczka"); series: results.data.report && results.data.report.variants ? results.data.report.variants.filter(function(v,i){return results.shown[i]}).map(function(v){return {name:v.name,bins:v.focus.distance.histogram}}) : [] }
        }
        Label { visible: !results.data.report; text: qsTr("Wybierz dane i zmień parametr. Pierwszy podgląd pojawi się automatycznie."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
