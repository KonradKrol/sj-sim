import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: card
    property string title
    property string detail
    property bool activeRoster: false
    property bool condensed: false
    padding: Theme.dp(10)
    implicitHeight: Math.max(Theme.dp(condensed ? 52 : 68),contentItem.implicitHeight+topPadding+bottomPadding)
    hoverEnabled: true
    Accessible.name: title+", "+detail
    Accessible.selected: checked
    background: Surface { fillColor: card.checked ? Theme.stadium : card.hovered ? Theme.selection : Theme.surface; edgeColor: card.checked ? Theme.accent : Theme.divider; strokeWidth: card.checked ? 2 : 1; sweep: card.condensed ? 12 : 22 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(3)
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(6)
            Label { text: card.title; color: card.checked ? Theme.snow : Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            Rectangle { visible: card.activeRoster; color: card.checked ? Theme.signal : Theme.success; implicitWidth: Theme.dp(6); implicitHeight: width; radius: width/2; Accessible.ignored: true }
        }
        Label { text: card.detail; color: card.checked ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
    }
    ToolTip.visible: hovered && !down
    ToolTip.text: title
    ToolTip.delay: 800
}
