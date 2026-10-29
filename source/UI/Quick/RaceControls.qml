import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: controls
    readonly property var history: ui.find("resultsHistory")
    enabled: !history || !history.values.paused
    signal optionsRequested(int section)
    readonly property var liveControl: ui.find("checkBox_liveCompetition")
    columns: width >= 800 ? 5 : 2
    columnSpacing: 8; rowSpacing: 6
    Node { visible: !ui.context.complete && !ui.context.roundComplete; Layout.fillWidth: true; Layout.minimumWidth: 0; node: ui.find("pushButton_jump") }
    RowLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0
        Node { Layout.fillWidth: true; Layout.minimumWidth: 0; node: ui.find("pushButton_goToNext"); opacity: enabled ? 1 : 0.45; Behavior on opacity { NumberAnimation { duration: Theme.motion } } }
        SButton { objectName: "editNextKoGroups"; visible: !!ui.context.nextKoEditable; text: qsTr("Grupy KO…"); quiet: true; onClicked: ui.editNextKoGroups() }
    }
    RowLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.maximumWidth: controls.width >= 700 ? 220 : 9999
        Label { text: qsTr("Belka"); color: Theme.muted; font.pointSize: Theme.caption }
        Node { Layout.fillWidth: true; Layout.minimumWidth: 90; node: ui.find("spinBox_actualGate") }
    }
    SCheck {
        objectName: "raceLiveToggle"; text: qsTr("Na żywo")
        Layout.fillWidth: true; Layout.minimumWidth: 0
        checked: !!controls.liveControl && !!controls.liveControl.values.value
        enabled: !!controls.liveControl && controls.liveControl.values.enabled
        onClicked: ui.activate(controls.liveControl.values.id)
    }
    RowLayout {
        Layout.columnSpan: controls.width<800 ? 2 : 1
        Layout.fillWidth: true; spacing: Theme.dp(4)
        Label { text: qsTr("Opcje"); color: Theme.muted; font.pointSize: Theme.caption }
        Repeater {
            model: [qsTr("Przebieg"),qsTr("Warunki"),qsTr("Skok")]
            SButton {
                objectName: index===0 ? "raceOptions" : index===1 ? "raceConditionsOptions" : "raceJumpOptions"
                id: optionButton
                text: modelData; padding: Theme.dp(6); font.pointSize: Theme.caption
                hint: qsTr("Ustawienia: %1").arg(modelData)
                implicitWidth: contentItem.implicitWidth+Theme.dp(12)
                contentItem: RowLayout {
                    spacing: Theme.dp(4)
                    Label { text: optionButton.text; color: Theme.ink; font: optionButton.font; Layout.fillWidth: true; elide: Text.ElideRight }
                    DropdownIndicator { tint: Theme.accent; Layout.preferredWidth: Theme.dp(10); Layout.preferredHeight: Theme.dp(7) }
                }
                background: Rectangle { color: optionButton.hovered || optionButton.down ? Theme.selection : Theme.raised; radius: Theme.dp(4); border.width: optionButton.visualFocus ? 2 : 1; border.color: optionButton.hovered || optionButton.visualFocus ? Theme.accent : Theme.outline }
                Layout.fillWidth: true; Layout.minimumWidth: 0
                onClicked: controls.optionsRequested(index)
            }
        }
    }
}
