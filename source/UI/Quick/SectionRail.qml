import QtQuick 2.15
import QtQuick.Controls 2.15

ScrollView {
    id: rail
    property var options: []
    property int currentIndex: 0
    signal selected(int index)
    contentWidth: availableWidth
    clip: true
    Column {
    width: rail.availableWidth
    spacing: 6
    Repeater {
        model: rail.options
        SButton {
            width: rail.width
            text: modelData
            font.family: Theme.displayFamily
            font.pointSize: Theme.section
            foreground: rail.currentIndex===index ? Theme.snow : Theme.muted
            background: Surface { fillColor: rail.currentIndex===index ? Theme.stadium : parent.hovered ? Theme.selection : "transparent"; strokeWidth: parent.activeFocus ? 2 : 0; edgeColor: Theme.accent; sweep: 18 }
            onClicked: rail.selected(index)
        }
    }
    }
}
