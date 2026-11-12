import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: catalog
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var editor: fields.editor || null
    readonly property bool draftEditor: !!editor && editor.values.kind === "simulationDraft"
    readonly property var list: fields.list ? fields.list.values : ({})
    readonly property bool savesCatalog: ui.profile === "SimulationSavesWindow"
    readonly property bool parallel: width>=Theme.dp(900)
    property bool editing: false
    readonly property bool hasHillStudio: !!fields.hillStudio
    readonly property bool hasStudio: !!fields.athleteStudio || hasHillStudio
    readonly property bool studioActive: hasStudio
    property var athleteBackend: null
    property var hillBackend: null
    readonly property var seasonBackend: fields.seasonBackend || null
    onFieldsChanged: {
        if (fields.athleteStudio) athleteBackend = fields.athleteStudio
        if (fields.hillStudio) hillBackend = fields.hillStudio
    }
    readonly property int editRequest: fields.editRequest || 0
    onEditRequestChanged: if(editRequest>0)editing=true
    onEditorChanged: if(editor)editing=true
    QtObject {
        id: seasonLibrary
        readonly property bool available: !!catalog.fields.seasonLibrary
        readonly property bool hill: !!catalog.fields.seasonHill
        readonly property var profiles: catalog.fields.seasonProfiles || []
        readonly property int selectedIndex: {
            var selected=catalog.fields.seasonSelectedId || ""
            for(var i=0;i<profiles.length;++i)if(profiles[i].id===selected)return profiles[i].index
            return -1
        }
        readonly property var profile: catalog.editor && catalog.editor.values.backend ? catalog.editor.values.backend.profile : ({})
        readonly property string packDirectory: "season"
        readonly property int packIndex: 0
        signal uiChanged()
        function select(index) {
            catalog.forceActiveFocus()
            for(var i=0;i<profiles.length;++i)if(profiles[i].index===index){
                if(catalog.seasonBackend)catalog.seasonBackend.selectSeasonProfile(profiles[i].id,!!catalog.fields.seasonHill)
                return
            }
        }
        function showStatistics(){var button=ui.find("pushButton_jumperStats");if(button)ui.activate(button.values.id)}
        function remove() {}
        function move(from,to) {}
        function sortByCountry() {}
        onProfilesChanged: Qt.callLater(function(){seasonLibrary.uiChanged()})
        onSelectedIndexChanged: Qt.callLater(function(){seasonLibrary.uiChanged()})
    }
    spacing: Theme.compact ? 4 : 8
    Loader {
        visible: active
        active: catalog.studioActive
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
        sourceComponent: catalog.hasHillStudio ? hillStudio : athleteStudio
    }
    Component { id: hillStudio; HillStudio { backend: catalog.hillBackend } }
    Component { id: athleteStudio; AthleteStudio { backend: catalog.athleteBackend } }
    SeasonRosterChoice { visible: !!catalog.fields.rosterBackend; backend: catalog.fields.rosterBackend || null; controlName: "seasonActiveRosterCatalog"; Layout.fillWidth: true; Layout.maximumWidth: Theme.dp(560) }
    Loader {
        visible: active
        active: !catalog.studioActive && ui.profile === "DatabaseEditorWindow"
        Layout.fillWidth: true
        sourceComponent: Component { DataPackBar {} }
    }
    Node { visible: !catalog.studioActive && !catalog.fields.singleClickEdit; node: catalog.fields.toolbar || null; Layout.alignment: Qt.AlignRight; Layout.maximumWidth: catalog.editor || ui.profile!=="SimulationSavesWindow" ? 280 : 600; Layout.fillWidth: true }
    RowLayout {
        visible: !catalog.studioActive && !catalog.fields.singleClickEdit && !!catalog.fields.actionsRow && catalog.fields.actionsRow.length>0
        Layout.fillWidth: true
        Layout.maximumWidth: 600
        Layout.alignment: Qt.AlignRight
        HelpDot {
            visible: catalog.savesCatalog
            explanation: qsTr("Ctrl+klik zaznacza kilka zapisów, Shift+klik zaznacza zakres. Przycisk Usuń usuwa wszystkie zaznaczone zapisy po potwierdzeniu.")
        }
        Repeater { model: catalog.fields.actionsRow || []; Node { node: modelData; Layout.fillWidth: true; Layout.minimumWidth: 0 } }
    }
    SectionTabs {
        visible: !catalog.studioActive && !catalog.parallel && !!catalog.editor
        Layout.fillWidth: true
        options: [qsTr("Lista"),qsTr("Edycja")]
        currentIndex: catalog.editing ? 1 : 0
        onSelected: catalog.editing=index===1
    }
    RowLayout {
        visible: !catalog.studioActive
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        spacing: Theme.compact ? 8 : 20
        DataTable {
            visible: !catalog.fields.seasonLibrary && (catalog.parallel || !catalog.editing || !catalog.editor)
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: catalog.editor ? 380 : 1000
            table: catalog.list.table || null; ownerId: catalog.list.id || -1
            ranking: catalog.savesCatalog
            searchOpen: !catalog.savesCatalog && searchEnabled && width >= 320 && height >= 240
            searchEnabled: true
            actions: catalog.list.tableActions || []
            expandedActions: !!catalog.fields.singleClickEdit
            singleClickOpen: !!catalog.fields.singleClickEdit
        }
        Loader {
            visible: active && (catalog.parallel || !catalog.editing)
            active: !!catalog.fields.seasonLibrary
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 380
            sourceComponent: catalog.fields.seasonHill ? seasonHills : seasonAthletes
            Component { id: seasonAthletes; AthleteLibrary { backend: seasonLibrary; season: true; loadViewState: function(){var owner=catalog.fields.seasonBackend;return owner ? owner.seasonLibraryState(false) : ({})}; saveViewState: function(state){if(catalog.seasonBackend)catalog.seasonBackend.rememberSeasonLibraryState(false,state)}; onActivated: catalog.editing=true } }
            Component { id: seasonHills; HillLibrary { backend: seasonLibrary; season: true; loadViewState: function(){var owner=catalog.fields.seasonBackend;return owner ? owner.seasonLibraryState(true) : ({})}; saveViewState: function(state){if(catalog.seasonBackend)catalog.seasonBackend.rememberSeasonLibraryState(true,state)}; onActivated: catalog.editing=true } }
        }
        ScrollView {
            id: editorScroll
            objectName: "catalogEditorScroll"
            visible: !catalog.draftEditor && !!catalog.editor && (catalog.parallel || catalog.editing)
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: 620
            contentWidth: availableWidth; clip: true
            Node { width: editorScroll.availableWidth; node: catalog.editor }
        }
        Loader {
            visible: active
            active: catalog.draftEditor && (catalog.parallel || catalog.editing)
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: 620
            sourceComponent: Component { SimulationDraftWorkspace { node: catalog.editor } }
        }
    }
}
