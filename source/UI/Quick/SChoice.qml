import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

ComboBox {
    id: control
    implicitHeight: 42
    implicitWidth: 180
    font.pointSize: Theme.body
    leftPadding: 12
    rightPadding: 32
    hoverEnabled: true
    contentItem: Text {
        text: control.displayText
        font: control.font
        color: Theme.ink
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    indicator: Text {
        text: "⌄"; color: Theme.muted
        anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
        font.pointSize: Theme.body
    }
    background: Surface {
        fillColor: control.hovered ? Theme.selection : Theme.raised
        edgeColor: Theme.accent
        strokeWidth: control.activeFocus ? 2 : 0
        sweep: 12
    }
    delegate: ItemDelegate {
        width: control.popup.width
        text: control.textRole ? model[control.textRole] : modelData
        highlighted: control.highlightedIndex === index
        contentItem: Text { text: parent.text; font: control.font; color: Theme.ink; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
        background: Rectangle { color: parent.highlighted ? Theme.selection : Theme.surface }
    }
    popup: Popup {
        objectName: "choicePopup"
        readonly property real above: Math.max(0,control.mapToItem(Overlay.overlay,0,0).y-8)
        readonly property real below: Math.max(0,(control.Window.window ? control.Window.window.height : 600)-control.mapToItem(Overlay.overlay,0,control.height).y-8)
        readonly property real desired: Math.min(320,contentItem.implicitHeight+8)
        y: below >= desired || below >= above ? control.height+4 : -height-4
        margins: 8
        width: control.width
        padding: 4
        implicitHeight: Math.min(desired,Math.max(above,below))
        contentItem: ListView {
            implicitHeight: contentHeight
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollBar.vertical: ScrollBar {}
        }
        background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    }
}
