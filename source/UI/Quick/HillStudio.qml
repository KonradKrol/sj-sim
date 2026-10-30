import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: studio
    objectName: "hillStudio"
    property var backend
    property bool editing: false
    property bool copying: false
    readonly property bool parallel: width>=Theme.dp(920)
    spacing: Theme.dp(6)
    StudioPackBar { Layout.fillWidth: true; backend: studio.backend }
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(4)
            SButton { objectName: "hillAdd"; text: qsTr("+ Skocznia"); primary: true; onClicked: {studio.backend.add();studio.editing=true} }
            SButton { text: "↶"; Accessible.name: qsTr("Cofnij"); hint: Accessible.name; quiet: true; enabled: studio.backend.canUndo; onClicked: studio.backend.undo() }
            SButton { text: "↷"; Accessible.name: qsTr("Ponów"); hint: Accessible.name; quiet: true; enabled: studio.backend.canRedo; onClicked: studio.backend.redo() }
            Item { Layout.fillWidth: true }
            SButton {
                id: toolsButton
                text: qsTr("Opcje skoczni"); Accessible.name: qsTr("Opcje skoczni"); onClicked: tools.open()
                ActionMenu {
                    id: tools; y: toolsButton.height
                    entries: [{key:"copy",text:qsTr("Skopiuj z klasycznego modelu")},{key:"duplicate",text:qsTr("Duplikuj skocznię"),enabled:studio.backend.selectedIndex>=0},{key:"remove",text:qsTr("Usuń skocznię"),enabled:studio.backend.selectedIndex>=0}]
                    onSelected: {if(entry.key==="copy")studio.copying=!studio.copying;else if(entry.key==="duplicate")studio.backend.duplicate();else if(entry.key==="remove")studio.backend.remove()}
                }
            }
        }
    RowLayout {
        visible: studio.copying; Layout.fillWidth: true
        SChoice { id: legacy; Layout.fillWidth: true; searchable: true; model: studio.backend.legacyHills.map(function(p){return p.name}); Accessible.name: qsTr("Skocznia do skopiowania") }
        SButton { text: qsTr("Utwórz kopię"); enabled: legacy.currentIndex>=0; onClicked: {studio.backend.copyLegacy(legacy.currentIndex);studio.copying=false;studio.editing=true} }
    }
    Label { visible: text.length>0 && text!==qsTr("Zapisano"); text: studio.backend.message; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    SectionTabs { visible: !studio.parallel; options: [qsTr("Lista"),qsTr("Edycja")]; currentIndex: studio.editing?1:0; onSelected: studio.editing=index===1; Layout.fillWidth: true }
    RowLayout {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: Theme.dp(14)
        HillLibrary { backend: studio.backend; visible: studio.parallel || !studio.editing; Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: Theme.dp(280); Layout.maximumWidth: studio.parallel?Theme.dp(320):Infinity; Layout.minimumHeight: 0; onActivated: studio.editing=true }
        Loader { active: studio.backend.selectedIndex>=0; visible: active && (studio.parallel || studio.editing); Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: Theme.dp(950); sourceComponent: Component { HillEditor { backend: studio.backend } } }
        EmptyState { visible: studio.backend.selectedIndex<0 && (studio.parallel || studio.editing); Layout.fillWidth: true; Layout.fillHeight: true; title: qsTr("Stwórz pierwszą skocznię"); description: qsTr("Punkt K, HS i perki. Dodaj skocznię lub zaimportuj starsze dane."); actionText: qsTr("Dodaj skocznię"); onTriggered: {studio.backend.add();studio.editing=true} }
    }
}
