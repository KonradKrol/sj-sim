import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: row
    property var event
    property var backend
    property bool compact: false
    objectName: "calendar-row-"+event.id
    signal activated(string eventId)
    activeFocusOnTab: true
    Keys.onReturnPressed: row.activated(row.event.id)
    Keys.onSpacePressed: row.select(Qt.ControlModifier)
    function select(modifiers) {
        var view=row.ListView.view;var position=row.event.number-1;var source=row.backend;var eventId=row.event.id
        Qt.callLater(function(){if(view && view.visible){view.currentIndex=position;var current=view.itemAtIndex(position);if(current)current.forceActiveFocus()}})
        source.select(eventId,modifiers)
    }
    implicitHeight: 88
    Accessible.role: Accessible.ListItem
    Accessible.name: event.title + ", " + event.type
    Accessible.selected: !!event.selected
    Surface { anchors.fill: parent; fillColor: row.event.selected ? Theme.selection : Theme.surface; strokeWidth: row.event.selected ? 2 : 0; edgeColor: Theme.accent; sweep: 16 }
    MouseArea {
        objectName: "calendar-select-"+row.event.id
        anchors.fill: parent
        onClicked: row.select(mouse.modifiers)
        onDoubleClicked: if(!(mouse.modifiers & (Qt.ControlModifier|Qt.ShiftModifier)))row.activated(row.event.id)
    }
    RowLayout {
        anchors.fill: parent; anchors.margins: 10
        spacing: 10
        SCheck { Layout.preferredWidth: 24; text: ""; checked: !!row.event.selected; Accessible.name: qsTr("Zaznacz %1").arg(row.event.title); onClicked: row.backend.select(row.event.id,Qt.ControlModifier) }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 3
            Label { text: row.event.number + ". " + row.event.title; font.family: Theme.displayFamily; font.pointSize: Theme.section; color: Theme.ink; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { text: row.event.type + (row.event.team ? " · " + qsTr("Drużynowy") : "") + (row.event.trainings ? " · " + qsTr("Treningi: %1").arg(row.event.trainings) : "") + (row.event.trial ? " · " + qsTr("Próbna") : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { visible: !!row.event.locked; text: row.event.played ? qsTr("Rozegrany") : row.event.cancelled ? qsTr("Odwołany") : qsTr("W trakcie"); color: Theme.warning; font.pointSize: Theme.caption }
        }
        SButton { visible: row.compact; text: "›"; quiet: true; Accessible.name: qsTr("Edytuj %1").arg(row.event.title); onClicked: row.activated(row.event.id) }
    }
}
