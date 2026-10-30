import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: roster
    ViewArrival { target: roster; expressive: true; emphasis: arrivalHeading }
    objectName: "rosterWorkspace"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var backend: fields.backend
    readonly property bool compact: width < 800
    property bool choosing: false
    property bool limitsOpen: false
    implicitHeight: 580
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        RowLayout {
            visible: !!roster.fields.nameField
            Layout.fillWidth: true
            ProjectedControl { node: roster.fields.nameField || null; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            ProjectedControl { node: roster.fields.defaultField || null; Layout.fillWidth: true; Layout.minimumWidth: 0 }
        }
        RowLayout {
            visible: roster.backend.savedList
            Layout.fillWidth: true; spacing: Theme.dp(6)
            SectionTabs { Layout.fillWidth: true; Layout.minimumWidth: 0; optionNamePrefix: "rosterView-"; options: [qsTr("Kolejność"),qsTr("Według krajów")]; currentIndex: roster.backend.grouped ? 1 : 0; onSelected: roster.backend.grouped=index===1 }
            SButton {
                objectName: "rosterLimitsToggle"
                text: roster.backend.exceededCountries ? qsTr("Limity (%1)").arg(roster.backend.exceededCountries) : roster.width<500 ? qsTr("Limity") : qsTr("Limity krajów")
                Accessible.name: qsTr("Limity krajów")
                hint: roster.backend.exceededCountries ? qsTr("Liczba krajów z przekroczonym limitem: %1").arg(roster.backend.exceededCountries) : ""
                primary: roster.limitsOpen
                foreground: primary ? Theme.accentText : roster.backend.exceededCountries ? Theme.danger : Theme.ink
                implicitHeight: Theme.dp(40); font.pointSize: Theme.body
                onClicked: roster.limitsOpen=!roster.limitsOpen
            }
        }
        RosterLimits { visible: roster.backend.savedList && roster.limitsOpen; backend: roster.backend; Layout.fillWidth: true; listHeight: Math.min(Theme.dp(150),Math.max(Theme.dp(65),roster.height*0.25)) }
        RowLayout {
            Layout.fillWidth: true
            SButton { objectName: "rosterChoose"; visible: roster.backend.membership && roster.compact; text: roster.choosing ? qsTr("Lista · %1").arg(roster.backend.count) : (roster.backend.presets.length>0 ? qsTr("Dodaj") : qsTr("Dodaj zawodników")); primary: !roster.choosing; Layout.fillWidth: true; onClicked: roster.choosing=!roster.choosing }
            Label { id: arrivalHeading; visible: !roster.compact; text: roster.backend.membership ? qsTr("Wybór i kolejność") : qsTr("Kolejność skoczków"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true }
            SButton { objectName: "rosterOpenPresets"; visible: roster.compact && roster.backend.presets.length>0; text: qsTr("Listy"); Accessible.name: qsTr("Zapisane listy"); hint: qsTr("Zapisane listy"); onClicked: savedLists.open() }
            SButton { objectName: "rosterUndo"; text: qsTr("Cofnij"); enabled: roster.backend.canUndo; quiet: true; onClicked: roster.backend.undo() }
            SButton {
                visible: !roster.backend.grouped
                text: "⋯"; quiet: true; Accessible.name: qsTr("Opcje listy"); onClicked: options.open()
                ActionMenu {
                    id: options
                    entries: [{id:"name",text:qsTr("Według nazwiska")},{id:"country",text:qsTr("Według kraju")},{id:"random",text:qsTr("Losowo")}].concat(roster.fields.externalActions || [])
                    onSelected: {if(entry.button)ui.activate(entry.id);else roster.backend.sort(entry.id)}
                }
            }
        }
        Loader {
            active: !roster.compact && roster.backend.presets.length>0; visible: active
            Layout.fillWidth: true; Layout.preferredHeight: item ? item.implicitHeight : 0
            sourceComponent: Component { RosterPresets { backend: roster.backend; onApplied: roster.choosing=false } }
        }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: 20
            RosterLibrary { visible: roster.backend.membership && (!roster.compact || roster.choosing); backend: roster.backend; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 380 }
            Rectangle { visible: roster.backend.membership && !roster.compact; Layout.fillHeight: true; Layout.preferredWidth: 1; color: Theme.divider }
            RosterOrder { visible: !roster.compact || !roster.choosing || !roster.backend.membership; backend: roster.backend; title: roster.backend.savedList ? qsTr("Wybrani zawodnicy") : roster.backend.membership ? qsTr("Lista startowa") : qsTr("Drużyna"); Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 500 }
        }
        ProjectedControl { visible: !!roster.fields.submit; node: roster.fields.submit || null; Layout.fillWidth: true; Layout.preferredHeight: visible ? implicitHeight : 0 }
    }
    Popup {
        enter: PopupFade {}
        exit: PopupFade { leaving: true }
        id: savedLists; objectName: "rosterPresetsPanel"
        parent: Overlay.overlay
        x: Math.max(8,(parent.width-width)/2); y: Math.max(8,(parent.height-height)/2)
        width: Math.min(420,parent.width-16); padding: 12; modal: true; focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
        contentItem: ColumnLayout {
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Zapisane listy"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true }
                SButton { text: "×"; quiet: true; Accessible.name: qsTr("Zamknij"); onClicked: savedLists.close() }
            }
            RosterPresets { backend: roster.backend; stacked: true; Layout.fillWidth: true; onApplied: {roster.choosing=false;savedLists.close()} }
        }
    }
    Shortcut { sequence: "Ctrl+Z"; enabled: roster.visible && roster.backend.canUndo && !ui.editingText; onActivated: roster.backend.undo() }
}
