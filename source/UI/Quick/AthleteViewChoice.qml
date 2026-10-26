import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: choice
    property int view: 0
    property bool selected: false
    text: [qsTr("Karta"),qsTr("Profil"),qsTr("Studio")][view]
    implicitWidth: Math.max(Theme.dp(110),contentItem.implicitWidth+leftPadding+rightPadding); implicitHeight: Math.max(Theme.dp(43),contentItem.implicitHeight+topPadding+bottomPadding); padding: Theme.dp(7)
    Accessible.name: qsTr("Widok: %1").arg(text); Accessible.role: Accessible.RadioButton; Accessible.checked: selected
    background: Rectangle { color: choice.selected ? Theme.stadium : choice.hovered ? Theme.selection : Theme.surface; radius: Theme.dp(6); border.width: choice.selected || choice.activeFocus ? 2 : 1; border.color: choice.selected ? Theme.accent : Theme.divider }
    contentItem: RowLayout {
        spacing: Theme.dp(7)
        Item {
            Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(23)
            Rectangle { x: 0; y: 0; width: choice.view===0 ? parent.width*.4 : parent.width; height: choice.view===0 ? parent.height : choice.view===1 ? parent.height*.3 : parent.height*.42; radius: 2; color: choice.selected ? Theme.signal : Theme.accent }
            Rectangle { x: choice.view===0 ? parent.width*.53 : 0; y: choice.view===0 ? 0 : choice.view===1 ? parent.height*.44 : parent.height*.56; width: choice.view===0 ? parent.width*.47 : choice.view===2 ? parent.width*.45 : parent.width; height: choice.view===0 ? parent.height*.45 : choice.view===1 ? parent.height*.56 : parent.height*.44; radius: 2; color: choice.selected ? Theme.snow : Theme.outline }
            Rectangle { visible: choice.view!==1; x: parent.width*.55; y: parent.height*.57; width: parent.width*.45; height: parent.height*.43; radius: 2; color: choice.selected ? Theme.headerMuted : Theme.divider }
        }
        Label { text: choice.text; font.family: Theme.displayFamily; font.pointSize: Theme.section; color: choice.selected ? Theme.snow : Theme.ink }
    }
}
