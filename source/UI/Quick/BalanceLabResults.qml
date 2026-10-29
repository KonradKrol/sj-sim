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
    property bool showPreview: false
    readonly property bool hasFull: !!backend.fullResult.report
    readonly property var preview: backend.previewResult || ({})
    readonly property var data: showPreview && preview.report ? preview : backend.result || ({})
    readonly property var savedConfig: data.config || ({})
    readonly property string savedView: data.options ? data.options.view : question.view
    readonly property string focusName: data.report && data.report.variants ? data.report.variants[2].focus.name : ""
    property string lastFullRevision: ""
    Connections {
        target: results.backend
        function onResultChanged() {
            var revision=String(results.backend.fullResult.sourceRevision || "")
            if(revision!==results.lastFullRevision){results.showPreview=false;results.lastFullRevision=revision}
        }
    }
    readonly property var summaries: data.summaries || []
    readonly property var analyses: data.analyses || []
    readonly property var current: analyses[2] || ({})
    readonly property var names: [qsTr("Balans z gry"),qsTr("Zapamiętane ustawienia"),qsTr("Twoje zmiany")]
    readonly property var metrics: ["distance","place","total","points","falls","wins","podiums"]
    readonly property var labels: [qsTr("Odległość · m"),qsTr("Miejsce w konkursie — mniej znaczy lepiej"),qsTr("Suma punktów w konkursie"),qsTr("Punkty za skok"),qsTr("Upadki · %"),qsTr("Wygrane konkursy · %"),qsTr("Podium · %")]
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
    function comparisonText() {
        var metres=value(summaries[2],"distance")-value(summaries[0],"distance")
        var places=value(summaries[2],"place")-value(summaries[0],"place")
        if(!isFinite(metres) || !isFinite(places))return qsTr("Brak wystarczających wyników klasyfikowanych startów do porównania średnich.")
        return qsTr("W tej próbie „Twoje zmiany” względem „Balansu z gry”: różnica średniej odległości %1 m; różnica średniego miejsca %2. Minus przy miejscu oznacza poprawę, np. −3 to średnio o trzy pozycje wyżej. To opis próby, nie dowód istotności różnicy.").arg(format(metres)).arg(format(places))
    }
    function mapEntries() {
        var cells=data.grid || [],columns=data.columns || 3,rows=data.rows || 0,out=[{header:true,label:qsTr("Y / X")}]
        for(var x=0;x<columns;++x)out.push({header:true,label:cells[x]?format(cells[x].x,1):""})
        for(var y=rows-1;y>=0;--y){out.push({header:true,label:cells[y*columns]?format(cells[y*columns].y,1):""});for(x=0;x<columns;++x)out.push({header:false,cell:cells[y*columns+x]})}
        return out
    }
    function cellDescription(cell) {
        if(cell.error)return cell.error
        var summary=cell.values ? cell.values[mapVariant] : null,range=interval(summary,metrics[metric])
        var text=qsTr("%1: %2; %3: %4. %5: %6.").arg(axisLabel(data.options ? data.options.axisX : "")).arg(format(cell.x)).arg(axisLabel(data.options ? data.options.axisY : "")).arg(format(cell.y)).arg(labels[metric]).arg(format(value(summary,metrics[metric])))
        if(typeof range.lo==="number")text+=qsTr(" Wybrane pasmo: %1–%2.").arg(format(range.lo)).arg(format(range.hi))
        return text
    }
    contentWidth: availableWidth; clip: true
    ColumnLayout {
        width: results.availableWidth; spacing: Theme.dp(14)
        Label {
            text: results.data.full ? qsTr("Wynik pełnej próby — zachowany") : qsTr("Szybki podgląd — orientacyjny wynik")
            color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
        Label {
            visible: !!results.data.report
            text: qsTr("Analizowany skoczek: %1. Konkursów w każdym z trzech wariantów: %2. Skocznia: %3.").arg(results.focusName).arg(results.data.report ? results.data.report.completed : 0).arg(results.savedConfig.hill ? results.savedConfig.hill.name : "")
            color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
        Label {
            visible: results.data.full && results.backend.stale || !!results.data.outdated
            text: results.data.full ? qsTr("To wynik wcześniejszych ustawień. Jest bezpiecznie zachowany. Nowe ustawienia sprawdzisz w podglądzie poniżej albo przyciskiem „Uruchom pełną próbę”.") : qsTr("Ustawienia się zmieniły. Te liczby pochodzą z poprzedniego podglądu; trwa aktualizacja.")
            color: Theme.warning; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
        Pane {
            visible: results.hasFull; Layout.fillWidth: true; padding: Theme.dp(12)
            background: Rectangle { color: Theme.raised; radius: Theme.dp(8) }
            contentItem: ColumnLayout {
                Label { text: qsTr("Szybki podgląd nowych ustawień"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label {
                    text: results.backend.previewBusy ? qsTr("Aktualizowanie podglądu… pełna próba pozostaje na ekranie.") : results.preview.report ? qsTr("Tylko %1 konkursów na wariant. Twoje zmiany: średnio %2 m i miejsce %3. To wstępna wskazówka, nie dokładny pomiar.").arg(results.preview.report.completed).arg(results.format(results.value(results.preview.summaries[2],"distance"))).arg(results.format(results.value(results.preview.summaries[2],"place"))) : qsTr("Zmień parametr, a tutaj pojawi się osobny podgląd. Wynik pełnej próby nie zostanie zastąpiony.")
                    color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap
                }
                SButton {
                    objectName: "balanceLabShowFull"
                    text: results.showPreview ? qsTr("Wróć do zachowanej pełnej próby") : qsTr("Obejrzyj wykresy szybkiego podglądu")
                    enabled: results.showPreview || !!results.preview.report && !results.preview.outdated
                    quiet: true; onClicked: results.showPreview=!results.showPreview
                }
            }
        }
        Label { text: qsTr("Trzy warianty tego samego eksperymentu"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        GridLayout {
            Layout.fillWidth: true; columns: width>760?3:1; columnSpacing: Theme.dp(14); rowSpacing: Theme.dp(5)
            Repeater {
                model: results.names
                ColumnLayout {
                    required property string modelData
                    required property int index
                    Layout.fillWidth: true; Layout.alignment: Qt.AlignTop; spacing: 0
                    SCheck { text: modelData; checked: results.shown[index]; onClicked: results.toggle(index) }
                    Label {
                        text: index===0 ? qsTr("Punkt startowy: balans gry i dane wybranej próby.") : index===1 ? qsTr("Kopia ustawień z przycisku „Zapamiętaj do porównania”. Możesz porównać poprzedni pomysł z nowym.") : qsTr("Efekt parametrów, które edytujesz w laboratorium. Karty poniżej pokazują tę serię z wyświetlonej próby.")
                        color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap
                    }
                }
            }
        }
        Label {
            visible: results.summaries.length===3
            text: results.comparisonText()
            color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
        GridLayout {
            Layout.fillWidth: true; columns: width>=850?4:2; columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(10)
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Średnia odległość"); value: results.format(results.value(results.summaries[2],"distance"))+" m"; detail: qsTr("Względem gry: %1 m").arg(results.format(results.value(results.summaries[2],"distance")-results.value(results.summaries[0],"distance"))) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Średnie miejsce"); value: results.format(results.value(results.summaries[2],"place")); detail: qsTr("Podium: %1 %").arg(results.format(results.value(results.summaries[2],"podiums"))) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Średnia suma punktów w konkursie"); value: results.format(results.value(results.summaries[2],"total"))+qsTr(" pkt"); detail: qsTr("Względem gry: %1 pkt").arg(results.format(results.value(results.summaries[2],"total")-results.value(results.summaries[0],"total"))) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Upadki"); value: results.format(results.value(results.summaries[2],"falls"))+" %"; detail: qsTr("Dyskwalifikacje: %1 · ocenione skoki: %2").arg(results.summaries[2]?results.summaries[2].dsq:0).arg(results.summaries[2]?results.summaries[2].jumps:0) }
        }
        SectionTabs { Layout.fillWidth: true; options: [qsTr("Analiza"),qsTr("Wyniki wszystkich skoczków"),qsTr("Odtwarzanie"),qsTr("Cele strojenia")]; currentIndex: results.tab; onSelected: results.tab=index }
        Loader {
            active: results.tab===1; visible: active; Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(500)
            sourceComponent: Component {
                ListView {
                    clip: true; model: results.data.report && results.data.report.variants ? results.data.report.variants[2].athletes : []
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate { required property var modelData; width: ListView.view.width; text: qsTr("%1 · średnie miejsce: %2 · odległość: %3 m · suma: %4 pkt").arg(modelData.name).arg(results.format(modelData.place.mean)).arg(results.format(modelData.distance.mean)).arg(results.format(modelData.total.mean)); onClicked: {results.backend.setContext("focus",modelData.index);results.tab=0} }
                }
            }
        }
        Loader { active: results.tab===2; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabReplay { backend: results.backend } } }
        Loader { active: results.tab===3; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabTargets { backend: results.backend } } }
        ColumnLayout {
            visible: results.tab===0; Layout.fillWidth: true; spacing: Theme.dp(14)
            Label { text: qsTr("Co pokazuje wykres lub mapa?"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            GridLayout {
                Layout.fillWidth: true; columns: width>680?2:1
                SChoice { Layout.fillWidth: true; model: results.labels; currentIndex: results.metric; Accessible.name: qsTr("Miara wyniku"); onActivated: results.backend.setOption("metric",currentIndex) }
                SChoice { Layout.fillWidth: true; model: [qsTr("Typowe wyniki — środkowe 80%"),qsTr("Dokładność średniej — przedział ufności 95%"),qsTr("Pokaż tylko średnią")]; currentIndex: results.band; Accessible.name: qsTr("Rodzaj pasma"); onActivated: results.backend.setOption("band",currentIndex) }
            }
            Label { text: results.band===0 ? qsTr("Pasmo obejmuje środkowe 80% wyników: odrzuca po 10% skrajnych z każdej strony. Dla miejsc chodzi o konkursy, dla odległości o skoki. Szersze pasmo oznacza większy rozrzut; odczyt z histogramu jest przybliżony.") : results.band===1 ? qsTr("Przedział ufności 95% pokazuje dokładność oszacowanej średniej, nie zakres miejsc ani odległości pojedynczych startów. Więcej konkursów zwykle go zawęża.") : qsTr("Pokazujesz same średnie. Wybierz pasmo powyżej, aby zobaczyć rozrzut wyników lub dokładność średniej."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Loader {
                active: !results.data.matrix && (results.data.grid || []).length>0; visible: active; Layout.fillWidth: true
                sourceComponent: Component { BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("Jak wynik zmienia się wraz z parametrem?"); xLabel: results.axisLabel(results.data.options?results.data.options.axisX:""); yLabel: results.labels[results.metric]; series: results.curves(); onPointSelected: function(x){if(!results.backend.busy && !results.backend.previewBusy && !(results.data.full && results.backend.stale))results.backend.selectCell(x,0)} } }
            }
            ColumnLayout {
                visible: !!results.data.matrix; Layout.fillWidth: true
                Label { text: qsTr("Który wariant chcesz obejrzeć na mapie?"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SChoice { Layout.fillWidth: true; model: results.names; currentIndex: results.mapVariant; Accessible.name: qsTr("Wariant mapy"); onActivated: results.mapVariant=currentIndex }
                Label { text: qsTr("Mapa: %1 × %2 · %3 konkursów na komórkę · zakres %4 – %5").arg(results.axisLabel(results.data.options?results.data.options.axisX:"")).arg(results.axisLabel(results.data.options?results.data.options.axisY:"")).arg(results.data.gridSamplesPerCell || 0).arg(results.format(results.mapMin)).arg(results.format(results.mapMax)); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Kolumny: %1. Wiersze: %2. Liczba w komórce oznacza: %3. Kolor pokazuje wielkość wartości, nie ocenę „dobrze” lub „źle”.").arg(results.axisLabel(results.data.options ? results.data.options.axisX : "")).arg(results.axisLabel(results.data.options ? results.data.options.axisY : "")).arg(results.labels[results.metric]); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                ScrollView {
                    id: mapScroll
                    Layout.fillWidth: true; Layout.preferredHeight: mapGrid.implicitHeight+Theme.dp(20)
                    contentWidth: Math.max(availableWidth,((results.data.columns || 3)+1)*Theme.dp(58))
                    contentHeight: mapGrid.implicitHeight; clip: true
                    GridLayout {
                        id: mapGrid
                        width: mapScroll.contentWidth
                        columns: (results.data.columns || 3)+1; columnSpacing: 3; rowSpacing: 3
                        Repeater {
                            model: results.mapEntries()
                            Button {
                                id: cell
                                required property var modelData
                                readonly property var entry: modelData.cell || ({})
                                readonly property real amount: entry.values ? (results.value(entry.values[results.mapVariant],results.metrics[results.metric])-results.mapMin)/Math.max(.00001,results.mapMax-results.mapMin) : 0
                                Layout.fillWidth: true; Layout.minimumWidth: Theme.dp(48); Layout.preferredHeight: Math.max(Theme.dp(40),implicitContentHeight+8)
                                text: modelData.header ? modelData.label : entry.error ? "—" : results.format(results.value(entry.values[results.mapVariant],results.metrics[results.metric]),1)
                                flat: modelData.header; hoverEnabled: !modelData.header
                                activeFocusOnTab: !modelData.header
                                contentItem: Label { text: cell.text; color: Theme.ink; font.bold: cell.modelData.header; font.pointSize: Theme.caption; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                background: Rectangle { radius: 4; color: cell.modelData.header ? Theme.raised : Qt.tint(Theme.surface,Qt.rgba(Theme.accent.r,Theme.accent.g,Theme.accent.b,.06+.28*cell.amount)); border.color: cell.activeFocus?Theme.accent:Theme.divider }
                                Accessible.name: modelData.header ? text : results.cellDescription(entry)
                                ToolTip.visible: hovered && !modelData.header; ToolTip.text: results.cellDescription(entry)
                                onClicked: if(!modelData.header && !entry.error && !results.backend.busy && !results.backend.previewBusy && !(results.data.full && results.backend.stale))results.backend.selectCell(entry.x,entry.y)
                            }
                        }
                    }
                }
                Label { visible: !!results.data.full && results.backend.stale; text: qsTr("To zachowana mapa. Możesz odczytywać wyniki; aby przenieść komórkę do aktualnego eksperymentu, najpierw przelicz mapę dla nowych ustawień."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            Loader {
                active: (results.savedView==="landing") && results.analyses.length>0; visible: active; Layout.fillWidth: true
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
            Loader { active: results.savedView==="wind" && results.analyses.length>0; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabWind { backend: results.backend; analyses: results.analyses; shown: results.shown; names: results.names } } }
            Loader { active: results.savedView==="generator" && !!results.data.generators; visible: active; Layout.fillWidth: true; sourceComponent: Component { BalanceLabGenerator { data: results.data; shown: results.shown } } }
            PlaygroundHistogram { visible: !!results.data.report && !!results.data.report.variants; Layout.fillWidth: true; caption: qsTr("Rozkład odległości wybranego skoczka"); series: results.data.report && results.data.report.variants ? results.data.report.variants.map(function(v,i){return {name:results.names[i],bins:v.focus.distance.histogram,shown:results.shown[i]}}).filter(function(v){return v.shown}) : [] }
        }
        Label { visible: !results.data.report; text: qsTr("Wybierz dane i zmień parametr. Pierwszy podgląd pojawi się automatycznie."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
