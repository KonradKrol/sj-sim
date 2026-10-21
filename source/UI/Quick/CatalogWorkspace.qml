import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: catalog
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var editor: fields.editor || null
    readonly property var list: fields.list ? fields.list.values : ({})
    readonly property bool parallel: width>=850
    property bool editing: false
    readonly property int editRequest: fields.editRequest || 0
    onEditRequestChanged: if(editRequest>0)editing=true
    onEditorChanged: if(editor)editing=true
    spacing: 8
    Node { node: catalog.fields.toolbar || null; Layout.alignment: Qt.AlignRight; Layout.maximumWidth: catalog.editor || ui.profile!=="SimulationSavesWindow" ? 280 : 600; Layout.fillWidth: true }
    RowLayout {
        visible: !!catalog.fields.actionsRow && catalog.fields.actionsRow.length>0
        Layout.fillWidth: true
        Layout.maximumWidth: 600
        Layout.alignment: Qt.AlignRight
        Repeater { model: catalog.fields.actionsRow || []; Node { node: modelData; Layout.fillWidth: true; Layout.minimumWidth: 0 } }
    }
    SectionTabs {
        visible: !catalog.parallel && !!catalog.editor
        Layout.fillWidth: true
        options: [qsTr("Lista"),qsTr("Edycja")]
        currentIndex: catalog.editing ? 1 : 0
        onSelected: catalog.editing=index===1
    }
    RowLayout {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        spacing: 20
        DataTable {
            visible: catalog.parallel || !catalog.editing || !catalog.editor
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: catalog.editor ? 380 : 1000
            table: catalog.list.table || null; ownerId: catalog.list.id || -1
            actions: catalog.list.tableActions || []
        }
        ScrollView {
            id: editorScroll
            objectName: "catalogEditorScroll"
            visible: !!catalog.editor && (catalog.parallel || catalog.editing)
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            Layout.preferredWidth: 620
            contentWidth: availableWidth; clip: true
            Node { width: editorScroll.availableWidth; node: catalog.editor }
        }
    }
}
