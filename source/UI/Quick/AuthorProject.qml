import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: project
    property string title
    property string description
    property url destination
    property bool featured: false
    implicitHeight: content.implicitHeight + 28
    padding: 14
    hoverEnabled: true
    Accessible.name: title
    Accessible.description: description
    ToolTip.visible: hovered || activeFocus
    ToolTip.text: destination.toString()
    ToolTip.delay: 600
    onClicked: Qt.openUrlExternally(destination)
    contentItem: ColumnLayout {
        id: content
        spacing: 4
        RowLayout {
            Layout.fillWidth: true
            Label { text: project.title; color: project.featured ? Theme.snow : Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.ExtraBold; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
            Label { text: "↗"; color: project.featured ? Theme.signal : Theme.accent; font.pointSize: Theme.section; Accessible.ignored: true }
        }
        Label { text: project.description; color: project.featured ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption; lineHeight: 1.2; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
    }
    background: Surface {
        sweep: 22
        fillColor: project.featured ? (project.hovered ? Theme.track : Theme.stadium) : project.hovered ? Theme.raised : Theme.selection
        edgeColor: project.featured ? Theme.signal : Theme.accent
        strokeWidth: project.activeFocus ? 2 : 0
    }
}
