import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: calendar
    ViewArrival { target: calendar; expressive: true; emphasis: arrivalHeading }
    objectName: "calendarWorkspace"
    property var node
    readonly property var backend: node ? node.values.backend : null
    readonly property bool compact: editorArea.width < 850 || height < 420
    readonly property bool tight: compact && height < 300
    readonly property int editRequest: node ? node.values.editRequest || 0 : 0
    property bool editing: editRequest>0
    onEditRequestChanged: if(editRequest>0)editing=true
    property bool renaming: backend ? backend.newCalendar : false
    property int section: 0
    property bool addingMany: false
    property bool showRosterAssignment: false
    property bool monthView: false
    property string panel: "events"
    onPanelChanged: if(planningScroll.contentItem)planningScroll.contentItem.contentY=0
    implicitHeight: 640
    RowLayout {
        anchors.fill: parent; spacing: Theme.compact ? 6 : 16
        Pane {
            visible: calendar.width>=1180
            Layout.preferredWidth: 210; Layout.fillHeight: true; padding: 12
            background: Surface { fillColor: Theme.surface; sweep: 22 }
            contentItem: ColumnLayout {
                spacing: 8
                Label { id: arrivalHeading; text: qsTr("Kalendarze"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                SButton { objectName: "calendarCreateSidebar"; text: qsTr("Nowy kalendarz"); primary: false; Layout.fillWidth: true; onClicked: {calendar.backend.catalogAction("add");calendar.renaming=true;calendar.editing=false} }
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
            SChoice { visible: calendar.width<1180 && calendar.backend.calendars.length>1 && !(calendar.compact && calendar.editing); Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.columnSpan: calendar.compact && !calendar.tight ? 2 : 1; model: calendar.backend.calendars; currentIndex: calendar.backend.calendarIndex; Accessible.name: qsTr("Edytowany kalendarz"); onActivated: calendar.backend.selectCalendar(currentIndex) }
            SField { id: nameInput; visible: !calendar.compact || calendar.renaming; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.columnSpan: calendar.compact ? 3 : 1; text: calendar.backend.calendarName; Accessible.name: qsTr("Nazwa edytowanego kalendarza"); onEditingFinished: {if(text!==calendar.backend.calendarName)calendar.backend.renameCalendar(text);calendar.renaming=false} }
            SButton { visible: calendar.width<1180 && !(calendar.compact && calendar.editing); objectName: "calendarCreate"; text: calendar.compact ? qsTr("+ Kalendarz") : qsTr("Nowy kalendarz"); primary: false; Accessible.name: qsTr("Nowy kalendarz"); onClicked: {calendar.backend.catalogAction("add");calendar.renaming=true;calendar.editing=false;nameInput.forceActiveFocus();nameInput.selectAll()} }
            SButton { objectName: "calendarActivate"; visible: calendar.backend.calendarIndex>=0 && (calendar.backend.events.length>0 || !calendar.backend.activeCalendar) && !calendar.tight && !(calendar.compact && calendar.editing); Layout.columnSpan: calendar.compact ? 2 : 1; Layout.fillWidth: calendar.compact; text: calendar.backend.activeCalendar ? qsTr("Aktywny kalendarz") : qsTr("Ustaw jako aktywny"); primary: false; foreground: calendar.backend.activeCalendar ? Theme.muted : Theme.accent; enabled: !calendar.backend.activeCalendar; onClicked: calendar.backend.activateCalendar() }
            SButton {
                text: "⚙"; Accessible.name: qsTr("Ustawienia kalendarza"); hint: Accessible.name; quiet: true; implicitHeight: calendar.tight ? 32 : 40; onClicked: catalogMenu.open()
                ActionMenu {
                    id: catalogMenu
                    entries: [{id:"roster",text:qsTr("Lista zawodników")},{id:"active",text:qsTr("Ustaw jako aktywny"),enabled:!calendar.backend.activeCalendar},{id:"classes",text:calendar.section===0?qsTr("Klasyfikacje"):qsTr("Konkursy")},{id:"rename",text:qsTr("Zmień nazwę kalendarza")},{id:"up",text:qsTr("Kalendarz w górę")},{id:"down",text:qsTr("Kalendarz w dół")},{id:"remove",text:qsTr("Usuń kalendarz")},{id:"load",text:qsTr("Wczytaj preset")},{id:"save",text:qsTr("Zapisz jako preset")}]
                    onSelected: {if(entry.id==="roster")calendar.showRosterAssignment=!calendar.showRosterAssignment;else if(entry.id==="active")calendar.backend.activateCalendar();else if(entry.id==="classes")calendar.section=calendar.section===0?1:0;else if(entry.id==="rename"){calendar.renaming=true;nameInput.forceActiveFocus();nameInput.selectAll()}else if(entry.id==="load"||entry.id==="save")calendar.backend.preset(entry.id==="save");else calendar.backend.catalogAction(entry.id)}
                }
            }
        }
        SectionTabs {
            visible: calendar.backend.calendarIndex>=0
            Layout.fillWidth: true; dense: true
            optionNamePrefix: "calendarPanel-"
            options: [qsTr("Kalendarz"),qsTr("Szybkie planowanie"),qsTr("Domyślne zasady")]
            currentIndex: calendar.panel==="planning" ? 1 : calendar.panel==="defaults" ? 2 : 0
            onSelected: calendar.panel=["events","planning","defaults"][index]
        }
        Label { text: calendar.backend.error; visible: text.length>0; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        ScrollView {
            id: planningScroll
            visible: calendar.panel!=="events"; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: planningScroll.availableWidth; spacing: Theme.dp(12)
                CalendarDatePlanner { visible: calendar.panel==="planning"; backend: calendar.backend; Layout.fillWidth: true }
                ColumnLayout {
                    visible: calendar.panel==="defaults"; Layout.fillWidth: true; spacing: Theme.dp(10)
                    Label { text: qsTr("Domyślne zasady kalendarza"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    Label { text: qsTr("Dotyczą nowo dodawanych wydarzeń. Możesz wskazać inne zasady niż domyślne z paczki."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    Repeater { model: calendar.backend.ruleDefaults; ColumnLayout { required property var modelData; Layout.fillWidth: true; Label { text: parent.modelData.title; color: Theme.muted; font.pointSize: Theme.caption } SChoice { Layout.fillWidth: true; Layout.maximumWidth: Theme.dp(600); model: parent.modelData.options; currentIndex: parent.modelData.current; objectName: "calendarDefaultRule-"+parent.modelData.role; onActivated: function(index){var role=parent.modelData.role;var backend=calendar.backend;Qt.callLater(function(){if(backend)backend.setDefaultRule(role,index)})} } } }
                }
            }
        }
        RowLayout {
            visible: calendar.panel==="events" && calendar.showRosterAssignment && calendar.backend.calendarIndex>=0
            Layout.fillWidth: true
            Label { text: qsTr("Lista zawodników"); color: Theme.muted; font.pointSize: Theme.caption }
            SChoice {
                objectName: "calendarRosterChoice"
                Layout.fillWidth: true; Layout.minimumWidth: 0
                model: calendar.backend.rosterChoices; textRole: "label"
                currentIndex: {var choices=calendar.backend.rosterChoices;for(var i=0;i<choices.length;++i)if(choices[i].id===calendar.backend.rosterId)return i;return 0}
                Accessible.name: qsTr("Lista zawodników kalendarza")
                onActivated: calendar.backend.setRosterId(calendar.backend.rosterChoices[index].id)
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Przed konkursem ta lista stanie się aktywna. Skład możesz zmienić w przygotowaniu.")
            }
        }
        SectionTabs { visible: calendar.panel==="events" && !calendar.tight && !(calendar.compact && calendar.editing); Layout.fillWidth: true; options: [qsTr("Konkursy"),qsTr("Klasyfikacje")]; currentIndex: calendar.section; onSelected: calendar.section=index }
        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            visible: active
            active: calendar.panel==="events" && calendar.section===0 && calendar.monthView && calendar.backend.datesEnabled
            sourceComponent: ColumnLayout {
                RowLayout {
                    Layout.fillWidth: true
                    SButton { text: qsTr("Lista i edycja"); primary: false; onClicked: calendar.monthView=false }
                    SButton { text: qsTr("Szybkie planowanie"); onClicked: calendar.panel="planning" }
                }
                CalendarDateOverview { Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; backend: calendar.backend; onEditEvent: function(eventId){if(eventId.length)calendar.backend.select(eventId);calendar.monthView=false;calendar.editing=true;eventInspector.section=0} }
            }
        }
        CalendarClassifications { visible: calendar.panel==="events" && calendar.section===1; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; backend: calendar.backend; editorNode: calendar.node.values.classificationEditor }
        ColumnLayout {
        visible: calendar.panel==="events" && calendar.section===0 && !(calendar.monthView && calendar.backend.datesEnabled)
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        SectionTabs { visible: false; Layout.fillWidth: true; options: [qsTr("Konkursy"),qsTr("Edycja")]; currentIndex: calendar.editing ? 1 : 0; onSelected: calendar.editing=index === 1 }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: Theme.compact ? 6 : 16
            ColumnLayout {
                visible: !calendar.compact || !calendar.editing
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                Layout.preferredWidth: 320
                spacing: calendar.tight ? 4 : 8

                RowLayout {
                    visible: calendar.backend.events.length>0 && (!calendar.compact || !calendar.editing)
                    Layout.fillWidth: true; spacing: Theme.dp(3)
                    SButton { objectName: "calendarAdd"; text: "+"; Accessible.name: qsTr("Dodaj konkurs"); hint: Accessible.name; primary: false; foreground: Theme.accent; implicitWidth: Theme.controlHeight; dense: true; onClicked: {calendar.backend.add();calendar.editing=true} }
                    SIconButton { objectName: "calendarCopy"; text: ""; iconName: "copy"; actionName: qsTr("Kopiuj konkursy"); enabled: calendar.backend.selection.count>0; onClicked: calendar.backend.duplicate() }
                    SIconButton { objectName: "calendarRemove"; text: "×"; actionName: qsTr("Usuń konkursy"); foreground: Theme.danger; enabled: calendar.backend.selection.count>0 && !calendar.backend.selection.locked; onClicked: calendar.backend.remove() }
                    SIconButton { text: "↶"; actionName: qsTr("Cofnij"); visible: calendar.backend.canUndo; onClicked: calendar.backend.undo() }
                    SIconButton { text: "↷"; actionName: qsTr("Ponów"); visible: calendar.backend.canRedo; onClicked: calendar.backend.redo() }
                    Item { Layout.fillWidth: true }
                    SIconButton { objectName: "calendarMonthToggle"; text: "▦"; actionName: qsTr("Widok kalendarza"); visible: calendar.backend.datesEnabled; onClicked: calendar.monthView=!calendar.monthView }
                    SIconButton {
                        text: "↕"; actionName: qsTr("Kolejność i zaznaczanie"); onClicked: eventMenu.open()
                        ActionMenu { id: eventMenu; entries: [{id:"many",text:qsTr("Dodaj wiele konkursów")},{id:"future",text:qsTr("Zaznacz przyszłe konkursy")},{id:"up",text:qsTr("Przesuń konkursy w górę"),enabled:!calendar.backend.selection.locked},{id:"down",text:qsTr("Przesuń konkursy w dół"),enabled:!calendar.backend.selection.locked}]; onSelected: {if(entry.id==="many")calendar.addingMany=!calendar.addingMany;else if(entry.id==="future")calendar.backend.selectAll(true);else calendar.backend.move(entry.id==="up"?-1:1)} }
                    }
                }
                RowLayout {
                    visible: calendar.addingMany; Layout.fillWidth: true
                    Label { text: qsTr("Liczba konkursów"); color: Theme.muted; font.pointSize: Theme.caption }
                    NumberField { id: batchCount; direct: false; fields: ({id:"calendarBatchCount",name:"calendarBatchCount",value:5,minimum:1,maximum:100,step:1,decimals:0}); Layout.preferredWidth: 110 }
                    SButton { text: qsTr("Dodaj"); primary: true; onClicked: {calendar.backend.addMany(batchCount.currentValue);calendar.addingMany=false;calendar.editing=true} }
                }
                Label { visible: !!calendar.backend.message; text: calendar.backend.message; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                RowLayout {
                    visible: calendar.backend.events.length>0
                    Layout.fillWidth: true
                    Label { text: calendar.backend.selection.count ? qsTr("Zaznaczono: %1").arg(calendar.backend.selection.count) : qsTr("Konkursy: %1").arg(calendar.backend.events.length); font.pointSize: Theme.caption; color: Theme.muted; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                    SButton { objectName: "calendarClearSelection"; implicitHeight: calendar.tight?24:40; visible: calendar.backend.selection.count>0; text: "×"; quiet: true; Accessible.name: qsTr("Wyczyść zaznaczenie"); onClicked: calendar.backend.clearSelection() }
                    SButton { objectName: "calendarEditSelection"; implicitHeight: calendar.tight?28:40; visible: calendar.compact; enabled: calendar.backend.selection.count>0; text: qsTr("Edytuj"); onClicked: calendar.editing=true }
                    SButton { visible: !calendar.compact; text: qsTr("Przyszłe"); implicitHeight: calendar.tight ? 32 : 40; font.pointSize: calendar.tight ? Theme.caption : Theme.body; quiet: true; hint: qsTr("Zaznacz przyszłe konkursy"); onClicked: calendar.backend.selectAll(true) }
                }
                ListView {
                    id: rows
                    objectName: "calendarEvents"
                    move: Transition { NumberAnimation { properties: "x,y"; duration: Theme.motion; easing.type: Easing.OutCubic } }
                    moveDisplaced: Transition { NumberAnimation { properties: "x,y"; duration: Theme.motion; easing.type: Easing.OutCubic } }
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                    model: calendar.backend.eventModel
                    clip: true; spacing: Theme.compact ? 2 : 4
                    activeFocusOnTab: true
                    ScrollBar.vertical: ScrollBar {}
                    delegate: CalendarEventRow { width: rows.width; event: model.entry; backend: calendar.backend; viewport: rows; compact: calendar.compact; onActivated: function(eventId){var workspace=calendar;workspace.editing=true;workspace.backend.select(eventId)} }
                    EmptyState {
                        parent: rows
                        anchors.fill: parent; compact: parent.height<150; visible: rows.count===0
                        title: calendar.backend.calendarIndex>=0 ? qsTr("Kalendarz jest pusty") : qsTr("Utwórz pierwszy kalendarz")
                        description: calendar.backend.calendarIndex>=0 ? qsTr("Dodaj konkursy lub wczytaj preset. Potem ustalisz skocznie i zasady.") : qsTr("Kalendarz określa konkursy i ich kolejność w sezonie.")
                        actionObjectName: "calendarEmptyAction"
                        actionText: calendar.backend.calendarIndex>=0 ? qsTr("Dodaj konkurs") : qsTr("Utwórz kalendarz z konkursem")
                        onTriggered: {if(calendar.backend.calendarIndex>=0){calendar.backend.add();calendar.editing=true}else{calendar.backend.catalogAction("add");calendar.backend.add();calendar.editing=true;calendar.renaming=true;nameInput.forceActiveFocus();nameInput.selectAll()}}
                    }
                }
            }
            CalendarInspector {
                id: eventInspector
                visible: calendar.backend.events.length>0 && (!calendar.compact || calendar.editing)
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
