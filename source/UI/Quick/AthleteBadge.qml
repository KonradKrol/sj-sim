import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Item {
    id: badge
    property var backend
    property var perk
    property var design
    property string family: "medal"
    property bool ribbon: false
    property bool managedLevel: false
    readonly property int selected: Number((backend.profile.perks || [])[perk.pair] || 0)
    readonly property bool chosen: selected*perk.side>0
    readonly property int tier: chosen ? Math.abs(selected) : 0
    readonly property real artSize: Theme.dp(ribbon ? 55 : 78)
    implicitWidth: Theme.dp(ribbon ? 250 : 160)
    implicitHeight: ribbon ? Math.max(artSize,name.implicitHeight+Theme.dp(16)) : artSize+name.implicitHeight+Theme.dp(8)
    AbstractButton {
        id: pick
        objectName: "athletePerk-"+badge.perk.key
        anchors.fill: parent
        hoverEnabled: true
        property bool suppressed: false
        property bool helpRequested: false
        checked: badge.chosen
        Accessible.role: Accessible.CheckBox; Accessible.checked: checked
        Accessible.name: qsTr("%1%2").arg(badge.perk.name).arg(badge.chosen ? qsTr(" · poziom %1").arg(badge.tier) : "")
        Accessible.description: badge.perk.description
        onHoveredChanged: if(!hovered){suppressed=false;helpRequested=false}
        onActiveFocusChanged: if(!activeFocus)helpRequested=false
        Keys.onPressed: {if(event.key===Qt.Key_F1){helpRequested=true;event.accepted=true}else event.accepted=false}
        background: Rectangle {
            color: badge.ribbon ? (badge.chosen ? (badge.managedLevel ? Theme.stadium : badge.perk.color) : badge.design.surface) : "transparent"
            radius: badge.ribbon ? Theme.dp(2) : Theme.dp(10)
            border.width: pick.activeFocus ? 2 : badge.ribbon ? 1 : 0
            border.color: pick.activeFocus ? badge.design.accent : badge.chosen ? (badge.managedLevel ? Theme.accent : badge.perk.color) : badge.design.line
            Rectangle { visible: badge.ribbon && badge.chosen && !badge.managedLevel; anchors.right: parent.right; width: Theme.dp(5); height: parent.height; color: badge.tier===2 ? "#f1cf87" : "#ffffff" }
        }
        onReleased: {suppressed=true;helpRequested=false;badge.backend.setPerk(badge.perk.pair,badge.perk.side,badge.chosen ? badge.tier : 1,true)}
        ToolTip {
            visible: pick.helpRequested || pick.hovered && !pick.suppressed && !pick.down
            delay: pick.helpRequested ? 0 : 700; timeout: 6000
            text: badge.perk.description
            width: Math.min(320,pick.Window.window ? pick.Window.window.width-24 : 280)
        }
    }
    Image {
        id: art
        width: badge.artSize; height: width
        x: badge.ribbon ? Theme.dp(4) : (badge.width-width)/2
        y: 0
        source: "icons/athlete/"+badge.family+"-"+badge.perk.key+"-"+badge.tier+".svg"
        sourceSize.width: 192; sourceSize.height: 192
        scale: pick.down ? .9 : badge.chosen ? 1.06 : pick.hovered ? 1.02 : .95
        Behavior on scale { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutBack } }
        Accessible.ignored: true
    }
    Label {
        id: name
        x: badge.ribbon ? badge.artSize+Theme.dp(10) : Theme.dp(2)
        y: badge.ribbon ? (badge.height-height)/2 : badge.artSize+Theme.dp(2)
        width: badge.ribbon ? Math.max(0,badge.width-x-(badge.chosen ? level.width+Theme.dp(16) : Theme.dp(12))) : badge.width-Theme.dp(4)
        text: badge.perk.name
        color: badge.ribbon && badge.chosen ? "#ffffff" : badge.chosen ? badge.design.ink : badge.design.muted
        font.family: badge.ribbon ? Theme.bodyFamily : Theme.displayFamily
        font.pointSize: badge.ribbon ? Theme.body : Theme.section
        font.weight: badge.chosen ? Font.Bold : Font.Medium
        horizontalAlignment: badge.ribbon ? Text.AlignLeft : Text.AlignHCenter
        wrapMode: Text.WordWrap
    }
    AbstractButton {
        id: level
        objectName: "athletePerkLevel-"+badge.perk.key
        visible: badge.chosen
        x: badge.ribbon ? badge.width-width-Theme.dp(10) : Math.min(badge.width-width,art.x+art.width-Theme.dp(14))
        y: badge.ribbon ? (badge.height-height)/2 : Theme.dp(8)
        width: Math.max(Theme.dp(badge.managedLevel ? 90 : 31),levelLabel.implicitWidth+Theme.dp(12)); height: Math.max(Theme.dp(badge.managedLevel ? 34 : 30),levelLabel.implicitHeight+Theme.dp(7))
        hoverEnabled: true
        Accessible.name: badge.tier===1 ? qsTr("Wzmocnij %1 do poziomu II").arg(badge.perk.name) : qsTr("Zmień %1 na poziom I").arg(badge.perk.name)
        background: Rectangle { color: badge.tier===2 ? "#f1cf87" : "#eaf1f0"; radius: badge.managedLevel ? Theme.dp(4) : badge.ribbon ? 2 : height/2; border.width: level.activeFocus ? 3 : 1; border.color: level.activeFocus ? badge.design.accent : "#23363c" }
        contentItem: Label { id: levelLabel; text: badge.managedLevel ? (badge.tier===2 ? qsTr("Poziom II") : qsTr("Wzmocnij")) : badge.tier===2 ? "II" : "I ↑"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; color: "#293128"; font.family: Theme.mono; font.pointSize: Theme.caption; font.bold: true }
        onReleased: badge.backend.setPerk(badge.perk.pair,badge.perk.side,badge.tier===1 ? 2 : 1,false)
        ToolTip.visible: hovered && !down
        ToolTip.text: Accessible.name
        ToolTip.delay: 600
    }
}
