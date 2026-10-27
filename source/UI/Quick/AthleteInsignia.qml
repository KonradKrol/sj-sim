import QtQuick 2.15
import QtQuick.Controls 2.15

Flow {
    id: marks
    property var backend
    property string family: "medal"
    property real badgeSize: Theme.dp(48)
    AthletePerkCatalog { id: catalog }
    spacing: Theme.dp(3)
    Repeater {
        model: 3
        AbstractButton {
            id: mark
            required property int index
            readonly property int value: Number((marks.backend.selectedPerks || [])[index] || 0)
            readonly property var perk: catalog.selected(index,value)
            objectName: "athleteInsignia-"+index
            width: marks.badgeSize; height: width
            visible: value!==0
            scale: value!==0 ? 1 : .65
            Behavior on scale { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutBack } }
            hoverEnabled: true
            Accessible.name: qsTr("%1 · poziom %2").arg(perk.name).arg(Math.abs(value))
            contentItem: AthletePerkArt { family: marks.family; perkKey: mark.perk.key; tier: Math.abs(mark.value); sourceSize.width: 120; sourceSize.height: 120; Accessible.ignored: true }
            background: Rectangle { color: "transparent"; border.width: mark.activeFocus ? 2 : 0; border.color: Theme.accent; radius: 4 }
            onClicked: marks.backend.setPerk(index,value>0?1:-1,Math.abs(value),true)
            ToolTip.visible: hovered && !down
            ToolTip.text: Accessible.name
            ToolTip.delay: 700
        }
    }
}
