import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: popup
    width: 300
    padding: 20
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    contentItem: ColumnLayout {
        spacing: 12
        Label { text: qsTr("Wygląd i dostępność"); font.pointSize: Theme.body; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SCheck { text: qsTr("Ciemny motyw"); checked: appearance && appearance.dark; onClicked: appearance.dark = checked; Layout.fillWidth: true }
        SCheck { text: qsTr("Ogranicz animacje"); checked: appearance && appearance.reducedMotion; onClicked: appearance.reducedMotion = checked; Layout.fillWidth: true }
        Label { text: qsTr("Rozmiar tekstu"); color: Theme.muted; font.pointSize: Theme.caption }
        SChoice { model: [qsTr("Standardowy"), qsTr("Większy · 115%"), qsTr("Duży · 130%")]; currentIndex: !appearance ? 0 : appearance.textScale >= 1.3 ? 2 : appearance.textScale >= 1.15 ? 1 : 0; onActivated: appearance.textScale = [1,1.15,1.3][currentIndex]; Layout.fillWidth: true }
        SButton { text: qsTr("Gotowe"); onClicked: popup.close(); Layout.fillWidth: true }
    }
}
