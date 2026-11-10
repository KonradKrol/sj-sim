import QtQuick 2.15
import QtQuick.Controls 2.15

SButton {
    id: tool
    property url iconSource
    foreground: checked ? Theme.snow : Theme.ink
    dense: true
    quiet: true
    font.pointSize: Theme.caption
    leftPadding: Theme.dp(6); rightPadding: Theme.dp(6)
    implicitWidth: Math.max(Theme.dp(36),toolContents.implicitWidth+Theme.dp(16))
    implicitHeight: Math.max(Theme.dp(36),Theme.bodyMetrics.height+Theme.dp(12))
    contentItem: Item {
        Row {
            id: toolContents
            anchors.centerIn: parent
            height: Math.max(toolIcon.visible ? toolIcon.height : 0,toolLabel.implicitHeight)
            spacing: Theme.dp(4)
            Image { id: toolIcon; anchors.verticalCenter: parent.verticalCenter; visible: source.toString().length>0; source: tool.iconSource; sourceSize.width: 48; sourceSize.height: 48; width: Theme.dp(20); height: Theme.dp(20); opacity: tool.enabled ? 1 : .45; fillMode: Image.PreserveAspectFit }
            Label { id: toolLabel; anchors.verticalCenter: parent.verticalCenter; visible: tool.text.length>0; text: tool.text; font: tool.font; color: tool.foreground; opacity: tool.enabled ? 1 : .45 }
        }
    }
    background: Rectangle {
        radius: Theme.expressive || Theme.minimal ? Theme.controlRadius : Theme.dp(5)
        color: tool.checked ? Theme.stadium : tool.down ? Theme.selection : tool.hovered ? Theme.selection : Theme.raised
        border.width: tool.visualFocus ? Theme.focusBorder : Math.max(1,Theme.controlBorder)
        border.color: tool.visualFocus ? Theme.accent : tool.checked ? Theme.stadium : Theme.outline
    }
}
