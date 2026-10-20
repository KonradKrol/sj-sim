import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: tabs
    property var options: []
    property var optionEnabled: []
    property int currentIndex: 0
    signal selected(int index)
    implicitHeight: row.implicitHeight + 8
    readonly property bool overflow: row.width > width
    SButton {
        id: previous
        visible: tabs.overflow
        width: 32; height: parent.height - 8
        text: "‹"; quiet: true
        Accessible.name: qsTr("Poprzednie zakładki")
        enabled: viewport.contentX > 0
        onClicked: viewport.contentX = Math.max(0,viewport.contentX-viewport.width*.7)
    }
    Flickable {
    id: viewport
    anchors.left: parent.left; anchors.right: parent.right
    anchors.leftMargin: tabs.overflow ? 34 : 0; anchors.rightMargin: tabs.overflow ? 34 : 0
    height: parent.height
    contentWidth: row.width
    contentHeight: height
    flickableDirection: Flickable.HorizontalFlick
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }
    Row {
        id: row
        spacing: 6
        Repeater {
            model: tabs.options
            SButton {
                text: modelData
                quiet: true
                font.family: Theme.displayFamily
                font.pointSize: Theme.section
                foreground: tabs.currentIndex === index ? Theme.snow : Theme.muted
                background: Surface {
                    fillColor: tabs.currentIndex===index ? Theme.stadium : parent.hovered ? Theme.selection : "transparent"
                    edgeColor: Theme.accent; strokeWidth: parent.activeFocus ? 2 : 0; sweep: 18
                }
                enabled: tabs.optionEnabled.length <= index || tabs.optionEnabled[index]
                checkable: true
                checked: tabs.currentIndex === index
                onClicked: tabs.selected(index)
            }
        }
    }
    }
    SButton {
        visible: tabs.overflow
        anchors.right: parent.right
        width: 32; height: parent.height - 8
        text: "›"; quiet: true
        Accessible.name: qsTr("Następne zakładki")
        enabled: viewport.contentX + viewport.width < viewport.contentWidth - 1
        onClicked: viewport.contentX = Math.min(viewport.contentWidth-viewport.width,viewport.contentX+viewport.width*.7)
    }
    onCurrentIndexChanged: Qt.callLater(function() {
        var child = row.children[currentIndex]
        if (child) viewport.contentX = Math.max(0, Math.min(child.x, viewport.contentWidth - viewport.width))
    })
}
