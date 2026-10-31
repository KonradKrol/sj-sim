import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SButton {
    id: control
    objectName: "sortByCountry"
    text: qsTr("Kraj ↑")
    Accessible.name: qsTr("Sortuj według kraju")
    hint: qsTr("Sortuj według kraju")
    signal confirmed()
    onClicked: confirmation.open()
    Popup {
        id: confirmation
        objectName: "confirmCountrySort"
        parent: Overlay.overlay; anchors.centerIn: parent
        width: Math.min(Theme.dp(440),parent.width-24)
        property real measuredHeight: Theme.dp(220)
        height: Math.min(measuredHeight,parent.height-24)
        Timer { id: measureConfirmation; interval: 0; onTriggered: confirmation.measuredHeight=Math.ceil(content.implicitHeight+confirmation.padding*2) }
        Component.onCompleted: measureConfirmation.restart()
        padding: Theme.dp(18); modal: true; focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
        contentItem: ColumnLayout {
            id: content
            spacing: Theme.dp(12)
            onImplicitHeightChanged: measureConfirmation.restart()
            Label { text: qsTr("Posortować według kraju?"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: qsTr("Kolejność w paczce zostanie zmieniona. W obrębie każdego kraju obecna kolejność zostanie zachowana. Możesz cofnąć sortowanie."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            RowLayout {
                Layout.fillWidth: true
                SButton { objectName: "cancelCountrySort"; text: qsTr("Anuluj"); Layout.fillWidth: true; onClicked: confirmation.close() }
                SButton { objectName: "confirmCountrySortAction"; text: qsTr("Sortuj"); primary: true; Layout.fillWidth: true; onClicked: {confirmation.close();control.confirmed()} }
            }
        }
    }
}
