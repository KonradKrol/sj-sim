import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

AbstractButton {
    id: card
    objectName: "simulationMode"
    property string number
    property string title
    property string description
    property string detail
    property bool featured: false
    property bool expansive: false
    readonly property bool compact: Theme.compact || Window.window && Window.window.height < 760
    hoverEnabled: true
    implicitWidth: 320
    implicitHeight: Math.max(Theme.compact ? 152 : compact ? 188 : 192, content.implicitHeight + padding * 2)
    Accessible.name: title
    Accessible.description: description
    padding: compact ? 16 : 24
    contentItem: Column {
        id: content
        spacing: card.compact ? 10 : 14
        Item {
            width: parent.width; height: Theme.captionMetrics.height + 4
            Label { text: card.number; color: card.featured ? Theme.signal : Theme.accent; font.family: Theme.mono; font.pointSize: Theme.caption }
            Label { text: "Ctrl+" + parseInt(card.number,10); anchors.right: parent.right; color: card.featured ? Theme.headerMuted : Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        }
        Label {
            text: card.title; color: card.featured ? Theme.snow : Theme.ink
            font.family: Theme.displayFamily; font.pointSize: card.expansive ? Theme.display*1.5 : card.featured && !card.compact ? Theme.display : Theme.heading
            font.weight: Font.ExtraBold; font.capitalization: Font.AllUppercase
            width: parent.width; wrapMode: Text.WordWrap
        }
        Label { text: card.description; color: card.featured ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.body; lineHeight: 1.2; width: parent.width; wrapMode: Text.WordWrap }
        Item {
            width: parent.width; height: Math.max(action.implicitHeight,32)
            Label { id: action; text: card.detail; color: card.featured ? Theme.signal : Theme.accent; font.pointSize: Theme.caption; font.weight: Font.DemiBold; width: parent.width - 42; wrapMode: Text.WordWrap; anchors.verticalCenter: parent.verticalCenter }
            Label { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; text: "↗"; color: card.featured ? Theme.signal : Theme.accent; font.pointSize: Theme.heading }
        }
    }
    background: Surface {
        sweep: card.featured ? 48 : 28
        corner: 8
        fillColor: card.featured ? Theme.stadium : card.hovered ? Theme.raised : Theme.surface
        edgeColor: card.activeFocus ? Theme.accent : card.featured ? Theme.signal : Theme.accent
        strokeWidth: card.activeFocus || card.hovered ? 2 : 0
    }
    scale: card.down ? 0.985 : 1
    Behavior on scale { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
}
