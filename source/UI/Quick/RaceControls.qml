import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Flow {
    id: controls
    readonly property var history: ui.find("resultsHistory")
    readonly property var liveControl: ui.find("checkBox_liveCompetition")
    readonly property var jumpControl: ui.find("pushButton_jump")
    readonly property var nextControl: ui.find("pushButton_goToNext")
    readonly property real toolHeight: Math.max(Theme.dp(38),Theme.bodyMetrics.height+Theme.dp(12))
    enabled: !history || !history.values.paused
    spacing: Theme.dp(8)
    implicitHeight: childrenRect.height
    RaceToolGroup {
        width: implicitWidth
        SButton {
            readonly property bool jumping: !ui.context.complete && !ui.context.roundComplete
            readonly property var sourceControl: jumping ? controls.jumpControl : controls.nextControl
            text: sourceControl ? sourceControl.values.text : ""
            enabled: !!sourceControl && sourceControl.values.enabled
            primary: true; dense: true; font.pointSize: Theme.body
            implicitWidth: Theme.dp(160)
            implicitHeight: controls.toolHeight
            onClicked: ui.activate(sourceControl.values.id)
        }
        SToolButton {
            objectName: "raceLiveToggle"; text: qsTr("Na żywo")
            checked: !!controls.liveControl && !!controls.liveControl.values.value
            enabled: !!controls.liveControl && controls.liveControl.values.enabled
            onClicked: ui.activate(controls.liveControl.values.id)
        }
        SToolButton { objectName: "editNextKoGroups"; visible: !!ui.context.nextKoEditable; text: qsTr("Grupy KO…"); onClicked: ui.editNextKoGroups() }
    }
    RaceToolGroup {
        width: implicitWidth
        Label { text: qsTr("Belka"); color: Theme.muted; font.pointSize: Theme.caption; Layout.leftMargin: Theme.dp(4); Layout.rightMargin: Theme.dp(2) }
        RowLayout {
            spacing: Theme.dp(6)
            readonly property var gate: ui.find("spinBox_actualGate")
            SToolButton { text: "−"; autoRepeat: true; autoRepeatDelay: 250; autoRepeatInterval: 60; enabled: !!parent.gate && parent.gate.values.enabled && !ui.context.complete && !ui.context.roundComplete && parent.gate.values.value>parent.gate.values.minimum; onClicked: ui.edit(parent.gate.values.id,parent.gate.values.value-1) }
            Label { text: parent.gate ? parent.gate.values.value : "—"; font.family: Theme.mono; font.bold: true; font.pointSize: Theme.section; color: Theme.ink; horizontalAlignment: Text.AlignHCenter; Layout.minimumWidth: Theme.dp(32) }
            SToolButton { text: "+"; autoRepeat: true; autoRepeatDelay: 250; autoRepeatInterval: 60; enabled: !!parent.gate && parent.gate.values.enabled && !ui.context.complete && !ui.context.roundComplete && parent.gate.values.value<parent.gate.values.maximum; onClicked: ui.edit(parent.gate.values.id,parent.gate.values.value+1) }
        }
        SToolButton { text: ""; iconSource: "qrc:/ui/icons/tools/auto.svg"; Accessible.name: qsTr("Dobierz belkę"); enabled: !ui.context.complete && !ui.context.roundComplete && (!ui.context.liveStage || ui.context.liveStage>=5); hint: qsTr("Dobierz belkę do aktualnych warunków i pozostałych zawodników."); onClicked: ui.suggestRaceGate(false) }
        SToolButton {
            objectName: "raceCoachGate"; text: "−1"
            hint: qsTr("Obniż belkę o 1 na życzenie trenera"); Accessible.name: hint
            checked: Number(ui.context.coachReduction)>0
            readonly property var coachControl: ui.find("pushButton_coachGate")
            enabled: !!coachControl && coachControl.values.enabled && !!ui.context.canLowerCoachGate && !ui.context.complete && !ui.context.roundComplete && (!ui.context.liveStage || ui.context.liveStage>=5)
            onClicked: ui.lowerCoachGate()
        }
    }
    RaceToolGroup {
        width: implicitWidth
        SToolButton { text: qsTr("Czekaj 30s"); readonly property var waitControl: ui.find("pushButton_generateNewWinds"); enabled: !ui.context.complete && !!waitControl && waitControl.values.enabled; onClicked: if(waitControl)ui.activate(waitControl.values.id) }
        SToolButton {
            objectName: "raceConditionsOptions"
            text: ""; iconSource: "qrc:/ui/icons/weather/rain.svg"
            hint: qsTr("Ustawienia wiatru i opadów"); Accessible.name: hint
            readonly property var settingsControl: ui.find("pushButton_windsGeneratorSettings")
            enabled: !!settingsControl && settingsControl.values.enabled
            onClicked: if(settingsControl)ui.activate(settingsControl.values.id)
        }
    }
    RaceToolGroup {
        width: implicitWidth
        SToolButton {
            objectName: "skipCurrentJump"; text: "DNS"
            hint: qsTr("Pomiń skok aktualnego zawodnika"); Accessible.name: hint
            enabled: !ui.context.complete && !ui.context.roundComplete && (!ui.context.liveStage || ui.context.liveStage>=5)
            onClicked: ui.skipCurrentJump()
        }
        SToolButton {
            objectName: "raceManipulateJump"; text: ""; iconSource: "qrc:/ui/icons/tools/adjust.svg"
            hint: qsTr("Jednorazowo manipuluj skokiem"); Accessible.name: qsTr("Manipulacja skoku")
            checked: !!ui.context.manipulationPending
            readonly property var manipulationControl: ui.find("pushButton_manipulateJump")
            enabled: !!manipulationControl && manipulationControl.values.enabled && !ui.context.complete && !ui.context.roundComplete
            onClicked: if(manipulationControl)ui.activate(manipulationControl.values.id)
        }
    }
}
