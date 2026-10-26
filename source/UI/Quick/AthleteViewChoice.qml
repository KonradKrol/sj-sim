import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: choice
    property int view: 0
    property bool selected: false
    readonly property var accents: ["#d5f575","#a34429","#49a387","#f2cf8b","#bba4fb"]
    text: [qsTr("Emblematy"),qsTr("Naszywki"),qsTr("Żetony"),qsTr("Karta"),qsTr("Postać")][view]
    implicitWidth: contentItem.implicitWidth+leftPadding+rightPadding
    implicitHeight: Math.max(Theme.dp(44),contentItem.implicitHeight+topPadding+bottomPadding)
    padding: Theme.dp(8)
    Accessible.name: qsTr("Widok: %1").arg(text); Accessible.role: Accessible.RadioButton; Accessible.checked: selected
    background: Rectangle { color: choice.selected ? Theme.stadium : choice.hovered ? Theme.selection : Theme.surface; radius: Theme.dp(4); border.width: choice.activeFocus || choice.selected ? 2 : 1; border.color: choice.activeFocus ? Theme.accent : choice.selected ? choice.accents[choice.view] : Theme.divider }
    contentItem: RowLayout {
        spacing: Theme.dp(7)
        Item {
            Layout.preferredWidth: Theme.dp(23); Layout.preferredHeight: Theme.dp(26)
            Rectangle { anchors.fill: parent; color: ["#111d25","#f1ebdb","#e5f0e9","#243b60","#ebeaf7"][choice.view]; radius: choice.view===2 ? 8 : 1; border.width: 1; border.color: choice.accents[choice.view] }
            Rectangle { x: Theme.dp(4); y: Theme.dp(4); width: choice.view===3 ? Theme.dp(6) : Theme.dp(15); height: choice.view===4 ? Theme.dp(14) : Theme.dp(5); radius: choice.view===4 ? width/2 : 1; color: choice.accents[choice.view] }
            Rectangle { x: Theme.dp(4); y: Theme.dp(17); width: Theme.dp(6); height: Theme.dp(5); radius: choice.view===2 ? 3 : 0; color: choice.accents[choice.view] }
            Rectangle { x: Theme.dp(13); y: Theme.dp(17); width: Theme.dp(6); height: Theme.dp(5); radius: choice.view===2 ? 3 : 0; color: choice.accents[choice.view] }
        }
        Label { text: choice.text; font.family: Theme.displayFamily; font.pointSize: Theme.section; color: choice.selected ? Theme.snow : Theme.ink }
    }
}
