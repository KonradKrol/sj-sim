import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: pane
    property var backend
    property bool selected: false
    readonly property var rows: selected ? (backend.grouped ? backend.groupedRows : backend.ordered) : backend.library
    padding: Theme.dp(10)
    background: Rectangle { color: pane.selected ? Theme.surface : Theme.raised; radius: Theme.radius; border.color: pane.selected ? Theme.outline : Theme.divider }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        RowLayout {
            Layout.fillWidth: true
            Label { text: pane.selected ? qsTr("Wybrani zawodnicy") : qsTr("Wszyscy zawodnicy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { text: pane.selected ? pane.backend.count : pane.backend.library.count; color: Theme.accent; font.family: Theme.mono; font.pointSize: Theme.section }
        }
        SField {
            Layout.fillWidth: true; Layout.minimumWidth: 0
            objectName: pane.selected ? "rosterOrderSearch" : "rosterSearch"
            text: pane.selected ? pane.backend.orderQuery : pane.backend.query
            placeholderText: qsTr("Szukaj nazwiska lub kraju")
            Accessible.name: placeholderText
            onTextEdited: if(pane.selected)pane.backend.orderQuery=text;else pane.backend.query=text
        }
        ListView {
            id: list
            objectName: pane.selected ? "savedRosterSelected" : "savedRosterCandidates"
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            model: pane.rows; spacing: Theme.dp(3); clip: true
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}
            section.property: pane.backend.grouped ? "countryCode" : ""
            section.delegate: Label {
                required property string section
                width: list.width; text: section; color: Theme.muted
                font.pointSize: Theme.caption; font.bold: true
                topPadding: Theme.dp(9); bottomPadding: Theme.dp(4)
            }
            delegate: Item {
                id: row
                required property var model
                width: list.width; height: Math.max(Theme.dp(36),Theme.bodyMetrics.height+Theme.dp(12))
                readonly property bool chosen: !!model.included
                readonly property bool marked: !!model.marked
                Rectangle { anchors.fill: parent; radius: Theme.dp(4); color: row.marked ? Theme.selection : Theme.surface; border.color: row.marked ? Theme.accent : Theme.divider }
                RowLayout {
                    anchors.fill: parent; anchors.margins: Theme.dp(4); spacing: Theme.dp(7)
                    SavedRosterGrip { backend: pane.backend; athlete: row.model; selected: pane.selected; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(28) }
                    SCheck {
                        text: ""; checked: row.marked
                        enabled: pane.selected || !row.chosen
                        Accessible.name: row.model.athleteName
                        Layout.preferredWidth: Theme.dp(26)
                        onClicked: if(pane.selected)pane.backend.mark(row.model.athleteId,Qt.ControlModifier);else pane.backend.selectCandidate(row.model.athleteId)
                    }
                    Image { source: row.model.flagSource || ""; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(18); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                    Label { text: row.model.athleteName; color: !pane.selected && row.chosen ? Theme.muted : Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                    Label { text: row.model.countryCode; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
                    SButton {
                        text: pane.selected ? "−" : row.chosen ? "✓" : "+"
                        enabled: pane.selected || !row.chosen; quiet: true
                        Layout.preferredWidth: Theme.dp(30); Layout.preferredHeight: row.height-Theme.dp(8)
                        Accessible.name: pane.selected ? qsTr("Usuń %1 z listy").arg(row.model.athleteName) : qsTr("Dodaj %1 do listy").arg(row.model.athleteName)
                        onClicked: if(pane.selected)pane.backend.toggle(row.model.athleteId);else pane.backend.include(row.model.athleteId)
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
                    if(!source || source.backend!==pane.backend)return
                    var id=source.athlete.athleteId
                    var backend=pane.backend, receiving=pane.selected, sending=source.selected
                    var targetId="", at=list.indexAt(drop.x,drop.y+list.contentY)
                    if(receiving && sending && !backend.grouped && at>=0)targetId=pane.rows.get(at).athleteId
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
                title: pane.selected ? qsTr("Lista jest pusta") : qsTr("Brak pasujących zawodników")
                description: pane.selected ? qsTr("Pusta lista oznacza wszystkich zawodników. Dodaj wybranych, aby ograniczyć skład.") : qsTr("Zmień nazwisko lub kraj, aby zobaczyć innych zawodników.")
            }
            Keys.onDeletePressed: if(pane.selected){pane.backend.removeMarked();event.accepted=true}
        }
        RowLayout {
            Layout.fillWidth: true
            SButton {
                Layout.fillWidth: true
                primary: !pane.selected
                text: pane.selected ? qsTr("Usuń zaznaczonych (%1)").arg(pane.backend.markedCount) : qsTr("Dodaj zaznaczonych (%1)").arg(pane.backend.candidateMarkedCount)
                enabled: pane.selected ? pane.backend.markedCount>0 : pane.backend.candidateMarkedCount>0
                onClicked: if(pane.selected)pane.backend.removeMarked();else pane.backend.addMarkedCandidates()
            }
            SButton { visible: !pane.selected; text: qsTr("Dodaj widocznych"); enabled: pane.rows.count>0; onClicked: pane.backend.addVisible() }
        }
    }
}
