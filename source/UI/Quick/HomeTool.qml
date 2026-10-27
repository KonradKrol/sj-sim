import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: card
    property string title
    property string description
    property string actionText
    property int entranceDelay: 195
    property real measurementWidth: Theme.dp(320)
    MenuEntrance { id: entrance; target: card; delay: card.entranceDelay }
    opacity: entrance.progress
    transform: Translate { y: (1-entrance.progress)*Theme.dp(40) }
    hoverEnabled: true
    padding: Theme.dp(Theme.compact ? 12 : 22)
    implicitWidth: Theme.dp(320)
    readonly property real textWidth: Math.max(1,measurementWidth-leftPadding-rightPadding-Theme.dp(18)-arrow.implicitWidth)
    implicitHeight: Math.max(Theme.dp(Theme.compact ? 100 : 142),Math.max(arrow.implicitHeight,titleMeasure.implicitHeight+descriptionMeasure.implicitHeight+actionMeasure.implicitHeight+Theme.dp(19))+topPadding+bottomPadding)
    Accessible.name: actionText
    Accessible.description: description
    Text { id: titleMeasure; visible: false; width: card.textWidth; text: card.title; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; wrapMode: Text.WordWrap }
    Text { id: descriptionMeasure; visible: false; width: card.textWidth; text: card.description; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
    Text { id: actionMeasure; visible: false; width: card.textWidth; text: card.actionText; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; font.weight: Font.DemiBold; wrapMode: Text.WordWrap }
    background: Rectangle {
        color: card.down ? Theme.selection : card.hovered ? Theme.raised : Theme.surface
        radius: Theme.radius
        border.width: card.visualFocus ? 2 : 0
        border.color: Theme.accent
    }
    contentItem: RowLayout {
        spacing: Theme.dp(18)
        ColumnLayout {
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            spacing: Theme.dp(7)
            Label { text: card.title; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: card.description; color: Theme.muted; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: card.actionText; color: Theme.accent; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; font.weight: Font.DemiBold; Layout.fillWidth: true; wrapMode: Text.WordWrap; Layout.topMargin: Theme.dp(5) }
        }
        Label { id: arrow; text: "→"; color: Theme.accent; font.pointSize: Theme.heading; Accessible.ignored: true }
    }
}
