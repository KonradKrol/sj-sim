import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: replay
    property var backend
    property int variant: 2
    property int selected: 0
    property int step: 0
    property bool playing: false
    readonly property var jumps: backend.replay.jumps || []
    readonly property var jump: jumps[selected] || ({})
    readonly property var sections: jump.detail ? (jump.detail.exposures || []).filter(function(s){return s.traversed>0}) : []
    readonly property var section: sections[Math.min(step,sections.length-1)] || ({})
    function format(v){return typeof v==="number"?v.toLocaleString(Qt.locale(Theme.localeName),'f',2):"—"}
    spacing: Theme.dp(10)
    Label { text: qsTr("Odtwórz konkurs i obejrzyj wiatr faktycznie napotkany przez wybranego skoczka. Kroki odpowiadają zapisanym odcinkom ekspozycji, a nie animacji fizycznego toru lotu."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SChoice { Layout.fillWidth: true; model: [qsTr("Ustawienia gry"),qsTr("Punkt odniesienia"),qsTr("Bieżące")]; currentIndex: replay.variant; Accessible.name: qsTr("Wariant odtworzenia"); onActivated: replay.variant=currentIndex }
    PlaygroundNumber { id: contest; Layout.fillWidth: true; caption: qsTr("Numer konkursu"); value: 1; minimum: 1; maximum: replay.backend.fullResult.config ? replay.backend.fullResult.config.competitions : 1; step: 1 }
    SButton { objectName: "balanceLabReplay"; text: qsTr("Odtwórz konkurs"); enabled: !replay.backend.busy && !!replay.backend.fullResult.report; onClicked: replay.backend.replayContest(contest.currentValue-1,replay.variant) }
    Label { visible: !!replay.backend.replay.differentEngineBuild; text: qsTr("Raport pochodzi z innej kompilacji silnika. Odtworzenie używa bieżącej wersji i może różnić się od zapisanych wyników."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SChoice { Layout.fillWidth: true; model: replay.jumps.map(function(j){var standing=(replay.backend.replay.standings || []).filter(function(s){return s.athlete===j.athlete})[0];return (standing?standing.name:String(j.athlete+1))+" · "+qsTr("Seria %1").arg(j.round)+" · "+j.distance+" m"}); currentIndex: replay.selected; searchable: true; Accessible.name: qsTr("Skok do analizy"); onActivated: {replay.selected=currentIndex;replay.step=0} }
    RowLayout { SButton { text: replay.playing ? qsTr("Pauza") : qsTr("Odtwarzaj odcinki"); enabled: replay.sections.length>0; onClicked: {if(replay.step>=replay.sections.length-1)replay.step=0;replay.playing=!replay.playing} } Label { text: qsTr("Odcinek %1 / %2").arg(replay.sections.length?replay.step+1:0).arg(replay.sections.length); color: Theme.muted; font.pointSize: Theme.caption } }
    Slider { Layout.fillWidth: true; from: 0; to: Math.max(1,replay.sections.length-1); stepSize: 1; value: replay.step; enabled: replay.sections.length>0; Accessible.name: qsTr("Odcinek lotu"); onMoved: {replay.playing=false;replay.step=Math.round(value)} }
    Timer { interval: 700; repeat: true; running: replay.playing && replay.visible; onTriggered: {if(replay.step>=replay.sections.length-1)replay.playing=false;else replay.step++} }
    Label { text: qsTr("Od %1 do %2 m · wiatr wzdłużny %3 m/s · boczny %4 m/s").arg(replay.format(replay.section.begin)).arg(replay.format((replay.section.begin || 0)+(replay.section.traversed || 0))).arg(replay.format(replay.section.head)).arg(replay.format(replay.section.cross)); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    BalanceLabPlot { Layout.fillWidth: true; caption: qsTr("Wiatr na rzeczywiście przebytej trasie"); xLabel: qsTr("Odległość · m"); yLabel: "m/s"; forcedPoint: replay.step; series: [{name:qsTr("Wzdłużny"),colorIndex:2,points:replay.sections.map(function(s){return {x:s.begin+s.traversed/2,y:s.head,lo:s.minimumHead,hi:s.maximumHead}})},{name:qsTr("Boczny"),colorIndex:1,points:replay.sections.map(function(s){return {x:s.begin+s.traversed/2,y:s.cross}})}]; onPointSelected: function(x){for(var i=0;i<replay.sections.length;++i){var s=replay.sections[i];if(x<=s.begin+s.traversed){replay.step=i;break}}} }
    Label { text: qsTr("Skok: %1 m · %2 pkt · pomiar %3 m/s · odczuty wiatr %4 m/s · rekompensata %5 pkt").arg(replay.format(replay.jump.distance)).arg(replay.format(replay.jump.points)).arg(replay.format(replay.jump.wind)).arg(replay.format(replay.jump.experiencedWind)).arg(replay.format(replay.jump.compensation)); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: qsTr("Belka jury: %1 · faktyczna: %2 · odniesienie: %3 · rekompensata belki: %4 pkt").arg(replay.jump.detail?replay.jump.detail.juryGate:0).arg(replay.jump.detail?replay.jump.detail.actualGate:0).arg(replay.jump.detail?replay.jump.detail.referenceGate:0).arg(replay.format(replay.jump.detail?replay.jump.detail.gateCompensation:0)); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
