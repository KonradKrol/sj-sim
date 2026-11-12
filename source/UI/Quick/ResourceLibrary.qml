import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: library
    objectName: "resourceLibraryView"
    property var node
    readonly property var backend: node ? node.values.backend : null
    property string category: ""
    readonly property var categories: [{id:"",name:qsTr("Wszystko")},{id:"Athletes",name:qsTr("Skoczkowie")},{id:"Hills",name:qsTr("Skocznie")},{id:"Rules",name:qsTr("Zasady")},{id:"Points",name:qsTr("Punktacje")},{id:"Calendars",name:qsTr("Kalendarze")},{id:"Saves",name:qsTr("Symulacje")}]
    spacing: Theme.dp(12)
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(8)
        SButton { text: qsTr("Wczytaj paczkę"); primary: true; onClicked: library.backend.importArchive() }
        SButton { text: qsTr("Eksportuj (%1)").arg(library.backend ? library.backend.selectedCount : 0); enabled: library.backend && library.backend.selectedCount>0; onClicked: library.backend.exportSelected() }
        SButton { text: qsTr("Wyczyść wybór"); visible: library.backend && library.backend.selectedCount>0; quiet: true; onClicked: library.backend.clearSelection() }
    }
    Label { Layout.fillWidth: true; text: qsTr("Zaznacz zasoby, które chcesz udostępnić. Zdjęcia i zależności kalendarzy zostaną dołączone."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(5)
        Repeater {
            model: library.categories
            SButton { required property var modelData; text: modelData.name; dense: true; primary: library.category===modelData.id; font.pointSize: Theme.body; font.family: Theme.bodyFamily; onClicked: library.category=modelData.id }
        }
    }
    ListView {
        id: resourceList
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: Theme.dp(220)
        clip: true; spacing: Theme.dp(4)
        model: library.backend ? library.backend.resources.filter(function(r){return !library.category || r.kind===library.category}) : []
        ScrollBar.vertical: ScrollBar {}
        delegate: ItemDelegate {
            required property var modelData
            width: resourceList.width; height: Math.max(Theme.dp(48),row.implicitHeight+Theme.dp(12))
            onClicked: library.backend.toggle(modelData.key)
            background: Rectangle { color: modelData.selected?Theme.selection:Theme.surface; radius: Theme.radius; border.color: modelData.selected?Theme.accent:Theme.divider }
            contentItem: RowLayout {
                id: row; spacing: Theme.dp(12)
                Label { text: modelData.selected?"☑":"☐"; color: modelData.selected?Theme.accent:Theme.muted; font.pointSize: Theme.section }
                ColumnLayout {
                    Layout.fillWidth: true; spacing: Theme.dp(2)
                    Label { text: modelData.name; color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                    Label { text: modelData.category; color: Theme.muted; font.pointSize: Theme.caption }
                }
                Label { text: modelData.count>0?modelData.count:""; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.body }
            }
        }
        EmptyState { anchors.centerIn: parent; width: parent.width; visible: resourceList.count===0; title: qsTr("Brak zasobów"); description: qsTr("Wczytaj paczkę lub utwórz zasoby w edytorze.") }
        DropArea { anchors.fill: parent; onEntered: if(!drag.hasUrls || drag.urls.length!==1 || !String(drag.urls[0]).toLowerCase().endsWith(".sjsim-pack"))drag.accepted=false; onDropped: library.backend.importFile(String(drop.urls[0])) }
    }
    Label { text: library.backend ? library.backend.message : ""; visible: text.length>0; Layout.fillWidth: true; color: Theme.ink; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
    RowLayout {
        Layout.fillWidth: true
        Label { text: library.backend?library.backend.dataPath:""; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideMiddle }
        SButton { text: qsTr("Otwórz folder"); quiet: true; dense: true; onClicked: library.backend.openFolder() }
        SButton { text: qsTr("Zmień folder"); quiet: true; dense: true; onClicked: library.backend.chooseFolder() }
    }
}
