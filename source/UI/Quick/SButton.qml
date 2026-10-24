import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control
    property bool primary: false
    property bool quiet: false
    property bool inverse: false
    property string hint: ""
    property bool hintSuppressed: false
    property bool helpRequested: false
    readonly property bool hintVisible: hint.length>0 && !down && (helpRequested || hovered && !hintSuppressed)
    property color foreground: primary ? Theme.accentText : inverse ? Theme.snow : Theme.ink
    implicitHeight: Math.max(primary ? (Theme.compact ? 30 : 48) : (Theme.compact ? 26 : 40), label.implicitHeight + (Theme.compact ? 4 : 16))
    implicitWidth: Math.max(40, label.implicitWidth + (Theme.compact ? 12 : 28))
    padding: Theme.compact ? 2 : 6
    hoverEnabled: true
    font.family: primary ? Theme.displayFamily : Theme.bodyFamily
    font.pointSize: primary ? Theme.section : Theme.body
    font.weight: primary ? Font.ExtraBold : Font.DemiBold
    Accessible.name: text
    Accessible.description: hint
    onHoveredChanged: if(!hovered){hintSuppressed=false;helpRequested=false}
    onActiveFocusChanged: if(!activeFocus)helpRequested=false
    Connections {
        target: control
        function onPressed() { control.hintSuppressed=true;control.helpRequested=false }
    }
    Keys.onPressed: {
        if(event.key===Qt.Key_F1 && hint.length){helpRequested=true;event.accepted=true}
        else event.accepted=false
    }
    ToolTip.visible: hintVisible
    ToolTip.text: hint
    ToolTip.delay: helpRequested ? 0 : 700
    ToolTip.timeout: 6000
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
