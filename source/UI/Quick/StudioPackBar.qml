import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: bar
    property var backend
    property bool athletes: false
    spacing: Theme.dp(4)
    Label { text: bar.athletes ? qsTr("Paczka skoczków") : qsTr("Paczka skoczni"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(8)
        SChoice {
            objectName: "studioPackChoice"
            Layout.fillWidth: true; Layout.minimumWidth: 0
            model: bar.backend.packs.map(function(p){return p.name+" · "+p.count})
            currentIndex: bar.backend.packIndex
            Accessible.name: bar.athletes ? qsTr("Paczka skoczków") : qsTr("Paczka skoczni")
            onActivated: bar.backend.selectPack(index)
        }
        SButton { objectName: "manageStudioPack"; text: qsTr("Ustawienia paczki"); onClicked: settings.open() }
    }
    Popup {
        id: settings
        objectName: "studioPackSettings"
        parent: Overlay.overlay; anchors.centerIn: parent
        width: Math.min(Theme.dp(560),parent.width-24)
        height: Math.min(body.implicitHeight+padding*2,parent.height-24)
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
                Label { text: qsTr("Ustawienia paczki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Zmiany dotyczą wybranej paczki. Zapisują się od razu; istniejące sezony mają własne dane."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Nazwa paczki"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true }
                RowLayout {
                    Layout.fillWidth: true
                    SField { id: packName; objectName: "studioPackName"; Layout.fillWidth: true; Layout.minimumWidth: 0; maximumLength: 100; Accessible.name: qsTr("Nazwa paczki") }
                    SButton { objectName: "saveStudioPackName"; text: qsTr("Zapisz nazwę"); enabled: packName.text.trim().length>0 && packName.text.trim()!==bar.backend.packName; onClicked: bar.backend.renamePack(packName.text) }
                }
                Label { text: qsTr("Zarządzanie paczkami"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true }
                Flow {
                    Layout.fillWidth: true; spacing: Theme.dp(6)
                    SButton { text: qsTr("Nowa paczka"); onClicked: {bar.backend.createPack();settings.close()} }
                    SButton { text: qsTr("Importuj paczkę"); onClicked: {settings.close();bar.backend.importPack()} }
                    SButton { text: qsTr("Eksportuj paczkę"); onClicked: {settings.close();bar.backend.exportPack()} }
                    SButton { visible: bar.athletes; text: qsTr("Otwórz folder paczki"); onClicked: {settings.close();bar.backend.openPackFolder()} }
                }
                SButton { objectName: "closeStudioPack"; text: qsTr("Gotowe"); primary: true; Layout.fillWidth: true; onClicked: settings.close() }
            }
        }
    }
}
