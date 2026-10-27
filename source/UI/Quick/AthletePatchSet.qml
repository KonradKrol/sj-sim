import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: set
    property var backend
    AthletePerkCatalog { id: catalog }
    spacing: Theme.dp(4)
    Label { text: qsTr("Przypisane perki"); color: Theme.muted; font.pointSize: Theme.caption }
    Label { visible: (set.backend.selectedPerks || []).every(function(v){return v===0}); text: qsTr("Brak"); color: Theme.muted; font.pointSize: Theme.body }
    Repeater {
        model: 3
        AbstractButton {
            id: assigned
            required property int index
            readonly property int value: Number((set.backend.selectedPerks || [])[index] || 0)
            property int displayedValue: 0
            onValueChanged: if(value!==0)displayedValue=value
            Component.onCompleted: displayedValue=value
            property real reveal: value!==0 ? 1 : 0
            Behavior on reveal { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
            readonly property bool enhanced: Math.abs(value)>=2
            readonly property bool empowered: Math.abs(value)===3
            readonly property var perk: catalog.selected(index,displayedValue)
            objectName: "athleteAssigned-"+index
            visible: reveal>0; enabled: value!==0; hoverEnabled: true; opacity: reveal; clip: true
            Layout.fillWidth: true; Layout.minimumWidth: 0
            implicitHeight: (Math.max(Theme.dp(33),nameMeasure.implicitHeight,removeLabel.implicitHeight)+topPadding+bottomPadding)*reveal; padding: Theme.dp(4)
            Accessible.name: qsTr("Usuń przypisany perk: %1").arg(perk.name)
            Text {
                id: nameMeasure
                visible: false
                width: Math.max(1,(set.width>0 ? set.width : Theme.dp(250))-Theme.dp(50)-removeLabel.implicitWidth)
                text: nameLabel.text; font.family: Theme.bodyFamily; font.pointSize: Theme.body; wrapMode: Text.WordWrap
            }
            background: Rectangle { color: assigned.empowered ? "#241237" : assigned.enhanced ? "#ffcd57" : assigned.hovered ? Theme.selection : "transparent"; radius: Theme.dp(5); border.width: assigned.enhanced || assigned.activeFocus ? 2 : 0; border.color: assigned.empowered ? "#ffb941" : assigned.enhanced ? "#64400a" : Theme.accent }
            contentItem: RowLayout {
                spacing: Theme.dp(6)
                AthletePerkArt { family: "patch"; perkKey: assigned.perk.key; tier: Math.abs(assigned.value); sourceSize.width: 96; sourceSize.height: 96; Layout.preferredWidth: Theme.dp(30); Layout.preferredHeight: Theme.dp(33); Accessible.ignored: true }
                Label { id: nameLabel; text: qsTr("%1 · %2").arg(assigned.perk.name).arg(["","I","II","III"][Math.abs(assigned.value)]); Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap; color: assigned.empowered ? "#fff1d2" : assigned.enhanced ? "#30210c" : Theme.ink; font.family: Theme.bodyFamily; font.pointSize: Theme.body }
                Label { id: removeLabel; text: "×"; color: assigned.empowered ? "#fff1d2" : assigned.enhanced ? "#30210c" : Theme.muted; font.pointSize: Theme.section }
            }
            onClicked: set.backend.setPerk(index,value>0?1:-1,Math.abs(value),true)
            ToolTip.visible: hovered && !down
            ToolTip.text: qsTr("Usuń perk")
            ToolTip.delay: 700
        }
    }
}
