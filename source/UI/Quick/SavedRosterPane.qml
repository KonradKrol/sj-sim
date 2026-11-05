import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: pane
    property var backend
    property bool selected: false
    property bool compact: false
    readonly property var rows: selected ? backend.ordered : backend.library
    padding: Theme.dp(compact ? 4 : 10)
    background: Rectangle { color: pane.selected ? Theme.surface : Theme.raised; radius: Theme.radius; border.color: pane.selected ? Theme.outline : Theme.divider }
    contentItem: ColumnLayout {
        spacing: Theme.dp(pane.compact ? 4 : 8)
        RowLayout {
            visible: !pane.compact
            Layout.fillWidth: true
            Label { text: pane.selected ? qsTr("Wybrani zawodnicy") : qsTr("Wszyscy zawodnicy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { text: pane.selected ? pane.backend.count : pane.backend.library.count; color: Theme.accent; font.family: Theme.mono; font.pointSize: Theme.section }
        }
        SField {
            Layout.fillWidth: true; Layout.minimumWidth: 0
            implicitHeight: pane.compact ? Math.max(28,contentHeight+8) : Math.max(Theme.controlHeight,contentHeight+18)
            topPadding: pane.compact ? 4 : 6; bottomPadding: topPadding
            objectName: pane.selected ? "rosterOrderSearch" : "rosterSearch"
            text: pane.selected ? pane.backend.orderQuery : pane.backend.query
            placeholderText: qsTr("Szukaj nazwiska lub kraju")
            Accessible.name: placeholderText
            onTextEdited: if(pane.selected)pane.backend.orderQuery=text;else pane.backend.query=text
        }
        SavedRosterList {
            backend: pane.backend; selected: pane.selected; grouped: pane.backend.grouped
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        }
        SButton {
            visible: !pane.selected && !pane.compact
            text: qsTr("Dodaj widocznych")
            hint: qsTr("Dodaje wszystkich skoczków pasujących do wyszukiwania i kraju, także spoza widocznego fragmentu listy.")
            enabled: pane.rows.count>0; Layout.fillWidth: true
            onClicked: pane.backend.addVisible()
        }
    }
}
