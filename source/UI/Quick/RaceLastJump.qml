import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: jump
    objectName: "raceLastJump"
    property bool dense: false
    readonly property var summary: ui.jumpSummary
    readonly property bool narrow: width<480
    padding: dense ? 4 : 10
    function signedColor(value){return value>0 ? Theme.success : value<0 ? Theme.danger : Theme.ink}
    function signedText(text,value){var shown=text || "—";if(value===0 && parseFloat(shown)===0)shown=shown.replace(/^[-−]/,"");return (value>0 ? "+" : "")+shown}
    background: Rectangle {
        color: Theme.surface; border.color: Theme.outline
        Rectangle { width: 24; height: 3; color: Theme.signal }
    }
    contentItem: ColumnLayout {
        spacing: 3
        RowLayout {
            Layout.fillWidth: true; spacing: 8
            Label { text: ui.context.liveStage ? ["",qsTr("Lot"),qsTr("Odległość"),qsTr("Lądowanie"),qsTr("Noty"),qsTr("Wynik")][ui.context.liveStage] : qsTr("Ostatni skok"); color: Theme.muted; font.pointSize: Theme.caption }
            Label { text: jump.summary.name || "—"; color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
            Label { text: jump.summary.position || ""; color: Theme.muted; font.pointSize: Theme.caption }
        }
        GridLayout {
            Layout.fillWidth: true
            columns: jump.narrow ? 1 : 2; columnSpacing: 10; rowSpacing: 2
            RowLayout {
                Layout.fillWidth: true; spacing: 10
                Label { objectName: "lastJumpDistance"; text: jump.summary.distance || "—"; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold }
                Label { objectName: "lastJumpPoints"; text: jump.summary.points || "—"; color: Theme.accent; font.family: Theme.mono; font.pointSize: Theme.body; Layout.fillWidth: true }
            }
            RowLayout {
                Layout.fillWidth: true; spacing: 6
                JudgeMarks { objectName: "lastJumpJudges"; judges: jump.summary.judges || [] }
                Label { text: jump.summary.judgesPoints || "—"; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.fillWidth: true; horizontalAlignment: Text.AlignRight }
            }
        }
        GridLayout {
            Layout.fillWidth: true
            columns: jump.narrow ? 3 : 4; columnSpacing: 8; rowSpacing: 2
            JumpMetric { caption: qsTr("Wiatr"); value: jump.signedText(jump.summary.wind,jump.summary.windValue); valueColor: jump.signedColor(jump.summary.windValue); objectName: "lastJumpWind"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Belka"); value: jump.summary.gate || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Ląd."); value: jump.summary.landing || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Rekomp."); value: jump.signedText(jump.summary.compensation,jump.summary.compensationValue); valueColor: jump.signedColor(jump.summary.compensationValue); objectName: "lastJumpCompensation"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Za wiatr"); value: jump.summary.windPoints || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Za belkę"); value: jump.summary.gatePoints || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Seria"); value: jump.summary.roundPosition || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { visible: !!jump.summary.groupPosition; caption: qsTr("Grupa"); value: jump.summary.groupPosition || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { visible: !!jump.summary.competitionPosition; caption: qsTr("Konkurs"); value: jump.summary.competitionPosition || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { caption: qsTr("Suma"); value: jump.summary.totalPoints || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
        }
        Flow {
            Layout.fillWidth: true
            spacing: 8
            JumpMetric { inlineValue: true; visible: !!jump.summary.takeoff; caption: qsTr("Wybicie"); value: jump.summary.takeoff || "" }
            JumpMetric { inlineValue: true; visible: !!jump.summary.flight; caption: qsTr("Lot"); value: jump.summary.flight || "" }
            JumpMetric { inlineValue: true; visible: !!jump.summary.random; caption: qsTr("Los."); value: jump.summary.random || "" }
            JumpMetric { inlineValue: true; visible: !!jump.summary.snow; caption: qsTr("Śnieg"); value: jump.summary.snow || "" }
        }
        Label { visible: !!jump.summary.dsq || !!jump.summary.hs95; text: [jump.summary.dsq,jump.summary.hs95].filter(function(value){return !!value}).join(" · "); color: jump.summary.dsq ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
