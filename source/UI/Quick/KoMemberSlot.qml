import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SButton {
    id: member
    property string groupId
    property int slot: -1
    property string athleteId
    property string athleteName
    property string flagSource
    property bool dropTarget: false
    signal activated(int athleteIndex)
    signal chooseRequested()
    implicitHeight: Math.max(Theme.controlHeight,Theme.bodyMetrics.height+Theme.dp(16))
    padding: Theme.dp(6)
    Accessible.name: qsTr("Zmień zawodnika: %1").arg(athleteName)
    onClicked: chooseRequested()
    contentItem: RowLayout {
        spacing: Theme.dp(6)
        Label { text: "⠿"; color: Theme.muted; font.pointSize: Theme.body; Layout.preferredWidth: Theme.dp(24); horizontalAlignment: Text.AlignHCenter; Accessible.ignored: true }
        Image { source: member.flagSource; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: 24; Layout.preferredHeight: 18; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
        Label { text: member.athleteName; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
        Label { text: "⌄"; color: Theme.muted; font.pointSize: Theme.body; Accessible.ignored: true }
    }
    background: Rectangle { color: member.dropTarget ? Theme.selection : member.hovered ? Theme.raised : Theme.surface; radius: Theme.dp(4); border.width: member.dropTarget || member.activeFocus ? 2 : 1; border.color: member.dropTarget || member.activeFocus ? Theme.accent : Theme.divider }
}
