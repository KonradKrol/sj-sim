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
        Label { text: qsTr("Skala interfejsu"); color: Theme.muted; font.pointSize: Theme.caption }
        NumberField { objectName: "appearanceUiScale"; direct: false; label: qsTr("Skala interfejsu"); fields: ({name:"appearanceUiScale",value:appearance.uiScale*100,minimum:50,maximum:150,step:5,decimals:0,suffix:"%"}); Layout.fillWidth: true; onCommitted: appearance.uiScale=value/100 }
        Label { text: qsTr("Rozmiar tekstu"); color: Theme.muted; font.pointSize: Theme.caption }
        NumberField { objectName: "appearanceTextSize"; direct: false; label: qsTr("Rozmiar tekstu"); fields: ({name:"appearanceTextSize",value:appearance.textScale*100,minimum:75,maximum:200,step:5,decimals:0,suffix:"%"}); Layout.fillWidth: true; onCommitted: appearance.textScale=value/100 }
        SButton { objectName: "appearanceDone"; text: qsTr("Gotowe"); onClicked: popup.close(); Layout.fillWidth: true }
        }
    }
}
