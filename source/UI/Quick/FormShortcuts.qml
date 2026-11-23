import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Season-only shortcut menu: five simple ways to steer the automatic form engine.
SButton {
    id: trigger
    objectName: "athleteFormShortcuts"
    property var backend
    text: qsTr("Zmień formę ▾")
    dense: true
    Accessible.name: qsTr("Zmień formę")
    onClicked: menu.opened ? menu.close() : menu.open()

    Popup {
        id: menu
        objectName: "athleteFormShortcutsMenu"
        y: trigger.height + Theme.dp(6)
        width: Theme.dp(300)
        padding: Theme.dp(10)
        property bool exact: false
        onClosed: exact = false
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: Theme.motion }
            NumberAnimation { property: "scale"; from: .94; to: 1; duration: Theme.motion; easing.type: Easing.OutCubic }
        }
        exit: Transition { NumberAnimation { property: "opacity"; to: 0; duration: Theme.motion * .6 } }
        transformOrigin: Popup.Top
        contentItem: ColumnLayout {
            spacing: Theme.dp(4)
            Repeater {
                model: [
                    {label: qsTr("Zwiększ formę"), detail: qsTr("Okres formy o poziom wyżej"), action: 0},
                    {label: qsTr("Zmniejsz formę"), detail: qsTr("Okres formy o poziom niżej"), action: 1},
                    {label: qsTr("Wprowadź w kryzys"), detail: qsTr("Okres kryzysu"), action: 2},
                    {label: qsTr("Wprowadź w szczyt"), detail: qsTr("Okres szczytu formy"), action: 3},
                    {label: qsTr("Ustal precyzyjnie…"), detail: qsTr("Dokładna wartość; okres przesunie się o krok w jej stronę"), action: 4}
                ]
                ItemDelegate {
                    id: option
                    required property var modelData
                    required property int index
                    Layout.fillWidth: true
                    padding: Theme.dp(8)
                    enabled: modelData.action===4 || !!trigger.backend.profile.formLocked
                    opacity: !menu.opened ? 0 : enabled ? 1 : .45
                    Behavior on opacity { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
                    transform: Translate { y: menu.opened ? 0 : Theme.dp(6); Behavior on y { NumberAnimation { duration: Theme.motion + option.index * 30; easing.type: Easing.OutCubic } } }
                    Accessible.name: modelData.label
                    background: Rectangle { radius: Theme.dp(6); color: option.down ? Theme.selection : option.hovered ? Theme.raised : "transparent" }
                    contentItem: ColumnLayout {
                        spacing: Theme.dp(1)
                        Label { text: option.modelData.label; color: Theme.ink; font.bold: true; font.pointSize: Theme.body }
                        Label { text: option.modelData.detail; color: Theme.muted; font.pointSize: Theme.caption }
                    }
                    onClicked: {
                        if(modelData.action===4){menu.exact=true;exactValue.forceActiveFocus();return}
                        trigger.backend.steerForm(modelData.action,0);menu.close()
                    }
                }
            }
            RowLayout {
                visible: menu.exact
                Layout.fillWidth: true; spacing: Theme.dp(6)
                SField {
                    id: exactValue
                    objectName: "athleteFormExactValue"
                    Layout.fillWidth: true
                    placeholderText: qsTr("Forma 1–100")
                    Accessible.name: qsTr("Dokładna forma")
                    validator: IntValidator { bottom: 1; top: 100 }
                    text: String(trigger.backend.profile.form || "")
                    onAccepted: apply.clicked()
                }
                SButton {
                    id: apply
                    text: qsTr("Ustaw"); primary: true; dense: true
                    enabled: exactValue.acceptableInput
                    onClicked: {trigger.backend.steerForm(4,Number(exactValue.text));menu.close()}
                }
            }
            Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider; Layout.topMargin: Theme.dp(4) }
            Label {
                Layout.fillWidth: true; wrapMode: Text.WordWrap
                color: Theme.muted; font.pointSize: Theme.caption
                text: trigger.backend.profile.formLocked
                      ? qsTr("To tylko skróty. Pod spodem forma działa na okresach, od kryzysu do szczytu, które same trwają i się zmieniają. Skrót zmienia tylko okres. Forma dochodzi do niego stopniowo, we własnym rytmie, więc efekt widać w kolejnych dniach.")
                      : qsTr("Automatyczna forma jest wyłączona, więc nie ma okresów formy. Możesz tylko ustalić dokładną wartość.")
            }
        }
    }
}
