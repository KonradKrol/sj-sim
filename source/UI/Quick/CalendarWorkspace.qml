import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: calendar
    objectName: "calendarWorkspace"
    property var node
    readonly property var backend: node ? node.values.backend : null
    readonly property bool compact: editorArea.width < 850 || height < 420
    readonly property bool tight: compact && height < 300
    property bool editing: false
    property bool renaming: false
    property int section: 0
    property bool addingMany: false
    property bool monthView: false
    implicitHeight: 640
    RowLayout {
        anchors.fill: parent; spacing: Theme.compact ? 6 : 16
        Pane {
            visible: calendar.width>=1180
            Layout.preferredWidth: 210; Layout.fillHeight: true; padding: 12
            background: Surface { fillColor: Theme.surface; sweep: 22 }
            contentItem: ColumnLayout {
                spacing: 8
                Label { text: qsTr("Kalendarze"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                SButton { objectName: "calendarCreateSidebar"; text: qsTr("Nowy kalendarz"); primary: true; Layout.fillWidth: true; onClicked: {calendar.backend.catalogAction("add");calendar.renaming=true;calendar.editing=false} }
                ListView {
                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true; spacing: 4
                    model: calendar.backend.calendarEntries
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate {
                        width: ListView.view.width; height: 66
                        highlighted: modelData.selected
                        contentItem: Column {
                            Label { width: parent.width; text: modelData.name; color: Theme.ink; font.pointSize: Theme.body; elide: Text.ElideRight }
                            Label { text: modelData.active ? qsTr("Aktywny") : qsTr("Konkursy: %1").arg(modelData.count); color: modelData.active ? Theme.accent : Theme.muted; font.pointSize: Theme.caption }
                        }
                        background: Rectangle { color: parent.highlighted ? Theme.selection : "transparent"; radius: 6 }
                        onClicked: calendar.backend.selectCalendar(modelData.index)
                    }
                }
            }
        }
        ColumnLayout {
        id: editorArea
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
        spacing: calendar.tight?2:8
        GridLayout {
            Layout.fillWidth: true; columns: calendar.compact ? 3 : 5; columnSpacing: 6; rowSpacing: 4
            SButton { objectName: "calendarReturnToList"; visible: calendar.compact && calendar.editing; text: "←"; implicitHeight: 32; Accessible.name: qsTr("Konkursy"); quiet: true; onClicked: calendar.editing=false }
            Label { visible: calendar.compact && calendar.editing; text: calendar.backend.calendarName; color: Theme.ink; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            SChoice { visible: calendar.width<1180 && calendar.backend.calendars.length>0 && !(calendar.compact && calendar.editing); Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.columnSpan: calendar.compact && !calendar.tight ? 2 : 1; model: calendar.backend.calendars; currentIndex: calendar.backend.calendarIndex; Accessible.name: qsTr("Edytowany kalendarz"); onActivated: calendar.backend.selectCalendar(currentIndex) }
            SField { id: nameInput; visible: !calendar.compact || calendar.renaming; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.columnSpan: calendar.compact ? 3 : 1; text: calendar.backend.calendarName; Accessible.name: qsTr("Nazwa edytowanego kalendarza"); onEditingFinished: {if(text!==calendar.backend.calendarName)calendar.backend.renameCalendar(text);calendar.renaming=false} }
            SButton { visible: calendar.width<1180 && !(calendar.compact && calendar.editing); objectName: "calendarCreate"; text: calendar.compact ? "+" : qsTr("Nowy kalendarz"); Accessible.name: qsTr("Nowy kalendarz"); onClicked: {calendar.backend.catalogAction("add");calendar.renaming=true;calendar.editing=false;nameInput.forceActiveFocus();nameInput.selectAll()} }
            SButton { objectName: "calendarActivate"; visible: calendar.backend.calendarIndex>=0 && !calendar.tight && !(calendar.compact && calendar.editing); Layout.columnSpan: calendar.compact ? 2 : 1; Layout.fillWidth: calendar.compact; text: calendar.backend.activeCalendar ? qsTr("Aktywny kalendarz") : qsTr("Ustaw jako aktywny"); primary: !calendar.backend.activeCalendar; enabled: !calendar.backend.activeCalendar; onClicked: calendar.backend.activateCalendar() }
            SButton {
                text: "⋯"; Accessible.name: qsTr("Opcje kalendarza"); quiet: true; implicitHeight: calendar.tight ? 32 : 40; onClicked: catalogMenu.open()
                ActionMenu {
                    id: catalogMenu
                    entries: [{id:"active",text:qsTr("Ustaw jako aktywny"),enabled:!calendar.backend.activeCalendar},{id:"classes",text:calendar.section===0?qsTr("Klasyfikacje"):qsTr("Konkursy")},{id:"rename",text:qsTr("Zmień nazwę kalendarza")},{id:"up",text:qsTr("Kalendarz w górę")},{id:"down",text:qsTr("Kalendarz w dół")},{id:"remove",text:qsTr("Usuń kalendarz")},{id:"load",text:qsTr("Wczytaj preset")},{id:"save",text:qsTr("Zapisz jako preset")}]
                    onSelected: {if(entry.id==="active")calendar.backend.activateCalendar();else if(entry.id==="classes")calendar.section=calendar.section===0?1:0;else if(entry.id==="rename"){calendar.renaming=true;nameInput.forceActiveFocus();nameInput.selectAll()}else if(entry.id==="load"||entry.id==="save")calendar.backend.preset(entry.id==="save");else calendar.backend.catalogAction(entry.id)}
                }
            }
        }
        SectionTabs { visible: !calendar.tight && !(calendar.compact && calendar.editing); Layout.fillWidth: true; options: [qsTr("Konkursy"),qsTr("Klasyfikacje")]; currentIndex: calendar.section; onSelected: calendar.section=index }
        Flow {
            visible: calendar.section===0 && (!calendar.compact || !calendar.editing)
            Layout.fillWidth: true; spacing: 6
            SButton { objectName: "calendarEnableDates"; visible: !calendar.backend.datesEnabled && (!calendar.tight || calendar.backend.canEnableDates); text: calendar.backend.canEnableDates ? qsTr("Włącz daty zawodów") : qsTr("Sezon bez dat"); enabled: calendar.backend.canEnableDates; hint: qsTr("Daty można włączyć przed rozpoczęciem sezonu. Ustawienie znajdziesz też w ustawieniach symulacji."); onClicked: calendar.backend.setDatesEnabled(true) }
            SButton { objectName: "calendarMonthToggle"; visible: calendar.backend.datesEnabled; text: calendar.monthView ? qsTr("Lista i edycja") : qsTr("Widok kalendarza"); primary: calendar.monthView; onClicked: calendar.monthView=!calendar.monthView }
            SButton { objectName: "calendarPlanDates"; visible: calendar.backend.datesEnabled; text: qsTr("Zaplanuj terminy"); onClicked: {calendar.monthView=false;calendar.editing=true;eventInspector.section=3} }
        }
        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            visible: active
            active: calendar.section===0 && calendar.monthView && calendar.backend.datesEnabled
            sourceComponent: CalendarDateOverview { backend: calendar.backend; onEditEvent: function(eventId){if(eventId.length)calendar.backend.select(eventId);calendar.monthView=false;calendar.editing=true;eventInspector.section=0} }
        }
        CalendarClassifications { visible: calendar.section===1; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; backend: calendar.backend; editorNode: calendar.node.values.classificationEditor }
        ColumnLayout {
        visible: calendar.section===0 && !(calendar.monthView && calendar.backend.datesEnabled)
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        SectionTabs { visible: false; Layout.fillWidth: true; options: [qsTr("Konkursy"),qsTr("Edycja")]; currentIndex: calendar.editing ? 1 : 0; onSelected: calendar.editing=index === 1 }
        Flow {
            visible: !calendar.compact || !calendar.editing
            Layout.fillWidth: true
            spacing: 6
            SButton { objectName: "calendarAdd"; text: qsTr("Dodaj konkurs"); primary: !calendar.tight; implicitHeight: calendar.tight?32:48; font.pointSize: calendar.tight?Theme.caption:Theme.section; enabled: calendar.backend.calendarIndex >= 0; onClicked: {calendar.backend.add();calendar.editing=true} }
            SButton { visible: !calendar.tight; text: qsTr("Dodaj wiele"); enabled: calendar.backend.calendarIndex>=0; onClicked: calendar.addingMany=!calendar.addingMany }
            SButton { visible: !calendar.compact; text: qsTr("Kopiuj"); enabled: calendar.backend.selection.count > 0; onClicked: calendar.backend.duplicate() }
            SButton { visible: !calendar.compact; text: qsTr("Usuń"); enabled: calendar.backend.selection.count > 0 && !calendar.backend.selection.locked; onClicked: calendar.backend.remove() }
            SButton { visible: !calendar.compact; text: "↑"; Accessible.name: qsTr("Przesuń konkurs w górę"); enabled: calendar.backend.selection.count > 0 && !calendar.backend.selection.locked; onClicked: calendar.backend.move(-1) }
            SButton { visible: !calendar.compact; text: "↓"; Accessible.name: qsTr("Przesuń konkurs w dół"); enabled: calendar.backend.selection.count > 0 && !calendar.backend.selection.locked; onClicked: calendar.backend.move(1) }
            SButton {
                visible: calendar.compact; text: "⋯"; implicitHeight: calendar.tight?32:40; Accessible.name: qsTr("Opcje konkursów"); quiet: true
                onClicked: eventMenu.open()
                ActionMenu {
                    id: eventMenu
                    entries: [{id:"future",text:qsTr("Zaznacz przyszłe konkursy")},{id:"copy",text:qsTr("Kopiuj"),enabled:calendar.backend.selection.count > 0},{id:"remove",text:qsTr("Usuń"),enabled:calendar.backend.selection.count > 0 && !calendar.backend.selection.locked},{id:"up",text:qsTr("Przesuń konkurs w górę"),enabled:!calendar.backend.selection.locked},{id:"down",text:qsTr("Przesuń konkurs w dół"),enabled:!calendar.backend.selection.locked}]
                    onSelected: {if(entry.id === "future")calendar.backend.selectAll(true);else if(entry.id === "copy")calendar.backend.duplicate();else if(entry.id === "remove")calendar.backend.remove();else calendar.backend.move(entry.id === "up" ? -1 : 1)}
                }
            }
            SButton { visible: !calendar.tight; text: qsTr("Cofnij"); enabled: calendar.backend.canUndo; onClicked: calendar.backend.undo() }
            SButton { visible: !calendar.tight; text: qsTr("Ponów"); enabled: calendar.backend.canRedo; onClicked: calendar.backend.redo() }
        }
        RowLayout {
            visible: calendar.addingMany; Layout.fillWidth: true
            Label { text: qsTr("Liczba konkursów"); color: Theme.muted; font.pointSize: Theme.caption }
            NumberField { id: batchCount; direct: false; fields: ({id:"calendarBatchCount",name:"calendarBatchCount",value:5,minimum:1,maximum:100,step:1,decimals:0}); Layout.preferredWidth: 110 }
            SButton { text: qsTr("Dodaj"); primary: true; onClicked: {calendar.backend.addMany(batchCount.currentValue);calendar.addingMany=false;calendar.editing=true} }
        }
        Label { visible: !!calendar.backend.message; text: calendar.backend.message; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: Theme.compact ? 6 : 16
            ColumnLayout {
                visible: !calendar.compact || !calendar.editing
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                Layout.preferredWidth: 320
                spacing: calendar.tight ? 4 : 8
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: calendar.backend.selection.count ? qsTr("Zaznaczono: %1").arg(calendar.backend.selection.count) : qsTr("Konkursy: %1").arg(calendar.backend.events.length); font.pointSize: Theme.caption; color: Theme.muted; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                    SButton { objectName: "calendarClearSelection"; implicitHeight: calendar.tight?24:40; visible: calendar.backend.selection.count>0; text: "×"; quiet: true; Accessible.name: qsTr("Wyczyść zaznaczenie"); onClicked: calendar.backend.clearSelection() }
                    SButton { objectName: "calendarEditSelection"; implicitHeight: calendar.tight?28:40; visible: calendar.compact; enabled: calendar.backend.selection.count>0; text: qsTr("Edytuj"); onClicked: calendar.editing=true }
                    SButton { visible: !calendar.compact; text: qsTr("Przyszłe"); implicitHeight: calendar.tight ? 32 : 40; font.pointSize: calendar.tight ? Theme.caption : Theme.body; quiet: true; hint: qsTr("Zaznacz przyszłe konkursy"); onClicked: calendar.backend.selectAll(true) }
                }
                ListView {
                    id: rows
                    objectName: "calendarEvents"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                    model: calendar.backend.eventModel
                    clip: true; spacing: Theme.compact ? 2 : 4
                    activeFocusOnTab: true
                    ScrollBar.vertical: ScrollBar {}
                    delegate: CalendarEventRow { width: rows.width; event: model.entry; backend: calendar.backend; viewport: rows; compact: calendar.compact; onActivated: function(eventId){var workspace=calendar;workspace.editing=true;workspace.backend.select(eventId)} }
                    Label { visible: rows.count === 0; anchors.centerIn: parent; width: parent.width; text: calendar.backend.calendarIndex>=0 ? qsTr("Dodaj konkurs lub wczytaj preset.") : qsTr("Utwórz kalendarz, aby dodać konkursy."); color: Theme.muted; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
                }
            }
            CalendarInspector {
                id: eventInspector
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
    }
    }
    Shortcut { sequence: "Ctrl+A"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.selectAll() }
    Shortcut { sequence: "Delete"; enabled: calendar.visible && !calendar.backend.selection.locked && !ui.editingText; onActivated: calendar.backend.remove() }
    Shortcut { sequence: "Ctrl+Up"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.move(-1) }
    Shortcut { sequence: "Ctrl+Down"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.move(1) }
    Shortcut { sequence: "Ctrl+Z"; enabled: calendar.visible && calendar.backend.canUndo && !ui.editingText; onActivated: calendar.backend.undo() }
    Shortcut { sequence: "Ctrl+Shift+Z"; enabled: calendar.visible && calendar.backend.canRedo && !ui.editingText; onActivated: calendar.backend.redo() }
    Shortcut { sequence: "Insert"; enabled: calendar.visible && !ui.editingText; onActivated: calendar.backend.add() }
}
