import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: popup
    property var backend
    property string operation
    property var sources: ({entries:[],seasons:[],seasonIndex:-1})
    property int sourceIndex: 0
    parent: Overlay.overlay
    modal: true; dim: false; focus: true
    margins: 12
    onOpened: {x=Math.max(12,Math.min(x,parent.width-width-12));y=Math.max(12,Math.min(y,parent.height-height-12))}
    width: Math.min(Theme.dp(420),parent ? parent.width-24 : Theme.dp(420))
    padding: Theme.dp(14)
    implicitHeight: contentItem.implicitHeight+2*padding
    function openAt(action,button) {
        operation=action;sources=backend.toolSources(action,-1);sourceIndex=0
        var point=button.mapToItem(parent,0,button.height+6)
        x=Math.max(12,Math.min(point.x,parent.width-width-12))
        y=Math.max(12,Math.min(point.y,parent.height-implicitHeight-12))
        open()
    }
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.accent; strokeWidth: 1 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(10)
        Label {
            text: popup.operation==="copy-order" ? qsTr("Kolejność z konkursu") : popup.operation==="copy-exact" ? qsTr("Lista z konkursu") : popup.operation==="classification" ? qsTr("Sortuj klasyfikacją") : qsTr("Sortuj wynikami konkursu")
            color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true
            Layout.fillWidth: true; wrapMode: Text.WordWrap
        }
        SChoice {
            visible: popup.sources.seasons.length>1
            Layout.fillWidth: true; Layout.minimumWidth: 0
            model: popup.sources.seasons
            currentIndex: popup.sources.seasonIndex
            Accessible.name: qsTr("Sezon")
            onActivated: {popup.sources=popup.backend.toolSources(popup.operation,currentIndex);popup.sourceIndex=0}
        }
        SChoice {
            visible: popup.sources.entries.length>0
            Layout.fillWidth: true; Layout.minimumWidth: 0
            searchable: true
            model: popup.sources.entries; textRole: "label"
            optionImages: popup.sources.entries.map(function(entry){return entry.flag})
            currentIndex: popup.sourceIndex
            Accessible.name: popup.operation==="classification" ? qsTr("Klasyfikacja") : qsTr("Konkurs")
            onActivated: popup.sourceIndex=currentIndex
        }
        Label { visible: popup.sources.entries.length===0; text: qsTr("Brak dostępnych źródeł w tym sezonie."); color: Theme.muted; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: popup.operation==="copy-exact"; text: qsTr("Zastąpi uczestników i kolejność. Składy drużyn pozostaną."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            SButton { text: qsTr("Anuluj"); quiet: true; onClicked: popup.close() }
            SButton { text: popup.operation==="copy-exact" ? qsTr("Zastąp listę") : qsTr("Zastosuj"); primary: true; enabled: popup.sources.entries.length>0; onClicked: {popup.backend.sourceTool(popup.operation,popup.sources.seasonIndex,popup.sourceIndex);popup.close()} }
        }
    }
}
