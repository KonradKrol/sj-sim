import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id:button
    property var theme
    property bool primary: false
    implicitHeight:40
    leftPadding:14; rightPadding:14
    hoverEnabled:true
    font.pixelSize:13
    contentItem:Text {
        text:button.text; font:button.font
        color:button.primary ? "white" : button.theme.ink
        horizontalAlignment:Text.AlignHCenter; verticalAlignment:Text.AlignVCenter
        elide:Text.ElideRight
    }
    background:Rectangle {
        radius:7
        color:button.primary ? (button.down ? "#2156bd" : button.hovered ? "#3d7ce7" : "#2d6bdd") : (button.down ? (theme.dark ? "#354a65" : "#dce6f4") : button.hovered ? (theme.dark ? "#2d4059" : "#eaf0f9") : (theme.dark ? "#243247" : "#f2f5fa"))
        border.width:button.activeFocus ? 2 : 1
        border.color:button.activeFocus ? "#639aff" : button.primary ? "transparent" : theme.dark ? "#35465d" : "#e0e7f1"
        Behavior on color { ColorAnimation { duration:110 } }
        scale:button.down ? .985 : 1
        Behavior on scale { NumberAnimation { duration:90 } }
    }
}
