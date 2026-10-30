import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: tabs
    property var options: []
    property var optionEnabled: []
    property int currentIndex: 0
    property string optionNamePrefix: ""
    signal selected(int index)
    implicitHeight: row.implicitHeight + (Theme.compact ? 4 : 8)
    readonly property bool overflow: row.width > width
    function revealCurrent() {
        var button=buttons.itemAt(currentIndex)
        indicator.selectedButton=button
        if(!button || !overflow)return
        if(button.x<viewport.contentX)viewport.contentX=button.x
        else if(button.x+button.width>viewport.contentX+viewport.width)viewport.contentX=Math.max(0,button.x+button.width-viewport.width)
    }
    onCurrentIndexChanged: Qt.callLater(revealCurrent)
    onWidthChanged: Qt.callLater(revealCurrent)
    onOptionsChanged: Qt.callLater(revealCurrent)
    SButton {
        id: previous
        visible: tabs.overflow
        width: 32; height: parent.height - (Theme.compact ? 4 : 8)
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
    Rectangle {
        id: indicator
        property Item selectedButton
        x: selectedButton ? selectedButton.x : 0
        width: selectedButton ? selectedButton.width : 0
        height: selectedButton ? selectedButton.height : 0
        color: Theme.stadium; radius: Theme.radius
        Behavior on x { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
        Behavior on width { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
    }
    Row {
        id: row
        spacing: Theme.compact ? 3 : 6
        Repeater {
            id: buttons
            onItemAdded: Qt.callLater(tabs.revealCurrent)
            onItemRemoved: Qt.callLater(tabs.revealCurrent)
            model: tabs.options
            SButton {
                objectName: tabs.optionNamePrefix.length ? tabs.optionNamePrefix + index : ""
                text: modelData
                quiet: true
                font.family: Theme.displayFamily
                font.pointSize: Theme.section
                foreground: tabs.currentIndex === index ? Theme.snow : Theme.muted
                background: Surface {
                    fillColor: tabs.currentIndex===index ? "transparent" : parent.hovered ? Theme.selection : "transparent"
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
        width: 32; height: parent.height - (Theme.compact ? 4 : 8)
        text: "›"; quiet: true
        Accessible.name: qsTr("Następne zakładki")
        enabled: viewport.contentX + viewport.width < viewport.contentWidth - 1
        onClicked: viewport.contentX = Math.min(viewport.contentWidth-viewport.width,viewport.contentX+viewport.width*.7)
    }
}
