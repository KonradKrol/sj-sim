import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: bar
    implicitHeight: Math.min(100, flow.implicitHeight + (Theme.compact ? 8 : 16))
    color: Theme.raised
    Flickable {
        anchors.fill: parent; anchors.margins: Theme.compact ? 4 : 8
        contentWidth: width
        contentHeight: flow.implicitHeight
        clip: true
        ScrollBar.vertical: ScrollBar {}
        Flow {
            id: flow
            width: parent.width
            spacing: Theme.compact ? 3 : 6
            Repeater {
                model: ui.actions
                SButton {
                    text: modelData.text + "  ·  " + modelData.sequence
                    enabled: modelData.enabled
                    onClicked: ui.triggerAction(modelData.id)
                    hint: modelData.sequence
                }
            }
        }
    }
}
