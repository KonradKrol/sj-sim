import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: bar
    property var backend
    property bool athletes: false
    spacing: Theme.dp(4)
    RowLayout {
        Layout.fillWidth: true
        Label { text: bar.athletes ? qsTr("Paczka skoczków") : qsTr("Paczka skoczni"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        SButton { text: qsTr("Biblioteka"); quiet: true; dense: true; onClicked: ui.openResourceLibrary() }
    }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(700)?4:3; columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(6)
        SChoice {
            Layout.columnSpan: bar.width>Theme.dp(700)?1:3
            objectName: "studioPackChoice"
            Layout.fillWidth: true; Layout.minimumWidth: 0
            model: bar.backend.packs.map(function(p){return p.name+" · "+p.count})
            currentIndex: bar.backend.packIndex
            Accessible.name: bar.athletes ? qsTr("Paczka skoczków") : qsTr("Paczka skoczni")
            onActivated: bar.backend.selectPack(index)
        }
        SButton { objectName: "importStudioPack"; text: qsTr("Importuj"); primary: true; onClicked: bar.backend.importPack() }
        SButton { objectName: "exportStudioPack"; text: qsTr("Eksportuj"); foreground: Theme.accent; onClicked: bar.backend.exportPack() }
        SButton { objectName: "manageStudioPack"; text: qsTr("Ustawienia paczki"); quiet: true; onClicked: settings.open() }
    }
    Popup {
        id: settings
        objectName: "studioPackSettings"
        parent: Overlay.overlay; anchors.centerIn: parent
        width: Math.min(Theme.dp(560),parent.width-24)
        property real measuredHeight: Theme.dp(300)
        height: Math.min(measuredHeight,parent.height-24)
        Timer { id: measurePack; interval: 0; onTriggered: settings.measuredHeight=Math.ceil(body.implicitHeight+settings.padding*2) }
        Component.onCompleted: measurePack.restart()
        padding: Theme.dp(18); modal: true; focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        onOpened: packName.text=bar.backend.packName
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
        contentItem: ScrollView {
            id: content
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                id: body
                width: content.availableWidth; spacing: Theme.dp(12)
                onImplicitHeightChanged: measurePack.restart()
                Label { text: qsTr("Ustawienia paczki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Zmiany dotyczą wybranej paczki. Zapisują się od razu; istniejące sezony mają własne dane."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Nazwa paczki"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true }
                RowLayout {
                    Layout.fillWidth: true
                    SField { id: packName; objectName: "studioPackName"; Layout.fillWidth: true; Layout.minimumWidth: 0; maximumLength: 100; Accessible.name: qsTr("Nazwa paczki") }
                    SButton { objectName: "saveStudioPackName"; text: qsTr("Zapisz nazwę"); enabled: packName.text.trim().length>0 && packName.text.trim()!==bar.backend.packName; onClicked: bar.backend.renamePack(packName.text) }
                }
                Label { text: qsTr("Zarządzanie paczkami"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true }
                Label {
                    text: qsTr("Udostępnij plik .sjsim-pack. Zdjęcia są dołączane do paczki. W Bibliotece możesz połączyć różne zasoby w jednym pliku.")
                    color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap
                }
                Flow {
                    Layout.fillWidth: true; spacing: Theme.dp(6)
                    SButton { text: qsTr("Nowa paczka"); onClicked: {bar.backend.createPack();settings.close()} }
                    SButton { visible: bar.athletes; text: qsTr("Otwórz folder paczki"); onClicked: {settings.close();bar.backend.openPackFolder()} }
                }
                SButton { objectName: "closeStudioPack"; text: qsTr("Gotowe"); primary: true; Layout.fillWidth: true; onClicked: settings.close() }
            }
        }
    }
}
