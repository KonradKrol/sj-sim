import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control
    property bool primary: false
    property bool quiet: false
    property bool inverse: false
    property string hint: ""
    property color foreground: primary ? Theme.accentText : inverse ? Theme.snow : Theme.ink
    implicitHeight: Math.max(primary ? 48 : 40, label.implicitHeight + 16)
    implicitWidth: Math.max(40, label.implicitWidth + 28)
    hoverEnabled: true
    font.family: primary ? Theme.displayFamily : Theme.bodyFamily
    font.pointSize: primary ? Theme.section : Theme.body
    font.weight: primary ? Font.ExtraBold : Font.DemiBold
    Accessible.name: text
    ToolTip.visible: (hovered || activeFocus) && hint.length > 0
    ToolTip.text: hint
    ToolTip.delay: 500
    contentItem: Text {
        id: label
        text: control.text
        font: control.font
        color: control.foreground
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        opacity: control.enabled ? 1 : 0.55
    }
    background: Surface {
        sweep: control.primary ? 20 : 12
        corner: control.quiet ? 8 : 6
        fillColor: control.inverse ? (control.hovered || control.down ? Theme.track : "transparent") : control.primary ? (control.down ? Qt.darker(Theme.accent,1.15) : Theme.accent) : control.down ? Theme.selection : control.hovered ? Theme.raised : control.quiet ? "transparent" : Theme.raised
        strokeWidth: control.activeFocus ? 2 : 0
        edgeColor: control.inverse ? Theme.signal : Theme.accent
        Rectangle { visible: control.activeFocus && control.primary; anchors.fill: parent; anchors.margins: 6; color: "transparent"; radius: 6; border.color: Theme.accentText }
    }
}
