import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: studio
    objectName: "athleteStudio"
    property var backend
    property bool editing: false
    readonly property bool parallel: width>=Theme.dp(940)
    spacing: Theme.dp(8)
    StudioPackBar { Layout.fillWidth: true; backend: studio.backend; athletes: true }
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(4)
            SButton { objectName: "athleteAdd"; text: qsTr("+ Skoczek"); primary: true; onClicked: {studio.backend.add();studio.editing=true} }
            SButton { objectName: "athleteUndo"; text: "↶"; Accessible.name: qsTr("Cofnij"); hint: qsTr("Cofnij"); quiet: true; enabled: studio.backend.canUndo; onClicked: studio.backend.undo() }
            SButton { objectName: "athleteRedo"; text: "↷"; Accessible.name: qsTr("Ponów"); hint: qsTr("Ponów"); quiet: true; enabled: studio.backend.canRedo; onClicked: studio.backend.redo() }
            Item { Layout.fillWidth: true }
            SIconButton { objectName: "athleteDuplicate"; text: ""; iconName: "copy"; actionName: qsTr("Duplikuj skoczka"); enabled: studio.backend.selectedIndex>=0; onClicked: {studio.backend.duplicate();studio.editing=true} }
        }
    Label { visible: text.length>0 && text!==qsTr("Zapisano"); text: studio.backend.message; Layout.fillWidth: true; color: Theme.muted; font.pointSize: Theme.caption; maximumLineCount: 2; elide: Text.ElideRight; wrapMode: Text.WordWrap }
    SectionTabs {
        visible: !studio.parallel
        Layout.fillWidth: true
        options: [qsTr("Lista"),qsTr("Edycja")]
        currentIndex: studio.editing ? 1 : 0
        onSelected: studio.editing=index===1
    }
    RowLayout {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        spacing: Theme.dp(16)
        AthleteLibrary {
            backend: studio.backend
            visible: studio.parallel || !studio.editing
            Layout.preferredWidth: Theme.dp(300); Layout.maximumWidth: studio.parallel ? Theme.dp(340) : Infinity
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            onActivated: studio.editing=true
        }
        ColumnLayout {
            visible: studio.parallel || studio.editing
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: Theme.dp(1000)

            Loader {
                active: studio.backend.selectedIndex>=0
                visible: active
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                sourceComponent: Component { AthleteEditor { objectName: "athleteEditor"; backend: studio.backend } }
            }
            EmptyState {
                visible: studio.backend.selectedIndex<0
                Layout.fillWidth: true; Layout.fillHeight: true
                title: qsTr("Stwórz pierwszego skoczka")
                description: qsTr("OVR, forma, lądowanie i trzy pary perków. Zdjęcie jest opcjonalne.")
                actionText: qsTr("Dodaj skoczka"); onTriggered: {studio.backend.add();studio.editing=true}
            }
        }
    }
}
