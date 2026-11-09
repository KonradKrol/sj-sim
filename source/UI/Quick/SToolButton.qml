import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SButton {
    id: tool
    property url iconSource
    dense: true
    quiet: true
    font.pointSize: Theme.caption
    leftPadding: Theme.dp(6); rightPadding: Theme.dp(6)
    implicitWidth: Math.max(Theme.dp(36),contentItem.implicitWidth+Theme.dp(16))
    implicitHeight: Math.max(Theme.dp(36),Theme.bodyMetrics.height+Theme.dp(12))
    contentItem: RowLayout {
        spacing: Theme.dp(4)
        Item { Layout.fillWidth: true }
        Image { visible: source.toString().length>0; source: tool.iconSource; sourceSize.width: 48; sourceSize.height: 48; Layout.preferredWidth: Theme.dp(20); Layout.preferredHeight: Theme.dp(20); opacity: tool.enabled ? 1 : .45; fillMode: Image.PreserveAspectFit }
        Label { visible: tool.text.length>0; text: tool.text; font: tool.font; color: tool.foreground; opacity: tool.enabled ? 1 : .45 }
        Item { Layout.fillWidth: true }
    }
    background: Rectangle { radius: Theme.dp(4); color: tool.checked || tool.down ? Theme.selection : tool.hovered ? Theme.raised : "transparent"; border.width: tool.visualFocus ? 2 : tool.checked ? 1 : 0; border.color: tool.visualFocus ? Theme.accent : tool.checked ? Theme.outline : Theme.divider }
}
