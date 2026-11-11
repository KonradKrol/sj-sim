import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: presets
    property var backend
    property bool stacked: false
    signal applied()
    spacing: Theme.dp(8)
    Label { text: qsTr("Zawodnicy"); color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.caption }
    SChoice {
        objectName: "rosterPresetChoice"
        Layout.fillWidth: true; Layout.minimumWidth: 0
        model: [qsTr("Wszyscy zawodnicy")].concat(presets.backend.presets)
        currentIndex: presets.backend.participantList
        displayText: currentIndex<0 ? qsTr("Własny wybór") : currentText
        Accessible.name: qsTr("Lista zawodników")
        onActivated: {presets.backend.applyPreset(currentIndex-1,3);presets.applied()}
        background: Rectangle {
            radius: Theme.controlRadius
            color: Theme.selection
            border.color: Theme.accent
            border.width: parent.activeFocus ? Theme.focusBorder : 1
        }
    }
}
