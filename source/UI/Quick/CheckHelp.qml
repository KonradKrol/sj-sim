import QtQuick 2.15

Item {
    id: control
    property string text
    property string explanation
    property string controlName
    property bool checked: false
    property bool editable: true
    signal clicked(bool value)
    implicitHeight: Math.max(check.implicitHeight,22)
    SCheck {
        id: check
        objectName: control.controlName
        width: Math.min(implicitWidth,Math.max(0,control.width-28))
        text: control.text
        checked: control.checked
        enabled: control.editable
        onClicked: control.clicked(checked)
    }
    HelpDot { anchors.left: check.right; anchors.leftMargin: 6; anchors.verticalCenter: check.verticalCenter; explanation: control.explanation }
}
