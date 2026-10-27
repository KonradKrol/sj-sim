import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: studio
    objectName: "athleteStudio"
    property var backend
    property bool copying: false
    property bool renaming: false
    property bool editing: false
    readonly property bool parallel: width>=Theme.dp(940)
    spacing: Theme.dp(8)
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(780) ? 2 : 1
        columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(6)
        SChoice {
            Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(220)
            model: studio.backend.packs.map(function(p){return p.name+" · "+p.count})
            currentIndex: studio.backend.packIndex; Accessible.name: qsTr("Paczka nowego modelu")
            onActivated: studio.backend.selectPack(index)
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(4)
            SButton { objectName: "athleteAdd"; text: qsTr("+ Skoczek"); primary: true; onClicked: {studio.backend.add();studio.editing=true} }
            SButton { objectName: "athleteUndo"; text: "↶"; Accessible.name: qsTr("Cofnij"); hint: qsTr("Cofnij"); quiet: true; enabled: studio.backend.canUndo; onClicked: studio.backend.undo() }
            SButton { objectName: "athleteRedo"; text: "↷"; Accessible.name: qsTr("Ponów"); hint: qsTr("Ponów"); quiet: true; enabled: studio.backend.canRedo; onClicked: studio.backend.redo() }
            Item { Layout.fillWidth: true }
            SButton {
                id: packTools
                objectName: "athletePackTools"
                text: "•••"; Accessible.name: qsTr("Narzędzia paczki i skoczka")
                onClicked: tools.open()
                ActionMenu {
                    id: tools; y: packTools.height
                    entries: [{key:"copy",text:qsTr("Skopiuj ze starego modelu")},{key:"duplicate",text:qsTr("Duplikuj skoczka"),enabled:studio.backend.selectedIndex>=0},{key:"remove",text:qsTr("Usuń skoczka"),enabled:studio.backend.selectedIndex>=0},{key:"new",text:qsTr("Nowa paczka")},{key:"rename",text:qsTr("Zmień nazwę paczki")},{key:"folder",text:qsTr("Otwórz folder paczki")},{key:"import",text:qsTr("Importuj paczkę")},{key:"export",text:qsTr("Eksportuj paczkę")}]
                    onSelected: {
                        if(entry.key==="copy"){studio.copying=!studio.copying;studio.editing=true}
                        else if(entry.key==="duplicate")studio.backend.duplicate()
                        else if(entry.key==="remove")studio.backend.remove()
                        else if(entry.key==="new")studio.backend.createPack()
                        else if(entry.key==="rename")studio.renaming=!studio.renaming
                        else if(entry.key==="folder")studio.backend.openPackFolder()
                        else if(entry.key==="import")studio.backend.importPack()
                        else if(entry.key==="export")studio.backend.exportPack()
                    }
                }
            }
            HelpDot { explanation: qsTr("Nowy model ma osobny zapis i nie zmienia jeszcze symulacji. Zmiany zapisują się od razu; możesz je cofać.") }
        }
    }
    SField { visible: studio.renaming; Layout.fillWidth: true; text: studio.backend.packName; maximumLength: 100; Accessible.name: qsTr("Nazwa paczki"); onEditingFinished: {studio.backend.renamePack(text);studio.renaming=false} }
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
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                active: studio.copying; visible: active
                sourceComponent: Component { ScrollView { id: copyScroll; contentWidth: availableWidth; clip: true; AthleteCopy { width: copyScroll.availableWidth; backend: studio.backend; onFinished: studio.copying=false } } }
            }
            Loader {
                active: studio.backend.selectedIndex>=0
                visible: active && !studio.copying
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                sourceComponent: Component { AthleteEditor { objectName: "athleteEditor"; backend: studio.backend } }
            }
            EmptyState {
                visible: studio.backend.selectedIndex<0 && !studio.copying
                Layout.fillWidth: true; Layout.fillHeight: true
                title: qsTr("Stwórz pierwszego skoczka")
                description: qsTr("OVR, forma, lądowanie i trzy pary perków. Zdjęcie jest opcjonalne.")
                actionText: qsTr("Dodaj skoczka"); onTriggered: {studio.backend.add();studio.editing=true}
            }
        }
    }
}
