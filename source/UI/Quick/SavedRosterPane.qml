import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: pane
    property var backend
    property bool selected: false
    readonly property var rows: selected ? backend.ordered : backend.library
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
        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            sourceComponent: pane.backend.grouped ? groupedList : orderedList
            Component { id: groupedList; SavedRosterList { backend: pane.backend; selected: pane.selected; grouped: true } }
            Component { id: orderedList; SavedRosterList { backend: pane.backend; selected: pane.selected; grouped: false } }
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
