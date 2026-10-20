import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: bar
    implicitHeight: Math.min(100, flow.implicitHeight + 16)
    color: Theme.raised
    Flickable {
        anchors.fill: parent; anchors.margins: 8
        contentWidth: width
        contentHeight: flow.implicitHeight
        clip: true
        ScrollBar.vertical: ScrollBar {}
        Flow {
            id: flow
            width: parent.width
            spacing: 6
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
