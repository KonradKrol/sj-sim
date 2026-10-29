import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    ViewArrival { target: workspace; expressive: true; emphasis: arrivalHeading }
    objectName: "koGroupsWorkspace"
    property var node
    readonly property var fields: node ? node.values : ({})
    property real savedY: 0
    spacing: Theme.dp(12)
    function generate(index) { savedY=0;ui.edit(fields.methodId,index+1) }
    RowLayout {
        Layout.fillWidth: true
        Label { id: arrivalHeading; text: qsTr("Grupy KO"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true }
        Label { text: qsTr("%1 grup · %2 zawodników").arg((workspace.fields.groups || []).length).arg((workspace.fields.athletes || []).length); color: Theme.muted; font.pointSize: Theme.caption }
    }
    RowLayout {
        Layout.fillWidth: true
        SChoice { id: method; objectName: "koGroupingMethod"; enabled: (workspace.fields.athletes || []).length>0; model: workspace.fields.methods || []; currentIndex: Math.max(0,(workspace.fields.methodIndex || 0)-1); Layout.fillWidth: true; Layout.maximumWidth: 400; Accessible.name: qsTr("Sposób doboru grup"); onActivated: function(selectedIndex) {workspace.generate(selectedIndex)} }
        SButton { objectName: "koRegenerate"; visible: (workspace.fields.methodIndex || 0)>=3 && (workspace.fields.groups || []).length>0; text: (workspace.fields.groups || []).length ? qsTr("Losuj ponownie") : qsTr("Dobierz grupy"); onClicked: workspace.generate(method.currentIndex) }
    }
    Label { visible: (workspace.fields.groups || []).length>0; text: qsTr("Wybierz zawodnika lub przeciągnij uchwyt, aby zamienić miejsca."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    GridView {
        id: groups
        objectName: "koGroupsGrid"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        model: workspace.fields.groups || []
        readonly property int columns: Math.max(1,Math.floor(width/Theme.dp(340)))
        cellWidth: width/columns
        cellHeight: Theme.dp(32)+Theme.headingMetrics.height+(workspace.fields.largest || 1)*(Math.max(Theme.controlHeight,Theme.bodyMetrics.height+18)+Theme.dp(6))
        clip: true; boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        onMovementEnded: workspace.savedY=contentY
        onModelChanged: Qt.callLater(function(){groups.contentY=Math.max(0,Math.min(workspace.savedY,Math.max(0,groups.contentHeight-groups.height)))})
        delegate: Pane {
            id: card
            readonly property var group: modelData
            function memberAt(point) {
                for (var i=0;i<members.count;++i) {
                    var member=members.itemAt(i),p=card.mapToItem(member,point.x,point.y)
                    if (p.x>=0 && p.x<member.width && p.y>=0 && p.y<member.height) return member
                }
                return null
            }
            width: groups.cellWidth-Theme.dp(8); height: groups.cellHeight-Theme.dp(8)
            padding: Theme.dp(10)
            background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; sweep: 0 }
            contentItem: ColumnLayout {
                spacing: Theme.dp(6)
                Label { text: qsTr("Grupa %1").arg(card.group.number); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true }
                Repeater {
                    model: card.group.members
                    id: members
                    KoMemberSlot {
                        id: slotItem
                        groupId: card.group.id; slot: index
                        objectName: "koMember-"+groupId+"-"+slot
                        athleteId: modelData.id
                        athleteName: (workspace.fields.names || [])[modelData.index] || ""
                        flagSource: (workspace.fields.flags || [])[modelData.index] || ""
                        dropTarget: dragger.dragging && dragger.targetGroup===groupId && dragger.targetSlot===slot
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        onChooseRequested: picker.openFor(slotItem)
                        onActivated: function(selectedIndex) {workspace.savedY=groups.contentY;ui.assignKoMember(workspace.fields.id,groupId,slot,workspace.fields.athletes[selectedIndex].id)}
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }
        KoDragArea {
            id: dragger
            parent: groups; z: 5
            anchors.fill: parent
            viewport: groups
            onSwapRequested: function(groupId,slot,athleteId) {workspace.savedY=groups.contentY;ui.assignKoMember(workspace.fields.id,groupId,slot,athleteId)}
        }
        EmptyState { anchors.fill: parent; visible: groups.count===0; title: (workspace.fields.athletes || []).length ? qsTr("Grupy nie zostały dobrane") : qsTr("Brak zawodników"); description: (workspace.fields.athletes || []).length ? qsTr("Wybierz sposób doboru i dobierz grupy.") : qsTr("Dodaj zawodników do listy startowej."); actionText: (workspace.fields.athletes || []).length ? qsTr("Dobierz grupy") : qsTr("Lista startowa"); onTriggered: (workspace.fields.athletes || []).length ? workspace.generate(method.currentIndex) : ui.edit(ui.root.values.id,1) }
    }
    SButton { visible: workspace.fields.submitId !== undefined; text: qsTr("Rozpocznij serię"); primary: true; Layout.alignment: Qt.AlignRight; onClicked: ui.activate(workspace.fields.submitId) }
    KoAthletePicker {
        id: picker
        fields: workspace.fields
        onChosen: function(groupId,slot,athleteIndex) {workspace.savedY=groups.contentY;ui.assignKoMember(workspace.fields.id,groupId,slot,workspace.fields.athletes[athleteIndex].id)}
    }
}
