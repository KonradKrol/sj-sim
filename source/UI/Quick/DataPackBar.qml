import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: bar
    spacing: 12
    Node {
        node: ui.find("dataPackChoice")
        Layout.fillWidth: true
        Layout.minimumWidth: 0
    }
    SButton {
        objectName: "manageDataPack"
        text: qsTr("Ustawienia paczki")
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
                    model: ["dataPackName", "dataPackAgeFormat", "dataPackReferenceYear", "createDataPack", "importDataPack", "exportDataPack"]
                    Node {
                        width: parent.width
                        node: ui.find(modelData)
                        visible: !!node && node.values.available !== false
                            && (ui.root.values.value === 0 || (modelData !== "dataPackAgeFormat" && modelData !== "dataPackReferenceYear"))
                    }
                }
                Label {
                    width: parent.width
                    text: qsTr("Wybrana paczka jest używana w grze. Eksport zapisuje plik JSON do udostępnienia.")
                    color: Theme.muted
                    wrapMode: Text.WordWrap
                }
                SButton { objectName: "closeDataPackSettings"; width: parent.width; text: qsTr("Gotowe"); onClicked: settings.close() }
            }
        }
    }
}
