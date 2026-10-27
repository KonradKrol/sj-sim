import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: results
    property var backend
    property bool individual: false
    property int selectedVariant: 1
    property int selectedAthlete: backend.report.config ? backend.report.config.focus : 0
    readonly property var report: backend.report
    readonly property var variants: report.variants || []
    readonly property int current: Math.max(0,Math.min(selectedVariant,variants.length-1))
    readonly property var variant: variants[current] || ({})
    readonly property var athlete: variant.athletes ? variant.athletes[Math.max(0,Math.min(selectedAthlete,variant.athletes.length-1))] : ({})
    signal variantSelected(int index)
    signal athleteSelected(int index)
    signal replayRequested(int variant,int competition)
    function paired(variant,key){return individual?variant.athletes[selectedAthlete][key] || ({}):variant[key] || ({})}
    function format(value,decimals){return typeof value==="number" && isFinite(value)?value.toLocaleString(Qt.locale(Theme.localeName),'f',decimals===undefined?1:decimals):"—"}
    function mean(metric){return metric?format(metric.mean):"—"}
    function interval(metric){return metric && typeof metric.ciLow==="number" ? qsTr("95%: %1–%2").arg(format(metric.ciLow)).arg(format(metric.ciHigh)):""}
    function series(metric,athleteMetric){var indices=current===0?[0]:[0,current];return indices.map(function(i){var source=athleteMetric?variants[i].athletes[results.selectedAthlete][metric]:variants[i][metric];return {name:variants[i].name,bins:metric==="places"?source:source.histogram || []}})}
    contentWidth: availableWidth; clip: true
    ColumnLayout {
        width: results.availableWidth; spacing: 14
        RowLayout {
            Layout.fillWidth: true
            Label { text: results.report.config ? results.report.config.name : ""; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            SChoice { objectName: "playgroundResultVariant"; Layout.preferredWidth: Math.min(260,results.width*.5); model: results.variants.map(function(v){return v.name}); currentIndex: results.current; Accessible.name: qsTr("Wariant wyników"); onActivated: results.variantSelected(currentIndex) }
        }
        Label { text: qsTr("%1 pełnych powtórzeń · %2 wątków · %3 s · belka bazowa %5%4").arg(results.report.completed || 0).arg(results.report.threadsUsed || 0).arg(results.format((results.report.elapsedMs || 0)/1000)).arg(results.report.canceled?qsTr(" · przerwano"):"").arg(results.report.commonGate || 0); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: results.report.competitionType==="team"; text: qsTr("Konkurs drużynowy: miejsca i sumy dotyczą drużyny. Rozkłady skoków dotyczą wybranego zawodnika."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: results.report.autoGateAtBound===true; text: qsTr("Automatyczna belka osiągnęła granicę zakresu. Sprawdź parametry skoczni i wybierz belkę ręcznie."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        RowLayout {
            visible: results.individual; Layout.fillWidth: true
            Label { text: qsTr("Skoczek"); color: Theme.muted; font.pointSize: Theme.caption }
            SChoice { objectName: "playgroundResultAthlete"; Layout.fillWidth: true; model: results.variants.length?results.variants[0].athletes.map(function(a){return a.name}):[]; currentIndex: results.selectedAthlete; searchable: true; Accessible.name: qsTr("Analizowany skoczek"); onActivated: results.athleteSelected(currentIndex) }
        }
        Label { visible: results.individual && results.athlete.starts===0; text: qsTr("Ten zawodnik nie wystartował, np. nie znalazł się w składzie drużyny."); color: Theme.warning; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        GridLayout {
            Layout.fillWidth: true; columns: results.width<550?2:4; columnSpacing: 8; rowSpacing: 8
            PlaygroundMetric { Layout.fillWidth: true; caption: results.individual?qsTr("Średnie miejsce"):qsTr("Średnia odległość"); value: results.individual?results.mean(results.athlete.place):results.mean(results.variant.distance)+" m"; detail: results.interval(results.individual?results.athlete.place:results.variant.distance) }
            PlaygroundMetric { Layout.fillWidth: true; caption: results.individual?qsTr("Zwycięstwa"):qsTr("Średnia nota skoku"); value: results.individual?results.format(100*(results.athlete.wins || 0)/Math.max(1,results.athlete.starts),1)+" %":results.mean(results.variant.points)+qsTr(" pkt"); detail: results.individual?qsTr("Podium: %1 %").arg(results.format(100*(results.athlete.podiums || 0)/Math.max(1,results.athlete.starts))):results.interval(results.variant.points) }
            PlaygroundMetric { Layout.fillWidth: true; caption: qsTr("Upadki"); value: results.format(100*(results.individual?results.athlete.falls:results.variant.falls)/Math.max(1,results.individual?results.athlete.jumps:results.variant.jumps),2)+" %"; detail: qsTr("Na wszystkie oddane skoki") }
            PlaygroundMetric { Layout.fillWidth: true; caption: results.individual?qsTr("Średnia suma"):qsTr("Oddane skoki"); value: results.individual?results.mean(results.athlete.total)+qsTr(" pkt"):results.format(results.variant.jumps,0); detail: results.individual && results.report.competitionType==="team" ? qsTr("DSQ drużyny: %1").arg(results.format(results.athlete.teamDsq,0)) : qsTr("DSQ: %1").arg(results.format(results.individual?results.athlete.dsq:results.variant.dsq,0)) }
        }
        Pane {
            Layout.fillWidth: true; padding: 14
            background: Surface { fillColor: Theme.surface; sweep: 22 }
            contentItem: ColumnLayout {
                spacing: 8
                Label { text: qsTr("Porównanie z wariantem bazowym"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                Label { text: qsTr("Skoczek: %1. Te same numery konkursów i strumienie losowania; różnica = wariant − baza.").arg(results.individual?results.athlete.name:results.variants.length?results.variants[0].focus.name:""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                GridLayout {
                    Layout.fillWidth: true; columns: results.width<650?2:4; columnSpacing: 8; rowSpacing: 6
                    Repeater {
                        model: results.variants
                        ColumnLayout {
                            required property var modelData
                            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                            Label { text: parent.modelData.name; color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
                            Label { text: qsTr("Miejsce: %1").arg(results.mean(results.paired(parent.modelData,"pairedPlace"))); color: results.paired(parent.modelData,"pairedPlace").mean<0?Theme.accent:Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body }
                            Label { text: results.interval(results.paired(parent.modelData,"pairedPlace")); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            Label { text: qsTr("Suma: %1 pkt").arg(results.mean(results.paired(parent.modelData,"pairedPoints"))); color: Theme.muted; font.pointSize: Theme.caption }
                            Label { text: qsTr("Odległość: %1 m").arg(results.mean(results.paired(parent.modelData,"pairedDistance"))); color: Theme.muted; font.pointSize: Theme.caption }
                        }
                    }
                }
                HelpDot { explanation: qsTr("Ujemna zmiana miejsca oznacza poprawę. Przedział 95% dotyczy średniej z niezależnych symulowanych konkursów; nie jest walidacją realizmu silnika. Zmiana awansu wpływa na liczbę skoków i sumę punktów. Wyniki bez klasyfikacji są wyłączone z porównania miejsc.") }
            }
        }
        Pane {
            Layout.fillWidth: true; padding: 14
            background: Surface { fillColor: Theme.surface; sweep: 22 }
            contentItem: ColumnLayout {
                spacing: 14
                PlaygroundHistogram { visible: results.individual; Layout.fillWidth: true; caption: qsTr("Rozkład miejsc"); places: true; unclassified: results.report.fieldSize?results.report.fieldSize+1:results.report.config?results.report.config.athletes+1:0; series: results.variants.length?results.series("places",true):[] }
                PlaygroundHistogram { Layout.fillWidth: true; caption: qsTr("Rozkład odległości · m"); series: results.variants.length?results.series("distance",results.individual):[] }
                Label { text: qsTr("Odległość ≈ P5 / mediana / P95: %1 / %2 / %3 m").arg(results.format((results.individual?results.athlete.distance:results.variant.distance || {}).p05)).arg(results.format((results.individual?results.athlete.distance:results.variant.distance || {}).p50)).arg(results.format((results.individual?results.athlete.distance:results.variant.distance || {}).p95)); color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                PlaygroundHistogram { Layout.fillWidth: true; caption: qsTr("Rozkład not skoku · pkt"); series: results.variants.length?results.series("points",results.individual):[] }
            }
        }
        Pane {
            visible: !results.individual; Layout.fillWidth: true; padding: 14
            background: Surface { fillColor: Theme.surface; sweep: 22 }
            contentItem: ColumnLayout {
                spacing: 6
                Label { text: qsTr("Wiatr i wynik"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                Label { text: qsTr("Korelacja z odległością: %1 · z notą: %2").arg(results.format(results.variant.windDistanceCorrelation,3)).arg(results.format(results.variant.windPointsCorrelation,3)); color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Zależność opisowa. Wpływ parametru oceniaj przez kontrolowane warianty, nie przez samą korelację."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Repeater { model: results.variant.insights || []; Label { required property var modelData; text: modelData.text; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } }
            }
        }
        Pane {
            visible: results.individual; Layout.fillWidth: true; padding: 14
            background: Surface { fillColor: Theme.surface; sweep: 22 }
            contentItem: ColumnLayout {
                spacing: 8
                Label { text: qsTr("Przykłady konkursów"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                Label { text: qsTr("Szczegóły skoczka wybranego przy uruchamianiu. Kliknij konkurs, aby odtworzyć pełne wyniki i wszystkie skoki."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                ListView {
                    objectName: "playgroundSamples"
                    Layout.fillWidth: true; Layout.preferredHeight: 260; clip: true
                    model: results.variant.samples || []
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate {
                        required property var modelData
                        width: ListView.view.width
                        text: results.selectedAthlete===results.report.config.focus ? qsTr("Konkurs %1 · miejsce %2 · %3 pkt · belka %4").arg(modelData.competition+1).arg(modelData.place>0?modelData.place:qsTr("N/K")).arg(results.format(modelData.points)).arg(modelData.gate) : qsTr("Konkurs %1 · odtwórz pełne wyniki · belka %2").arg(modelData.competition+1).arg(modelData.gate)
                        font.pointSize: Theme.body
                        onClicked: results.replayRequested(results.current,modelData.competition)
                    }
                }
            }
        }
    }
}
