import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control
    property bool primary: false
    implicitHeight: 38
    leftPadding: 16
    rightPadding: 16
    font.pixelSize: 13
    scale: down ? 0.98 : 1
    Behavior on scale { NumberAnimation { duration: 90 } }
    background: Rectangle {
        radius: control.primary ? 18 : 10
        color: control.primary ? (control.hovered ? "#3b65d7" : "#2e55c5") : (control.hovered ? "#e8e9e6" : "#f0f1ee")
        border.color: control.activeFocus ? "#2e55c5" : "transparent"
        border.width: 2
        opacity: control.enabled ? 1 : 0.5
        Behavior on color { ColorAnimation { duration: 120 } }
    }
    contentItem: Text {
        text: control.text
        font: control.font
        color: control.primary ? "white" : control.enabled ? "#363b39" : "#959a96"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
