import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: replay
    property var backend
    property int selectedVariant: 0
    signal variantSelected(int index)
    signal athleteSelected(int index)
    property int selectedAthlete: backend.report.config ? backend.report.config.focus : 0
    readonly property var result: backend.replayResult
    readonly property var variants: backend.report.variants || []
    readonly property var standings: (result.standings || []).slice().sort(function(a,b){return (a.place>0?a.place:9999)-(b.place>0?b.place:9999)})
    function format(value){return typeof value==="number"?value.toLocaleString(Qt.locale(),'f',1):"—"}
    spacing: 10
    Flow {
        Layout.fillWidth: true; spacing: 8
        SChoice { objectName: "playgroundReplayVariant"; width: Math.min(260,replay.width); model: replay.variants.map(function(v){return v.name}); currentIndex: replay.selectedVariant; Accessible.name: qsTr("Wariant do odtworzenia"); onActivated: replay.variantSelected(currentIndex) }
        NumberField { id: competition; width: 180; direct: false; label: qsTr("Numer konkursu"); fields: ({name:"playground-replay",value:replay.result.competition!==undefined?replay.result.competition+1:1,minimum:1,maximum:replay.backend.report.config?replay.backend.report.config.competitions:1,step:1,decimals:0}) }
        SButton { objectName: "playgroundReplay"; text: qsTr("Odtwórz konkurs"); enabled: !replay.backend.busy && replay.variants.length>0; primary: true; onClicked: replay.backend.replay(replay.selectedVariant,competition.currentValue-1) }
    }
    Label { text: qsTr("Odtwarza rzeczywisty przebieg z konfiguracji raportu, nawet poza zachowaną próbką. Nie uruchamia konkursu w Twoim sezonie."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { visible: !!result.standings; text: qsTr("Konkurs %1 · %2 · belka %3%4").arg((result.competition || 0)+1).arg(replay.variants[result.variant || 0]?replay.variants[result.variant || 0].name:"").arg(result.gate || 0).arg(result.team?qsTr(" · miejsca i sumy drużyn"):""); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { visible: replay.result.differentEngineBuild===true; text: qsTr("Raport pochodzi z innej kompilacji silnika. To odtworzenie używa bieżącej wersji i może różnić się od zapisanych wyników."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    GridLayout {
        visible: !!result.standings; Layout.fillWidth: true; Layout.fillHeight: true; columns: replay.width<720?1:2; columnSpacing: 12; rowSpacing: 10
        Pane {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 180; padding: 12
            background: Surface { fillColor: Theme.surface; sweep: 20 }
            contentItem: ColumnLayout {
                Label { text: qsTr("Wyniki · wybierz skoczka"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                ListView {
                    objectName: "playgroundReplayStandings"; Layout.fillWidth: true; Layout.fillHeight: true; clip: true; model: replay.standings
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate {
                        required property var modelData
                        width: ListView.view.width
                        highlighted: modelData.athlete===replay.selectedAthlete
                        contentItem: RowLayout {
                            Label { text: parent.parent.modelData.eligible===false?"—":parent.parent.modelData.teamDsq?"DSQ":parent.parent.modelData.place>0?parent.parent.modelData.place+".":qsTr("N/K"); color: Theme.ink; font.family: Theme.mono; font.bold: true; Layout.preferredWidth: 40 }
                            Label { text: parent.parent.modelData.name; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                            Label { text: replay.format(parent.parent.modelData.points); color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body }
                        }
                        onClicked: replay.athleteSelected(modelData.athlete)
                    }
                }
            }
        }
        Pane {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 220; padding: 12
            background: Surface { fillColor: Theme.surface; sweep: 20 }
            contentItem: ColumnLayout {
                Label { text: qsTr("Skoki zawodnika"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                ListView {
                    id: attempts; objectName: "playgroundReplayJumps"; Layout.fillWidth: true; Layout.fillHeight: true; clip: true; spacing: 8
                    model: (replay.result.jumps || []).filter(function(j){return j.athlete===replay.selectedAthlete})
                    ScrollBar.vertical: ScrollBar {}
                    EmptyState { anchors.fill: parent; visible: attempts.count===0; compact: true; title: qsTr("Brak oddanych skoków") }
                    delegate: Pane {
                        required property var modelData
                        width: ListView.view.width; padding: 10
                        background: Rectangle { color: Theme.raised; radius: 8 }
                        contentItem: ColumnLayout {
                            spacing: 4
                            RowLayout {
                                Layout.fillWidth: true
                                Label { text: qsTr("Seria %1").arg(parent.parent.parent.modelData.round); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
                                Label { text: parent.parent.parent.modelData.dsq?"DSQ":parent.parent.parent.modelData.fall?qsTr("Upadek"):qsTr("Ustany"); color: parent.parent.parent.modelData.fall?Theme.warning:Theme.muted; font.pointSize: Theme.caption }
                            }
                            Label { text: replay.format(parent.parent.modelData.distance)+" m · "+replay.format(parent.parent.modelData.points)+" pkt"; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
                            Label { text: qsTr("Wiatr %1 m/s · rekompensata %2 pkt · noty %3 pkt").arg(replay.format(parent.parent.modelData.wind)).arg(replay.format(parent.parent.modelData.compensation)).arg(replay.format(parent.parent.modelData.judges)); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            Label { text: qsTr("Wybicie %1 · lot %2 · losowa część %3").arg(replay.format(parent.parent.modelData.takeoff)).arg(replay.format(parent.parent.modelData.flight)).arg(replay.format(parent.parent.modelData.random)); color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        }
                    }
                }
            }
        }
    }
    EmptyState { visible: !result.standings; Layout.fillWidth: true; Layout.fillHeight: true; title: qsTr("Odtwórz dowolny konkurs"); description: qsTr("Wybierz wariant i numer. Zobaczysz pełne wyniki oraz parametry każdego skoku.") }
}
