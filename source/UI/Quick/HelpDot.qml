import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

ToolButton {
    id: help
    property string explanation
    property bool prominent: false
    implicitWidth: prominent ? 34 : 22; implicitHeight: implicitWidth
    focusPolicy: Qt.StrongFocus
    onClicked: forceActiveFocus()
    padding: 0
    Accessible.name: qsTr("Pomoc")
    Accessible.description: explanation
    contentItem: Text { text: "?"; color: help.hovered || help.activeFocus ? Theme.accent : help.prominent ? Theme.ink : Theme.muted; font.pointSize: help.prominent ? Theme.heading : Theme.caption; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
    background: Rectangle { radius: width/2; color: help.hovered || help.activeFocus ? Theme.selection : Theme.raised; border.color: help.activeFocus ? Theme.accent : Theme.outline }
    ToolTip {
        parent: help
        x: 0; y: help.height+6
        margins: 8
        visible: help.hovered || help.activeFocus
        text: help.explanation
        delay: help.hovered ? 350 : 0
        timeout: -1
        width: Math.min(360, help.Window.window ? help.Window.window.width-24 : 300)
    }
}
