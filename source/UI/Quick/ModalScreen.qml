import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    implicitWidth: 560
    implicitHeight: title.implicitHeight + bodyContent.implicitHeight + buttons.implicitHeight + 56
    color: Theme.background
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 16
        spacing: 12
        Label { id: title; text: ui.title; visible: text.length > 0; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        ScrollView {
            id: body
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: 0; Layout.preferredHeight: 0
            contentWidth: availableWidth; clip: true
            Node { id: bodyContent; width: body.availableWidth; node: ui.root }
        }
        RowLayout {
            id: buttons
            objectName: "modalButtons"
            Layout.fillWidth: true
            Repeater {
                model: ui.root.values.buttons || []
                Node { node: modelData; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredHeight: implicitHeight }
            }
        }
    }
}
