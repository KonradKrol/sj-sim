import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: popup
    width: 300
    height: Math.min(implicitHeight,parent ? parent.height : implicitHeight)
    padding: 20
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    contentItem: ScrollView {
        id: scroll
        contentWidth: availableWidth
        implicitHeight: settings.implicitHeight
        clip: true
        ColumnLayout {
        id: settings
        width: scroll.availableWidth
        spacing: 12
        Label { text: qsTr("Wygląd i dostępność"); font.pointSize: Theme.body; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SCheck { text: qsTr("Ciemny motyw"); checked: appearance && appearance.dark; onClicked: appearance.dark = checked; Layout.fillWidth: true }
        SCheck { text: qsTr("Ogranicz animacje"); checked: appearance && appearance.reducedMotion; onClicked: appearance.reducedMotion = checked; Layout.fillWidth: true }
        SCheck { objectName: "compactUiToggle"; text: qsTr("Kompaktowy interfejs"); checked: appearance && appearance.compactUi; onClicked: appearance.compactUi = checked; Layout.fillWidth: true }
        Label { text: qsTr("Rozmiar tekstu"); color: Theme.muted; font.pointSize: Theme.caption }
        SChoice { objectName: "appearanceTextSize"; model: [qsTr("Bardzo mały · 75%"),qsTr("Mniejszy · 85%"),qsTr("Standardowy"),qsTr("Większy · 115%"),qsTr("Duży · 130%")]; currentIndex: !appearance ? 2 : appearance.textScale >= 1.3 ? 4 : appearance.textScale >= 1.15 ? 3 : appearance.textScale >= 1 ? 2 : appearance.textScale >= .85 ? 1 : 0; onActivated: appearance.textScale = [.75,.85,1,1.15,1.3][currentIndex]; Layout.fillWidth: true }
        SButton { objectName: "appearanceDone"; text: qsTr("Gotowe"); onClicked: popup.close(); Layout.fillWidth: true }
        }
    }
}
