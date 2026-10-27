import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Control {
    id: choice
    property var backend
    property var perk
    readonly property int selected: Number((backend.selectedPerks || [])[perk.pair] || 0)
    readonly property bool chosen: selected*perk.side>0
    readonly property int tier: chosen ? Math.abs(selected) : 0
    padding: Theme.dp(8)
    background: Rectangle { color: choice.chosen ? Theme.selection : Theme.raised; radius: Theme.dp(6); Behavior on color { ColorAnimation { duration: Theme.motion } } }
    contentItem: RowLayout {
            spacing: Theme.dp(7)
            Rectangle {
                color: Theme.stadium; radius: Theme.dp(4); Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(28)
                Image { anchors.fill: parent; anchors.margins: Theme.dp(3); source: "icons/hill/"+choice.perk.key+".svg"; sourceSize.width: 48; sourceSize.height: 48; Accessible.ignored: true }
            }
            Label { text: choice.perk.name; color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
        PerkStrengthSelector {
            backend: choice.backend; perk: choice.perk; prefix: "hillPerk"
            levelNames: [qsTr("Lekko"),qsTr("Wyraźnie"),qsTr("Mocno")]
            Layout.alignment: Qt.AlignVCenter
        }
        HelpDot { explanation: choice.perk.description }
    }
}
