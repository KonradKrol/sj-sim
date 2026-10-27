import QtQuick
import QtQuick.Layouts

GridLayout {
    id: tools
    property bool compact: false
    readonly property real cardHeight: Math.max(databaseCard.implicitHeight,settingsCard.implicitHeight)
    columns: compact || width < Theme.dp(660) ? 1 : 2
    uniformCellWidths: true
    uniformCellHeights: true
    columnSpacing: Theme.gap
    rowSpacing: Theme.gap
    HomeTool {
        id: databaseCard
        objectName: "homeDatabase"
        title: qsTr("Baza danych")
        description: qsTr("%1 zawodników · %2 skoczni").arg(ui.library.jumpers).arg(ui.library.hills)
        actionText: qsTr("Edytuj bazę danych")
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: tools.cardHeight
        Layout.preferredHeight: tools.cardHeight
        Layout.maximumHeight: tools.cardHeight
        onClicked: ui.launch("database")
    }
    HomeTool {
        id: settingsCard
        objectName: "homeSettings"
        entranceDelay: 260
        title: qsTr("Ustawienia")
        description: qsTr("Preferencje aplikacji i ustawienia symulacji.")
        actionText: qsTr("Otwórz ustawienia")
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: tools.cardHeight
        Layout.preferredHeight: tools.cardHeight
        Layout.maximumHeight: tools.cardHeight
        onClicked: ui.launch("settings")
    }
}
