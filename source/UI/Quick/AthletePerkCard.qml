import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: card
    property var backend
    property var perk
    property bool dense: false
    readonly property int selected: Number((backend.profile.perks || [])[perk.pair] || 0)
    readonly property bool chosen: selected*perk.side>0
    readonly property bool advanced: chosen && Math.abs(selected)===2
    readonly property color gold: Theme.dark ? "#f5cf84" : "#80500c"
    readonly property color ink: chosen ? Theme.snow : Theme.ink
    padding: Theme.dp(dense ? 10 : 13)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle {
        color: card.chosen ? "#142e42" : Theme.surface; radius: Theme.dp(9)
        border.width: card.advanced ? 2 : 1
        border.color: card.advanced ? card.gold : card.chosen ? card.perk.tint : Theme.divider
        Rectangle { anchors.left: parent.left; anchors.top: parent.top; anchors.bottom: parent.bottom; anchors.margins: Theme.dp(6); width: Theme.dp(card.advanced ? 5 : 3); radius: 2; color: card.chosen ? card.perk.tint : Theme.divider }
    }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        AbstractButton {
            id: select
            objectName: "athletePerk-"+card.perk.key
            Layout.fillWidth: true
            implicitHeight: contentItem.implicitHeight; padding: 0
            checked: card.chosen
            Accessible.name: card.perk.name
            Accessible.description: card.perk.description
            Accessible.role: Accessible.CheckBox; Accessible.checked: checked
            background: Rectangle { color: "transparent"; radius: 6; border.width: select.activeFocus ? 2 : 0; border.color: Theme.accent }
            contentItem: RowLayout {
                spacing: Theme.dp(10)
                Rectangle {
                    Layout.preferredWidth: Theme.dp(card.dense ? 34 : 43); Layout.preferredHeight: width
                    radius: Theme.dp(8); color: card.perk.tint
                    Image { anchors.centerIn: parent; width: parent.width*.7; height: width; source: "icons/athlete/"+card.perk.key+".svg"; sourceSize.width: 64; sourceSize.height: 64; Accessible.ignored: true; scale: card.chosen ? 1 : .87; Behavior on scale { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } } }
                }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: Theme.dp(3)
                    Label { text: card.perk.name; Layout.fillWidth: true; wrapMode: Text.WordWrap; color: card.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                    Label { text: card.perk.description; Layout.fillWidth: true; wrapMode: Text.WordWrap; color: card.chosen ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption }
                }
                Label { visible: card.chosen; text: card.advanced ? "II" : "I"; color: card.advanced ? "#f5cf84" : Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.fontSize(28); font.weight: Font.Bold }
            }
            onClicked: card.backend.setPerk(card.perk.pair,card.perk.side,card.chosen ? Math.abs(card.selected) : 1,true)
        }
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(4)
            Label { text: card.advanced ? qsTr("POZIOM II") : card.chosen ? qsTr("WYBRANY") : qsTr("WYBIERZ PERK"); color: card.advanced ? "#f5cf84" : card.chosen ? Theme.headerMuted : Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Repeater {
                model: 2
                SButton {
                    required property int index
                    objectName: "athletePerkLevel-"+card.perk.key+"-"+(index+1)
                    text: index===0 ? "I" : "II"
                    implicitWidth: Theme.dp(40); implicitHeight: Theme.dp(29)
                    font.family: Theme.mono; font.bold: true
                    Accessible.name: qsTr("%1 · poziom %2").arg(card.perk.name).arg(index+1)
                    checked: card.chosen && Math.abs(card.selected)===index+1
                    foreground: checked ? (index===1 ? "#382408" : Theme.accentText) : card.chosen ? Theme.headerMuted : Theme.muted
                    background: Rectangle { color: parent.checked ? (parent.index===1 ? "#f5cf84" : Theme.accent) : "transparent"; radius: Theme.dp(4); border.color: parent.activeFocus ? Theme.accent : card.chosen ? "#577083" : Theme.divider; border.width: 1 }
                    onClicked: card.backend.setPerk(card.perk.pair,card.perk.side,index+1,false)
                }
            }
        }
    }
}
