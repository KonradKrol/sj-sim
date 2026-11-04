import QtQuick

SButton {
    id: control
    property string actionName
    property string iconName
    Accessible.name: actionName
    hint: actionName
    quiet: true
    implicitWidth: Theme.dp(34)
    implicitHeight: Theme.dp(34)
    font.pointSize: Theme.section
    Item {
        anchors.centerIn: parent
        width: 17; height: 17
        visible: control.iconName === "copy"
        opacity: control.enabled ? 1 : 0.45
        Rectangle { x: 5; y: 4; width: 11; height: 12; radius: 1; color: "transparent"; border.color: control.foreground }
        Rectangle { x: 1; y: 0; width: 11; height: 12; radius: 1; color: Theme.surface; border.color: control.foreground }
    }
}
