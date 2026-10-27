import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: equipped
    property var backend
    property var design
    property string family: "medal"
    property bool vertical: false
    property bool framed: false
    AthletePerkCatalog { id: catalog }
    columns: vertical || width<Theme.dp(510) ? 1 : 3
    columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(5)
    Repeater {
        model: 3
        AbstractButton {
            id: slot
            required property int index
            readonly property int value: Number((equipped.backend.profile.perks || [])[index] || 0)
            readonly property var perk: catalog.selected(index,value)
            objectName: "athleteAssigned-"+index
            Layout.fillWidth: true; Layout.minimumWidth: 0
            implicitHeight: Math.max(Theme.dp(58),contentItem.implicitHeight+Theme.dp(12)); padding: Theme.dp(6)
            enabled: value!==0
            hoverEnabled: true
            Accessible.name: value ? qsTr("Usuń perk: %1").arg(perk.name) : qsTr("Brak perka · %1").arg(catalog.pairs[index])
            background: Rectangle { color: equipped.framed ? equipped.design.surface : "transparent"; radius: equipped.family==="coin" ? Theme.dp(28) : Theme.dp(4); border.width: slot.activeFocus ? 2 : equipped.framed ? 1 : 0; border.color: slot.activeFocus ? equipped.design.accent : equipped.design.line }
            contentItem: RowLayout {
                spacing: Theme.dp(7)
                Item {
                    Layout.preferredWidth: Theme.dp(42); Layout.preferredHeight: Theme.dp(44)
                    AthletePerkArt { visible: slot.value!==0; anchors.fill: parent; family: equipped.family; perkKey: slot.perk.key; tier: Math.abs(slot.value); sourceSize.width: 96; sourceSize.height: 96; Accessible.ignored: true }
                    Rectangle { visible: !slot.value; anchors.centerIn: parent; width: Theme.dp(28); height: width; radius: equipped.family==="coin" ? width/2 : 4; color: "transparent"; border.width: 1; border.color: equipped.design.line; Label { anchors.centerIn: parent; text: "+"; font.pointSize: Theme.section; color: equipped.design.muted } }
                }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                    Label { text: catalog.pairs[slot.index]; color: equipped.design.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
                    Label { text: slot.value ? slot.perk.name : "—"; color: equipped.design.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                }
                Label { visible: Math.abs(slot.value)>=2; text: Math.abs(slot.value)===3 ? "III" : "II"; color: equipped.design.accent; font.family: Theme.mono; font.pointSize: Theme.section; font.bold: true }
                Label { visible: slot.hovered || slot.activeFocus; text: "×"; color: equipped.design.muted; font.pointSize: Theme.section }
            }
            onClicked: equipped.backend.setPerk(index,value>0?1:-1,Math.abs(value),true)
            ToolTip.visible: hovered && !down
            ToolTip.text: qsTr("Usuń przypisany perk")
            ToolTip.delay: 700
        }
    }
}
