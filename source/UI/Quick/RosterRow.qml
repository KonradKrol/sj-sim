import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: row
    property var backend
    property string athleteId
    property string athleteName
    property string countryCode
    property string flagSource: ""
    property bool included: false
    property bool marked: false
    property int position: 0
    property bool activeSlot: true
    property bool ordered: false
    property var viewport
    signal moveRequested()
    implicitHeight: Theme.compact ? Math.max(30,Theme.bodyMetrics.height+(ordered && backend.activeLimit > 0 ? Theme.captionMetrics.height : 0)+6) : Math.max(backend.activeLimit > 0 ? 56 : 44,Theme.bodyMetrics.height+16)
    objectName: (ordered ? "roster-entry-" : "roster-candidate-") + athleteId
    activeFocusOnTab: true
    Accessible.role: ordered ? Accessible.ListItem : Accessible.CheckBox
    Accessible.name: ordered ? qsTr("%1. %2, %3").arg(position).arg(athleteName).arg(countryCode) : athleteName + ", " + countryCode
    Accessible.selected: marked
    Accessible.checked: included
    Keys.onSpacePressed: ordered ? backend.mark(athleteId,event.modifiers) : backend.toggle(athleteId)
    Keys.onDeletePressed: if(ordered && backend.membership){backend.mark(athleteId);backend.removeMarked()}
    Keys.onPressed: {
        if(ordered && (event.modifiers & Qt.ControlModifier) && (event.key === Qt.Key_Up || event.key === Qt.Key_Down)){
            if(!marked)backend.mark(athleteId)
            backend.step(event.key === Qt.Key_Up ? -1 : 1);event.accepted=true
        }
    }
    Rectangle { anchors.fill: parent; radius: 6; color: row.marked ? Theme.selection : row.activeSlot ? Theme.surface : Theme.raised; border.color: row.activeFocus ? Theme.accent : Theme.divider; border.width: row.activeFocus ? 2 : 1 }
    MouseArea { anchors.fill: parent; onClicked: {row.forceActiveFocus();row.ordered ? row.backend.mark(row.athleteId,mouse.modifiers) : row.backend.toggle(row.athleteId)} }
    RowLayout {
        anchors.fill: parent; anchors.leftMargin: 6; anchors.rightMargin: 8
        spacing: Theme.compact ? 4 : 8
        SCheck { visible: !row.ordered; checked: row.included; text: ""; Layout.preferredWidth: 30; Accessible.name: row.athleteName; onClicked: row.backend.toggle(row.athleteId) }
        SButton {
            id: grip
            objectName: row.ordered ? "roster-grip-" + row.athleteId : ""
            visible: row.ordered; text: "⠿"; quiet: true
            Layout.preferredWidth: 34; Layout.preferredHeight: Theme.compact ? 28 : 40
            Accessible.name: qsTr("Przenieś %1").arg(row.athleteName)
            hint: row.backend.membership ? qsTr("Przeciągnij lub wybierz pozycję") : qsTr("Przeciągnij na zawodnika, aby zamienić miejsca")
            onClicked: {if(!row.marked)row.backend.mark(row.athleteId);row.moveRequested()}
        }
        Label { visible: row.ordered; text: row.position; font.family: Theme.mono; font.pointSize: Theme.caption; color: Theme.muted; Layout.preferredWidth: 30; horizontalAlignment: Text.AlignRight }
        ColumnLayout {
            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
            Label { text: row.athleteName; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: row.ordered && row.backend.activeLimit > 0; text: row.activeSlot ? qsTr("Skład") : qsTr("Rezerwa"); color: row.activeSlot ? Theme.muted : Theme.warning; font.pointSize: Theme.caption }
        }
        Image { source: row.flagSource;sourceSize.width: 32;sourceSize.height: 24;Layout.preferredWidth: 28;Layout.preferredHeight: 21;fillMode: Image.PreserveAspectFit;visible: source.toString().length>0;Accessible.ignored: true }
        Label { text: row.countryCode; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        SButton { visible: row.ordered && row.backend.membership; text: "×"; quiet: true; Layout.preferredWidth: 32; Accessible.name: qsTr("Usuń %1 z listy").arg(row.athleteName); onClicked: row.backend.toggle(row.athleteId) }
    }
    DropArea {
        id: dropTarget
        anchors.fill: parent
        enabled: row.ordered
        keys: ["sj-roster"]
        property bool after: false
        onEntered: after=drag.y>height/2
        onPositionChanged: after=drag.y>height/2
        onDropped: function(event) {
            if(event.source && event.source.backend === row.backend){row.backend.moveBefore(event.source.athleteId,row.athleteId,dropTarget.after);event.acceptProposedAction()}
        }
    }
    Rectangle { visible: dropTarget.containsDrag; x: 0; y: dropTarget.after ? parent.height-2 : 0; width: parent.width; height: 3; color: Theme.accent; z: 2 }
}
