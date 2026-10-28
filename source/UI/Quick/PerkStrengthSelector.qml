import QtQuick 2.15
import QtQuick.Controls 2.15

Row {
    id: selector
    property var backend
    property var perk
    property string prefix: "athletePerk"
    readonly property var levelNames: [qsTr("Lekko"),qsTr("Wyraźnie"),qsTr("Skrajnie")]
    readonly property int selected: Number((backend.selectedPerks || [])[perk.pair] || 0)
    readonly property int tier: selected*perk.side>0 ? Math.abs(selected) : 0
    signal applied(int level)
    spacing: Theme.dp(4)
    Repeater {
        model: 3
        AbstractButton {
            id: strength
            required property int index
            objectName: selector.prefix+"-"+selector.perk.key+"-"+(index+1)
            hoverEnabled: true
            readonly property bool selectedTier: selector.tier===index+1
            checked: selectedTier
            leftPadding: Theme.dp(7); rightPadding: leftPadding
            topPadding: Theme.dp(6); bottomPadding: topPadding
            width: Math.max(Theme.dp(62),contentItem.implicitWidth+Theme.dp(14))
            height: Math.max(28,contentItem.implicitHeight+Theme.dp(12))
            Accessible.role: Accessible.CheckBox
            Accessible.name: selector.perk.name+": "+selector.levelNames[index]
            Accessible.checked: selectedTier
            Accessible.description: selectedTier ? qsTr("Kliknij ponownie, aby usunąć perk.") : qsTr("Wybierz tę siłę perka.")
            contentItem: Row {
                spacing: Theme.dp(5)
                Item {
                    width: Theme.dp(12); height: levelName.implicitHeight
                    Repeater {
                        model: 3
                        Rectangle {
                            required property int index
                            x: Theme.dp(index*4); y: (parent.height+Theme.dp(10))/2-height
                            width: Theme.dp(2); height: Theme.dp(4+index*3)
                            radius: 1
                            color: index<=strength.index ? (strength.selectedTier ? "#142e42" : Theme.accent) : Theme.divider
                        }
                    }
                }
                Label {
                    id: levelName
                    text: selector.levelNames[strength.index]
                    color: strength.selectedTier ? "#142e42" : Theme.ink
                    font.family: Theme.bodyFamily; font.pointSize: Theme.caption; font.bold: strength.selectedTier
                }
            }
            background: Rectangle {
                color: strength.selectedTier ? "#ffcd57" : strength.hovered ? Theme.selection : Theme.surface
                radius: Theme.dp(4)
                border.width: strength.selectedTier || strength.visualFocus ? 2 : 1
                border.color: strength.visualFocus ? Theme.accent : strength.selectedTier ? "#b37a14" : Theme.outline
            }
            onClicked: {
                selector.backend.setPerk(selector.perk.pair,selector.perk.side,index+1,selector.tier===index+1)
                selector.applied(selector.tier)
            }
        }
    }
}
