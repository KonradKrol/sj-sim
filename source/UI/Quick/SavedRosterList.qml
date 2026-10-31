import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ListView {
    id: list
    property var backend
    property bool selected: false
    property bool grouped: false
    readonly property var rowsModel: selected ? (grouped ? backend.groupedRows : backend.ordered) : (grouped ? backend.groupedLibrary : backend.library)
    objectName: list.selected ? "savedRosterSelected" : "savedRosterCandidates"
    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
    model: list.rowsModel; spacing: Theme.dp(3); clip: true
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {}
    section.property: list.grouped ? "countryCode" : ""
    section.delegate: Label {
        required property string section
        width: list.width; text: section; color: Theme.muted
        font.pointSize: Theme.caption; font.bold: true
        topPadding: Theme.dp(9); bottomPadding: Theme.dp(4)
    }
    delegate: Item {
        id: row
        required property var model
        width: list.width; height: Math.max(36,Theme.dp(48),Theme.bodyMetrics.height+16)
        readonly property bool chosen: !!model.included
        readonly property bool marked: !!model.marked
        Rectangle { anchors.fill: parent; radius: Theme.dp(4); color: row.marked ? Theme.selection : Theme.surface; border.color: row.marked ? Theme.accent : Theme.divider }
        RowLayout {
            anchors.fill: parent; anchors.margins: Theme.dp(6); spacing: Theme.dp(7)
            SavedRosterGrip { backend: list.backend; athlete: row.model; selected: list.selected; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(28) }
            SCheck {
                text: ""; checked: row.marked
                enabled: list.selected || !row.chosen
                Accessible.name: row.model.athleteName
                Layout.preferredWidth: Math.max(28,Theme.dp(28)); Layout.minimumWidth: 0; Layout.preferredHeight: row.height-Theme.dp(12); Layout.minimumHeight: 0
                onClicked: if(list.selected)list.backend.mark(row.model.athleteId,Qt.ControlModifier);else list.backend.selectCandidate(row.model.athleteId)
            }
            Image { source: row.model.flagSource || ""; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(18); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
            Label { text: row.model.athleteName; color: !list.selected && row.chosen ? Theme.muted : Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            Label { text: row.model.countryCode; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
            SButton {
                text: list.selected ? "−" : row.chosen ? "✓" : "+"
                enabled: list.selected || !row.chosen; quiet: true
                Layout.preferredWidth: Math.max(28,Theme.dp(30)); Layout.minimumWidth: 0; Layout.preferredHeight: row.height-Theme.dp(12); Layout.minimumHeight: 0
                Accessible.name: list.selected ? qsTr("Usuń %1 z listy").arg(row.model.athleteName) : qsTr("Dodaj %1 do listy").arg(row.model.athleteName)
                onClicked: if(list.selected)list.backend.toggle(row.model.athleteId);else list.backend.include(row.model.athleteId)
            }
        }
    }
    DropArea {
        anchors.fill: parent
        keys: ["saved-roster"]
        onPositionChanged: {
            var edge=Theme.dp(35)
            if(drag.y<edge)list.contentY=Math.max(list.originY,list.contentY-Theme.dp(20))
            else if(drag.y>height-edge)list.contentY=Math.min(list.originY+Math.max(0,list.contentHeight-list.height),list.contentY+Theme.dp(20))
        }
        onDropped: {
            var source=drop.source
            if(!source || source.backend!==list.backend)return
            var id=source.athlete.athleteId
            var backend=list.backend, receiving=list.selected, sending=source.selected
            var targetId="", at=list.indexAt(drop.x,drop.y+list.contentY)
            if(receiving && sending && !list.grouped && at>=0)targetId=list.rowsModel.get(at).athleteId
            Qt.callLater(function(){
                if(!backend)return
                if(receiving){if(!sending)backend.include(id);else if(targetId.length)backend.moveBefore(id,targetId)}
                else if(sending)backend.toggle(id)
            })
            drop.acceptProposedAction()
        }
    }
    EmptyState {
        anchors.fill: parent; visible: list.count===0; compact: parent.height<180
        title: list.selected ? qsTr("Lista jest pusta") : qsTr("Brak pasujących zawodników")
        description: list.selected ? qsTr("Pusta lista oznacza wszystkich zawodników. Dodaj wybranych, aby ograniczyć skład.") : qsTr("Zmień nazwisko lub kraj, aby zobaczyć innych zawodników.")
    }
    Keys.onDeletePressed: if(list.selected){list.backend.removeMarked();event.accepted=true}
}
