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
    readonly property bool parallel: width>=850
    property bool editing: false
    readonly property bool hasHillStudio: !!fields.hillStudio
    readonly property bool hasStudio: !!fields.athleteStudio || hasHillStudio
    readonly property bool studioActive: hasStudio
    readonly property int editRequest: fields.editRequest || 0
    onEditRequestChanged: if(editRequest>0)editing=true
    onEditorChanged: if(editor)editing=true
    spacing: Theme.compact ? 4 : 8
    Loader {
        visible: active
        active: catalog.studioActive
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
        sourceComponent: catalog.hasHillStudio ? hillStudio : athleteStudio
    }
    Component { id: hillStudio; HillStudio { backend: catalog.fields.hillStudio } }
    Component { id: athleteStudio; AthleteStudio { backend: catalog.fields.athleteStudio } }
    Loader {
        visible: active
        active: !catalog.studioActive && ui.profile === "DatabaseEditorWindow"
        Layout.fillWidth: true
        sourceComponent: Component { DataPackBar {} }
    }
    Node { visible: !catalog.studioActive; node: catalog.fields.toolbar || null; Layout.alignment: Qt.AlignRight; Layout.maximumWidth: catalog.editor || ui.profile!=="SimulationSavesWindow" ? 280 : 600; Layout.fillWidth: true }
    RowLayout {
        visible: !catalog.studioActive && !!catalog.fields.actionsRow && catalog.fields.actionsRow.length>0
        Layout.fillWidth: true
        Layout.maximumWidth: 600
        Layout.alignment: Qt.AlignRight
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
            visible: catalog.parallel || !catalog.editing || !catalog.editor
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: catalog.editor ? 380 : 1000
            table: catalog.list.table || null; ownerId: catalog.list.id || -1
            searchEnabled: true
            actions: catalog.list.tableActions || []
            expandedActions: !!catalog.fields.singleClickEdit
            singleClickOpen: !!catalog.fields.singleClickEdit
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
