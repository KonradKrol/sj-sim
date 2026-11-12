import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: bar
    columns: width>Theme.dp(700)?4:3
    columnSpacing: 8; rowSpacing: 6
    Node {
        Layout.columnSpan: bar.width>Theme.dp(700)?1:3
        node: ui.find("dataPackChoice")
        Layout.fillWidth: true
        Layout.minimumWidth: 0
    }
    SButton { text: qsTr("Importuj"); primary: true; onClicked: {var n=ui.find("importDataPack");if(n)ui.activate(n.values.id)} }
    SButton { text: qsTr("Eksportuj"); foreground: Theme.accent; onClicked: {var n=ui.find("exportDataPack");if(n)ui.activate(n.values.id)} }
    SButton {
        objectName: "manageDataPack"
        text: qsTr("Ustawienia paczki")
        quiet: true
        onClicked: settings.open()
    }
    Popup {
        enter: PopupFade {}
        exit: PopupFade { leaving: true }
        id: settings
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(620, parent.width - 24)
        height: Math.min(contentHeight + padding * 2, parent.height - 24)
        padding: 20
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
        contentItem: ScrollView {
            id: scroll
            contentWidth: availableWidth
            clip: true
            Column {
                width: scroll.availableWidth
                spacing: 12
                Label { width: parent.width; text: qsTr("Ustawienia paczki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; wrapMode: Text.WordWrap }
                Repeater {
                    model: ["dataPackName", "dataPackAgeFormat", "dataPackReferenceYear", "createDataPack"]
                    Node {
                        width: parent.width
                        node: ui.find(modelData)
                        visible: !!node && node.values.available !== false
                            && (ui.root.values.value === 0 || (modelData !== "dataPackAgeFormat" && modelData !== "dataPackReferenceYear"))
                    }
                }
                Column {
                    width: parent.width; spacing: Theme.dp(8); visible: ui.root.values.value===2
                    Label { text: qsTr("Domyślne zasady"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true }
                    Repeater { model: ["competition","qualifications","trial","training","teamCompetition","teamTrial"]; Node { required property string modelData; width: parent.width; node: ui.find("packDefault-"+modelData) } }
                }
                Label {
                    width: parent.width
                    text: qsTr("Udostępnij plik .sjsim-pack. W Bibliotece możesz połączyć różne zasoby w jednym pliku.")
                    color: Theme.muted
                    wrapMode: Text.WordWrap
                }
                SButton { text: qsTr("Biblioteka"); onClicked: {settings.close();ui.openResourceLibrary()} }
                SButton { objectName: "closeDataPackSettings"; width: parent.width; text: qsTr("Gotowe"); onClicked: settings.close() }
            }
        }
    }
}
