import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
    id: field
    property var design
    selectByMouse: true
    color: design.ink; placeholderTextColor: design.muted
    selectionColor: design.accent; selectedTextColor: design.accentInk
    font.family: design.variant===4 ? Theme.mono : Theme.bodyFamily
    font.pointSize: Theme.body
    implicitWidth: Theme.dp(180)
    implicitHeight: Math.max(Theme.dp(39),contentHeight+Theme.dp(14))
    leftPadding: Theme.dp(10); rightPadding: Theme.dp(10)
    background: Rectangle {
        color: field.design.surface
        radius: field.design.variant===2 ? Theme.dp(18) : field.design.variant===4 ? Theme.dp(3) : field.design.variant===1 ? Theme.dp(6) : 0
        border.width: field.activeFocus ? 2 : 1
        border.color: field.activeFocus ? field.design.accent : field.design.line
        Rectangle { visible: false; anchors.bottom: parent.bottom; width: parent.width; height: field.activeFocus ? 2 : 1; color: field.activeFocus ? field.design.accent : field.design.line }
    }
}
