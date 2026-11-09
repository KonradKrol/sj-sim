import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: controls
    readonly property var history: ui.find("resultsHistory")
    enabled: !history || !history.values.paused
    readonly property var liveControl: ui.find("checkBox_liveCompetition")
    columns: width>=1180 ? 5 : width>=800 ? 3 : 2
    columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(10)
    Node { visible: !ui.context.complete && !ui.context.roundComplete; Layout.preferredWidth: Theme.dp(120); Layout.maximumWidth: Theme.dp(160); node: ui.find("pushButton_jump") }
    RowLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0
        Node { Layout.fillWidth: true; Layout.minimumWidth: 0; node: ui.find("pushButton_goToNext"); opacity: enabled ? 1 : 0.45; Behavior on opacity { NumberAnimation { duration: Theme.motion } } }
        SButton { objectName: "editNextKoGroups"; visible: !!ui.context.nextKoEditable; text: qsTr("Grupy KO…"); quiet: true; onClicked: ui.editNextKoGroups() }
    }
    RowLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.maximumWidth: controls.width >= 700 ? 340 : 9999
        Label { text: qsTr("Belka"); color: Theme.muted; font.pointSize: Theme.caption }
        RowLayout {
            spacing: Theme.dp(6)
            readonly property var gate: ui.find("spinBox_actualGate")
            SToolButton { text: "−"; enabled: !!parent.gate && parent.gate.values.value>parent.gate.values.minimum; onClicked: ui.edit(parent.gate.values.id,parent.gate.values.value-1) }
            Label { text: parent.gate ? parent.gate.values.value : "—"; font.family: Theme.mono; font.bold: true; font.pointSize: Theme.section; color: Theme.ink; horizontalAlignment: Text.AlignHCenter; Layout.minimumWidth: Theme.dp(32) }
            SToolButton { text: "+"; enabled: !!parent.gate && parent.gate.values.value<parent.gate.values.maximum; onClicked: ui.edit(parent.gate.values.id,parent.gate.values.value+1) }
        }
        SToolButton { text: ""; iconSource: "qrc:/ui/icons/tools/auto.svg"; Accessible.name: qsTr("Dobierz belkę"); visible: !ui.context.complete; enabled: !ui.context.roundComplete && (!ui.context.liveStage || ui.context.liveStage>=5); hint: qsTr("Dobierz belkę do aktualnych warunków i pozostałych zawodników."); onClicked: ui.suggestRaceGate(false) }
        Label { visible: ui.context.gateSuggestionDelta!==undefined && ui.context.gateSuggestionDelta!==null; text: Number(ui.context.gateSuggestionDelta)===0 ? qsTr("Bez zmian") : (Number(ui.context.gateSuggestionDelta)>0 ? "+" : "")+ui.context.gateSuggestionDelta; color: Theme.muted; font.pointSize: Theme.caption }
        SToolButton { text: "↶"; hint: qsTr("Cofnij zmianę belki"); Accessible.name: hint; visible: !!ui.context.gateUndoAvailable; onClicked: ui.suggestRaceGate(true) }
    }
    SToolButton {
        objectName: "raceLiveToggle"; text: qsTr("Na żywo")
        checked: !!controls.liveControl && !!controls.liveControl.values.value
        enabled: !!controls.liveControl && controls.liveControl.values.enabled
        onClicked: ui.activate(controls.liveControl.values.id)
    }
    RowLayout {
        Layout.columnSpan: controls.width<1180 ? 2 : 1
        Layout.fillWidth: true; spacing: Theme.dp(9)
        SToolButton { text: qsTr("Czekaj 30s"); readonly property var waitControl: ui.find("pushButton_generateNewWinds"); visible: !ui.context.complete; enabled: !!waitControl && waitControl.values.enabled; onClicked: if(waitControl)ui.activate(waitControl.values.id) }
        SToolButton {
            objectName: "raceConditionsOptions"
            text: ""; iconSource: "qrc:/ui/icons/weather/rain.svg"
            hint: qsTr("Ustawienia wiatru i opadów"); Accessible.name: hint
            readonly property var settingsControl: ui.find("pushButton_windsGeneratorSettings")
            enabled: !!settingsControl && settingsControl.values.enabled
            onClicked: if(settingsControl)ui.activate(settingsControl.values.id)
        }
        Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: Theme.dp(20); color: Theme.outline }
        SToolButton {
            objectName: "skipCurrentJump"; text: "DNS"
            hint: qsTr("Pomiń skok aktualnego zawodnika"); Accessible.name: hint
            enabled: !ui.context.complete && !ui.context.roundComplete && (!ui.context.liveStage || ui.context.liveStage>=5)
            onClicked: ui.skipCurrentJump()
        }
        SToolButton {
            objectName: "raceCoachGate"; text: "−1"
            hint: qsTr("Obniż belkę o 1 na życzenie trenera"); Accessible.name: hint
            checked: Number(ui.context.coachReduction)>0
            readonly property var coachControl: ui.find("pushButton_coachGate")
            enabled: !!coachControl && coachControl.values.enabled && !!ui.context.canLowerCoachGate && !ui.context.complete && !ui.context.roundComplete && (!ui.context.liveStage || ui.context.liveStage>=5)
            onClicked: ui.lowerCoachGate()
        }
        SToolButton {
            objectName: "raceManipulateJump"; text: ""; iconSource: "qrc:/ui/icons/tools/adjust.svg"
            hint: qsTr("Jednorazowo manipuluj skokiem"); Accessible.name: qsTr("Manipulacja skoku")
            checked: !!ui.context.manipulationPending
            readonly property var manipulationControl: ui.find("pushButton_manipulateJump")
            enabled: !!manipulationControl && manipulationControl.values.enabled && !ui.context.complete && !ui.context.roundComplete
            onClicked: if(manipulationControl)ui.activate(manipulationControl.values.id)
        }
        Item { Layout.fillWidth: true }
    }
}
