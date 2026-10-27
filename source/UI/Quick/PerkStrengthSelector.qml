import QtQuick 2.15
import QtQuick.Controls 2.15

Row {
    id: selector
    property var backend
    property var perk
    property string prefix: "athletePerk"
    property var levelNames: [qsTr("Zwykły"),qsTr("Wzmocniony"),qsTr("Potężny")]
    readonly property int selected: Number((backend.selectedPerks || [])[perk.pair] || 0)
    readonly property int tier: selected*perk.side>0 ? Math.abs(selected) : 0
    signal applied(int level)
    readonly property real cellSize: Math.max(28,Theme.dp(36))
    spacing: Theme.dp(4)
    Repeater {
        model: 3
        AbstractButton {
            id: strength
            required property int index
            objectName: selector.prefix+"-"+selector.perk.key+"-"+(index+1)
            hoverEnabled: true
            checkable: true
            checked: selector.tier===index+1
            width: selector.cellSize; height: width
            Accessible.role: Accessible.CheckBox
            Accessible.name: selector.perk.name+": "+selector.levelNames[index]
            Accessible.checked: checked
            contentItem: Item {
                Row {
                    anchors.centerIn: parent
                    height: 16; spacing: 3
                    Repeater {
                        model: 3
                        Rectangle {
                            required property int index
                            width: 3; height: 6+index*5; y: parent.height-height
                            radius: 1
                            color: index<=strength.index ? (strength.checked ? Theme.snow : Theme.accent) : (strength.checked ? Theme.track : Theme.divider)
                        }
                    }
                }
            }
            background: Rectangle { color: strength.checked ? Theme.stadium : strength.hovered ? Theme.selection : Theme.surface; radius: Theme.dp(4); border.width: strength.visualFocus ? 2 : 1; border.color: strength.visualFocus ? Theme.accent : Theme.divider; Behavior on color { ColorAnimation { duration: Theme.motion } } }
            ToolTip.visible: hovered || visualFocus
            ToolTip.text: selector.levelNames[index]
            ToolTip.delay: hovered ? 450 : 0
            ToolTip.timeout: 3000
            onClicked: {
                selector.backend.setPerk(selector.perk.pair,selector.perk.side,index+1,selector.tier===index+1)
                selector.applied(selector.tier)
            }
        }
    }
}
