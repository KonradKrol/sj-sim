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
        Label { text: qsTr("Skala interfejsu (%)"); color: Theme.muted; font.pointSize: Theme.caption }
        SpinBox {
            objectName: "appearanceUiScale"; from: 50; to: 150; stepSize: 5; editable: true
            value: Math.round(appearance.uiScale*100); font.pointSize: Theme.body; Layout.fillWidth: true
            textFromValue: function(value,locale){return value.toString()}
            valueFromText: function(text,locale){return Number(text.replace("%", "").trim())}
            validator: IntValidator { bottom: 50; top: 150 }
            onValueModified: appearance.uiScale=value/100
            Accessible.name: qsTr("Skala interfejsu")
        }
        Label { text: qsTr("Rozmiar tekstu (%)"); color: Theme.muted; font.pointSize: Theme.caption }
        SpinBox {
            objectName: "appearanceTextSize"; from: 75; to: 200; stepSize: 5; editable: true
            value: Math.round(appearance.textScale*100); font.pointSize: Theme.body; Layout.fillWidth: true
            textFromValue: function(value,locale){return value.toString()}
            valueFromText: function(text,locale){return Number(text.replace("%", "").trim())}
            validator: IntValidator { bottom: 75; top: 200 }
            onValueModified: appearance.textScale=value/100
            Accessible.name: qsTr("Rozmiar tekstu")
        }
        SButton { objectName: "appearanceDone"; text: qsTr("Gotowe"); onClicked: popup.close(); Layout.fillWidth: true }
        }
    }
}
