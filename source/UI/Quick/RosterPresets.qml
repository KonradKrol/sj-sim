import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: presets
    property var backend
    property bool stacked: false
    signal applied()
    padding: Theme.dp(9)
    background: Surface { fillColor: Theme.selection; edgeColor: Theme.accent; strokeWidth: 1 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(6)
        Label { text: qsTr("Uczestnicy konkursu"); font.bold: true; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true }
        SChoice {
            id: choice; objectName: "rosterPresetChoice"
            visible: presets.backend.presets.length>0
            Layout.fillWidth: true; Layout.minimumWidth: 0
            model: presets.backend.presets
            currentIndex: -1
            displayText: currentIndex<0 ? qsTr("Wybierz listę zawodników") : currentText
            Accessible.name: qsTr("Lista uczestników")
            onActivated: {presets.backend.applyPreset(currentIndex,3);presets.applied()}
        }
        SButton {
            text: qsTr("Wszyscy zawodnicy"); Layout.fillWidth: true
            onClicked: {choice.currentIndex=-1;presets.backend.applyPreset(-1,3);presets.applied()}
        }
    }
}
