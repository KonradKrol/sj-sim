import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: status
    objectName: "athleteFormStatus"
    property int value: 50
    readonly property int band: value<20 ? 0 : value<35 ? 1 : value<45 ? 2 : value<56 ? 3 : value<70 ? 4 : value<85 ? 5 : value<95 ? 6 : 7
    readonly property var labels: [qsTr("Głęboki kryzys"),qsTr("Słaba"),qsTr("Obniżona"),qsTr("Typowa"),qsTr("Podwyższona"),qsTr("Wysoka"),qsTr("Forma życia"),qsTr("Absolutny szczyt")]
    readonly property color tint: [Theme.danger,Theme.danger,Theme.warning,Theme.muted,Theme.accent,Theme.success,Theme.accent,Theme.signal][band]
    implicitWidth: 0
    implicitHeight: Math.max(Theme.dp(32),Theme.bodyMetrics.height+Theme.dp(8))
    padding: Theme.dp(4)
    hoverEnabled: true
    ToolTip.visible: hovered
    ToolTip.text: labels[band]
    ToolTip.delay: 500
    Accessible.role: Accessible.StaticText; Accessible.name: labels[band]
    background: Surface {
        visible: status.band===0 || status.band>=6
        fillColor: status.band===7 ? Theme.stadium : status.band===6 ? Theme.selection : Qt.rgba(Theme.danger.r,Theme.danger.g,Theme.danger.b,.08)
        edgeColor: status.tint; strokeWidth: 1; corner: Theme.dp(4); sweep: Theme.dp(status.band===7 ? 12 : 6)
    }
    contentItem: RowLayout {
        spacing: Theme.dp(5)
        Item {
            visible: status.band===0 || status.band>=6
            Layout.preferredWidth: Theme.dp(18); Layout.preferredHeight: Theme.dp(18)
            Rectangle { visible: status.band>=6; anchors.centerIn: parent; width: Theme.dp(status.band===7 ? 12 : 8); height: width; rotation: 45; color: status.band===7 ? status.tint : "transparent"; border.width: 1; border.color: status.tint; radius: 1 }
            Rectangle { visible: status.band===7; anchors.centerIn: parent; width: Theme.dp(5); height: width; rotation: 45; color: Theme.stadium }
            Label { visible: status.band===0; anchors.centerIn: parent; text: "!"; color: status.tint; font.bold: true; font.pointSize: Theme.section }
        }
        Label {
            objectName: "athleteFormStatusText"
            text: status.labels[status.band]; color: status.tint
            font.family: Theme.displayFamily
            font.pointSize: Theme.body; font.weight: status.band===0 || status.band>=6 ? Font.Bold : Font.Medium
            Layout.fillWidth: true; Layout.minimumWidth: 0; maximumLineCount: 1; elide: Text.ElideRight
            fontSizeMode: Text.HorizontalFit; minimumPointSize: Theme.caption
        }
    }
}
