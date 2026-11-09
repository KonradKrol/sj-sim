import QtQuick 2.15

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
        width: Theme.dp(22); height: width
        visible: control.iconName === "add"
        opacity: control.enabled ? 1 : 0.45
        Rectangle { anchors.centerIn: parent; width: parent.width; height: Theme.dp(2); radius: height/2; color: control.foreground }
        Rectangle { anchors.centerIn: parent; height: parent.height; width: Theme.dp(2); radius: width/2; color: control.foreground }
    }
    Item {
        anchors.centerIn: parent
        width: 17; height: 17
        visible: control.iconName === "copy"
        opacity: control.enabled ? 1 : 0.45
        Rectangle { x: 5; y: 4; width: 11; height: 12; radius: 1; color: "transparent"; border.color: control.foreground }
        Rectangle { x: 1; y: 0; width: 11; height: 12; radius: 1; color: Theme.surface; border.color: control.foreground }
    }
    Item {
        anchors.centerIn: parent
        width: 17; height: 17
        visible: control.iconName === "delete"
        opacity: control.enabled ? 1 : 0.45
        Rectangle { x: 5; y: 1; width: 7; height: 3; radius: 1; color: "transparent"; border.color: control.foreground }
        Rectangle { x: 2; y: 4; width: 13; height: 1; color: control.foreground }
        Rectangle { x: 4; y: 5; width: 9; height: 11; radius: 1; color: "transparent"; border.color: control.foreground }
        Rectangle { x: 7; y: 7; width: 1; height: 6; color: control.foreground }
        Rectangle { x: 10; y: 7; width: 1; height: 6; color: control.foreground }
    }
}
