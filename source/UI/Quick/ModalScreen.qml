import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    implicitWidth: 560
    implicitHeight: bodyContent.implicitHeight + buttons.implicitHeight + 44
    color: Theme.background
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 16
        spacing: 12
        ScrollView {
            id: body
            objectName: "modalBody"
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: 0; Layout.preferredHeight: 0
            contentWidth: availableWidth; clip: true
            Column {
                id: bodyContent
                width: body.availableWidth; spacing: 12
                Label { text: ui.title; visible: text.length > 0; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; width: parent.width; wrapMode: Text.WordWrap }
                Node { width: parent.width; node: ui.root }
            }
        }
        GridLayout {
            id: buttons
            objectName: "modalButtons"
            Layout.fillWidth: true
            columns: Math.max(1,Math.min(buttonRepeater.count,Math.floor((width+8)/128)))
            columnSpacing: 8; rowSpacing: 8
            Repeater {
                id: buttonRepeater
                model: ui.root.values.buttons || []
                Node { node: modelData; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredHeight: implicitHeight }
            }
        }
    }
}
