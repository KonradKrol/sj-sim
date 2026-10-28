import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: jump
    objectName: "raceLastJump"
    property bool dense: false
    readonly property var summary: ui.jumpSummary
    readonly property bool narrow: width<480
    property bool windOpen: false
    readonly property string windKey: summary.windProfileKey || ""
    onWindKeyChanged: windOpen=false
    Loader {
        objectName: "jumpWindLoader"
        active: jump.windOpen && !!jump.summary.windProfile
        onLoaded: if(status===Loader.Ready && item)item.open()
        sourceComponent: Component { JumpWindProfile { recorded: jump.summary.windProfile || ({}); athlete: jump.summary.name || ""; onClosed: jump.windOpen=false } }
    }
    ChangeFeedback { id: resultArrival; target: jump; trigger: [jump.summary.name,jump.summary.distance,jump.summary.points,jump.summary.selectedRound].join("|"); duration: Theme.motion*2 }
    implicitHeight: content.implicitHeight + topPadding + bottomPadding
    padding: dense ? 4 : 8
    function signedColor(value){return value>0 ? Theme.success : value<0 ? Theme.danger : Theme.ink}
    function signedText(text,value){var shown=text || "—";if(value===0 && parseFloat(shown)===0)shown=shown.replace(/^[-−]/,"");return (value>0 ? "+" : "")+shown}
    background: Rectangle {
        color: Theme.surface; border.color: Theme.outline
        Rectangle { width: 24; height: 3; color: Theme.signal }
    }
    contentItem: Column {
        id: content
        objectName: "lastJumpContent"
        opacity: 0.6+resultArrival.progress*0.4
        transform: Translate { y: (1-resultArrival.progress)*Theme.dp(8) }
        spacing: 3
        RowLayout {
            width: parent.width; spacing: 8
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 1
                RowLayout {
                    Layout.fillWidth: true; spacing: 8
                    Image { objectName: "lastJumpFlag"; source: jump.summary.flag || ""; visible: source.toString().length>0; Layout.preferredWidth: 24; Layout.preferredHeight: 18; sourceSize.width: 48; sourceSize.height: 36; fillMode: Image.PreserveAspectFit }
                    Label { text: jump.summary.name || "—"; color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
                }
                Label {
                    objectName: "lastJumpLeaderGap"
                    visible: jump.summary.leaderGap !== undefined
                    text: jump.summary.leaderGap>0 ? qsTr("Do lidera: %1 pkt").arg(Number(jump.summary.leaderGap).toLocaleString(Qt.locale(Theme.localeName),'f',1)) : qsTr("Lider")
                    color: Theme.muted; font.pointSize: Theme.caption
                }
            }
            SChoice {
                objectName: "jumpRoundChoice"
                visible: (jump.summary.roundChoices || []).length>1 && !ui.context.liveStage
                model: jump.summary.roundChoices || []; currentIndex: jump.summary.selectedRound || 0
                Layout.preferredWidth: Math.max(72,Theme.dp(110)); Layout.maximumWidth: 110
                font.pointSize: Theme.caption; Accessible.name: qsTr("Seria skoku")
                onActivated: function(index){ui.selectPreviewRound(index)}
            }
            Rectangle {
                color: Theme.selection; radius: 6
                implicitWidth: rank.implicitWidth+16; implicitHeight: rank.implicitHeight+6
                Row {
                    id: rank
                    anchors.centerIn: parent; spacing: 6
                    Label { objectName: "lastJumpPosition"; text: jump.summary.positionNumber>0 ? jump.summary.positionNumber+"." : "—"; color: Theme.accent; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold }
                    Label { text: qsTr("miejsce"); color: Theme.ink; font.pointSize: Theme.caption; anchors.verticalCenter: parent.verticalCenter }
                }
            }
        }
        GridLayout {
            width: parent.width
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
            width: parent.width
            columns: jump.narrow ? 3 : 4; columnSpacing: 8; rowSpacing: 2
            JumpMetric { inlineValue: !jump.narrow; caption: jump.summary.windProfile ? qsTr("Wiatr ↗") : qsTr("Wiatr"); value: jump.signedText(jump.summary.wind,jump.summary.windValue); valueColor: jump.signedColor(jump.summary.windValue); interactive: !!jump.summary.windProfile; onClicked: jump.windOpen=true; objectName: "lastJumpWind"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; caption: qsTr("Belka"); value: jump.summary.gate || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; caption: qsTr("Ląd."); value: jump.summary.landing || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; caption: qsTr("Rekomp."); value: jump.signedText(jump.summary.compensation,jump.summary.compensationValue); valueColor: jump.signedColor(jump.summary.compensationValue); objectName: "lastJumpCompensation"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; caption: qsTr("Za wiatr"); value: jump.summary.windPoints || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; caption: qsTr("Za belkę"); value: jump.summary.gatePoints || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; objectName: "lastJumpRoundPosition"; caption: qsTr("W serii"); value: jump.summary.roundPositionNumber>0 ? jump.summary.roundPositionNumber+"." : "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; visible: !!jump.summary.groupPosition; caption: qsTr("Grupa"); value: jump.summary.groupPosition || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; visible: !!jump.summary.competitionPosition; caption: qsTr("Konkurs"); value: jump.summary.competitionPosition || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            JumpMetric { inlineValue: !jump.narrow; caption: qsTr("Suma"); value: jump.summary.totalPoints || "—"; Layout.fillWidth: true; Layout.minimumWidth: 0 }
        }
        Flow {
            visible: !!jump.summary.takeoff || !!jump.summary.flight || !!jump.summary.random || !!jump.summary.snow
            width: parent.width
            spacing: 8
            JumpMetric { inlineValue: true; visible: !!jump.summary.takeoff; caption: qsTr("Wybicie"); value: jump.summary.takeoff || "" }
            JumpMetric { inlineValue: true; visible: !!jump.summary.flight; caption: qsTr("Lot"); value: jump.summary.flight || "" }
            JumpMetric { inlineValue: true; visible: !!jump.summary.random; caption: qsTr("Los."); value: jump.summary.random || "" }
            JumpMetric { inlineValue: true; visible: !!jump.summary.snow; caption: qsTr("Śnieg"); value: jump.summary.snow || "" }
        }
        JumpWeights { width: parent.width; summary: jump.summary }
        Label { width: parent.width; visible: !!jump.summary.dsq || !!jump.summary.hs95; text: [jump.summary.dsq,jump.summary.hs95].filter(function(value){return !!value}).join(" · "); color: jump.summary.dsq && !jump.summary.dns ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
    }
}
