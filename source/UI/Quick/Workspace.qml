import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    Shortcut {
        sequence: "Ctrl+Shift+F"
        enabled: workspace.visible && ui.profile==="SimulationSaveManagerWindow"
        onActivated: {var action=ui.find("developerAdvanceForm");if(action)ui.activate(action.values.id)}
    }

    readonly property bool tabbed: ui.root.values.kind === "tabs"
    readonly property bool season: ui.profile === "SimulationSaveManagerWindow"
    readonly property var contentNode: tabbed && ui.root.children.length ? ui.root.children[0] : ui.root
    readonly property bool libraryPage: contentNode && contentNode.values.kind === "resourceLibrary"
    readonly property bool jumpPage: contentNode && contentNode.values.kind === "jumpManipulation"
    readonly property bool weatherPage: contentNode && contentNode.values.kind === "weatherSetup"
    readonly property bool simulationDraftPage: contentNode && contentNode.values.kind === "simulationDraft"
    readonly property bool formPage: contentNode && contentNode.values.kind === "formGenerator"
    readonly property bool koPage: contentNode && contentNode.values.kind === "koGroups"
    readonly property bool rosterPage: contentNode && contentNode.values.kind === "roster"
    readonly property bool catalogPage: contentNode && (contentNode.values.kind === "catalog" || contentNode.values.kind === "seasonRosters")
    readonly property bool calendarPage: contentNode && contentNode.values.kind === "calendar"
    readonly property bool sideTabs: tabbed && width >= 1200 && height >= 550
    property bool rebuildingOverview: false
    readonly property string seasonScaleKey: appearance.uiScale+":"+appearance.textScale
    onSeasonScaleKeyChanged: if(season){rebuildingOverview=true;Qt.callLater(function(){workspace.rebuildingOverview=false})}
    spacing: 0
    Loader {
        active: workspace.season
        Layout.fillWidth: true
        Layout.preferredHeight: active && item ? item.implicitHeight : 0
        Layout.minimumHeight: active && item ? item.implicitHeight : 0
        Layout.maximumHeight: active && item ? item.implicitHeight : 0
        Layout.fillHeight: false
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
        visible: !workspace.libraryPage && !workspace.jumpPage && !workspace.koPage && !workspace.catalogPage && !workspace.calendarPage && !workspace.rosterPage && !workspace.formPage && !workspace.simulationDraftPage && !workspace.weatherPage
        objectName: "workspaceScroll"
        Layout.fillWidth: true; Layout.fillHeight: true
        Layout.minimumWidth: 0; Layout.minimumHeight: 0
        Layout.preferredWidth: 0; Layout.preferredHeight: 0
        contentWidth: availableWidth
        clip: true
        Pane {
            width: scroll.availableWidth
            padding: Theme.compact ? 4 : workspace.width < 700 ? 12 : 24
            background: Item {}
            contentItem: Loader { active: !workspace.rebuildingOverview; sourceComponent: Component { Node { node: workspace.contentNode; tableHeightLimit: workspace.season ? Math.max(180,scroll.availableHeight-(Theme.compact ? 8 : workspace.width<700 ? 24 : 48)-Theme.dp(100)) : 540 } } }
        }
    }
        Loader {
            visible: active; active: workspace.libraryPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.margins: Theme.dp(16)
            sourceComponent: Component { ResourceLibrary { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.jumpPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.dp(8)
            sourceComponent: Component { JumpManipulationWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.weatherPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.dp(8)
            sourceComponent: Component { WeatherWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.simulationDraftPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.dp(8)
            sourceComponent: Component { SimulationDraftWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.koPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.dp(12)
            sourceComponent: Component { KoGroupsWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.catalogPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.compact || (workspace.contentNode.values.kind === "seasonRosters" && workspace.height < 600) ? 4 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: workspace.contentNode.values.kind === "seasonRosters" ? rosterCatalog : normalCatalog
            Component { id: rosterCatalog; SeasonRostersWorkspace { node: workspace.contentNode } }
            Component { id: normalCatalog; CatalogWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.formPage
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.margins: Theme.compact ? 4 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: Component { FormGeneratorWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active; active: workspace.rosterPage
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.minimumHeight: 0; Layout.minimumWidth: 0
            Layout.margins: Theme.compact ? 4 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
            sourceComponent: workspace.contentNode && workspace.contentNode.values.backend && workspace.contentNode.values.backend.savedList ? savedRoster : competitionRoster
            Component { id: savedRoster; SavedRosterWorkspace { node: workspace.contentNode } }
            Component { id: competitionRoster; RosterWorkspace { node: workspace.contentNode } }
        }
        Loader {
            visible: active
            active: workspace.calendarPage
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.margins: Theme.compact ? 4 : workspace.width < 700 || workspace.height < 600 ? 12 : 20
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
        active: workspace.season && ui.root.values.value > 0 && !((workspace.calendarPage || (workspace.catalogPage && workspace.contentNode.values.kind === "seasonRosters")) && (workspace.width < 700 || workspace.height < 600))
        Layout.fillWidth: true
        Layout.preferredHeight: active && item ? item.implicitHeight : 0
        Layout.maximumHeight: active && item ? item.implicitHeight : 0
        sourceComponent: Component { SeasonFooter {} }
    }
    ActionBar { visible: ui.actions.length > 0 && !workspace.jumpPage && !workspace.formPage && !workspace.simulationDraftPage && !workspace.weatherPage && !workspace.rosterPage && !workspace.koPage; Layout.fillWidth: true }
}
