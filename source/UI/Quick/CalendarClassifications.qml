import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: classes
    ViewArrival { target: classes; expressive: false }
    objectName: "calendarClassesWorkspace"
    property var backend
    property var editorNode
    property bool editing: false
    readonly property bool compact: width<800
    readonly property bool tight: height<300
    spacing: 8
    SButton { visible: classes.tight && classes.editing; text: "←"; Accessible.name: qsTr("Lista"); quiet: true; implicitHeight: 32; onClicked: classes.editing=false }
    SectionTabs { visible: classes.compact && !classes.tight; Layout.fillWidth: true; options: [qsTr("Lista"),qsTr("Edycja")]; currentIndex: classes.editing?1:0; onSelected: classes.editing=index===1 }
    RowLayout {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: 16
        ColumnLayout {
            visible: !classes.compact || !classes.editing
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 280
            Flow {
                Layout.fillWidth: true; spacing: 6
                SButton { visible: !classes.tight || !classes.editing; objectName: "calendarClassAdd"; text: qsTr("Dodaj klasyfikację"); Accessible.name: qsTr("Dodaj klasyfikację"); primary: !classes.tight; implicitHeight: classes.tight?32:48; font.pointSize: classes.tight?Theme.caption:Theme.section; enabled: classes.backend.calendarIndex>=0; onClicked: {classes.backend.classAction("add");classes.editing=true} }
                SButton { visible: !classes.tight; text: qsTr("Utwórz kopię"); enabled: !!classes.backend.classId; onClicked: classes.backend.classAction("copy") }
                SButton { visible: !classes.tight; text: qsTr("Usuń"); enabled: !!classes.backend.classId && !classes.backend.classLocked; onClicked: classes.backend.classAction("remove") }
                SButton { text: qsTr("Cofnij"); implicitHeight: classes.tight?32:40; font.pointSize: classes.tight?Theme.caption:Theme.body; enabled: classes.backend.canUndo; onClicked: classes.backend.undo() }
                SButton { visible: !classes.tight; text: qsTr("Ponów"); enabled: classes.backend.canRedo; onClicked: classes.backend.redo() }
                SButton {
                    visible: classes.tight; text: "⋯"; quiet: true; implicitHeight: 32; Accessible.name: qsTr("Opcje klasyfikacji")
                    onClicked: classMenu.open()
                    ActionMenu {
                        id: classMenu
                        entries: [{id:"copy",text:qsTr("Utwórz kopię"),enabled:!!classes.backend.classId},{id:"remove",text:qsTr("Usuń"),enabled:!!classes.backend.classId&&!classes.backend.classLocked},{id:"redo",text:qsTr("Ponów"),enabled:classes.backend.canRedo}]
                        onSelected: entry.id==="redo"?classes.backend.redo():classes.backend.classAction(entry.id)
                    }
                }
            }
            Label { visible: !!classes.backend.classId; text: qsTr("Zmiany zapisują się od razu w kalendarzu."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            ListView {
                id: entries
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                clip: true; model: classes.backend.classEntries; spacing: 4
                ScrollBar.vertical: ScrollBar {}
                EmptyState { parent: entries; anchors.fill: parent; compact: parent.height<150; visible: entries.count===0; title: qsTr("Brak klasyfikacji"); description: qsTr("Klasyfikacja sumuje wyniki wybranych konkursów. Dodaj ją, a następnie przypisz konkursy w sekcji Powiązania."); actionText: classes.backend.calendarIndex>=0 ? qsTr("Dodaj klasyfikację") : ""; onTriggered: {classes.backend.classAction("add");classes.editing=true} }
                delegate: ItemDelegate {
                    width: ListView.view.width; height: 68
                    contentItem: Column {
                        Label { width: parent.width; text: modelData.name; color: Theme.ink; font.pointSize: Theme.body; elide: Text.ElideRight }
                        Label { text: modelData.team?qsTr("Drużynowa"):qsTr("Indywidualna"); color: Theme.muted; font.pointSize: Theme.caption }
                    }
                    background: Rectangle { color: modelData.selected?Theme.selection:Theme.surface; radius: 6 }
                    onClicked: {classes.backend.selectClass(modelData.id);classes.editing=true}
                }
            }
        }
        ColumnLayout {
            visible: !!classes.backend.classId && (!classes.compact || classes.editing)
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.preferredWidth: 600
            Label { visible: classes.backend.classLocked; text: qsTr("Wyniki są już rozegrane. Możesz zmienić nazwę; aby zmienić punktację, utwórz kopię."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            ScrollView { id: editor; objectName: "calendarClassEditorScroll"; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; contentWidth: availableWidth; clip: true; Loader { width: editor.availableWidth; source: Qt.resolvedUrl("Node.qml"); onLoaded: item.node=Qt.binding(function(){return classes.editorNode}) } }
        }
    }
}
