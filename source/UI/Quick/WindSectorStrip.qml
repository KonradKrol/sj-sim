import QtQuick 2.15
import QtQuick.Controls 2.15

Flickable {
    id: strip
    property var sectors: []
    property int currentIndex: 0
    signal selected(int index)
    implicitHeight: 112 * Theme.scale
    contentWidth: row.width
    contentHeight: height
    flickableDirection: Flickable.HorizontalFlick
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    ScrollBar.horizontal: ScrollBar {}
    Row {
        id: row
        spacing: 8
        Repeater {
            model: strip.sectors
            AbstractButton {
                id: sector
                readonly property var values: modelData.values
                readonly property bool chosen: index === strip.currentIndex
                width: Math.max(112,(strip.width - Math.max(0,strip.sectors.length-1)*8)/Math.max(1,strip.sectors.length))
                height: strip.height - 8
                padding: 12
                hoverEnabled: true
                Accessible.role: Accessible.PageTab
                Accessible.name: qsTr("Sektor %1").arg(index+1) + ", " + values.range
                Accessible.selected: chosen
                onClicked: strip.selected(index)
                background: Surface { fillColor: sector.chosen ? Theme.stadium : sector.hovered ? Theme.selection : Theme.surface; strokeWidth: sector.activeFocus ? 2 : 0; edgeColor: Theme.accent; sweep: 20 }
                contentItem: Item {
                    Label { text: sector.values.range; color: sector.chosen ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption; anchors.top: parent.top }
                    Label { text: Number(sector.values.strength.value).toLocaleString(Qt.locale(Theme.localeName),'f',1); color: sector.chosen ? Theme.signal : Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; anchors.left: parent.left; anchors.bottom: parent.bottom }
                    Label { text: "↑"; rotation: sector.values.direction.value; color: sector.chosen ? Theme.signal : Theme.accent; font.pointSize: Theme.heading; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; Accessible.ignored: true; Behavior on rotation { RotationAnimation { duration: Theme.motion; direction: RotationAnimation.Shortest } } }
                }
            }
        }
    }
    onCurrentIndexChanged: Qt.callLater(function() {
        var child = row.children[strip.currentIndex]
        if (child) contentX = Math.max(0,Math.min(child.x,contentWidth-width))
    })
}
