import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

RowLayout {
    id: root
    property var backend
    readonly property var diagnostic: backend ? backend.profile.formDiagnostic || {} : {}
    readonly property bool automatic: !!(backend && backend.profile.formLocked && !diagnostic.pending)
    spacing: Theme.dp(6)
    SButton {
        id: trigger
        objectName: "athleteFormShortcuts"
        text: qsTr("Zmień formę") + " ▾"
        dense: true
        onClicked: menu.opened ? menu.close() : menu.open()
    }
    HelpDot {
        explanation: qsTr("Stopniowa zmiana działa w kolejnych dniach. Gwałtowna działa od razu. Możesz też ustalić formę od 1 do 100.")
    }
    Item { Layout.fillWidth: true }
    Popup {
        id: menu
        objectName: "athleteFormShortcutsMenu"
        parent: trigger
        y: trigger.height + Theme.dp(6)
        width: Math.min(Theme.dp(430), trigger.Window.window ? trigger.Window.window.width-Theme.dp(24) : Theme.dp(430))
        padding: Theme.dp(12)
        margins: Theme.dp(12)
        property bool exact: false
        onOpened: exactValue.text=String(root.backend.profile.form)
        onClosed: exact=false
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
        enter: Transition { NumberAnimation { property: "opacity"; from: 0; to: 1; duration: Theme.motion } }
        contentItem: ColumnLayout {
            spacing: Theme.dp(10)
            GridLayout {
                visible: root.automatic
                columns: 2
                Layout.fillWidth: true
                columnSpacing: Theme.dp(8)
                rowSpacing: Theme.dp(8)
                Repeater {
                    model: [
                        {label: qsTr("Stopniowa podwyżka"), symbol: "↗", action: 0, up: true},
                        {label: qsTr("Stopniowy spadek"), symbol: "↘", action: 1, up: false},
                        {label: qsTr("Gwałtowna zwyżka"), symbol: "↑", action: 5, up: true},
                        {label: qsTr("Gwałtowne pogorszenie"), symbol: "↓", action: 6, up: false},
                        {label: qsTr("Głęboki kryzys"), symbol: "−", action: 2, up: false},
                        {label: qsTr("Wejście na szczyt"), symbol: "+", action: 3, up: true}
                    ]
                    Button {
                        id: actionButton
                        objectName: "athleteFormAction-" + modelData.action
                        required property var modelData
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        implicitHeight: Theme.dp(60)
                        padding: Theme.dp(9)
                        hoverEnabled: true
                        readonly property bool gradual: modelData.action<2
                        readonly property bool atLimit: modelData.action===0 ? root.diagnostic.regimeIndex===6 : modelData.action===1 && root.diagnostic.regimeIndex===0
                        enabled: root.automatic && !atLimit && !(gradual && root.diagnostic.gradualSteeringLocked)
                        opacity: enabled ? 1 : .4
                        Accessible.name: modelData.label
                        background: Rectangle {
                            radius: Theme.dp(8)
                            color: actionButton.down ? Theme.selection : actionButton.hovered ? Theme.raised : Theme.surface
                            border.color: actionButton.activeFocus ? Theme.accent : Theme.outline
                        }
                        contentItem: RowLayout {
                            spacing: Theme.dp(8)
                            Text { text: actionButton.modelData.symbol; color: actionButton.modelData.up ? Theme.success : Theme.danger; font.pointSize: Theme.heading; font.bold: true; Layout.preferredWidth: Theme.dp(22); horizontalAlignment: Text.AlignHCenter }
                            Text { text: actionButton.modelData.label; color: Theme.ink; font.family: Theme.bodyFamily; font.pointSize: Theme.body; font.weight: Font.DemiBold; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        }
                        ToolTip.visible: hovered && gradual && root.diagnostic.gradualSteeringLocked
                        ToolTip.text: qsTr("Stopniowa zmiana już działa")
                        onClicked: {root.backend.steerForm(modelData.action,0);menu.close()}
                    }
                }
            }
            Label {
                visible: root.automatic && root.diagnostic.gradualSteeringLocked===true
                text: qsTr("Stopniowa zmiana jest już aktywna")
                color: Theme.muted
                font.pointSize: Theme.caption
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
            SButton {
                visible: !menu.exact
                Layout.fillWidth: true
                text: qsTr("Ustal precyzyjnie") + "…"
                dense: true
                onClicked: {menu.exact=true;exactValue.forceActiveFocus();exactValue.selectAll()}
            }
            ColumnLayout {
                visible: menu.exact
                Layout.fillWidth: true
                spacing: Theme.dp(5)
                Label { text: qsTr("Forma · zakres 1–100"); color: Theme.muted; font.pointSize: Theme.caption }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.dp(8)
                    SField {
                        id: exactValue
                        objectName: "athleteFormExactValue"
                        Layout.fillWidth: true
                        Accessible.name: qsTr("Forma od 1 do 100")
                        validator: IntValidator { bottom: 1; top: 100 }
                        onAccepted: if(apply.enabled)apply.clicked()
                    }
                    SButton {
                        id: apply
                        text: qsTr("Ustaw"); primary: true; dense: true
                        enabled: exactValue.acceptableInput
                        onClicked: {root.backend.steerForm(4,Number(exactValue.text));menu.close()}
                    }
                }
            }
        }
    }
}
