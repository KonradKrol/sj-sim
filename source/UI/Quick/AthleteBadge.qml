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
    property bool hill: false
    property string prefix: "athletePerk"
    property real perkPulse: 1
    property real burstProgress: 1
    SequentialAnimation {
        id: selectionFeedback
        NumberAnimation { target: badge; property: "perkPulse"; to: 1.14; duration: Theme.motion/2; easing.type: Easing.OutCubic }
        NumberAnimation { target: badge; property: "perkPulse"; to: 1; duration: Theme.motion; easing.type: Easing.OutBack }
    }
    NumberAnimation { id: powerBurst; target: badge; property: "burstProgress"; from: 0; to: 1; duration: Theme.motion*4; easing.type: Easing.OutCubic }
    function animateSelection() {
        if(!Theme.motion)return
        selectionFeedback.restart()
        if(empowered)powerBurst.restart()
    }
    Connections { target: Theme; function onMotionChanged(){if(!Theme.motion){selectionFeedback.stop();powerBurst.stop();badge.perkPulse=1;badge.burstProgress=1}} }
    readonly property int selected: Number((backend.selectedPerks || [])[perk.pair] || 0)
    readonly property bool chosen: selected*perk.side>0
    readonly property int tier: chosen ? Math.abs(selected) : 0
    readonly property bool enhanced: managedLevel && tier>=2
    readonly property bool empowered: tier===3
    onEmpoweredChanged: if(!empowered){powerBurst.stop();burstProgress=1}
    readonly property real artSize: Theme.dp(ribbon ? 34 : 70)
    readonly property bool inlineLevels: ribbon && width>=strengths.implicitWidth+artSize+Math.max(100,Theme.dp(160))+Theme.dp(32)
    readonly property real headerHeight: ribbon ? Math.max(artSize+Theme.dp(8),name.implicitHeight+Theme.dp(10)) : artSize+name.implicitHeight+Theme.dp(8)
    implicitWidth: Theme.dp(ribbon ? 250 : 160)
    implicitHeight: (inlineLevels ? Math.max(headerHeight,strengths.implicitHeight) : headerHeight+strengths.implicitHeight)+Theme.dp(8)
    Rectangle {
        anchors.fill: parent
        color: badge.empowered ? "#241237" : badge.ribbon ? (badge.chosen ? (badge.managedLevel ? Theme.stadium : badge.perk.color) : badge.design.surface) : "transparent"
        radius: badge.ribbon ? Theme.dp(2) : Theme.dp(10)
        border.width: badge.empowered || badge.enhanced ? 2 : pick.activeFocus ? 2 : badge.ribbon ? 1 : 0
        border.color: pick.activeFocus ? badge.design.accent : badge.enhanced ? "#ffb941" : badge.chosen ? (badge.managedLevel ? "#96c2ff" : badge.perk.color) : badge.design.line
        Rectangle { visible: badge.enhanced; anchors.fill: parent; anchors.margins: Theme.dp(4); color: "transparent"; border.width: 1; border.color: "#a98136"; radius: Theme.dp(1) }
        Rectangle { visible: badge.ribbon && badge.chosen && !badge.managedLevel; anchors.right: parent.right; width: Theme.dp(5); height: parent.height; color: badge.tier===2 ? "#f1cf87" : "#ffffff" }
    }
    AbstractButton {
        id: pick
        objectName: badge.prefix+"-"+badge.perk.key
        width: badge.inlineLevels ? badge.width-strengths.width-Theme.dp(12) : badge.width
        height: badge.headerHeight
        hoverEnabled: true
        property bool suppressed: false
        property bool helpRequested: false
        checked: badge.chosen
        Accessible.role: Accessible.CheckBox; Accessible.checked: checked
        Accessible.name: badge.perk.name+(badge.chosen ? " · "+strengths.levelNames[badge.tier-1] : "")
        Accessible.description: badge.perk.description
        onHoveredChanged: if(!hovered){suppressed=false;helpRequested=false}
        onActiveFocusChanged: if(!activeFocus)helpRequested=false
        Keys.onPressed: {if(event.key===Qt.Key_F1){helpRequested=true;event.accepted=true}else event.accepted=false}
        background: null
        onClicked: {suppressed=true;helpRequested=false;badge.backend.setPerk(badge.perk.pair,badge.perk.side,badge.chosen ? badge.tier : 1,true);if(badge.chosen)badge.animateSelection()}
        ToolTip {
            visible: pick.helpRequested || pick.hovered && !pick.suppressed && !pick.down
            delay: pick.helpRequested ? 0 : 700; timeout: 6000
            text: badge.perk.description
            width: Math.min(320,pick.Window.window ? pick.Window.window.width-24 : 280)
        }
    }
    AthletePerkArt {
        id: art
        width: badge.artSize; height: width
        x: badge.ribbon ? Theme.dp(4) : (badge.width-width)/2
        y: badge.ribbon ? Theme.dp(4) : 0
        family: badge.family; perkKey: badge.perk.key; tier: badge.tier; hill: badge.hill
        sourceSize.width: 192; sourceSize.height: 192
        scale: pick.down ? .9 : badge.chosen ? 1.06 : pick.hovered ? 1.02 : .95
        transform: Scale { origin.x: art.width/2; origin.y: art.height/2; xScale: badge.perkPulse; yScale: badge.perkPulse }
        Behavior on scale { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutBack } }
        Accessible.ignored: true
    }
    Item {
        x: art.x+art.width/2; y: art.y+art.height/2
        visible: badge.burstProgress<1
        opacity: 1-badge.burstProgress
        rotation: badge.burstProgress*30
        Accessible.ignored: true
        Rectangle {
            width: art.width*(1+badge.burstProgress*2); height: width
            x: -width/2; y: -height/2
            radius: width/2; color: "transparent"; border.width: Theme.dp(2); border.color: "#ffcd63"
        }
        Repeater {
            model: 8
            Rectangle {
                required property int index
                readonly property real angle: index*Math.PI/4
                readonly property real distance: art.width*(.5+badge.burstProgress)
                x: Math.cos(angle)*distance-width/2; y: Math.sin(angle)*distance-height/2
                width: Theme.dp(4); height: Theme.dp(12)*(1-badge.burstProgress)+1
                rotation: index*45+90; radius: width/2; color: "#ffdf9d"
            }
        }
    }
    Label {
        id: name
        x: badge.ribbon ? badge.artSize+Theme.dp(10) : Theme.dp(2)
        y: badge.ribbon ? (badge.headerHeight-height)/2 : badge.artSize+Theme.dp(2)
        width: badge.ribbon ? Math.max(0,badge.width-x-Theme.dp(10)-(badge.inlineLevels ? strengths.implicitWidth+Theme.dp(8) : 0)) : badge.width-Theme.dp(4)
        text: badge.perk.name
        color: badge.empowered ? "#fff1d2" : badge.ribbon && badge.chosen ? "#ffffff" : badge.chosen ? badge.design.ink : badge.design.muted
        font.family: badge.ribbon ? Theme.bodyFamily : Theme.displayFamily
        font.pointSize: badge.ribbon ? Theme.body : Theme.section
        font.weight: badge.chosen ? Font.Bold : Font.Medium
        horizontalAlignment: badge.ribbon ? Text.AlignLeft : Text.AlignHCenter
        wrapMode: Text.WordWrap
    }
    PerkStrengthSelector {
        id: strengths
        x: badge.inlineLevels ? badge.width-width-Theme.dp(6) : badge.ribbon ? Theme.dp(5) : (badge.width-width)/2
        y: badge.inlineLevels ? (badge.headerHeight-height)/2 : badge.headerHeight
        backend: badge.backend; perk: badge.perk; prefix: badge.prefix
        onApplied: function(level){if(level>0)badge.animateSelection()}
    }
}
