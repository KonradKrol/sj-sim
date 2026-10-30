import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: selector
    property real availableWidth: Infinity
    readonly property int columns: availableWidth>=buttonWidth*4+spacing*3 ? 4 : availableWidth>=buttonWidth*2+spacing ? 2 : 1
    readonly property real buttonHeight: Math.max(28,labelMetrics.height+Theme.dp(12))
    implicitWidth: columns*buttonWidth+(columns-1)*spacing
    implicitHeight: Math.ceil(4/columns)*buttonHeight+(Math.ceil(4/columns)-1)*spacing
    width: implicitWidth
    height: implicitHeight
    property var backend
    property var perk
    property string prefix: "athletePerk"
    readonly property var levelNames: [qsTr("Lekko"),qsTr("Wyraźnie"),qsTr("Mocno"),qsTr("Skrajnie")]
    readonly property real buttonWidth: Math.max(Theme.dp(62),Math.max(labelMetrics.advanceWidth(levelNames[0]),labelMetrics.advanceWidth(levelNames[1]),labelMetrics.advanceWidth(levelNames[2]),labelMetrics.advanceWidth(levelNames[3]))+Theme.dp(35))
    FontMetrics { id: labelMetrics; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; font.bold: true }
    readonly property int selected: Number((backend.selectedPerks || [])[perk.pair] || 0)
    readonly property int tier: selected*perk.side>0 ? Math.abs(selected) : 0
    signal applied(int level)
    readonly property real spacing: Theme.dp(4)
    Repeater {
        model: 4
        AbstractButton {
            id: strength
            required property int index
            objectName: selector.prefix+"-"+selector.perk.key+"-"+(index+1)
            hoverEnabled: true
            readonly property bool selectedTier: selector.tier===index+1
            checked: selectedTier
            leftPadding: Theme.dp(7); rightPadding: leftPadding
            topPadding: Theme.dp(6); bottomPadding: topPadding
            width: selector.buttonWidth
            height: selector.buttonHeight
            x: (index%selector.columns)*(selector.buttonWidth+selector.spacing)
            y: Math.floor(index/selector.columns)*(selector.buttonHeight+selector.spacing)
            Accessible.role: Accessible.CheckBox
            Accessible.name: selector.perk.name+": "+selector.levelNames[index]
            Accessible.checked: selectedTier
            Accessible.description: selectedTier ? qsTr("Kliknij ponownie, aby usunąć perk.") : qsTr("Wybierz tę siłę perka.")
            contentItem: Row {
                spacing: Theme.dp(5)
                Item {
                    width: Theme.dp(16); height: levelName.implicitHeight
                    Repeater {
                        model: 4
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
                color: strength.selectedTier ? (strength.index===1 ? "#e4d4ad" : strength.index===2 ? "#efc66b" : "#ffcd57") : strength.hovered ? Theme.selection : Theme.surface
                radius: Theme.dp(4)
                border.width: strength.selectedTier || strength.visualFocus ? 2 : 1
                border.color: strength.visualFocus ? Theme.accent : strength.selectedTier ? (strength.index===1 ? "#8b7853" : "#b37a14") : Theme.outline
            }
            onClicked: {
                selector.backend.setPerk(selector.perk.pair,selector.perk.side,index+1,selector.tier===index+1)
                selector.applied(selector.tier)
            }
        }
    }
}
