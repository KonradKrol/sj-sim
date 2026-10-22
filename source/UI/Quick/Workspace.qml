import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    readonly property bool tabbed: ui.root.values.kind === "tabs"
    readonly property bool season: ui.profile === "SimulationSaveManagerWindow"
    readonly property var contentNode: tabbed && ui.root.children.length ? ui.root.children[0] : ui.root
    readonly property bool formPage: contentNode && contentNode.values.kind === "formGenerator"
    readonly property bool rosterPage: contentNode && contentNode.values.kind === "roster"
    readonly property bool catalogPage: contentNode && contentNode.values.kind === "catalog"
    readonly property bool calendarPage: contentNode && contentNode.values.kind === "calendar"
    readonly property bool sideTabs: tabbed && width >= 1200 && height >= 550
    spacing: 0
    Loader {
        active: workspace.season
        Layout.fillWidth: true
        Layout.preferredHeight: active && item ? item.implicitHeight : 0
        sourceComponent: Component { SeasonBar {} }
    }
    SectionTabs {
        visible: workspace.tabbed && !workspace.sideTabs
        Layout.fillWidth: true
        Layout.leftMargin: 12; Layout.rightMargin: 12; Layout.topMargin: workspace.height < 600 ? 6 : 12
        options: ui.root.values.options || []
        currentIndex: ui.root.values.value || 0
        onSelected: { workspace.forceActiveFocus(); ui.edit(ui.root.values.id,index); scroll.contentItem.contentY = 0 }
    }
    RowLayout {
        Layout.fillWidth: true; Layout.fillHeight: true
        Layout.minimumHeight: 0; Layout.preferredHeight: 0
        spacing: 0
        SectionRail {
            visible: workspace.sideTabs
            Layout.preferredWidth: 164; Layout.fillHeight: true
            Layout.minimumHeight: 0
            Layout.topMargin: 24; Layout.leftMargin: 16
            options: ui.root.values.options || []
            currentIndex: ui.root.values.value || 0
            onSelected: { workspace.forceActiveFocus(); ui.edit(ui.root.values.id,index); scroll.contentItem.contentY=0 }
        }
        ScrollView {
        id: scroll
        visible: !workspace.catalogPage && !workspace.calendarPage && !workspace.rosterPage && !workspace.formPage
        objectName: "workspaceScroll"
        Layout.fillWidth: true; Layout.fillHeight: true
        Layout.minimumWidth: 0; Layout.minimumHeight: 0
        Layout.preferredWidth: 0; Layout.preferredHeight: 0
        contentWidth: availableWidth
        clip: true
        Pane {
            width: scroll.availableWidth
            padding: Theme.compact ? 8 : workspace.width < 700 ? 12 : 24
            background: Item {}
            contentItem: Node { node: workspace.catalogPage || workspace.calendarPage || workspace.rosterPage || workspace.formPage ? null : workspace.contentNode }
        }
    }
        Loader {
            visible: active; active: workspace.catalogPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.compact ? 8 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: Component { CatalogWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.formPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.compact ? 8 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: Component { FormGeneratorWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.rosterPage
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.minimumHeight: 0; Layout.minimumWidth: 0
            Layout.margins: Theme.compact ? 8 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: Component { RosterWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active
            active: workspace.calendarPage
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.margins: Theme.compact ? 8 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: Component { CalendarWorkspace { node: workspace.contentNode } }
        }
    }
    Loader {
        active: ui.profile === "SingleJumpsConfigWindow" || ui.profile === "CompetitionConfigWindow"
        Layout.fillWidth: true
        Layout.preferredHeight: active && item ? item.implicitHeight : 0
        sourceComponent: Component { SetupFooter {} }
    }
    Loader {
        active: workspace.season && ui.root.values.value > 0 && !(workspace.calendarPage && (workspace.width < 700 || workspace.height < 600))
        Layout.fillWidth: true
        Layout.preferredHeight: active && item ? item.implicitHeight : 0
        sourceComponent: Component { SeasonFooter {} }
    }
    ActionBar { visible: ui.actions.length > 0 && !workspace.formPage && !workspace.rosterPage; Layout.fillWidth: true }
}
