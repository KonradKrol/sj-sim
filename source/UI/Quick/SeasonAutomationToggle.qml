import QtQuick 2.15
import QtQuick.Controls 2.15

Switch {
    id: control
    property string detail
    property string explanation
    implicitWidth: 0
    implicitHeight: Math.max(Theme.dp(64),contentItem.implicitHeight+topPadding+bottomPadding)
    padding: Theme.dp(10); leftPadding: Theme.dp(12); rightPadding: Theme.dp(60)
    hoverEnabled: true
    Accessible.description: detail
    contentItem: Column {
        spacing: Theme.dp(4)
        Label { text: control.text; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; width: parent.width; elide: Text.ElideRight }
        Label { text: control.detail; color: Theme.muted; font.pointSize: Theme.caption; width: parent.width; wrapMode: Text.WordWrap }
    }
    indicator: Rectangle {
        width: Theme.dp(38); height: Theme.dp(23); radius: height/2
        x: control.width-width-control.padding; y: (control.height-height)/2
        color: control.checked ? Theme.accent : Theme.outline
        opacity: control.enabled ? 1 : .45
        Rectangle {
            width: parent.height-Theme.dp(6); height: width; radius: width/2
            y: Theme.dp(3); x: control.checked ? parent.width-width-Theme.dp(3) : Theme.dp(3)
            color: control.checked ? Theme.accentText : Theme.surface
            Behavior on x { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
        }
    }
    background: Surface {
        fillColor: control.checked ? Theme.selection : control.hovered ? Theme.raised : Theme.surface
        edgeColor: control.activeFocus || control.checked ? Theme.accent : Theme.outline
        strokeWidth: control.activeFocus ? 2 : 1
        corner: Theme.dp(8); sweep: Theme.dp(14)
    }
    ToolTip.visible: (hovered || activeFocus) && explanation.length>0
    ToolTip.text: explanation
    ToolTip.delay: 500
}
