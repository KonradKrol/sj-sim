import QtQuick 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: tools
    property bool compact: false
    readonly property real cardHeight: Math.max(databaseCard.implicitHeight,settingsCard.implicitHeight)
    readonly property real cardWidth: Math.max(1,((width>0 ? width : Theme.dp(640))-(columns-1)*columnSpacing)/columns)
    columns: compact || width < Theme.dp(660) ? 1 : 2
    columnSpacing: Theme.gap
    rowSpacing: Theme.gap
    HomeTool {
        id: databaseCard
        objectName: "homeDatabase"
        measurementWidth: tools.cardWidth
        title: qsTr("Baza danych")
        description: qsTr("%1 zawodników · %2 skoczni").arg(ui.library.jumpers).arg(ui.library.hills)
        actionText: qsTr("Edytuj bazę danych")
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: 1
        Layout.minimumHeight: tools.cardHeight
        Layout.preferredHeight: tools.cardHeight
        Layout.maximumHeight: tools.cardHeight
        onClicked: ui.launch("database")
    }
    HomeTool {
        id: settingsCard
        objectName: "homeSettings"
        measurementWidth: tools.cardWidth
        entranceDelay: 260
        title: qsTr("Ustawienia")
        description: qsTr("Preferencje aplikacji i ustawienia symulacji.")
        actionText: qsTr("Otwórz ustawienia")
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: 1
        Layout.minimumHeight: tools.cardHeight
        Layout.preferredHeight: tools.cardHeight
        Layout.maximumHeight: tools.cardHeight
        onClicked: ui.launch("settings")
    }
}
