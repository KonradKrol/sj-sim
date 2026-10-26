import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: set
    property var backend
    AthletePerkCatalog { id: catalog }
    spacing: Theme.dp(4)
    Label { text: qsTr("Przypisane perki"); color: Theme.muted; font.pointSize: Theme.caption }
    Label { visible: (set.backend.profile.perks || []).every(function(v){return v===0}); text: qsTr("Brak"); color: Theme.muted; font.pointSize: Theme.body }
    Repeater {
        model: 3
        AbstractButton {
            id: assigned
            required property int index
            readonly property int value: Number((set.backend.profile.perks || [])[index] || 0)
            readonly property var perk: catalog.selected(index,value)
            objectName: "athleteAssigned-"+index
            visible: value!==0; hoverEnabled: true
            Layout.fillWidth: true; Layout.minimumWidth: 0
            implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding; padding: Theme.dp(4)
            Accessible.name: qsTr("Usuń przypisany perk: %1").arg(perk.name)
            background: Rectangle { color: assigned.hovered ? Theme.selection : "transparent"; radius: Theme.dp(5); border.width: assigned.activeFocus ? 2 : 0; border.color: Theme.accent }
            contentItem: RowLayout {
                spacing: Theme.dp(6)
                Image { source: "icons/athlete/patch-"+assigned.perk.key+"-"+Math.abs(assigned.value)+".svg"; sourceSize.width: 96; sourceSize.height: 96; Layout.preferredWidth: Theme.dp(30); Layout.preferredHeight: Theme.dp(33); Accessible.ignored: true }
                Label { text: assigned.perk.name; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap; color: Theme.ink; font.pointSize: Theme.body }
                Label { visible: Math.abs(assigned.value)===2; text: "II"; color: Theme.warning; font.family: Theme.mono; font.bold: true; font.pointSize: Theme.body }
                Label { text: "×"; color: Theme.muted; font.pointSize: Theme.section }
            }
            onClicked: set.backend.setPerk(index,value>0?1:-1,Math.abs(value),true)
            ToolTip.visible: hovered && !down
            ToolTip.text: qsTr("Usuń perk")
            ToolTip.delay: 700
        }
    }
}
