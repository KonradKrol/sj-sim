import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: badge
    objectName: "releaseBadge"
    readonly property var parts: ui.version.split(" ")
    spacing: 8
    Accessible.role: Accessible.StaticText
    Accessible.name: "Sj.Sim " + ui.version
    Label { text: "SJ.SIM"; color: Theme.muted; font.pointSize: Theme.caption; font.family: Theme.bodyFamily; font.weight: Font.DemiBold }
    Label { text: badge.parts[0]; color: Theme.ink; font.pointSize: Theme.caption; font.family: Theme.mono }
    Item {
        visible: badge.parts.length > 1
        implicitWidth: stage.implicitWidth + 16
        implicitHeight: stage.implicitHeight + 6
        Surface { anchors.fill: parent; fillColor: Theme.signal; sweep: 8; corner: 4 }
        Label { id: stage; anchors.centerIn: parent; text: badge.parts.slice(1).join(" "); color: Theme.stadium; font.pointSize: Theme.caption; font.family: Theme.mono }
    }
}
