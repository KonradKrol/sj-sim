import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: presets
    property var backend
    property bool stacked: false
    signal applied()
    columns: stacked ? 1 : 2
    columnSpacing: 12; rowSpacing: 8
    SChoice {
        id: choice; objectName: "rosterPresetChoice"
        Layout.fillWidth: true; Layout.minimumWidth: 0
        model: presets.backend.presets
        displayText: qsTr("Lista: %1").arg(currentText)
        Accessible.name: qsTr("Zapisana lista")
    }
    RowLayout {
        Layout.fillWidth: true; spacing: 6
        SButton { objectName: "rosterPresetAdd"; text: qsTr("Dodaj"); Layout.fillWidth: true; onClicked: {presets.backend.applyPreset(choice.currentIndex,0);presets.applied()} }
        SButton { objectName: "rosterPresetRemove"; text: qsTr("Usuń"); Layout.fillWidth: true; onClicked: {presets.backend.applyPreset(choice.currentIndex,1);presets.applied()} }
        SButton { objectName: "rosterPresetOrder"; text: qsTr("Kolejność"); Layout.fillWidth: true; onClicked: {presets.backend.applyPreset(choice.currentIndex,2);presets.applied()} }
    }
}
