import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: calendar
    objectName: "calendarWorkspace"
    property var node
    readonly property var backend: node ? node.values.backend : null
    readonly property bool compact: width < 850 || height < 420
    readonly property bool tight: compact && height < 300
    property bool editing: false
    property bool renaming: false
    implicitHeight: 640
    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        RowLayout {
            Layout.fillWidth: true
            SButton { objectName: "calendarReturnToList"; visible: calendar.tight && calendar.editing; text: "←"; Accessible.name: qsTr("Konkursy"); quiet: true; onClicked: calendar.editing=false }
            SChoice { visible: calendar.backend.calendars.length > 0; Layout.fillWidth: true; Layout.minimumWidth: 0; model: calendar.backend.calendars; currentIndex: calendar.backend.calendarIndex; Accessible.name: qsTr("Kalendarz"); onActivated: calendar.backend.selectCalendar(currentIndex) }
            SField { visible: !calendar.compact || calendar.renaming; Layout.fillWidth: true; Layout.minimumWidth: 0; text: calendar.backend.calendarName; Accessible.name: qsTr("Nazwa kalendarza"); onEditingFinished: { if(text !== calendar.backend.calendarName)calendar.backend.renameCalendar(text);calendar.renaming=false } }
            SButton { visible: calendar.compact && (calendar.editing || calendar.tight) && calendar.backend.canUndo; text: qsTr("Cofnij"); onClicked: calendar.backend.undo() }
            SButton {
                text: calendar.compact ? "⋯" : qsTr("Kalendarze i presety"); Accessible.name: qsTr("Kalendarze i presety"); quiet: true; onClicked: catalogMenu.open()
                ActionMenu {
                    id: catalogMenu
                    entries: [{id:"rename",text:qsTr("Zmień nazwę kalendarza")},{id:"add",text:qsTr("Nowy kalendarz")},{id:"up",text:qsTr("Kalendarz w górę")},{id:"down",text:qsTr("Kalendarz w dół")},{id:"remove",text:qsTr("Usuń kalendarz")},{id:"load",text:qsTr("Wczytaj preset")},{id:"save",text:qsTr("Zapisz jako preset")}]
                    onSelected: { if(entry.id === "rename")calendar.renaming=true;else if(entry.id === "load" || entry.id === "save")calendar.backend.preset(entry.id === "save");else calendar.backend.catalogAction(entry.id) }
                }
            }
        }
        SectionTabs { visible: calendar.compact && !calendar.tight; Layout.fillWidth: true; options: [qsTr("Konkursy"),qsTr("Edycja")]; currentIndex: calendar.editing ? 1 : 0; onSelected: calendar.editing=index === 1 }
        Flow {
            visible: !calendar.compact || !calendar.editing
            Layout.fillWidth: true
            spacing: 6
            SButton { objectName: "calendarAdd"; text: qsTr("Dodaj konkurs"); primary: true; enabled: calendar.backend.calendarIndex >= 0 || calendar.backend.calendars.length === 0; onClicked: {calendar.backend.add();calendar.editing=true} }
            SButton { visible: !calendar.compact; text: qsTr("Kopiuj"); enabled: calendar.backend.selection.count > 0; onClicked: calendar.backend.duplicate() }
            SButton { visible: !calendar.compact; text: qsTr("Usuń"); enabled: calendar.backend.selection.count > 0 && !calendar.backend.selection.locked; onClicked: calendar.backend.remove() }
            SButton { visible: !calendar.compact; text: "↑"; Accessible.name: qsTr("Przesuń konkurs w górę"); enabled: calendar.backend.selection.count > 0 && !calendar.backend.selection.locked; onClicked: calendar.backend.move(-1) }
            SButton { visible: !calendar.compact; text: "↓"; Accessible.name: qsTr("Przesuń konkurs w dół"); enabled: calendar.backend.selection.count > 0 && !calendar.backend.selection.locked; onClicked: calendar.backend.move(1) }
            SButton {
                visible: calendar.compact; text: "⋯"; Accessible.name: qsTr("Opcje konkursów"); quiet: true
                onClicked: eventMenu.open()
                ActionMenu {
                    id: eventMenu
                    entries: [{id:"copy",text:qsTr("Kopiuj"),enabled:calendar.backend.selection.count > 0},{id:"remove",text:qsTr("Usuń"),enabled:calendar.backend.selection.count > 0 && !calendar.backend.selection.locked},{id:"up",text:qsTr("Przesuń konkurs w górę"),enabled:!calendar.backend.selection.locked},{id:"down",text:qsTr("Przesuń konkurs w dół"),enabled:!calendar.backend.selection.locked}]
                    onSelected: {if(entry.id === "copy")calendar.backend.duplicate();else if(entry.id === "remove")calendar.backend.remove();else calendar.backend.move(entry.id === "up" ? -1 : 1)}
                }
            }
            SButton { visible: calendar.tight; text: qsTr("Edycja"); enabled: calendar.backend.selection.count > 0; onClicked: calendar.editing=true }
            SButton { visible: !calendar.tight; text: qsTr("Cofnij"); enabled: calendar.backend.canUndo; onClicked: calendar.backend.undo() }
        }
        Label { visible: !!calendar.backend.message; text: calendar.backend.message; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: 16
            ColumnLayout {
                visible: !calendar.compact || !calendar.editing
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                Layout.preferredWidth: 320
                spacing: 8
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: qsTr("Konkursy: %1").arg(calendar.backend.events.length); font.pointSize: Theme.caption; color: Theme.muted; Layout.fillWidth: true }
                    SButton { text: qsTr("Przyszłe"); implicitHeight: calendar.tight ? 32 : 40; font.pointSize: calendar.tight ? Theme.caption : Theme.body; quiet: true; hint: qsTr("Zaznacz przyszłe konkursy"); onClicked: calendar.backend.selectAll(true) }
                }
                ListView {
                    id: rows
                    objectName: "calendarEvents"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                    model: calendar.backend.events
                    clip: true; spacing: 8
                    activeFocusOnTab: true
                    ScrollBar.vertical: ScrollBar {}
                    delegate: CalendarEventRow { width: rows.width; event: modelData; backend: calendar.backend; onActivated: calendar.editing=true }
                    Label { visible: rows.count === 0; anchors.centerIn: parent; width: parent.width; text: qsTr("Dodaj konkurs lub wczytaj preset."); color: Theme.muted; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
                }
            }
            CalendarInspector {
                visible: !calendar.compact || calendar.editing
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                Layout.preferredWidth: 500
                compact: calendar.compact
                onEditRequested: calendar.editing=true
                backend: calendar.backend
                rulesNode: calendar.node.values.rulesEditor
            }
        }
    }
    Shortcut { sequence: "Ctrl+A"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.selectAll() }
    Shortcut { sequence: "Delete"; enabled: calendar.visible && !calendar.backend.selection.locked && !ui.editingText; onActivated: calendar.backend.remove() }
    Shortcut { sequence: "Ctrl+Up"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.move(-1) }
    Shortcut { sequence: "Ctrl+Down"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.move(1) }
    Shortcut { sequence: "Ctrl+Z"; enabled: calendar.visible && calendar.backend.canUndo && !ui.editingText; onActivated: calendar.backend.undo() }
    Shortcut { sequence: "Insert"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.add() }
}
