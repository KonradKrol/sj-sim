import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: studio
    objectName: "athleteStudio"
    property var backend
    property bool copying: false
    property bool renaming: false
    property bool showList: false
    readonly property bool wide: width>=Theme.dp(970)
    property string query: ""
    readonly property var rows: backend.profiles.filter(function(p){return p.label.toLocaleLowerCase().indexOf(studio.query.toLocaleLowerCase())>=0})
    spacing: Theme.dp(10)
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(8)
        SChoice { Layout.fillWidth: true; model: studio.backend.packs.map(function(p){return p.name+" · "+p.count}); currentIndex: studio.backend.packIndex; Accessible.name: qsTr("Paczka nowego modelu"); onActivated: studio.backend.selectPack(index) }
        SButton {
            id: packTools
            objectName: "athletePackTools"
            text: qsTr("Paczka…")
            onClicked: tools.open()
            ActionMenu {
                id: tools
                y: packTools.height
                entries: [{key:"new",text:qsTr("Nowa paczka")},{key:"rename",text:qsTr("Zmień nazwę")},{key:"copy",text:qsTr("Skopiuj ze starego modelu")},{key:"import",text:qsTr("Importuj paczkę")},{key:"export",text:qsTr("Eksportuj ze zdjęciami")}]
                onSelected: {
                    if(entry.key==="new")studio.backend.createPack()
                    else if(entry.key==="rename")studio.renaming=!studio.renaming
                    else if(entry.key==="copy")studio.copying=!studio.copying
                    else if(entry.key==="import")studio.backend.importPack()
                    else if(entry.key==="export")studio.backend.exportPack()
                }
            }
        }
        HelpDot { explanation: qsTr("Osobny zapis, niezależny od starej bazy i silnika. Nowi skoczkowie nie startują jeszcze w konkursach. Zmiany zapisują się od razu; możesz je cofać.") }
    }
    SField { visible: studio.renaming; Layout.fillWidth: true; text: studio.backend.packName; maximumLength: 100; Accessible.name: qsTr("Nazwa paczki"); onEditingFinished: {studio.backend.renamePack(text);studio.renaming=false} }
    Loader {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        active: studio.copying; visible: active
        sourceComponent: Component {
            ScrollView {
                id: copyScroll
                contentWidth: availableWidth; clip: true
                AthleteCopy { width: copyScroll.availableWidth; backend: studio.backend; onFinished: studio.copying=false }
            }
        }
    }
    RowLayout {
        visible: !studio.copying
        Layout.fillWidth: true
        SButton { objectName: "athleteAdd"; text: qsTr("+ Skoczek"); primary: true; onClicked: {studio.backend.add();studio.showList=false} }
        SButton { visible: !studio.wide && studio.backend.profiles.length>0; text: studio.showList ? qsTr("Edytor") : qsTr("Lista (%1)").arg(studio.backend.profiles.length); onClicked: studio.showList=!studio.showList }
        SButton { text: "↶"; Accessible.name: qsTr("Cofnij"); hint: qsTr("Cofnij"); quiet: true; enabled: studio.backend.canUndo; onClicked: studio.backend.undo() }
        SButton { text: "↷"; Accessible.name: qsTr("Ponów"); hint: qsTr("Ponów"); quiet: true; enabled: studio.backend.canRedo; onClicked: studio.backend.redo() }
        Item { Layout.fillWidth: true }
        Label { visible: studio.width>Theme.dp(650); text: studio.backend.message || qsTr("Osobny zapis · bez zmian w symulacji"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap; maximumLineCount: 3; elide: Text.ElideRight }
    }
    Label { visible: !studio.copying && studio.width<=Theme.dp(650) && text.length>0; text: studio.backend.message; Layout.fillWidth: true; color: Theme.muted; font.pointSize: Theme.caption; maximumLineCount: 2; elide: Text.ElideRight; wrapMode: Text.WordWrap }
    RowLayout {
        visible: !studio.copying
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: Theme.dp(16)
        ColumnLayout {
            visible: studio.wide || studio.showList
            Layout.preferredWidth: Theme.dp(220); Layout.maximumWidth: studio.wide ? Theme.dp(280) : Infinity
            Layout.fillWidth: !studio.wide; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: Theme.dp(8)
            SField { Layout.fillWidth: true; placeholderText: qsTr("Znajdź skoczka"); Accessible.name: placeholderText; onTextEdited: studio.query=text }
            ListView {
                objectName: "athleteStudioList"
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                clip: true; spacing: Theme.dp(5); model: studio.rows
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                delegate: AbstractButton {
                    id: row
                    required property var modelData
                    width: ListView.view.width; implicitHeight: contentItem.implicitHeight+Theme.dp(20); padding: Theme.dp(10)
                    checked: studio.backend.selectedIndex===modelData.index
                    Accessible.name: modelData.label; Accessible.checked: checked
                    background: Rectangle { color: row.checked ? Theme.stadium : row.hovered ? Theme.selection : Theme.surface; radius: Theme.dp(7); border.width: row.activeFocus ? 2 : 0; border.color: Theme.accent }
                    contentItem: RowLayout {
                        spacing: Theme.dp(9)
                        Image { source: row.modelData.flag; Layout.preferredWidth: Theme.dp(25); Layout.preferredHeight: Theme.dp(16); sourceSize.width: 50; sourceSize.height: 32; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                        ColumnLayout {
                            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                            Label { text: row.modelData.name || qsTr("Nowy"); Layout.fillWidth: true; elide: Text.ElideRight; color: row.checked ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption }
                            Label { text: row.modelData.surname || qsTr("skoczek"); Layout.fillWidth: true; elide: Text.ElideRight; color: row.checked ? Theme.snow : Theme.ink; font.pointSize: Theme.body }
                        }
                        Label { text: row.modelData.overall; color: row.checked ? Theme.signal : Theme.accent; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
                    }
                    onClicked: {studio.backend.select(modelData.index);studio.showList=false}
                }
                EmptyState { anchors.fill: parent; visible: studio.rows.length===0; compact: true; title: studio.query.length ? qsTr("Brak dopasowań") : qsTr("Pusta paczka"); description: studio.query.length ? qsTr("Zmień wyszukiwanie.") : qsTr("Dodaj skoczka lub skopiuj istniejącego.") }
            }
            Flow {
                Layout.fillWidth: true; spacing: 2
                SButton { text: qsTr("Duplikuj"); quiet: true; enabled: studio.backend.selectedIndex>=0; onClicked: studio.backend.duplicate() }
                SButton { text: qsTr("Usuń"); quiet: true; enabled: studio.backend.selectedIndex>=0; onClicked: studio.backend.remove() }
            }
        }
        Loader {
            active: studio.backend.selectedIndex>=0
            visible: active && (studio.wide || !studio.showList)
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            sourceComponent: Component { AthleteEditor { objectName: "athleteEditor"; backend: studio.backend } }
        }
        EmptyState {
            visible: studio.backend.selectedIndex<0 && (studio.wide || !studio.showList)
            Layout.fillWidth: true; Layout.fillHeight: true
            title: qsTr("Stwórz pierwszego skoczka")
            description: qsTr("OVR, forma, lądowanie i trzy pary perków. Zdjęcie jest opcjonalne.")
            actionText: qsTr("Dodaj skoczka"); onTriggered: studio.backend.add()
        }
    }
}
