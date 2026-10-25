import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

ToolButton {
    id: help
    property string explanation
    implicitWidth: 22; implicitHeight: 22
    padding: 0
    Accessible.name: qsTr("Pomoc")
    Accessible.description: explanation
    contentItem: Text { text: "?"; color: help.hovered || help.activeFocus ? Theme.accent : Theme.muted; font.pointSize: Theme.caption; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
    background: Rectangle { radius: width/2; color: help.hovered || help.activeFocus ? Theme.selection : Theme.raised; border.color: help.activeFocus ? Theme.accent : Theme.outline }
    ToolTip {
        visible: help.hovered || help.activeFocus
        text: help.explanation
        delay: help.hovered ? 350 : 0
        timeout: -1
        width: Math.min(360, help.Window.window ? help.Window.window.width-24 : 300)
    }
}
